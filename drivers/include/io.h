/*****************************************************************************
 Copyright 2016 Broadcom Limited.  All rights reserved.

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
 * @file io.h
 *
 * @brief This file provides register read/write macros.
 */

#ifndef IO_H
#define IO_H

#include <stdint.h>

/** @brief Read 32 bits
 *
 * @param[in]   addr    The address from which the value has to be read
 *
 * @return      The value read
 */
#define readl(addr)         \
    (*((const volatile uint32_t *) (addr)))

/** @brief Write 32 bits
 *
 * @param[in]   addr    The address at which the value has to be written
 * @param[in]   val     The value to be written
 */
#define writel(addr, val)   \
    (*((volatile uint32_t *) (addr)) = (uint32_t)(val))

/** @brief Read 16 bits
 *
 * @param[in]   addr    The address from which the value has to be read
 *
 * @return      The value read
 */
#define readw(addr)         \
    (*((const volatile uint16_t *) (addr)))

/** @brief Write 16 bits
 *
 * @param[in]   addr    The address at which the value has to be written
 * @param[in]   val     The value to be written
 */
#define writew(addr, val)   \
    (*((volatile uint16_t *) (addr)) = (uint16_t)(val))

/** @brief Read 8 bits
 *
 * @param[in]   addr    The address from which the value has to be read
 *
 * @return      The value read
 */
#define readb(addr)         \
    (*((const volatile uint8_t *) (addr)))

/** @brief 8 bits write
 *
 * @param[in]   addr    The address at which the value has to be written
 * @param[in]   val     The value to be written
 */
#define writeb(addr, val)   \
    (*((volatile uint8_t *) (addr)) = (uint8_t)(val))

/** @brief Read from 32 bits register
 *
 * @param[in]   addr    Register address
 *
 * @return      The value read
 */
#define reg_read32(addr)        readl(addr)

/** @brief Write to 32 bits register
 *
 * @param[in]   addr    Register address
 * @param[in]   val     The value to be written
 */
#define reg_write32(addr, val)  writel(addr, val)

/** @brief Read from 16 bits register
 *
 * @param[in]   addr    Register address
 *
 * @return      The value read
 */
#define reg_read16(addr)        readw(addr)

/** @brief Write to 16 bits register
 *
 * @param[in]   addr    Register address
 * @param[in]   val     The value to be written
 */
#define reg_write16(addr, val)  writew(addr, val)
/** @brief Read from 8 bits register
 *
 * @param[in]   addr    Register address
 *
 * @return      The value read
 */
#define reg_read8(addr)         readb(addr)

/** @brief Write to 8 bits register
 *
 * @param[in]   addr    Register address
 * @param[in]   val     The value to be written
 */
#define reg_write8(addr, val)   writeb(addr, val)

#endif /* IO_H */
