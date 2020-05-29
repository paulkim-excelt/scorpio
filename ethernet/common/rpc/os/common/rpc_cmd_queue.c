/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

#include <stddef.h>
#include <string.h>
#include <imgl.h>
#include <bcm_err.h>
#include <utils.h>
#include <system.h>
#include <nvm_pt.h>
#include <rpc_cmd_queue_osil.h>
#include <rpc_cmds.h>
#include <ipc.h>
#include <imgl_ipc_cmds.h>
#include <ipc_osil.h>
#include <lw_queue.h>
#include <osil/sys_ipc_osil.h>

#if defined(ENABLE_ETS)
#include <ets_ipc.h>
#endif

#ifdef ENABLE_ETH_SWITCH
#include <comms_ipc.h>
#endif

#include "ee.h"

/**
    @name RPC Command Queue message state macros
    @{
    @brief RPC Command Queue message state macros
*/
typedef uint32_t RPC_CmdQStatusType;
#define RPC_CMDQ_MSG_STATUS_INIT    (0UL) /**< @brief Message in Init State */
#define RPC_CMDQ_MSG_STATUS_RUNNING (1UL) /**< @brief Message in Running State,
                                                Command issued to slave */
#define RPC_CMDQ_MSG_STATUS_DONE    (2UL) /**< @brief Message response came from Slave */
/** @} */

/**
    @brief Structure used to manage entries in the MsgQ of system
*/
typedef struct _RPC_CmdQMsgType {
    RPC_CmdQStatusType      status;                     /**< @brief Current Status              */
    RPC_ChannelType         channel;                    /**< @brief channel typedef             */
    RPC_ChannelIdType       id;                         /**< @brief channel data                */
    uint8_t                 groupID;                    /**< @brief Notification group ID       */
    uint16_t                compID;                     /**< @brief Component ID                */
    uint16_t                cmdID;                      /**< @brief Async Notification ID       */
    uint32_t                payLoadSize;                /**< @brief Payload size in bytes       */
    uint8_t                 payLoad[RPC_MSG_PAYLOAD_SZ];/**< @brief notifcation payLoad         */
    int32_t                 result;                     /**< @brief result returned by client   */
    uint32_t                curNumResp;                 /**< @brief Number of collated responses*/
    uint32_t                reqNumResp;                 /**< @brief Number of needed responses  */
    const MSGQ_MsgHdrType  *hdr;                        /**< @brief Handle to messag queue when
                                                                    command is provided to local
                                                                    service provider            */
    uint32_t                pendingResponseMask;        /**< @brief bits set for all pending
                                                                    responses from slaves       */
} RPC_CmdQMsgType;

/**
    @brief getting payload from local clients
*/
uint8_t RPC_EventPayLoad[RPC_RESPPAYLOADSZ];

/**
    @brief this will be needed for merge command
*/
uint8_t RPC_TmpPayLoad[RPC_RESPPAYLOADSZ];

static void RPC_CmdQReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                  uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RPC_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief RPC loader messages memory
*/
static RPC_CmdQMsgType RPC_CmdQMsgs[MSGQ_SIZE];

/**
    @brief Get the i-th message from server

    Retrieve the i-th message from server.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]  idx         Index of the message

    @return                 Pointer to the i-th message
    @return                 NULL if idx >= MSGQ_SIZE

    @post None

    @trace #TBD

    @limitations None
*/
void* RPC_CmdQGetMessage(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {
        pRet = (void *)(&RPC_CmdQMsgs[idx]);
    }
    return pRet;
}

/**
    @brief #MSGQ_HdrQType instance of RPC command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(RPC_CmdQHdrQ);

/**
    @brief Command Message Queue

    The message queue for the events notified to switch. System task
    processes them asynchronously.

    @trace #TBD
*/
MSGQ_DEFINE((RPC_CmdQ), (IPC_SERVER_TASK), (RPC_CMDQ_EVENT),
            (MSGQ_CLIENT_SIGNAL_ENABLE), RPC_CmdQMsgType, (&RPC_CmdQHdrQ),
            (RPC_CmdQGetMessage));


LWQ_BufferType RPC_CmdQFreeQ;
LWQ_BufferType RPC_CmdQFullQ;
static RPC_CmdQMsgType RPC_CmdQLocalQ[LWQ_BUFFER_MAX_LENGTH];

/**
*/
void RPC_CmdQInit(void)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;

    /* Initialize all the LWQ used by RPC CmdQ */
    LWQ_Init(&RPC_CmdQFreeQ);
    LWQ_Init(&RPC_CmdQFullQ);

    /* Add all free MsgQ header pointers to the free queue */
    for(i = 0U; i < LWQ_BUFFER_MAX_LENGTH; i++){
        retVal = LWQ_Push(&RPC_CmdQFreeQ, i);
        if(BCM_ERR_OK != retVal){
            RPC_CmdQReportError(0U, retVal, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
        }
    }
    BCM_MemSet(RPC_CmdQLocalQ, 0U, sizeof(RPC_CmdQLocalQ));
}

/**
    @Brief Handle messages in RPC command queue
*/
static void RPC_CmdQTransferCmd(void)
{
    uint32_t  idx     = 0UL;
    uint32_t  lastIdx = 0xFFUL;
    int32_t   retVal  = BCM_ERR_OK;

    /* We loop on RPC_CmdQ and process commands as long as we get a */
    /* new command. This can happen if the command processing fails */
    /* for the command being added via #RPC_CmdQAddCmd. In such     */
    /* failure, we need to pick the next command for processing.    */
    do {
        retVal = MSGQ_GetMsgIdx(&RPC_CmdQ, &idx);
        if ((BCM_ERR_OK == retVal) && (lastIdx != idx)) {
            lastIdx = idx;
            RPC_CmdQMsgType *msg = &RPC_CmdQMsgs[idx];
            if (RPC_CMDQ_MSG_STATUS_INIT == msg->status) {
                RPC_CmdQRpcMsgType tmp;
                tmp.channel = msg->channel;
                tmp.id = msg->id;
                tmp.cmd = RPC_CMDID(msg->groupID, msg->compID, msg->cmdID);
                tmp.payLoad = msg->payLoad;
                tmp.payLoadSize = msg->payLoadSize;
                tmp.error = BCM_ERR_OK;
                retVal = RPC_CmdQAddCmd(&tmp);
                if (BCM_ERR_OK == retVal) {
                    msg->status = RPC_CMDQ_MSG_STATUS_RUNNING;
                } else {
                    /* Will be re-attempted for queueing later */
                }
            }
        } else {
            retVal = BCM_ERR_BUSY;
        }
    } while (BCM_ERR_OK == retVal);
}


/** @brief RPC Process Commands

    Todo

    @behavior Sync, Re-entrant

    @pre Todo

    @post Todo

    @trace Todo
    @vtrace #RPC_SYS_IFC

    @limitations This API may not be supported on all the chipsets
*/
static void RPC_CmdQProcessCmd()
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t    i = 0U;
    uint8_t popIdx = 0U;
    RPC_CmdQMsgType *msg = NULL;

    while (0UL < LWQ_Length(RPC_CmdQFullQ)) {
        /* Peek for first job */
        i = LWQ_PeekFirst(RPC_CmdQFullQ);
        msg = &RPC_CmdQLocalQ[i];
        if (RPC_CMDQ_MSG_STATUS_INIT == msg->status) {
            uint32_t rpcCmd = RPC_CMDID(msg->groupID, msg->compID, msg->cmdID);
            retVal = BCM_ERR_OK;
            uint32_t forwardtoSlaves = 0UL;
            if (RPC_CHANNEL_LOCAL != msg->channel) {
                switch (msg->groupID) {
#ifdef RPC_SYS_IFC
                case BCM_GROUPID_SYS:
                    retVal = RPC_SYS_IFC.sendCmd(msg->cmdID, msg->compID, msg->payLoad, msg->payLoadSize, RPC_CMDQ_EVENT, &msg->hdr, &forwardtoSlaves);
                break;
#endif
#ifdef RPC_COMMS_IFC
                case BCM_GROUPID_COMMS:
                    retVal = RPC_COMMS_IFC.sendCmd(msg->cmdID, msg->compID, msg->payLoad, msg->payLoadSize, RPC_CMDQ_EVENT, &msg->hdr, &forwardtoSlaves);
                break;
#endif
                default:
                break;
                }
                if ((BCM_ERR_OK == retVal) && (NULL != msg->hdr)) {
                    msg->status = RPC_CMDQ_MSG_STATUS_RUNNING;
                    msg->reqNumResp++;
                }
            } else {
                forwardtoSlaves = 1UL;
            }
            if (BCM_ERR_OK == retVal) {
                if (0UL != forwardtoSlaves) {
                    for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
                        if (RPC_CHANN_MODE_MASTER == RPC_GetChannelMode(IPC_ChannCfg[i].mode)) {
                            retVal = RPC_Send(i, rpcCmd, msg->payLoad, msg->payLoadSize);
                            if (BCM_ERR_OK == retVal) {
                                msg->reqNumResp++;
                                msg->status = RPC_CMDQ_MSG_STATUS_RUNNING;
                                msg->pendingResponseMask |= (1UL << i);
                            }
                        }
                    }
                }
            } else {
                /* Todo: failed to send command to local subsystem. What to do? */
                msg->result = BCM_ERR_UNKNOWN;
                RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, BCM_ERR_UNKNOWN,
                    (uint32_t)__LINE__, (uint32_t)(RPC_CmdQFullQ & 0xFFFFFFFFUL),
                    (uint32_t)((RPC_CmdQFullQ >> 32UL) & 0xFFFFFFFFUL), rpcCmd);
            }
        }
        if (RPC_CMDQ_MSG_STATUS_RUNNING != msg->status) {
            msg->status = RPC_CMDQ_MSG_STATUS_DONE;
            uint32_t rpcCmd = RPC_CMDID(msg->groupID, msg->compID, msg->cmdID);
            if (RPC_CHANNEL_LOCAL == msg->channel) {
                uint32_t idx = 0UL;
                retVal = MSGQ_GetMsgIdx(&RPC_CmdQ, &idx);
                if (BCM_ERR_OK == retVal) {

                    RPC_CmdQMsgs[idx].groupID = msg->groupID;
                    RPC_CmdQMsgs[idx].compID = msg->compID;
                    RPC_CmdQMsgs[idx].cmdID = msg->cmdID;
                    RPC_CmdQMsgs[idx].payLoadSize = msg->payLoadSize;
                    RPC_CmdQMsgs[idx].result = msg->result;
                    BCM_MemCpy(RPC_CmdQMsgs[idx].payLoad, msg->payLoad, msg->payLoadSize);

                    retVal = MSGQ_CompleteMsgIdx(&RPC_CmdQ, idx);
                    if (BCM_ERR_OK != retVal) {
                        /* Todo: Decide what action to be taken */
                    }
                } else {
                    /* Something is seriously wrong */
                    RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, retVal,
                        (uint32_t)__LINE__, msg->id, rpcCmd, msg->payLoadSize);
                }
            } else {
                BCM_MemMove(msg->payLoad + 4UL, msg->payLoad, msg->payLoadSize);
                BCM_MemCpy(msg->payLoad, &msg->result, 4UL);
                retVal = RPC_Send(msg->id, rpcCmd, msg->payLoad, msg->payLoadSize + 4UL);
                if (BCM_ERR_OK != retVal) {
                    /* Todo: Decide what action to be taken */
                    RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, BCM_ERR_UNKNOWN,
                        (uint32_t)__LINE__, msg->id, rpcCmd, msg->payLoadSize + 4UL);
                }
            }
        } else {
            break;
        }
        retVal = LWQ_Pop(&RPC_CmdQFullQ, &popIdx);
        if(BCM_ERR_OK == retVal){
            retVal = LWQ_Push(&RPC_CmdQFreeQ, popIdx);
            if(BCM_ERR_OK != retVal){
                RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, retVal, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
            }
        }
    }
}

/**
*/
int32_t RPC_CmdQAddCmd(RPC_CmdQRpcMsgType *aMsg)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t i = 0U;

    if (NULL == aMsg) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RPC_CHANNEL_MAX <= aMsg->channel) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RPC_RESPPAYLOADSZ < aMsg->payLoadSize) {
        retVal = BCM_ERR_NOMEM;
    } else {
        retVal = LWQ_Pop(&RPC_CmdQFreeQ, &i);
        if(BCM_ERR_OK == retVal){
            RPC_CmdQLocalQ[i].status = RPC_CMDQ_MSG_STATUS_INIT;
            RPC_CmdQLocalQ[i].channel = aMsg->channel;
            RPC_CmdQLocalQ[i].id = aMsg->id;
            RPC_CmdQLocalQ[i].groupID = RPC_GET_GROUPID(aMsg->cmd);
            RPC_CmdQLocalQ[i].compID = RPC_GET_COMP(aMsg->cmd);
            RPC_CmdQLocalQ[i].cmdID = RPC_GET_CMDID(aMsg->cmd);
            RPC_CmdQLocalQ[i].payLoadSize = aMsg->payLoadSize;
            if (aMsg->payLoadSize > 0) {
                BCM_MemCpy(RPC_CmdQLocalQ[i].payLoad, aMsg->payLoad, aMsg->payLoadSize);
            }
            RPC_CmdQLocalQ[i].result = BCM_ERR_OK;
            RPC_CmdQLocalQ[i].curNumResp = 0UL;
            RPC_CmdQLocalQ[i].reqNumResp = 0UL;
            RPC_CmdQLocalQ[i].hdr = NULL;
            RPC_CmdQLocalQ[i].pendingResponseMask = 0UL;
            retVal = LWQ_Push(&RPC_CmdQFullQ, i);
            if (BCM_ERR_OK != retVal) {
                /*Should never happen. But, what to do */
                RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, BCM_ERR_UNKNOWN,
                                    (uint32_t)__LINE__, (uint32_t)(RPC_CmdQFullQ & 0xFFFFFFFFUL),
                                    (uint32_t)((RPC_CmdQFullQ >> 32UL) & 0xFFFFFFFFUL), 0UL);
            }
        }
    }
    RPC_CmdQProcessCmd();

    return retVal;
}


/**
*/
void RPC_CmdQAddResponse(RPC_CmdQRpcMsgType *aMsg)
{
    RPC_CmdQMsgType *lMsg = NULL;
    uint8_t    j = 0U;
    int32_t retVal = BCM_ERR_OK;

    if ((NULL != aMsg) && (0UL < LWQ_Length(RPC_CmdQFullQ))) {
        /* Peek for first job */
        j = LWQ_PeekFirst(RPC_CmdQFullQ);
        lMsg = &RPC_CmdQLocalQ[j];

        if (RPC_CMDID(lMsg->groupID, lMsg->compID, lMsg->cmdID) == aMsg->cmd) {
            if (RPC_CHANNEL_LOCAL == aMsg->channel) {
                lMsg->hdr = NULL;
            } else {
                lMsg->pendingResponseMask &= ~(1UL << aMsg->id);
            }
            if (0UL == lMsg->curNumResp) {
                lMsg->result = aMsg->error;
                lMsg->payLoadSize = aMsg->payLoadSize;
                BCM_MemCpy(lMsg->payLoad, aMsg->payLoad, aMsg->payLoadSize);
            } else {
                BCM_MemCpy(RPC_TmpPayLoad, lMsg->payLoad, lMsg->payLoadSize);
                switch (lMsg->groupID) {
#ifdef RPC_SYS_IFC
                    case BCM_GROUPID_SYS:
                        if(NULL != RPC_SYS_IFC.mergeResult) {
                            retVal = RPC_SYS_IFC.mergeResult(lMsg->cmdID, lMsg->compID, lMsg->payLoadSize,
                                                             lMsg->payLoad, &lMsg->result,
                                                             RPC_TmpPayLoad, lMsg->result,
                                                             aMsg->payLoad, aMsg->error);
                        }
                    break;
#endif
#ifdef RPC_COMMS_IFC
                    case BCM_GROUPID_COMMS:
                        if(NULL != RPC_COMMS_IFC.mergeResult) {
                            retVal = RPC_COMMS_IFC.mergeResult(lMsg->cmdID, lMsg->compID, lMsg->payLoadSize,
                                                             lMsg->payLoad, &lMsg->result,
                                                             RPC_TmpPayLoad, lMsg->result,
                                                             aMsg->payLoad, aMsg->error);
                        }
                    break;
#endif
                    default:
                    break;
                }
                if (BCM_ERR_OK != retVal) {
                    RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, BCM_ERR_UNKNOWN,
                               (uint32_t)__LINE__, RPC_CMDID(lMsg->groupID, lMsg->compID, lMsg->cmdID), aMsg->cmd, 0UL);
                }
            }
            lMsg->curNumResp++;
            if (lMsg->curNumResp == lMsg->reqNumResp) {
                lMsg->status = RPC_CMDQ_MSG_STATUS_DONE;
                RPC_CmdQProcessCmd();
                /* Check if there are more Cmds to be handled */
                RPC_CmdQTransferCmd();
            }
        } else {
            /*  Unexpected response */
            RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, BCM_ERR_UNKNOWN,
                       (uint32_t)__LINE__, RPC_CMDID(lMsg->groupID, lMsg->compID, lMsg->cmdID), aMsg->cmd, 0UL);
        }
    }
}

/** @brief RPC Command Queue Event Handler

    Todo

    @behavior Sync, Re-entrant

    @pre Todo

    @post Todo

    @trace Todo
    @vtrace #RPC_SYS_IFC

    @limitations This API may not be supported on all the chipsets
*/

void RPC_CmdQEventHandler(void)
{
    int32_t          retVal = BCM_ERR_OK;
    uint8_t          j      = 0U;
    RPC_CmdQMsgType *msg    = NULL;

    RPC_CmdQTransferCmd();

    if (0UL < LWQ_Length(RPC_CmdQFullQ)) {
        /* Peek for first job */
        j = LWQ_PeekFirst(RPC_CmdQFullQ);
        msg = &RPC_CmdQLocalQ[j];
        if (NULL != msg->hdr) {
            uint8_t        cmdID = 0U;
            BCM_CompIDType compID = 0U;
            uint32_t       replyLen = 0UL;
            RPC_CmdQRpcMsgType tmp;
            compID = msg->compID;
            switch (msg->groupID) {
#ifdef RPC_SYS_IFC
            case BCM_GROUPID_SYS:
                retVal = RPC_SYS_IFC.getStatus(msg->hdr, compID, &cmdID, RPC_EventPayLoad, &replyLen);
            break;
#endif
#ifdef RPC_COMMS_IFC
            case BCM_GROUPID_COMMS:
                retVal = RPC_COMMS_IFC.getStatus(msg->hdr, compID, &cmdID, RPC_EventPayLoad, &replyLen);
            break;
#endif
            default:
            break;
            }
            if (BCM_ERR_BUSY != retVal) {
                tmp.channel = RPC_CHANNEL_LOCAL;
                tmp.id = 0UL;
                tmp.cmd = RPC_CMDID(msg->groupID, compID, cmdID);
                tmp.payLoad = RPC_EventPayLoad;
                tmp.payLoadSize = replyLen;
                tmp.error = retVal;
                RPC_CmdQAddResponse(&tmp);
            }
        }
    }
}

/**
*/
void RPC_CmdQDeInit(void)
{
    /* TODO: Add logic to check current command if it is Reboot command */
}


/** @} */


/**
    @addtogroup grp_rpc_cmd_queue_ifc
    @{
*/

/**

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t RPC_CmdQSendCmd(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                       const uint8_t *const aParams, uint32_t aSize, uint32_t aClientMask,
                       const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_OK;
    RPC_CmdQMsgType msg;

    if (((aParams == NULL) && (0UL != aSize))
        || (aSize > RPC_RESPPAYLOADSZ)
        || (NULL == aHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        msg.status = RPC_CMDQ_MSG_STATUS_INIT;
        msg.channel = RPC_CHANNEL_LOCAL;
        msg.id = 0UL;
        msg.groupID = aGrpID;
        msg.compID = aCompID;
        msg.cmdID = aCmdID;
        msg.payLoadSize = aSize;
        msg.result = BCM_ERR_OK;
        if(aParams != NULL){
            BCM_MemCpy(msg.payLoad, aParams, aSize);
        }
        msg.curNumResp = 0UL;
        msg.reqNumResp = 0UL;
        msg.hdr = NULL;
        msg.pendingResponseMask = 0UL;

        /* Post the event into the queue */
        retVal = MSGQ_SendMsg(&RPC_CmdQ, &msg, aClientMask, aHdr);
        if (BCM_ERR_OK != retVal){
            RPC_CmdQReportError(BRCM_SWARCH_RPC_SEND_CMD_PROC, retVal, (uint32_t)__LINE__, (uint32_t) msg.groupID,
                            (uint32_t) msg.compID, (uint32_t) msg.cmdID);
        }
    }
    return retVal;
}

int32_t RPC_CmdQGetCmdStatus(const MSGQ_MsgHdrType *const aHdr, int32_t * const aResponse,
                           uint8_t *const aParams, uint32_t * const aSize)
{
    int32_t err;
    RPC_CmdQMsgType msg;
    int32_t status;

    if ((NULL == aHdr)
        || ((NULL != aSize) && ((NULL == aParams) || (RPC_RESPPAYLOADSZ < *aSize)))) {
        status = BCM_ERR_INVAL_PARAMS;
    } else {
        err = MSGQ_RecvMsg(&RPC_CmdQ, aHdr, &msg);
        if (BCM_ERR_OK == err) {
            if (NULL != aSize) {
                BCM_MemCpy(aParams, msg.payLoad, msg.payLoadSize);
                *aSize = msg.payLoadSize;
            }
            *aResponse = msg.result;
            status = BCM_ERR_OK;
        } else if (BCM_ERR_BUSY == err) {
            status = BCM_ERR_BUSY;
        } else {
            status = BCM_ERR_UNKNOWN;
        }
    }

    return status;
}


/** @} */
