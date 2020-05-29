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
/**
    @defgroup grp_eth_cntlrdrv_impl Ethernet Controller Client Design
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_impl
    @{

    @file ethernet/drivers/controller/lib/ethernet.c
    @brief Ethernet Controller Client Design
    @version 1.26 Imported from docx
*/

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <interrupt.h>
#include <bcm_err.h>
#include <utils.h>
#include <ethernet.h>
#include <eth_osil.h>
#include "eth_cntlr_switch.h"
#include "eth_cntlr.h"

/**
    @name Ethernet API IDs
    @{
    @brief API IDs for Ethernet driver
*/
#define BRCM_SWDSGN_ETHER_TX_BUF_INFO_Q_SIZE_MACRO      () /**< @brief #ETHER_TX_BUF_INFO_Q_SIZE */
#define BRCM_SWDSGN_ETHER_BUF_INFO_TYPE                 () /**< @brief #ETHER_BufInfoType */
#define BRCM_SWDSGN_ETHER_TXBUFINFOQ_GLOBAL             () /**< @brief #ETHER_TxBufInfoQ */
#define BRCM_SWDSGN_ETHER_REPORTERROR_PROC              (0x80) /**< @brief #ETHER_ReportError */
#define BRCM_SWDSGN_ETHER_INIT_PROC                     (0x81) /**< @brief #ETHER_Init */
#define BRCM_SWDSGN_ETHER_DEINIT_PROC                   (0x82) /**< @brief #ETHER_DeInit */
#define BRCM_SWDSGN_ETHER_SETMODE_PROC                  (0x83) /**< @brief #ETHER_SetMode */
#define BRCM_SWDSGN_ETHER_GETMODE_PROC                  (0x84) /**< @brief #ETHER_GetMode */
#define BRCM_SWDSGN_ETHER_SETMACADDR_PROC               (0x85) /**< @brief #ETHER_SetMacAddr */
#define BRCM_SWDSGN_ETHER_GETMACADDR_PROC               (0x86) /**< @brief #ETHER_GetMacAddr */
#define BRCM_SWDSGN_ETHER_GETTXBUFFER_PROC              (0x87) /**< @brief #ETHER_GetTxBuffer */
#define BRCM_SWDSGN_ETHER_SEND_PROC                     (0x88) /**< @brief #ETHER_Send */
#define BRCM_SWDSGN_ETHER_ENABLETXTIMESTAMP_PROC        (0x89) /**< @brief #ETHER_EnableTxTimestamp */
#define BRCM_SWDSGN_ETHER_GETTXTIMESTAMP_PROC           (0x8A) /**< @brief #ETHER_GetTxTimestamp */
#define BRCM_SWDSGN_ETHER_GETRXTIMESTAMP_PROC           (0x8B) /**< @brief #ETHER_GetRxTimestamp */
#define BRCM_SWDSGN_ETHER_GETRXSTATS_PROC               (0x8C) /**< @brief #ETHER_GetRxStats */
#define BRCM_SWDSGN_ETHER_TIMEINIT_PROC                 (0x8D) /**< @brief #ETHER_TimeInit */
#define BRCM_SWDSGN_ETHER_TIMEINITSTATEHANDLER_PROC     (0x8E) /**< @brief #ETHER_TimeInitStateHandler */
#define BRCM_SWDSGN_ETHER_GETTIME_PROC                  (0x8F) /**< @brief #ETHER_GetTime */
#define BRCM_SWDSGN_ETHER_SETTIME_PROC                  (0x90) /**< @brief #ETHER_SetTime */
#define BRCM_SWDSGN_ETHER_SETCORRECTIONTIME_PROC        (0x91) /**< @brief #ETHER_SetCorrectionTime */
#define BRCM_SWDSGN_ETHER_ENABLELOOPBACK_PROC           (0x92) /**< @brief #ETHER_EnableLoopBack */
#define BRCM_SWDSGN_ETHER_GETRXPKT_PROC                 (0x93) /**< @brief #ETHER_GetRxPkt */
#define BRCM_SWDSGN_ETHER_GETTXPKT_PROC                 (0x94) /**< @brief #ETHER_GetTxPkt */
#define BRCM_SWDSGN_ETHER_PUTRXPKT_PROC                 (0x95) /**< @brief #ETHER_PutRxPkt */
#define BRCM_SWDSGN_ETHER_PUTTXPKT_PROC                 (0x96) /**< @brief #ETHER_PutTxPkt */
#define BRCM_SWDSGN_ETHER_SETGPTIMER_PROC               (0x97) /**< @brief #ETHER_SetGPTimer */
#define BRCM_SWDSGN_ETHER_PROCESSXMTDPKTS_PROC          (0x98) /**< @brief #ETHER_ProcessXmtdPkts */
#define BRCM_SWDSGN_ETHER_PROCESSRCVDPKTS_PROC          (0x99) /**< @brief #ETHER_ProcessRcvdPkts */
#define BRCM_SWDSGN_ETHER_PROCESSPKTS_PROC              (0x9A) /**< @brief #ETHER_ProcessPkts */
#define BRCM_SWDSGN_ETHER_CNTLRTXTSDONEIND_PROC         (0x9B) /**< @brief #ETHER_CntlrTxTSDoneInd */
#define BRCM_SWDSGN_ETHER_PROCESSTIMESTAMPS_PROC        (0x9C) /**< @brief #ETHER_ProcessTimestamps */
#define BRCM_SWDSGN_ETHER_MDIOREAD_PROC                 (0x9D) /**< @brief #ETHER_MdioRead */
#define BRCM_SWDSGN_ETHER_MDIOWRITE_PROC                (0x9D) /**< @brief #ETHER_MdioWrite */
#define BRCM_SWDSGN_ETHER_MDIOCHECKSTATUS_PROC          (0x9D) /**< @brief #ETHER_MdioCheckStatus */
/** @} */

/**
 * @{
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
#define ETHER_TX_BUF_INFO_Q_SIZE        (128UL)
/** @} */

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
typedef struct _ETHER_BufInfoType {
    uint8_t * buf;
    uint8_t * adaptBuf;
} ETHER_BufInfoType;

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static ETHER_BufInfoType ETHER_TxBufInfoQ[ETHER_TX_BUF_INFO_Q_SIZE] COMP_SECTION(".eth.data")= {{0}};

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_TIMEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_TIMEINITSTATEHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
static void ETHER_ReportError(uint32_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1,
        uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_AMC_ID & BCM_LOGMASK_USER),
                (uint8_t)aInstanceID, aApiID, aErr, 4UL, values);
}

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
void ETHER_Init(ETHER_HwIDType aID, const ETHER_CfgType *const aConfig)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.ctrlCfg = aConfig;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_INIT, &ethIO);
    if (BCM_ERR_OK != retVal) {
        ETHER_ReportError(aID, BRCM_SWARCH_ETHER_INIT_PROC, retVal,
                        0UL, 0UL, 0UL, __LINE__);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
int32_t ETHER_DeInit(ETHER_HwIDType aID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_DEINIT, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
int32_t ETHER_SetMode(ETHER_HwIDType aID, ETHER_ModeType aMode)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.mode = aMode;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_SET_MODE, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
int32_t ETHER_GetMode(ETHER_HwIDType aID, ETHER_ModeType *const aMode)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_MODE, &ethIO);
    if (BCM_ERR_OK == retVal) {
        *aMode = ethIO.mode;
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
int32_t ETHER_SetMacAddr(ETHER_HwIDType aID, const uint8_t *const aMacAddr)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.macAddrIn = aMacAddr;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_SET_MACADDR, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_INITIALISATION
 */
int32_t ETHER_GetMacAddr(ETHER_HwIDType aID, uint8_t *const aMacAddr)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.macAddrOut = aMacAddr;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_MACADDR, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
int32_t ETHER_GetTxBuffer(ETHER_HwIDType aID, ETHER_PrioType aPriority,
                          uint32_t *const aBufIdx, uint8_t **const aBuf,
                          uint32_t *const aLenInOut)
{
    ETHER_IOType ethIO;
    int32_t retVal;
    uint32_t len;

    if ((NULL == aBuf) || (NULL == aBufIdx) || (NULL == aLenInOut)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        len = *aLenInOut;
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.priority = aPriority;
        ethIO.buffIdxInOut = aBufIdx;
        ethIO.buf = NULL;
        ethIO.lenInOut = &len;

        retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_BUF, &ethIO);
        if (BCM_ERR_OK == retVal) {
            *aBuf = ethIO.buf;
            ETHER_TxBufInfoQ[*aBufIdx].buf = *aBuf;
            retVal = ETHER_CntlrSwtTxAdaptBuffer(aID, *aBufIdx, aBuf, &len);
            ETHER_TxBufInfoQ[*aBufIdx].adaptBuf = *aBuf;
            if (len < *aLenInOut) {
                *aLenInOut = len;
            }
        }
        if (BCM_ERR_OK != retVal) {
            *aLenInOut = 0x0UL;
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
int32_t ETHER_Send(ETHER_HwIDType aID, uint32_t aTxConfirmation,
                   uint32_t aBufIdx, uint32_t aLen,
                   const uint8_t *const aDestMacAddr)
{
    int32_t retVal = BCM_ERR_OK;
    int32_t err = BCM_ERR_OK;
    uint8_t * pktBuf = NULL;
    ETHER_IOType ethIO;
    ETHER_TimeIOType ethTimeIO;
    uint32_t pktLen = aLen;

    /* aLen = 0UL means packet would be freed. This packet
     * wouldn't be transmitted on line, so there is no meaning
     * of transmit confirmation. Client if waiting on this
     * transmit confirmation should timeout as this packet would
     * be freed and there is not transmit confirmation */
    if ((0UL == aLen) && (TRUE == aTxConfirmation)) {
        err = BCM_ERR_INVAL_PARAMS;
    }

    /* If aLen equal to zero, just free the packet */
    if (0UL == aLen) {
        retVal = ETHER_CntlrSwtTxProcessFrame(aID, aBufIdx, NULL, &pktLen);
    } else {
        if (pktLen < (ETHER_MINFRMSIZENOCRC - (2UL * ETHER_MAC_ADDR_SIZE))) {
            pktLen =  (ETHER_MINFRMSIZENOCRC - (2UL * ETHER_MAC_ADDR_SIZE));
        }
        pktBuf = ETHER_TxBufInfoQ[aBufIdx].buf;
        retVal = ETHER_CntlrSwtTxProcessFrame(aID, aBufIdx, &pktBuf, &pktLen);
    }

    /* If transmit confirmation is not enabled or if aLen equals to Zero,
     * no need to process for timestamp */
    if ((1UL == aTxConfirmation) && (0UL != aLen)) {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.buffIdxInOut = &aBufIdx;

        retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_IS_TX_PKT_TS_MARKED, &ethIO);
        if (BCM_ERR_OK == retVal) {
            pktBuf = ETHER_TxBufInfoQ[aBufIdx].adaptBuf;
            ethTimeIO.retVal = BCM_ERR_UNKNOWN;
            ethTimeIO.hwID = aID;
            ethTimeIO.buffIdxInOut = &aBufIdx;
            ethTimeIO.pktBuf = pktBuf;

            retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_UPDATE_EGRESS_PKT_INFO,
                                            &ethTimeIO);
        } else {
            /* Following are reasons to reach here:
             *  1. If this is not a valid buffer, ignore the
             *     error and proceed. If there is something terribly
             *     wrong with the buffer like invalid bufIdx or
             *     channel then the send routine will catch it and
             *     throw an error.
             *  2. gPTP general message, no timestamp required.
             */
            retVal = BCM_ERR_OK;
        }

        if (BCM_ERR_OK == retVal) {
            ethIO.retVal = BCM_ERR_UNKNOWN;
            ethIO.hwID = aID;
            ethIO.txConfirmation = aTxConfirmation;
            ethIO.buffIdxInOut = &aBufIdx;
            ethIO.lenInOut = &pktLen;
            ethIO.macAddrIn = aDestMacAddr;

            retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_SEND, &ethIO);
            if (BCM_ERR_OK != retVal) {
                ethTimeIO.retVal = BCM_ERR_UNKNOWN;
                ethTimeIO.hwID = aID;
                ethTimeIO.buffIdxInOut = &aBufIdx;
                ethTimeIO.portIdx = 0x0;

                retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_DISABLE_EGRESS_TIMESTAMP,
                                            &ethTimeIO);
            }
        }
    } else {
        /* Clear the timestamp flag, if aLen = 0 or aTxConfirmation = FALSE  */
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.buffIdxInOut = &aBufIdx;

        retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_IS_TX_PKT_TS_MARKED, &ethIO);
        if (BCM_ERR_OK == retVal) {
            ethTimeIO.retVal = BCM_ERR_UNKNOWN;
            ethTimeIO.hwID = aID;
            ethTimeIO.buffIdxInOut = &aBufIdx;
            ethTimeIO.portIdx = 0x0;

            retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_DISABLE_EGRESS_TIMESTAMP,
                    &ethTimeIO);
            if (BCM_ERR_OK != retVal) {
                ETHER_ReportError(aID, BRCM_SWARCH_ETHER_SEND_PROC, retVal,
                        0UL, 0UL, 0UL, __LINE__);
            }
        }
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.txConfirmation = aTxConfirmation;
        ethIO.buffIdxInOut = &aBufIdx;
        ethIO.lenInOut = &pktLen;
        ethIO.macAddrIn = aDestMacAddr;

        retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_SEND, &ethIO);
    }

    /* Handle the case of aLen = 0 and aTxConfirmation = TRUE */
    if (BCM_ERR_OK != err) {
        retVal = err;
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_ENABLETXTIMESTAMP_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_EnableTxTimestamp(ETHER_HwIDType aID, uint32_t aBufIdx)
{
    ETHER_IOType ethIO;
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ETHER_HW_ID_MAX > aID) {
        ethIO.retVal = BCM_ERR_UNKNOWN;
        ethIO.hwID = aID;
        ethIO.buffIdxInOut = &aBufIdx;

        retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_TX_MARK_TS_PKT, &ethIO);
        if(BCM_ERR_OK == retVal) {
            ethTimeIO.retVal = BCM_ERR_UNKNOWN;
            ethTimeIO.hwID = aID;
            ethTimeIO.buffIdxInOut = &aBufIdx;
            /* TODO: For controller port it is assumed the time hardware
             * port index would be zero. It would be nice to have this
             * information as a config parameter */
            ethTimeIO.portIdx = 0x0;

            retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_ENABLE_EGRESS_TIMESTAMP,
                                        &ethTimeIO);
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXTIMESTAMP_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_GetTxTimestamp(ETHER_HwIDType aID, uint32_t aBufIdx,
                             ETHER_TimestampType* const aTS,
                             ETHER_TimestampQualType* const aTSQual)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.buffIdxInOut = &aBufIdx;
    /* TODO: For controller port it is assumed the time hardware
     * port index would be zero. It would be nice to have this
     * information as a config parameter */
    ethTimeIO.portIdx = 0x0;
    ethTimeIO.timestamp = aTS;
    ethTimeIO.tsQuality = aTSQual;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_GET_EGRESS_TIMESTAMP,
            &ethTimeIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETRXTIMESTAMP_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_GetRxTimestamp(ETHER_HwIDType aID, const uint8_t* const aPktBuf,
                             ETHER_TimestampType* const aTS,
                             ETHER_TimestampQualType* const aTSQual)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.pktBuf = aPktBuf;
    /* TODO: For controller port it is assumed the time hardware
     * port index would be zero. It would be nice to have this
     * information as a config parameter */
    ethTimeIO.portIdx = 0x0;
    ethTimeIO.timestamp = aTS;
    ethTimeIO.tsQuality = aTSQual;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_GET_INGRESS_TIMESTAMP,
            &ethTimeIO);
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
int32_t ETHER_GetRxStats(ETHER_HwIDType aID, ETHER_RxStatsType *const aStats)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.stats = aStats;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_STATS, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_TIMEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
void ETHER_TimeInit(ETHER_HwIDType aID,
                    ETHER_TimeCfgType *const aCfg)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.timeCfg = aCfg;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_TIME_INIT, &ethTimeIO);
    if (BCM_ERR_OK != retVal) {
        ETHER_ReportError(aID, BRCM_SWARCH_ETHER_TIMEINIT_PROC,
                        retVal, 0UL, 0UL, 0UL, __LINE__);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_TIMEINITSTATEHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
void ETHER_TimeInitStateHandler(ETHER_HwIDType aID,
                    ETHER_TimeInitCmdType aCmd,
                    uint64_t aSysTimeDiffNs,
                    ETHER_TimeInitOutType *const aOutput)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;
    ETHER_TimeInitOutType timeOut;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.initCmd = aCmd;
    ethTimeIO.timeVal = &(timeOut);
    ethTimeIO.sysTimeDiffNs = aSysTimeDiffNs;

    do {
        retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_TIME_INIT_STATE_HANDLER,
                &ethTimeIO);
    } while (BCM_ERR_EAGAIN == retVal);
    if (BCM_ERR_OK == retVal) {
        if (aOutput != NULL) {
            BCM_MemCpy(aOutput, &(timeOut), sizeof(ETHER_TimeInitOutType));
        }
    } else {
        ETHER_ReportError(aID, BRCM_SWARCH_ETHER_TIMEINITSTATEHANDLER_PROC,
                        retVal, 0UL, 0UL, 0UL, __LINE__);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETTIME_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_GetTime(ETHER_HwIDType aID,
                      ETHER_TimestampType* const aTS,
                      ETHER_TimestampQualType* const aTSQual)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.timestamp = aTS;
    ethTimeIO.tsQuality = aTSQual;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_GET_TIME, &ethTimeIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETTIME_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_SetTime(ETHER_HwIDType aID, const ETHER_TimestampType* const aTS)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.roTimestamp = aTS;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_SET_TIME, &ethTimeIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETCORRECTIONTIME_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
int32_t ETHER_SetCorrectionTime(ETHER_HwIDType aID,
                                const ETHER_TimestampDiffType* const aTD,
                                const ETHER_RateRatioType*     const aRR)
{
    ETHER_TimeIOType ethTimeIO;
    int32_t retVal;

    ethTimeIO.retVal = BCM_ERR_UNKNOWN;
    ethTimeIO.hwID = aID;
    ethTimeIO.tsDiff = aTD;
    ethTimeIO.roRateRatio = aRR;

    retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_SET_CORRECTION_TIME,
            &ethTimeIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_MDIOREAD_PROC
 * @trace #BRCM_SWREQ_ETHER_MDIO
 */
int32_t ETHER_MdioRead(MDIO_HwIDType aID,
                    MDIO_PktType *aPkt,
                    MDIO_JobIDType *aJobID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.mdioHwID = aID;
    ethIO.mdioPkt = aPkt;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_MDIO_READ, &ethIO);
    if (BCM_ERR_OK == retVal) {
        *aJobID = ethIO.mdioJobId;
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_MDIOWRITE_PROC
 * @trace #BRCM_SWREQ_ETHER_MDIO
 */
int32_t ETHER_MdioWrite(MDIO_HwIDType aID,
                        MDIO_PktType *aPkt,
                        MDIO_JobIDType *aJobID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.mdioHwID = aID;
    ethIO.mdioPkt = aPkt;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_MDIO_WRITE, &ethIO);
    if (BCM_ERR_OK == retVal) {
        *aJobID = ethIO.mdioJobId;
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_MDIOCHECKSTATUS_PROC
 * @trace #BRCM_SWREQ_ETHER_MDIO
 */
int32_t ETHER_MdioCheckStatus(MDIO_HwIDType aID,
                            MDIO_JobIDType aJobID,
                            MDIO_PktType *aPkt)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.mdioHwID = aID;
    ethIO.mdioPkt = aPkt;
    ethIO.mdioJobId = aJobID;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_MDIO_CHECKSTATUS, &ethIO);
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
void ETHER_EnableLoopBack(ETHER_HwIDType aID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.hwID = aID;
    ethIO.retVal = BCM_ERR_UNKNOWN;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_ENABLE_LOOP_BACK, &ethIO);
    if (BCM_ERR_OK != retVal) {
        ETHER_ReportError(aID, BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC,
                        retVal, 0UL, 0UL, 0UL, __LINE__);
    }
    return;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static int32_t ETHER_GetRxPkt(ETHER_HwIDType aID,
                    uint32_t * const aBufIdx,
                    uint8_t ** const aPkt,
                    uint32_t * const aRaiseRxCb,
                    uint32_t * const aPktLen)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.buffIdxInOut = aBufIdx;
    ethIO.buf = NULL;
    ethIO.lenInOut = aPktLen;
    ethIO.raiseCb = aRaiseRxCb;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_RX_PKT, &ethIO);
    if (BCM_ERR_OK == retVal) {
        *aPkt = ethIO.buf;
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static int32_t ETHER_GetTxPkt(ETHER_HwIDType aID,
                        uint32_t * const aTxBufIdx,
                        uint32_t * const aRaiseTxCb)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.buffIdxInOut = aTxBufIdx;
    ethIO.raiseCb = aRaiseTxCb;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_GET_TX_PKT, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static void ETHER_PutRxPkt(ETHER_HwIDType aID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_PUT_RX_PKT, &ethIO);
    if (BCM_ERR_OK != retVal) {
        ETHER_ReportError(aID, BRCM_SWDSGN_ETHER_PUTRXPKT_PROC,
                        retVal, 0UL, 0UL, 0UL, __LINE__);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static int32_t ETHER_PutTxPkt(ETHER_HwIDType aID)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_PUT_TX_PKT, &ethIO);

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWREQ_ETHER_GPTIME
 */
int32_t ETHER_SetGPTimer(ETHER_HwIDType aID,
                         uint32_t aPeriod)
{
    ETHER_IOType ethIO;
    int32_t retVal;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.timerPeriod = aPeriod;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_SET_GPTIMER, &ethIO);
    if (BCM_ERR_OK != retVal) {
        ETHER_ReportError(aID, BRCM_SWARCH_ETHER_SETGPTIMER_PROC,
                            retVal, aPeriod, 0UL, 0UL, __LINE__);
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static int32_t ETHER_ProcessXmtdPkts(ETHER_HwIDType aID)
{
    int32_t retVal;
    uint32_t buffIdx = 0UL;
    uint32_t raiseTxCb = FALSE;

    retVal = ETHER_GetTxPkt(aID, &buffIdx, &raiseTxCb);
    if ((BCM_ERR_OK == retVal)
        && (TRUE == raiseTxCb)) {
        ETHER_TxDoneInd(aID, buffIdx, ETHER_RESULT_SUCCESS);
        retVal = ETHER_CntlrSwtTxDoneInd(aID, buffIdx);
        if (BCM_ERR_OK == retVal) {
            retVal = ETHER_PutTxPkt(aID);
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
static int32_t ETHER_ProcessRcvdPkts(ETHER_HwIDType aID)
{
    int32_t retVal;
    uint8_t *rxPkt;
    uint32_t bufIdx;
    uint32_t raiseRxCb = FALSE;
    uint32_t rxPktLen = 0UL;
    uint8_t * buf;
    uint32_t isMgmtFrame;
    uint32_t lineNo = __LINE__;

    retVal = ETHER_GetRxPkt(aID, &bufIdx, &rxPkt, &raiseRxCb, &rxPktLen);
    lineNo = __LINE__;
    if ((BCM_ERR_OK == retVal)
        && (TRUE == raiseRxCb)) {
        buf = &rxPkt[2UL * ETHER_MAC_ADDR_SIZE];
        rxPktLen -= (2UL * ETHER_MAC_ADDR_SIZE);
        retVal = ETHER_CntlrSwtRxProcessFrame(aID, bufIdx, &buf,
                &rxPktLen, &isMgmtFrame);
        lineNo = __LINE__;
        if (BCM_ERR_OK == retVal) {
            ETHER_RxInd(aID, buf, rxPktLen,
                    &rxPkt[ETHER_MAC_ADDR_SIZE], rxPkt);
            ETHER_PutRxPkt(aID);
            retVal = ETHER_CntlrSwtRxDoneInd(aID, bufIdx);
            lineNo = __LINE__;
        }
    }

    if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            ETHER_ReportError(aID, BRCM_SWDSGN_ETHER_PROCESSRCVDPKTS_PROC,
                retVal, 0UL, (uint32_t)BRCM_SWDSGN_ETHER_PROCESSRCVDPKTS_PROC,
                0UL, lineNo);
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_COMMUNICATION
 */
void ETHER_ProcessPkts(ETHER_HwIDType aID)
{
    int32_t retVal;

    do {
        retVal = ETHER_ProcessXmtdPkts(aID);
        if ((BCM_ERR_EAGAIN != retVal) && (BCM_ERR_OK != retVal)) {
            ETHER_ReportError(aID, BRCM_SWARCH_ETHER_PROCESSPKTS_PROC,
                retVal, 0UL, 0UL, 0UL, __LINE__);
        }
    } while (BCM_ERR_OK == retVal);

    do {
        retVal = ETHER_ProcessRcvdPkts(aID);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            ETHER_ReportError(aID, BRCM_SWARCH_ETHER_PROCESSPKTS_PROC,
                    retVal, 0UL, 0UL, 0UL, __LINE__);
        }
    } while (BCM_ERR_OK == retVal);

}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
static void ETHER_CntlrTxTSDoneInd(ETHER_HwIDType aID,
                             uint32_t aBufIdx)
{
    int32_t retVal;
    ETHER_IOType ethIO;
    uint32_t lineNo = __LINE__;

    ethIO.retVal = BCM_ERR_UNKNOWN;
    ethIO.hwID = aID;
    ethIO.buffIdxInOut = &aBufIdx;

    retVal = ETHER_SysCmdReq(ETHER_CNTLRIOCMD_TX_TS_DONE_IND, &ethIO);
    lineNo = __LINE__;
    if (BCM_ERR_OK == retVal) {
            ETHER_TxDoneInd(aID, aBufIdx, ETHER_RESULT_SUCCESS);
            retVal = ETHER_CntlrSwtTxDoneInd(aID, aBufIdx);
            lineNo = __LINE__;
    }
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
        ETHER_ReportError((uint8_t)aID,
                BRCM_SWDSGN_ETHER_CNTLRTXTSDONEIND_PROC,
                retVal, aBufIdx, 0UL, 0UL, lineNo);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
void ETHER_ProcessTimestamps(uint32_t aID)
{
    int32_t retVal;
    uint32_t bufIdx;
    ETHER_TimeIOType ethTimeIO;

    do {
        ethTimeIO.retVal = BCM_ERR_UNKNOWN;
        ethTimeIO.hwID = aID;
        ethTimeIO.buffIdxInOut = &bufIdx;

        retVal = ETHER_TimeSysCmdReq(ETHER_TIMEIOCMD_PROCESS_TS,
                                    &ethTimeIO);
        if (BCM_ERR_OK == retVal) {
            ETHER_CntlrTxTSDoneInd(aID, bufIdx);
        }
    } while (BCM_ERR_OK == retVal);
}
/** @} */
/* Nothing past this line */
