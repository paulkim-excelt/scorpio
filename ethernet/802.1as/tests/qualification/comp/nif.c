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

#include <stdint.h>
#include <nif.h>
#include <utils.h>
#include <bcm_time.h>
#include <ets_osil.h>
#include <osil/bcm_osil.h>
#include <ee.h>

const ETHER_TimeCfgType *const ETHER_TimeCfg = NULL;
const uint32_t ETHER_TimeCfgSize = 0UL;

typedef struct _NIF_Buffer {
    uint8_t used;
    uint8_t buf[ETS_MAX_PDU_SIZE];
} NIF_Buffer;

#define NUM_BUFFERS 5U
static NIF_Buffer buffers[NUM_BUFFERS] = {{0U}};

static int64_t seconds= 0LL;
static int32_t nanoseconds= 0L;

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
    if (aXcvrIdx <= 1UL) {
        *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
    } else {
        *aLinkState = ETHXCVR_LINKSTATE_DOWN;
    }

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
    seconds = aTS->seconds;
    nanoseconds = aTS->nanoseconds;

    return BCM_ERR_OK;
}

int32_t NIF_GetTime(NIF_CntrlIDType aCtrlIdx,
                    ETHER_TimestampType* const aTS,
                    ETHER_TimestampQualType* const aTSQual)
{
    *aTSQual = ETHER_TIMESTAMPQUAL_VALID;

    aTS->seconds = seconds + 5ULL;
    aTS->nanoseconds = nanoseconds;

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
        buffers[aBufIdx].used = 2U;
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
        if (buffers[i].used == 0U) {
            *aBufIdx        = i;
            *aBuf           = buffers[i].buf;
            buffers[i].used = 1U;
            retVal = BCM_ERR_OK;
            break;
        }
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

TASK(TestCommsTask)
{
    uint32_t i;
    ETHER_TimestampType timestamp;
    ETHER_TimestampQualType timestampQual = ETHER_TIMESTAMPQUAL_VALID;
    BCM_TimeType systemTime;

    do {
        (void)BCM_WaitEvent(SystemEvent0);
        (void)BCM_ClearEvent(SystemEvent0);

        for (i = 0UL; i < NUM_BUFFERS; ++i) {
            if (buffers[i].used == 2U) {
                buffers[i].used = 0U;

                ETS_TxPktConfirmation (0ULL, i);

                BCM_GetTime(&systemTime);
                timestamp.seconds = systemTime.s;
                timestamp.nanoseconds = systemTime.ns;
                ETS_TxPktTSIndication(0ULL, i, NULL, &timestamp, &timestampQual);
            }
        }
    } while (1U);
    (void)BCM_TerminateTask();
}

void PeriodicTimerAlarmCb(void)
{
    SetEvent(TestCommsTask, SystemEvent0);
}
