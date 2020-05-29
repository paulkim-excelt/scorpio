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
    @defgroup grp_bl_utils_misc Miscellaneous
    @ingroup grp_bl_utils

    @addtogroup grp_bl_utils_misc
    @{
    @section sec_bl_utils_misc_overview Overview
    This file specifies common utility functions (miscellaneous) APIs

    @file bl_utils.h
    @brief Common utility functions

    @version 0.30 Imported from docx
*/

#ifndef BL_UTILS_H
#define BL_UTILS_H

#include <stdint.h>
#include <bl_compiler.h>
#include <bl_arm_utils.h>

/**
    @name Utility API IDs
    @{
    @brief API IDs for BCM Utility
*/
#define BRCM_ARCH_BL_UTILS_MISC_MIN_MACRO           (0x00U) /**< @brief #BL_MIN */
#define BRCM_ARCH_BL_UTILS_MISC_MAX_MACRO           (0x01U) /**< @brief #BL_MAX */
#define BRCM_ARCH_BL_UTILS_MISC_TRUE_FALSE_MACRO    (0x02U) /**< @brief #BL_FALSE */
#define BRCM_ARCH_BL_UTILS_MISC_MEM_CPY_PROC        (0x03U) /**< @brief #BL_BCM_MemCpy */
#define BRCM_ARCH_BL_UTILS_MISC_MEM_SET_PROC        (0x04U) /**< @brief #BL_BCM_MemSet */
#define BRCM_ARCH_BL_UTILS_MISC_MEM_MOVE_PROC       (0x05U) /**< @brief #BL_BCM_MemMove */
#define BRCM_ARCH_BL_UTILS_MISC_MEM_CMP_PROC        (0x06U) /**< @brief #BL_BCM_MemCmp */
#define BRCM_ARCH_BL_UTILS_MISC_STRN_CPY_PROC       (0x07U) /**< @brief #BL_BCM_StrnCpy */
#define BRCM_ARCH_BL_UTILS_MISC_STR_TO_LONG_PROC    (0x08U) /**< @brief #BL_BCM_StrToLong */
/** @} */

/**
    @brief Find minimum of two values @a a and @a b

    @trace #BRCM_ARCH_BL_UTILS_MISC_MIN_MACRO #BRCM_REQ_BL_UTILS_MISC_MIN
*/
#define BL_MIN(a, b)   (((a) > (b)) ? (b) : (a))

/**
    @brief Find maximum of two values @a a and @a b

    @trace #BRCM_ARCH_BL_UTILS_MISC_MAX_MACRO #BRCM_REQ_BL_UTILS_MISC_MAX
*/
#define BL_MAX(a, b)   (((a) > (b)) ? (a) : (b))

/**
    @name TRUE/FALSE IDs
    @{
    @brief Define false and true

    @trace #BRCM_ARCH_BL_UTILS_MISC_TRUE_FALSE_MACRO #BRCM_REQ_BL_UTILS_MISC_TRUE_FALSE
*/
#define BL_FALSE   (0UL) /**< @brief FALSE Macro */
#define BL_TRUE    (1UL) /**< @brief TRUE Macro */
/** @} */

/** @brief Copies aSize number bytes from aSrc to aDest.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination array where the content is to be copied.
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of bytes to copy

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_MEM_CPY_PROC  #BRCM_REQ_BL_UTILS_MISC_MEM_CPY
*/
extern void BL_BCM_MemCpy(void *aDest,
                          const void *aSrc,
                          uint32_t aSize);

/** @brief Fill memory with a constant byte.

    Fills aSize number of elements in memory pointed by aStr with a value aVal.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aStr    Pointer to the block of memory to fill
    @param[in]       aVal    The value to be set
    @param[in]       aSize   Number of bytes to be set to the value

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_MEM_SET_PROC  #BRCM_REQ_BL_UTILS_MISC_MEM_SET
*/
extern void BL_BCM_MemSet(void *aStr,
                          uint8_t aVal,
                          uint32_t aSize);

/** @brief Moves given number bytes from src to dest.

    Moves aSize number of elements from memory pointed by aSrc to memory pointed by aDest
    The memory areas can be overlapping.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination memory block where the content is to be copied
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of bytes to copy

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_MEM_MOVE_PROC  #BRCM_REQ_BL_UTILS_MISC_MEM_MOVE
*/
extern void BL_BCM_MemMove(void *aDest,
                           const void *aSrc,
                           uint32_t aSize);

/** @brief Compare given number bytes from given buffer to reference buffer

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aBuf       Pointer to the buffer to be compared
    @param[in]       aBufRef    Pointer to the reference buffer
    @param[in]       aSize      Number of bytes to be compared

    Return values are documented in reverse-chronological order
    @retval         0L          Given number of bytes are equal
    @retval         <0L         At a given location, byte value in aBuf is less than aBufRef
    @retval         >0L         At a given location, byte value in aBuf is greater than aBufRef

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_MEM_CMP_PROC  #BRCM_REQ_BL_UTILS_MISC_MEM_CMP
*/
extern int32_t BL_BCM_MemCmp(const void *aBuf,
                             const void *aBufRef,
                             uint32_t aSize);

/** @brief Copies a given number (aSize) of characters from aSrc to aDest.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]    aDest   Pointer to the destination array where the content is to be copied.
    @param[in]       aSrc    Pointer to the source of data to be copied
    @param[in]       aSize   Number of characters to copy

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_STRN_CPY_PROC  #BRCM_REQ_BL_UTILS_MISC_STRN_CPY
*/

extern void BL_BCM_StrnCpy(void *aDest,
                           const void *aSrc,
                           uint32_t aSize);

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
    @retval       LONG_MAX       if the integer in aBuf is a large positive causing overflow
    @retval       LONG_MIN       if the integer in aBuf is a large negative causing underflow

    @post None

    @trace  #BRCM_ARCH_BL_UTILS_MISC_STR_TO_LONG_PROC  #BRCM_REQ_BL_UTILS_MISC_STR_TO_LONG
*/
int32_t BL_BCM_StrToLong(char *aBuf,
                         char **aEndPtr,
                         int32_t aBase);

#endif /* BL_UTILS_H */
/** @} */