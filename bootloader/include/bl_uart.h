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
    @defgroup grp_bl_uart UART
    @ingroup grp_bl_drivers

    @addtogroup grp_bl_uart
    @{
    @section sec_bl_uart_fsm State Machine
    @image html bl_uart_state_machine.jpg "UART Driver State Machine"

    @file bl_uart.h
    @brief Interface for UART driver
    This header file defines interface for UART driver.

    @version 0.30 Imported from docx
*/

#ifndef BL_UART_H
#define BL_UART_H

#include <stdint.h>

/**
    @name UART driver API IDs
    @{
    @brief API IDs for UART drier
 */
#define BRCM_ARCH_BL_UART_HW_ID_TYPE        (0x00U) /**< @brief #BL_UART_HwIDType */
#define BRCM_ARCH_BL_UART_BAUD_RATE_TYPE    (0x01U) /**< @brief #BL_UART_BaudRateType */
#define BRCM_ARCH_BL_UART_STOP_BITS_TYPE    (0x02U) /**< @brief #BL_UART_StopBitsType */
#define BRCM_ARCH_BL_UART_PARITY_TYPE       (0x03U) /**< @brief #BL_UART_ParityType */
#define BRCM_ARCH_BL_UART_STATE_TYPE        (0x04U) /**< @brief #BL_UART_StateType */
#define BRCM_ARCH_BL_UART_CONFIG_TYPE       (0x05U) /**< @brief #BL_UART_ConfigType */
#define BRCM_ARCH_BL_UART_INIT_PROC         (0x06U) /**< @brief #BL_UART_Init */
#define BRCM_ARCH_BL_UART_PUT_CHAR_PROC     (0x07U) /**< @brief #BL_UART_PutChar */
/** @} */

/**
    @brief Index of the UART controller

    @trace #BRCM_ARCH_BL_UART_HW_ID_TYPE #BRCM_REQ_BL_UART_HW_ID
*/
typedef uint32_t BL_UART_HwIDType;

/**
    @name BL_UART_BaudRateType
    @{
    @brief Baud rate of the UART port

    @trace #BRCM_ARCH_BL_UART_BAUD_RATE_TYPE #BRCM_REQ_BL_UART_BAUD_RATE
 */
typedef uint32_t BL_UART_BaudRateType; /**< @brief typedef for UART baud rate */
#define BL_UART_BAUD_9600      (0UL)   /**< @brief Baud rate 9600 bps */
#define BL_UART_BAUD_115200    (1UL)   /**< @brief Baud rate 115200 bps */
/** @} */

/**
    @name BL_UART_StopBitsType
    @{
    @brief Number of stop bits for the UART transaction

    @trace #BRCM_ARCH_BL_UART_STOP_BITS_TYPE #BRCM_REQ_BL_UART_STOP_BITS
 */
typedef uint32_t BL_UART_StopBitsType; /**< @brief typedef for UART stop bits */
#define BL_UART_STOP1  (1UL)           /**< @brief 1 stop bit */
#define BL_UART_STOP2  (2UL)           /**< @brief 2 stop bits */
/** @} */

/**
    @name BL_UART_ParityType
    @{
    @brief Parity for the UART transaction

    @trace #BRCM_ARCH_BL_UART_PARITY_TYPE #BRCM_REQ_BL_UART_PARITY
 */
typedef uint32_t BL_UART_ParityType; /**< @brief typedef for the UART parity */
#define BL_UART_PARITY_NONE    (0UL) /**< @brief No parity */
#define BL_UART_PARITY_EVEN    (1UL) /**< @brief Even parity */
#define BL_UART_PARITY_ODD     (2UL) /**< @brief Odd parity */
/** @} */

/**
    @name BL_UART_StateType
    @{
    @brief State of the UART driver

    @trace #BRCM_ARCH_BL_UART_STATE_TYPE #BRCM_REQ_BL_UART_STATE
 */
typedef uint32_t BL_UART_StateType; /**< @brief typedef for UART states */
#define BL_UART_UNINIT     (0UL)    /**< @brief UART driver is not initialized */
#define BL_UART_INIT       (1UL)    /**< @brief UART driver is initialized */
/** @} */

/**
    @brief UART configuration structure

    @trace #BRCM_ARCH_BL_UART_CONFIG_TYPE #BRCM_REQ_BL_UART_CFG_STRUCTURE
 */
typedef struct {
    BL_UART_BaudRateType  baud;     /**< @brief Baud rate of the UART  */
    BL_UART_StopBitsType  stopBits; /**< @brief Number of stop bits */
    BL_UART_ParityType    parity;   /**< @brief Parity bits */
} BL_UART_ConfigType;

/** @brief Initialize UART

    This API initializes the given UART controller with the given configuration.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aConfig     Pointer to configuration structure

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UART_INIT_PROC  #BRCM_REQ_BL_UART_INIT

    @limitations None
*/
extern void BL_UART_Init(BL_UART_HwIDType aId,
                         const BL_UART_ConfigType *const aConfig);

/** @brief Push the character on UART

    This API pushes the given character out on UART.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aCh         Character to be pushed

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_UART_PUT_CHAR_PROC  #BRCM_REQ_BL_UART_PUT_CHAR

    @limitations None
*/
extern void BL_UART_PutChar(BL_UART_HwIDType aId,
                            char aCh);

#endif /* BL_UART_H */

/** @} */
