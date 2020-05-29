/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_sys_ipc_impl System IPC Handler Implementation
    @ingroup grp_sys_ipc

    @addtogroup grp_sys_ipc_impl
    @{

    @file sys_ipc.c
    @brief System IPC Handler Implementation

    @version 0.1 Initial version
*/

#include <osil/sys_ipc_osil.h>
#include <stdlib.h>
#include <otp.h>
#include <mcu.h>
#include <mcu_ext.h>
#include <chip_config.h>
#include <build_info.h>
#include <utils.h>
#include <rpc_comp_ifc.h>
#include "ee.h"

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component

    @note If any new interface (macro/typedef/function or any other) is being
    added, the corresponding API ID shall be added at the end of the  Component
    Design ID list, immaterial of where the interface is placed in the header.
    This is to ensure that the existing SWDSGN IDs remains unchanged.
*/
#define BRCM_SWDSGN_COMP_CONTEXT_GLOBAL       (0x80U)   /**< @brief #COMP_Context        */
#define BRCM_SWDSGN_COMP_FUNCTION_PART_PROC   (0x81U)   /**< @brief #COMP_FunctionPart   */
#define BRCM_SWDSGN_COMP_FUNCTION_PROC        (0x82U)   /**< @brief #COMP_Function       */
#define BRCM_SWDSGN_COMP_INT_STATE_MACRO      (0x83U)   /**< @brief #COMP_IntStateType   */
#define BRCM_SWDSGN_COMP_CONTEXT_DEFINE_MACRO (0x84U)   /**< @brief #COMP_CONTEXT_DEFINE */
#define BRCM_SWDSGN_COMP_INTCONTEXT_TYPE      (0x85U)   /**< @brief #COMP_IntContextType */
#define BRCM_SWDSGN_COMP_INTCONTEXT_GLOBAL    (0x86U)   /**< @brief #COMP_IntContext     */
/** @} */


/* Allocate memory for creating the response for incoming commands */
static uint8_t SYS_CmdReply[RPC_MSG_PAYLOAD_SZ];

static uint8_t SYS_RebootCmdReceived = FALSE;
static SYS_RebootCmdType SYS_RebootRequest;


/**
    @brief Structure used to manage entries in the MsgQ of system
*/
typedef struct _SYS_CmdRespType {
    uint8_t                groupId;                     /**< @brief Notification group ID */
    BCM_CompIDType         compID;                      /**< @brief Component ID */
    SYS_CmdIdType          cmdID;                       /**< @brief Async Notification ID */
    uint32_t               payloadLen;                  /**< @brief Payload size in bytes */
    uint8_t                payload[RPC_RESPPAYLOADSZ];  /**< @brief Space for notifcation payload */
    int32_t                ret;                         /**< @brief Return status */
} SYS_CmdRespType;


/**
    @brief SYS events memory
*/
SYS_CmdRespType SYS_Cmds[MSGQ_SIZE];

/**
    @brief Get the i-th message from server

    Retrieve the i-th message from server.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]  idx         Index of the message

    @return                 Pointer to the i-th message

    @post None

    @note To be used.

    @trace #TBD

    @limitations None
*/
void* SYS_GetCmd(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {
        pRet = (void *)(&SYS_Cmds[idx]);
    }
    return pRet;
}

/**
    @brief #MSGQ_HdrQType instance of SYS command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(SYS_CmdHdrQ);

/**
    @brief Command Message Queue

    The message queue for the events notified to switch. System task
    processes them asynchronously.

    @trace #TBD
*/
MSGQ_DEFINE((SYS_CmdQ), (SYS_TASK_NAME), (IPC_SYS_MSGQ_EVENT),
            (MSGQ_CLIENT_SIGNAL_ENABLE), SYS_CmdRespType, (&SYS_CmdHdrQ),
            (SYS_GetCmd));



/**
    @code{.unparsed}
    BCM_ReportError();
    @endcode
*/
static void SYS_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_SYS_ID & BCM_LOGMASK_USER), 0U, aApiID,
                    aErr, 4UL, values);
}

/** @brief IPC common command handler

    This is the handler for all system commands. They are processed
    synchronously in the same context.

    @behavior Sync, Non-rentrant

    @pre None

    @param[in]      aCmdID      Command ID value
    @param[in]      aCompID     Component ID value
    @param[in]      aCmd        Management command information
    @param[in]      aLen        Length of the management command message
    @param[out]     aReplyLen   Length in bytes of the response

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command processed successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters i.e. NULL pointers
    @retval     #BCM_ERR_NOSUPPORT      For commands that are not supported
                                        by this handler

    @post None
*/
static int32_t SYS_CmdHandler(SYS_CmdIdType aCmdID, BCM_CompIDType aCompID,
                                    uint8_t *aCmd, uint32_t aLen, uint32_t *aReplyLen)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t param;
    SYS_HandleType response;
#if defined(ENABLE_OTP) || defined(ENABLE_DBGMEM)
    SYS_HandleType command;
#endif
#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
    uint32_t data, data2;
    uint32_t *temp;
#endif

#if defined(ENABLE_OTP) || defined(ENABLE_DBGMEM)
    command.u8Ptr = aCmd;
#endif

    response.u8Ptr = SYS_CmdReply;

    switch (aCmdID){
        case SYS_CMDID_SW_VERSION:
            param = sizeof(response.osVersion->str);
            BuildInfo_GetVersionInfo(&response.osVersion->str[0],
                    &param);
            *aReplyLen = sizeof(SYS_OSSWVersionType);
            retVal = BCM_ERR_OK;
            break;
        case SYS_CMDID_REBOOT:
            SYS_RebootCmdReceived = TRUE;
            SYS_RebootRequest = *(command.reboot);
            /* Actual Reboot in case of chips which support self-reboot, will be
               performed after sending response to HOST and REBOOT operation is
               assumed to succeed later.
               In case of chips which DON'T support self-reboot, action is just to
               de-init IPC after sending the response to HOST */
            *aReplyLen = 0UL;
            retVal = BCM_ERR_OK;
            break;
        case SYS_CMDID_CHIP_ID:
            MCU_GetVersion(response.mcuVersion);
            *aReplyLen = sizeof(MCU_VersionType);
            retVal = BCM_ERR_OK;
            break;
        case SYS_CMDID_PING:
            BCM_MemSet(response.ping,0U,sizeof(SYS_PingType));
            /* Command to inform that firmware is running */
            response.ping->mode = SYS_BOOTMODE_FW;
            MCU_GetVersion(&response.ping->version);
            *aReplyLen = sizeof(SYS_PingType);
            retVal = BCM_ERR_OK;
            break;
#ifdef ENABLE_DBGMEM
        case SYS_CMDID_MEM_WRITE:
            retVal = DBGMEM_Copy(command.memAccess->addr,
                    (uint32_t )(intptr_t)&command.memAccess->data[0],
                    command.memAccess->width,
                    command.memAccess->len);
            *aReplyLen = sizeof(SYS_MemAccessType);
            break;
        case SYS_CMDID_MEM_READ:
            retVal = DBGMEM_Copy((uint32_t)(intptr_t)&response.memAccess->data[0],
                    command.memAccess->addr,
                    command.memAccess->width,
                    command.memAccess->len);
            *aReplyLen = sizeof(SYS_MemAccessType);
            break;
#endif
#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
        case SYS_CMDID_OTP_RD:
            retVal = OTP_Read(OTP_HW_ID_0, command.otpInfo->addr,
                    (uint32_t *)&(response.otpInfo->data[0]));
            response.otpInfo->size = 4UL;
            *aReplyLen = 12UL;
            break;
        case SYS_CMDID_OTP_WR_SEC_KEY:
            retVal = BCM_ERR_NOSUPPORT;
            break;
        case SYS_CMDID_OTP_ENABLE_SEC:
            retVal = BCM_ERR_NOSUPPORT;
            break;
        case SYS_CMDID_OTP_WRITE_CUST:
            retVal = BCM_ERR_NOSUPPORT;
            break;
        case SYS_CMDID_OTP_WR_MAC_ADDR:
            retVal = BCM_ERR_NOSUPPORT;
            break;
        case SYS_CMDID_OTP_RD_MAC_ADDR:
            temp = (uint32_t *)(&response.otpInfo->data[0UL]);
            *temp = 0x3UL;

            /* Read MAC address copy 1 */
            retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_0_OCTET123_ADDR,
                    &data);
            if ((BCM_ERR_OK != retVal) && (BCM_ERR_DATA_INTEG != retVal)) {
                break;
            }
            if (BCM_ERR_DATA_INTEG == retVal) {
                *temp = (*temp) & (~0x1UL);
            }
            retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_0_OCTET456_ADDR, &data2);
            if ((BCM_ERR_OK != retVal) && (BCM_ERR_DATA_INTEG != retVal)) {
                break;
            }
            if (BCM_ERR_DATA_INTEG == retVal) {
                *temp = (*temp) & (~0x1UL);
            }

            response.otpInfo->data[4UL] = (data >> 16UL) & 0xFFUL;
            response.otpInfo->data[5UL] = (data >> 8UL) & 0xFFUL;
            response.otpInfo->data[6UL] = data & 0xFFUL;
            response.otpInfo->data[7UL] = (data2 >> 16UL) & 0xFFUL;
            response.otpInfo->data[8UL] = (data2 >> 8UL) & 0xFFUL;
            response.otpInfo->data[9UL] = data2 & 0xFFUL;

            /* Read MAC address copy 2 */
            retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_1_OCTET123_ADDR, &data);
            if ((BCM_ERR_OK != retVal) && (BCM_ERR_DATA_INTEG != retVal)) {
                break;
            }
            if (BCM_ERR_DATA_INTEG == retVal) {
                *temp = (*temp) & (~0x2UL);
            }
            retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_1_OCTET456_ADDR, &data2);
            if (BCM_ERR_OK != retVal) {
                break;
            }
            if (BCM_ERR_DATA_INTEG == retVal) {
                *temp = (*temp) & (~0x2UL);
            }

            response.otpInfo->data[10UL] = (data >> 16UL) & 0xFFUL;
            response.otpInfo->data[11UL] = (data >> 8UL) & 0xFFUL;
            response.otpInfo->data[12UL] = data & 0xFFUL;
            response.otpInfo->data[13UL] = (data2 >> 16UL) & 0xFFUL;
            response.otpInfo->data[14UL] = (data2 >> 8UL) & 0xFFUL;
            response.otpInfo->data[15UL] = data2 & 0xFFUL;

            response.otpInfo->size = 16UL;
            *aReplyLen = 24UL;

            break;
#endif
        default:
            retVal = BCM_ERR_NOSUPPORT;
            break;
    }

    /* copy the response to payload */
    BCM_MemCpy(aCmd, SYS_CmdReply, *aReplyLen);

    if (BCM_ERR_OK != retVal){
        SYS_ReportError(BRCM_SWARCH_SYS_PROCESS_CMDS_PROC, retVal,
                        (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/** @brief Retreive status of management commands

    This API queries the status of completed management commands which were
    previously issued to SYS

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      pHdr        Pointer to the message queue header used to
                                retrieve the status
    @param[in]      aCompID     Pointer to the component id to be filled by SYS
    @param[out]     aCmdID      Pointer to the command id to be filled by SYS
    @param[out]     aReply      Pointer to reply message to be filled by SYS
    @param[in]      aReplyLen   Pointer to length of reply message to be filled by SYS

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command status retrieved successfully

    @post None

    @trace  #TBD

    @limitations This API may not be supported on all the chipsets
*/
static int32_t SYS_CmdStatus(const MSGQ_MsgHdrType* const pHdr,
                          BCM_CompIDType aCompID, SYS_CmdIdType* const aCmdID,
                           uint8_t *aReply, uint32_t *aReplyLen)
{
    int32_t           retVal = BCM_ERR_OK;
    SYS_CmdRespType cmdResp;

    if ((pHdr != NULL) && (aReply != NULL) && (aReplyLen != NULL)) {
        retVal = MSGQ_RecvMsg(&SYS_CmdQ, pHdr, &cmdResp);
        if (BCM_ERR_OK == retVal) {
            *aCmdID  = cmdResp.cmdID;

            if(RPC_RESPPAYLOADSZ > cmdResp.payloadLen){
                /* Copy the response payload */
                BCM_MemCpy(aReply, cmdResp.payload, cmdResp.payloadLen);
                *aReplyLen = cmdResp.payloadLen;
            } else {
                /* Case when the response payload is incorrect */
                cmdResp.ret = BCM_ERR_INVAL_PARAMS;
            }

            /* Copy the response status */
            if (BCM_ERR_BUSY != cmdResp.ret) {
                retVal = cmdResp.ret;
            } else {
                retVal = BCM_ERR_UNKNOWN;
            }
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/** @} */

/**
    @addtogroup grp_sys_ipc
    @{
*/

/** @brief Send commands to SYS

    This API queues the management commands to SYS

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Management command information
    @param[in]      aLen                Length of the management command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     pHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command posted successfully

    @post None

    @trace  #TBD
    @trace #BRCM_SWARCH_COMP_FUNCTION_PROC
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY

    @limitations This API may not be supported on all the chipsets
*/
static int32_t SYS_SendCmd(SYS_CmdIdType aCmdID, BCM_CompIDType aCompID, uint8_t *aCmd,
                        uint32_t aLen, const uint32_t aClientMask,
                        const MSGQ_MsgHdrType** const pHdr, uint32_t *aForwardToSlaves)
{
    int32_t              retVal = BCM_ERR_OK;
    SYS_CmdRespType      cmdResp;
    uint32_t             sendCmd = TRUE;
    SYS_HandleType       paylPtr;
    MCU_ExtendedInfoType   stackingInfo = {0U};

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        SYS_ReportError(BRCM_SWARCH_SYS_SENDCMD_PROC, retVal,
                       (uint32_t)__LINE__, 0UL, 0UL, 0UL);
        goto error;
    }

    if ((aCmd != NULL) && (pHdr != NULL) && (aForwardToSlaves != NULL)
        && (aClientMask != 0UL) && (RPC_RESPPAYLOADSZ > aLen)) {
        /* Map the incoming request */
        cmdResp.cmdID   = aCmdID;
        cmdResp.compID  = aCompID;
        cmdResp.ret     = BCM_ERR_UNKNOWN;
        /* Copy the command payload */
        BCM_MemCpy(cmdResp.payload, aCmd, aLen);
        cmdResp.payloadLen = aLen;
        /* All system commands are consumed by System Task */
        *aForwardToSlaves = 0UL;
        paylPtr.u8Ptr = aCmd;

        switch (aCmdID) {
#ifdef ENABLE_DBGMEM
            case SYS_CMDID_MEM_WRITE:
            case SYS_CMDID_MEM_READ:
                if ((0U == stackingInfo.stackingEn) ||
                    ((1U == stackingInfo.stackingEn) &&
                     (stackingInfo.mstSlvMode == paylPtr.memAccess->deviceID))) {
                    *aForwardToSlaves = FALSE;
                    sendCmd = TRUE;
                } else {
                    *aForwardToSlaves = TRUE;
                    sendCmd = FALSE;
                }
                break;
#endif
            default:
                break;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    if ((BCM_ERR_OK == retVal) && (TRUE == sendCmd)) {
        retVal = MSGQ_SendMsg(&SYS_CmdQ, &cmdResp, aClientMask, pHdr);
        if ((BCM_ERR_OK != retVal) || (*pHdr == NULL)) {
            SYS_ReportError(BRCM_SWARCH_SYS_SENDCMD_PROC, retVal,(uint32_t) aClientMask,
                             (uint32_t) pHdr, 0UL, 0UL);
        }
    }

error:
    return retVal;
}

/**
*/
void SYS_ProcessCmds(void)
{
    uint32_t idx = 0UL;
    int32_t    retVal = BCM_ERR_OK;
    SYS_CmdRespType *eventPtr;
    do {
        retVal = MSGQ_GetMsgIdx(&SYS_CmdQ, &idx);

        if(BCM_ERR_OK == retVal) {
            uint32_t replyLen = 0UL;
            eventPtr = &SYS_Cmds[idx];

            eventPtr->ret = SYS_CmdHandler(eventPtr->cmdID,
                                                 eventPtr->compID,
                                                 eventPtr->payload,
                                                 eventPtr->payloadLen,
                                                 &replyLen);

            eventPtr->payloadLen = replyLen;

            /* Update the command processing as complete */
            retVal = MSGQ_CompleteMsgIdx(&SYS_CmdQ, idx);
        }
    } while(BCM_ERR_OK == retVal);
}

int32_t SYS_GetRebootRequest(SYS_RebootCmdType *aReboot)
{
    int32_t retVal;

    if (NULL == aReboot) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (FALSE == SYS_RebootCmdReceived) {
        retVal = BCM_ERR_NOT_FOUND;
    } else {
        *aReboot = SYS_RebootRequest;
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

const RPC_CompType SYS_RpcIfc = {
    .sendCmd = SYS_SendCmd,
    .getStatus= SYS_CmdStatus,
    .mergeResult= NULL,
};
/** @} */
