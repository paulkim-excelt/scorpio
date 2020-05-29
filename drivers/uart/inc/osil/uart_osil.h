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

/** @defgroup grp_uartdrv_il Integration Guide
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_il
    @{
    @section sec_uart_integration Integration Guide

    @section sec_uart_il_seq Sequence Diagram
    @subsection subsec_uart_il_seq UART_IRQHandler
    @image html uart_seq_il_diagram.jpg "UART_IRQHandler sequence diagram"

    @file uart_osil.h
    @brief UART OSIL interface

    @version 1.11 Imported from docx
*/

#ifndef UART_OSIL_H
#define UART_OSIL_H

#include <uart.h>
#include <svc.h>
#include <bcm_err.h>

/**
    @name UART Integration Interface IDs
    @{
    @brief Integration Interface IDs for UART
*/
#define BRCM_SWARCH_UART_CONFIG_GLOBAL          (0x40U) /**< @brief #UART_Config       */
#define BRCM_SWARCH_UART_CMD_TYPE               (0x41U) /**< @brief #UART_CmdType      */
#define BRCM_SWARCH_UART_IO_TYPE                (0x42U) /**< @brief #UART_IOType       */
#define BRCM_SWARCH_UART_SYS_CMD_REQUEST_PROC   (0x42U) /**< @brief #UART_SysCmdReq    */
#define BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC   (0x44U) /**< @brief #UART_SysCmdHandler*/
#define BRCM_SWARCH_UART_IRQ_HANDLER_PROC       (0x45U) /**< @brief #UART_IRQHandler   */
#define BRCM_SWARCH_UART_IRQ_HANDLER0_PROC      (0x46U) /**< @brief #UART_IRQHandler0  */
#define BRCM_SWARCH_UART_IRQ_HANDLER1_PROC      (0x47U) /**< @brief #UART_IRQHandler1  */
#define BRCM_SWARCH_UART_IRQ_HANDLER2_PROC      (0x48U) /**< @brief #UART_IRQHandler2  */
/** @} */

/** @brief UART Configuration structure array (one per instance)

    @trace #BRCM_SWREQ_UART_INIT
*/
extern const UART_ConfigType UART_Config[];
/**
    @name UART_CmdType
    @{
    @brief UART driver command type

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
*/
typedef uint32_t UART_CmdType;                     /**< @brief UART driver command type */
#define UART_CMD_INIT                   (0x1UL)    /**< @brief UART init command */
#define UART_CMD_SEND                   (0x2UL)    /**< @brief UART send command */
#define UART_CMD_SEND_BREAK             (0x3UL)    /**< @brief UART Send Break command */
#define UART_CMD_CLR_BREAK              (0x4UL)    /**< @brief UART Clear Break command */
#define UART_CMD_TX_STATUS              (0x5UL)    /**< @brief UART Tx Status command */
#define UART_CMD_GET_STATE              (0x6UL)    /**< @brief UART Get State command */
#define UART_CMD_GET_STATS              (0x7UL)    /**< @brief UART Get Stats command */
#define UART_CMD_DEINIT                 (0x8UL)    /**< @brief UART deinit command */
/** @} */

/**
    @brief UART IO structure

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
*/
typedef struct _UART_IOType {
    int32_t retVal;     /**< @brief Return value as set by #UART_SysCmdHandler */
    UART_HwIDType id;   /**< @brief UART ID */
    const uint8_t* data;/**< @brief UART data pointer to send */
    uint32_t size;      /**< @brief UART size to be sent  */
    UART_TxStatusType status;/**< @brief TX Status      */
    UART_StateType  state;   /**< @brief Driver State   */
    UART_StatsType stats;    /**< @brief Statistics     */
} UART_IOType;

/** @brief UART driver system command request

    Interface to raise System Command Request for UART driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to In-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by #UART_SysCmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic ID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t UART_SysCmdReq(UART_CmdType aCmd, UART_IOType *const aIO);

/** @brief Handle UART driver commands

    This interface handles all the UART driver commands requested from system
    command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    UART driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #UARTIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_UART_KERNEL_HANDLER

    @limitations None
*/
extern void UART_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief Handle UART Interrupts

    This interface shall be called when UART interrupt is asserted. The
    integration layer shall also pass the index of the controller which has
    asserted the interrupt

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    @return void

    @post None

    @note To be used by integrator in the actual interrupt handler.

    @trace  #BRCM_SWREQ_UART_KERNEL_HANDLER

    @limitations None

    @snippet uart_osil.c Usage of UART_IRQHandler
*/
extern void UART_IRQHandler(UART_HwIDType aId);


/** @brief Handle UART Interrupts for Instance 0

    This interface shall be installed for UART instance 0 interrupt handling.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @note To be developed by integrator and installed in NVIC.

    @trace  #BRCM_SWREQ_UART_KERNEL_HANDLER

    @limitations None
*/
extern void UART_IRQHandler0(void);

/** @brief Handle UART Interrupts for Instance 1

    This interface shall be installed for UART instance 1 interrupt handling.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @note To be developed by integrator and installed in NVIC.

    @trace  #BRCM_SWREQ_UART_KERNEL_HANDLER

    @limitations None
*/
extern void UART_IRQHandler1(void);

/** @brief Handle UART Interrupts for Instance 2

    This interface shall be installed for UART instance 2 interrupt handling.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @note To be developed by integrator and installed in NVIC.

    @trace  #BRCM_SWREQ_UART_KERNEL_HANDLER

    @limitations None
*/
extern void UART_IRQHandler2(void);

#endif /* UART_OSIL_H */

/** @} */
