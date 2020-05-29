/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_circq_impl Implementation
    @ingroup grp_circq

    @addtogroup grp_circq_impl
    @{

    @file circ_queue.c
    @brief Circular Queue Implementation
    This source file contains the implementation of functions for circular queue
    @version 1.0 updated for traceability
    @version 0.1 Initial version
*/

#include <string.h>
#include <circ_queue.h>
#include <cortex.h>
#include <atomic.h>
#include <utils.h>

/**
    @name Circular Queue Design IDs
    @{
    @brief Design IDs for Circular Queue
*/
#define BRCM_SWDSGN_CIRCQ_INCR_IDX_MACRO     (0x80UL) /**< @brief #CIRCQ_INCR_IDX         */
#define BRCM_SWDSGN_CIRCQ_UPDATE_IDX_MACRO   (0x81UL) /**< @brief #CIRCQ_UPDATE_IDX       */
#define BRCM_SWDSGN_CIRCQ_INDEX_MACRO        (0x82UL) /**< @brief #CIRCQ_INDEX_READ_SHIFT */
#define BRCM_SWDSGN_CIRCQ_COPYDATA_PROC      (0x83UL) /**< @brief #CIRCQ_CopyData         */
#define BRCM_SWDSGN_CIRCQ_RESET_PROC         (0x84UL) /**< @brief #CIRCQ_Reset            */
#define BRCM_SWDSGN_CIRCQ_ISEMPTY_PROC       (0x85UL) /**< @brief #CIRCQ_IsEmpty          */
#define BRCM_SWDSGN_CIRCQ_ISFULL_PROC        (0x86UL) /**< @brief #CIRCQ_IsFull           */
#define BRCM_SWDSGN_CIRCQ_GETFILLEDSIZE_PROC (0x87UL) /**< @brief #CIRCQ_GetFilledSize    */
#define BRCM_SWDSGN_CIRCQ_PUSH_PROC          (0x88UL) /**< @brief #CIRCQ_Push             */
#define BRCM_SWDSGN_CIRCQ_POP_PROC           (0x89UL) /**< @brief #CIRCQ_Pop              */
#define BRCM_SWDSGN_CIRCQ_PEEK_PROC          (0x8AUL) /**< @brief #CIRCQ_Peek             */
/** @} */

/**
    @brief Increments @a idx with modulo @a sz

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_POP_PROC
    @trace #BRCM_SWARCH_CIRCQ_PEEK_PROC
    @trace #BRCM_SWARCH_CIRCQ_ISEMPTY_PROC
*/
#define CIRCQ_INCR_IDX(idx, sz)     ((idx) = (((idx) + 1UL) & ((sz) - 1UL)))

/**
    @brief Increments @a idx by @a val with module @a sz

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_POP_PROC
    @trace #BRCM_SWARCH_CIRCQ_PEEK_PROC
    @trace #BRCM_SWARCH_CIRCQ_PUSH_PROC
*/
#define CIRCQ_UPDATE_IDX(idx, val, sz)  ((idx) = (((idx) + (val)) & ((sz) - 1UL)))

/**
    @name Circular Queue Index Fields
    @{
    @brief Field related macros for Circular Queue Index

    @trace #BRCM_SWREQ_CIRCQ_CONFIGURATION
    @trace #BRCM_SWARCH_CIRCQ_OVERWRITE_TYPE
    @trace #BRCM_SWARCH_CIRCQ_INDEX_TYPE
    @trace #BRCM_SWARCH_CIRCQ_TYPE
    @trace #BRCM_SWARCH_CIRCQ_INDEX_INIT_MACRO
    @trace #BRCM_SWARCH_CIRCQ_INDEX_DEFINE_MACRO
    @trace #BRCM_SWARCH_CIRCQ_INIT_MACRO
    @trace #BRCM_SWARCH_CIRCQ_DEFINE_MACRO
    @trace #BRCM_SWARCH_CIRCQ_V2_MACRO
    @trace #BRCM_SWARCH_CIRCQ_RESET_PROC
    @trace #BRCM_SWARCH_CIRCQ_ISEMPTY_PROC
    @trace #BRCM_SWARCH_CIRCQ_ISFULL_PROC
    @trace #BRCM_SWARCH_CIRCQ_GETFILLEDSIZE_PROC
    @trace #BRCM_SWARCH_CIRCQ_PUSH_PROC
    @trace #BRCM_SWARCH_CIRCQ_POP_PROC
    @trace #BRCM_SWARCH_CIRCQ_PEEK_PROC
*/
#define CIRCQ_INDEX_READ_SHIFT      (16UL)      /**< @brief Read Shift in Index */
#define CIRCQ_INDEX_WRITE_MASK      (0xFFFFUL)  /**< @brief Write Mask in Index */
/** @} */

/** @brief Copy Data from Circular Queue

    Copy data from circular queue to aData and return the count of
    objects copied. Circular Queue is un-modified.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aQ          Pointer to Circular Queue
    @param[out]     aData       Pointer where data of Circular Queue
                                should be copied
    @param[in]      aCnt        Maximum number of objects that can be copied
    @param[out]     aRdIdx      Updated read index to be set after copying

    @return    Count of objects copied

    @post None

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_POP_PROC
    @trace #BRCM_SWARCH_CIRCQ_PEEK_PROC

    @limitations None

    @todo update pseudocode
    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static uint32_t CIRCQ_CopyData(const CIRCQ_Type *const aQ, char * const aData,
                uint32_t aCnt, uint32_t * const aRdIdx)
{
    uint32_t space;
    uint32_t i = 0UL;
    uint32_t wrIdx;
    uint32_t rdIdx;
    uint32_t count;

    count = aCnt;

    CIRCQ_IndexType *circQIdx = aQ->qIndex;
    uint32_t qIndex = circQIdx->qIndex;

    wrIdx = (qIndex & CIRCQ_INDEX_WRITE_MASK);
    rdIdx = (qIndex >> CIRCQ_INDEX_READ_SHIFT);
    CIRCQ_INCR_IDX(rdIdx, aQ->count);

    if (wrIdx == rdIdx) {
        /* Nothing to be done. Queue is empty */
    } else if (wrIdx > rdIdx) {
        space = wrIdx - rdIdx;
        if (count <= space) {
            space = count;
        }
        BCM_MemCpy((void*)&aData[i*aQ->objSize],
               (void*)&aQ->buffer[rdIdx*aQ->objSize],
               space*aQ->objSize);
        CIRCQ_UPDATE_IDX(rdIdx, space - 1UL, aQ->count);
        i += space;
    } else {
        space = aQ->count - rdIdx;
        if (count <= space) {
            space = count;
        }
        BCM_MemCpy((void*)&aData[i*aQ->objSize],
               (void*)&aQ->buffer[rdIdx*aQ->objSize],
               space*aQ->objSize);
        CIRCQ_UPDATE_IDX(rdIdx, space - 1UL, aQ->count);
        i += space;
        count -= space;
        space = wrIdx;
        if (count > 0UL) {
            if (count <= space) {
                space = count;
            }
            CIRCQ_INCR_IDX(rdIdx, aQ->count);
            BCM_MemCpy((void*)&aData[i*aQ->objSize],
                   (void*)&aQ->buffer[rdIdx*aQ->objSize],
                   space*aQ->objSize);
            CIRCQ_UPDATE_IDX(rdIdx, space - 1UL, aQ->count);
            i += space;
        }
    }

    *aRdIdx = rdIdx;

    return i;
}
/** @} */

/**
    @addtogroup grp_circq_ifc
    @{
*/

/**

    @trace #BRCM_SWREQ_CIRCQ_INITIALIZATION
    @trace #BRCM_SWARCH_CIRCQ_RESET_PROC

    @code{.c}
    Atomically set the Queue->qIndex->qIndex to empty
        Upper 16-bits to (size-1). Lower 16-bits to 0.
    @endcode
*/
int32_t CIRCQ_Reset(const CIRCQ_Type *const aQ)
{
    int32_t err = BCM_ERR_OK;

    if (aQ != NULL) {
        (void)ATOMIC_Set(&aQ->qIndex->qIndex, (aQ->count - 1UL) << CIRCQ_INDEX_READ_SHIFT);
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }
    return err;
}

/**
    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_ISEMPTY_PROC

    @code{.c}
    CIRCQ_INCR_IDX(rdIdx, count)
    if rdIdx == wrIndex
        Queue is Empty
    else
        Queue is not Empty
    @endcode
*/
int32_t CIRCQ_IsEmpty(const CIRCQ_Type *const aQ, uint32_t *const aIsEmpty)
{
    uint32_t rdidx;
    uint32_t wridx;
    int32_t err = BCM_ERR_OK;

    if ((aQ != NULL) && (aIsEmpty != NULL)) {
        CIRCQ_IndexType *circQIdx = aQ->qIndex;
        rdidx = (circQIdx->qIndex) >> CIRCQ_INDEX_READ_SHIFT;
        wridx = (circQIdx->qIndex) & CIRCQ_INDEX_WRITE_MASK;
        CIRCQ_INCR_IDX(rdidx, aQ->count);
        if (rdidx == wridx) {
            *aIsEmpty = 1UL;
        } else {
            *aIsEmpty = 0UL;
        }
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }
    return err;
}

/**
    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_ISFULL_PROC

    @code{.c}
    if rdIdx == wrIdx
        Queue is Full
    else
        Queue is not Full
    @endcode
*/
int32_t CIRCQ_IsFull(const CIRCQ_Type *const aQ, uint32_t *const aIsFull)
{
    uint32_t rdidx;
    uint32_t wridx;
    int32_t err = BCM_ERR_OK;

    if ((aQ != NULL) && (aIsFull != NULL)) {
        CIRCQ_IndexType *circQIdx = aQ->qIndex;
        rdidx = (circQIdx->qIndex) >> CIRCQ_INDEX_READ_SHIFT;
        wridx = (circQIdx->qIndex) & CIRCQ_INDEX_WRITE_MASK;
        if (rdidx == wridx) {
            *aIsFull = 1UL;
        } else {
            *aIsFull = 0UL;
        }
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }
    return err;
}

/**

    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_GETFILLEDSIZE_PROC

    @code{.c}
    return Modulo (wrIdx - rdIdx - 1, count)
    @endcode
*/
int32_t CIRCQ_GetFilledSize(const CIRCQ_Type *const aQ, uint32_t *const aSize)
{
    uint32_t rdidx;
    uint32_t wridx;
    int32_t err;

    if ((NULL != aQ)
        && (NULL != aSize)) {
        CIRCQ_IndexType *circQIdx = aQ->qIndex;
        rdidx = (circQIdx->qIndex) >> CIRCQ_INDEX_READ_SHIFT;
        wridx = (circQIdx->qIndex) & CIRCQ_INDEX_WRITE_MASK;

        if (rdidx < wridx) {
            *aSize = wridx - rdidx - 1UL;
        } else if (rdidx == wridx) {
            *aSize = aQ->count - 1UL;
        } else {
            *aSize  = wridx + (aQ->count - rdidx - 1UL);
        }
        err = BCM_ERR_OK;
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

/**
    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_PUSH_PROC

    @todo update pseudocode
    @code{.c}
    @endcode
*/
int32_t CIRCQ_Push(const CIRCQ_Type *const aQ, const char *const aData, uint32_t aCnt)
{
    uint32_t isFull;
    uint32_t space;
    int32_t err = BCM_ERR_OK;
    uint32_t i = 0UL;
    const char *inPtr = aData;
    uint32_t wrIdx;
    uint32_t rdIdx;
    uint32_t old_wrIdx;
    uint32_t old_rdIdx;
    uint32_t exclusive = 0UL;
    uint32_t count = aCnt;
    CIRCQ_IndexType *circQIdx;

    if ((aQ == NULL) || (aData == NULL)) {
        err = BCM_ERR_INVAL_PARAMS;
        goto end;
    }

    circQIdx = aQ->qIndex;
    err = CIRCQ_IsFull(aQ, &isFull);
    if (err != BCM_ERR_OK) {
        goto end;
    }
    if ((isFull == 1UL) && (aQ->overwrite == 0UL)) {
        err = BCM_ERR_NOMEM;
        goto end;
    }

    if (1UL == aQ->overwrite) {
        while (count > aQ->count) {
            i += aQ->count;
            count -= aQ->count;
        }
    } else {
        if (count > aQ->count) {
            count = aQ->count;
        }
    }

    if (count == 0UL) {
        goto end;
    }

    (void)ATOMIC_Inc(&circQIdx->qLock);

    do {
        uint32_t qIndex = CORTEX_LdrEx(&circQIdx->qIndex);
        wrIdx = (qIndex & CIRCQ_INDEX_WRITE_MASK);
        rdIdx = (qIndex >> CIRCQ_INDEX_READ_SHIFT);
        old_wrIdx = wrIdx;
        old_rdIdx = rdIdx;

        if (wrIdx <= rdIdx) {
            space = rdIdx - wrIdx;
        } else {
            space = rdIdx + (aQ->count - wrIdx);
        }
        if ((1UL == aQ->overwrite) && (count > space)) {
            rdIdx = wrIdx;
        } else {
            count = MIN(space, count);
        }
        CIRCQ_UPDATE_IDX(wrIdx, count, aQ->count);
        exclusive = CORTEX_StrEx(&circQIdx->qIndex , (rdIdx << CIRCQ_INDEX_READ_SHIFT) | (wrIdx));
    } while (1UL == exclusive);
    wrIdx = old_wrIdx;
    rdIdx = old_rdIdx;

    while (count > 0UL) {
        if (rdIdx < wrIdx) {
            space = aQ->count - wrIdx;
            if (count <= space) {
                space = count;
            }
            BCM_MemCpy(&aQ->buffer[wrIdx*aQ->objSize], &inPtr[i*aQ->objSize], space*aQ->objSize);
            CIRCQ_UPDATE_IDX(wrIdx, space, aQ->count);
            i += space;
            count -= space;
        } else {
            if (aQ->overwrite == 0UL) {
                if (rdIdx > wrIdx) {
                    space = rdIdx - wrIdx;
                    if (count <= space) {
                        space = count;
                    }
                    BCM_MemCpy(&aQ->buffer[wrIdx*aQ->objSize], &inPtr[i*aQ->objSize], space*aQ->objSize);
                    CIRCQ_UPDATE_IDX(wrIdx, space, aQ->count);
                    i += space;
                    count -= space;
                } else {
                    break;
                }
            } else {
                space = aQ->count - wrIdx;
                if (count <= space) {
                    space = count;
                }
                BCM_MemCpy(&aQ->buffer[wrIdx*aQ->objSize], &inPtr[i*aQ->objSize], space*aQ->objSize);
                CIRCQ_UPDATE_IDX(wrIdx, space, aQ->count);
                i += space;
                count -= space;
                if (rdIdx < wrIdx) {
                    rdIdx = wrIdx;
                }
            }
        }
    }
    (void)ATOMIC_Dec(&circQIdx->qLock);
end:
    return err;
}

/**
    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_POP_PROC

    @todo update pseudocode
    @code{.c}
    @endcode
*/
uint32_t CIRCQ_Pop(const CIRCQ_Type *const aQ, char * const aData, uint32_t aCnt)
{
    uint32_t isEmpty;
    uint32_t i = 0UL;
    int32_t err = BCM_ERR_OK;
    uint32_t wrIdx;
    uint32_t rdIdx;
    uint32_t exclusive;
    uint32_t count;
    uint32_t qIndex;
    CIRCQ_IndexType *circQIdx;

    count = aCnt;
    if ((aQ == NULL) || (aData == NULL)) {
        goto end;
    }
    circQIdx = aQ->qIndex;

    if (circQIdx->qLock > 0UL) {
        goto end;
    }
    err = CIRCQ_IsEmpty(aQ, &isEmpty);
    if (err != BCM_ERR_OK) {
        goto end;
    }
    if (isEmpty == 1UL) {
        goto end;
    }
    if (count == 0UL) {
        goto end;
    }

    i = CIRCQ_CopyData(aQ,aData,aCnt,&rdIdx);

    if (i != 0UL) {
        do {
            exclusive = 0UL;
            qIndex = CORTEX_LdrEx(&circQIdx->qIndex);
            wrIdx = (qIndex & CIRCQ_INDEX_WRITE_MASK);
            exclusive = CORTEX_StrEx(&circQIdx->qIndex , (rdIdx << CIRCQ_INDEX_READ_SHIFT) | (wrIdx));
        } while (1UL == exclusive);
    }
end:
    return i;
}

/**
    @trace #BRCM_SWREQ_CIRCQ_DATA_ACCESS
    @trace #BRCM_SWARCH_CIRCQ_PEEK_PROC

    @todo update pseudocode
    @code{.c}
    @endcode
*/
uint32_t CIRCQ_Peek(const CIRCQ_Type *const aQ, char * const aData, uint32_t aCnt)
{
    uint32_t isEmpty;
    uint32_t i = 0UL;
    int32_t err = BCM_ERR_OK;
    uint32_t rdIdx;
    uint32_t count;
    CIRCQ_IndexType *circQIdx;

    count = aCnt;
    if ((aQ == NULL) || (aData == NULL)) {
        goto end;
    }
    circQIdx = aQ->qIndex;
    if (circQIdx->qLock > 0UL) {
        goto end;
    }
    err = CIRCQ_IsEmpty(aQ, &isEmpty);
    if (err != BCM_ERR_OK) {
        goto end;
    }
    if (isEmpty == 1UL) {
        goto end;
    }
    if (count == 0UL) {
        goto end;
    }
    i = CIRCQ_CopyData(aQ,aData,aCnt,&rdIdx);
end:
    return i;
}

/** @} */
