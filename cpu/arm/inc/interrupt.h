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
    @defgroup grp_cortex_interrupt Interrupts
    @ingroup grp_cortex

    @addtogroup grp_cortex_interrupt
    @{
    @section sec_cortex_interrupt_overview Overview
    APIs to Disable/Enable and Suspend/Resume interrupts

    @file cpu/arm/include/interrupt.h
    @brief Interrupt related APIs

    @version 1.0 Import from document
*/

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <inttypes.h>

/**
    @name ARM Cotex Interrupt Control API
    @{
    @brief Architecture IDs for ARM Cortex Interrupts
*/
#define BRCM_SWARCH_INTR_FLAG_TYPE       (0x30U)   /**< @brief #INTR_FlagType    */
#define BRCM_SWARCH_INTR_SUSPEND_PROC    (0x31U)   /**< @brief #INTR_Suspend     */
#define BRCM_SWARCH_INTR_RESUME_PROC     (0x32U)   /**< @brief #INTR_Resume      */
/** @} */

/**
    @name Interrupt Flag Type
    @{
    @brief interrupt flag type

    This type is used during IRQ suspend/resume calls.

    @trace #BRCM_SWREQ_INTR_CORTEX
*/
typedef uint32_t INTR_FlagType;
/** @}*/

/** @brief Suspend system interrupts

    This API disables the system interrupt and returns the current state of
    interrupt.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return INTR_FlagType (Status of the interrupt)

    @post Interrupts are suspended

    @trace #BRCM_SWREQ_INTR_CORTEX

    @limitations None
*/
INTR_FlagType INTR_Suspend(void);

/** @brief Resume system interrupt

    This API restores the system interrupt saved by INTR_Suspend() API.

    @behavior Sync, Non-reentrant

    @pre Interrupts are suspended

    @param[in]      flag        Interrupt status flag (as obtained from the previous
                                INTR_Suspend() API.

    @return void

    @post Interrupts are restored to flag

    @trace #BRCM_SWREQ_INTR_CORTEX

    @limitations None
*/
void INTR_Resume(INTR_FlagType flag);

#endif /* INTERRUPT_H */
/** @} */
