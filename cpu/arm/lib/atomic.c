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
    @defgroup grp_atomics_impl Implementation
    @ingroup grp_atomics

    @addtogroup grp_atomics_impl
    @{

    @file atomic.c
    @brief Atomic Operations Implementation
    This source file contains the implementation of functions for atomics

    @version 0.1 Initial version
*/

#include <atomic.h>

/**
    @name Atomics Design IDs
    @{
    @brief Design IDs for Atomics
*/
#define BRCM_SWDSGN_ATOMIC_GET_PROC         (0x00U)   /**< @brief #ATOMIC_Get        */
#define BRCM_SWDSGN_ATOMIC_SET_PROC         (0x01U)   /**< @brief #ATOMIC_Set        */
#define BRCM_SWDSGN_ATOMIC_INC_PROC         (0x02U)   /**< @brief #ATOMIC_Inc        */
#define BRCM_SWDSGN_ATOMIC_DEC_PROC         (0x03U)   /**< @brief #ATOMIC_Dec        */
#define BRCM_SWDSGN_ATOMIC_ADD_PROC         (0x04U)   /**< @brief #ATOMIC_Add        */
#define BRCM_SWDSGN_ATOMIC_SUB_PROC         (0x05U)   /**< @brief #ATOMIC_Sub        */
#define BRCM_SWDSGN_ATOMIC_SETBIT_PROC      (0x06U)   /**< @brief #ATOMIC_SetBit     */
#define BRCM_SWDSGN_ATOMIC_SETBITMASK_PROC  (0x07U)   /**< @brief #ATOMIC_SetBitMask */
#define BRCM_SWDSGN_ATOMIC_CLRBIT_PROC      (0x08U)   /**< @brief #ATOMIC_ClrBit     */
#define BRCM_SWDSGN_ATOMIC_CLRBITMASK_PROC  (0x09U)   /**< @brief #ATOMIC_ClrBitMask */
#define BRCM_SWDSGN_ATOMIC_CHECKUPDATE_PROC (0x0AU)   /**< @brief #ATOMIC_CheckUpdate*/
#define BRCM_SWDSGN_ATOMIC_CLRNGETBIT_PROC  (0x0BU)   /**< @brief #ATOMIC_ClrNGetBit */
/** @} */

/** @} */

/**
    @addtogroup grp_atomics
    @{
*/

/**
    @trace #BRCM_SWARCH_ATOMIC_GET_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
uint32_t ATOMIC_Get(volatile uint32_t *const aAddr)
{
    return *aAddr;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_SET_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
uint32_t ATOMIC_Set(volatile uint32_t * const aAddr, const uint32_t aVal)
{
    uint32_t exclusive, oldVal;

    do {
        exclusive = 0UL;
        oldVal = CORTEX_LdrEx(aAddr);
        exclusive = CORTEX_StrEx(aAddr, aVal);
    } while (1UL == exclusive);

    return oldVal;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_INC_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
uint32_t ATOMIC_Inc(volatile uint32_t *const aCnt)
{
    uint32_t exclusive;
    uint32_t val;
    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aCnt);
        val += 1UL;
        exclusive = CORTEX_StrEx(aCnt, val);
    } while (1UL == exclusive);

    return val;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_DEC_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
uint32_t ATOMIC_Dec(volatile uint32_t * const aCnt)
{
    uint32_t exclusive;
    uint32_t val;
    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aCnt);
        val -= 1UL;
        exclusive = CORTEX_StrEx(aCnt, val);
    } while (1UL == exclusive);

    return val;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_ADD_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
void ATOMIC_Add(volatile uint32_t * const aCnt, uint32_t aVal)
{
    uint32_t exclusive;
    uint32_t val;
    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aCnt);
        val += aVal;
        exclusive = CORTEX_StrEx(aCnt, val);
    } while (1UL == exclusive);
}

/**
    @trace #BRCM_SWARCH_ATOMIC_SUB_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
void ATOMIC_Sub(volatile uint32_t * const aCnt, uint32_t aVal)
{
    uint32_t exclusive, val;

    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aCnt);
        val -=  aVal;
        exclusive = CORTEX_StrEx(aCnt, val);
    } while (1UL == exclusive);
}

/**
    @trace #BRCM_SWARCH_ATOMIC_SETBIT_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
void ATOMIC_SetBit(volatile uint32_t * const aAddr, uint32_t aBitNo)
{
    uint32_t exclusive, val;

    if (aBitNo < 32UL) {
        do {
            exclusive = 0UL;
            val = CORTEX_LdrEx(aAddr);
            val |= (1UL << aBitNo);
            exclusive = CORTEX_StrEx(aAddr, val);
        } while (1UL == exclusive);
    }
}

/**
    @trace #BRCM_SWARCH_ATOMIC_CLRBIT_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
void ATOMIC_ClrBit(volatile uint32_t * const aAddr, uint32_t aBitNo)
{
    uint32_t exclusive, val;

    if (aBitNo < 32UL) {
        do {
            exclusive = 0UL;
            val = CORTEX_LdrEx(aAddr);
            val &= (~(1UL << aBitNo));
            exclusive = CORTEX_StrEx(aAddr, val);
        } while (1UL == exclusive);
    }
}

/**
    @trace #BRCM_SWARCH_ATOMIC_SETBITMASK_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
void ATOMIC_SetBitMask(volatile uint32_t * const aAddr, uint32_t aBitMask)
{
    uint32_t exclusive, val;

    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aAddr);
        val |= aBitMask;
        exclusive = CORTEX_StrEx(aAddr, val);
    } while (1UL == exclusive);
}

/**
    @trace #BRCM_SWARCH_ATOMIC_CLRBITMASK_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION
*/
uint32_t ATOMIC_ClrBitMask(volatile uint32_t * const aAddr, uint32_t aBitMask)
{
    uint32_t exclusive, val;
    uint32_t retVal;

    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aAddr);
        retVal = val & aBitMask;
        val &= ~(aBitMask);
        exclusive = CORTEX_StrEx(aAddr, val);
    } while (1UL == exclusive);

    return retVal;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_CHECKUPDATE_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
uint32_t ATOMIC_CheckUpdate(volatile uint32_t *const aAddr, uint32_t aExpected, uint32_t aNew)
{
    uint32_t exclusive, val;
    do {
        exclusive = 0UL;
        val = CORTEX_LdrEx(aAddr);
        if (val == aExpected) {
            exclusive = CORTEX_StrEx(aAddr, aNew);
        } /* else exclusive will be 0UL */
    } while (1UL == exclusive);
    return val;
}

/**
    @trace #BRCM_SWARCH_ATOMIC_CLRNGETBIT_PROC
    @trace #BRCM_SWREQ_ATOMIC_OPERATION

    @todo update pseudocode
    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
uint32_t ATOMIC_ClrNGetBit(volatile uint32_t * const aAddr)
{
    uint32_t idx;
    uint32_t exclusive;
    uint32_t flags;

    do {
        flags = CORTEX_LdrEx(aAddr);
        idx = 0xFFFFFFFFUL;
        if (0UL != flags) {
            idx = CORTEX_BitReverse(flags);
            idx = CORTEX_Clz(idx);
            flags &= ~(1UL << idx);
        }
        exclusive = CORTEX_StrEx(aAddr, flags);
    } while (1UL == exclusive);

    return idx;
}

/** @} */
