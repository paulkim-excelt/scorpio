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
    @defgroup grp_memtest_ifc Memory tests
    @ingroup grp_memtest

    @addtogroup grp_memtest_ifc
    @{
    @section sec_err_overview Overview
    Memory testing routines

    @file bcm_memtest.h
    @brief Memory tests
    This header file contains the interface functions for some memory
    testing routines.
    @version 1.0 Initial Version
*/

#ifndef BCM_MEMTEST_H
#define BCM_MEMTEST_H

#include <stdint.h>

/**
    @name BCM MemTest API IDs
    @{
    @brief API IDs for BCM MemTest APIs
*/
#define BRCM_SWARCH_BCM_MEMTEST_PROC        (0x10U)     /**< @brief #BCM_MemTest        */
#define BRCM_SWARCH_BCM_MEMTESTNOSTACK_PROC (0x11U)     /**< @brief #BCM_MemTestNoStack */
#define BRCM_SWARCH_BCM_MEMTESTMARCHC_PROC  (0x12U)     /**< @brief #BCM_MemTestMarchC  */
#define BRCM_SWARCH_BCM_MEMTESTHAMMER_PROC  (0x13U)     /**< @brief #BCM_MemTestHammer  */
/** @} */

/** @brief Test the volatile memory (SRAM/DRAM) for defects

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects. This pointer has to be aligned to a cache
                                 line size. Failure to do so will result in an undefined
                                 behaviour
    @param[in]       aLen        Length of the memory to be tested

    Return values are documented in reverse-chronological order
    @retval        0             if no defects were uncovered
    @retval        1             if a certain defect was encountered during the memory testing

    @post None

    @trace  #BRCM_SWREQ_BCM_MEM_TEST
*/
uint32_t BCM_MemTest(void *const aMemAddr, const uint32_t aLen);

/** @brief Test volatile memory (SRAM/DRAM) for defects without using stack

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aMemAddr    Start address of the piece of memory to be tested
                                 for defects. The memory region must be non-cacheable.
    @param[in]       aLen        Length of the memory to be tested

    Return values are documented in reverse-chronological order
    @retval        0             if no defects were uncovered
    @retval        1             if a certain defect was encountered during the memory testing

    @post None

    @trace  #BRCM_SWREQ_BCM_MEM_TEST
*/
uint32_t BCM_MemTestNoStack(void *const aMemAddr, const uint32_t aLen);

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

    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations This memory should be configured as device memory

*/
int32_t BCM_MemTestMarchC(volatile uint32_t *const aMemAddr, const uint32_t aLen, const uint32_t aPattern);

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

    @trace #BRCM_SWREQ_BCM_MEM_TEST

    @limitations This memory should be configured as device memory

*/
int32_t BCM_MemTestHammer(volatile uint32_t *const aMemAddr, const uint32_t aLen, const uint32_t aPattern);
#endif /* BCM_MEMTEST_H */
/** @} */

