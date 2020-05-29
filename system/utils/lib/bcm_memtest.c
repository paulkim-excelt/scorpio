/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_memtest_impl BCM Error Implementation
    @ingroup grp_memtest

    @addtogroup grp_memtest_impl
    @{

    @file bcm_memtest.c
    @brief Memory tests
    This file implements the memory testing routines

    @version 0.1 Initial version
*/

#include <limits.h>
#include <stdint.h>
#include <cache.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <bcm_memtest.h>

/**
    @name BCM MemTest API Design IDs
    @{
    @brief Design IDs for BCM MemTest API
*/
#define BRCM_SWDSGN_BCM_MEMTEST_PROC        (0x80UL) /**< @brief #BCM_MemTest           */
#define BRCM_SWDSGN_BCM_MEMTESTNOSTACK_PROC (0x81UL) /**< @brief #BCM_MemTestNoStack    */
#define BRCM_SWDSGN_BCM_MEMTESTMARCHC_PROC  (0x82UL) /**< @brief #BCM_MemTestMarchC     */
#define BRCM_SWDSGN_BCM_MEMTESTHAMMER_PROC  (0x83UL) /**< @brief #BCM_MemTestHammer     */
/** @} */

/** @brief Test the volatile memory (SRAM/DRAM) for defects

    This function implements a modified MATS algorithm to test
    for any memory defects.

    MATS, in its original form, provides 100% fault
    coverage for Stuck At Faults (SAF), and Stuck Open Faults (SOF).
    It provides a partial fault coverage for Address Faults (AF) and
    various types of Coupling Faults (CF). If we get lucky,
    we may uncover other kinds of faults along the way, but
    is in no way guarenteed.

    MATS march algorithm is the following -
    (bi)(wa), (up)(ra, wa'), (down)(ra')

    Given that we operate on byte addressable memories, we will need
    to run the march test 4 (log(8) + 1) times using 4 different
    background words (a's in the above representation) to complete
    the MATS algorithm.

    Since we typically would not have time to run the march test 4
    times, our implementation will only attempt to run the algorithm
    once. To offset this, and to get an increased AF coverage, we use
    different background words at different addresses.

    With this setup, we will still retain 100% fault coverage for SAF's
    and SOF's and will still have a partial coverage for the AF's and CF's.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects
    @param[in]       aLen        Length of the memory to be tested

    Return values are documented in reverse-chronological order
    @retval        0             if no defects were uncovered
    @retval        1             if a certain defect was encountered during the memory testing

    @post Tested memory range may have been filled with garbage.

    @trace #BRCM_SWARCH_BCM_MEMTEST_PROC
    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations The aMemAddr pointer should be aligned to a cache line size.

*/
uint32_t BCM_MemTest(void *const aMemAddr, const uint32_t aLen)
{
    uint32_t memErrorSeen = 0U;
    const uint32_t memcpyBlockSz = 4096U;
    uint8_t *const memAddr = (uint8_t*)aMemAddr;

    /* Divide the aLen into chunks of size memcpyBlockSz */
    const uint32_t numIter = aLen / memcpyBlockSz;
    const uint32_t rem = aLen % memcpyBlockSz;

    if (numIter > 0U) {
        /*
            Apply a 'fast' mem test algorithm to the memory region with -
                start addr: aMemAddr + rem
                end addr  : aMemAddr + aLen - 1U

            The length of this region is a multiple of memcpyBlockSz.
        */
        uint32_t i;
        uint8_t *const memcpyMemAddr = memAddr + rem;
        uint8_t *const lastBlockAddr = memcpyMemAddr + ((numIter - 1U) * memcpyBlockSz);

        /*
            Write background words to the first block, flush cache
        */
        for (i = 0U; i < memcpyBlockSz; i++) {
            memcpyMemAddr[i] = (uint8_t)i;
        }
        DCACHE_FlushRange((uint32_t)memcpyMemAddr, memcpyBlockSz);

        /*
            Now for each of the remaining blocks, copy the data from the previous block.
            Flush the cache along the way.
        */
        for (i = 1U; i < numIter; i++) {
            const uint8_t *const src = memcpyMemAddr + ((i - 1U) * memcpyBlockSz);
            uint8_t *const dst = memcpyMemAddr + (i * memcpyBlockSz);
            BCM_MemCpy(dst, src, memcpyBlockSz);
            DCACHE_FlushRange((uint32_t)dst, memcpyBlockSz);
        }

        /*
            Validate the contents of the last block.
            Invert the contents of this block, and write them back. Flush cahces.
        */
        for (i = 0U; i < memcpyBlockSz; i++) {
            memErrorSeen |= (uint32_t)(lastBlockAddr[i] != (uint8_t)i);
            lastBlockAddr[i] = ~lastBlockAddr[i];
        }
        DCACHE_FlushRange((uint32_t)lastBlockAddr, memcpyBlockSz);

        /*
            Starting with the block number (numIter - 2), copy the contents of
            the next block into the current block. This should carry over the inverted
            background words that we wrote to the last block to all the blocks.
            Flush caches along the way.
        */
        for (i = numIter - 1U; i > 0U; i--) {
            const uint8_t *const src = memcpyMemAddr + (i * memcpyBlockSz);
            uint8_t *const dst = memcpyMemAddr + ((i - 1U) * memcpyBlockSz);
            BCM_MemCpy(dst, src, memcpyBlockSz);
            DCACHE_FlushRange((uint32_t)dst, memcpyBlockSz);
        }

        /*
            Finally, validate the contents of the first block.
        */
        for (i = 0; i < memcpyBlockSz; i++) {
            memErrorSeen |= (uint32_t)(lastBlockAddr[i] != ((uint8_t)(~i)));
        }
    }

    if (rem > 0U) {
        /*
            Run the mem test algorithm to the memory region with -
                start addr: aMemAddr
                end addr  : aMemAddr + rem - 1U

            The length of this region is a multiple of memcpyBlockSz.
        */
        uint32_t i;

        /* Write background words, flush cache */
        for (i = 0U; i < rem; i++) {
            memAddr[i] = (uint8_t)i;
        }
        DCACHE_FlushRange((uint32_t)memAddr, rem);

        /* Validate the background words written, invert them and write them back. */
        for (i = 0U; i < rem; i++) {
            memErrorSeen |= (uint32_t)(memAddr[i] != (uint8_t)i);
            memAddr[i] = ~memAddr[i];
        }
        DCACHE_FlushRange((uint32_t)memAddr, rem);

        /* Validate if the inverted background words are properly read */
        for (i = rem - 1U; (UINT_MAX) != i; i--) {
            memErrorSeen |= (uint32_t)(memAddr[i] != ((uint8_t)(~i)));
        }
    }

    return memErrorSeen;
}

/** @brief Test volatile memory (SRAM/DRAM) for defects without using stack

    This function implements the same memory test that the function
    BCM_MemTest implements (on the 'reminder' memory region in its
    implementation). This function though does not use stack, and thus
    is suitable in situations where the stack isn't setup yet.

    The intent is that the callers use this API first to test a
    small memory region. This tested memory region can then be
    used to allocate the stack space, and call into much faster
    BCM_MemTest API to test a larger memory region.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects. The memory region must be non-cacheable.
    @param[in]       aLen        Length of the memory to be tested

    Return values are documented in reverse-chronological order
    @retval        0             if no defects were uncovered
    @retval        1             if a certain defect was encountered during the memory testing

    @post Tested memory range may have been filled with garbage.

    @trace #BRCM_SWARCH_BCM_MEMTESTNOSTACK_PROC
    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations None

*/
uint32_t BCM_MemTestNoStack(void *const aMemAddr, const uint32_t aLen)
{
    uint32_t ret;

    asm volatile (
        "        movs    r2, #0         \n"
        "        b.n     b1             \n"
        "loop1:  strb    r2, [r0, r2]   \n"
        "        adds    r2, #1         \n"
        "b1:     cmp     r2, r1         \n"
        "        bne.n   loop1          \n"
        "        mov.w   ip, #0         \n"
        "        movs    r2, #0         \n"
        "        b.n     b2             \n"
        "loop2:  ldrb    r3, [r0, r2]   \n"
        "        sub     r3, r2, r3     \n"
        "        ands.w  r3, r3, #255   \n"
        "        it      ne             \n"
        "        orrne.w ip, ip, #1     \n"
        "        mvns    r3, r2         \n"
        "        strb    r3, [r0, r2]   \n"
        "        adds    r2, #1         \n"
        "b2:     cmp     r2, r1         \n"
        "        bne.n   loop2          \n"
        "        b.n     b3             \n"
        "loop3:  ldrb    r2, [r0, r1]   \n"
        "        mvns    r3, r1         \n"
        "        sub     r3, r2, r3     \n"
        "        ands.w  r3, r3, #255   \n"
        "        it      ne             \n"
        "        orrne.w ip, ip, #1     \n"
        "b3:     subs    r1, #1         \n"
        "        bge.n   loop3          \n"
        "        mov     %0, ip         \n"
        : "=r" (ret)
        ::
    );

    return ret;
}
/** @brief Perform march-C test

    This function implements the march-C memory test with a unique pattern provided.
    The memory under test should be configured as device memory.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects. The memory region must be device memory.
    @param[in]       aLen        Length of the memory to be tested in words
    @param[in]       aPattern    Unique pattern to be used for test

    Return values are documented in reverse-chronological order
    @retval   BCM_ERR_OK         If no defects are uncovered
    @retval   BCM_ERR_DATA_INTEG If a certain defect is encountered during the memory testing

    @post Tested memory range may have been filled with garbage.

    @trace #BRCM_SWARCH_BCM_MEMTESTMARCHC_PROC
    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations This memory should be configured as device memory

*/
int32_t BCM_MemTestMarchC(volatile uint32_t *const aMemAddr, const uint32_t aLen, const uint32_t aPattern)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;
    uint32_t inversePattern = ~aPattern;

    /* inc address, write a */
    for (i = 0UL; i < aLen; i++) {
        aMemAddr[i] = aPattern;
    }

    /* inc address, read a write b */
    for (i = 0UL; i < aLen; i++) {
        if (aPattern != aMemAddr[i]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
        aMemAddr[i] = inversePattern;
    }

    /* inc address, read b write a */
    for (i = 0UL; i < aLen; i++) {
        if (inversePattern != aMemAddr[i]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
        aMemAddr[i] = aPattern;
    }

    /* dec address, read a write b */
    for (i = aLen ; i > 0UL; i--) {
        if (aPattern != aMemAddr[i - 1UL]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
        aMemAddr[i - 1UL] = inversePattern;
    }

    /* dec address, read b write a */
    for (i = aLen ; i > 0UL; i--) {
        if (inversePattern != aMemAddr[i - 1UL]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
        aMemAddr[i - 1UL] = aPattern;
    }

    /* dec address, read a */
    for (i = aLen ; i > 0UL; i--) {
        if (aPattern != aMemAddr[i - 1UL]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
    }

exit:
    return retVal;
}

/** @brief Perform Hammer test

    This function implements the Hammer memory test with a unique pattern provided.
    The memory under test should be configured as device memory.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects. The memory region must be device memory.
    @param[in]       aLen        Length of the memory to be tested in words
    @param[in]       aPattern    Unique pattern to be used for test

    Return values are documented in reverse-chronological order
    @retval   BCM_ERR_OK         If no defects are uncovered
    @retval   BCM_ERR_DATA_INTEG If a certain defect is encountered during the memory testing

    @post Tested memory range may have been filled with garbage.

    @trace #BRCM_SWARCH_BCM_MEMTESTHAMMER_PROC
    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations This memory should be configured as device memory

*/
int32_t BCM_MemTestHammer(volatile uint32_t *const aMemAddr, const uint32_t aLen, const uint32_t aPattern)
{
    uint32_t i,j,k;
    int32_t retVal = BCM_ERR_OK;
    uint32_t inversePattern = ~aPattern;

    /* inc address, write a */
    for (i = 0UL; i < aLen; i++) {
        aMemAddr[i] = aPattern;
    }

    for (i = 0UL; i < 2UL; i++) {

        /* inc address, read a write b 10 times, read b*/
        for (j = 0UL; j < aLen; j++) {
            if (aPattern != aMemAddr[j]) {
                retVal = BCM_ERR_DATA_INTEG;
                goto exit;
            }

            for (k = 0UL; k < 10UL; k++) {
                aMemAddr[j] = inversePattern;
            }

            if (inversePattern != aMemAddr[j]) {
                retVal = BCM_ERR_DATA_INTEG;
                goto exit;
            }
        }

        /* dec address, read b write a 10 times, read a*/
        for (j = aLen; j > 0UL; j--) {
            if (inversePattern != aMemAddr[j - 1UL]) {
                retVal = BCM_ERR_DATA_INTEG;
                goto exit;
            }

            for (k = 0UL; k < 10UL; k++) {
                aMemAddr[j - 1UL] = aPattern;
            }

            if (aPattern != aMemAddr[j - 1UL]) {
                retVal = BCM_ERR_DATA_INTEG;
                goto exit;
            }
        }
    }

exit:
    return retVal;
}
/** @} */

