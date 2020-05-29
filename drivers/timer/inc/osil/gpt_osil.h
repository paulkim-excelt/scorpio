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

/** @defgroup grp_timdrv_il Integration Guide
    @ingroup grp_timdrv

    @addtogroup grp_timdrv_il
    @{
    @section sec_gpt_integration Integration Guide

    @section sec_gpt_il_seq Sequence Diagram
    @image html gpt_seq_il_diagram_svc.jpg "Timer Driver SVC layer interaction"

    @file gpt_osil.h
    @brief GPT driver Integration Interfaces
    This header file contains the integration interface for GPT driver
    @version 0.30 Imported from docx
*/

#ifndef GPT_OSIL_H
#define GPT_OSIL_H

#include <system.h>
#include <gpt.h>

/**
    @name GPT driver interface IDs
    @{
    @brief Interface IDs for GPT driver
*/
#define BRCM_SWARCH_TIM_CMD_TYPE               (0x40U) /**< @brief #TIM_CmdType */
#define BRCM_SWARCH_TIM_IO_TYPE                (0x41U) /**< @brief #TIM_IOType */
#define BRCM_SWARCH_TIM_SYSCMDHANDLER_PROC     (0x43U) /**< @brief #TIM_SysCmdHandler */
#define BRCM_SWARCH_TIM_SYSCMDREQ_PROC         (0x44U) /**< @brief #TIM_SysCmdReq */
#define BRCM_SWARCH_TIM_IRQHANDLER_PROC        (0x45U) /**< @brief #TIM_IRQHandler */
/** @} */

/**
    @name TIM_CmdType
    @{
    @brief Timer command type

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef uint32_t TIM_CmdType;                   /**< @brief Timer command type */
#define TIM_CMD_INIT                    (0x0UL) /**< @brief Timer command for initialization */
#define TIM_CMD_DEINIT                  (0x1UL) /**< @brief Timer command for de-initialization */
#define TIM_CMD_GET_TIME_ELAPSED        (0x2UL) /**< @brief Timer command to get the time elapsed */
#define TIM_CMD_GET_TIME_REMAINING      (0x3UL) /**< @brief Timer command to get the time remaining */
#define TIM_CMD_START_TIMER             (0x4UL) /**< @brief Timer command to start the timer */
#define TIM_CMD_STOP_TIMER              (0x5UL) /**< @brief Timer command to stop the timer */
#define TIM_CMD_ENABLE_NOTIFICATION     (0x6UL) /**< @brief Timer command to enable notification */
#define TIM_CMD_DISABLE_NOTIFICATION    (0x7UL) /**< @brief Timer command to disable notification */
#define TIM_CMD_GET_FREE_RUNNING_TIME   (0x8UL) /**< @brief Timer command to get current time of free running timer */
#define TIM_CMD_GET_TIME                (0x9UL) /**< @brief Timer command to get current time */
/** @} */

/**
    @brief GTP IO structure

    Structure for Timer SVC interface

    @trace #BRCM_SWREQ_TIM_KERNEL_INTERFACE
*/
typedef struct _TIM_IOType {
    int32_t retVal;                 /**< @brief SVC response */
    TIM_ChanIDType chanID;          /**< @brief Timer channel ID */
    TIM_TickType elapsedTime;       /**< @brief Time elapsed */
    TIM_TickType remainingTime;     /**< @brief Time remaining */
    TIM_TickType targetTime;        /**< @brief Target time */
    uint64_t sysTime;               /**< @brief Time(in ms) of the timer
                                         configured for system time usage */
    uint64_t sysTimeRaw;            /**< @brief Time(in ns) of the timer */
    const TIM_ConfigType *config;   /**< @brief Timer configuration data */
} TIM_IOType;

/** @brief Timer system command request

    Interface for Timer system command request.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Timer command
    @param[inout]   aIO         Pointer to Timer IO

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Timer command operation success
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid magic number
    @retval     Others                  Error values specific to the command
                                        raised
    @retval     #BCM_ERR_INVAL_PARAMS   aIO is NULL

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_TIM_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t TIM_SysCmdReq(TIM_CmdType aCmd,
                             TIM_IOType *const aIO);

/** @brief Timer system command handler

    This interface handles all the Timer driver commands requested from
    #SysCmdReq.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMagicID    Magic ID for Timer
    @param[in]      aCmd        System command request
    @param[inout]   aSysIO      Pointer to system request IO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall hook
    it to SVC layer.

    @trace  #BRCM_SWREQ_TIM_KERNEL_HANDLER

    @limitations None
*/
extern void TIM_SysCmdHandler(uint32_t aMagicID,
                              uint32_t aCmd,
                              uint8_t *aSysIO);

/** @brief Handle timer interrupts

    This interface handles Timer interrupts.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChanID     Timer Channel ID

    @return     void

    @post TBD

    @note This interface shall be called from ISR handler for GPT driver.

    @limitations None

    @trace  #BRCM_SWREQ_TIM_KERNEL_HANDLER

    @snippet gpt_osil.c Usage of TIM_IRQHandler
*/
extern void TIM_IRQHandler(TIM_ChanIDType aChanID);

#endif
/** @} */
