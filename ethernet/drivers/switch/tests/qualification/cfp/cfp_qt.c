/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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

/** @brief Integration test for CFP */

#include "ee.h"
#include <string.h>
#include <ulog.h>
#include <utils.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <nif.h>
#include <nif_switch.h>
#include <eth_xcvr.h>
#include <osil/bcm_osil.h>
#include "cfp.h"

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define NUM_PKTS                4UL

#ifdef __BCM8956X__
uint32_t TestPort = 2UL;
#else
uint32_t TestPort = 0UL;
#endif

static uint32_t numRxPkts = 0UL;
static uint32_t numExpectedRxPkts = 0UL;

static const uint8_t pdelay_req_pkt[]= {0x12U, 0x02U, 0x00U, 0x36U,
                                        0x00U, 0x00U, 0x00U, 0x08U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0xAAU, 0xBBU, 0xCCU, 0xFFU,
                                        0xFEU, 0xDDU, 0xEEU, 0xFFU,
                                        0x00U, 0x01U, 0x00U, 0x01U,
                                        0x05U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U, 0x00U, 0x00U,
                                        0x00U, 0x00U};

static uint8_t sync_pkt[]= {0x10U, 0x02U, 0x00U, 0x2CU,
                            0x00U, 0x00U, 0x02U, 0x08U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0xAAU, 0xBBU, 0xCCU, 0xFFU,
                            0xFEU, 0xDDU, 0xEEU, 0xFFU,
                            0x00U, 0x01U, 0x00U, 0x01U,
                            0x00U, 0xFBU, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U};

static volatile int32_t cfp_qt_result = BCM_AT_NOT_STARTED;

const CFP_ConfigType cfpConfig =
{
    .magicId = CFP_CONFIG_MAGIC_ID,
    .pktLenCorr = CFP_PKTLENCORR_NONE,
#ifdef __BCM8956X__
    .portEnableMask = 0x4U,
#else
    .portEnableMask = 0x1U,
#endif
    .ruleList = {
        {
            .key = {
                .l2Framing         = CFP_L2FRAMING_DIXV2,
                .l3Framing         = CFP_L3FRAMING_NONIP,
                .flags             = 0x88F7UL,
                .flagsMask         = 0xFFFFUL,
                .ingressPortBitmap = 0x1FFU,
                .cTagFlags         = CFP_KEY_TAG_UN_TAGGED_MASK,
                .sTagFlags         = CFP_KEY_TAG_UN_TAGGED_MASK,
                .udfList           = {
                    {
                        .value         = 0x000EU,
                        .mask          = 0xFFFFU,
                        .baseAndOffset = 2U,
                    },
                    {
                        .value         = 0xC200U,
                        .mask          = 0xFFFFU,
                        .baseAndOffset = 1U,
                    },
                    {
                        .value         = 0x0180U,
                        .mask          = 0xFFFFU,
                        .baseAndOffset = 0U,
                    },
                    {
                        .value         = 0x0200U,
                        .mask          = 0x0F00U,
                        .baseAndOffset = 7U,
                    },
                },
                .numEnabledUDFs    = 4U,
            },
            .action = {
                .meter = {
                    .policerFlags = CFP_POLICERMODE_DISABLED << CFP_METER_MODE_SHIFT,
                },
                .dstMapIBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT) | 0x80U,
                .dstMapOBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT) | 0x80U,
                .tcFlags = (1U << CFP_ACTION_CHANGE_TC_SHIFT) | 7U,
                .otherFlags = (CFP_ACTION_BYPASS_VLAN_MASK | CFP_ACTION_BYPASS_EAP_MASK | CFP_ACTION_BYPASS_STP_MASK),
            },
            .rowAndSlice = CFP_MAX_RULES | ((CFP_MAX_SLICES << CFP_ROWANDSLICE_SLICE_SHIFT) & CFP_ROWANDSLICE_SLICE_MASK),
        },
    },
    .ruleListSz = 1U
};

void DelayTimerAlarmCb(void)
{
    SetEvent(CFPTest, SystemEvent0);
}

static int32_t SendPktsAndVerify(const NIF_CntrlIDType* const ethCtrlIdx,
                                 uint32_t numExpected)
{
    uint32_t              i            = 0UL;
    uint8_t               *pdu         = NULL;
    uint32_t              bufIdx       = 0xFFUL;
    uint32_t              length;
    int32_t               ret;
    ETHERSWT_MgmtInfoType mgmtInfo;
    BCM_EventMaskType     mask;
    const uint8_t         dstMacAddr[] = {0x01U, 0x80U, 0xC2U, 0x00U, 0x00U, 0x0EU};

    numRxPkts = 0UL;
    numExpectedRxPkts = numExpected;

    for (; i < NUM_PKTS; ++i) {
        length = 90U;
        ret = NIF_GetTxBuffer(*ethCtrlIdx, ETHER_ETHERTYPE_GPTP, 6U, &bufIdx, &pdu, &length);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("NIF_GetTxBuffer() failed with %d\n", ret);
            goto err;
        }

        if (0 == (i % 2U)) {
            length = sizeof(pdelay_req_pkt);
            BCM_MemCpy(pdu, pdelay_req_pkt, length);
        } else {
            length = sizeof(sync_pkt);
            BCM_MemCpy(pdu, sync_pkt, length);
        }

        mgmtInfo.switchIdx = 0UL;
        mgmtInfo.PortIdx   = TestPort;
        ret = NIF_SwtSetMgmtInfo(*ethCtrlIdx, bufIdx, &mgmtInfo);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("NIF_SwtSetMgmtInfo() failed with %d\n", ret);
            goto err;
        }
        ret = NIF_Send(*ethCtrlIdx, ETHER_ETHERTYPE_GPTP, 1UL, bufIdx, length, dstMacAddr);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("NIF_Send() failed with %d\n", ret);
            goto err;
        }
    }

    /* Now wait for upto 30 ticks for packets to be received */
    SetRelAlarm(DelayTimerAlarm, 30U, 0U);

    BCM_WaitEvent(SystemEvent0);
    BCM_GetEvent(CFPTest, &mask);
    if (mask & SystemEvent0) {
        BCM_ClearEvent(SystemEvent0);
        ULOG_ERR("Sent %lu packets and received %lu back, expected %lu\n", NUM_PKTS, numRxPkts, numExpected);
        if (numRxPkts != numExpected) {
            ret = BCM_ERR_UNKNOWN;
        }
    }
    /* Disable the alarm so that it can be reused */
    CancelAlarm(DelayTimerAlarm);
err:
    return ret;
}
/**
  @defgroup grp_cfp_qt Qualification Tests
  @ingroup grp_eth_switch_cfp

  @addtogroup grp_cfp_qt
  @{

  @file cfp_qt.c
  @brief CFP Integration Test
  This source file contains the qualification tests for CFP
  @version 0.1 Initial version
*/

/** @brief Brief description of COMP_IT1

  This test simulates a whitelisting/blacklisting use case.

  @code{.unparsed}
  # Activate CFP Test Task
  # Setup
    - Retrieve the NIF controller index for a gPTP client
    - Enable Ethernet loopback on test port
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that no packets are received
  # Have a single rule in the CFP configuration to forward gPTP PDelay Request
    packets, identified by destination MAC address 01:80:c2:00:00:0e, ethertype
    0x88f7 and message type 2 (in the common header), to the ARM CPU.
  # Initialize the CFP block using CFP_Init()
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that only half (PDelay Request) the packets are
    received
  # Retrieve statistics using CFP_GetStats() for row 0 and ensure that the green
    statistics counter is half of transmitted count, while others are zero
  # Insert a rule at row 0, slice 2 using CFP_AddRule(), to drop all gPTP PDUs.
    Verify that the call fails with BCM_ERR_NOPERM
  # Insert the same rule using CFP_AddRule(), to drop all gPTP PDUs, this time
    letting the module chose the row and slice. Verify that the rule successfully
    gets programmed at row 1 and slice 2
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that only half (PDelay Request) PDUs are received
  # Update the rule at row 1 (to drop all gPTP PDUs) using CFP_UpdateRule()
    to now forward them to the CPU
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that all are received
  # Disable test port using CFP_EnablePort()
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that none are received
  # Re-enable test port using CFP_EnablePort()
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that all are received
  # Remove the rule at row 1 using CFP_RemoveRule()
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that only half (PDelay Request) the PDUs are
    received
  # De-initialize the CFP block using CFP_DeInit()
  # Transmit gPTP packets, (alternating between Sync and PDelay Request PDUs)
    destined to test port and verify that none are received
  # Terminate CFP Test Task
@endcode
*/

TASK(CFPTest)
{
    NIF_CntrlIDType ethCtrlIdx = ~(0x0ULL);
    int32_t         ret;
    uint32_t        row;
    uint32_t        slice;
    CFP_KeyType     key;
    CFP_ActionType  action;
    CFP_StatsType   stats;
    CFP_RuleType    rule;

    /* Setup */
    ret = NIF_GetCtrlIdx(0U, BCM_ETS_ID, 0U, &ethCtrlIdx);

    /* Set loopback on Ethernet port */
    ETHXCVR_SetLoopbackMode(TestPort, ETHXCVR_BOOLEAN_TRUE);

    /* Transmit gPTP packets and verify that none are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, 0UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Initialize CFP block */
    ULOG_ERR("Initializing CFP\n");
    ret = CFP_Init(0UL, &cfpConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_Init() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and confirm that only the PDelay Request packets are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, NUM_PKTS/2UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Retrieve stats */
    ret = CFP_GetStats(0UL, 0UL, &stats);
    if ((BCM_ERR_OK != ret) || (stats.green != NUM_PKTS/2UL) || (stats.yellow != 0UL) ||
        (stats.red != 0UL)) {
        ULOG_ERR("CFP_GetStats() failed with %d, GreenCntr = %lu \
                 YellowCntr = %lu RedCntr = %lu\n", ret, stats.green, stats.yellow,
                 stats.red);
        cfp_qt_result = ret;
        goto err;
    }

    /* Insert rule to drop all gPTP packets */
    BCM_MemCpy(&key, &cfpConfig.ruleList[0U].key, sizeof(CFP_KeyType));
    BCM_MemCpy(&action, &cfpConfig.ruleList[0U].action, sizeof(CFP_ActionType));
    key.udfList[3U].value = 0x0U;
    key.udfList[3U].baseAndOffset = 0U;
    key.numEnabledUDFs = 3U;
    action.dstMapIBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
    action.dstMapOBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);

    row = 0UL;
    slice = 2UL;
    ULOG_ERR("Inserting rule to drop all gPTP packets at row 0, slice 2\n");
    ret = CFP_AddRule(0UL, &key, &action, &row, &slice);
    if (BCM_ERR_NOPERM != ret) {
        ULOG_ERR("CFP_AddRule() did not fail with BCM_ERR_NOPERM when trying to overwrite static rule at row 0\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    ULOG_ERR("Inserting rule to drop all gPTP packets \n");
    row = CFP_MAX_RULES;
    slice = CFP_MAX_SLICES;
    ret = CFP_AddRule(0UL, &key, &action, &row, &slice);
    if ((BCM_ERR_OK != ret) || (row != 1UL) || (slice != 2UL)) {
        ULOG_ERR("CFP_AddRule() failed with %d row=%lu slice=%lu\n", ret, row, slice);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and confirm that only half are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, NUM_PKTS/2UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Update Rule to forward all gPTP packets */
    ULOG_ERR("Updating rule to forward all gPTP packets\n");
    action.dstMapIBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT) | 0x80U,
    action.dstMapOBFlags = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT) | 0x80U,
    ret = CFP_UpdateRule(0UL, row, &action);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_UpdateRule() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Fetch the configuration and verify that it took effect */
    ret = CFP_GetRowConfig(0UL, row, &rule);
    if ((BCM_ERR_OK != ret) || (rule.action.dstMapOBFlags != ((CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT) | 0x80U))
        || (rule.key.numEnabledUDFs != 3U)) {
        ULOG_ERR("CFP_GetRowConfig() failed with %d, numEnabledUDFs = lu, dstMapOBFlags = 0x%x\n",
                   ret, rule.key.numEnabledUDFs, rule.action.dstMapOBFlags);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and confirm that all are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, NUM_PKTS);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Disable test port */
    ULOG_ERR("Disabling port %u\n", TestPort);
    ret = CFP_DisablePort(0UL, TestPort);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_EnablePort() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and confirm that none are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, 0UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /*Re-enable test port */
    ULOG_ERR("Re-enabling port %u\n", TestPort);
    ret = CFP_EnablePort(0UL, TestPort);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_EnablePort() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and confirm that all are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, NUM_PKTS);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Remove rule for all gPTP packets */
    ULOG_ERR("Removing rule for forwarding gPTP packets\n");
    ret = CFP_RemoveRule(0UL, row);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_RemoveRule() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and verify that only half are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, NUM_PKTS/2UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* De-Initialize CFP block */
    ULOG_ERR("De-initializing CFP\n");
    ret = CFP_DeInit(0UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CFP_DeInit() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }

    /* Transmit gPTP packets and verify that none are received */
    ret = SendPktsAndVerify(&ethCtrlIdx, 0UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SendPktsAndVerify() failed with %d\n", ret);
        cfp_qt_result = ret;
        goto err;
    }
err:
    ULOG_ERR("\n Test completed");
    if (BCM_AT_EXECUTING == cfp_qt_result) {
        cfp_qt_result = BCM_ERR_OK;
    }
    BCM_TerminateTask();
}
/** @} */

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        cfp_qt_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(CFPTest);
        ret = BCM_ERR_OK;
    }

    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = cfp_qt_result;
    }

    return ret;
}

void ApplicationInit()
{
}

void ETS_RxPktIndication (uint64_t aCtrlIdx,
                          const uint8_t *const aPktBuf,
                          uint16_t aPktLen,
                          const uint8_t *const aSrcMacAddr,
                          const uint8_t *const aDestMacAddr,
                          uint32_t aPriority)
{
    numRxPkts++;
}

void ETS_RxPktTSIndication(uint64_t aCtrlIdx,
                     const uint8_t *const aBuf,
                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                     const ETHER_TimestampType *const aTS,
                     const ETHER_TimestampQualType* const aTSQual)
{
}

void ETS_TxPktConfirmation (uint64_t aCtrlIdx,
                            uint32_t aBufIdx)
{
}

void ETS_TxPktTSIndication(uint64_t aCtrlIdx,
                           uint32_t aBufIdx,
                           const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                           const ETHER_TimestampType *const aTS,
                           const ETHER_TimestampQualType* const aTSQual)
{
}

void ETS_RxMgmtInfoIndication(uint64_t aCtrlIdx,
                              const uint8_t *const aBuf,
                              const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
}

