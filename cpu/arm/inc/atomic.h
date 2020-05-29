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
    @defgroup grp_atomics Atomics
    @ingroup grp_cortex

    @addtogroup grp_atomics
    @{
    @section sec_atomics_overview Overview
    Atomic APIs shall be used where thread critical data is being updated and
    needs exclusive access. Atomic APIs are build on top of ARM special
    instructions like LDREX/STREX/CLREX which provide exclusive access to a
    memory by using local/global monitor.

    @limitations  It is user responsibility to ensure that addresses passed to
    these APIs are normal cached memory (not device memory).

    @file atomic.h
    @brief Atomic APIs

    @version 1.0 Initial version
*/

#ifndef ATOMIC_H
#define ATOMIC_H

#include <cortex.h>

/**
    @name Atomics Architecture IDs
    @{
    @brief Architecture IDs for Atomics
*/
#define BRCM_SWARCH_ATOMIC_GET_PROC         (0x00U)   /**< @brief #ATOMIC_Get        */
#define BRCM_SWARCH_ATOMIC_SET_PROC         (0x01U)   /**< @brief #ATOMIC_Set        */
#define BRCM_SWARCH_ATOMIC_INC_PROC         (0x02U)   /**< @brief #ATOMIC_Inc        */
#define BRCM_SWARCH_ATOMIC_DEC_PROC         (0x03U)   /**< @brief #ATOMIC_Dec        */
#define BRCM_SWARCH_ATOMIC_ADD_PROC         (0x04U)   /**< @brief #ATOMIC_Add        */
#define BRCM_SWARCH_ATOMIC_SUB_PROC         (0x05U)   /**< @brief #ATOMIC_Sub        */
#define BRCM_SWARCH_ATOMIC_SETBIT_PROC      (0x06U)   /**< @brief #ATOMIC_SetBit     */
#define BRCM_SWARCH_ATOMIC_SETBITMASK_PROC  (0x07U)   /**< @brief #ATOMIC_SetBitMask */
#define BRCM_SWARCH_ATOMIC_CLRBIT_PROC      (0x08U)   /**< @brief #ATOMIC_ClrBit     */
#define BRCM_SWARCH_ATOMIC_CLRBITMASK_PROC  (0x09U)   /**< @brief #ATOMIC_ClrBitMask */
#define BRCM_SWARCH_ATOMIC_CHECKUPDATE_PROC (0x0AU)   /**< @brief #ATOMIC_CheckUpdate*/
#define BRCM_SWARCH_ATOMIC_CLRNGETBIT_PROC  (0x0BU)   /**< @brief #ATOMIC_ClrNGetBit */
/** @} */

/** @brief Exclusive load for Atomic operation

    This API performs an exclusive load (ARM ldrex instruction).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address of the volatile variable

    @retval uint32_t  Value of the volatile variable at aAddr

    @post aAddr is marked for exclusive access

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern uint32_t ATOMIC_Get(volatile uint32_t * const aAddr);

/** @brief Sets a value at the address atomically

    This API Sets the value at the given address atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[out]     aAddr       Address of the volatile variable
    @param[in]      aVal        Value to be stored in the volatile variable

    @return Old value at the given address

    @post aAddr is cleared for exclusive access

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern uint32_t ATOMIC_Set(volatile uint32_t * const aAddr, const uint32_t aVal);

/** @brief Increment counter atomically and return it

    This API increments a counter atomically

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aCnt        Address of the counter variable

    @retval uint32_t Value of the variable after increment

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aCnt shall be in normal cached memory
*/
extern uint32_t ATOMIC_Inc(volatile uint32_t * const aCnt);

/** @brief Decrement counter atomically and return it

    This API decrements a counter atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aCnt        Address of the counter variable

    @retval uint32_t Value of the variable after decrement

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aCnt shall be in normal cached memory
*/
extern uint32_t ATOMIC_Dec(volatile uint32_t * const aCnt);

/** @brief Add a value to counter atomically

    This API adds a value to a counter atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aCnt    Address of the counter variable
    @param[in]      aVal    Value to be added to counter

    @return void

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aCnt shall be in normal cached memory
*/
extern void ATOMIC_Add(volatile uint32_t * const aCnt, uint32_t aVal);

/** @brief Substract a value from a counter atomically

    This API substracts a value from a counter atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aCnt        Address of the counter variable
    @param[in]      aVal        Value to be substracted from counter

    @return void

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aCnt shall be in normal cached memory
*/
extern void ATOMIC_Sub(volatile uint32_t * const aCnt, uint32_t aVal);

/** @brief Set a bit atomically

    This API sets a bit at the given address atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aAddr       Address of the memory
    @param[in]      aBitNo      Bit number

    @return void

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern void ATOMIC_SetBit(volatile uint32_t * const aAddr, uint32_t aBitNo);

/** @brief Clear a bit atomically

    This API clears a bit at the given address atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aAddr       Address of the memory
    @param[in]      aBitNo      Bit number

    @return void

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern void ATOMIC_ClrBit(volatile uint32_t * const aAddr, uint32_t aBitNo);

/** @brief Set bitmask atomically

    This API sets bitmask at the given address atomically.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[inout]   aAddr       Address of the memory
    @param[in]      aBitMask    Bit mask

    @return void

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern void ATOMIC_SetBitMask(volatile uint32_t * const aAddr, uint32_t aBitMask);

/** @brief Clear bitmask atomically

    This API clears bitmask at the given bitmask atomically.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aAddr       Address of the memory
    @param[in]      aBitMask    Bit mask

    @retval Masked previous value

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern uint32_t ATOMIC_ClrBitMask(volatile uint32_t * const aAddr, uint32_t aBitMask);

/** @brief Updates the value at the address atomically

    This API updates the value atomically at the given address with the new value
    only if the existing value matches with the exptected value.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aAddr       Address of the memory
    @param[in]      aExpected   Value expected at the address before attempting to modify it
    @param[in]      aNew        Value to be set

    @retval Old value at the given address

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in normal cached memory
*/
extern uint32_t ATOMIC_CheckUpdate(volatile uint32_t *const aAddr, uint32_t aExpected,
                             uint32_t aNew);

/** @brief Gets the index of a set bit after clearing.

    This API gets the index of a bit which was set and clears it before returning

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]       aAddr       Address of the memory

    @retval index of the set bit. If no bit is set, it returns 0xFFFFFFFFUL

    @post None

    @trace  #BRCM_SWREQ_ATOMIC_OPERATION

    @limitations aAddr shall be in noramal cached memory
*/
extern uint32_t ATOMIC_ClrNGetBit(volatile uint32_t * const aAddr);
#endif /* ATOMIC_H */
/** @} */
