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
    @addtogroup grp_cortex_common
    @{

    @file cortex_fpu.h
    @brief ARM Cortex FPU Special instructions
    This header file contains the interface functions for ARM Cortex
    Processor implementing the FPUv5 ISA with single precision support.

    @version 0.1 Initial Version
*/

#ifndef CORTEX_FPU_H
#define CORTEX_FPU_H

#include <compiler.h>
#include <stdint.h>

/**
    @name Cortex FPU Special Instructions Architecture IDs
    @{
    @brief Architecture IDs for Cortex FPU Special Instructions

*/
#define BRCM_SWARCH_CORTEX_SQRTF_PROC           (0x10U) /**< @brief #CORTEX_Sqrtf           */
#define BRCM_SWARCH_CORTEX_FLOATTOFIXEDU16_PROC (0x11U) /**< @brief #CORTEX_FloatToFixedU16 */
#define BRCM_SWARCH_CORTEX_FLOATTOFIXEDS16_PROC (0x12U) /**< @brief #CORTEX_FloatToFixedS16 */
#define BRCM_SWARCH_CORTEX_FLOATTOFIXEDU32_PROC (0x13U) /**< @brief #CORTEX_FloatToFixedU32 */
#define BRCM_SWARCH_CORTEX_FLOATTOFIXEDS32_PROC (0x14U) /**< @brief #CORTEX_FloatToFixedS32 */
/** @} */

/** @brief Compute the square root

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Single precision floating point value whose
                                square root is to be computed

    @return Square root of @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline float CORTEX_Sqrtf(const float aData)
{
    float ret;
#if defined(GNU) || defined(ARMCC)
    __asm__ __volatile__ (
        "vsqrt.f32 %[ret], %[data]"
            : [ret] "=t" (ret)
            : [data] "t" (aData));
#else
#error "Unsupported compiler"
#endif
    return ret;
}

/** @brief Compute the 16 bit unsigned fixed point representation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Single precision floating point value whose
                                fixed point representation is to be computed
    @param[in]      aFracBits   Number of fraction bits in the fixed point
                                representation

    @return 16 bit unsigned fixed point representation of @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint16_t CORTEX_FloatToFixedU16(const float aData, const int32_t aFracBits)
{
    uint32_t ret;
#if defined(GNU)
    __asm__ __volatile__ (
      "VCVT.u16.f32 %[ret], %[data], %[fracBits]"
        : [ret] "=t" (ret)
        : [data] "0" (aData), [fracBits] "I" (aFracBits));
#else
    ret = (uint32_t)(aData * (1U << aFracBits));
#endif
    return ret;
}

/** @brief Compute the 16 bit signed fixed point representation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Single precision floating point value whose
                                fixed point representation is to be computed
    @param[in]      aFracBits   Number of fraction bits in the fixed point
                                representation

    @return 16 bit signed fixed point representation of @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline int16_t CORTEX_FloatToFixedS16(const float aData, const int32_t aFracBits)
{
    int32_t ret;
#if defined(GNU)
    __asm__ __volatile__ (
      "VCVT.s16.f32 %[ret], %[data], %[fracBits]"
        : [ret] "=t" (ret)
        : [data] "0" (aData), [fracBits] "I" (aFracBits));
#else
    ret = (int32_t)(aData * (1U << aFracBits));
#endif
    return ret;
}

/** @brief Compute the 32 bit unsigned fixed point representation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Single precision floating point value whose
                                fixed point representation is to be computed
    @param[in]      aFracBits   Number of fraction bits in the fixed point
                                representation

    @return 32 bit unsigned fixed point representation of @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline uint32_t CORTEX_FloatToFixedU32(const float aData, const uint32_t aFracBits)
{
    uint32_t ret;
#if defined(GNU)
    __asm__ __volatile__ (
      "VCVT.u32.f32 %[ret], %[data], %[fracBits]"
        : [ret] "=t" (ret)
        : [data] "0" (aData), [fracBits] "I" (aFracBits));
#else
    ret = (uint32_t)(aData * (1U << aFracBits));
#endif
    return ret;
}

/** @brief Compute the 32 bit signed fixed point representation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Single precision floating point value whose
                                fixed point representation is to be computed
    @param[in]      aFracBits   Number of fraction bits in the fixed point
                                representation

    @return 32 bit signed fixed point representation of @a aData

    @post None

    @trace #BRCM_SWREQ_CORTEX_COMMON

    @limitations None
*/
static inline int32_t CORTEX_FloatToFixedS32(const float aData, const uint32_t aFracBits)
{
    int32_t ret;
#if defined(GNU)
    __asm__ __volatile__ (
      "VCVT.s32.f32 %[ret], %[data], %[fracBits]"
        : [ret] "=t" (ret)
        : [data] "0" (aData), [fracBits] "I" (aFracBits));
#else
    ret = (int32_t)(aData * (1U << aFracBits));
#endif
    return ret;
}

#endif /* CORTEX_FPU_H */
/** @} */
