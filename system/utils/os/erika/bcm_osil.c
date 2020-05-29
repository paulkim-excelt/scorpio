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


/**
    @addtogroup grp_utils_os
    @{

    @file bcm_osil.c
    @brief OS Function Wrappers
    This source file contains the wrapper functions for OS related APIs.
    @version 1.0 Initial Version
*/

#include <osil/bcm_osil.h>
#include "ee.h"
#include "kernel/oo/inc/ee_oo_internal.h"

/**
    @name  OS wrapper functions Design IDs
    @{
    @brief Design IDs for OS wrapper functions
*/
#define BRCM_SWDSGN_BCM_OS_WAITEVENT_PROC           (0x00U)         /**< @brief #BCM_WaitEvent              */
#define BRCM_SWDSGN_BCM_OS_SETEVENT_PROC            (0x01U)         /**< @brief #BCM_SetEvent               */
#define BRCM_SWDSGN_BCM_OS_GETEVENT_PROC            (0x02U)         /**< @brief #BCM_GetEvent               */
#define BRCM_SWDSGN_BCM_OS_CLEAREVENT_PROC          (0x03U)         /**< @brief #BCM_ClearEvent             */
#define BRCM_SWDSGN_BCM_OS_ACTIVATETASK_PROC        (0x04U)         /**< @brief #BCM_ActivateTask           */
#define BRCM_SWDSGN_BCM_OS_TERMINATETASK_PROC       (0x05U)         /**< @brief #BCM_TerminateTask          */
#define BRCM_SWDSGN_BCM_OS_THREADNOTTERMINATED_PROC (0x06U)         /**< @brief #BCM_ThreadNotTerminated    */
#define BRCM_SWDSGN_BCM_OS_STARTOS_PROC             (0x07U)         /**< @brief #BCM_StartOS                */
#define BRCM_SWDSGN_BCM_OS_SETRELALARM_PROC         (0x08U)         /**< @brief #BCM_SetRelAlarm            */
#define BRCM_SWDSGN_BCM_OS_CANCELALARM_PROC         (0x09U)         /**< @brief #BCM_CancelAlarm            */
/** @} */

/**
    Detailed design of BCM_WaitEvent
    This function will act as a wrapper over BCM_WaitEvent function.
    It will takes event mask and waits for one of those events.

    @trace #BRCM_SWARCH_BCM_OS_WAITEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = WaitEvent(aEventMask);
    if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == retVal){
        ret = BCM_ERR_BUSY;
    } else if (E_OS_ACCESS == retVal){
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else{
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_WaitEvent(BCM_EventMaskType aEventMask)
{
    int32_t ret;
    StatusType retVal;

    retVal = WaitEvent((EventMaskType)aEventMask);

    if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == retVal){
        ret = BCM_ERR_BUSY;
    } else if (E_OS_ACCESS == retVal){
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else{
        ret = BCM_ERR_OK;
    }


    return ret;
}


/**
    Detailed design of BCM_SetEvent
    This function will act as a wrapper over BCM_SetEvent function.
    It sets an Event on a given Task with a given Mask.

    @trace #BRCM_SWARCH_BCM_OS_SETEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = SetEvent(aTaskID, aEventMask);
    if (E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_SetEvent(BCM_TaskType aTaskID, BCM_EventMaskType aEventMask)
{
    int32_t ret;
    StatusType retVal;

    retVal = SetEvent((TaskType)aTaskID, (EventMaskType)aEventMask);

    if (E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/**
    Detailed design of BCM_GetEvent
    This function will act as a wrapper over BCM_GetEvent function.
    It will get the EventMask set on a given task.

    @trace #BRCM_SWARCH_BCM_OS_SETEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = GetEvent(aTaskID, aEventMask);
    if (E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_GetEvent(BCM_TaskType aTaskID, BCM_EventMaskType *aEventMask)
{
    int32_t ret;
    StatusType retVal;

    retVal = GetEvent((TaskType)aTaskID, (EventMaskRefType)aEventMask);

    if (E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/**
    Detailed design of BCM_ClearEvent
    This function will act as a wrapper over BCM_ClearEvent function.
    It Clears an Event for given Task and Mask.

    @trace #BRCM_SWARCH_BCM_OS_SETEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = ClearEvent(aEventMask);
    if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_ClearEvent(BCM_EventMaskType aEventMask)
{
    int32_t ret;
    StatusType retVal;

    retVal = ClearEvent((EventMaskType)aEventMask);

    if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == retVal) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}


/**
    Detailed design of BCM_ActivateTask
    This function will act as a wrapper over BCM_ActivateTask function.
    It activates a task.

    @trace #BRCM_SWARCH_BCM_OS_SETEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = ActivateTask(aTaskID);
    if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_LIMIT == retVal) {
        ret = BCM_ERR_MAX_ATTEMPS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_ActivateTask(BCM_TaskType aTaskID)
{
    int32_t ret;
    StatusType retVal;

    retVal = ActivateTask((TaskType)aTaskID);

    if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_LIMIT == retVal) {
        ret = BCM_ERR_MAX_ATTEMPS;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/**
    Detailed design of BCM_TerminateTask
    This function will act as a wrapper over BCM_TerminateTask function.
    It terminates a task.

    @trace #BRCM_SWARCH_BCM_OS_SETEVENT_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = TerminateTask();
    if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == retVal){
        ret = BCM_ERR_BUSY;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_TerminateTask()
{
    int32_t ret;
    StatusType retVal;

    retVal = TerminateTask();

    if (E_OK == retVal) {
        /* Retained for consistency, but, will never get executed and not
         * possible to get code coverage */
        ret = BCM_ERR_OK;
    } else if (E_OS_CALLEVEL == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == retVal){
        ret = BCM_ERR_BUSY;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

/**
    Detailed design of BCM_ThreadNotTerminated

    @trace #BRCM_SWARCH_BCM_OS_TERMINATETASK_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    EE_thread_not_terminated
    @endcode
*/
void BCM_ThreadNotTerminated(void)
{
    EE_thread_not_terminated();
}

/**
    Detailed design of BCM_StartOS

    @trace #BRCM_SWARCH_BCM_OS_STRTOS_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    statusVal = StartOS((AppModeType)Mode);
    if (E_OK == statusVal) {
        ret = BCM_ERR_OK;
    } else if (E_OS_STATE == statusVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_SYS_INIT == statusVal) {
        ret = BCM_ERR_UNINIT;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
    @endcode
*/
int32_t BCM_StartOS(BCM_AppModeType aMode)
{
    int32_t ret;
    StatusType statusVal;

    statusVal = StartOS((AppModeType)aMode);
    if (E_OK == statusVal) {
        ret = BCM_ERR_OK;
    } else if (E_OS_STATE == statusVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_SYS_INIT == statusVal) {
        ret = BCM_ERR_UNINIT;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

/**
    Detailed design of BCM_SetRelAlarm
    This function will act as a wrapper over SetRelAlarm function.
    It sets a relative value for a alarm.

    @trace #BRCM_SWARCH_BCM_OS_SETRELALARM_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = SetRelAlarm((AlarmType)aAlarmID, (TickType)aIncrement,
                        (TickType)aCycle);

    if ((E_OS_ID == retVal) || (E_OS_VALUE == retVal)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }
    return ret
    @endcode
*/
int32_t BCM_SetRelAlarm(BCM_AlarmType aAlarmID,
                        BCM_TickType aIncrement,
                        BCM_TickType aCycle)
{
    int32_t ret;
    StatusType retVal;

    retVal = SetRelAlarm((AlarmType)aAlarmID, (TickType)aIncrement,
                        (TickType)aCycle);

    if ((E_OS_ID == retVal) || (E_OS_VALUE == retVal)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/**
    Detailed design of BCM_CancelAlarm
    This function will act as a wrapper over CancelAlarm function.
    This cancels an active alarm.

    @trace #BRCM_SWARCH_BCM_OS_CANCELALARM_PROC
    @trace #BRCM_SWREQ_BCM_OS_WRAPPER

    @code{.c}
    retVal = CancelAlarm(aAlarmID);

    if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret
    @endcode
*/
int32_t BCM_CancelAlarm(BCM_AlarmType aAlarmID)
{
    int32_t ret;
    StatusType retVal;

    retVal = CancelAlarm((AlarmType)aAlarmID);

    if (E_OS_ID == retVal) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == retVal) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != retVal) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/** @} */
