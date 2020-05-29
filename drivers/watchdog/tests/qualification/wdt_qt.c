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
#include <wdt.h>
#include <osil/wdt_osil.h>
#include <bcm_time.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <mcu.h>
#include <shell.h>
#include <osil/bcm_osil.h>
#define GetModuleLogLevel() ULOG_LVL_INFO

#define WDT_IT1             (1UL)

static int32_t WDTIT1_Status = BCM_AT_NOT_STARTED;

/* Number of times the watchdog was serviced */
static volatile uint32_t WDTIT1_ServiceCnt = 0UL;

/* NMI handler status */
static volatile uint32_t WDTIT1_NMIHandlerStatus = 0UL;

void ApplicationInit(void)
{
    /* Dummy Init */
}

void WDT_IncAlarm_cb(void)
{
    if (BCM_AT_NOT_STARTED != WDTIT1_Status) {
        (void)SetEvent(WDTIT1_Task, SystemEvent0);
    }
}

TASK(WDTIT1_Task)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i = 0UL;
    /* Activate the watchdog */
    retVal = WDT_SetMode(0UL, WDT_MODE_ACTIVE);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("\nWDT_SetMode Error: %ld \t %ld", retVal, __LINE__);
        goto err;
    }
    do {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_ClearEvent(SystemEvent0);
        retVal = WDT_Service(0UL);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("\nWDT_Service Error: %ld \t %ld", retVal, __LINE__);
            goto err;
        }
        i++;
    } while(i < 100UL);

err:
    if ((BCM_AT_EXECUTING == WDTIT1_Status)
        && (BCM_ERR_OK != retVal)) {
        WDTIT1_Status = retVal;
    }
    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType retErika = E_OK;
    TaskStateType taskState = SUSPENDED;

    switch (aIndex) {
    case WDT_IT1:
        retErika = GetTaskState(WDTIT1_Task, &taskState);
        if (BCM_ERR_OK == retErika) {
            if (SUSPENDED == taskState) {
                if (BCM_AT_EXECUTING != WDTIT1_Status) {
                    WDTIT1_Status = BCM_AT_EXECUTING;
                    retVal = BCM_ActivateTask(WDTIT1_Task);
                    if (BCM_ERR_OK != retVal) {
                        WDTIT1_Status = BCM_AT_NOT_STARTED;
                        ULOG_ERR("Task activation failed, error: %d\n",
                                retVal);
                    }
                }
            } else {
                WDTIT1_Status = BCM_AT_EXECUTING;
                ULOG_ERR("Test is already running\n");
            }
        } else {
            ULOG_ERR("GetTaskState: Error(%d)\n", retErika);
            retVal = BCM_ERR_UNKNOWN;
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
    MCU_ResetReasonType reset = MCU_GetResetReason();

    switch (aIndex) {
    case WDT_IT1:
        if (MCU_RESETREASON_WD == reset) {
            retVal = BCM_ERR_OK;
        } else {
            retVal = WDTIT1_Status;
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}
