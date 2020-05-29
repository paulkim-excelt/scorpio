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
    @defgroup grp_cortex_mx_interrupt Implementation
    @ingroup grp_cortex_interrupt

    @addtogroup grp_cortex_mx_interrupt
    @{

    @file cpu/arm/cortex_mx/interrupt.c
    @brief Cortex-M CPU NVIC and Interrupt Control Implementation

    @version 0.1 Initial version
*/

#include <compiler.h>
#include <interrupt.h>

/**
    @name ARM Cotex Mx Interrupt Control
    @{
    @brief Design IDs for ARM Cortex-M Interrupt control
*/
#define BRCM_SWDSGN_INTR_SUSPEND_PROC  (0xB0U) /**< @brief #INTR_Suspend          */
#define BRCM_SWDSGN_INTR_RESUME_PROC   (0xB1U) /**< @brief #INTR_Resume           */
/** @} */


/** @} */

/**
    @addtogroup grp_cortex_interrupt
    @{
*/

/**
    @trace #BRCM_SWARCH_INTR_SUSPEND_PROC
    @trace #BRCM_SWREQ_INTR_CORTEX_MX
*/
INTR_FlagType INTR_Suspend(void)
{
    INTR_FlagType flg;
#if defined(GNU) || defined(ARMCC)
    COMP_ASM("mrs %0, primask" : "=r" (flg));
    COMP_ASM("cpsid i");
#else   /* !ARMCC */
#error Unsupported compiler
#endif /* ARMCC */

    return flg;
}

/**
    @trace #BRCM_SWARCH_INTR_RESUME_PROC
    @trace #BRCM_SWREQ_INTR_CORTEX_MX
*/
void INTR_Resume(INTR_FlagType flag)
{
#if defined(GNU) || defined(ARMCC)
    COMP_ASM("msr primask, %0" :: "r" (flag));
#else   /* !ARMCC */
#error Unsupported compiler
#endif /* ARMCC */
}


/** @} */
