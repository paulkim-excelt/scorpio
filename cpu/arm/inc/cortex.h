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
    @defgroup grp_cortex_common Cortex Special Instructions
    @ingroup grp_cortex

    @addtogroup grp_cortex_common
    @{

    @file cortex.h
    @brief ARM Cortex Special instructions
    This header file contains the interface functions/macros for ARM Cortex
    Processor architecture special instructions

    @version 0.1 Initial Version
*/

#ifndef ARM_H
#define ARM_H

#include <compiler.h>
#include <inttypes.h>

/**
    @name Cortex Special Instructions Architecture IDs
    @{
    @brief Architecture IDs for Cortex Special Instructions

*/
#define BRCM_SWARCH_CORTEX_DMB_MACRO     (0x10U)   /**< @brief #CORTEX_DMB       */
#define BRCM_SWARCH_CORTEX_DSB_MACRO     (0x11U)   /**< @brief #CORTEX_DSB       */
#define BRCM_SWARCH_CORTEX_ISB_MACRO     (0x12U)   /**< @brief #CORTEX_ISB       */
#define BRCM_SWARCH_CORTEX_WFI_MACRO     (0x13U)   /**< @brief #CORTEX_WFI       */
#define BRCM_SWARCH_CORTEX_CLZ_PROC      (0x14U)   /**< @brief #CORTEX_Clz       */
#define BRCM_SWARCH_CORTEX_RBIT_PROC     (0x15U)   /**< @brief #CORTEX_BitReverse*/
#define BRCM_SWARCH_CORTEX_LDREX_PROC    (0x16U)   /**< @brief #CORTEX_LdrEx     */
#define BRCM_SWARCH_CORTEX_STREX_PROC    (0x17U)   /**< @brief #CORTEX_StrEx     */
/** @} */

/**
    @brief Data Memory Barrier

    Data Memory Barrier instruction for ARM Cortex Architecture.

    @trace #BRCM_SWREQ_CORTEX_COMMON
*/
#define CORTEX_DMB()   COMP_ASM("dmb")

/**
    @brief Data Synchronization Barrier

    Data Synchronization Barrier instruction for ARM Cortex Architecture.

    @trace #BRCM_SWREQ_CORTEX_COMMON
*/
#define CORTEX_DSB()   COMP_ASM("dsb")

/**
    @brief Instruction Synchronization Barrier

    Instruction Synchronization Barrier instruction for ARM Cortex Architecture.

    @trace #BRCM_SWREQ_CORTEX_COMMON
*/
#define CORTEX_ISB()   COMP_ASM("isb")

/**
    @brief Wait for Interrupt

    Wait for Interrupt instruction for ARM Cortex Architecture.

    @trace #BRCM_SWREQ_CORTEX_COMMON
*/
#define CORTEX_WFI()   COMP_ASM("wfi")

/** @brief Compute Leading Zeros

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       32-bit data in which leading zeros to be counted

    @return uint32_t containing number of leading zeros in @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint32_t CORTEX_Clz(uint32_t aData)
{
    return (uint32_t)__builtin_clz(aData);
}

/** @brief Perform Bit Position Reversal

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       32-bit input which needs to be bit reversed

    @return uint32_t containing reversed bit positions in @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint32_t CORTEX_BitReverse(uint32_t aData)
{
    uint32_t val;

#if defined(GNU) || defined(ARMCC)
    COMP_ASM( "rbit %0, %1"
                :"=r"(val)
                :"r"(aData)
                :"cc");
#else
#error Unsupported compiler
#endif
    return val;
}

/** @brief Perform Exclusive Load

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       pointer from which Exclusive load need
                                to be performed

    @return uint32_t value at @a aAddr

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint32_t CORTEX_LdrEx(volatile uint32_t *const aAddr)
{
    uint32_t val = 0UL;
#if defined(GNU) || defined(ARMCC)
    COMP_ASM( "ldrex %0, [%1]"
        :"=r"(val)
        :"r"(aAddr)
        :"cc");
#else   /* !ARMCC */
#error Unsupported compiler
#endif /* ARMCC */
    return val;
}

/** @brief Perform Exclusive Store

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr   pointer to which Exclusive store need
                            to be performed

    @retval         0UL     Exclusive store successful
    @retval         1UL     Exclusive store failed

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint32_t CORTEX_StrEx(volatile uint32_t * const aAddr, const uint32_t aVal)
{
    uint32_t status = 0UL;

#if defined(GNU) || defined(ARMCC)
    COMP_ASM(
            "mov r1, %1;"
            "strex r0, r1, [%2];"
            "mov %0, r0;"
            :"=r"(status)
            :"r"(aVal), "r"(aAddr)
            :"r0", "r1");
#else   /* !ARMCC */
#error Unsupported compiler
#endif /* ARMCC */

    return status;
}

#endif /* ARM_H */
/** @} */
