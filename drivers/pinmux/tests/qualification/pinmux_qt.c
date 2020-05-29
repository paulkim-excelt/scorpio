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
#include <pinmux.h>
#include <gpio.h>
#include <testcfg.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_ERROR

#define PINMUX_IT1          (1UL)

#define PINMUXIT1_TESTPIN0  (PINMUX_TESTPIN0)
#define PINMUXIT1_TESTPIN1  (PINMUX_TESTPIN1)

static int32_t PinmuxIT1_Status = BCM_AT_NOT_STARTED;
static uint32_t pinmuxTestIdx = 0UL;

/* Dummy Interrupt call back table */
const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS] = {
};

void ApplicationInit(void)
{
    /* Dummy Init */
}

TASK(PinmuxIT1_Task)
{
    GPIO_LevelType oldGpioLvl, newGpioLvl;

    ULOG_ERR("PINMUX_IT: START\n");
    PINMUX_SetPinMode(PINMUXIT1_TESTPIN0, PINMUX_PIN_MODE_GPIO);
    PINMUX_SetPinDirection(PINMUXIT1_TESTPIN0, PINMUX_PIN_DIRECTION_OUT);
    oldGpioLvl = GPIO_ReadChannel(PINMUXIT1_TESTPIN0);
    newGpioLvl = GPIO_FlipChannel(PINMUXIT1_TESTPIN0);
    if (newGpioLvl == oldGpioLvl) {
        ULOG_ERR("\nError: PinDirection Fail &ld", __LINE__);
        goto err;
    }

    oldGpioLvl = GPIO_ReadChannel(PINMUXIT1_TESTPIN1);
    PINMUX_RefreshPortDirection();
    newGpioLvl = GPIO_FlipChannel(PINMUXIT1_TESTPIN1);
    if (newGpioLvl != oldGpioLvl) {
        ULOG_ERR("\nError: Refresh Port Direction Fail &ld", __LINE__);
        goto err;
    }

    PinmuxIT1_Status = BCM_ERR_OK;

err:
    if (BCM_AT_EXECUTING == PinmuxIT1_Status) {
        PinmuxIT1_Status= BCM_ERR_UNKNOWN;
    }
    ULOG_ERR("PINMUX_IT: END\n");
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
    case PINMUX_IT1:
        retErika = GetTaskState(PinmuxIT1_Task, taskStatePtr);
        if (E_OK == retErika) {
            if (SUSPENDED == taskState) {
                PinmuxIT1_Status = BCM_AT_EXECUTING;
                pinmuxTestIdx = aIndex;
                retVal = BCM_ActivateTask(PinmuxIT1_Task);
                if (BCM_ERR_OK != retVal) {
                    retVal = BCM_AT_NOT_STARTED;
                    PinmuxIT1_Status = BCM_AT_NOT_STARTED;
                    ULOG_ERR("PinmuxIT1_Task activation failed, error: %d\n\r", retVal);
                }
            } else {
                retVal = BCM_AT_EXECUTING;
                ULOG_ERR("PinmuxIT1_Task is already running \n\r");
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("GetTaskState PinmuxIT1_Task failed Error: %d\n\r", retErika);
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
    case PINMUX_IT1:
        if (aIndex == pinmuxTestIdx) {
            retVal = PinmuxIT1_Status;
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
