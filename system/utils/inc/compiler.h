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
    @defgroup grp_compiler Compiler Abstractions
    @ingroup grp_utils

    @addtogroup grp_compiler
    @{
    @section sec_compiler_overview Overview
    This file specfies compiler abstractions used by diffrent software
    components in the system.

    @limitations The behaviour of these abstractions are
    compiler dependent

    @file compiler.h
    @brief Compiler abstractions

    This file specfies compiler abstractions used by diffrent software
    components in the system. The behaviour of these abstractions are
    compiler dependent.

    @version 0.1 Initial Version
*/

#ifndef COMPILER_H
#define COMPILER_H

#if defined(GNU)
#include "compiler_gcc.h"
#elif defined(ARMCC)   /* !GNU */
#include "compiler_armcc.h"
#else   /* !ARMCC */
#error Unsupported compiler
#endif /* ARMCC */


/**
    @name Compiler Abstraction Architecture IDs
    @{
    @brief Architecture IDs for Compiler Abstraction
*/
#define BRCM_SWARCH_COMP_SECTION_MACRO    (0x00U)   /**< @brief #COMP_SECTION */
#define BRCM_SWARCH_COMP_ALIGN_MACRO      (0x01U)   /**< @brief #COMP_ALIGN   */
#define BRCM_SWARCH_COMP_ASM_MACRO        (0x02U)   /**< @brief #COMP_ASM     */
/** @} */

/**
    @brief Macro to place content in specific section

    Macro to specify that a data shall be placed in a section (s)

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU) || defined(ARMCC)
#define COMP_SECTION(s)     __attribute__((section(s)))
#else   /* defined(GNU) || defined(ARMCC) */
#error Unsupported Compiler
#endif  /* defined(GNU) || defined(ARMCC) */

/**
    @brief Alignment

    Alignment using compiler directive

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU) || defined(ARMCC)
#define COMP_ALIGN(a)       __attribute__((aligned(a)))
#else   /* defined(GNU) || defined(ARMCC) */
#error Unsupported Compiler
#endif  /* defined(GNU) || defined(ARMCC) */

/**
    @brief Assembly

    Inline assembly

    @trace #BRCM_SWREQ_COMP_ABSTRACTION
*/
#if defined(GNU)
#define COMP_ASM  __asm volatile
#elif defined(ARMCC) /* defined(GNU) */
#define COMP_ASM  __asm volatile
#else   /* defined(ARMCC) */
#error Unsupported Compiler
#endif  /* defined(GNU) || defined(ARMCC) */

#endif /* COMPILER_H */
/** @} */
