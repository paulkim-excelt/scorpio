/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_comms_notify_impl Comms Notification Implementation
    @ingroup grp_comms_sw

    @addtogroup grp_comms_notify_impl
    @{

    @file ethernet/common/comms/os/common/comms.c
    @brief Subsystem Notifications Implementation

    @version 0.1 Initial version
*/

#include <comms.h>
#include <msg_queue.h>
#include <bcm_err.h>
#include <stdlib.h>
#include "ee.h"

/**
    @name Subsystem Notificaiton Design IDs
    @{
    @brief Design IDs for Subsystem Notificaiton

*/
#define BRCM_SWDSGN_COMMS_CONTEXT_GLOBAL       (0x80U)   /**< @brief #COMP_Context        */
#define BRCM_SWDSGN_COMMS_FUNCTION_PART_PROC   (0x81U)   /**< @brief #COMP_FunctionPart   */
#define BRCM_SWDSGN_COMMS_FUNCTION_PROC        (0x82U)   /**< @brief #COMP_Function       */
#define BRCM_SWDSGN_COMMS_INT_STATE_MACRO      (0x83U)   /**< @brief #COMP_IntStateType   */
#define BRCM_SWDSGN_COMMS_CONTEXT_DEFINE_MACRO (0x84U)   /**< @brief #COMP_CONTEXT_DEFINE */
#define BRCM_SWDSGN_COMMS_INTCONTEXT_TYPE      (0x85U)   /**< @brief #COMP_IntContextType */
#define BRCM_SWDSGN_COMMS_INTCONTEXT_GLOBAL    (0x86U)   /**< @brief #COMP_IntContext     */
/** @} */

/**
    @brief Global context for COMP_Context

    Detailed description of the GComp_Context

    @anchor COMP_Context
    @trace #BRCM_SWARCH_COMP_CONTEXT_TYPE
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY
*/
typedef struct _COMMS_SubsystemNotifType {
    COMMS_SubsystemIDType id;
    COMMS_SubsystemStateType state;
} COMMS_SubsystemNotifType;

/**
    @brief Global context for COMP_Context

    Detailed description of the GComp_Context

    @anchor COMP_Context
    @trace #BRCM_SWARCH_COMP_CONTEXT_TYPE
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY
*/
COMMS_SubsystemNotifType COMMS_SubsystemNotif[MSGQ_SIZE];

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
void* COMMS_GetNotif(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {
        pRet = (void *)(&COMMS_SubsystemNotif[idx]);
    }
    return pRet;
}

/**
    @brief #MSGQ_HdrQType instance of COMMS command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(COMMS_NotifHdrQ);

/**
    @brief Command Message Queue

    The message queue for the events notified to switch. System task
    processes them asynchronously.

    @trace #TBD
*/
MSGQ_DEFINE((COMMS_NotifQ), (CommsTask), (COMMS_NOTIF_EVENT),
            (MSGQ_CLIENT_SIGNAL_DISABLE), COMMS_SubsystemNotifType, (&COMMS_NotifHdrQ),
            (COMMS_GetNotif));

/** @} */

/**
    @addtogroup grp_comms_notify
    @{
*/

/**
    This function could further check the task ID which is calling this API and Is the task
    expected to raise this notification, in future.

    @trace #BRCM_SWARCH_COMP_FUNCTION_PROC
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY

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
void COMMS_NotifyState(COMMS_SubsystemIDType aID, COMMS_SubsystemStateType aState)
{
    COMMS_SubsystemNotifType notif;
    if ((aID < COMMS_SUBSYSTEM_MAX) && (aState <= COMMS_SUBSYSTEM_STATE_ERROR)) {
        notif.id = aID;
        notif.state = aState;
        (void)MSGQ_SendMsg(&COMMS_NotifQ, &notif, MSGQ_NO_CLIENTMASK, NULL);
    }
}

/**
    This function could further check the task ID which is calling this API and Is the task
    expected to raise this notification, in future.

    @trace #BRCM_SWARCH_COMP_FUNCTION_PROC
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY

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
int32_t COMMS_GetNotification(COMMS_SubsystemIDType *aID, COMMS_SubsystemStateType *aState)
{
    uint32_t idx;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    if ((NULL != aID) && (NULL != aState)) {
        retVal = MSGQ_GetMsgIdx(&COMMS_NotifQ, &idx);
        if (BCM_ERR_OK == retVal) {
            *aID = COMMS_SubsystemNotif[idx].id;
            *aState = COMMS_SubsystemNotif[idx].state;
            (void)MSGQ_CompleteMsgIdx(&COMMS_NotifQ, idx);
        } else {
            retVal = BCM_ERR_NOMEM;
        }
    }
    return retVal;
}

/** @} */
