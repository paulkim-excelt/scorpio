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

#include <osil/bcm_osil.h>
#include <osil/bcm_osil_svc.h>
#include <svc.h>
#include "ee.h"
#include "kernel/oo/inc/ee_oo_internal.h"

/**
    @name BCM_OsCmdType
    @{
    @brief BCM_OS SVC Command
    @trace #BRCM_SWARCH_BCM_IL_CMD_TYPE    #BRCM_REQ_BCM_IL_CMD
*/
typedef uint32_t BCM_OsCmdType;                      /**< @brief BCM_OS SVC Command */
#define BCM_CMD_ACTIVATETASK        (0x1UL)      /**< @brief BCM_ActivateTask */
#define BCM_CMD_TERMINATETASK       (0x2UL)      /**< @brief BCM_TerminateTask */
#define BCM_CMD_THREADNOTTERMINATED (0x3UL)      /**< @brief BCM_ThreadNotTerminated */
#define BCM_CMD_STARTOS             (0x4UL)      /**< @brief BCM_StartOS */
#define BCM_CMD_SETEVENT            (0x5UL)      /**< @brief BCM_SetEvent */
#define BCM_CMD_CLEAREVENT          (0x6UL)      /**< @brief BCM_ClearEvent */
#define BCM_CMD_WAITEVENT           (0x7UL)      /**< @brief BCM_WaitEvent */
#define BCM_CMD_SETRELALARM         (0x8UL)      /**< @brief BCM_SetRelAlarm */
#define BCM_CMD_CANCELALARM         (0x9UL)      /**< @brief BCM_CancelAlarm */
/** @} */


/**
    @brief  Structure to define input and output parameters

    @trace #BRCM_SWARCH_BCM_IL_BCM_OsIOType_TYPE #BRCM_REQ_BCM_IL_BCM_OsIOType
*/
typedef struct _BCM_OsIOType {
    StatusType status;                            /**< @brief status of system call */
    BCM_TaskType taskId;
    BCM_AppModeType appMode;
    BCM_EventMaskType eventMask;
    BCM_AlarmType alarmID;
    BCM_TickType alarmInc;
    BCM_TickType alarmCycle;
} BCM_OsIOType;


/** @brief BCM OSIL Error Log

    This API is for logging errors

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      aApiID        API ID
    @param[in]      aErr          Error Code
    @param[in]      aVal0         Value0
    @param[in]      aVal1         Value1
    @param[in]      aVal2         Value2
    @param[in]      aVal3         Value3

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @limitations none

    @code{.unparsed}
    values = {aVal0, aVal1, aVal2, aVal3}
    BCM_ReportError((BCM_OSI_ID & BCM_LOGMASK_USER), 0U, aApiID, aErr, 4UL, values)
    @endcode
*/
static void BCM_OsReportError(uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_OSI_ID & BCM_LOGMASK_USER), 0U, aApiID, aErr, 4UL, values);
}

/**
    @code{.c}

    Build SVC Request
    ret = SVC_Request(&sysReqIO)
    if (ret is BCM_ERR_OK)
        ret = sysReqIO.response
    else
        Log error

    return ret
    @endcode
*/
int32_t BCM_SysCmdReq(const uint32_t aCmd, BCM_OsIOType *aIO)
{
    int32_t ret;
    SVC_RequestType sysReqIO;

    sysReqIO.response = BCM_ERR_UNKNOWN;

    /* Build SVC command struct */
    sysReqIO.sysReqID = SVC_OSI_ID;
    sysReqIO.magicID = SVC_MAGIC_OSI_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t*)aIO;
    /* Send SVC command */
    ret = SVC_Request(&sysReqIO);
    if (BCM_ERR_OK == ret) {
        ret = sysReqIO.response;
    }
    return ret;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _BCM_SVCOsIOTypee {
    uint8_t *data;
    BCM_OsIOType *io;
} BCM_SVCOsIOTypee;


/* SVC Command Handler */
int32_t BCM_CmdHandler(uint32_t aCmd, BCM_OsIOType *aBCMOSIO)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aBCMOSIO) {
        aBCMOSIO->status = E_OS_VALUE;
        ret = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    switch(aCmd){
    case BCM_CMD_ACTIVATETASK:
        aBCMOSIO->status = ActivateTask((TaskType)aBCMOSIO->taskId);
        break;
    case BCM_CMD_TERMINATETASK:
        aBCMOSIO->status = TerminateTask();
        EE_oo_call_PreTaskHook();
        break;
    case BCM_CMD_THREADNOTTERMINATED:
        EE_thread_not_terminated();
        EE_hal_enableIRQ();
        EE_oo_call_PreTaskHook();
        aBCMOSIO->status = BCM_ERR_OK;
        break;
    case BCM_CMD_STARTOS:
        aBCMOSIO->status = StartOS((AppModeType)aBCMOSIO->appMode);
        break;
    case BCM_CMD_SETEVENT:
        aBCMOSIO->status = SetEvent((TaskType)aBCMOSIO->taskId,
                                    (EventMaskType)aBCMOSIO->eventMask);
        break;
    case BCM_CMD_CLEAREVENT:
        aBCMOSIO->status = ClearEvent((EventMaskType)aBCMOSIO->eventMask);
        break;
    case BCM_CMD_WAITEVENT:
        aBCMOSIO->status = WaitEvent((EventMaskType)aBCMOSIO->eventMask);
        break;
    case BCM_CMD_SETRELALARM:
        aBCMOSIO->status = SetRelAlarm((AlarmType)aBCMOSIO->alarmID,
                                    (TickType)aBCMOSIO->alarmInc,
                                    (TickType)aBCMOSIO->alarmCycle);
        break;
    case BCM_CMD_CANCELALARM:
        aBCMOSIO->status = CancelAlarm((AlarmType)aBCMOSIO->alarmID);
        break;
    default:
        aBCMOSIO->status = E_OS_VALUE;
        ret = BCM_ERR_INVAL_PARAMS;
        break;
    }

done:
    return ret;
}

/**
    @code{.c}
    if aSysIO.bcmOsIO is not NULL
        if aMagicID is SVC_MAGIC_OSI_ID
            ret = BCM_CmdHandler(aCmd, aSysIO.bcmOsIO)
            if ret is not BCM_ERR_OK
                Log error
        else
            Log error
    else
        Log error

    @endcode
*/
void BCM_OsSysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    int32_t ret;
    BCM_SVCOsIOTypee bcmSVCOsIO;
    bcmSVCOsIO.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_OSI_ID == aMagicID){
            ret = BCM_CmdHandler(aCmd, bcmSVCOsIO.io);
            if (BCM_ERR_OK != ret) {
                BCM_OsReportError(BRCM_SWARCH_BCM_OSIL_SYSCMDHDLR_PROC, ret, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
            }
        } else {
            BCM_OsReportError(BRCM_SWARCH_BCM_OSIL_SYSCMDHDLR_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, aCmd, aMagicID, (uint32_t)aSysIO);
        }
    } else {
        BCM_OsReportError(BRCM_SWARCH_BCM_OSIL_SYSCMDHDLR_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
    }
}

int32_t BCM_ActivateTask(BCM_TaskType aTaskID)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .taskId = aTaskID,
    };

    ret = BCM_SysCmdReq(BCM_CMD_ACTIVATETASK, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_ACTIVATETASK_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, (uint32_t)aTaskID, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OS_ID == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_LIMIT == bcmOsIO.status) {
        ret = BCM_ERR_MAX_ATTEMPS;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;

}

int32_t BCM_TerminateTask()
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
    };

    ret = BCM_SysCmdReq(BCM_CMD_TERMINATETASK, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_TERMINATETASK_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, 0UL, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OK == bcmOsIO.status) {
        ret = BCM_ERR_OK;
    } else if (E_OS_CALLEVEL == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == bcmOsIO.status){
        ret = BCM_ERR_BUSY;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

void BCM_ThreadNotTerminated(void)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
    };

    ret = BCM_SysCmdReq(BCM_CMD_THREADNOTTERMINATED, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_THRNOTTERMINAT_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, 0UL, 0UL);
    }

    return;
}

int32_t BCM_StartOS(BCM_AppModeType aMode)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .appMode= aMode,
    };

    ret = BCM_SysCmdReq(BCM_CMD_STARTOS, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_STRTOS_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, (uint32_t)aMode, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OK == bcmOsIO.status) {
        ret = BCM_ERR_OK;
    } else if (E_OS_STATE == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_SYS_INIT == bcmOsIO.status) {
        ret = BCM_ERR_UNINIT;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;

}

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

int32_t BCM_SetEvent(BCM_TaskType aTaskID, BCM_EventMaskType aEventMask)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .taskId = aTaskID,
        .eventMask = aEventMask,
    };

    ret = BCM_SysCmdReq(BCM_CMD_SETEVENT, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_SETEVENT_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, (uint32_t)aTaskID, (uint32_t)aEventMask);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OS_STATE == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == bcmOsIO.status) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OS_ID == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;

}

int32_t BCM_ClearEvent(BCM_EventMaskType aEventMask)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .eventMask = aEventMask,
    };

    ret = BCM_SysCmdReq(BCM_CMD_CLEAREVENT, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_CLEAREVENT_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, (uint32_t)aEventMask, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OS_CALLEVEL == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_ACCESS == bcmOsIO.status) {
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;

}

int32_t BCM_WaitEvent(BCM_EventMaskType aEventMask)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .eventMask = aEventMask,
    };

    ret = BCM_SysCmdReq(BCM_CMD_WAITEVENT, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_WAITEVENT_PROC, ret, (uint32_t)__LINE__, (uint32_t)bcmOsIO.status, (uint32_t)aEventMask, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OS_CALLEVEL == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OS_RESOURCE == bcmOsIO.status){
        ret = BCM_ERR_BUSY;
    } else if (E_OS_ACCESS == bcmOsIO.status){
        ret = BCM_ERR_NOPERM;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else{
        ret = BCM_ERR_OK;
    }

    return ret;
}

int32_t BCM_SetRelAlarm(BCM_AlarmType aAlarmID,
                        BCM_TickType aIncrement,
                        BCM_TickType aCycle)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .alarmID = aAlarmID,
        .alarmInc = aIncrement,
        .alarmCycle = aCycle,
    };

    ret = BCM_SysCmdReq(BCM_CMD_SETRELALARM, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_SETRELALARM_PROC, ret,
                (uint32_t)__LINE__, (uint32_t)bcmOsIO.status,
                (uint32_t)aAlarmID, (uint32_t)aIncrement);
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_SETRELALARM_PROC, ret,
                (uint32_t)__LINE__, (uint32_t)aCycle, 0UL, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if ((E_OS_ID == bcmOsIO.status) || (E_OS_VALUE == bcmOsIO.status)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

int32_t BCM_CancelAlarm(BCM_AlarmType aAlarmID)
{
    int ret;
    BCM_OsIOType bcmOsIO = {
        .status = E_OS_NOFUNC,
        .alarmID = aAlarmID,
    };

    ret = BCM_SysCmdReq(BCM_CMD_CANCELALARM, &bcmOsIO);
    if (BCM_ERR_OK != ret) {
        BCM_OsReportError(BRCM_SWARCH_BCM_OS_SETRELALARM_PROC, ret,
                (uint32_t)__LINE__, (uint32_t)bcmOsIO.status,
                (uint32_t)aAlarmID, 0UL);
        bcmOsIO.status = E_OS_NOFUNC;
    }

    if (E_OS_ID == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if(E_OS_STATE == bcmOsIO.status) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (E_OK != bcmOsIO.status) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}
