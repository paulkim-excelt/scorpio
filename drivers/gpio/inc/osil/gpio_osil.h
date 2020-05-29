/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_gpiodrv_il GPIO Integration Guide
    @ingroup grp_gpiodrv

    @addtogroup grp_gpiodrv_il
    @{
    @section sec_gpio_integration Integration Guide
    This section describes GPIO Driver IL (Integration Layer) in detail. GPIO
    driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for GPIO Driver
    onto #SysCmdHandler.

    @section sec_gpio_il_seq Sequence Diagram
    Below figure describes the sequence of GPIO operation #GPIO_ReadChannel.
    The other GPIO APIs also follow a similar sequence.

    @todo image html gpio_seq_il_diagram_svc.jpg "GPIO Driver-SVC Layer Interaction"

    Brief description of life lines shown in above sequence diagram:
    - Client: Client or user layer
    - GPIO Driver Interface: GPIO driver interface layer
    - SVC Layer: Supervisor call layer. To be implemented by the integrator
    - GPIO Driver IL: GPIO driver integration layer

    @file gpio_osil.h
    @brief GPIO driver  Integration interfaces
    This header file contains the integration interface for GPIO driver.
    @version 1.03 Imported from docx
*/

#ifndef GPIO_OSIL_H
#define GPIO_OSIL_H

#include <gpio.h>
#include <system.h>
#include <chip_config.h>

/**
    @name GPIO interface IDs
    @{
    @brief GPIO driver integration interface IDs.
*/
#define BRCM_SWARCH_GPIO_CMD_TYPE              (0x80U) /**< @brief #GPIO_CmdType         */
#define BRCM_SWARCH_GPIO_IO_TYPE               (0x81U) /**< @brief #GPIO_IOType          */
#define BRCM_SWARCH_GPIO_SYS_CMD_REQ_PROC      (0x82U) /**< @brief #GPIO_SysCmdReq       */
#define BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC  (0x83U) /**< @brief #GPIO_SysCmdHandler   */
#define BRCM_SWARCH_GPIO_IRQ0HANDLER_PROC      (0x84U) /**< @brief #GPIO_IRQ0Handler     */
#define BRCM_SWARCH_GPIO_IRQ1HANDLER_PROC      (0x85U) /**< @brief #GPIO_IRQ1Handler     */
#define BRCM_SWARCH_GPIO_DRV_IRQHANDLER_PROC   (0x86U) /**< @brief #GPIO_DrvIRQHandler   */
#define BRCM_SWARCH_GPIO_INTR_CB_TBL_GLOBAL    (0x87U) /**< @brief #GPIO_IntrCbTbl       */
/** @} */

/**
    @name GPIO_CmdType
    @{
    @brief GPIO driver command type

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef uint32_t GPIO_CmdType;                     /**< @brief GPIO driver command type */
#define GPIO_CMD_READ_CHANNEL           (0x1UL)    /**< @brief GPIO read channel command */
#define GPIO_CMD_WRITE_CHANNEL          (0x2UL)    /**< @brief GPIO write channel command */
#define GPIO_CMD_READ_PORT              (0x3UL)    /**< @brief GPIO read port command */
#define GPIO_CMD_WRITE_PORT             (0x4UL)    /**< @brief GPIO write port command */
#define GPIO_CMD_READ_CHANNEL_GROUP     (0x5UL)    /**< @brief GPIO read channel group command */
#define GPIO_CMD_WRITE_CHANNEL_GROUP    (0x6UL)    /**< @brief GPIO write channel group command */
#define GPIO_CMD_FLIP_CHANNEL           (0x7UL)    /**< @brief GPIO flip channel command */
#define GPIO_CMD_ENABLE_INTERRUPT       (0x8UL)    /**< @brief GPIO enable interrupt command */
#define GPIO_CMD_DISABLE_INTERRUPT      (0x9UL)    /**< @brief GPIO disable interrupt command */
/** @} */

/**
    @brief GPIO IO structure

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef struct _GPIO_IOType {
    int32_t retVal;                                 /**< @brief Return value as set by #GPIO_SysCmdHandler */
    GPIO_ChannelType ChannelId;                     /**< @brief GPIO channel ID */
    GPIO_LevelType ChannelLevel;                    /**< @brief GPIO channel level */
    GPIO_PortLevelType PortLevel;                   /**< @brief GPIO port level */
    GPIO_PortType PortId;                           /**< @brief GPIO port ID */
    const GPIO_ChannelGroupType* ChannelGroupIdPtr; /**< @brief Pointer to GPIO channel group */
} GPIO_IOType;

/**
    @brief GPIO Driver Interrupt call back table.

    This callback table contains callback functions for each pin as one to one mapping. If pin does not
    have interrupt enabled, entry for that pin must be NULL. If interrupt has occured on any pin for which
    the corresponding entry in table is NULL, driver clears the interrupr and reports error.

    @trace #BRCM_SWREQ_GPIO
*/
extern const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS];

/** @brief GPIO driver system command request

    Interface to raise System Command Request for GPIO driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to In-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by #GPIO_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic ID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t GPIO_SysCmdReq(GPIO_CmdType aCmd, GPIO_IOType *const aIO);

/** @brief Handle GPIO driver commands

    This interface handles all the GPIO driver commands requested from system
    command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    GPIO driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #GPIOIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_GPIO_KERNEL_HANDLER

    @limitations None
*/
extern void GPIO_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief GPIO Interrupt Handler

    GPIO Interrupt Handler 0.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      None

    Return values are documented in reverse-chronological order
    @retval         None

    @post TBD

    @trace #BRCM_SWREQ_GPIO_KERNEL_HANDLER

    @limitations None
*/
extern void GPIO_IRQ0Handler(void);


/** @brief GPIO Interrupt Handler

    GPIO Interrupt Handler 1.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      None

    Return values are documented in reverse-chronological order
    @retval         None

    @post TBD

    @trace #BRCM_SWREQ_GPIO_KERNEL_HANDLER

    @limitations None
*/
extern void GPIO_IRQ1Handler(void);

/** @brief GPIO Driver Interrupt Handler

    GPIO Interrupt Handler in driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      None

    Return values are documented in reverse-chronological order
    @retval         None

    @post TBD

    @trace #BRCM_SWREQ_GPIO_KERNEL_HANDLER

    @limitations None
*/
extern void GPIO_DrvIRQHandler(GPIO_PortType aPort);

#endif  /* GPIO_OSIL_H */
/** @} */
