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
  @defgroup grp_gpiodrv_ifc GPIO Interface
  @ingroup grp_gpiodrv

  @addtogroup grp_gpiodrv_ifc
  @{
  @section sec_gpio_overview Overview
  @image html gpio_overview.jpg "Component Overview"
  The above figure describes GPIO driver architecture and its interaction with
  clients (application) and System

  The GPIO Driver abstracts the access to the microcontroller's hardware GPIO
  pins. Furthermore, it allows the grouping of those pins.

  The GPIO pins are shared with other peripheral functions, and when these
  shared peripherals are not enabled, the IO pins can be deployed as GPIO pins.
  Number of available GPIO pins varies with the chip-set and is based on the
  available number of balls on the package. Refer to @ref sec_ref_trm for
  details about GPIO pins.

  @section sec_gpio_fsm State Machine
  NA

  @section sec_gpio_seq Sequence Diagrams
  This section describes sequence diagrams for application programming
  interfaces to better understand GPIO driver interface. Sequence diagrams are
  based on synchronous behavior of the interface.

  This is sequence diagram for the operation #GPIO_ReadChannel. The other GPIO
  APIs follow a similar sequence as this sequence diagram.
  @image html gpio_seq_diagram_read_channel.jpg "GPIO_ReadChannel sequence"

  @section sec_gpio_perf Performance & Memory

  | Performance       | Description                                       |
  |:-----------------:|:-------------------------------------------------:|
  | CPU Load          |   NA                                              |
  | Memory Bandwidth  |   NA                                              |
  | HW Utilization    |   NA                                              |

  @includedoc drivers_gpio.dep

  @limitations None

  @file gpio.h
  @brief GPIO interface
  This header file contains the interface functions for GPIO driver.
  @version 1.03 Imported from docx
*/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
    @name GPIO driver API IDs
    @{
    @brief API IDs for GPIO driver
*/
#define BRCM_SWARCH_GPIO_LEVEL_TYPE                   (0x00U) /**< @brief #GPIO_LevelType */
#define BRCM_SWARCH_GPIO_CHANNEL_TYPE                 (0x01U) /**< @brief #GPIO_ChannelType */
#define BRCM_SWARCH_GPIO_PORT_TYPE                    (0x02U) /**< @brief #GPIO_PortType */
#define BRCM_SWARCH_GPIO_CHANNEL_GROUP_TYPE           (0x03U) /**< @brief #GPIO_ChannelGroupType */
#define BRCM_SWARCH_GPIO_PORT_LEVEL_TYPE              (0x04U) /**< @brief #GPIO_PortLevelType */
#define BRCM_SWARCH_GPIO_READ_CHANNEL_PROC            (0x05U) /**< @brief #GPIO_ReadChannel */
#define BRCM_SWARCH_GPIO_WRITE_CHANNEL_PROC           (0x06U) /**< @brief #GPIO_WriteChannel */
#define BRCM_SWARCH_GPIO_READ_PORT_PROC               (0x07U) /**< @brief #GPIO_ReadPort */
#define BRCM_SWARCH_GPIO_WRITE_PORT_PROC              (0x08U) /**< @brief #GPIO_WritePort */
#define BRCM_SWARCH_GPIO_READ_CHANNEL_GROUP_PROC      (0x09U) /**< @brief #GPIO_ReadChannelGroup */
#define BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC     (0x0AU) /**< @brief #GPIO_WriteChannelGroup */
#define BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC            (0x0BU) /**< @brief #GPIO_FlipChannel */
#define BRCM_SWARCH_GPIO_ENABLE_INTERRUPT_PROC        (0x0CU) /**< @brief #GPIO_EnableInterrupt */
#define BRCM_SWARCH_GPIO_DISABLE_INTERRUPT_PROC       (0x0DU) /**< @brief #GPIO_DisableInterrupt */
#define BRCM_SWARCH_GPIO_INTR_CB_PROC                 (0x0EU) /**< @brief #GPIO_IntrCbType */
/** @} */

/**
    @name GPIO_LevelType
    @{
    @brief GPIO channel level

    Type for the possible levels that a GPIO channel can have

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef uint8_t GPIO_LevelType; /**< @brief GPIO channel level type */
#define GPIO_LEVEL_LOW          (0x00U) /**< @brief Physical State Low */
#define GPIO_LEVEL_HIGH         (0x01U) /**< @brief Physical State High */
/** @} */

/**
    @name GPIO_ChannelType
    @{
    @brief GPIO channel type

    Specifies the numeric ID of a GPIO channel

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef uint32_t GPIO_ChannelType;              /**< @brief GPIO channel type */
#define GPIO_CHANNEL_0                  (0UL)   /**< @brief Channel 0 ID */
#define GPIO_CHANNEL_1                  (1UL)   /**< @brief Channel 1 ID */
#define GPIO_CHANNEL_2                  (2UL)   /**< @brief Channel 2 ID */
#define GPIO_CHANNEL_3                  (3UL)   /**< @brief Channel 3 ID */
#define GPIO_CHANNEL_4                  (4UL)   /**< @brief Channel 4 ID */
#define GPIO_CHANNEL_5                  (5UL)   /**< @brief Channel 5 ID */
#define GPIO_CHANNEL_6                  (6UL)   /**< @brief Channel 6 ID */
#define GPIO_CHANNEL_7                  (7UL)   /**< @brief Channel 7 ID */
#define GPIO_CHANNEL_8                  (8UL)   /**< @brief Channel 8 ID */
#define GPIO_CHANNEL_9                  (9UL)   /**< @brief Channel 9 ID */
#define GPIO_CHANNEL_10                 (10UL)  /**< @brief Channel 10 ID */
#define GPIO_CHANNEL_11                 (11UL)  /**< @brief Channel 11 ID */
#define GPIO_CHANNEL_12                 (12UL)  /**< @brief Channel 12 ID */
#define GPIO_CHANNEL_13                 (13UL)  /**< @brief Channel 13 ID */
#define GPIO_CHANNEL_14                 (14UL)  /**< @brief Channel 14 ID */
#define GPIO_CHANNEL_15                 (15UL)  /**< @brief Channel 15 ID */
#define GPIO_CHANNEL_16                 (16UL)  /**< @brief Channel 16 ID */
#define GPIO_CHANNEL_17                 (17UL)  /**< @brief Channel 17 ID */
#define GPIO_CHANNEL_18                 (18UL)  /**< @brief Channel 18 ID */
#define GPIO_CHANNEL_19                 (19UL)  /**< @brief Channel 19 ID */
#define GPIO_CHANNEL_20                 (20UL)  /**< @brief Channel 20 ID */
#define GPIO_CHANNEL_21                 (21UL)  /**< @brief Channel 21 ID */
#define GPIO_CHANNEL_22                 (22UL)  /**< @brief Channel 22 ID */
#define GPIO_CHANNEL_23                 (23UL)  /**< @brief Channel 23 ID */
#define GPIO_CHANNEL_24                 (24UL)  /**< @brief Channel 24 ID */
#define GPIO_CHANNEL_25                 (25UL)  /**< @brief Channel 25 ID */
#define GPIO_CHANNEL_26                 (26UL)  /**< @brief Channel 26 ID */
#define GPIO_CHANNEL_27                 (27UL)  /**< @brief Channel 27 ID */
#define GPIO_CHANNEL_28                 (28UL)  /**< @brief Channel 28 ID */
#define GPIO_CHANNEL_29                 (29UL)  /**< @brief Channel 29 ID */
#define GPIO_CHANNEL_30                 (30UL)  /**< @brief Channel 30 ID */
#define GPIO_CHANNEL_31                 (31UL)  /**< @brief Channel 31 ID */
#define GPIO_CHANNEL_32                 (32UL)  /**< @brief Channel 32 ID */
#define GPIO_CHANNEL_33                 (33UL)  /**< @brief Channel 33 ID */
#define GPIO_CHANNEL_34                 (34UL)  /**< @brief Channel 34 ID */
#define GPIO_CHANNEL_35                 (35UL)  /**< @brief Channel 35 ID */
#define GPIO_CHANNEL_36                 (36UL)  /**< @brief Channel 36 ID */
#define GPIO_CHANNEL_37                 (37UL)  /**< @brief Channel 37 ID */
#define GPIO_CHANNEL_38                 (38UL)  /**< @brief Channel 38 ID */
#define GPIO_CHANNEL_39                 (39UL)  /**< @brief Channel 39 ID */
/** @} */

/**
    @name GPIO_PortType
    @{
    @brief GPIO Port type
    Specifies the numeric ID of a GPIO port

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef uint32_t GPIO_PortType;                 /**< @brief GPIO Port type */
#define GPIO_PORT0                      (0UL)   /**< @brief GPIO Port ID 0 */
#define GPIO_PORT1                      (1UL)   /**< @brief GPIO Port ID 1 */
/** @} */

/**
    @brief GPIO channel group

    Type for the definition of a channel group, which consists of several
    adjoining channels within a port.

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef struct _GPIO_ChannelGroupType {
    uint32_t mask;             /**< @brief Element mask which defines the
                                    positions of the channel group */
    uint8_t offset;            /**< @brief Position of the Channel Group on the
                                    port, counted from the LSB */
    GPIO_PortType port;        /**< @brief Port on which the Channel group is
                                    defined*/
} GPIO_ChannelGroupType;

/**
    @brief GPIO port level type

    Type for the value of a GPIO port, where each bit in the value represents
    the level for each channel.

    @trace #BRCM_SWREQ_GPIO_KERNEL_INTERFACE
*/
typedef uint32_t GPIO_PortLevelType;


/** @brief GPIO Driver Interrupt Handler Callback

    Callback function for GPIO Interrupt Handler.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChannelId   Chaneel on which interrupt occurred.

    Return values are documented in reverse-chronological order
    @retval         None

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_GPIO

    @limitations None
*/
typedef void (*GPIO_IntrCbType)(GPIO_ChannelType aChannelId);


/** @brief Read level of a GPIO channel

    API to read the level of a GPIO channel.
    @note
    -# If the respective pin is not configured for GPIO function, the level
       returned is always #GPIO_LOW.
    -# If the respective GPIO pin is configured as an output, the level
       returned reflects the actual level set as output on the GPIO pin.
    -# If an invalid channel ID is passed (e.g., greater than maximum number of
       available channels), the level returned is #GPIO_LOW.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChannelId  ID of GPIO Channel

    @retval     #GPIO_HIGH              Physical level of the corresponding channel is High
    @retval     #GPIO_LOW               Physical level of the corresponding channel is Low

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
GPIO_LevelType GPIO_ReadChannel(GPIO_ChannelType aChannelId);

/** @brief Set the level of a GPIO channel

    API to set the level of a GPIO Channel.

    @note If the respective GPIO pin is not configured for GPIO output, this
    API has no effect on the pin output.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChannelId  ID of GPIO Channel
    @param[out]     aLevel      GPIO level to be set

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
void GPIO_WriteChannel(GPIO_ChannelType aChannelId, GPIO_LevelType aLevel);

/** @brief Read GPIO port level

    API to read the level of a GPIO Port.

    @note
    -# If the respective GPIO pin is not configured for GPIO function, the
       level returned for that channel is always #GPIO_LOW.
    -# If the respective GPIO pin is configured as an output, the level
       returned for that channel reflects the actual level set as output on the
       GPIO pin.
    -# If an invalid Port ID is passed (e.g., greater than maximum number of
       available ports,) the value returned is #GPIO_LOW.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aPortId     GPIO port ID

    @retval     #GPIO_PortLevelType     Level of all channels of the given Port

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
GPIO_PortLevelType GPIO_ReadPort(GPIO_PortType aPortId);

/** @brief Set GPIO port level

    API to set the level of a GPIO port.

    @note When this API is called, GPIO Channels that are not configured for
    GPIO output remains unchanged.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aPortId     GPIO port ID
    @param[in]      aLevel      Level to be written

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
void GPIO_WritePort(GPIO_PortType aPortId, GPIO_PortLevelType aLevel);

/** @brief Read GPIO channel group

    API to read a subset of the adjoining bits of a GPIO port.
    @note
    -# If the respective GPIO pin in the Channel Group is not configured for
       GPIO function, the level returned for that channel is always #GPIO_LOW.
    -# If the respective GPIO pin is configured as an output, the level
       returned for that channel reflects the actual level set as output on the
       GPIO pin.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChannelGroupIdPtr  Pointer to Channel group.
                                        @note If the pointer to channel group
                                        parameters is NULL, the value returned
                                        is #GPIO_LOW

    @retval     #GPIO_PortLevelType     Level of a subset of the adjoining bits of a port

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
GPIO_PortLevelType GPIO_ReadChannelGroup(const GPIO_ChannelGroupType *const aChannelGroupIdPtr);

/** @brief Set GPIO channel group level

    API to set a subset of the adjoining channels of a port to a specified
    level.
    @note When this API is called, GPIO Channels in the Channel Group that are
    not configured for GPIO output remains unchanged.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChannelGroupIdPtr  Pointer to channel group
    @param[in]      aLevel              Level to be written

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
void GPIO_WriteChannelGroup(const GPIO_ChannelGroupType *const aChannelGroupIdPtr,
        GPIO_PortLevelType aLevel);

/** @brief Flip GPIO channel level

    API to flip (change from High to Low or from Low to High) the level of a
    GPIO channel and return the level of the channel after flip.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aChannelId  GPIO channel ID

    @retval     #GPIO_HIGH              Level of the channel after the flip is
                                        High
    @retval     #GPIO_LOW               Level of the channel after the flip is
                                        Low

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
GPIO_LevelType GPIO_FlipChannel(GPIO_ChannelType aChannelId);


/** @brief Enables interrupt for a GPIO channel

    API to enable interupt for a  GPIO channel.
    @note
    -# If the respective pin is not configured for GPIO function, the API will
       will fail and error returned will be #BCM_ERR_NOSUPPORT.
    -# If the respective GPIO pin is configured as an output, the API will
       will fail and error returned will be #BCM_ERR_NOSUPPORT.
    -# If an invalid channel ID is passed (e.g., greater than maximum number of
       available channels), the API will fail and error returned will
       be #BCM_ERR_INVAL_PARAMS.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChannelId  ID of GPIO Channel

    @retval     #BCM_ERR_OK              Interrupt is enabled succefully.
    @retval     #BCM_ERR_NOSUPPORT       Interrupt is not supported for this pin.
    @retval     #BCM_ERR_INVAL_PARAMS    Invalid channel.

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
int32_t GPIO_EnableInterrupt(GPIO_ChannelType aChannelId);

/** @brief Disables interrupt for a GPIO channel

    API to disable interupt for a  GPIO channel.
    @note
    -# If the respective pin is not configured for GPIO function, the API will
       will fail and error returned will be #BCM_ERR_NOSUPPORT.
    -# If the respective GPIO pin is configured as an output, the API will
       will fail and error returned will be #BCM_ERR_NOSUPPORT.
    -# If an invalid channel ID is passed (e.g., greater than maximum number of
       available channels), the API will fail and error returned will
       be #BCM_ERR_INVAL_PARAMS.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChannelId  ID of GPIO Channel

    @retval     #BCM_ERR_OK              Interrupt is disabled succefully.
    @retval     #BCM_ERR_NOSUPPORT       Interrupt is not supported for this pin.
    @retval     #BCM_ERR_INVAL_PARAMS    Invalid channel.

    @post TBD

    @trace  #BRCM_SWREQ_GPIO

    @limitations None
*/
int32_t GPIO_DisableInterrupt(GPIO_ChannelType aChannelId);

#endif /* GPIO_H */
/** @} */