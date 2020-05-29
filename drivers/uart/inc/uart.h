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
    @defgroup grp_uartdrv_ifc UART
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_ifc
    @{
    @section sec_uart_overview Overview
    @note
    - Configuration of pins for UART functionality shall be done using PinMux
    driver before UART driver initialization
    - UART controller is reset during system initialization to ensure defined
    state of controller
    - UART APIs are called from privileged software
    - System linker shall map a section “.data.drivers”

    @image html uart_overview.jpg "Driver overview diagram"
    UART driver interface layer is a very thin layer which abstracts the
    underlying UART IP to the clients.

    UART driver can be used by logging framework, shell and/or by any other
    client who wants to communicate over UART.

    @section sec_uart_fsm State Machine
    @image html uart_state_machine.jpg "Driver state machine "
    - #UART_UNINIT: This is default state when UART is not initialized or when
    #UART_DeInit is completed successfully.
    - #UART_INIT: This state is entered when #UART_Init is called and controller
    is initialized successfully

    @section sec_uart_seq Sequence Diagrams
    The below diagram depicts the use case for transmitting and receiving UART
    data
    @image html uart_seq_diagram.jpg "Use case diagram for transmitting and receiving UART data"

    @section sec_comp_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_uart.dep

    @limitations UART driver is designed to work in interrupt mode only

    @file uart.h
    @brief Interface for UART driver
    This header file defines interface for UART driver.

    @version 1.11 Imported from docx
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
    @name UART driver API IDs
    @{
    @brief API IDs for UART drier
 */
#define BRCM_SWARCH_UART_HW_ID_TYPE           (0x00U) /**< @brief #UART_HwIDType    */
#define BRCM_SWARCH_UART_FIFO_LVL_TYPE        (0x01U) /**< @brief #UART_FifoLvlType */
#define BRCM_SWARCH_UART_BAUD_TYPE            (0x02U) /**< @brief #UART_BaudType    */
#define BRCM_SWARCH_UART_STOP_BITS_TYPE       (0x03U) /**< @brief #UART_StopBitsType*/
#define BRCM_SWARCH_UART_PARITY_TYPE          (0x04U) /**< @brief #UART_ParityType  */
#define BRCM_SWARCH_UART_STATE_TYPE           (0x05U) /**< @brief #UART_StateType   */
#define BRCM_SWARCH_UART_TX_STATUS_TYPE       (0x06U) /**< @brief #UART_TxStatusType*/
#define BRCM_SWARCH_UART_ERROR_TYPE           (0x07U) /**< @brief #UART_ErrorType   */
#define BRCM_SWARCH_UART_STATS_TYPE           (0x08U) /**< @brief #UART_StatsType   */
#define BRCM_SWARCH_UART_TX_CB_TYPE_PROC      (0x09U) /**< @brief #UART_TxCbType    */
#define BRCM_SWARCH_UART_RX_CB_TYPE_PROC      (0x0AU) /**< @brief #UART_RxCbType    */
#define BRCM_SWARCH_UART_ERR_CB_TYPE_PROC     (0x0BU) /**< @brief #UART_ErrCbType   */
#define BRCM_SWARCH_UART_CONFIG_TYPE          (0x0CU) /**< @brief #UART_ConfigType  */
#define BRCM_SWARCH_UART_INIT_PROC            (0x0DU) /**< @brief #UART_Init        */
#define BRCM_SWARCH_UART_SEND_PROC            (0x0EU) /**< @brief #UART_Send        */
#define BRCM_SWARCH_UART_GET_TX_STATUS_PROC   (0x0FU) /**< @brief #UART_GetTxStatus */
#define BRCM_SWARCH_UART_DEINIT_PROC          (0x10U) /**< @brief #UART_DeInit      */
#define BRCM_SWARCH_UART_GET_STATS_PROC       (0x11U) /**< @brief #UART_GetStats    */
#define BRCM_SWARCH_UART_SEND_BREAK_PROC      (0x12U) /**< @brief #UART_SendBreak   */
#define BRCM_SWARCH_UART_CLR_BREAK_PROC       (0x13U) /**< @brief #UART_ClrBreak    */
#define BRCM_SWARCH_UART_GET_STATE_PROC       (0x14U) /**< @brief #UART_GetState    */
/** @} */

/**
    @brief Index of the UART controller

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
*/
typedef uint32_t UART_HwIDType;

/**
    @name UART_FifoLvlType
    @{
    @brief HW RX/TX FIFO trigger point for the callback

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_FifoLvlType;       /**< @brief UART FIFO level   */
#define UART_FIFO_LVL_1DIV8      (0UL)   /**< @brief FIFO crosses 1/8  */
#define UART_FIFO_LVL_1DIV4      (1UL)   /**< @brief FIFO crosses 1/4  */
#define UART_FIFO_LVL_1DIV2      (2UL)   /**< @brief FIFO crosses 1/2  */
#define UART_FIFO_LVL_3DIV4      (3UL)   /**< @brief FIFO crosses 3/4  */
#define UART_FIFO_LVL_7DIV8      (4UL)   /**< @brief FIFO crosses 7/8  */
/** @} */

/**
    @name UART_BaudRateType
    @{
    @brief Baud rate of the UART port

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_BaudType;         /**< @brief typedef for UART baud rate */
#define UART_BAUD_9600              (0UL)   /**< @brief Baud rate 9600 bps */
#define UART_BAUD_19200             (1UL)   /**< @brief Baud rate 19200 bps */
#define UART_BAUD_115200            (2UL)   /**< @brief Baud rate 115200 bps */
/** @} */

/**
    @name UART_StopBitsType
    @{
    @brief Number of stop bits for the UART transaction

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_StopBitsType;         /**< @brief typedef for UART stop bits */
#define UART_STOP_BITS1             (1UL)   /**< @brief 1 stop bit */
#define UART_STOP_BITS2             (2UL)   /**< @brief 2 stop bits */
/** @} */

/**
    @name UART_ParityType
    @{
    @brief Parity for the UART transaction

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_ParityType;           /**< @brief typedef for the UART parity */
#define UART_PARITY_NONE            (0UL)   /**< @brief No parity */
#define UART_PARITY_EVEN            (1UL)   /**< @brief Even parity */
#define UART_PARITY_ODD             (2UL)   /**< @brief Odd parity */
/** @} */

/**
    @name UART_StateType
    @{
    @brief State of the UART driver

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
 */
typedef uint32_t UART_StateType;            /**< @brief typedef for UART states         */
#define UART_STATE_UNINIT                 (0UL)   /**< @brief UART driver is not initialized  */
#define UART_STATE_INIT                   (1UL)   /**< @brief UART driver is initialized      */
/** @} */

/**
    @name UART_TxStatusType
    @{
    @brief Status of the UART Tx

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
 */
typedef uint32_t UART_TxStatusType;         /**< @brief typedef for UART states         */
#define UART_TX_STATUS_BUFFER_IDLE  (1UL)   /**< @brief Buffer is empty. UART driver
                                                    can accept new buffer               */
#define UART_TX_STATUS_FIFO_IDLE    (2UL)   /**< @brief FIFO is empty,
                                                        no pendind data to send         */
/** @} */


/**
    @name UART_ErrorType
    @{
    @brief UART Error Type

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef uint32_t UART_ErrorType;                  /**< @brief typedef for UART errors */
#define UART_ERROR_FRAMING    (0x00000001UL)   /**< @brief Framing Error */
#define UART_ERROR_PARITY     (0x00000002UL)   /**< @brief Parity Error */
#define UART_ERROR_BREAK      (0x00000004UL)   /**< @brief Break Error */
#define UART_ERROR_OVERRUN    (0x00000008UL)   /**< @brief Overrun Error */
#define UART_ERROR_TIMEOUT    (0x00000010UL)   /**< @brief Timeout Error */
/** @} */

/**
    @brief UART status counters

    @trace #BRCM_SWREQ_UART_KERNEL_INTERFACE
 */
typedef struct _UART_StatsType {
    uint32_t isrCnt;    /**< @brief ISR counter (incremented by core driver each time ISR function is called) */
    uint32_t txIsrCnt;  /**< @brief TX ISR counter (incremented by core driver for each TX interrupt) */
    uint32_t rxIsrCnt;  /**< @brief RX ISR counter (incremented by core driver for each RX interrupt) */
    uint16_t oeErr;     /**< @brief Overrun error counter (incremented by driver for each Overrun flag set) */
    uint16_t beErr;     /**< @brief Break error counter (incremented by driver for each break condition flag) */
    uint16_t peErr;     /**< @brief Parity error counter (incremented by driver for each parity error) */
    uint16_t feErr;     /**< @brief Framing error counter (incremented by driver for each framing error) */
} UART_StatsType;

/** @brief UART Tx callback type

    This should be implemented by the client who has registered for the UART Tx
    callback

    @note This callback is called from UART interrupt context, so callback
    function shall try to return as soon as possible (avoid any heavy processing
    in the callback).

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]     aData       Pointer to input buffer that shall be filled by
                                the client
    @param[in]      aSize       Size up to which the buffer may be filled

    @return     uint32_t                Actual size of the buffer filled by the client

    @post None

    @trace #BRCM_SWREQ_UART_INIT

    @limitations None
*/
typedef void (*UART_TxCbType)(void);

/** @brief UART Rx callback type

    This should be implemented by the client who has registered for the UART Rx
    callback

    @note This callback is called from UART interrupt context, so callback
    function shall try to return as soon as possible (avoid any heavy processing
    in the callback).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aData       Pointer to data received on the UART controller
    @param[in]      aSize       Size of the data received (in bytes)

    @return     void

    @post None

    @trace #BRCM_SWREQ_UART_INIT

    @limitations None
*/
typedef void (*UART_RxCbType)(uint8_t* aData,
                              uint32_t aSize);

/** @brief UART Err callback type

    This should be implemented by the client who has registered for the UART Err
    callback

    @note This callback is called from UART interrupt context, so callback
    function shall try to return as soon as possible (avoid any heavy processing
    in the callback).

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aErr        Type of error on the UART controller
    @param[in]      aData       Data received on the UART controller

    @return     void

    @post None

    @trace #BRCM_SWREQ_UART_INIT

    @limitations None
*/
typedef void (*UART_ErrCbType)(UART_ErrorType aErr,
                               uint8_t aData);

/**
    @brief UART configuration structure

    @trace #BRCM_SWREQ_UART_INIT
 */
typedef struct _UART_ConfigType {
    UART_FifoLvlType   txFifoLvl;       /**< @brief Transmit Fifo Level */
    UART_FifoLvlType   rxFifoLvl;       /**< @brief Receive Fifo Level */
    UART_BaudType      baud;            /**< @brief Baud rate of the UART  */
    UART_StopBitsType  stopBits;        /**< @brief Number of stop bits */
    UART_ParityType    parity;          /**< @brief Parity bits */
    uint32_t           loopBackMode;    /**< @brief UART loop back mode enable flag.
                                             If Set to 1, UART shall be configured in Loopback mode */
    UART_TxCbType      txCb;            /**< @brief transmit confirmation callback */
    UART_RxCbType      rxCb;            /**< @brief recive confirmation callback   */
    UART_ErrCbType      errCb;          /**< @brief Error indication               */
} UART_ConfigType;

/** @brief Initialize UART

    This API initializes the given UART controller with the given configuration

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aConfig     Pointer to the configuration structure

    @return     void

    @post UART driver state changes to #UART_INIT

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
void UART_Init(UART_HwIDType aId);

/** @brief Send data over uart

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[in]      aData       Pointer to data to be sent
    @param[in]      aSize       Length of data to be sent

    @return     #BCM_ERR_OK             Success
                #BCM_ERR_INVAL_STATE    UART is not initialized
                #BCM_ERR_INVAL_PARAMS   aId is invalid

    @post UART driver state changes to #UART_ENABLED

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
int32_t UART_Send(UART_HwIDType aId, const uint8_t *aData, uint32_t aLength);

/** @brief Checks for transmit buffer status and transmit FIFO status

    This API checks if 
      1) all buffer is pushed into FIFO
      2) all data in FIFO is send over line

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    @return     Flags with below description
                UART_TX_STATUS_BUFFER_IDLE  if Buffer is written to FIFO
                UART_TX_STATUS_FIFO_IDLE    if FIFO is also empty along with BUFFER_IDLE

    @post None

    @trace #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern UART_TxStatusType UART_GetTxStatus(UART_HwIDType aId);

/** @brief De-initialize UART

    This API de-initializes UART. Resets the given UART controller and put back
    to its original state (same as Power on reset)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post UART driver state changes to #UART_UNINIT

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
int32_t UART_DeInit(UART_HwIDType aId);

/** @brief Get the UART statistics data

    This API retrieves the UART controller statistics in aStats

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[out]     aStats      UART statistics

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
int32_t UART_GetStats(UART_HwIDType aId,
                   UART_StatsType *const aStats);

/** @brief Send Break

    This API is used to send Break on the respective hardware ID. After calling
    this API, a continuous low-level is outputted on the UART until cleared by
    calling the #UART_ClrBreak API.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern int32_t UART_SendBreak(UART_HwIDType aId);

/** @brief Clear Break

    This API is used to Clear Break on the respective hardware ID

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aId         Index of the UART controller

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid hardware index

    @post None

    @trace  #BRCM_SWREQ_UART_COMMUNICATION

    @limitations None
*/
extern int32_t UART_ClrBreak(UART_HwIDType aId);

/** @brief Get the state of the UART

    This API retrieves the current state of the UART controller in aState
    argument

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the UART controller
    @param[out]     aState      Current state of UART

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_UART_INIT

    @limitations None
*/
int32_t UART_GetState(UART_HwIDType aId,
                      UART_StateType *const aState);

#endif /* UART_H */

/** @} */
