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
    @defgroup grp_arm_utils ARM Utilities
    @ingroup grp_system_sw

    @addtogroup grp_arm_utils
    @{
    @section sec_comp_overview Overview
    ARM Utilities provide APIs for Endianness conversion

    @file arm_utils.h
    @brief ARM specific utility functions

    This header file contains the interface functions for Endianness conversion

    @version 1.3 Import from Document
*/

#ifndef ARM_UTILS_H
#define ARM_UTILS_H

#include <compiler.h>
#include <stdint.h>

/** @brief Convert from host's endian format to big endian format

    This API converts 16-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @return Data in big endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint16_t Host2BE16(uint16_t aData)
{
    uint16_t val;

#if defined(__GNU__) || defined(__ARMCC__)
    COMP_ASM( "rev16 %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#else   /* !__ARMCC__ */
#error Unsupported compiler
#endif /* __ARMCC__ */

    return val;
}

/** @brief Convert from host's endian format to little endian format

    This API converts 16-bit data from host's endian format to little endian format.

    @behavior Sync, Re-entrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]      aData       Host data to be converted

    @return Data in little endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint16_t Host2LE16(uint16_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 16-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @return Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint16_t BE2Host16(uint16_t aData)    { return Host2BE16(aData); }

/** @brief Convert from little endian format to host's endian format

    This API converts 16-bit data from little endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]      aData       Data in litte endian format

    @return Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint16_t LE2Host16(uint16_t aData)    { return aData; }

/** @brief Convert from host's endian format to big endian format

    This API converts 32-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_BUSY           if currently executing something
    @retval     #BCM_ERR_INVAL_STATE    (aArg1->state is not GROUP_1)
    @retval     #BCM_ERR_INVAL_PARAMS   (aArg1 is NULL) or
                                        (aArg2 is NULL) or
                                        (aArg3 is NULL)
    @return Data in big endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint32_t Host2BE32(uint32_t aData)
{
    uint32_t val;

#if defined(__GNU__) || defined(__ARMCC__)
    COMP_ASM( "rev %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
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

    @return Data in little endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint32_t Host2LE32(uint32_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 32-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @return Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint32_t BE2Host32(uint32_t aData)    { return Host2BE32(aData); }

/** @brief Convert from little endian format to host's endian format

    This API converts 32-bit data from little endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in litte endian format

    @return Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint32_t LE2Host32(uint32_t aData)    { return aData; }

/** @brief Convert from big endian format to host's endian format

    This API converts 64-bit data from big endian format to host's endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Data in big endian format

    @return Data in host's endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint64_t BE2Host64(uint64_t aData)
{
    return (((uint64_t)(Host2BE32(
                 (uint32_t)((aData << 32UL) >> 32UL))) << 32UL) |
                 (uint32_t)Host2BE32(((uint32_t)(aData >> 32UL))));
}

/** @brief Convert from host's endian format to big endian format

    This API converts 64-bit data from host's endian format to big endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @return Data in big endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint64_t Host2BE64(uint64_t aData) { return BE2Host64(aData); }

/** @brief Convert from host's endian format to little endian format

    This API converts 64-bit data from host's endian format to little endian format.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aData       Host data to be converted

    @return Data in little endian format

    @post None

    @trace  #BRCM_ARCH_COMP_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
COMP_INLINE uint64_t Host2LE64(uint64_t aData)    { return aData; }

#endif /* ARM_UTILS_H */
/** @} */
