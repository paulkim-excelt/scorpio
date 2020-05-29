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

#include <stdint.h>
#include <nif.h>
#include <utils.h>
#include <bcm_time.h>
#include <ets_osil.h>
#include <osil/bcm_osil.h>
#include <ee.h>
#include <ulog.h>

#define GetModuleLogLevel()  ULOG_LVL_INFO
#define SEQNUM_OFFSET        (31U)
#define POT_OFFSET           (34U)
#define ETS_MSG_TYPE_MASK                 0x0FU
#define FREE                 (0U)
#define ALLOCATED            (1U)
#define USED                 (2U)

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
                            0x00U, 0x01U, 0x00U, 0x00U,
                            0x00U, 0xFDU, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U};

static uint8_t fwup_pkt[]= {0x18U, 0x02U, 0x00U, 0x4CU,
                            0x00U, 0x00U, 0x00U, 0x08U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x00U, 0x00U,
                            0xAAU, 0xBBU, 0xCCU, 0xFFU,
                            0xFEU, 0xDDU, 0xEEU, 0xFFU,
                            0x00U, 0x01U, 0x00U, 0x00U,
                            0x02U, 0xFDU,
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

static const uint8_t src_mac_addr[] = {0x02U, 0x01U, 0x02U, 0x03U, 0x04U, 0x05U};

uint16_t syncSeqNum;

typedef struct _NIF_Time {
    int64_t seconds;
    int32_t nanoseconds;
} NIF_Time;

typedef struct _NIF_Buffer {
    uint8_t state;
    uint8_t buf[ETS_MAX_PDU_SIZE];
    NIF_CntrlIDType ctrlIdx;
} NIF_Buffer;

#define NUM_BUFFERS (4U * (ETS_MAX_INTERFACES))
static NIF_Buffer buffers[NUM_BUFFERS] = {{0U}};

static NIF_Time nifTime = {0U};

int32_t NIF_GetCtrlIdx(ETHERSWT_VLANIDType aVlanID,
                       uint16_t aCompID,
                       uint8_t aInstID,
                       NIF_CntrlIDType *const aCtrlIdx)
{
    *aCtrlIdx = 0ULL;
    return BCM_ERR_OK;
}

void ETHER_RxInd(ETHER_HwIDType aID, const uint8_t *const aBuf,
                 uint32_t aLen, const uint8_t *const aSrcMacAddr,
                 const uint8_t *const aDestMacAddr)
{
}

void ETHER_TxDoneInd(ETHER_HwIDType aID, uint32_t aBufIdx,
                     ETHER_ResultType aResult)
{
}

int32_t NIF_GetLinkState(uint32_t aXcvrIdx,
                         ETHXCVR_LinkStateType *const aLinkState)
{
    *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    return BCM_ERR_OK;
}

int32_t NIF_GetMacAddr(NIF_CntrlIDType aCtrlIdx,
                       uint8_t *const aMacAddr)
{
    uint8_t macAddr[ETHER_MAC_ADDR_SIZE] = {0x00U, 0x11U, 0x22U, 0x33U, 0x44U, 0x55U};
    BCM_MemCpy(aMacAddr, macAddr, ETHER_MAC_ADDR_SIZE);
    return BCM_ERR_OK;
}

int32_t NIF_SetTime(NIF_CntrlIDType aCtrlIdx,
                    const ETHER_TimestampType* const aTS)
{
    nifTime.seconds = aTS->seconds;
    nifTime.nanoseconds = aTS->nanoseconds;

    return BCM_ERR_OK;
}

int32_t NIF_GetTime(NIF_CntrlIDType aCtrlIdx,
                    ETHER_TimestampType* const aTS,
                    ETHER_TimestampQualType* const aTSQual)
{
    *aTSQual = ETHER_TIMESTAMPQUAL_VALID;

    nifTime.seconds += 5ULL;
    aTS->seconds = nifTime.seconds;
    aTS->nanoseconds = nifTime.nanoseconds;

    return BCM_ERR_OK;
}

int32_t NIF_SwtSetMgmtInfo(NIF_CntrlIDType aCtrlIdx,
                           uint32_t aBufIdx,
                           const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    return BCM_ERR_OK;
}

int32_t NIF_Send(NIF_CntrlIDType aCtrlIdx,
                 ETHER_EtherType aEtherType,
                 uint32_t aIsTxConfEnable,
                 uint32_t aBufIdx,
                 uint32_t aLen,
                 const uint8_t *const aDestMacAddr)
{
    if (aBufIdx < NUM_BUFFERS) {
        buffers[aBufIdx].state = USED;
    }
    else {
        ULOG_ERR("\n NIF_Send aBufIdx > NUM_BUFFERS  aBufIdx: %u", aBufIdx);
    }
    return BCM_ERR_OK;
}

int32_t NIF_GetTxBuffer(NIF_CntrlIDType aCtrlIdx,
                        ETHER_EtherType aEtherType,
                        ETHER_PrioType aPriority,
                        uint32_t *const aBufIdx,
                        uint8_t **const aBuf,
                        uint32_t *const aLenInOut)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_NOMEM;

    for (i = 0; i < NUM_BUFFERS; ++i) {
        if (buffers[i].state == FREE) {
            *aBufIdx        = i;
            *aBuf           = buffers[i].buf;
            buffers[i].state = ALLOCATED;
            buffers[i].ctrlIdx = aCtrlIdx;
            retVal = BCM_ERR_OK;
            break;
        }
    }
    if( retVal != BCM_ERR_OK) {
        ULOG_ERR("\n NIF_GetTxBuffer retVal BCM_ERR_NOMEM : %u", retVal);
    }
    return retVal;
}

int32_t NIF_SetCorrectionTime(NIF_CntrlIDType aCtrlIdx,
                              const ETHER_TimestampDiffType* const aTD,
                              const ETHER_RateRatioType* const aRR)
{
    return BCM_ERR_OK;
}

int32_t NIF_EnableTxTimestamp(NIF_CntrlIDType aCtrlIdx,
                              uint32_t aBufIdx)
{
    return BCM_ERR_OK;
}

void ETHERSWT_LinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
                                 ETHERSWT_PortIDType aPortID,
                                 ETHXCVR_LinkStateType aLinkState)
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

void ETHERSWT_MgmtInfoInd(ETHER_HwIDType aCntrlID,
                          const uint8_t *const aData,
                          const ETHERSWT_MgmtInfoType *const aMgmtInfo)


{
}

void ETHXCVR_LinkStateChangeInd(ETHXCVR_IDType aID,
                                ETHXCVR_LinkStateType aLinkState)
{
}

void ETHXCVR_ModeIndication(ETHXCVR_IDType aID, ETHXCVR_ModeType aMode)
{
}

void ResetSequenceNum()
{
    syncSeqNum                  = 0U;
    sync_pkt[SEQNUM_OFFSET - 1] = 0U;
    sync_pkt[SEQNUM_OFFSET]     = 0U;
    fwup_pkt[SEQNUM_OFFSET]     = 0U;
    fwup_pkt[SEQNUM_OFFSET - 1] = 0U;
}
void SendSyncPacket(BCM_TimeType *systemTime, ETHERSWT_MgmtInfoType *mgmtInfo)
{
    ETS_TimestampType      time;
    ETHER_TimestampType timestamp;
    ETHER_TimestampQualType timestampQual = ETHER_TIMESTAMPQUAL_VALID;
    time.seconds = systemTime->s;
    time.nanoseconds = systemTime->ns;
    /* Increment the sequence number */
    if(0UL == ++sync_pkt[SEQNUM_OFFSET]) {
        ++sync_pkt[SEQNUM_OFFSET - 1];
    }
    syncSeqNum = (sync_pkt[SEQNUM_OFFSET - 1] << 8) + sync_pkt[SEQNUM_OFFSET];
    /* Insert Sync */

    ETS_RxPktIndication(0ULL, sync_pkt, sizeof(sync_pkt), src_mac_addr, dst_mac_addr, 6UL);

    ETS_RxMgmtInfoIndication(0ULL, sync_pkt, mgmtInfo);

    timestamp.seconds = time.seconds + 5ULL;
    timestamp.nanoseconds = time.nanoseconds;
    ETS_RxPktTSIndication(0ULL, sync_pkt, mgmtInfo, &timestamp, &timestampQual);
}

void SendFollowUpPacket(BCM_TimeType *systemTime, ETHERSWT_MgmtInfoType *mgmtInfo)
{
    uint64_t               seconds = 0ULL;
    uint32_t               nanoseconds = 0UL;
    /* Set the POT */

    seconds = systemTime->s;
    nanoseconds = Host2BE32(systemTime->ns);
    fwup_pkt[POT_OFFSET] = (uint8_t)((seconds >> 40UL) & 0xFFU);
    fwup_pkt[POT_OFFSET + 1U] = (uint8_t)((seconds >> 32UL) & 0xFFU);
    fwup_pkt[POT_OFFSET + 2U] = (uint8_t)((seconds >> 24UL) & 0xFFU);
    fwup_pkt[POT_OFFSET + 3U] = (uint8_t)((seconds >> 16UL) & 0xFFU);
    fwup_pkt[POT_OFFSET + 4U] = (uint8_t)((seconds >> 8UL) & 0xFFU);
    fwup_pkt[POT_OFFSET + 5U] = (uint8_t)(seconds & 0xFFU);
    BCM_MemCpy(&fwup_pkt[POT_OFFSET + 6U], &nanoseconds, sizeof(nanoseconds));

    /* Increment the sequence number */
    if(0UL == ++fwup_pkt[SEQNUM_OFFSET]) {
        ++fwup_pkt[SEQNUM_OFFSET - 1];
    }
    /* Insert Follow Up */

    ETS_RxPktIndication(0ULL, fwup_pkt, sizeof(fwup_pkt), src_mac_addr, dst_mac_addr, 6UL);
    ETS_RxMgmtInfoIndication(0ULL, fwup_pkt, mgmtInfo);
}
TASK(TestCommsTask)
{
    uint32_t i;

    ETHER_TimestampType timestamp;
    ETHER_TimestampQualType timestampQual = ETHER_TIMESTAMPQUAL_VALID;
    BCM_TimeType systemTime;
    ETHERSWT_MgmtInfoType mgmtInfo;
    uint32_t               counter = 0UL;
    uint32_t               testTime = 20000UL;
    ResetSequenceNum();
    do {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_ClearEvent(SystemEvent0);
         counter ++;
         if ((counter % 40) == 0) {
             counter = 0;
             /* Insert a Sync + Follow Up on the slave port every 31.25ms */
             mgmtInfo.switchIdx = 0UL;
             mgmtInfo.PortIdx   = 0ULL;
             BCM_GetTime(&systemTime);
             SendSyncPacket(&systemTime, &mgmtInfo);
             SendFollowUpPacket(&systemTime, &mgmtInfo);
         }
         for (i = 0UL; i < NUM_BUFFERS; ++i) {
            if (buffers[i].state == USED) {
                ETS_TxPktConfirmation (buffers[i].ctrlIdx, i);
                BCM_GetTime(&systemTime);
                timestamp.seconds = systemTime.s;
                timestamp.nanoseconds = systemTime.ns;
                if(((buffers[i].buf[0U] & ETS_MSG_TYPE_MASK) == ETS_MSG_SYNC) ||
                    ((buffers[i].buf[0U] & ETS_MSG_TYPE_MASK) == ETS_MSG_PDELAY_REQ) ||
                    ((buffers[i].buf[0U] & ETS_MSG_TYPE_MASK) == ETS_MSG_PDELAY_RESP)) {
                        ETS_TxPktTSIndication(buffers[i].ctrlIdx, i, NULL, &timestamp, &timestampQual);
                }
                buffers[i].state = FREE;
            }
        }
    } while (--testTime);
    BCM_SetEvent(TestTask, SystemEvent0);
    (void)BCM_TerminateTask();
}

void PeriodicTimerAlarmCb(void)
{
    SetEvent(TestCommsTask, SystemEvent0);
}
