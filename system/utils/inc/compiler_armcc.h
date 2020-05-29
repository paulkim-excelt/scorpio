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
 * @file compiler_armcc.h
 *
 * @brief Compiler abstractions for armcc compiler
 */

#ifndef COMPILER_ARMCC_H
#define COMPILER_ARMCC_H

/**
 * @brief Static inline functions
 *
 * Compiler does not ensure that it will
 * be always inlined!!
 */

#define COMP_INLINE             static __inline

/**
 * @brief Always inline
 *
 * Use this macro before function defintion to
 * force this function as inline
 */
#define COMP_ALWAYS_INLINE      static __attribute__((always_inline))

/**
 * @brief Never inline
 *
 * Use this macro before a function definition to
 * ensure that function is never inlined
 */
#define COMP_NEVER_INLINE       __attribute__((noinline))

/**
 * @brief Error attributes
 *
 * Can be used to report an error during compilation.
 */
#define COMP_ERROR(aMsg)     __attribute__((error(aMsg)))

/**
 * @brief Warning attributes
 *
 * Can be used on a function declaration to warn user
 * that this function is not emited during link time
 * and is being used.
 */
#define COMP_WARNING(aMsg)   __attribute__((warning(aMsg)))

/**
 * @brief Interrupt function
 *
 * Macro to specify that a data shall be placed in a section (s)
 */
#define COMP_IRQ(irq)       __attribute__((interrupt(irq)))

/**
 * @brief Packing of structures/variables
 */
#define COMP_PACKED         __attribute__((packed))

/**
 * @brief Packing of structures/variables
 */
#define COMP_PACK(x)        x __PACKED__

#endif /* COMPILER_ARMCC_H */
