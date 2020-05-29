/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
#include <ulog.h>
#include <bcm_time.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#define TASK_SWITCH_TEST_CNT        (5UL)
#define GetModuleLogLevel() ULOG_LVL_INFO

static uint32_t task0_cnt;
static uint32_t task1_cnt;

static int32_t TestStatus = BCM_AT_NOT_STARTED;

TASK(Task0)
{
    BCM_EventMaskType mask = 0UL;

    do {
        BCM_WaitEvent(TriggerEvent0);
        BCM_GetEvent(Task0, &mask);
        if (mask & TriggerEvent0) {
            BCM_ClearEvent(TriggerEvent0);
        }
        ULOG_INFO("Task0 run: %d\n\r", task0_cnt++);
        BCM_SetEvent(Task1, TriggerEvent1);
    } while(task0_cnt <= TASK_SWITCH_TEST_CNT);

    BCM_TerminateTask();
}

TASK(Task1)
{
    BCM_EventMaskType mask = 0UL;

    do {
        BCM_WaitEvent(TriggerEvent1);
        BCM_GetEvent(Task1, &mask);
        if (mask & TriggerEvent1) {
            BCM_ClearEvent(TriggerEvent1);
        }
        ULOG_INFO("Task1 run: %d\n\r", task1_cnt++);
    } while(task1_cnt <= TASK_SWITCH_TEST_CNT);

    if ((task0_cnt >= TASK_SWITCH_TEST_CNT) &&
            (task1_cnt >= TASK_SWITCH_TEST_CNT)) {
        TestStatus = BCM_ERR_OK;
    } else {
        TestStatus = BCM_ERR_UNKNOWN;
    }

    BCM_TerminateTask();
}

/* Start Test */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
        case 1UL:
            TestStatus = BCM_AT_EXECUTING;
            task0_cnt = 0UL;
            task1_cnt = 0UL;
            BCM_ActivateTask(Task0);
            BCM_ActivateTask(Task1);
            break;
        default:
            retVal = BCM_AT_NOT_AVAILABLE;
            break;
    }
    return retVal;
}

/* Get Test results */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
        case 1UL:
            retVal = TestStatus;
            break;
        default:
            retVal = BCM_AT_NOT_AVAILABLE;
            break;
    }
    return retVal;
}

void ApplicationInit()
{
    /* dummy */
}

void TaskSwitchCb()
{
    SetEvent(Task0, TriggerEvent0);
}
