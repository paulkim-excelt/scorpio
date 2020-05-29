/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_eth_cntlrdrv_cntlr_buf_impl Ethernet Controller Buffer
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_cntlr_buf_impl
    @{

    @file ethernet/drivers/controller/lib/eth_cntlr_buf.c
*/

#include <string.h>
#include <atomic.h>
#include <utils.h>
#include "eth_cntlr.h"
#include "eth_cntlr_buf.h"

/**
    @name Ethernet controller buffer manager interfce IDs
    @{
    @brief Interface IDs for Ethernet controller driver buffer manager
*/
#define BRCM_SWDSGN_ETHER_CNTLRDEINITTXBUF_PROC         (0xAFU)  /**< @brief #ETHER_CntlrDeInitTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRINITTXBUF_PROC           (0xB0U)  /**< @brief #ETHER_CntlrInitTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRPEEKTXDEQUEUEBUF_PROC    (0xB1U)  /**< @brief #ETHER_CntlrPeekTxDequeueBuf */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUETXBUF_PROC        (0xB2U)  /**< @brief #ETHER_CntlrDequeueTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRDEALLOCTXBUF_PROC        (0xB3U)  /**< @brief #ETHER_CntlrDeallocTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRGETDEQUEUEDTXBUF_PROC    (0xB4U)  /**< @brief #ETHER_CntlrGetDequeuedTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRALLOCTXBUF_PROC          (0xB5U)  /**< @brief #ETHER_CntlrAllocTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRFREETXBUF_PROC           (0xB6U)  /**< @brief #ETHER_CntlrFreeTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRQUEUETXBUF_PROC          (0xB7U)  /**< @brief #ETHER_CntlrQueueTxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRDEINITRXBUF_PROC         (0xB8U)  /**< @brief #ETHER_CntlrDeInitRxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRINITRXBUF_PROC           (0xB9U)  /**< @brief #ETHER_CntlrInitRxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRPEEKRXDEQUEUEBUF_PROC    (0xBAU)  /**< @brief #ETHER_CntlrPeekRxDequeueBuf */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUERXBUF_PROC        (0xBBU)  /**< @brief #ETHER_CntlrDequeueRxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRDEALLOCRXBUF_PROC        (0xBCU)  /**< @brief #ETHER_CntlrDeallocRxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRQUEUERXBUF_PROC          (0xBDU)  /**< @brief #ETHER_CntlrQueueRxBuf */
#define BRCM_SWDSGN_ETHER_CNTLRGETDEQUEUEDRXBUF_PROC    (0xBEU)  /**< @brief #ETHER_CntlrGetDequeuedRxBuf */
/** @} */

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEINITTXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
void ETHER_CntlrDeInitTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo)
{
    uint32_t i;

    /* Reset channel info */
    aTxBufMgrInfo->allocIdx = 0UL;
    aTxBufMgrInfo->deallocIdx = 0UL;
    aTxBufMgrInfo->queueIdx = 0UL;
    aTxBufMgrInfo->dequeueIdx = 0UL;

    /* Reset all the buffers */
    for (i = 0UL; i < aTxBufMgrInfo->pktBuffCnt; i++) {
        aTxBufMgrInfo->pktBuffInfo[i].state = ETHER_PKTBUFFSTATE_FREE;
        aTxBufMgrInfo->pktBuffInfo[i].pktBuff = NULL;
        aTxBufMgrInfo->pktBuffInfo[i].flags = 0UL;
    }
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRINITTXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
void ETHER_CntlrInitTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo,
                        ETHER_TxPktType *const aTxChanPkt)
{
    uint32_t i;

    /* Set channel info */
    aTxBufMgrInfo->allocIdx = 0UL;
    aTxBufMgrInfo->deallocIdx = 0UL;
    aTxBufMgrInfo->queueIdx = 0UL;
    aTxBufMgrInfo->dequeueIdx = 0UL;

    /* Set all the buffers */
    for (i = 0UL; i < aTxBufMgrInfo->pktBuffCnt; i++) {
        aTxBufMgrInfo->pktBuffInfo[i].state = ETHER_PKTBUFFSTATE_FREE;
        aTxBufMgrInfo->pktBuffInfo[i].pktBuff = aTxChanPkt[i].pktBuff;
        aTxBufMgrInfo->pktBuffInfo[i].flags = 0UL;
    }
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRPEEKTXDEQUEUEBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
uint32_t ETHER_CntlrPeekTxDequeueBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo)
{
    return aTxBufMgrInfo->dequeueIdx;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEQUEUETXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrDequeueTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo)
{
    uint32_t dequeueIdx;
    uint32_t buffIdx;
    uint32_t volatile *state;
    int32_t retVal = BCM_ERR_OK;

    dequeueIdx = aTxBufMgrInfo->dequeueIdx;

    buffIdx = aTxBufMgrInfo->buffIdxQ[dequeueIdx];
    state = &(aTxBufMgrInfo->pktBuffInfo[buffIdx].state);
    if (ETHER_PKTBUFFSTATE_QUEUED !=
            ATOMIC_CheckUpdate(state, ETHER_PKTBUFFSTATE_QUEUED,
                ETHER_PKTBUFFSTATE_DEQUEUED)) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        dequeueIdx = ModInc(dequeueIdx, (aTxBufMgrInfo->descCnt));
        aTxBufMgrInfo->dequeueIdx = dequeueIdx;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEALLOCTXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrDeallocTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo)
{
    uint32_t buffIdx;
    uint32_t deallocIdx;
    uint32_t volatile *state;
    int32_t retVal = BCM_ERR_OK;

    deallocIdx = aTxBufMgrInfo->deallocIdx;
    buffIdx = aTxBufMgrInfo->buffIdxQ[deallocIdx];

    aTxBufMgrInfo->pktBuffInfo[buffIdx].flags &= ~ETHER_PKTFLAGS_CBEN;

    state = &(aTxBufMgrInfo->pktBuffInfo[buffIdx].state);
    if (ETHER_PKTBUFFSTATE_DEQUEUED != ATOMIC_CheckUpdate(state,
                                    ETHER_PKTBUFFSTATE_DEQUEUED,
                                    ETHER_PKTBUFFSTATE_FREE)) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        aTxBufMgrInfo->deallocIdx = ModInc(deallocIdx, aTxBufMgrInfo->descCnt);
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRGETDEQUEUEDTXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrGetDequeuedTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo,
                                  uint32_t *const aBufIdx,
                                  uint32_t *const aFlag)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t dequeueIdx;
    uint32_t deallocIdx;

    dequeueIdx = aTxBufMgrInfo->dequeueIdx;
    deallocIdx = aTxBufMgrInfo->deallocIdx;

    /* If (deallocIdx == dequeueIdx):
     *      If channel state = ACTIVE:
     *          Channel no new buffer transmitted.
     *      If channel state = UNINT:
     *          Channel still to be initialised or previously deinitialised.
     */

    if (deallocIdx == dequeueIdx) {
        retVal = BCM_ERR_EAGAIN;
        goto err;
    }

    *aBufIdx = aTxBufMgrInfo->buffIdxQ[deallocIdx];

    if (ETHER_PKTBUFFSTATE_DEQUEUED !=
            aTxBufMgrInfo->pktBuffInfo[*aBufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }

    *aFlag = aTxBufMgrInfo->pktBuffInfo[*aBufIdx].flags;

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRALLOCTXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrAllocTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo,
                            uint32_t *const aBufIdx,
                            uint8_t **const aBuf)
{
    int32_t retVal;
    uint32_t i;
    uint32_t allocIdx;
    uint32_t volatile *state;
    uint32_t isExclusive = FALSE;
    uint32_t bufState;

    /* allocIdx point to probable to be allocated buffer index */
    allocIdx = aTxBufMgrInfo->allocIdx;

    /* Search for free buffer */
    i = allocIdx;
    do {
        if ((aTxBufMgrInfo->pktBuffInfo[i].flags & ETHER_PKTFLAGS_1588TXCONF)
                != ETHER_PKTFLAGS_1588TXCONF) {
            state = &(aTxBufMgrInfo->pktBuffInfo[i].state);
            bufState = ATOMIC_CheckUpdate(state, ETHER_PKTBUFFSTATE_FREE,
                    ETHER_PKTBUFFSTATE_ALLOC);
            if (ETHER_PKTBUFFSTATE_FREE == bufState) {
                isExclusive = TRUE;
                break;
            } else {
                isExclusive = FALSE;
            }
        }
        i = ModInc(i, aTxBufMgrInfo->pktBuffCnt);
    } while (i != allocIdx);

    if (TRUE == isExclusive) {
        /* Increment the allocIdx */
        aTxBufMgrInfo->allocIdx = ModInc(allocIdx, aTxBufMgrInfo->pktBuffCnt);

        /* Free buffer allocated */
        *aBufIdx = i;
        *aBuf = aTxBufMgrInfo->pktBuffInfo[i].pktBuff;
        retVal = BCM_ERR_OK;
    } else {
        /* No free buffer */
        *aBufIdx = ETHER_INVAL_BUFF_IDX;
        *aBuf = NULL;
        retVal = BCM_ERR_NOMEM;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRFREETXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrFreeTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo,
                        uint32_t aBufIdx)
{
    int32_t retVal;
    uint32_t volatile *state;

    aTxBufMgrInfo->pktBuffInfo[aBufIdx].flags = 0UL;
    state = &(aTxBufMgrInfo->pktBuffInfo[aBufIdx].state);
    if (ETHER_PKTBUFFSTATE_ALLOC != ATOMIC_CheckUpdate(state,
                ETHER_PKTBUFFSTATE_ALLOC,
                ETHER_PKTBUFFSTATE_FREE)) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRQUEUETXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrQueueTxBuf(ETHER_TxBufMgrInfoType *const aTxBufMgrInfo,
                        uint32_t aBufIdx,
                        uint8_t **const aBuf,
                        uint32_t aTxConfirmation)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t volatile *state;
    uint32_t queueIdx;
    uint32_t newQueueIdx;

    /* Verify and update state of the buffer */
    state = &(aTxBufMgrInfo->pktBuffInfo[aBufIdx].state);
    if (ETHER_PKTBUFFSTATE_ALLOC != ATOMIC_CheckUpdate(state,
                                        ETHER_PKTBUFFSTATE_ALLOC,
                                        ETHER_PKTBUFFSTATE_QUEUED)) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {

        /* Get the queueIdx for buffer index Q */
        queueIdx = aTxBufMgrInfo->queueIdx;
        newQueueIdx = ModInc(queueIdx, (aTxBufMgrInfo->descCnt));
        aTxBufMgrInfo->queueIdx = newQueueIdx;

        /* Queue the buffer index to buffIdxQ */
        aTxBufMgrInfo->buffIdxQ[queueIdx] = aBufIdx;

        *aBuf = aTxBufMgrInfo->pktBuffInfo[aBufIdx].pktBuff;

        if (FALSE == aTxConfirmation) {
            aTxBufMgrInfo->pktBuffInfo[aBufIdx].flags &= ~ETHER_PKTFLAGS_CBEN;
        } else {
            aTxBufMgrInfo->pktBuffInfo[aBufIdx].flags |= ETHER_PKTFLAGS_CBEN;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEINITRXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
void ETHER_CntlrDeInitRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo)
{
    uint32_t i;

    /* Reset channel info */
    aRxBufMgrInfo->allocIdx = 0UL;
    aRxBufMgrInfo->queueIdx = 0UL;
    aRxBufMgrInfo->dequeueIdx = 0UL;

    /* Clear all the buffers */
    for (i = 0UL; i < aRxBufMgrInfo->pktBuffCnt; i++) {
        aRxBufMgrInfo->pktBuffInfo[i].state = ETHER_PKTBUFFSTATE_FREE;
    }

}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRINITRXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
void ETHER_CntlrInitRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo,
                        ETHER_RxPktType *const aRxChanPkt,
                        uint32_t aQueueCnt)
{
    uint32_t j;
    uint32_t queueIdx;

    aRxBufMgrInfo->allocIdx = 0UL;
    aRxBufMgrInfo->dequeueIdx = 0UL;

    /* Initialise the RxChanETHER_Dev */
    for (j = 0UL; j < aRxBufMgrInfo->pktBuffCnt; j++) {
        aRxBufMgrInfo->pktBuffInfo[j].pktBuff = &(aRxChanPkt[j]);
        aRxBufMgrInfo->pktBuffInfo[j].state = ETHER_PKTBUFFSTATE_FREE;
        if (aQueueCnt > j) {
            aRxBufMgrInfo->pktBuffInfo[j].state = ETHER_PKTBUFFSTATE_QUEUED;

            /* Queue buffer index to buffIdxQ */
            aRxBufMgrInfo->buffIdxQ[j] = j;
        }
    }

    /* queueIdx is the index to queue next buffer to buffIdxQ */
    queueIdx = aQueueCnt;
    queueIdx %= aRxBufMgrInfo->pktBuffCnt;
    aRxBufMgrInfo->queueIdx = queueIdx;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRPEEKRXDEQUEUEBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
uint32_t ETHER_CntlrPeekRxDequeueBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo)
{
    return aRxBufMgrInfo->dequeueIdx;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEQUEUERXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrDequeueRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo,
        uint8_t **const aBuf, uint32_t *const aDequeueIdx)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t dequeueIdx;
    uint32_t buffIdx;
    uint32_t volatile *state;
    uint32_t bufState;

    dequeueIdx = aRxBufMgrInfo->dequeueIdx;
    buffIdx = aRxBufMgrInfo->buffIdxQ[dequeueIdx];
    *aBuf = (uint8_t *)aRxBufMgrInfo->pktBuffInfo[buffIdx].pktBuff;
    state = &(aRxBufMgrInfo->pktBuffInfo[buffIdx].state);
    bufState = ATOMIC_CheckUpdate(state, ETHER_PKTBUFFSTATE_QUEUED,
                                        ETHER_PKTBUFFSTATE_DEQUEUED);
    if (ETHER_PKTBUFFSTATE_QUEUED != bufState) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        *aDequeueIdx = dequeueIdx;
        dequeueIdx = ModInc(dequeueIdx, aRxBufMgrInfo->pktBuffCnt);
        aRxBufMgrInfo->dequeueIdx = dequeueIdx;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRDEALLOCRXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
void ETHER_CntlrDeallocRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo)
{
    aRxBufMgrInfo->allocIdx = ModInc(aRxBufMgrInfo->allocIdx,
                                        aRxBufMgrInfo->pktBuffCnt);
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRQUEUERXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
void ETHER_CntlrQueueRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo)
{
    uint32_t queueIdx = aRxBufMgrInfo->queueIdx;

    aRxBufMgrInfo->pktBuffInfo[queueIdx].state = ETHER_PKTBUFFSTATE_QUEUED;
    aRxBufMgrInfo->buffIdxQ[queueIdx] = queueIdx;

    /* Increment & upate queueIdx */
    aRxBufMgrInfo->queueIdx = ModInc(queueIdx,
                                        aRxBufMgrInfo->pktBuffCnt);
}

/**
    @trace #BRCM_SWARCH_ETHER_CNTLRGETDEQUEUEDRXBUF_PROC
    @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
int32_t ETHER_CntlrGetDequeuedRxBuf(ETHER_RxBufMgrInfoType *const aRxBufMgrInfo,
                                  uint8_t **const aBuf,
                                  uint32_t *const aBufIdx)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t dequeueIdx;
    uint32_t allocIdx;

    allocIdx = aRxBufMgrInfo->allocIdx;
    dequeueIdx = aRxBufMgrInfo->dequeueIdx;

    if (dequeueIdx == allocIdx) {
        retVal = BCM_ERR_EAGAIN;
        goto err;
    }

    *aBufIdx = aRxBufMgrInfo->buffIdxQ[allocIdx];

    if (ETHER_PKTBUFFSTATE_DEQUEUED !=
            aRxBufMgrInfo->pktBuffInfo[*aBufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }

    *aBuf = (uint8_t *)aRxBufMgrInfo->pktBuffInfo[*aBufIdx].pktBuff;

err:
    return retVal;
}

/** @} */
