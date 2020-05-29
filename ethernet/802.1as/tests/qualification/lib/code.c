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

#include <stdlib.h>
#include <string.h>
#include <ee.h>
#include <console.h>
#include <bcm_test.h>
#include <ulog.h>
#include <utils.h>
#include <bcm_time.h>
#include <nif.h>
#include <circ_queue.h>
#include <osil/bcm_osil.h>
#include "ets.h"
#include "ets_osil.h"

#define GetModuleLogLevel()  ULOG_LVL_INFO
#define CHECK_ERR(ret,line)  if (BCM_ERR_OK != ret) {\
                                ULOG_ERR("\n Error: %d \t line %d",ret,line); \
                                etslib_qt_result = ret; return; \
                             }
#define TIMER_EVENT          (SystemEvent0)
#define TX_PKT_EVENT         (SystemEvent1)

#define SLAVE_INTF           (0UL)
#define SEQNUM_OFFSET        (31U)
#define POT_OFFSET           (34U)

#define TXPKTQ_SIZE          (16UL)

typedef struct _TxPacket{
    ETS_TimestampType timestamp;
    uint32_t          intfIdx;
    uint32_t          timestampValid;
    uint32_t          pktType;
} TxPacket;

static uint32_t slaveMode = 0UL;

static uint32_t InterfaceDown[ETS_MAX_INTERFACES] = {1UL};

static TxPacket pkts[TXPKTQ_SIZE];

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

static uint8_t fwup_pkt[]= {0x18U, 0x02U, 0x00U, 0x4CU,
                            0x00U, 0x00U, 0x00U, 0x08U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0xAAU, 0xBBU, 0xCCU, 0xFFU,
                            0xFEU, 0xDDU, 0xEEU, 0xFFU,
                            0x00U, 0x01U, 0x00U, 0x01U,
                            0x02U, 0xFBU,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U,
                            0x00U, 0x03U, 0x00U, 0x1CU,
                            0x00U, 0x80U, 0xC2U, 0x00U,
                            0x00U, 0x01U,
                            0x00U, 0x00U, 0x08U, 0x00U,
                            0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U};

static const uint8_t dst_mac_addr[] = {0x01U, 0x80U, 0xC2U, 0x00U, 0x00U, 0x0EU};

static ETS_ConfigType ets_config =
{
    .magicId = ETS_CONFIG_MAGIC_ID,
    .adminMode = ETS_ADMINMODE_ENABLE,
    .clockMode = ETS_CLOCKMODE_GM,
    .avnuSyncAbsenceTimeout = 0U,
    .gmRateRatio = 0x40000000,
    .intfCfg = {
        {
            .hwPortNum = 0U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
#if (ETS_MAX_INTERFACES > 1)
        {
            .hwPortNum = 1U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 2U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 3U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 4U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 5U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 6U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        },
        {
            .hwPortNum = 8U,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_8S,
            .initLogSyncInterval = ETS_INITLOGSYNC_31_25MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_125MS,
            .nbrPropDelay = 2000UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        }
#endif
    }
};

static volatile int32_t etslib_qt_result = BCM_AT_NOT_STARTED;

CIRCQ_V2_DEFINE((txPktQ), 4UL, TxPacket, txPktQBuf,
                      txPktQIdx, (CIRCQ_OVERWRITE_DISABLE), ".data.gptp");

void ETS_RxPktIndication (uint64_t aCtrlIdx,
                          const uint8_t *const aPktBuf,
                          uint16_t aPktLen,
                          const uint8_t *const aSrcMacAddr,
                          const uint8_t *const aDestMacAddr,
                          uint32_t aPriority)
{
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

#if (ETS_MAX_INTERFACES > 1UL)
void ETS_RxMgmtInfoIndication(uint64_t aCtrlIdx,
                              const uint8_t *const aBuf,
                              const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
}
#endif

int32_t ETS_Transmit(const uint32_t aIntfIdx,
                     const uint64_t aCtrlIdx,
                     const uint32_t aBufIdx,
                     const uint32_t aLen,
                     const uint32_t aPktType,
                     const uint32_t aIsEventMsg,
                     const uint8_t* const aDstMacAddr)
{
    int32_t rc         = BCM_ERR_OK;
    BCM_TimeType    systemTime = {0UL};
    TxPacket txPkt     = { .intfIdx    = aIntfIdx,
                           .pktType    = aPktType,
                         };

    if (1UL == aIsEventMsg){
        BCM_GetTime(&systemTime);
        txPkt.timestampValid = TRUE;
        txPkt.timestamp.seconds = systemTime.s;
        txPkt.timestamp.nanoseconds = systemTime.ns;
    } else {
            txPkt.timestampValid = FALSE;
    }
    rc = CIRCQ_Push(&txPktQ, (char*)&txPkt, 1UL);
    if (BCM_ERR_OK == rc) {
        BCM_SetEvent(ETS_ProtocolTask, TX_PKT_EVENT);
    }
    rc = NIF_Send(aCtrlIdx, ETHER_ETHERTYPE_GPTP, 0UL, aBufIdx, 0UL, aDstMacAddr);

    return rc;
}

void ETS_ConfigSaveCallback (const uint32_t aIntfIndex,
                             const uint32_t aPdelay,
                             const int32_t  aRateRatio)
{
    ULOG_INFO("PDELAY changed for intf %lu\n", aIntfIndex);
}

void ETS_RecordCallback(ETS_RecordType* const aRecord)
{
}

void ETS_EventCallback (const uint32_t aIntfIndex,
                        const ETS_EventType aEvent)
{
    if (ETS_EVENT_IF_DOWN == aEvent) {
        InterfaceDown[aIntfIndex] = 1UL;
    }
    ULOG_INFO("Event %lu for intf %lu\n", aEvent, aIntfIndex);
}

void Shell_ETS(uint32_t aConsoleID,
               char* aArgString,
               uint32_t aArgStringLen)
{
    CONSOLE_Write(aConsoleID, "Not implemented for qualification test\n");
}

void EthTimeSyncTimer_AlarmCb()
{
    SetEvent(ETS_ProtocolTask, TIMER_EVENT);
}

/**
  @defgroup grp_ets_qt Qualification Tests
  @ingroup grp_ets

  @addtogroup grp_ets_qt
  @{

  @file code.c
  @brief ETS Library Integration Test
  This source file contains the qualification tests for ETS library
  @version 1.25 Import from MS Word document
*/

/** @brief Brief description of COMP_IT1

  This test simulates the grandmaster functionality for gPTP

  @code{.unparsed}
  # Activate ETS Protocol Task
  # Initialize Ethernet Timesync using ETS_Init().
  # Check the state of the module with ETS_GetState(). It should be
    ETS_STATE_RUNNING
  # Insert timer ticks every 15625 microseconds with ETS_ProcessTimerEvent()
    for a total period of 2 seconds. Also insert a PDelay Request every second
    with ETS_RxPacket()
  # Retrieve the global status with ETS_GetGlobalStatus(). Verify that
        - gmStatus is ETS_GMSTATUS_PRESENT and
        - clockState is ETS_CLOCKSTATE_UPDATE_GM
  # Retrieve the interface status for all interfaces which are linked up and
    asCapable with ETS_GetIntfStatus(). Verify that
        - currentLogPdelayInterval is ETS_INITLOGPDELAY_1S and
        - currentLogSyncInterval is ETS_INITLOGSYNC_31_25MS.
  # Invoke the ETS_IsGptpEventMsg() API with a PDelay Request PDU as input and
    verify that is able to identify it as an event PDU.
  # Retrieve the grandmaster rate ratio with ETS_GetGrandMasterRateRatio().
    Verify that it is 0x40000000.
  # Set the global time to 100000s with ETS_SetGlobalTime()
  # Retrieve the network time with ETS_GetCurrentTime() and verify that it is
    greater than 100000s. Also check if the timestamp state is
    ETS_TIMESTAMPSTATE_SYNC
  # Retrieve the statistics for all interfaces which are linked up and asCapable
    using ETS_GetStats(). Verify that the syncTxCount, pDelayReqTxCount and
    pDelayRespTxCount are all no-zero.
  # Stop gPTP operation by invoking ETS_DeInit()
  # Terminate ETS Protocol Task

@endcode
*/

/** @brief Brief description of COMP_IT2

  This test simulates the slave functionality for gPTP

  @code{.unparsed}
  # Activate ETS Protocol Task
  # Initialize Ethernet Timesync using ETS_Init().
  # Check the state of the module with ETS_GetState(). It should be
    ETS_STATE_RUNNING
  # Insert timer ticks every 15625 microseconds with ETS_ProcessTimerEvent()
    for a total period of 2 seconds.
        - Insert a PDelay Request every second  with ETS_RxPacket()
        - Insert a Sync and Follow up PDU on the slave interface every 31.25ms
  # Retrieve the global status with ETS_GetGlobalStatus(). Verify that
        - gmStatus is ETS_GMSTATUS_PRESENT
        - clockState is ETS_CLOCKSTATE_UPDATE_SLAVE
        - isSyncReceived is 1UL
        - slavePort is 0UL
  # Retrieve the interface status for all interfaces which are linked up and
    asCapable with ETS_GetIntfStatus(). For all ports verify that the
    currentLogPdelayInterval is ETS_INITLOGPDELAY_1S and
    currentLogSyncInterval is ETS_INITLOGSYNC_31_25MS. For the slave ports
    additionally, verify that
        - syncReceiptTimeoutInterval is 93750UL
        - syncLastRxSeqId is the sequence number of the last Sync/Follow Up
          message inserted
        - rxPduInterval is ETS_INITLOGSYNC_31_25MS
  # Invoke the ETS_IsGptpEventMsg() API with a PDelay Request PDU as input and
    verify that is able to identify it as an event PDU.
  # Retrieve the grandmaster rate ratio with ETS_GetGrandMasterRateRatio().
    Verify that it is 0x40000001.
  # Retrieve the network time with ETS_GetCurrentTime() and verify that it is
    greater than the Precise Origin Timestamp in the last inserted Follow Up
    message. Also check if the timestamp state is ETS_TIMESTAMPSTATE_SYNC
  # Retrieve the statistics for all interfaces which are linked up and asCapable
    using ETS_GetStats(). For each, verify that the pDelayReqTxCount and
    pDelayRespTxCount are non-zero. For the slave port verify that syncRxCount
    and for the master port verify that the syncTxCount is non-zero.
  # Stop gPTP operation by invoking ETS_DeInit()
  # Terminate ETS Protocol Task

@endcode
*/

TASK(ETS_ProtocolTask)
{
    ETS_TimestampType      time;
    ETS_TimestampStateType timestampState;
    ETS_GlobalStatusType   globalStatus;
    BCM_EventMaskType      mask = 0UL;
    BCM_TimeType               systemTime = {0UL};
    int32_t                rateRatio;
    uint32_t               numPkts;
    uint32_t               tick_count = 0UL;
    uint64_t               seconds = 0ULL;
    uint32_t               nanoseconds = 0UL;
    uint32_t               response;
    int32_t                ret;
    uint32_t               i;

    if (1UL == slaveMode) {
        ets_config.clockMode = ETS_CLOCKMODE_SLAVE;
#if (ETS_MAX_INTERFACES == 1UL)
        ets_config.intfCfg[0UL].role = ETS_ROLE_SLAVE;
#else
        ets_config.intfCfg[SLAVE_INTF].role = ETS_ROLE_SLAVE;
#endif
    } else {
        ets_config.clockMode = ETS_CLOCKMODE_GM;
#if (ETS_MAX_INTERFACES == 1UL)
        ets_config.intfCfg[0UL].role = ETS_ROLE_MASTER;
#else
        ets_config.intfCfg[SLAVE_INTF].role = ETS_ROLE_MASTER;
#endif
    }
    ret = ETS_Init(&ets_config);
    CHECK_ERR(ret,__LINE__);

    if(ETS_STATE_RUNNING !=  ETS_GetState()) {
        ULOG_ERR("Error: RESET state\n");
        etslib_qt_result = BCM_ERR_INVAL_STATE;
        return;
    }

    /* Wait for 2 seconds for peer delay to complete multiple transcations */
    while (tick_count < 128UL) {
        BCM_WaitEvent(TIMER_EVENT | TX_PKT_EVENT);
        BCM_GetEvent(ETS_ProtocolTask, &mask);
        if (mask & TIMER_EVENT) {
            ETS_ProcessTimerEvent();
            tick_count++;
            BCM_ClearEvent(TIMER_EVENT);
        }

        /* Insert a Pdelay request every second */
        if (0UL == tick_count % 64UL) {
            for (i = 0UL ; i < ETS_MAX_INTERFACES; ++i) {
                BCM_GetTime(&systemTime);
                time.seconds = systemTime.s;
                time.nanoseconds = systemTime.ns;
                if (ETS_ADMINMODE_ENABLE == ets_config.intfCfg[i].asCapable) {
                    ETS_RxPacket(ets_config.intfCfg[i].hwPortNum,
                                 pdelay_req_pkt, sizeof(pdelay_req_pkt),
                                 dst_mac_addr, &time, 1UL);
                }
            }
        }

        if (1UL == slaveMode) {
            /* Insert a Sync + Follow Up on the slave port every 31.25ms */
            if (0UL == tick_count % 2UL) {
                BCM_GetTime(&systemTime);
                time.seconds = systemTime.s;
                time.nanoseconds = systemTime.ns;
                /* Increment the sequence number */
                sync_pkt[SEQNUM_OFFSET]++;
                /* Insert Sync */
                ETS_RxPacket(ets_config.intfCfg[SLAVE_INTF].hwPortNum,
                             sync_pkt, sizeof(sync_pkt), dst_mac_addr,
                             &time, TRUE);

                /* Set the POT to 100 seconds in the future */
                seconds = time.seconds + 100ULL;
                nanoseconds = Host2BE32(time.nanoseconds);
                fwup_pkt[POT_OFFSET] = (uint8_t)((seconds >> 40UL) & 0xFFU);
                fwup_pkt[POT_OFFSET + 1U] = (uint8_t)((seconds >> 32UL) & 0xFFU);
                fwup_pkt[POT_OFFSET + 2U] = (uint8_t)((seconds >> 24UL) & 0xFFU);
                fwup_pkt[POT_OFFSET + 3U] = (uint8_t)((seconds >> 16UL) & 0xFFU);
                fwup_pkt[POT_OFFSET + 4U] = (uint8_t)((seconds >> 8UL) & 0xFFU);
                fwup_pkt[POT_OFFSET + 5U] = (uint8_t)(seconds & 0xFFU);
                BCM_MemCpy(&fwup_pkt[POT_OFFSET + 6U], &nanoseconds, sizeof(nanoseconds));

                /* Increment the sequence number */
                fwup_pkt[SEQNUM_OFFSET]++;
                /* Insert Follow Up */
                ETS_RxPacket(ets_config.intfCfg[SLAVE_INTF].hwPortNum,
                             fwup_pkt, sizeof(fwup_pkt), dst_mac_addr,
                             &time, FALSE);
            }
        }

        if (mask & TX_PKT_EVENT) {
            numPkts = CIRCQ_Pop(&txPktQ, (char*)pkts, TXPKTQ_SIZE);
            for (i = 0UL; i < numPkts; i++) {

                ETS_TxConfirmation(pkts[i].intfIdx, pkts[i].pktType,
                        &pkts[i].timestamp, pkts[i].timestampValid);
            }
            BCM_ClearEvent(TX_PKT_EVENT);
        }
    };

    globalStatus.magicId = ETS_GLBL_STATUS_MAGIC_ID;
    ret = ETS_GetGlobalStatus(&globalStatus);
    CHECK_ERR(ret,__LINE__);
    if (1UL == slaveMode) {
        if ((ETS_GMSTATUS_PRESENT != globalStatus.gmStatus) ||
            (ETS_CLOCKSTATE_UPDATE_SLAVE != globalStatus.clockState) ||
            (0UL == globalStatus.isSyncReceived) ||
            (ets_config.intfCfg[SLAVE_INTF].hwPortNum != globalStatus.slavePort)) {
            ULOG_ERR("Error: Invalid global state: gmStatus %lu clockState %lu "
                     "slavePort %lu isSyncReceived %lu\n",
                     globalStatus.gmStatus, globalStatus.clockState,
                     globalStatus.slavePort, globalStatus.isSyncReceived);
            etslib_qt_result = BCM_ERR_UNKNOWN;
            return;
        }
    } else {
        if ((ETS_GMSTATUS_PRESENT != globalStatus.gmStatus) ||
            (ETS_CLOCKSTATE_UPDATE_GM != globalStatus.clockState)) {
            ULOG_ERR("Error: Invalid global state: gmStatus %lu clockState %lu\n",
                     globalStatus.gmStatus, globalStatus.clockState);
            etslib_qt_result = BCM_ERR_UNKNOWN;
            return;
        }
    }

    for (i = 0UL ; i < ETS_MAX_INTERFACES; ++i) {
        if ((ETS_ADMINMODE_ENABLE == ets_config.intfCfg[i].asCapable) &&
            (0UL == InterfaceDown[i])) {
            ETS_IntfStatusType  intfStatus;

            intfStatus.magicId = ETS_INTF_STATUS_MAGIC_ID;
            ret = ETS_GetIntfStatus(ets_config.intfCfg[i].hwPortNum, &intfStatus);
            CHECK_ERR(ret,__LINE__);

            if (ETS_ROLE_MASTER == ets_config.intfCfg[i].role) {
                if ((ETS_INITLOGPDELAY_1S != intfStatus.currentLogPdelayInterval) ||
                    (ETS_INITLOGSYNC_31_25MS != intfStatus.currentLogSyncInterval)) {
                    ULOG_ERR("Error: Invalid interface state: "
                             "currentLogPdelayInterval %d "
                             "currentLogSyncInterval %d for interface %lu\n",
                             intfStatus.currentLogPdelayInterval,
                             intfStatus.currentLogSyncInterval, i);
                    etslib_qt_result = BCM_ERR_UNKNOWN;
                    return;
                }
            } else if (ETS_ROLE_SLAVE == ets_config.intfCfg[i].role) {
                if ((ETS_INITLOGPDELAY_1S != intfStatus.currentLogPdelayInterval) ||
                    (ETS_INITLOGSYNC_31_25MS != intfStatus.currentLogSyncInterval) ||
                    ((31250ULL * ets_config.intfCfg[i].syncReceiptTimeout) != intfStatus.syncReceiptTimeoutInterval) ||
                    (sync_pkt[SEQNUM_OFFSET] != intfStatus.syncLastRxSeqId) ||
                    (ETS_INITLOGSYNC_31_25MS != intfStatus.rxPduInterval)){
                    ULOG_ERR("Error: Invalid interface state: "
                             "currentLogPdelayInterval %d "
                             "currentLogSyncInterval %d syncReceiptTimeout %lu "
                             "syncLastRxSeqId %u rxPduInterval %d for "
                             "interface %lu\n",
                            intfStatus.currentLogPdelayInterval,
                            intfStatus.currentLogSyncInterval,
                            (uint32_t)intfStatus.syncReceiptTimeoutInterval,
                            intfStatus.syncLastRxSeqId,
                            intfStatus.rxPduInterval, i);
                    etslib_qt_result = BCM_ERR_UNKNOWN;
                    return;
                }
            }

            ret = ETS_IsGptpEventMsg(ets_config.intfCfg[i].hwPortNum,
                                     pdelay_req_pkt, sizeof(pdelay_req_pkt),
                                     &response);
            CHECK_ERR(ret, __LINE__);
            if (1UL != response) {
                ULOG_ERR("Error: Unable to recognize PDelay Request as event message\n");
                etslib_qt_result = BCM_ERR_UNKNOWN;
                return;
            }
        }
    }

    ret = ETS_GetGrandMasterRateRatio(&rateRatio);
    CHECK_ERR(ret,__LINE__);
    if (((1UL == slaveMode) && (rateRatio != 0x40000001)) ||
        ((0UL == slaveMode) && (rateRatio != 0x40000000))) {
        ULOG_ERR("Error: Invalid Grandmaster Rate Ratio: 0x%x\n", rateRatio);
        etslib_qt_result = BCM_ERR_UNKNOWN;
        return;
    }

    if (ETS_CLOCKMODE_GM == ets_config.clockMode) {
        time.seconds = 100000ULL;
        time.nanoseconds = 0UL;
        ret = ETS_SetGlobalTime(&time);
        CHECK_ERR(ret,__LINE__);
    }

    ret = ETS_GetCurrentTime(&time, &timestampState);
    CHECK_ERR(ret,__LINE__);

    if (1UL == slaveMode) {
        if(time.seconds < seconds) {
            ULOG_ERR("Error: Time: %lus \n", time.seconds);
            etslib_qt_result = BCM_ERR_UNKNOWN;
            return;
        }
    }else {
        if(time.seconds < 100000ULL) {
            ULOG_ERR("Error: Time: %lus \n", time.seconds);
            etslib_qt_result = BCM_ERR_UNKNOWN;
            return;
        }
    }

    if (timestampState != ETS_TIMESTAMPSTATE_SYNC) {
        ULOG_ERR("Error: Timestamp state: %lu\n", timestampState);
        etslib_qt_result = BCM_ERR_UNKNOWN;
        return;
    }

    for (i = 0UL ; i < ETS_MAX_INTERFACES; ++i) {
        ETS_StatsType stats;
        stats.magicId = ETS_STATS_MAGIC_ID;
        ret = ETS_GetStats(ets_config.intfCfg[i].hwPortNum, &stats);
        CHECK_ERR(ret,__LINE__);

        if ((ETS_ADMINMODE_ENABLE == ets_config.intfCfg[i].asCapable) &&
           (0UL == InterfaceDown[i])){
            if (ETS_ROLE_SLAVE == ets_config.intfCfg[i].role) {
                if(stats.syncRxCount == 0UL ||
                        stats.pDelayReqTxCount == 0UL ||
                        stats.pDelayRespTxCount == 0UL) {
                    ULOG_ERR("Error: Intf %lu not transmitting/receiving packets\n", i);
                    etslib_qt_result = BCM_ERR_UNKNOWN;
                    return;
                }
            } else if (ETS_ROLE_MASTER == ets_config.intfCfg[i].role) {
                if(stats.syncTxCount == 0UL ||
                        stats.pDelayReqTxCount == 0UL ||
                        stats.pDelayRespTxCount == 0UL) {
                    ULOG_ERR("Error: Intf %lu not transmitting/receiving packets\n", i);
                    etslib_qt_result = BCM_ERR_UNKNOWN;
                    return;
                }
            }
        }
    }

    ret = ETS_DeInit();
    CHECK_ERR(ret,__LINE__);

    ULOG_INFO("\n Test completed");
    if (BCM_AT_EXECUTING == etslib_qt_result) {
        etslib_qt_result = BCM_ERR_OK;
    }

    BCM_TerminateTask();
}
/** @} */

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if ((1UL == aIndex) || (2UL == aIndex)) {
        if (1UL == aIndex) {
            BCM_MemSet(InterfaceDown, 1U, sizeof(InterfaceDown));
            slaveMode = 0UL;
            ULOG_INFO("\n Test started in GM mode");
        } else {
            BCM_MemSet(InterfaceDown, 1U, sizeof(InterfaceDown));
            slaveMode = 1UL;
            ULOG_INFO("\n Test started in Slave mode");
        }
        etslib_qt_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(ETS_ProtocolTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if ((1UL == aIndex) || (2UL == aIndex)) {
        ret = etslib_qt_result;
    }
    return ret;
}


void ApplicationInit()
{
}
