/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
#include "ee.h"
#include <ulog.h>
#include <bcm_test.h>
#include <bcm_err.h>
#include <gpio.h>
#include <pinmux.h>
#include <testcfg.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_INFO
#define GPIO_IT1                (1UL)

static int32_t GPIOIT1_Status = BCM_AT_NOT_STARTED;
static uint32_t gpioTestIdx = 0UL;

/* Dummy Interrupt call back table */
const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS] = {
};

void ApplicationInit(void)
{
    /* Dummy Init */
}

TASK(GPIOIT1_Task)
{
    GPIO_LevelType oldGpioLvl, newGpioLvl;
    GPIO_PortLevelType oldPortLvl, newPortLvl;
    uint32_t grpMask;

    ULOG_ERR("GPIO_IT: START\n");

    oldGpioLvl = GPIO_ReadChannel(GPIO_TEST_CHANNEL0);
    if (GPIO_LEVEL_LOW != oldGpioLvl) {
        ULOG_ERR("\nChannel Level Error \t line %ld", __LINE__);
        goto err;
    }
    GPIO_WriteChannel(GPIO_TEST_CHANNEL0, GPIO_LEVEL_HIGH);
    oldGpioLvl = GPIO_ReadChannel(GPIO_TEST_CHANNEL0);
    if (GPIO_LEVEL_HIGH != oldGpioLvl) {
        ULOG_ERR("\nChannel Level Error \t line %ld", __LINE__);
        goto err;
    }

    GPIO_WriteChannel(GPIO_TEST_CHANNEL0, GPIO_LEVEL_LOW);
    oldGpioLvl = GPIO_ReadChannel(GPIO_TEST_CHANNEL0);
    if (GPIO_LEVEL_LOW != oldGpioLvl) {
        ULOG_ERR("\nChannel Level Error \t line %ld", __LINE__);
        goto err;
    }

    oldPortLvl = 2UL;
    GPIO_WriteChannelGroup(&GPIO_TestChannGrp, oldPortLvl);
    newPortLvl = GPIO_ReadChannelGroup(&GPIO_TestChannGrp);
    if (oldPortLvl != newPortLvl) {
        ULOG_ERR("\nChannel Group Error \t line %ld", __LINE__);
        goto err;
    }

    oldPortLvl = GPIO_ReadPort(GPIO_TEST_PORT0);
    grpMask = GPIO_TestChannGrp.mask;
    newPortLvl = ((~(oldPortLvl & grpMask)) & grpMask);
    GPIO_WritePort(GPIO_TEST_PORT0, newPortLvl);
    oldPortLvl = newPortLvl;
    newPortLvl = GPIO_ReadPort(GPIO_TEST_PORT0) & grpMask;
    if (oldPortLvl != newPortLvl) {
        ULOG_ERR("\nPort Level Error \t line %ld", __LINE__);
        goto err;
    }

    oldGpioLvl = GPIO_ReadChannel(GPIO_TEST_CHANNEL0);
    newGpioLvl = GPIO_FlipChannel(GPIO_TEST_CHANNEL0);
    if (oldGpioLvl == newGpioLvl) {
        ULOG_ERR("\nChannel Flip Error \t line %ld", __LINE__);
        goto err;
    }

    GPIOIT1_Status = BCM_ERR_OK;
err:
    if (BCM_AT_EXECUTING == GPIOIT1_Status) {
        GPIOIT1_Status = BCM_ERR_UNKNOWN;
    }
    ULOG_ERR("GPIO_IT: END\n");
    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType  retErika = E_OK;
    TaskStateType taskState = SUSPENDED;
    TaskStateRefType taskStatePtr;

    taskStatePtr = &taskState;

    switch (aIndex) {
    case GPIO_IT1:
        retErika = GetTaskState(GPIOIT1_Task, taskStatePtr);
        if (E_OK == retErika) {
            if (SUSPENDED == taskState) {
                GPIOIT1_Status = BCM_AT_EXECUTING;
                gpioTestIdx = aIndex;
                retVal = BCM_ActivateTask(GPIOIT1_Task);
                if (BCM_ERR_OK != retVal) {
                    retVal = BCM_AT_NOT_STARTED;
                    GPIOIT1_Status = BCM_AT_NOT_STARTED;
                    ULOG_ERR("GPIOIT1_Task activation failed, error: %d\n\r", retVal);
                }
            } else {
                retVal = BCM_AT_EXECUTING;
                ULOG_ERR("GPIOIT1_Task is already running \n\r");
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("GetTaskState GPIOIT1_Task failed Error: %d\n\r", retErika);
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
    case GPIO_IT1:
        if (aIndex == gpioTestIdx) {
            retVal = GPIOIT1_Status;
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
