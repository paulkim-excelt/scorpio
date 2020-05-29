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
    @defgroup grp_rpc_server_async_impl Async Notifications Implementation
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_server_async_impl
    @{

    @file rpc_async.c
    @brief RPC Server Async Notifications Implementation

    @version 0.1 Initial version
*/
#include <ipc_osil.h>
#include <rpc_async_osil.h>
#include <msg_queue.h>
#include <stdlib.h>
#include "ee.h"

#include <bcm_utils.h>
#include <utils.h>

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component
*/
#define BRCM_SWDSGN_COMP_CONTEXT_GLOBAL       (0x80U)   /**< @brief #COMP_Context        */
#define BRCM_SWDSGN_COMP_FUNCTION_PART_PROC   (0x81U)   /**< @brief #COMP_FunctionPart   */
#define BRCM_SWDSGN_COMP_FUNCTION_PROC        (0x82U)   /**< @brief #COMP_Function       */
#define BRCM_SWDSGN_COMP_INT_STATE_MACRO      (0x83U)   /**< @brief #COMP_IntStateType   */
#define BRCM_SWDSGN_COMP_CONTEXT_DEFINE_MACRO (0x84U)   /**< @brief #COMP_CONTEXT_DEFINE */
#define BRCM_SWDSGN_COMP_INTCONTEXT_TYPE      (0x85U)   /**< @brief #COMP_IntContextType */
#define BRCM_SWDSGN_COMP_INTCONTEXT_GLOBAL    (0x86U)   /**< @brief #COMP_IntContext     */
/** @} */

static void RPC_AsyncReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                  uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RPC_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief Structure used to manage entries in the MsgQ of system
*/
typedef struct _RPC_AsycEventType {
    uint32_t               isResponse;                  /**< @brief Is this a response to a command */
    uint8_t                groupId;                     /**< @brief Notification group ID */
    uint16_t               compID;                      /**< @brief Component ID */
    uint16_t               cmdID;                       /**< @brief Async Notification ID */
    uint32_t               payloadSize;                 /**< @brief Payload size in bytes */
    uint8_t                payload[RPC_RESPPAYLOADSZ];  /**< @brief Space for notifcation payload */
} RPC_AsycEventType;

/**
    @brief RPC events memory
*/
static RPC_AsycEventType RPC_AsyncEvents[MSGQ_SIZE];

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
void* RPC_AsyncEventGetMessage(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {
        pRet = (void *)(&RPC_AsyncEvents[idx]);
    }
    return pRet;
}

/**
    @brief #MSGQ_HdrQType instance of RPC command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(RPC_AsyncEventHdrQ);

/**
    @brief Command Message Queue

    The message queue for the events notified to switch. System task
    processes them asynchronously.

    @trace #TBD
*/
MSGQ_DEFINE((RPC_AsyncEventQ), (IPC_SERVER_TASK), (RPC_ASYNC_EVENT),
            (MSGQ_CLIENT_SIGNAL_DISABLE), RPC_AsycEventType, (&RPC_AsyncEventHdrQ),
            (RPC_AsyncEventGetMessage));


/**
    @code{.unparsed}
    @endcode
*/
void RPC_AsyncProcessEvents(void)
{
    uint32_t idx = 0UL;
    int32_t    retVal = BCM_ERR_OK;
    RPC_AsycEventType *eventPtr;

    do {
        retVal = MSGQ_GetMsgIdx(&RPC_AsyncEventQ, &idx);

        if (BCM_ERR_OK == retVal) {
            eventPtr = &RPC_AsyncEvents[idx];

            if (eventPtr->payloadSize <= RPC_RESPPAYLOADSZ) {
                /* For events, notify them immediately */
                uint32_t i;
                uint32_t cmd = 0UL;
                if (FALSE == eventPtr->isResponse) {
                    cmd = RPC_ASYNCID(eventPtr->groupId, eventPtr->compID, eventPtr->cmdID);
                } else {
                    cmd = RPC_CMDID(eventPtr->groupId, eventPtr->compID, eventPtr->cmdID);
                }

                /* find the host channel and send the Async */
                for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
                    if (RPC_CHANN_MODE_SLAVE == RPC_GetChannelMode(IPC_ChannCfg[i].mode)) {
                        break;
                    }
                }
                if (i < RPC_MAX_CHANNELS) {
                    retVal = RPC_Send(i, cmd, eventPtr->payload, eventPtr->payloadSize);
                } else {
                    retVal = BCM_ERR_UNKNOWN;
                }
            } else {
                /* Cannot send the RPC message. Size passed is invalid */
                retVal = BCM_ERR_INVAL_PARAMS;
            }

            if (BCM_ERR_OK != retVal) {
                RPC_AsyncReportError(BRCM_SWARCH_RPC_ASYNC_PROCESS_PROC, retVal,
                               (uint32_t)__LINE__, 0UL, 0UL, 0UL);
                /* In case RPC send fails due to all buffers being filled, */
                /* then wait till HOST reads them out and clears buffers   */
                if (BCM_ERR_NOMEM == retVal) {
                    break;
                }
            }

            /* Update the command processing as complete */
            retVal = MSGQ_CompleteMsgIdx(&RPC_AsyncEventQ, idx);
        }
    } while(BCM_ERR_OK == retVal);

}


/** @} */

/**
    @addtogroup grp_rpc_server_async
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
int32_t RPC_AsyncEvent(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                       const uint8_t *const aParams, uint32_t aSize)
{
    int32_t retVal = BCM_ERR_OK;
    RPC_AsycEventType eventMsg;

    if (((aParams == NULL) && (0UL != aSize)) ||
         (aSize > RPC_RESPPAYLOADSZ)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        const MSGQ_MsgHdrType *qHdr;

        eventMsg.isResponse = FALSE;
        eventMsg.groupId = aGrpID;
        eventMsg.compID = aCompID;
        eventMsg.cmdID = aCmdID;
        eventMsg.payloadSize = aSize;
        if(aParams != NULL){
            BCM_MemCpy(eventMsg.payload, aParams, aSize);
        }
        /* Post the event into the queue */
        retVal = MSGQ_SendMsg(&RPC_AsyncEventQ, &eventMsg, MSGQ_NO_CLIENTMASK, &qHdr);
        if (BCM_ERR_OK != retVal){
            RPC_AsyncReportError(BRCM_SWARCH_RPC_ASYNC_NOTIFY_PROC, retVal, (uint32_t)__LINE__, (uint32_t) eventMsg.groupId,
                            (uint32_t) eventMsg.compID, (uint32_t) eventMsg.cmdID);
        }
    }
    return retVal;
}

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
int32_t RPC_AsyncResponse(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                       const uint8_t *const aParams, uint32_t aSize)
{
    int32_t retVal = BCM_ERR_OK;
    RPC_AsycEventType eventMsg;

    if (((aParams == NULL) && (0UL != aSize)) ||
         (aSize > RPC_RESPPAYLOADSZ)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        const MSGQ_MsgHdrType *qHdr;

        eventMsg.isResponse = TRUE;
        eventMsg.groupId = aGrpID;
        eventMsg.compID = aCompID;
        eventMsg.cmdID = aCmdID;
        eventMsg.payloadSize = aSize;
        if(aParams != NULL){
            BCM_MemCpy(eventMsg.payload, aParams, aSize);
        }
        /* Post the event into the queue */
        retVal = MSGQ_SendMsg(&RPC_AsyncEventQ, &eventMsg, MSGQ_NO_CLIENTMASK, &qHdr);
        if (BCM_ERR_OK != retVal){
            RPC_AsyncReportError(BRCM_SWARCH_RPC_ASYNC_NOTIFY_PROC, retVal, (uint32_t)__LINE__, (uint32_t) eventMsg.groupId,
                            (uint32_t) eventMsg.compID, (uint32_t) eventMsg.cmdID);
        }
    }
    return retVal;
}

/** @} */
