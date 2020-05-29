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
    @defgroup grp_utils_stdlib Standard Library Wrappers
    @ingroup grp_utils

    @addtogroup grp_utils_stdlib
    @{
    @section sec_stdlib_overview Overview
    This file specifies common utility wrappers for C Standard library APIs

    @file utils.h
    @brief C Standard library wrapper functiions

    @version 1.3 Export comments from docx
*/

#ifndef BCM_UTILS_H
#define BCM_UTILS_H

#include <arm_utils.h>
#include <stdint.h>
#include <compiler.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

/**
    @name C Standard library Architecture IDs
    @{
    @brief Architecture IDs for C Standard library

*/
#define BRCM_SWARCH_BCM_MEMCPY_PROC     (0x10U)   /**< @brief #BCM_MemCpy    */
#define BRCM_SWARCH_BCM_MEMSET_PROC     (0x11U)   /**< @brief #BCM_MemSet    */
#define BRCM_SWARCH_BCM_MEMMOVE_PROC    (0x12U)   /**< @brief #BCM_MemMove   */
#define BRCM_SWARCH_BCM_STRNCPY_PROC    (0x13U)   /**< @brief #BCM_StrnCpy   */
#define BRCM_SWARCH_BCM_STRTOLONG_PROC  (0x14U)   /**< @brief #BCM_StrToLong */
#define BRCM_SWARCH_BCM_INCMOD_PROC     (0x15U)   /**< @brief #BCM_IncMod    */
#define BRCM_SWARCH_BCM_DECMOD_PROC     (0x16U)   /**< @brief #BCM_DecMod    */
#define BRCM_SWARCH_BCM_GCD_PROC        (0x17U)   /**< @brief #BCM_GCD       */
#define BRCM_SWARCH_BCM_ROUNDUPPOWERTWO_PROC    (0x17U)   /**< @brief #BCM_RoundUpPowerTwo    */
/** @} */

/** @brief Copies aSize number bytes from aSrc to aDest.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination array where the content is to be copied.
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of bytes to copy

    @retval     void

    @post None

    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
static inline void BCM_MemCpy(void *aDest, const void *aSrc, uint32_t aSize)
{
    ((void)memcpy(aDest, aSrc, aSize));
}

/** @brief Fill memory with a constant byte.

    Fills aSize number of elements in memory pointed by aStr with a value aVal.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aStr    Pointer to the block of memory to fill
    @param[in]       aVal    The value to be set
    @param[in]       aSize   Number of bytes to be set to the value

    @retval     void

    @post None

    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
static inline void BCM_MemSet(void *aStr, uint8_t aVal, uint32_t aSize)
{
    ((void)memset(aStr, (int32_t)aVal, aSize));
}

/** @brief Moves given number bytes from src to dest.

    Moves aSize number of elements from memory pointed by aSrc to memory pointed by aDest
    The memory areas can be overlapping.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination memory block where the content is to be copied
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of bytes to copy

    @retval     void

    @post None

    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
static inline void BCM_MemMove(void *aDest, const void *aSrc, uint32_t aSize)
{
    ((void)memmove(aDest, aSrc, aSize));
}

/** @brief Copies a given number (aSize) of characters from aSrc to aDest.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination array where the content    is to be copied.
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of characters to copy

    @retval     void

    @post None

    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
static inline void BCM_StrnCpy(void *aDest, const void *aSrc, uint32_t aSize)
{
    ((void)strncpy(aDest, aSrc, aSize));
}

/** @brief Convert a string to a long integer

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aBuf        Pointer to the string which needs to be converted
    @param[inout]    aEndPtr     Location to store the address of first invalid
                                 character in aBuf (not optional)
    @param[in]       aBase       Integer base of the number given in aBuf. Shall
                                 be a value between 2 and 36 or 0

    Return values are documented in reverse-chronological order
    @retval       long_integer   if aBuf contains a valid integer
    @retval       LONG_MAX       if the integer in aBuf is a large positive    causing overflow
    @retval       LONG_MIN       if the integer in aBuf is a large negative causing underflow

    @post None

    @trace  #BRCM_SWREQ_BCM_STDLIB_WRAPPER
*/
static inline int32_t BCM_StrToLong(char *aBuf, char **aEndPtr, int32_t aBase)
{
    int32_t value;
    value = strtol(aBuf, aEndPtr, aBase);
    if ((**aEndPtr != '\0')) {
        value = 0x7FFFFFFFUL;
    }
    return value;
}

/** @brief Modulo increment

    This API does modulo increment of the value. Range of Modulo incremented value will be
    from 0 to (aMax - 1UL).    If aValue = (aMax - 1UL), incremented value will be 0.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue      Value to be modulo incremented
    @param[in]   aMax        Max value for modulo increment

    @retval     Modulo incremented value

    @post None

    @trace  #BRCM_SWREQ_BCM_UTILS
*/
static inline uint32_t BCM_IncMod(uint32_t aValue, uint32_t aMax)
{
    uint32_t tmpVal = aValue + 1UL;

    if (aMax == tmpVal) {
        tmpVal = 0UL;
    }

    return tmpVal;
}

/** @brief Modulo decrement

    This API does modulo decrement of the value. Range of Modulo decremented value will
    be from 0 to (aMax - 1UL). If aValue = 0, decremented value will be (aMax - 1UL).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue      Value to be modulo decremented
    @param[in]   aMax        Max value for modulo decrement

    @retval     Modulo decremented value

    @post None

    @trace  #BRCM_SWREQ_BCM_UTILS
*/
static inline uint32_t BCM_DecMod(uint32_t aValue, uint32_t aMax)
{
    uint32_t tmpVal;

    if (0UL == aValue) {
        tmpVal = aMax - 1UL;
    } else {
        tmpVal = aValue - 1UL;
    }

    return tmpVal;
}

/** @brief Greatest Common Divisor

    This API gets the Greatest Common Divisor (GCD) of 2 numbers

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue1      First input number
    @param[in]   aValue2      Second input number

    @retval     GCD of the 2 input numbers

    @post None

    @trace  #BRCM_SWREQ_BCM_UTILS
*/
static inline uint32_t BCM_GCD(uint32_t aValue1, uint32_t aValue2)
{
    while(aValue1 != aValue2)
    {
        if(aValue1 > aValue2)
            aValue1 -= aValue2;
        else
            aValue2 -= aValue1;
    }

    return aValue1;
}

/** @brief Round the number up to the nearest power of two

    This API will round the input number up to the nearest
    power of two

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aValue      Value to be rounded off

    @retval     Rounded power of two

    @post None

    @trace  #BRCM_SWREQ_BCM_UTILS
*/
static inline uint32_t BCM_RoundUpPowerTwo(uint32_t aValue)
{
    uint32_t x = aValue;

    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    ++x;

    return x;
}

#endif /* BCM_UTILS_H */
/** @} */

