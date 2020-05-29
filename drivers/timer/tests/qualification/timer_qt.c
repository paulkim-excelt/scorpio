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
#include "ee.h"
#include <gpt.h>
#include <utils.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <tim_rdb.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define TIMER_IT1               (1UL)
#define TIMER_IT2               (2UL)
#define TIMER_IT3               (3UL)

#define TIMER_CH_ID_0           (0UL)
#define TIMER_CH_ID_1           (1UL)
#define MAX_PERIODIC_TEST_CNT   (5UL)
#define MIN_SYSTEM_TEST_CNT     (2UL)
#define IT1_FREE_RUN_CH_ID      (TIMER_CH_ID_0)
#define IT1_PERIODIC_CH_ID      (TIMER_CH_ID_1)
#define IT2_PERIODIC_CH_ID      (TIMER_CH_ID_0)
#define IT2_ONE_SHOT_CH_ID      (TIMER_CH_ID_1)
#define IT3_SYS_TIMER_CH_ID     (TIMER_CH_ID_0)

/* Timer Ticks for 10ms at prescale of 16 is (50M/16)/100 */
#define TIMER_TICKS_DIV_16_10MS (31250UL)
/* Timer Ticks for 1ms at prescale of 16 is (50M/16)/1000 */
#define TIMER_TICKS_DIV_16_1MS  (3125UL)
/* Timer Ticks for 1ms at prescale of  is (50M/100 */
#define TIMER_TICKS_DIV_1_1MS   (50000UL)

static int32_t TIMERIT_Status = BCM_AT_NOT_STARTED;
static uint32_t timerTestIdx = 0UL;
static uint32_t periodicCbCount = 0UL;
static uint32_t oneShotCbCount = 0UL;
static uint32_t sysTimerCbCount = 0UL;

void TIMTest_OneShotCb(void);
void TIMTest_PeriodicCb(void);
void TIMTest_SystemTimerCb(void);

static const TIM_ConfigType TIM_Config1[] = {
    {
        .prescale = TIM_PRESCALE_DIV_16,
        .sysTimeEn = FALSE,
        .chanID = IT1_FREE_RUN_CH_ID,
        .chanMode = TIM_CHAN_MODE_FREE_RUNNING,
        .width = TIM_WIDTH_32_BIT,
        .cb = NULL,
    },
    {
        .prescale = TIM_PRESCALE_DIV_16,
        .sysTimeEn = FALSE,
        .chanID = IT1_PERIODIC_CH_ID,
        .chanMode = TIM_CHAN_MODE_PERIODIC,
        .width = TIM_WIDTH_32_BIT,
        .cb = TIMTest_PeriodicCb,
    },
};
static const TIM_ConfigType TIM_Config2[] = {
    {
        .prescale = TIM_PRESCALE_DIV_16,
        .sysTimeEn = FALSE,
        .chanID = IT2_PERIODIC_CH_ID,
        .chanMode = TIM_CHAN_MODE_PERIODIC,
        .width = TIM_WIDTH_32_BIT,
        .cb = TIMTest_PeriodicCb,
    },
    {
        .prescale = TIM_PRESCALE_DIV_16,
        .sysTimeEn = FALSE,
        .chanID = IT2_ONE_SHOT_CH_ID,
        .chanMode = TIM_CHAN_MODE_ONE_SHOT,
        .width = TIM_WIDTH_16_BIT,
        .cb = TIMTest_OneShotCb,
    },
};
static const TIM_ConfigType SysTimer = {
    .prescale = TIM_PRESCALE_DIV_1,
    .sysTimeEn = TRUE,
    .chanID = IT3_SYS_TIMER_CH_ID,
    .chanMode = TIM_CHAN_MODE_PERIODIC,
    .width = TIM_WIDTH_32_BIT,
    .cb = TIMTest_SystemTimerCb,
};

static uint32_t TIM_ConfigTbl1Size = (sizeof(TIM_Config1) / sizeof(TIM_ConfigType));
static uint32_t TIM_ConfigTbl2Size = (sizeof(TIM_Config2) / sizeof(TIM_ConfigType));

void TIMTest_SystemTimerCb(void)
{
    StatusType retErika = E_OK;
    sysTimerCbCount++;
    retErika = SetEvent(TIMERIT_Task, SytemTmrEvent);
    if (E_OK != retErika) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
    }
}

void TIMTest_PeriodicCb(void)
{
    StatusType retErika = E_OK;
    periodicCbCount++;
    retErika = SetEvent(TIMERIT_Task, PeriodicTmrEvent);
    if (E_OK != retErika) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
    }
}

void TIMTest_OneShotCb(void)
{
    StatusType retErika = E_OK;
    oneShotCbCount++;
    retErika = SetEvent(TIMERIT_Task, OneShotTmrEvent);
    if (E_OK != retErika) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
    }
}

static void TIMER_TestIT3(void)
{
    uint32_t value = SYS_TICK_US *100UL;
    ULOG_INFO("value:%d\n", value);
    uint64_t systemTimeStamp1;
    uint64_t systemTimeStamp2;

    /* Initialize timer */
    TIM_Init(SysTimer.chanID, &SysTimer);
    TIM_EnableNotification(SysTimer.chanID);
    TIM_StartTimer(SysTimer.chanID, value);
    (void)BCM_WaitEvent(SytemTmrEvent);
    (void)BCM_ClearEvent(SytemTmrEvent);
    systemTimeStamp1 = TIM_GetTime();
    if (0UL != systemTimeStamp1) {
        (void)BCM_WaitEvent(SytemTmrEvent);
        (void)BCM_ClearEvent(SytemTmrEvent);
        systemTimeStamp2 = TIM_GetTime();
        if (0UL == systemTimeStamp2) {
           TIMERIT_Status = BCM_ERR_UNKNOWN;
        }
        if (systemTimeStamp2 <= systemTimeStamp1) {
           TIMERIT_Status = BCM_ERR_UNKNOWN;
        }
        if (MIN_SYSTEM_TEST_CNT > sysTimerCbCount) {
           TIMERIT_Status = BCM_ERR_UNKNOWN;
        }
    } else {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
    }
    TIM_StopTimer(SysTimer.chanID);
    TIM_DeInit(SysTimer.chanID);
    sysTimerCbCount = 0UL;
    if (BCM_AT_EXECUTING == TIMERIT_Status) {
        TIMERIT_Status = BCM_ERR_OK;
    }
}

static void TIMER_TestIT2(void)
{
    uint32_t i;
    uint32_t periodicCount = 0UL;

    /* Initialize timers */
    for (i = 0UL; i < TIM_ConfigTbl2Size; i++) {
        TIM_Init(TIM_Config2[i].chanID, &TIM_Config2[i]);
    }
    /* Start One shot timer for 1ms interval*/
    TIM_EnableNotification(IT2_ONE_SHOT_CH_ID);
    TIM_StartTimer(IT2_ONE_SHOT_CH_ID, TIMER_TICKS_DIV_16_1MS);
    /* Start Periodic timer for 10ms interval*/
    TIM_EnableNotification(IT2_PERIODIC_CH_ID);
    TIM_StartTimer(IT2_PERIODIC_CH_ID, TIMER_TICKS_DIV_16_10MS);

    (void)BCM_WaitEvent(OneShotTmrEvent);
    (void)BCM_ClearEvent(OneShotTmrEvent);
    while (MAX_PERIODIC_TEST_CNT > periodicCount) {
        (void)BCM_WaitEvent(PeriodicTmrEvent);
        (void)BCM_ClearEvent(PeriodicTmrEvent);
        periodicCount++;
    }

    /* Verify if call back counts are proper */
    if (1UL != oneShotCbCount ||
        MAX_PERIODIC_TEST_CNT > periodicCbCount) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
        ULOG_ERR("Timer call back count error, OneShot:%d  Periodic:%d \n", oneShotCbCount, periodicCbCount);
    }

    /* Periodic timer has to be stopped before de-initializing */
    TIM_StopTimer(IT2_PERIODIC_CH_ID);
    /* De-Initialize timers */
    for (i = 0UL; i < TIM_ConfigTbl2Size; i++) {
        TIM_DeInit(TIM_Config2[i].chanID);
    }

    periodicCbCount = 0UL;
    oneShotCbCount = 0UL;
    if (BCM_AT_EXECUTING == TIMERIT_Status) {
        TIMERIT_Status = BCM_ERR_OK;
    }
}

static void TIMER_TestIT1(void)
{
    uint32_t i;
    TIM_TickType freeTimeStamp1;
    TIM_TickType freeTimeStamp2;
    TIM_TickType periodicTimeStamp[MAX_PERIODIC_TEST_CNT];
    uint32_t periodicCount = 0UL;

    /* Initialize timers */
    for (i = 0UL; i < TIM_ConfigTbl1Size; i++) {
        TIM_Init(TIM_Config1[i].chanID, &TIM_Config1[i]);
    }
    TIM_EnableNotification(IT1_PERIODIC_CH_ID);
    /* Get free running times */
    freeTimeStamp1 = TIM_GetFreeRunningTime(IT1_FREE_RUN_CH_ID);
    ULOG_INFO("Free Running Timestamps before: %d\n", freeTimeStamp1);

    /* Start Periodic timer */
    TIM_StartTimer(IT1_PERIODIC_CH_ID, TIMER_TICKS_DIV_16_10MS);

    while (MAX_PERIODIC_TEST_CNT > periodicCount) {
        (void)BCM_WaitEvent(PeriodicTmrEvent);
        (void)BCM_ClearEvent(PeriodicTmrEvent);
        periodicTimeStamp[periodicCount] = TIM_GetFreeRunningTime(IT1_FREE_RUN_CH_ID);
        periodicCount++;
    }
    /* Periodic timer has to be stopped before de-initializing */
    TIM_StopTimer(IT1_PERIODIC_CH_ID);

    ULOG_INFO("Periodic Time Stamps %d, %d, %d, %d, %d\n", periodicTimeStamp[0],
                                                           periodicTimeStamp[1],
                                                           periodicTimeStamp[2],
                                                           periodicTimeStamp[3],
                                                           periodicTimeStamp[4]);

    /* Verify free running timer again */
    freeTimeStamp2 = TIM_GetFreeRunningTime(IT1_FREE_RUN_CH_ID);
    ULOG_INFO("Free Running Timestamps after: %d\n", freeTimeStamp2);
    if (freeTimeStamp2 <= freeTimeStamp1) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
        ULOG_ERR("Freerunning timer not running after periodic !!\n");
        goto bcm_err;
    }
    /* Verify if Periodic call back count is proper */
    if (MAX_PERIODIC_TEST_CNT > periodicCbCount) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
        ULOG_ERR("Callback count error, cb count %d\n", periodicCbCount);
        goto bcm_err;
    }
    /* Verify if free running counts are proper */
    if ((freeTimeStamp2 - freeTimeStamp1) < (TIMER_TICKS_DIV_16_10MS * MAX_PERIODIC_TEST_CNT)) {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
        ULOG_ERR("Timer tick mismatch. Before:%d after:%d \n", freeTimeStamp1, freeTimeStamp2);
        goto bcm_err;
    }

    if (BCM_AT_EXECUTING == TIMERIT_Status) {
        TIMERIT_Status = BCM_ERR_OK;
    }

bcm_err:
    periodicCbCount = 0UL;
    for (i = 0UL; i < TIM_ConfigTbl1Size; i++) {
        TIM_DeInit(i);
    }
}

TASK(TIMERIT_Task)
{
    if (TIMER_IT1 == timerTestIdx) {
        TIMER_TestIT1();
    } else if (TIMER_IT2 == timerTestIdx) {
        TIMER_TestIT2();
    } else if (TIMER_IT3 == timerTestIdx) {
        TIMER_TestIT3();
    } else {
        TIMERIT_Status = BCM_ERR_UNKNOWN;
    }

    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType retErika = E_OK;
    TaskStateType taskState = SUSPENDED;
    TaskStateRefType taskStatePtr;

    taskStatePtr = &taskState;

    switch (aIndex) {
    case TIMER_IT1:
    case TIMER_IT2:
    case TIMER_IT3:
        retErika = GetTaskState(TIMERIT_Task, taskStatePtr);
        if (BCM_ERR_OK == retErika) {
            if (SUSPENDED == taskState) {
                TIMERIT_Status = BCM_AT_EXECUTING;
                timerTestIdx = aIndex;
                retVal = BCM_ActivateTask(TIMERIT_Task);
                if (BCM_ERR_OK != retVal) {
                    retVal = BCM_AT_NOT_STARTED;
                    TIMERIT_Status = BCM_AT_NOT_STARTED;
                    ULOG_ERR("TIMERIT_Task activation failed, error: %d\n\r", retVal);
                }
            } else {
                retVal = BCM_AT_EXECUTING;
                ULOG_ERR("TIMERIT_Task is already running \n\r");
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("GetTaskState TIMERIT_Task failed Error: %d\n\r", retErika);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch (aIndex) {
    case TIMER_IT1:
    case TIMER_IT2:
    case TIMER_IT3:
        if (aIndex == timerTestIdx) {
            retVal = TIMERIT_Status;
        } else {
            retVal = BCM_AT_NOT_AVAILABLE;
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return retVal;
}

void ApplicationInit(void)
{
}
