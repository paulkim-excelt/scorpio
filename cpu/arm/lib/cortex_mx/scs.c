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
    @defgroup grp_cortex_mx_scs_impl MPU related implementation
    @ingroup grp_cortex_mx

    @addtogroup grp_cortex_mx_scs_impl
    @{

    @file cpu/arm/cortex_mx/scs.c
    @brief Implementation of system control related functionality for
    Cortex-M

    This source file contains the implementation of system control related
    functions for Cortex-M Processors

    @version 0.1 Initial version
*/

#include <compiler.h>
#include <cortex_mx.h>
#include <interrupt.h>

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component
*/
#define BRCM_SWDSGN_CORTEX_MX_SCS_TYPE           (0x90U) /**< @brief #CORTEX_MX_SCSType              */
#define BRCM_SWDSGN_CORTEX_MX_AIRCR_MACRO        (0x91U) /**< @brief #CORTEX_MX_AIRCR_VECTRESET_MASK */
#define BRCM_SWDSGN_CORTEX_MX_CACHE_MACRO        (0x91U) /**< @brief #CORTEX_MX_CACHE_ENABLE_ICACHE  */
#define BRCM_SWDSGN_CORTEX_MX_SCS_MACRO          (0x92U) /**< @brief #CORTEX_MX_SCS_ACTLR            */
#define BRCM_SWDSGN_CORTEX_MX_SYSTEM_RESET_PROC  (0x93U) /**< @brief #CORTEX_MX_SystemReset          */
#define BRCM_SWDSGN_CORTEX_MX_VECTOR_RESET_PROC  (0x94U) /**< @brief #CORTEX_MX_VectorReset          */
#define BRCM_SWDSGN_CORTEX_MX_ICACHE_ENABLE_PROC (0x95U) /**< @brief #CORTEX_MX_ICacheEnable         */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_LNSIZE_PROC (0x96U) /**< @brief #CORTEX_MX_DCacheGetLineSize    */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_CLINV_PROC  (0x97U) /**< @brief #CORTEX_MX_DCacheCleanInvalidate*/
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_ENABLE_PROC (0x98U) /**< @brief #CORTEX_MX_DCacheEnable         */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_CLEAN_PROC  (0x99U) /**< @brief #CORTEX_MX_DCacheCleanAll       */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_CLRNG_PROC  (0x9AU) /**< @brief #CORTEX_MX_DCacheCleanRange     */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_INRNG_PROC  (0x9BU) /**< @brief #CORTEX_MX_DCacheInvalRange     */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_FLRNG_PROC  (0x9CU) /**< @brief #CORTEX_MX_DCacheFlushRange     */
#define BRCM_SWDSGN_CORTEX_MX_INIT_SYSTICK_PROC  (0x9DU) /**< @brief #CORTEX_MX_SysTickInit          */
#define BRCM_SWDSGN_CORTEX_MX_GTCNT_SYSTICK_PROC (0x9EU) /**< @brief #CORTEX_MX_SysTickGetCnt        */
#define BRCM_SWDSGN_CORTEX_MX_SYST_MACRO         (0x9FU) /**< @brief #CORTEX_MX_SYST_ENABLE          */
#define BRCM_SWDSGN_CORTEX_MX_NVIC_TYPE          (0xA0U) /**< @brief #CORTEX_MX_NVICType             */
#define BRCM_SWDSGN_CORTEX_MX_NVIC_MACRO         (0xA1U) /**< @brief #CORTEX_MX_NVIC_BASE            */
#define BRCM_SWDSGN_CORTEX_MX_NVIC_PROC          (0xA2U) /**< @brief #CORTEX_MX_NVICClearPending     */
#define BRCM_SWDSGN_CORTEX_MX_INTR_ENABLE_PROC   (0xA3U) /**< @brief #CORTEX_MX_INTREnable           */
#define BRCM_SWDSGN_CORTEX_MX_INTR_DISABLE_PROC  (0xA4U) /**< @brief #CORTEX_MX_INTRDisable          */
#define BRCM_SWDSGN_CORTEX_MX_ICACHE_DISABLE_PROC (0xA5U) /**< @brief #CORTEX_MX_ICacheDisable       */
#define BRCM_SWDSGN_CORTEX_MX_DCACHE_DISABLE_PROC (0xA6U) /**< @brief #CORTEX_MX_DCacheDisable       */
#define BRCM_SWDSGN_CORTEX_MX_CPASR_MACRO         (0xA7U) /**< @brief #CORTEX_MX_CPASR_FPU_MASK      */
#define BRCM_SWDSGN_CORTEX_MX_FPU_ENABLE_PROC     (0xA8U) /**< @brief #CORTEX_MX_FPUEnable           */
#define BRCM_SWDSGN_CORTEX_MX_FPU_DISABLE_PROC    (0xA9U) /**< @brief #CORTEX_MX_FPUDisable          */
#define BRCM_SWDSGN_CORTEX_MX_SET_THREADMODE_PROC (0xAAU)   /**< @brief #CORTEX_MX_SetThreadMode */
/** @} */

/**
    @brief System Control block structure for Cortex-M

    @trace #BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
typedef volatile struct _CORTEX_MX_SCSType {
    uint32_t ACTLR;
    uint32_t RSVD;
    uint32_t SYSTCSR;
    uint32_t SYSTRVR;
    uint32_t SYSTCVR;
    uint32_t SYSTCALIB;
    uint32_t RSVD0[(0xd00-0x020)/4];
    /* Offset 0xd00 */
    uint32_t CPUID;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint32_t SYSTEMPRIORITY[3];
    uint32_t SHCSR;
    uint32_t CFSR;
    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t MMFAR;
    uint32_t BFAR;
    uint32_t RSVD1[(0xd80-0xd3c)/4];
    /* Offset 0x0d80 */
    uint32_t CCSIDR;
    uint32_t CSSELR;
    uint32_t CPACR;
    uint32_t RSVD2[(0xF00-0xd8C)/4];
    /* Offset 0x0F00 */
    uint32_t STIR;
    uint32_t RSVD3[(0xF50-0xF04)/4];
    /* Offset 0x0F50 */
    uint32_t ICIALLU;
    uint32_t RSVD4;
    uint32_t ICIMVAU;
    uint32_t DCIMVAC;
    uint32_t DCISW;
    uint32_t DCCMVAU;
    uint32_t DCCMVAC;
    uint32_t DCCSW;
    uint32_t DCCIMVAC;
    uint32_t DCCISW;
} CORTEX_MX_SCSType;

/**
    @name Application Interrupt and Reset Control Register masks
    @{
    @brief Macros for bit fields in Application Interrupt and Reset
    Control Register

    @trace #BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_AIRCR_VECTRESET_MASK            (0x1UL)
#define CORTEX_MX_AIRCR_VECTRESET_SHIFT           (0UL)
#define CORTEX_MX_AIRCR_VECTCLRACTIVE_MASK        (0x2UL)
#define CORTEX_MX_AIRCR_VECTCLRACTIVE_SHIFT       (1UL)
#define CORTEX_MX_AIRCR_SYSRESETREQ_MASK          (0x4UL)
#define CORTEX_MX_AIRCR_SYSRESETREQ_SHIFT         (2UL)
#define CORTEX_MX_AIRCR_PRIGROUP_MASK             (0x700UL)
#define CORTEX_MX_AIRCR_PRIGROUP_SHIFT            (8UL)
#define CORTEX_MX_AIRCR_ENDIANNESS_MASK           (0x8000UL)
#define CORTEX_MX_AIRCR_ENDIANNESS_SHIFT          (15UL)
#define CORTEX_MX_AIRCR_VECTKEY_MASK              (0xFFFF0000UL)
#define CORTEX_MX_AIRCR_VECTKEY_SHIFT             (16UL)
#define CORTEX_MX_AIRCR_VECTKEY_KEY               (0x05FAUL)
/** @} */

/**
    @name Cortex-M Cache Control
    @{
    @brief Macros for bit fields in Cache Control

    @trace #BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_CACHE_ENABLE_ICACHE                (0x20000UL)
#define CORTEX_MX_CACHE_ENABLE_DCACHE                (0x10000UL)
#define CORTEX_MX_CACHE_CCSIDR_LINESIZE_SHIFT        (0UL)
#define CORTEX_MX_CACHE_CCSIDR_LINESIZE_MASK         (0x7UL)
#define CORTEX_MX_CACHE_CCSIDR_ASSOCIATIVITY_SHIFT   (3UL)
#define CORTEX_MX_CACHE_CCSIDR_ASSOCIATIVITY_MASK    (0x3FFUL << CORTEX_MX_CACHE_CCSIDR_ASSOCIATIVITY_SHIFT)
#define CORTEX_MX_CACHE_CCSIDR_NUMSETS_SHIFT         (13UL)
#define CORTEX_MX_CACHE_CCSIDR_NUMSETS_MASK          (0x7FFFUL << CORTEX_MX_CACHE_CCSIDR_NUMSETS_SHIFT)
/** @} */

/**
    @name Cortex-M SysTick Control
    @{
    @brief Macros for bit fields in SysTick Control

    @trace #BRCM_SWARCH_CORTEX_MX_INIT_SYSTICK_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_GTCNT_SYSTICK_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_SYST_ENABLE                 (1UL)
#define CORTEX_MX_SYST_DISABLE                (0UL)
#define CORTEX_MX_SYST_TICKINT_DISABLE        (0UL)
#define CORTEX_MX_SYST_TICKINT_ENABLE         (1UL)
#define CORTEX_MX_SYST_TICKINT_BIT_SHIFT      (1UL)
#define CORTEX_MX_SYST_CLKSRC_EXTERNAL        (0UL)
#define CORTEX_MX_SYST_CLKSRC_PROCESSOR       (1UL)
#define CORTEX_MX_SYST_CLKSRC_BIT_SHIFT       (2UL)
#define CORTEX_MX_SYST_COUNTER_MAX            (0xFFFFFFUL)
/** @} */

/**
    @brief Macro for CPU ID base address

    @trace #BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_SCS_ACTLR    (0xE000E008UL)

/**
    @brief Macros for the bit fields in the co-processor access control register

    @trace #BRCM_SWARCH_CORTEX_MX_FPU_ENABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_FPU_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_CPASR_FPU_MASK  (0x00F00000UL)

/**
    @brief Struct representing NVIC registers

    @trace #BRCM_SWARCH_CORTEX_MX_INTR_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
 */
typedef volatile struct _CORTEX_MX_NVICType {
    uint32_t ICTR;     /**< Intrerrupt Controller Type Register */
    uint32_t ISER[32]; /**< Interrupt Set-Enable Registers;
                            only ISER[0] - ISER[7] are valid */
    uint32_t ICER[32]; /**< Interrupt Clear-Enable Registers;
                            only ICER[0] - ICER[7] are valid */
    uint32_t ISPR[32]; /**< Interrupt Set-Pending Registers;
                            only ISPR[0] - ISPR[7] are valid */
    uint32_t ICPR[32]; /**< Interrupt Clear-Pending Registers;
                            only ICPR[0] - ICPR[7] are valid */
    uint32_t IABR[64]; /**< Interrupt Active Bit Register;
                            only IABR[0] - IABR[7] are valid */
    uint32_t IPR[64];  /**< Interrupt Priority Register;
                            only IPR[0] - IPR[59] are valid */
} CORTEX_MX_NVICType;

/**
    @name NVIC macros
    @{
    @brief Macros for NVIC operation

    @trace #BRCM_SWARCH_CORTEX_MX_INTR_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
#define CORTEX_MX_NVIC_BASE              (0xE000E100UL)
#define CORTEX_MX_NVIC_INTLINESNUM_MASK  (0xFUL)
#define CORTEX_MX_NVIC_CLEAR_PENDING     (0xFFFFFFFFUL)   /**< Clear all pending interrupt */
/** @} */

/** @brief Clear all pending interrupts in NVIC

    @behavior Sync, Non-reentrant

    @pre Interrupts are disabled

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWARCH_CORTEX_MX_INTR_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX

    @limitations None

    @code{.c}
    for (i=0; i< NVIC->ICTR; i++)
        NVIC->ICPR[i] = CORTEX_MX_NVIC_CLEAR_PENDING
    @endcode

*/
static void CORTEX_MX_NVICClearPending(void)
{
    CORTEX_MX_NVICType *nvic = (CORTEX_MX_NVICType *)CORTEX_MX_NVIC_BASE;
    uint32_t  i;
    uint32_t max = (nvic->ICTR + 1UL) << 5UL;

    for (i = 0UL; i < max; i++) {
        nvic->ICPR[i] = CORTEX_MX_NVIC_CLEAR_PENDING;
    }
}

/** @} */

/**
    @addtogroup grp_cortex_mx
    @{
*/

/**
    @trace #BRCM_SWARCH_CORTEX_MX_SYSTEM_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_SystemReset(void)
{
    uint32_t whileCondition = 1UL;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;
    uint32_t reg = scs->AIRCR;

    reg &= ~CORTEX_MX_AIRCR_VECTKEY_MASK;
    reg |= ((CORTEX_MX_AIRCR_VECTKEY_KEY << CORTEX_MX_AIRCR_VECTKEY_SHIFT) |
            CORTEX_MX_AIRCR_SYSRESETREQ_MASK);
    scs->AIRCR = reg;

    CORTEX_DSB();
    /* System reset asserts an reset request to external
       system to perform the reset which might be asyn
       Shall it wait forever or timeout? */
    while (1UL == whileCondition) { }       /* infinite loop is made intentionally */
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_VECTOR_RESET_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_VectorReset(void)
{
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;
    uint32_t reg = scs->AIRCR;

    reg &= ~CORTEX_MX_AIRCR_VECTKEY_MASK;
    reg |= ((CORTEX_MX_AIRCR_VECTKEY_KEY << CORTEX_MX_AIRCR_VECTKEY_SHIFT) |
            CORTEX_MX_AIRCR_VECTRESET_MASK);
    scs->AIRCR = reg;

    CORTEX_DSB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_ICACHE_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_ICacheEnable(void)
{
    /*
    This function is implemented in accordance with Section 4.1.3 (Initializing
    and enabling the L1 cache) in Cortex-M7 TRM r0p2 */
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    INTR_FlagType flags;

    flags = INTR_Suspend();
    scs->ICIALLU = 0UL;
    CORTEX_DSB();
    CORTEX_ISB();
    scs->CCR = (scs->CCR) | CORTEX_MX_CACHE_ENABLE_ICACHE;
    CORTEX_DSB();
    CORTEX_ISB();
    INTR_Resume(flags);
}

/**
    @code{.c}
    DSB()
    ISB()
    *BL_CCR &= ~0x2     Disable ICache
    REG[ICIALLU] = 0    invalidate I$
    DSB()
    ISB()
    @endcode
    @trace #BRCM_SWARCH_CORTEX_MX_ICACHE_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_ICacheDisable(void)
{
    /*
    This function is implemented in accordance with Section 4.1.3 (Initializing
    and enabling the L1 cache) in Cortex-M7 TRM r0p2 */
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    INTR_FlagType flags;

    flags = INTR_Suspend();
    CORTEX_DSB();
    CORTEX_ISB();
    scs->CCR = (scs->CCR) & (~CORTEX_MX_CACHE_ENABLE_ICACHE);
    scs->ICIALLU = 0UL;
    CORTEX_DSB();
    CORTEX_ISB();
    INTR_Resume(flags);
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_ENABLE_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_CLEAN_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_CLRNG_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_INRNG_PROC
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_FLRNG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
static uint32_t CORTEX_MX_DCacheGetLineSize(void)
{
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;
    /* Reset value of CCSELR (cache size selection register)
       in undefined, so set Dcache in CCSELR before accessing
       CCSIDR register */
    scs->CSSELR = (0x0UL << 1) | 0UL; /* level 1 data cache */
    CORTEX_DSB();
    return (scs->CCSIDR & CORTEX_MX_CACHE_CCSIDR_LINESIZE_MASK);
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
static void CORTEX_MX_DCacheCleanInvalidate(uint32_t aIsCleanNeeded,
                                            uint32_t aIsInvalNeeded)
{
    uint32_t setShift;
    uint32_t wayShift;
    uint32_t nSets;
    uint32_t nWays;
    uint32_t lineSize;
    uint32_t sw;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    /*  calculate set/ways shift required for DCISW write
        (ARM cortex v7m architecture reference manual, B2.2.7
        (Cache and branch predictor maintenance operations)
        Reset value of CCSELR (cache size selection register)
        in undefined, so set Dcache in CCSELR before accessing
        CCSIDR register */
    scs->CSSELR = (0x0UL << 1) | 0UL; /* level 1 data cache */
    CORTEX_DSB();
    lineSize = CORTEX_MX_DCacheGetLineSize();
    nSets = (scs->CCSIDR & CORTEX_MX_CACHE_CCSIDR_NUMSETS_MASK)
                 >> CORTEX_MX_CACHE_CCSIDR_NUMSETS_SHIFT;
    nWays = (scs->CCSIDR & CORTEX_MX_CACHE_CCSIDR_ASSOCIATIVITY_MASK)
                 >> CORTEX_MX_CACHE_CCSIDR_ASSOCIATIVITY_SHIFT;
    setShift = lineSize + 0x4UL;
    wayShift = (uint32_t)__builtin_clz(nWays) & 0x1FUL;

    /* iterate over all the sets and ways and invalidate it */
    do {
        uint32_t i = nWays;
        do {
            sw = ((i << wayShift) | (nSets << setShift));
            if ((1UL == aIsCleanNeeded) && (1UL == aIsInvalNeeded)) {
                scs->DCCISW = sw;  /* Clean and Invalidate */
            } else if ((0UL == aIsCleanNeeded) && (1UL == aIsInvalNeeded)) {
                scs->DCISW = sw;  /* Invalidate set way */
            } else if ((1UL == aIsCleanNeeded) && (0UL == aIsInvalNeeded)) {
                scs->DCCSW = sw;  /* Clean set way */
            } else {
                break;
            }
        } while ((i--) != 0UL);
    } while ((nSets--) != 0UL);

    CORTEX_DSB();
    CORTEX_ISB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheEnable(void)
{
    INTR_FlagType flags;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    flags = INTR_Suspend();
    CORTEX_MX_DCacheCleanInvalidate(0UL, 1UL);
    scs->CCR = (scs->CCR) | CORTEX_MX_CACHE_ENABLE_DCACHE;
    CORTEX_DSB();
    CORTEX_ISB();
    INTR_Resume(flags);
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_CLEAN_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheCleanAll(void)
{
    INTR_FlagType flags;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (0UL != ((scs->CCR) & CORTEX_MX_CACHE_ENABLE_DCACHE)) {
        flags = INTR_Suspend();
        CORTEX_MX_DCacheCleanInvalidate(1UL, 0UL);
        INTR_Resume(flags);
    }
}

/**
    @code{.c}
    DSB()
    ISB()
    Clean & Invalidate DCache
    Disable DCache
    DSB()
    ISB()
    @endcode
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheDisable(void)
{
    /*
    This function is implemented in accordance with Section 4.1.3 (Initializing
    and enabling the L1 cache) in Cortex-M7 TRM r0p2 */
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    INTR_FlagType flags;

    flags = INTR_Suspend();
    CORTEX_MX_DCacheCleanInvalidate(1UL, 1UL);
    scs->CCR = (scs->CCR) & (~CORTEX_MX_CACHE_ENABLE_DCACHE);
    CORTEX_DSB();
    CORTEX_ISB();
    INTR_Resume(flags);
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_CLRNG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheCleanRange(uint32_t aAddr, uint32_t aSize)
{
    uint32_t dAddr = aAddr;
    uint32_t dSize = aSize;
    uint32_t lineSize;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (0UL != ((scs->CCR) & CORTEX_MX_CACHE_ENABLE_DCACHE)) {
        CORTEX_DSB();
        lineSize = CORTEX_MX_DCacheGetLineSize();

        while (dSize > 0UL) {
            scs->DCCMVAU = dAddr; /* clean to PoU  */
            dAddr += (1UL << (lineSize + 0x04UL));
            dSize -= (1UL << (lineSize + 0x04UL));
        }

        CORTEX_DSB();
        CORTEX_ISB();
    }
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_INRNG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheInvalRange(uint32_t aAddr, uint32_t aSize)
{
    uint32_t dAddr = aAddr;
    uint32_t dSize = aSize;
    uint32_t lineSize;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (0UL != ((scs->CCR) & CORTEX_MX_CACHE_ENABLE_DCACHE)) {
        CORTEX_DSB();
        lineSize = CORTEX_MX_DCacheGetLineSize();

        while (dSize > 0UL) {
            scs->DCIMVAC = dAddr; /* invalidate to PoC */
            dAddr += (1UL << (lineSize + 0x04UL));
            dSize -= (1UL << (lineSize + 0x04UL));
        }

        CORTEX_DSB();
        CORTEX_ISB();
    }
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_DCACHE_FLRNG_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_DCacheFlushRange(uint32_t aAddr, uint32_t aSize)
{
    uint32_t dAddr = aAddr;
    uint32_t dSize = aSize;
    uint32_t lineSize;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (0UL != ((scs->CCR) & CORTEX_MX_CACHE_ENABLE_DCACHE)) {
        CORTEX_DSB();
        lineSize = CORTEX_MX_DCacheGetLineSize();

        while (dSize > 0UL) {
            scs->DCCIMVAC = dAddr; /* invalidate to PoC */
            dAddr += (1UL << (lineSize + 0x04UL));
            dSize -= (1UL << (lineSize + 0x04UL));
        }

        CORTEX_DSB();
        CORTEX_ISB();
    }
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_INIT_SYSTICK_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
int32_t CORTEX_MX_SysTickInit(uint32_t aTickCnt)
{
    int32_t retVal;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (aTickCnt > CORTEX_MX_SYST_COUNTER_MAX) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (CORTEX_MX_SYST_ENABLE == (scs->SYSTCSR & CORTEX_MX_SYST_ENABLE)) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        scs->SYSTCSR = 0UL;
        scs->SYSTRVR = aTickCnt;
        scs->SYSTCVR = 0UL;
        scs->SYSTCSR = (CORTEX_MX_SYST_CLKSRC_PROCESSOR << CORTEX_MX_SYST_CLKSRC_BIT_SHIFT)
                        | (CORTEX_MX_SYST_TICKINT_ENABLE << CORTEX_MX_SYST_TICKINT_BIT_SHIFT)
                        | CORTEX_MX_SYST_ENABLE;
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_GTCNT_SYSTICK_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
uint32_t CORTEX_MX_SysTickGetCnt(void)
{
    uint32_t cnt = 0UL;
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;

    if (CORTEX_MX_SYST_ENABLE == (scs->SYSTCSR & CORTEX_MX_SYST_ENABLE)) {
        cnt = scs->SYSTCVR;
    }

    return cnt;
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_INTR_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_INTREnable(void)
{
    CORTEX_MX_NVICClearPending();
    COMP_ASM("cpsie i");
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_INTR_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_INTRDisable(void)
{
    COMP_ASM("cpsid i");
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_FPU_ENABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_FPUEnable(void)
{
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;
    scs->CPACR |= CORTEX_MX_CPASR_FPU_MASK;
    CORTEX_DSB();
    CORTEX_ISB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_FPU_DISABLE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_FPUDisable(void)
{
    CORTEX_MX_SCSType *scs = (CORTEX_MX_SCSType *)CORTEX_MX_SCS_ACTLR;
    scs->CPACR &= ~CORTEX_MX_CPASR_FPU_MASK;
    CORTEX_DSB();
    CORTEX_ISB();
}

/**
    @trace #BRCM_SWARCH_CORTEX_MX_SET_THREADMODE_PROC
    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_MX_SetThreadMode(uint32_t aPriv, uint32_t aSpSel, uint32_t aStackPtr)
{
    COMP_ASM ("MRS r3, CONTROL;"
              "CMP %0, #0x0;"
              "ITE   EQ;"
              "BICEQ r3, r3, #0x1;"
              "ORRNE r3, r3, #0x1;"
              "CMP   %1, #0x1;"
              "ITT   EQ;"
              "MSREQ PSP, %2;"
              "ORREQ r3, r3, 0x2;"
              "MSR   CONTROL, r3;"
              "ISB;"
             ::"r" (aPriv), "r" (aSpSel), "r" (aStackPtr)
             :"r3");
}

/** @} */
