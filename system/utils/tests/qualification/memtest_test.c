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
/**
    @addtogroup grp_memtest_qt
    @{

    @file memtest_test.c
    @brief Qualification Test for memory testing API's

    @version 0.1 Initial Version
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <shell.h>
#include <compiler.h>
#include <ulog.h>
#include <atomic.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_memtest.h>

#include "utils_test.h"
#include "memtest_qt.h"

#define GetModuleLogLevel()     ULOG_LVL_INFO

#define MEMORY_TEST_BUFFER_SIZE             (10U * 1024U)
#define MEMORY_TEST_NO_STACK_BUFFER_SIZE    (342U)

static uint8_t COMP_SECTION(".pp.cached.test")   MemoryTestBuffer[MEMORY_TEST_BUFFER_SIZE];
static uint8_t COMP_SECTION(".pp.uncached.test") MemoryTestBufferNoStack[MEMORY_TEST_NO_STACK_BUFFER_SIZE];

static volatile uint32_t MemtestQT_Result = (uint32_t)BCM_AT_NOT_STARTED;

COMP_NEVER_INLINE int32_t ATOMIC_Get_Error(void)
{
    return (int32_t)ATOMIC_Get(&MemtestQT_Result);
}

COMP_NEVER_INLINE void ATOMIC_Set_Error(const int32_t err)
{
    (void)ATOMIC_Set(&MemtestQT_Result, (uint32_t)err);
}

#define MEMTEST_MARK_ERR(ret)                                   \
    if (BCM_AT_EXECUTING == ATOMIC_Get_Error()) {               \
        ULOG_ERR("\nError: %d \t line %d\n", ret ,__LINE__);    \
        ATOMIC_Set_Error((ret));                                \
    }

int32_t MemtestQT_Start(void)
{
    BCM_TimeType tb,ta, tdiff;
    uint32_t ret;

    ATOMIC_Set_Error(BCM_AT_EXECUTING);

    /* Test BCM_MemTestNoStack API */
    {
        BCM_GetTime(&tb);
        ret = BCM_MemTestNoStack(MemoryTestBufferNoStack, MEMORY_TEST_NO_STACK_BUFFER_SIZE);
        BCM_GetTime(&ta);

        if (0U == ret) {
            /*
                Test the garbage values.
                These should start from 255 and go down to zero.
            */
            uint32_t i;

            for (i = 0U; i < MEMORY_TEST_NO_STACK_BUFFER_SIZE; i++) {
                if (MemoryTestBufferNoStack[i] != (uint8_t)(~i)) {
                    MEMTEST_MARK_ERR(BCM_ERR_UNKNOWN);
                }
            }

        } else {
            MEMTEST_MARK_ERR(BCM_ERR_UNKNOWN);
        }

        if (BCM_AT_EXECUTING == ATOMIC_Get_Error()) {
            /* Test passed */
            tdiff = BCM_GetTimeAbsDiff(tb, ta);
            ULOG_INFO("BRCM_SWQTST_BCM_MEMTESTNOSTACK:Pass\n");
            ULOG_INFO("Test run time - %d sec %d ns. Buffer size - %d\n",
                        tdiff.s, (uint32_t)(tdiff.ns),
                        MEMORY_TEST_NO_STACK_BUFFER_SIZE);
            CONSOLE_QualReport(CONSOLE_UART,
                               BCM_UTL_ID,
                               BRCM_SWQTSEQ_MEMORY_SEQ0,
                               BRCM_SWQTST_BCM_MEMTESTNOSTACK,
                               CONSOLE_QUAL_PASS);
        } else {
            ULOG_INFO("BRCM_SWQTST_BCM_MEMTESTNOSTACK:Fail\n");
            CONSOLE_QualReport(CONSOLE_UART,
                               BCM_UTL_ID,
                               BRCM_SWQTSEQ_MEMORY_SEQ0,
                               BRCM_SWQTST_BCM_MEMTESTNOSTACK,
                               CONSOLE_QUAL_FAIL);
        }
    }

    /* Test BCM_MemTest API */
    {
        BCM_GetTime(&tb);
        ret = BCM_MemTest(MemoryTestBuffer, MEMORY_TEST_BUFFER_SIZE);
        BCM_GetTime(&ta);

        if (0U == ret) {
            /* Check the garbage values, and see if they are OK */
            uint32_t i;

            for (i = 0U; i < MEMORY_TEST_BUFFER_SIZE - 1U; i++) {
                if (MemoryTestBuffer[i] != (uint8_t)(~i)) {
                    MEMTEST_MARK_ERR(BCM_ERR_UNKNOWN);
                }
            }

        } else {
            MEMTEST_MARK_ERR(BCM_ERR_UNKNOWN);
        }

        if (BCM_AT_EXECUTING == ATOMIC_Get_Error()) {
            /* Test passed */
            tdiff = BCM_GetTimeAbsDiff(tb, ta);
            ULOG_INFO("BRCM_SWQTST_BCM_MEMTEST:Pass\n");
            ULOG_INFO("Test run time - %d sec %d ns. Buffer size - %d\n",
                        tdiff.s, (uint32_t)(tdiff.ns),
                        MEMORY_TEST_BUFFER_SIZE);
            CONSOLE_QualReport(CONSOLE_UART,
                               BCM_UTL_ID,
                               BRCM_SWQTSEQ_MEMORY_SEQ0,
                               BRCM_SWQTST_BCM_MEMTEST,
                               CONSOLE_QUAL_PASS);
        } else {
            ULOG_INFO("BRCM_SWQTST_BCM_MEMTEST:Fail\n");
            CONSOLE_QualReport(CONSOLE_UART,
                               BCM_UTL_ID,
                               BRCM_SWQTSEQ_MEMORY_SEQ0,
                               BRCM_SWQTST_BCM_MEMTEST,
                               CONSOLE_QUAL_FAIL);
        }
    }

    if (BCM_AT_EXECUTING == ATOMIC_Get_Error()) {
        ATOMIC_Set_Error(BCM_ERR_OK);
    }

    return 0;
}

int32_t MemtestQT_GetResult(void)
{
    return ATOMIC_Get_Error();
}

/** @} */
