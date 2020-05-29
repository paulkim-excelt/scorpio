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

/** @brief Integration test for Ethernet driver
 */

#include "ee.h"
#include <string.h>
#include <ulog.h>
#include <utils.h>
#include <ethernet.h>
#include <eth_xcvr.h>
#include <osil/eth_osil.h>
#include <osil/ethxcvr_osil.h>
#include <osil/brphy_osil.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#if defined(ENABLE_ETH_SWITCH)
#include <eth_switch_osil.h>
#endif

#define GetModuleLogLevel()     ULOG_LVL_ERROR

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

void ETHXCVR_LinkStateChangeInd(ETHXCVR_IDType aID,
                                  ETHXCVR_LinkStateType aLinkState)
{
    ULOG_ERR("%s:XcvrID:%x, LinkState:%x\n", __func__, aID, aLinkState);
}

void ETHXCVR_ModeIndication(ETHXCVR_IDType aID, ETHXCVR_ModeType aMode)
{
    ULOG_ERR("%s:XcvrID:%x, Mode:%x\n", __func__, aID, aMode);
}

#ifdef __BCM8956X__
#define ETHXCVR_PORT_ID           (2UL)     /* Port 2 (BRPHY) */
#define ETHXVCR_PHY_ID            (1UL)     /* BRPHY1 */
#else
#define ETHXCVR_PORT_ID           (0UL)     /* Port 0 (BRPHY) */
#define ETHXVCR_PHY_ID            (0UL)     /* BRPHY0 */
#endif

#define ETHER_HW_ID             (0UL)

#define ETHERNET_TX_IT1         (1UL)
#define EVENT_BUF_AVAILABLE     SystemEvent0
#define ETHIT1_PKT_LEN          (52UL)
#define ETHIT1_FIXED_PKT_LEN    (51UL)
#define ETHIT1_PKT_CNT          (10UL)

#define MARK_ERR(ret, line) {ULOG_ERR("\nError: %ld, \tline %d", ret, line); EthIT1_Status = ret;}

static const uint8_t EthIT1_MacAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const ETHXCVR_PortConfigType EthIT1_XcvrConfig = {
    .id = ETHXCVR_PORT_ID,
    .portEnable = ETHXCVR_BOOLEAN_TRUE,
    .speed = ETHXCVR_SPEED_100MBPS,
    .duplex = ETHXCVR_DUPLEXMODE_FULL,
    .autoNeg = ETHXCVR_BOOLEAN_FALSE,
    .flowControl = ETHXCVR_FLOWCONTROL_NONE,
    .jumbo = ETHXCVR_BOOLEAN_FALSE,
    .busMode = ETHXCVR_BUSMODE_INTG,
    .phy = {
        .hwID = ETHXVCR_PHY_ID,
        .accessMode = ETHXCVR_ACCESSMODE_MMAP,
        .phyMedia = ETHXCVR_PHYMEDIA_100BASET1,
        .masterMode = ETHXCVR_BOOLEAN_TRUE,
    }
};


static ETHER_TimeCfgType EthIT1_TimeConfig[] = {
    {
        .hwIdx = 0UL,
        .portEnableMask = P1588_PORT_MASK,
#if defined(__BCM8910X__)
        .sync = ETHER_TIMESYNC_NONE,
#endif
#if defined(__BCM8953X__) || defined(__BCM8956X__)
        .sync = ETHER_TIMESYNC_INTERNAL,
#endif
    },
};

ETHER_TimeCfgType *const ETHER_TimeCfg = EthIT1_TimeConfig;
const uint32_t ETHER_TimeCfgSize = sizeof(EthIT1_TimeConfig)/
                                    sizeof(ETHER_TimeCfgType);

static const ETHER_CfgType EthIT1_Config = {
    .hwID = 0UL,
    .speed = ETHER_SPEED_100MBPS,
    .duplexMode = ETHER_DUPLEX_MODE_FULL,
    .maxRxFrmLen = 1522UL,
    .macAddr = EthIT1_MacAddr,
#if defined(__BCM8910X__)
    .prioChanMap[0] = 0UL,
    .prioChanMap[1] = 0UL,
    .prioChanMap[2] = 1UL,
    .prioChanMap[3] = 1UL,
    .prioChanMap[4] = 2UL,
    .prioChanMap[5] = 2UL,
    .prioChanMap[6] = 3UL,
    .prioChanMap[7] = 3UL,
#endif
#if defined(__BCM8953X__)
    .prioChanMap[0] = 0UL,
    .prioChanMap[1] = 0UL,
    .prioChanMap[2] = 0UL,
    .prioChanMap[3] = 0UL,
    .prioChanMap[4] = 0UL,
    .prioChanMap[5] = 0UL,
    .prioChanMap[6] = 0UL,
    .prioChanMap[7] = 0UL,
#endif
};


static const uint8_t EthIT1_FixedPktPattern[ETHIT1_FIXED_PKT_LEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x18,
    0x08, 0x00, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6E, 0x65, 0x74, 0x20, 0x54,
    0x65, 0x73, 0x74, 0x20, 0x41, 0x50, 0x50, 0x3A, 0x20, 0x54, 0x65, 0x73,
    0x74, 0x20, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x43, 0x44, 0x55,
    0x11, 0x55, 0xAA,
};

static uint32_t EthIT1_TxCbCnt = 0UL;
static uint32_t EthIT1_RxCbCnt = 0UL;
static uint32_t EthIT1_TxPktCnt = 0UL;
static uint32_t EthIT1_TimeoutCnt = 0UL;

#define ETHIT1_TIMEOUT_CNT_MAX  (10UL)

static volatile int32_t EthIT1_Status = BCM_AT_NOT_STARTED;

#if defined(ENABLE_ETH_SWITCH)
static uint32_t EthIT1_PatCompOffset = 4UL;
#else
static uint32_t EthIT1_PatCompOffset = 0UL;
#endif

/* Rx Callback */
void ETHER_RxInd (ETHER_HwIDType aID, const uint8_t *const aBuf,
                uint32_t aLen, const uint8_t *const aSrcMacAddr,
                const uint8_t *const aDestMacAddr)
{
    int32_t retVal;
    uint32_t pktNum;

    /* Compare source address */
    retVal =  memcmp(aSrcMacAddr, &EthIT1_MacAddr, ETHER_MAC_ADDR_SIZE);
    if (0UL != retVal) {
        retVal = BCM_ERR_DATA_INTEG;
        MARK_ERR(retVal, __LINE__);
    }

    /* Compare destination address */
    retVal =  memcmp(aDestMacAddr, &EthIT1_FixedPktPattern[6UL], ETHER_MAC_ADDR_SIZE);
    if (0UL != retVal) {
        retVal = BCM_ERR_DATA_INTEG;
        MARK_ERR(retVal, __LINE__);
   }

    retVal = memcmp((void *)aBuf, &EthIT1_FixedPktPattern[EthIT1_PatCompOffset],
            aLen - EthIT1_PatCompOffset - 1UL);
    if (0 == retVal) {
        pktNum = aBuf[aLen - 1UL];
        if (pktNum == (EthIT1_RxCbCnt & 0xFF) ) {
            EthIT1_RxCbCnt++;
        } else {
            retVal = BCM_ERR_DATA_INTEG;
            ULOG_ERR("pktNum:0x%x, EthIT1_RxCbCnt:0x%x", pktNum, EthIT1_RxCbCnt);
            MARK_ERR(retVal, __LINE__);
        }
    } else {
        retVal = BCM_ERR_DATA_INTEG;
        MARK_ERR(retVal, __LINE__);
    }
}
/* Tx Callback */
void ETHER_TxDoneInd (ETHER_HwIDType aID, uint32_t aBufIndx,
                    ETHER_ResultType aResult)
{
    if (ETHER_RESULT_SUCCESS != aResult) {
        MARK_ERR(BCM_ERR_DATA_INTEG, __LINE__);
    }

    EthIT1_TxCbCnt++;
    /* Sent a notification to the client that buffers are available */
    BCM_SetEvent(EthIT1_Client, EVENT_BUF_AVAILABLE);
}

/* Helper function to set Ethernet controller to Active */
int32_t EthIT_SetModeActive(void)
{
    int32_t retVal;
    ETHER_ModeType mode;

    retVal = ETHER_SetMode(ETHER_HW_ID, ETHER_MODE_ACTIVE);
    if (BCM_ERR_OK == retVal) {
        retVal = ETHER_GetMode(ETHER_HW_ID, &mode);
        if (BCM_ERR_OK == retVal) {
            if (ETHER_MODE_ACTIVE != mode) {
                retVal = BCM_ERR_INVAL_STATE;
            }
        }
    }

    if (BCM_ERR_OK != retVal) {
        MARK_ERR(retVal, __LINE__);
    }

    return retVal;
}

/* Helper function to set Xcvr to Active */
int32_t EthIT_XcvrSetModeActive(void)
{
    int32_t retVal;
    ETHXCVR_ModeType xcvrMode;

    retVal = ETHXCVR_SetMode(ETHXCVR_PORT_ID, ETHXCVR_MODE_ACTIVE);
    if (BCM_ERR_OK == retVal) {
        retVal = ETHXCVR_GetMode(ETHXCVR_PORT_ID, &xcvrMode);
        if (BCM_ERR_OK == retVal) {
            if (ETHXCVR_MODE_ACTIVE != xcvrMode) {
                retVal = BCM_ERR_INVAL_STATE;
            }
        }
    }

    if (BCM_ERR_OK != retVal) {
        MARK_ERR(retVal, __LINE__);
    }

    return retVal;
}

static int32_t EthIT1_Transmit(ETHER_HwIDType aHwID, ETHER_PrioType aPriority)
{
    int32_t retVal;
    uint32_t i, bufIdx;
    uint32_t pktLen;
    uint8_t *pktBuf;
    BCM_EventMaskType mask = 0UL;

    for (i = 0UL; i < ETHIT1_PKT_CNT; ++i) {
        /* Get buffer from Ethernet Driver */
        while (1UL) {
            pktLen = ETHIT1_PKT_LEN;
            retVal = ETHER_GetTxBuffer(aHwID, aPriority, &bufIdx, &pktBuf,
                                       &pktLen);
            if (BCM_ERR_NOMEM == retVal) {
                /* No buffer available; retry after some buffers get free */
                BCM_WaitEvent(EVENT_BUF_AVAILABLE);
                BCM_GetEvent(EthIT1_Client, &mask);
                if (EVENT_BUF_AVAILABLE == (mask & EVENT_BUF_AVAILABLE)) {
                    BCM_ClearEvent(mask);
                    if (BCM_AT_EXECUTING == EthIT1_Status) {
                        continue;
                    } else {
                        retVal = EthIT1_Status;
                        break;
                    }
                }
            } else if (BCM_ERR_OK == retVal) {
                /* Buffer obtained */
                break;
            } else {
                MARK_ERR(retVal, __LINE__);
                goto err;
            }
        }
        if (BCM_ERR_OK != retVal) {
            break;
        }

        /* Copy the packet to be send, into the buffer */
        memcpy((void *)pktBuf, EthIT1_FixedPktPattern, pktLen - 1UL);
        pktBuf[pktLen - 1UL] = (uint8_t)EthIT1_TxPktCnt;

        /* Send the packet */
        retVal = ETHER_Send(aHwID, 1UL, bufIdx, pktLen,
                            &EthIT1_FixedPktPattern[6UL]);
        if (BCM_ERR_OK != retVal) {
            MARK_ERR(retVal, __LINE__);
            goto err;
        } else {
            EthIT1_TxPktCnt++;
        }
    }
err:
    return retVal;
}

int32_t EthIT1_TransmitTest(ETHER_HwIDType aHwID)
{
    int32_t retVal;
    ETHER_PrioType priority;

    for (priority = 0UL; priority <= ETHER_PRIO_MAX; priority++) {
        /* Channel equal to priority for this test case */
        retVal = EthIT1_Transmit(aHwID, priority);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }

    if (BCM_ERR_OK == retVal) {
        /* Wait for transmit completion of all packets */
        BCM_EventMaskType mask = 0UL;
        while (EthIT1_TxCbCnt < ((ETHER_PRIO_MAX + 1UL) * ETHIT1_PKT_CNT)) {
            if (BCM_AT_EXECUTING == EthIT1_Status) {
                BCM_WaitEvent(EVENT_BUF_AVAILABLE);
                BCM_GetEvent(EthIT1_Client, &mask);
                if (EVENT_BUF_AVAILABLE == (mask & EVENT_BUF_AVAILABLE)) {
                    BCM_ClearEvent(mask);
                    continue;
                }
            } else {
                retVal = EthIT1_Status;
                break;
            }
        }
    }

    return retVal;
}

TASK(EthIT1_Client)
{
    int32_t retVal;
    EthIT1_TxCbCnt = 0UL;
    EthIT1_RxCbCnt = 0UL;
    EthIT1_TxPktCnt = 0UL;

    /* Initialize Ethernet driver */
    ETHER_Init(ETHER_HW_ID, &EthIT1_Config);

    /* Initialize Ethernet Transceiver */
    ETHXCVR_Init(ETHXCVR_PORT_ID, &EthIT1_XcvrConfig);

    /* Enable Ethernet Loopback */
    ETHER_EnableLoopBack(ETHER_HW_ID_0);

    /* Set controller mode to Active */
    retVal = EthIT_SetModeActive();
    if (BCM_ERR_OK != retVal) {
        MARK_ERR(retVal, __LINE__);
        goto err;
    }

    /* Set xcvr mode to Active */
    retVal = EthIT_XcvrSetModeActive();
    if (BCM_ERR_OK != retVal) {
        MARK_ERR(retVal, __LINE__);
        goto err;
    }

    /* Transmit packets */
    retVal = EthIT1_TransmitTest(ETHER_HW_ID);
    if (BCM_ERR_OK != retVal) {
        MARK_ERR(retVal, __LINE__);
        goto err;
    }

    /* De-Initialize Ethernet Transceiver */
    ETHXCVR_DeInit(ETHXCVR_PORT_ID);

    /* Deinitialize Ethernet driver */
    ETHER_DeInit(ETHER_HW_ID);

    if ((EthIT1_RxCbCnt == EthIT1_TxCbCnt) && (EthIT1_RxCbCnt != 0UL)){
        EthIT1_Status = BCM_ERR_OK;
    }

err:
    (void)BCM_CancelAlarm(EthIT1_Alarm);
    (void)BCM_TerminateTask();
}

void EthIT1_AlarmCb(void)
{
    if (BCM_AT_EXECUTING == EthIT1_Status) {
        if (ETHIT1_TIMEOUT_CNT_MAX < EthIT1_TimeoutCnt) {
            EthIT1_Status = BCM_ERR_TIME_OUT;
            SetEvent(EthIT1_Client, EVENT_BUF_AVAILABLE);
        } else {
            EthIT1_TimeoutCnt++;
        }
    }
}

int32_t EthIT1_GetResult(void)
{
    return EthIT1_Status;
}

void ApplicationInit()
{
    /* Dummy Init */
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret;
    StatusType retErika = E_OK;
    TaskStateType taskState = SUSPENDED;

    switch(aIndex)
    {
    case ETHERNET_TX_IT1:
        retErika = GetTaskState(EthIT1_Client, &taskState);
        if (E_OK == retErika) {
            if (SUSPENDED == taskState) {
                if (BCM_AT_EXECUTING != EthIT1_Status) {
                    ret = BCM_SetRelAlarm(EthIT1_Alarm, 500UL, 100UL);
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("Set relative alarm failed, error: %d\n",
                                ret);
                    } else {
                        EthIT1_Status = BCM_AT_EXECUTING;
                        ret = BCM_ActivateTask(EthIT1_Client);
                        if (BCM_ERR_OK != ret) {
                            EthIT1_Status = BCM_AT_NOT_STARTED;
                            ULOG_ERR("Task activation failed, error: %d\n",
                                    ret);
                        }
                    }
                }
                ret = BCM_ERR_OK;
            } else {
                EthIT1_Status = BCM_AT_EXECUTING;
                ULOG_ERR("Test is already running\n");
                ret = BCM_ERR_OK;
            }
        } else {
            MARK_ERR(BCM_ERR_UNKNOWN, __LINE__);
            ret = BCM_ERR_UNKNOWN;
        }
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
    }

    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;

    switch(aIndex)
    {
    case ETHERNET_TX_IT1:
        ret = EthIT1_GetResult();
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
    }

    return ret;
}
