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

/** @brief Integration test for Ethernet Transceiver driver
 */

#include "ee.h"
#include <string.h>
#include <ulog.h>
#include <utils.h>
#include <imgl.h>
#include <ethernet.h>
#include <eth_xcvr.h>
#include <osil/ethxcvr_osil.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#ifdef ENABLE_ETH_SWITCH
#include <eth_switch.h>
#endif

#define GetModuleLogLevel()     ULOG_LVL_ERROR

#define CHECK_ERR_BREAK(ret, port)                                      \
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {            \
        ULOG_ERR("Error:%d for port %u line %u\n", ret, port, __LINE__);\
        break;                                                          \
    }                                                                   \

#define CHECK_PARAMS(param1, param2, port)                           \
    if ((param1 != param2) && (BCM_ERR_OK == ret)) {                 \
        ret = BCM_ERR_UNKNOWN;                                       \
        ULOG_ERR("Error: params [%u] != [%u] for port %u line %u\n", \
                param1, param2, port, __LINE__);        \
        break;                                                       \
    }

#define ETHXCVR_QT1         (1UL)

static volatile int32_t ETHXCVR_QT1_Status = BCM_AT_NOT_STARTED;

#if defined(ENABLE_ETHER_TEST)
void ETHER_RxInd (ETHER_HwIDType aID, const uint8_t *const aBuf,
                uint32_t aLen, const uint8_t *const aSrcMacAddr,
                const uint8_t *const aDestMacAddr)
{
}

void ETHER_TxDoneInd (ETHER_HwIDType aID, uint32_t aBufIndx,
                    ETHER_ResultType aResult)
{
}

void ETHXCVR_LinkStateChangeInd(ETHXCVR_IDType aID,
                                ETHXCVR_LinkStateType aLinkState)
{
}
#endif

#if defined(ENABLE_ETH_SWITCH)
void ETHERSWT_MgmtInfoInd(ETHER_HwIDType aCntrlID,
        const uint8_t *const aData,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
}

void ETHERSWT_TxTSInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo,
        const ETHER_TimestampType *const aTS,
        const ETHER_TimestampQualType* const aTSQual)
{
}

void ETHERSWT_RxTSInd(ETHER_HwIDType aCntrlID,
        const uint8_t *const aData,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo,
        const ETHER_TimestampType *const aTS,
        const ETHER_TimestampQualType* const aTSQual)
{
}

void ETHERSWT_LinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType aLinkState)
{
    ULOG_ERR("%s:SwtID:%x, Port:%x, LinkState:%x\n",
            __func__, aSwtID, aPortID, aLinkState);
}
#endif

TASK(TestTask)
{
    int32_t             ret = BCM_ERR_OK;
    uint32_t            i;
    ETHXCVR_BooleanType param;
    ETHXCVR_BooleanType newParam;
    ETHXCVR_ConfigType  config;

    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; i++) {
        ETHXCVR_GetPortConfig(i, &config.port[i]);
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ULOG_ERR("Detected port %u\n", config.port[i].id);
        }
    }

    ULOG_ERR("Testing SetMasterMode & GetMasterMode\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; i++) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            if ((config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_10BASET1) ||
                    (config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_100BASET1) ||
                    (config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_1000BASET1)) {
                ret = ETHXCVR_GetMasterMode(config.port[i].id, &param);
                CHECK_ERR_BREAK(ret, config.port[i].id);

                CHECK_PARAMS(param, config.port[i].phy.masterMode, config.port[i].id);

                if (ETHXCVR_BOOLEAN_TRUE == param) {
                    param = ETHXCVR_BOOLEAN_FALSE;
                } else {
                    param = ETHXCVR_BOOLEAN_TRUE;
                }
                ret = ETHXCVR_SetMasterMode(config.port[i].id, param);
                CHECK_ERR_BREAK(ret, config.port[i].id);

                ret = ETHXCVR_GetMasterMode(config.port[i].id, &newParam);
                CHECK_ERR_BREAK(ret, config.port[i].id);

                CHECK_PARAMS(param, newParam, config.port[i].id);

                ret = ETHXCVR_SetMasterMode(config.port[i].id, config.port[i].phy.masterMode);
                CHECK_ERR_BREAK(ret, config.port[i].id);
            }
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetLinkState\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ETHXCVR_LinkStateType link;
            ret = ETHXCVR_GetLinkState(config.port[i].id, &link);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetSpeed and SetSpeed\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ETHXCVR_SpeedType speed;
            ret = ETHXCVR_GetSpeed(config.port[i].id, &speed);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(speed, config.port[i].speed, config.port[i].id);

            ret = ETHXCVR_SetSpeed(config.port[i].id, speed);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetDuplex and SetDuplex\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ETHXCVR_DuplexModeType duplex;
            ret = ETHXCVR_GetDuplexMode(config.port[i].id, &duplex);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(duplex, config.port[i].duplex, config.port[i].id);

            ret = ETHXCVR_SetDuplexMode(config.port[i].id, ETHXCVR_DUPLEXMODE_FULL);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetFlowControl and SetFlowControl\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ETHXCVR_FlowControlType flowControl;
            ret = ETHXCVR_GetFlowControl(config.port[i].id, &flowControl);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(flowControl, config.port[i].flowControl, config.port[i].id);

            ret = ETHXCVR_SetFlowControl(config.port[i].id, ETHXCVR_FLOWCONTROL_NONE);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetSQI\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; i++) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            if ((config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_10BASET1) ||
                    (config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_100BASET1) ||
                    (config.port[i].phy.phyMedia == ETHXCVR_PHYMEDIA_1000BASET1)) {
                uint32_t sqi;
                ret = ETHXCVR_GetSQIValue(config.port[i].id, &sqi);
                CHECK_ERR_BREAK(ret, config.port[i].id);
            }
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetLoopbackMode and SetLoopbackMode\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ret = ETHXCVR_SetLoopbackMode(config.port[i].id, ETHXCVR_BOOLEAN_TRUE);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_GetLoopbackMode(config.port[i].id, &param);
            CHECK_ERR_BREAK(ret, config.port[i].id);
#ifdef __BCM8956X__
            if (config.port[i].id <= 4U) {
                continue;
            }
#endif
            CHECK_PARAMS(param, ETHXCVR_BOOLEAN_TRUE, config.port[i].id);

            ret = ETHXCVR_SetLoopbackMode(config.port[i].id, ETHXCVR_BOOLEAN_FALSE);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_GetLoopbackMode(config.port[i].id, &param);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(param, ETHXCVR_BOOLEAN_FALSE, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetJumboMode and SetJumboMode\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ret = ETHXCVR_GetJumboMode(config.port[i].id, &param);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(param, config.port[i].jumbo, config.port[i].id);

            if (ETHXCVR_BOOLEAN_TRUE == param) {
                param = ETHXCVR_BOOLEAN_FALSE;
            } else {
                param = ETHXCVR_BOOLEAN_TRUE;
            }

            ret = ETHXCVR_SetJumboMode(config.port[i].id, param);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_GetJumboMode(config.port[i].id, &newParam);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            CHECK_PARAMS(newParam, param, config.port[i].id);

            ret = ETHXCVR_SetJumboMode(config.port[i].id, config.port[i].jumbo);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing SetAutoNegMode, RestartAutoNeg and GetAutoNegStatus\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if ((ETHXCVR_BOOLEAN_TRUE == config.port[i].autoNeg) &&
            (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE)) {
            ETHXCVR_AutoNegStatusType status;

            ret = ETHXCVR_SetAutoNegMode(config.port[i].id, ETHXCVR_BOOLEAN_TRUE);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_RestartAutoNeg(config.port[i].id);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_GetAutoNegStatus(config.port[i].id, &status);
            CHECK_ERR_BREAK(ret, config.port[i].id);

            ret = ETHXCVR_SetAutoNegMode(config.port[i].id, config.port[i].autoNeg);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        goto err;
    }

    ULOG_ERR("Testing GetStats\n");
    for (i = 0UL; i < ETHXCVR_HW_ID_MAX; ++i) {
        if (config.port[i].portEnable == ETHXCVR_BOOLEAN_TRUE) {
            ETHXCVR_StatsType stats;
            ret = ETHXCVR_GetStats(config.port[i].id, &stats);
            CHECK_ERR_BREAK(ret, config.port[i].id);
        }
    }
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)){
        goto err;
    }

    if (ret == BCM_ERR_NOSUPPORT) {
        ret = BCM_ERR_OK;
    }
err:
    ULOG_ERR("\nTest completed\n");
    if (BCM_AT_EXECUTING == ETHXCVR_QT1_Status) {
        ETHXCVR_QT1_Status = ret;
    }
    BCM_TerminateTask();
}

void ApplicationInit()
{
    /* Dummy Init */
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ETHXCVR_QT1_Status = BCM_AT_EXECUTING;
        BCM_ActivateTask(TestTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;

    switch(aIndex) {
        case ETHXCVR_QT1:
            ret = ETHXCVR_QT1_Status;
            break;
        default:
            ret = BCM_AT_NOT_AVAILABLE;
    }

    return ret;
}
