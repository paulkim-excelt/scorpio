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

#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <system.h>
#include <ulog.h>
#include <console.h>
#include <build_info.h>
#include <bcm_utils.h>
#include <board.h>
#include <init.h>
#include <idle.h>
#ifdef ENABLE_IPC
#include <sys_ipc.h>
#include <rpc_async.h>
#include <ipc.h>
#include <osil/sys_ipc_osil.h>
#include <imgl_ipc_cmds.h>
#endif
#include <gpt.h>
#include <utils.h>
#include <chip_config.h>
#include <osil/bcm_osil.h>
#include <nvm_pt.h>
#include <imgl.h>
#include <rpc_cmd_queue.h>
#include <mcu_ext.h>
#include <dmu_rdb.h>
#if defined(ENABLE_OTP)
#include <otp.h>
#endif
#include <bcm_time.h>

#include "ee.h"
#include "ee_internal.h"

/**
    @name System Design IDs
    @{
    @brief System Design IDs
*/
#define BRCM_SWDSGN_SYS_PROCESSEVENTS_PROC          (0x90)        /**< @brief #SYS_ProcessEvents */
#define BRCM_SWDSGN_SYSNOTIFYEVENT_PROC             (0x91)        /**< @brief #RPC_AsyncEvent    */
/** @} */


#define GetModuleLogLevel()         (ULOG_LVL_INFO)

extern void ApplicationInit(void);

/* system monitor event */
#define SYSTEM_MONITOR_TASK         (BCM8956X_SystemTask)
#define SYSTEM_MONITOR_EVENTS       (SYSTEM_MONITOR_ALARM_EVENT)

#define NS_TO_MS(ns)                (ns / 1000000UL)

extern uint32_t EarlyInitTime;
uint32_t SYS_CommsReadyTime;
uint32_t SYS_SlaveBootTime;
uint32_t SYS_BootTime;

/* Variable to track the number of alarms raised */
static uint32_t SYS_AlarmCnt;

/**
    @code{.unparsed}
    @endcode
*/
static void BCM8956X_ProcessMonitorEvent()
{
    int32_t    retVal = BCM_ERR_OK;

#ifdef ENABLE_GPIO_GIO_V1
            (void)GPIO_FlipChannel(SYSTEM_MONITOR_GPIO);
#endif

#ifdef ENABLE_IPC
    uint8_t buf[RPC_ASYNCPAYLOADSZ];
    SYS_HandleType notification;
    notification.u8Ptr = buf;

    notification.keepAlive->count = SYS_AlarmCnt;

    retVal = RPC_AsyncEvent(BCM_GROUPID_SYS,
                             BCM_RSV_ID,
                             SYS_ASYNCID_KEEPALIVE,
                             notification.u8Ptr ,
                             sizeof(SYS_KeepAliveType));
    (void)retVal;
#endif

}


#ifdef ENABLE_SYSTEM_MONITOR
void SYS_NotfnAlarmCb(void)
{
    SYS_AlarmCnt++;
    (void)SetEvent(SYSTEM_MONITOR_TASK, SYSTEM_MONITOR_ALARM_EVENT);
}
#endif

void SYS_REBOOT_AlarmCb(void)
{
    (void)SetEvent(BCM8956X_SystemTask, SYS_REBOOT_EVENT);
}

SYS_UpdateExecCmdType BCM8956X_ExecCmd;

#if defined(ENABLE_TIMER_SP804) && !defined(ENABLE_TIMER_TEST)
static void SysTimer_PeriodicCb(void)
{
    int ret;
#if !defined(__OO_NO_ALARMS__) && defined(SystemTimer)
    ret = IncrementCounterHardware(SystemTimer);
    if (E_OK != ret) {
    }
#endif
}

#if defined(ENABLE_HRTIMER)
static void BCM8956X_HRTimerCb(void)
{
    int ret;
#if !defined(__OO_NO_ALARMS__) && defined(HRTimer)
    ret = IncrementCounterHardware(HRTimer);
    if (E_OK != ret) {
    }
#endif
}
#endif

const TIM_ConfigType TimerCfg[] = {
    {
        .prescale = TIM_PRESCALE_DIV_1,
        .chanID = 0UL,
        .chanMode = TIM_CHAN_MODE_PERIODIC,
        .width = TIM_WIDTH_32_BIT,
        .cb = SysTimer_PeriodicCb,
        .sysTimeEn = TRUE,
    },
#if defined(ENABLE_HRTIMER)
    {
        .prescale = TIM_PRESCALE_DIV_1,
        .chanID = 1UL,
        .chanMode = TIM_CHAN_MODE_PERIODIC,
        .width = TIM_WIDTH_32_BIT,
        .cb = BCM8956X_HRTimerCb,
        .sysTimeEn = FALSE,
    },
#endif
};
#endif

TASK(BCM8956X_SystemTask)
{
    BCM_EventMaskType mask;
    uint32_t alarmTicks = 0UL;
    SYS_RebootCmdType reboot;
    int32_t ret = BCM_ERR_OK;
    char consoleBuf[128UL];
    uint8_t blBootTime;
    uint8_t blXcvrInitTime;
    uint32_t entryTime;
    MCU_BootTimeInfoType bootTime;
#ifdef __BCM89564G__
    SYS_UpdateExecCmdType *flsInfo = (SYS_UpdateExecCmdType *)&BCM8956X_ExecCmd;
    MCU_ExtendedInfoType stackingInfo;
    uint32_t i,j,k;
    uint8_t pids[2UL] = {PTBL_ID_SYSCFG,PTBL_ID_FW};
    const MSGQ_MsgHdrType* hdr;
    int32_t retVal;
    const ITBL_Type *tbl = NULL;
    uint64_t SYS_SlaveBootStartTime;
    uint64_t SYS_SlaveBootEndTime;
#endif

#if defined(ENABLE_WATCHDOG_SP805)
    /* Initialize watchdog */
    WDT_Init(0UL, &WDT_Config[0UL]);
#endif

    MCU_GetBootTimeInfo(&bootTime);
    blBootTime = bootTime.blBootTime;
    blXcvrInitTime = bootTime.xcvrInitTime;
    entryTime = blBootTime + EarlyInitTime;
#ifdef ENABLE_WFI
    (void)IDLE_Init();
#endif

#if defined(ENABLE_GPIO_GIO_V1)
    PINMUX_Init();
#endif


#if defined(ENABLE_TIMER_SP804) && !defined(ENABLE_TIMER_TEST)
    TIM_Init(TimerCfg[0UL].chanID, &TimerCfg[0UL]);
    TIM_EnableNotification(TimerCfg[0UL].chanID);
    TIM_StartTimer(TimerCfg[0UL].chanID, (((SYS_TICK_US * 1000UL) * (uint64_t)TIMER_CLOCK)/1000000000UL));
#endif

#if defined(ENABLE_HRTIMER)
    TIM_Init(TimerCfg[1UL].chanID, &TimerCfg[1UL]);
#endif
    Board_Init();

#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
    OTP_Init(OTP_HW_ID_0);
#endif

    (void)BCM_ActivateTask(TaskShell);

#if defined(ENABLE_FLSMGR)
    /* This will internally initialize the flash, flash manager and PTM and
    wait for Acknowledgement from FLSMGR to be ready. Currently, not checking for
    error state. To be added in Future */
    FLSMGR_ServerInit();
    do {
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    } while (SYS_SUBSYSTEM_STATE_UNINIT == SYS_GetState(SYS_SUBSYSTEM_NVM));
#endif

    (void)BCM_ActivateTask(CommsTask);
    do {
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    } while (SYS_SUBSYSTEM_STATE_UNINIT == SYS_GetState(SYS_SUBSYSTEM_COMMS));

    SYS_CommsReadyTime = NS_TO_MS(BCM_GetTimeNs()) + entryTime;
    (void)BCM_ActivateTask(TaskSerialIO);

#ifdef __BCM89564G__
    SYS_SlaveBootStartTime = BCM_GetTimeNs();
    ret = MCU_GetExtendedInfo(&stackingInfo);

    if ((BCM_ERR_OK == ret) && (TRUE == stackingInfo.stackingEn) &&
            (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {
        flsInfo->flashId = 0UL;
        flsInfo->numImgs = 0UL;
        k = 0UL;
        for (j = 0UL; j < 2UL; j++) {
            tbl = IMGL_GetImgTbl(pids[j]);
            if (NULL != tbl) {
                flsInfo->numImgs++;
                flsInfo->imgHdr[k].isImgHdr = 1;
                flsInfo->imgHdr[k].pid = pids[j];
                flsInfo->imgHdr[k].imgID = 0UL;
                flsInfo->imgHdr[k].imgSize = sizeof(ITBL_Type);
                k++;
                for (i = 0UL; i < tbl->hdr.numImgs; i++) {
                    if (tbl->entry[i].maxSize > 0UL) {
                        flsInfo->imgHdr[k].isImgHdr = 0;
                        flsInfo->imgHdr[k].pid = pids[j];
                        flsInfo->imgHdr[k].imgID = tbl->entry[i].imgType;
                        flsInfo->imgHdr[k].imgSize = tbl->entry[i].actualSize;
                        k++;
                        flsInfo->numImgs++;
                    }
                }
            }
        }

        retVal = RPC_CmdQSendCmd(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_EXECUTE,
                (uint8_t *) flsInfo, sizeof(SYS_UpdateExecCmdType), SYS_NOTIF_EVENT, &hdr);

        do {
            (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
            (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
            ret = RPC_CmdQGetCmdStatus(hdr, &retVal, NULL, NULL);
        } while (BCM_ERR_BUSY == ret);
    }
    SYS_SlaveBootEndTime = BCM_GetTimeNs();
    SYS_SlaveBootTime = NS_TO_MS(SYS_SlaveBootEndTime - SYS_SlaveBootStartTime);
#endif

    ApplicationInit();

    ret = BCM_ERR_OK;

    RPC_AsyncResponse(BCM_GROUPID_SYS,
                         BCM_RSV_ID,
                         SYS_CMDID_EXECUTE,
                         (uint8_t*)&ret, sizeof(ret));

    SYS_BootTime = NS_TO_MS(BCM_GetTimeNs()) + entryTime;
    CONSOLE_PrintToBuffer(consoleBuf, sizeof(consoleBuf),
            "BootTime(ms): BL:%d XcvrInit:%d\n", blBootTime, blXcvrInitTime);
    CONSOLE_Write(CONSOLE_UART, consoleBuf);

    CONSOLE_PrintToBuffer(consoleBuf, sizeof(consoleBuf),
            "BootTime(ms): CommsReady:%d SysReady:%d\n",
            SYS_CommsReadyTime, SYS_BootTime);
    CONSOLE_Write(CONSOLE_UART, consoleBuf);

    while(1UL) {
        /* All System Processing, Error and Safety handling here */
        (void)BCM_WaitEvent(SystemEvent0 |
                            SYS_REBOOT_EVENT |
                            IPC_SYS_MSGQ_EVENT |
                            SYSTEM_MONITOR_ALARM_EVENT);
        (void)BCM_GetEvent(BCM8956X_SystemTask, &mask);
        (void)BCM_ClearEvent(mask);

        if (mask & SYSTEM_MONITOR_ALARM_EVENT) {
            BCM8956X_ProcessMonitorEvent();
        }

        if (mask & IPC_SYS_MSGQ_EVENT) {
            SYS_ProcessCmds();
        }

        if (BCM_ERR_OK == SYS_GetRebootRequest(&reboot)) {
            if(0UL < reboot.delayMs){
                alarmTicks = ((reboot.delayMs * 1000UL)/ SYS_TICK_US) +
                              ((0UL == ((reboot.delayMs * 1000UL) % SYS_TICK_US))? 0UL : 1UL);
                /* Set an alarm for the delay specified by the HOST */
                BCM_SetRelAlarm(SYS_REBOOT_Alarm, alarmTicks, 0);
            } else {
                /* In case of zero or no delay, set the mask so that */
                /* target reboot is processed immediately            */
                break;
            }
        }

        if (mask & SYS_REBOOT_EVENT) {
            break;
        }

    }

#ifdef __BCM89564G__
    ret = MCU_GetExtendedInfo(&stackingInfo);

    if ((BCM_ERR_OK == ret) && (TRUE == stackingInfo.stackingEn) &&
            (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {
        retVal = RPC_CmdQSendCmd(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_REBOOT,
                (uint8_t *) &reboot, sizeof(SYS_RebootCmdType), SYS_NOTIF_EVENT, &hdr);

        do {
            (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
            (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
            ret = RPC_CmdQGetCmdStatus(hdr, &retVal, NULL, NULL);
        } while (BCM_ERR_BUSY == ret);
    }
#endif

    while (SYS_SUBSYSTEM_STATE_UNINIT != SYS_GetState(SYS_SUBSYSTEM_COMMS)) {
        (void)BCM_SetEvent(CommsTask, ShutdownEvent);
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    }
#if defined(ENABLE_FLSMGR)
    while (SYS_SUBSYSTEM_STATE_UNINIT != SYS_GetState(SYS_SUBSYSTEM_NVM)) {
        (void)BCM_SetEvent(FLSMGR_Server, ShutdownEvent);
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    }
#endif

    if (SYS_BOOTMODE_BL == reboot.bootMode) {
        MCU_SetResetMode(MCU_RESETMODE_DWNL);
        MCU_ResetReq(MCU_RESETREQ_LOCAL);
    } else if ((SYS_BOOTMODE_FW == reboot.bootMode)
                || (SYS_BOOTMODE_DEFAULT == reboot.bootMode)) {
        MCU_ResetReq(MCU_RESETREQ_GLOBAL);
    }

    (void)BCM_TerminateTask();
}

void BCM8956X_StartOS(void)
{
    /* Start OS and kick tasks */
    (void)BCM_StartOS(0UL);
}

int main()
{
    /* Do Nothing here*/
    while(1UL) {}
    return BCM_ERR_OK;
}
