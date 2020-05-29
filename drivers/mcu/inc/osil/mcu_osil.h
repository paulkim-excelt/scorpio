/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_mcudrv_il Integration Guide
    @ingroup grp_mcudrv

    @addtogroup grp_mcudrv_il
    @{
    @section sec_mcu_integration Integration Guide

    @section sec_mcu_il_seq Sequence Diagram

    @subsection subsec_mcu_il_seq SVC Layer Interaction
    @image html mcu_seq_il_diagram_svc.jpg "SVC layer interaction"
    Above sequence diagram shows the interaction of MCU driver with system command layer (SVC interface). This is an
    example sequence diagram for #MCU_GetResetReason API.
    Similarly, these are the APIs which raise the system command handler and need integration:
    - #MCU_GetResetRawValue
    - #MCU_ResetReq
    - #MCU_SetLpmMode
    - #MCU_GetVersion
    - #MCU_GetResetMode
    - #MCU_SetResetMode

    @file mcu_osil.h
    @brief MCU Driver Integration Interface
    This header file contains the integration interface for MCU Driver

    @version 0.86 Imported from docx
*/
#ifndef MCU_OSIL_H
#define MCU_OSIL_H

#include <inttypes.h>
#include <mcu.h>
#include <bcm_err.h>
#if defined(__BCM8910X__)
#include <bcm8910x/mcu_osil.h>
#elif defined (__BCM8953X__)
#include <bcm8953x/mcu_osil.h>
#elif defined (__BCM8956X__)
#include <bcm8956x/mcu_osil.h>
#elif defined (__BCM8908X__)
#include <bcm8908x/mcu_osil.h>
#else
#error "unsupported chip family"
#endif

/**
    @name MCU Integration Interface IDs
    @{
    @brief Integration Interface IDs for MCU Driver
*/
#define BRCM_SWARCH_MCU_SYSCMDREQ_PROC         (0x42U) /**< @brief #MCU_SysCmdReq */
#define BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC     (0x43U) /**< @brief #MCU_SysCmdHandler */
/** @} */

/** @brief Interface to raise a request to MCU driver

    This interface shall post a MCU command to system. This interface is called by MCU driver APIs to post a command to
    system.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success (command was successfully processed and results are updated by MCU
                                        driver in the IO structure)
    @retval     Others                  Error value specific to the requested command
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_MCU_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t MCU_SysCmdReq(MCU_CmdType aCmd,
                                 MCU_IOType *aIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    MCU SVC Magic ID
    @param[in]      aCmd        Command requested by user
    @param[inout]   aSysIO      uint8_t * to be interpreted as #MCU_IOType

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_MCU_KERNEL_HANDLER

    @limitations None
*/
extern void MCU_SysCmdHandler(uint32_t aMagicID,
                                  uint32_t aCmd,
                                  uint8_t * aSysIO);


#endif /* MCU_OSIL_H */

/** @} */
