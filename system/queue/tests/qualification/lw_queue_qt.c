/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_lwq_qt Qualification Tests
    @ingroup grp_lwq

    @addtogroup grp_lwq_qt
    @{

    @file lw_queue_qt.c
    @brief LWQ Qualification Test

    @version 0.1 Initial Version
*/
#include <ulog.h>
#include <atomic.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <lw_queue.h>
#include "lw_queue_qt.h"
#define GetModuleLogLevel()     ULOG_LVL_INFO

/**
    @name LWQ Qualification Test Sequence IDs
    @{
    @brief LWQ Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_LWQ_SEQ0_PROC      (0xF0U)   /**< @brief #LWQ_QTSequence0  */
/** @} */


/**
    @brief Initialization of LWQ

    @pre None

    @test Initialize LWQ

    @post Length should be set to Zero

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_INIT   (0UL)

/**
    @brief Push entries into Queue before Queue is full

    @pre LWQ is initialized

    @test Push 15 entries into LWQ

    @post
        -# Push should succeed for 15 iterations
        -# Length should increment by 1 for every iteration

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_EMPTY_PUSH   (1UL)

/**
    @brief Push entries into Queue after Queue is full

    @pre LWQ is full

    @test Push an entry into LWQ

    @post
        -# #BCM_ERR_NOMEM should be returned
        -# Length should not be altered

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_FULL_PUSH   (2UL)

/**
    @brief Pop entries from Full Queue

    @pre LWQ is full

    @test Pop 15 entries from LWQ

    @post
        -# Pop should return objects in the order they are pushed.
        -# Length should decrement by 1 for every iteration

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_FULL_POP   (3UL)

/**
    @brief Pop entries from Empty Queue

    @pre LWQ is empty

    @test Pop an entry from LWQ

    @post
        -# #BCM_ERR_NOMEM should be returned
        -# Length shall be zero

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_EMPTY_POP   (4UL)

/**
    @brief Peek first/last entry in Empty Queue

    @pre LWQ is initialized

    @test Peek first/last entry in LWQ

    @post
        -# #BCM_ERR_NOMEM should be returned

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_EMPTY_PEEK   (5UL)

/**
    @brief Peek first/last entry in Non-Empty Queue

    @pre LWQ is initialized and some data is present in queue

    @test Peek first/last entry in LWQ

    @post
        -# Peek should return earliest object.
        -# Length should not change after a Peek operation

    @trace #BRCM_SWREQ_LWQ
*/
#define BRCM_SWQTST_LWQ_DATA_PEEK   (6UL)

/** @brief LWQ Test sequence

    @code{.c}
    LWQ_Init()
    LWQ_Length() should be Zero Length. #BRCM_SWQTST_LWQ_INIT
    LWQ_PeekFirst() should return 0U
    LWQ_PeekLast() should return 0U
    LWQ_Length() should be Zero Length. #BRCM_SWQTST_LWQ_EMPTY_PEEK
    for i 0 to 15
        LWQ_Push(i)
        LWQ_Length() should be (i+1). #BRCM_SWQTST_LWQ_EMPTY_PUSH
        LWQ_PeekFirst() should return 0U
        LWQ_PeekLast() should return i
        LWQ_Length() should be (i+1). #BRCM_SWQTST_LWQ_DATA_PEEK
    Queue is now Full
    LWQ_Push(7) should return #BCM_ERR_NOMEM
    LWQ_Length() should return 15. #BRCM_SWQTST_LWQ_FULL_PUSH
    for i 0 to 15
        LWQ_Pop() should return i
        LWQ_Length() should be (14-i). #BRCM_SWQTST_LWQ_FULL_POP
        if i!= 14
            LWQ_PeekFirst() should return (i + 1)
            LWQ_PeekLast() should return 14U
            LWQ_Length() should be (14-i). #BRCM_SWQTST_LWQ_DATA_PEEK
    Queue is now Empty
    LWQ_Pop() should return #BCM_ERR_NOMEM
    LWQ_Length() should be 0. #BRCM_SWQTST_LWQ_EMPTY_POP
    LWQ_PeekFirst() should return 0U
    LWQ_PeekLast() should return 0U
    LWQ_Length() should be 0. #BRCM_SWQTST_LWQ_EMPTY_PEEK
    @endcode

    @board board1
    @board board2

*/
#define BRCM_SWQTSEQ_LWQ_SEQ0   (0UL)

#define LWQ_MARK_ERR(ret)                                      \
    {                                                          \
        ULOG_ERR("\nError: %d \t line %d\n", ret ,__LINE__);   \
        if (BCM_AT_EXECUTING == LWQ_QTResult){                 \
            (LWQ_QTResult) = (ret);                            \
        }                                                      \
    }

static volatile int32_t LWQ_QTResult = BCM_AT_NOT_STARTED;
/** @brief Brief description of COMP_QTSequence0

    @todo Detailed description of COMP_QTSequence0

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWQTSEQ_COMP_SEQ0

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.
    @image html comp_flowchart.jpg

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
int32_t LWQQT_Sequence0(void)
{
    uint8_t i;
    int32_t ret0 = BCM_ERR_OK;
    uint8_t object;
    LWQ_BufferType buffer;
    LWQ_QTResult = BCM_AT_EXECUTING;

    LWQ_Init(&buffer);
    if (0UL == LWQ_Length(buffer)) {
        ULOG_INFO("BRCM_SWQTST_LWQ_INIT:Pass\n");
    } else {
        LWQ_MARK_ERR(ret0);
    }
    if ((0U == LWQ_PeekFirst(buffer))
        && (0U == LWQ_PeekLast(buffer))
        && (0UL == LWQ_Length(buffer))) {
        ULOG_INFO("BRCM_SWQTST_LWQ_EMPTY_PEEK:Pass\n");
    } else {
        LWQ_MARK_ERR(ret0);
    }
    for (i = 0U; i < 15U; i++) {
        ret0 = LWQ_Push(&buffer, i);
        if ((BCM_ERR_OK == ret0)
            && ((uint8_t)(i + 1U) == LWQ_Length(buffer))) {
            ULOG_INFO("BRCM_SWQTST_LWQ_EMPTY_PUSH:Pass\n");
        } else {
            LWQ_MARK_ERR(ret0);
        }
        if ((0U == LWQ_PeekFirst(buffer))
            && (i == LWQ_PeekLast(buffer))
            && ((uint8_t)(i + 1U) == LWQ_Length(buffer))) {
            ULOG_INFO("BRCM_SWQTST_LWQ_DATA_PEEK:Pass\n");
        } else {
            LWQ_MARK_ERR(ret0);
        }
    }

    /* Queue is now Full */

    ret0 = LWQ_Push(&buffer, 7U);
    if ((BCM_ERR_NOMEM == ret0)
        && (LWQ_BUFFER_MAX_LENGTH == LWQ_Length(buffer))) {
        ULOG_INFO("BRCM_SWQTST_LWQ_FULL_PUSH:Pass\n");
    } else {
        LWQ_MARK_ERR(ret0);
    }
    for (i = 0U; i < 15U; i++) {
        object = 0xFFU;
        ret0 = LWQ_Pop(&buffer, &object);
        if ((BCM_ERR_OK == ret0)
            && (i == object)
            && ((uint8_t)(14U - i) == LWQ_Length(buffer))) {
            ULOG_INFO("BRCM_SWQTST_LWQ_FULL_POP:Pass\n");
        } else {
            LWQ_MARK_ERR(ret0);
        }
        ULOG_INFO("%d %d %d\n", LWQ_PeekFirst(buffer), LWQ_PeekLast(buffer), LWQ_Length(buffer));
        if (i != 14U) {
            if (((i + 1U) == LWQ_PeekFirst(buffer))
                && (14U == LWQ_PeekLast(buffer))
                && ((uint8_t)(14U - i) == LWQ_Length(buffer))) {
                ULOG_INFO("BRCM_SWQTST_LWQ_DATA_PEEK:Pass\n");
            } else {
                LWQ_MARK_ERR(ret0);
            }
        }
    }

    /* Queue is now Empty */

    ret0 = LWQ_Pop(&buffer, &object);
    if ((BCM_ERR_NOMEM == ret0)
        && (0UL == LWQ_Length(buffer))) {
        ULOG_INFO("BRCM_SWQTST_LWQ_EMPTY_POP:Pass\n");
    } else {
        LWQ_MARK_ERR(ret0);
    }
    if ((0U == LWQ_PeekFirst(buffer))
        && (0U == LWQ_PeekLast(buffer))
        && (0UL == LWQ_Length(buffer))) {
        ULOG_INFO("BRCM_SWQTST_LWQ_EMPTY_POP:Pass\n");
    } else {
        LWQ_MARK_ERR(ret0);
    }

    return BCM_ERR_OK;

}


int32_t LWQQT_GetResult(void)
{
    int32_t ret = LWQ_QTResult;
    if (BCM_AT_EXECUTING == ret) {
        ret = BCM_ERR_OK;
    }
    return ret;
}

/** @} */


