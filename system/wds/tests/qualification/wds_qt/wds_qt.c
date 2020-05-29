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

#include "ee.h"
#include "eecfg.h"

#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <utils.h>
#include <osil/bcm_osil.h>

#include "wds.h"
#include "wds_osil.h"
#include <wdt.h>

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

static volatile int32_t wds_qt1_result = BCM_AT_NOT_STARTED;

static const uint8_t Test_SE_1_graph[3U][1U] = {{1U},{2U},{0U}};
static const uint8_t Test_SE_2_graph[4U][2U] = {{1U, 2U}, {2U, 3U}, {3U, WDS_INVALID_GRAPH_BUF_ENTRY}, {0U, WDS_INVALID_GRAPH_BUF_ENTRY}};
static const uint8_t Test_SE_3_graph[1U][1U] = {{0U}};

void WDS_QT_ErrCallback(const char * aName, int32_t aError,
                       const WDS_MonitorType *aMonitor, uint32_t aMaxAllowedErrors)
{
    ULOG_ERR("ERROR > Name %s aError %x \n",aName,aError);
}

uint64_t countAlarm = 0ULL;
void WDSTestAlarm_cb()
{
    if (countAlarm % 2UL == 0UL) {
        (void)SetEvent(WDSTest1, SystemEvent0);
        (void)SetEvent(WDSTest2, SystemEvent0);
        (void)SetEvent(WDSTest3, SystemEvent0);
    }
    if (countAlarm % 5UL == 0UL) {
        (void)SetEvent(WDSTestMonitor, SystemEvent0);
    }
    countAlarm++;
}

TASK(WDSTest1)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t count = 0U;
    BCM_EventMaskType mask = 0UL;
    WDS_ConfigType seConfig11 = {WDS_MODE_LOGICAL, 500UL, 2UL, 3UL, 0U, 2U};

    BCM_MemCpy(SE1.buffer, Test_SE_1_graph, sizeof(Test_SE_1_graph));

    retVal = WDS_EnableSE(&SE1, &seConfig11);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_EnableSE 0 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    /* Clear any pending event */
    (void)BCM_GetEvent(WDSTest1, &mask);
    (void)BCM_ClearEvent(mask);

    while (1) {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_GetEvent(WDSTest1, &mask);
        (void)BCM_ClearEvent(mask);

        if (countAlarm >= 100UL) {
            break;
        }

        for (count = 0U; count < SE1.maxState; count++) {
            retVal = WDS_NotifyState(&SE1, count);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("WDS_NotifyState Failed %x \n",retVal);
                wds_qt1_result = retVal;
                goto error;
            }
        }
    }

    retVal = WDS_DisableSE(&SE1);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_DisableSE 0 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

error:
    BCM_TerminateTask();
}

TASK(WDSTest2)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t count = 0U;
    BCM_EventMaskType mask = 0UL;
    WDS_ConfigType seConfig12 = {WDS_MODE_FREQ | WDS_MODE_LOGICAL | WDS_MODE_DEADLINE, 500UL, 2UL, 3UL, 0U, 3U};

    uint32_t Test_SE_12_cnt = 0UL;
    uint8_t Test_SE_12_state[3][5] = {{0U, 2U, 3U, 0xFFU, 0xFFU},
                                      {0U, 1U, 3U, 0xFFU, 0xFFU},
                                      {0U, 1U, 2U, 3U,    0xFFU}};

    BCM_MemCpy(SE2.buffer, Test_SE_2_graph, sizeof(Test_SE_2_graph));

    retVal = WDS_EnableSE(&SE2, &seConfig12);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_EnableSE 1 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    /* Clear any pending event */
    (void)BCM_GetEvent(WDSTest2, &mask);
    (void)BCM_ClearEvent(mask);

    while (1) {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_GetEvent(WDSTest2, &mask);
        (void)BCM_ClearEvent(mask);

        if (countAlarm >= 100UL) {
            break;
        }

        for (count = 0U; count < SE2.maxState; count++) {
            uint8_t gState = Test_SE_12_state[Test_SE_12_cnt][count];
            if (0xFFU != gState) {
                retVal = WDS_NotifyState(&SE2, gState);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("WDS_NotifyState Failed %x \n",retVal);
                    wds_qt1_result = retVal;
                    goto error;
                }
            }
        }
        Test_SE_12_cnt = (Test_SE_12_cnt+1UL) % 3UL;
    }

    retVal = WDS_DisableSE(&SE2);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_DisableSE 1 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

error:
    BCM_TerminateTask();
}

TASK(WDSTest3)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t count = 0U;
    BCM_EventMaskType mask = 0UL;
    WDS_ConfigType seConfig13 = {WDS_MODE_FREQ | WDS_MODE_LOGICAL | WDS_MODE_DEADLINE, 500UL, 2UL, 3UL, 0U, 0U};

    BCM_MemCpy(SE3.buffer, Test_SE_3_graph, sizeof(Test_SE_3_graph));

    retVal = WDS_EnableSE(&SE3, &seConfig13);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_EnableSE 0 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    /* Clear any pending event */
    (void)BCM_GetEvent(WDSTest3, &mask);
    (void)BCM_ClearEvent(mask);

    while (1) {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_GetEvent(WDSTest3, &mask);
        (void)BCM_ClearEvent(mask);

        if (countAlarm >= 100UL) {
            break;
        }

        for (count = 0U; count < SE3.maxState; count++) {
            retVal = WDS_NotifyState(&SE3, count);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("WDS_NotifyState Failed %x \n",retVal);
                wds_qt1_result = retVal;
                goto error;
            }
        }
    }

    retVal = WDS_DisableSE(&SE3);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_DisableSE 0 Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

error:
    BCM_TerminateTask();
}

TASK(WDSTestMonitor)
{
    int32_t retVal = BCM_ERR_OK;
    BCM_EventMaskType mask = 0UL;

    /* Enable the WDT driver to active state */
    retVal = WDT_SetMode(0UL, WDT_MODE_ACTIVE);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDT_SetMode Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    /* Set the WDT timer duration based on the configuration to WDS */
    retVal = WDT_SetTriggerCondition(0UL, WDS_CHECK_TIME_US);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDT_SetTriggerCondition Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    retVal = WDS_Init(WDS_SupervisionEntities, WDS_SUPERVISION_ENTITY_CNT);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_Init Failed %x \n",retVal);
        wds_qt1_result = retVal;
        goto error;
    }

    /* Clear any pending event */
    (void)BCM_GetEvent(WDSTestMonitor, &mask);
    (void)BCM_ClearEvent(mask);

    while (1) {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_GetEvent(WDSTestMonitor, &mask);
        (void)BCM_ClearEvent(mask);
        retVal = WDS_Monitor(WDS_SupervisionEntities, WDS_SUPERVISION_ENTITY_CNT);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("WDS_Monitor Failed %x \n",retVal);
            wds_qt1_result = retVal;
            goto error;
        } else {
            WDT_Service(0UL);
        }

        if (countAlarm >= 100UL) {
            break;
        }
    }

    retVal = WDS_DeInit(WDS_SupervisionEntities, WDS_SUPERVISION_ENTITY_CNT);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDS_DeInit Failed \n");
        wds_qt1_result = retVal;
        goto error;
    }

    /* Disable the WDT driver */
    retVal = WDT_SetMode(0UL, WDT_MODE_OFF);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("WDT_SetMode Failed \n");
        wds_qt1_result = retVal;
        goto error;
    }

error:
    if (BCM_AT_EXECUTING == wds_qt1_result) {
        wds_qt1_result = BCM_ERR_OK;
        ULOG_ERR("WDS_IT1 Passed\n");
    }

    BCM_TerminateTask();
    return;
}


int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        wds_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("\nTest started\n");
        BCM_ActivateTask(WDSTestMonitor);
        BCM_ActivateTask(WDSTest1);
        BCM_ActivateTask(WDSTest2);
        BCM_ActivateTask(WDSTest3);
        countAlarm = 0ULL;
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = wds_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}

