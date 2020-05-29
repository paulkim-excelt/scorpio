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
    @defgroup grp_bl_utils_arm ARM Utilities
    @ingroup grp_bl_utils

    @addtogroup grp_bl_utils_arm
    @{
    @section sec_bl_utils_arm_overview Overview
    ARM Utilities provide APIs for Endianness conversion

    @file bl_arm_utils.h
    @brief ARM specific utility functions

    This header file contains the interface functions for Endianness conversion

    @version 0.30 Import from Document
*/

#ifndef BL_ARM_UTILS_H
#define BL_ARM_UTILS_H

#include <bl_compiler.h>
#include <stdint.h>

/**
    @name ARM Utility API IDs
    @{
    @brief API IDs for ARM Utilities
*/
#define BRCM_ARCH_BL_UTILS_ARM_HOST2BE16_PROC       (0x00U) /**< @brief #BL_Host2BE16 */
#define BRCM_ARCH_BL_UTILS_ARM_HOST2LE16_PROC       (0x01U) /**< @brief #BL_Host2LE16 */
#define BRCM_ARCH_BL_UTILS_ARM_BE2HOST16_PROC       (0x02U) /**< @brief #BL_BE2Host16 */
#define BRCM_ARCH_BL_UTILS_ARM_LE2HOST16_PROC       (0x03U) /**< @brief #BL_LE2Host16 */
#define BRCM_ARCH_BL_UTILS_ARM_HOST2BE32_PROC       (0x04U) /**< @brief #BL_Host2BE32 */
#define BRCM_ARCH_BL_UTILS_ARM_HOST2LE32_PROC       (0x05U) /**< @brief #BL_Host2LE32 */
#define BRCM_ARCH_BL_UTILS_ARM_BE2HOST32_PROC       (0x06U) /**< @brief #BL_BE2Host32 */
#define BRCM_ARCH_BL_UTILS_ARM_LE2HOST32_PROC       (0x07U) /**< @brief #BL_LE2Host32 */
#define BRCM_ARCH_BL_UTILS_ARM_BE2HOST64_PROC       (0x08U) /**< @brief #BL_BE2Host64 */
#define BRCM_ARCH_BL_UTILS_ARM_HOST2BE64_PROC       (0x09U) /**< @brief #BL_Host2BE64 */
#define BRCM_ARCH_BL_UTILS_ARM_HOST2LE64_PROC       (0x0AU) /**< @brief #BL_Host2LE64 */
#define BRCM_ARCH_BL_UTILS_ARM_BIT_REVERSE_32_PROC  (0x0BU) /**< @brief #BL_BitReverse32 */
/** @} */

/** @brief Convert from host's endian format to big endian format

    This API converts 16-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint16_t    Data in big endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2BE16_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2BE16

    @limitations None
*/
BL_COMP_INLINE uint16_t BL_Host2BE16(uint16_t aData)
{
    uint16_t val;

#if defined(__GNU__)
    BL_COMP_ASM( "rev16 %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#elif defined(__ARMCC__)   /* !__GCC__ */
    BL_COMP_ASM( "rev16 val, aData");
#else   /* !__ARMCC__ */
#error Unsupported compiler
#endif /* __ARMCC__ */

    return val;
}

/** @brief Convert from host's endian format to little endian format

    This API converts 16-bit data from host's endian format to little endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint16_t    Data in little endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2LE16_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2LE16

    @limitations None
*/
BL_COMP_INLINE uint16_t BL_Host2LE16(uint16_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 16-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @retval     uint16_t    Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_BE2HOST16_PROC  #BCM_REQ_BL_UTILS_ARM_BE2HOST16

    @limitations None
*/
BL_COMP_INLINE uint16_t BL_BE2Host16(uint16_t aData)    { return BL_Host2BE16(aData); }

/** @brief Convert from little endian format to host's endian format

    This API converts 16-bit data from little endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in little endian format

    @retval     uint16_t    Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_LE2HOST16_PROC  #BCM_REQ_BL_UTILS_ARM_LE2HOST16

    @limitations None
*/
BL_COMP_INLINE uint16_t BL_LE2Host16(uint16_t aData)    { return aData; }

/** @brief Convert from host's endian format to big endian format

    This API converts 32-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint32_t    Data in big endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2BE32_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2BE32

    @limitations None
*/
BL_COMP_INLINE uint32_t BL_Host2BE32(uint32_t aData)
{
    uint32_t val;

#if defined(__GNU__)
    BL_COMP_ASM( "rev %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#elif defined(__ARMCC__)   /* !__GCC__ */
    BL_COMP_ASM( "rev val, aData");
#else   /* !__ARMCC__ */
#error Unsupported compiler
#endif /* __ARMCC__ */

    return val;
}

/** @brief Convert from host's endian format to little endian format

    This API converts 32-bit data from host's endian format to little endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint32_t    Data in little endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2LE32_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2LE32

    @limitations None
*/
BL_COMP_INLINE uint32_t BL_Host2LE32(uint32_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 32-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @retval     uint32_t    Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_BE2HOST32_PROC  #BCM_REQ_BL_UTILS_ARM_BE2HOST32

    @limitations None
*/
BL_COMP_INLINE uint32_t BL_BE2Host32(uint32_t aData)    { return BL_Host2BE32(aData); }

/** @brief Convert from little endian format to host's endian format

    This API converts 32-bit data from little endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in little endian format

    @retval     uint32_t    Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_LE2HOST32_PROC  #BCM_REQ_BL_UTILS_ARM_LE2HOST32

    @limitations None
*/
BL_COMP_INLINE uint32_t BL_LE2Host32(uint32_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 64-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @retval     uint64_t    Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_BE2HOST64_PROC  #BCM_REQ_BL_UTILS_ARM_BE2HOST64

    @limitations None
*/
BL_COMP_INLINE uint64_t BL_BE2Host64(uint64_t aData)
{
    return (((uint64_t)(BL_Host2BE32(
                 (uint32_t)((aData << 32UL) >> 32UL))) << 32UL) |
                 (uint32_t)BL_Host2BE32(((uint32_t)(aData >> 32UL))));
}

/** @brief Convert from host's endian format to big endian format

    This API converts 64-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint64_t    Data in big endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2BE64_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2BE64

    @limitations None
*/
BL_COMP_INLINE uint64_t BL_Host2BE64(uint64_t aData) { return BL_BE2Host64(aData); }

/** @brief Convert from host's endian format to little endian format

    This API converts 64-bit data from host's endian format to little endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @retval     uint64_t    Data in little endian format

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_HOST2LE64_PROC  #BCM_REQ_BL_UTILS_ARM_HOST2LE64

    @limitations None
*/
BL_COMP_INLINE uint64_t BL_Host2LE64(uint64_t aData)    { return aData; }

/** @brief Bit reverse 32 bit data

    This API bit reverses 32 bit data

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       data to be bit reversed

    @retval     uint32_t    Bit reversed value

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_ARM_BIT_REVERSE_32_PROC  #BCM_REQ_BL_UTILS_ARM_BIT_REVERSE_32

    @limitations None
*/
BL_COMP_INLINE uint32_t BL_BitReverse32(uint32_t aData)
{
    uint32_t val;

#if defined(__GNU__)
    BL_COMP_ASM( "rbit %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#elif defined(__ARMCC__)   /* !__GCC__ */
    BL_COMP_ASM( "rbit val, aData");
#else   /* !__ARMCC__ */
#error Unsupported compiler
#endif /* __ARMCC__ */

    return val;
}

#endif /* BL_ARM_UTILS_H */
/** @} */
