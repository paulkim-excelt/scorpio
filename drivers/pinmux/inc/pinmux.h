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
    @defgroup grp_pinmuxdrv_ifc PinMux Interface
    @ingroup grp_pinmuxdrv

    @addtogroup grp_pinmuxdrv_ifc
    @{
    @section sec_pinmux_overview Overview
    @image html pinmux_overview.jpg "PinMux Driver"
    The above figure depicts PinMux driver architecture and its interaction with

    The PinMux driver configures MCU pins for their respective functionality
    (for e.g. GPIO, CAN, IIC etc.) and also provides APIs to change the PinMux
    settings at run time. The PinMux driver defines a configuration structure
    (#PINMUX_ConfigType) for this purpose.

    The PinMux driver shall be initialized at early stage of boot-up before
    initialization of any other MCU drivers.

    The PinMux Driver provides the following configurations for the MCU pins:
    - Pin Mode (refer to #PINMUX_PinModeType)
    - Pin protection (Pin direction/ Pin mode changeable during runtime)
    - Pin hardware configurations
      + Pin direction (input/output)
      + Pin level initial value
      + Slew rate control
      + Activation of internal pull-ups and pull-downs
      + Drive strength

    @section sec_pinmux_fsm State Machine
    NA

    @section sec_pinmux_seq Sequence Diagrams
    This section describes sequence diagrams for application programming
    interfaces to better understand PinMux driver interface. Sequence diagrams
    are based on synchronous behavior of the interface.

    This is an example of a sequence diagram for PinMux operations. PinMux
    operation in below diagram is represented by #PINMUX_SetPinDirection. Other
    operations such as #PINMUX_RefreshPortDirection, #PINMUX_SetPinMode, etc.
    follow a similar sequence.

    @image html pinmux_seq_diagram_generic.jpg "Generic use-case"

    @section sec_pinmux_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_pinmux.deps

    @limitations None

    @file pinmux.h
    @brief PinMux Driver interface
    This header file contains the interface functions for PinMux driver
    @version 1.12 Imported fron docx
*/

#ifndef PINMUX_H
#define PINMUX_H

#include <stdint.h>
#include <gpio.h>

/**
    @name PinMux driver API IDs
    @{
    @brief API IDs for PinMux driver
*/
#define BRCM_SWARCH_PINMUX_PIN_TYPE                       (0x00U) /**< @brief #PINMUX_PinType */
#define BRCM_SWARCH_PINMUX_PIN_DIRECTION_TYPE             (0x01U) /**< @brief #PINMUX_PinDirectionType */
#define BRCM_SWARCH_PINMUX_PIN_LEVEL_TYPE                 (0x02U) /**< @brief #PINMUX_PinLevelType */
#define BRCM_SWARCH_PINMUX_PIN_HW_CFG_RES_PULL_TYPE       (0x03U) /**< @brief #PINMUX_PinResSelType */
#define BRCM_SWARCH_PINMUX_PIN_HW_CFG_RES_TYPE            (0x04U) /**< @brief #PINMUX_PinResEnType */
#define BRCM_SWARCH_PINMUX_PIN_HW_CFG_DRIVE_TYPE          (0x05U) /**< @brief #PINMUX_PinDrvPwrType */
#define BRCM_SWARCH_PINMUX_PIN_HW_CFG_SLEW_RATE_TYPE      (0x06U) /**< @brief #PINMUX_PinSlewRateType */
#define BRCM_SWARCH_PINMUX_HW_CONFIG_MACRO                (0x07U) /**< @brief #PINMUX_HW_CONFIG */
#define BRCM_SWARCH_PINMUX_PIN_MODE_TYPE                  (0x08U) /**< @brief #PINMUX_PinModeType */
#define BRCM_SWARCH_PINMUX_PIN_MODE_CFG_TYPE              (0x09U) /**< @brief #PINMUX_PinModeCfgType */
#define BRCM_SWARCH_PINMUX_PIN_DIRECTION_MACRO            (0x0AU) /**< @brief #PINMUX_PIN_DIRECTION_NOT_CHANGEABLE */
#define BRCM_SWARCH_PINMUX_PIN_MODE_MACRO                 (0x0BU) /**< @brief #PINMUX_PIN_MODE_NOT_CHANGEABLE */
#define BRCM_SWARCH_PINMUX_PIN_CFG_TYPE                   (0x0CU) /**< @brief #PINMUX_PinCfgType */
#define BRCM_SWARCH_PINMUX_CONFIG_TYPE                    (0x0DU) /**< @brief #PINMUX_ConfigType */
#define BRCM_SWARCH_PINMUX_CONFIG_GLOBAL                  (0x0EU) /**< @brief #PINMUX_Config */
#define BRCM_SWARCH_PINMUX_INIT_PROC                      (0x0FU) /**< @brief #PINMUX_Init */
#define BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC         (0x10U) /**< @brief #PINMUX_SetPinDirection */
#define BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC    (0x11U) /**< @brief #PINMUX_RefreshPortDirection */
#define BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC              (0x12U) /**< @brief #PINMUX_SetPinMode */
#define BRCM_SWARCH_PINMUX_PIN_INTR_TRIG_TYPE             (0x13U) /**< @brief #PINMUX_PinIntrTrigType */
/** @} */

/**
    @brief Type for PinMux Pin

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinType;

/**
    @name PINMUX_PinDirectionType
    @{
    @brief Pin direction

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint8_t PINMUX_PinDirectionType;    /**< @brief Type for direction of PinMux Pin */
#define PINMUX_PIN_DIRECTION_IN          (1U)         /**< @brief PinMux Pin direction set as input */
#define PINMUX_PIN_DIRECTION_OUT         (2U)         /**< @brief PinMux Pin direction set as onput */
/** @} */

/**
    @name PINMUX_PinLevelType
    @{
    @brief Pin level type

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint8_t PINMUX_PinLevelType;        /**< @brief Type for PinMux Pin level */
#define PINMUX_PIN_LEVEL_LOW   (1U)         /**< @brief PinMux Pin level low */
#define PINMUX_PIN_LEVEL_HIGH  (2U)         /**< @brief PinMux Pin level high */
/** @} */

/*
 * PinMux Pin Mode Hardware Configuration Data Values.
 */

/**
    @name PINMUX_PinResSelType
    @{
    @brief Pad resistor selection

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinResSelType;              /**< @brief Type for GPIO Pad resistor selection */
#define PINMUX_PIN_HW_CFG_RES_PULL_DWN      (0UL)   /**< @brief On-chip resistor pull-down */
#define PINMUX_PIN_HW_CFG_RES_PULL_UP       (1UL)   /**< @brief On-chip resistor pull-up */
/** @} */

/**
    @name PINMUX_PinResEnType
    @{
    @brief Pad resistor enable flag

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinResEnType;               /**< @brief Type for Pad resistor enable flag */
#define PINMUX_PIN_HW_CFG_RES_DIS           (0UL)   /**< @brief Pad resistor disable */
#define PINMUX_PIN_HW_CFG_RES_EN            (1UL)   /**< @brief Pad resistor enable */
/** @} */

/**
    @name PINMUX_PinDrvPwrType
    @{
    @brief Pin Drive Power Select

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinDrvPwrType;              /**< @brief Type for Pin Drive Power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_DEFAULT     (0x0UL)   /**< @brief Default Drive power as described in TRM*/
#define PINMUX_PIN_HW_CFG_DRIVE_2MA         (0x1UL)   /**< @brief 2-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_4MA         (0x2UL)   /**< @brief 4-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_6MA         (0x3UL)   /**< @brief 6-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_8MA         (0x4UL)   /**< @brief 8-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_10MA        (0x5UL)   /**< @brief 10-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_12MA        (0x6UL)   /**< @brief 12-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_14MA        (0x7UL)   /**< @brief 14-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_16MA        (0x8UL)   /**< @brief 16-mA Drive power Select */
#define PINMUX_PIN_HW_CFG_DRIVE_MASK        (0xFUL)   /**< @brief 16-mA Drive power Select */
/** @} */

/**
    @name PINMUX_PinSlewRateType
    @{
    @brief Pin Slew Rate

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinSlewRateType;            /**< @brief Type for Pin Slew Rate */
#define PINMUX_PIN_HW_CFG_SLEW_RATE_NORMAL  (0UL)   /**< @brief Normal Slew-Rate */
#define PINMUX_PIN_HW_CFG_SLEW_RATE_FAST    (1UL)   /**< @brief Fast Slew-Rate */
/** @} */

/**
    @brief Generate PinMux Hardware configuration

    Macro to generate PinMux Hardware configuration.

    @param[in]      aPadResEn   GPIO Pad resistor enable flag (refer to #PINMUX_PinResEnType)
    @param[in]      aPadResSel  GPIO Pad resistor select (refer to #PINMUX_PinResSelType)
    @param[in]      aDrvPwr     PinMux pin drive power select (refer to #PINMUX_PinDrvPwrType)
    @param[in]      aSlewRate   Pin Slew rate (refer to #PINMUX_PinSlewRateType)

    @retval     uint32_t              PinMux Hardware Configuration

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
#define PINMUX_HW_CONFIG(aPadResEn, aPadResSel, aDrvPwr, aSlewRate)  \
            ((aPadResEn) | ((aPadResSel) << 1UL) | ((aDrvPwr) << 2UL) |  ((aSlewRate) << 6UL))
#define PINMUX_HW_CONFIG_GET_PADRES_EN(aConfig)    \
            ((aConfig) & PINMUX_PIN_HW_CFG_RES_EN)              /**< @brief Extract Pad Resistor Enable Flag */
#define PINMUX_HW_CONFIG_GET_PADRES_SEL(aConfig)    \
            (((aConfig) >> 1UL) & PINMUX_PIN_HW_CFG_RES_PULL_UP)/**< @brief Extract Pad Resistor Select */
#define PINMUX_HW_CONFIG_GET_DRV_PWR(aConfig)    \
            (((aConfig) >> 2UL) & PINMUX_PIN_HW_CFG_DRIVE_MASK) /**< @brief Extract Pad Drive Strength */
#define PINMUX_HW_CONFIG_GET_SLEW_RATE(aConfig)    \
            (((aConfig) >> 6UL) & PINMUX_PIN_HW_CFG_SLEW_RATE_FAST) /**< @brief Extract Pad Slew Rate */

/**
    @name PINMUX_PinModeType
    @{
    @brief PinMux pin mode

    @note It may not be possible to configure individual pins with a particular
    mode. Depending on the chip-set, at a time only a group of pins might be
    configured with a particular pin mode. Refer to @ref sec_ref_trm for more
    details.

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinModeType;                        /**< @brief Type for Pin Mode */
#define PINMUX_PIN_MODE_DISABLE     (0UL)                       /**< @brief Pin disabled */
#define PINMUX_PIN_MODE_SPI         (1UL)                       /**< @brief PinMux Mode SPI */
#define PINMUX_PIN_MODE_QSPI        (2UL)                       /**< @brief PinMux Mode QPI */
#define PINMUX_PIN_MODE_IIC         (3UL)                       /**< @brief PinMux Mode IIC */
#define PINMUX_PIN_MODE_I2S         (4UL)                       /**< @brief PinMux Mode I2S */
#define PINMUX_PIN_MODE_CCT         (5UL)                       /**< @brief PinMux Mode CCT */
#define PINMUX_PIN_MODE_CCT_ETR     (6UL)                       /**< @brief PinMux Mode CCT */
#define PINMUX_PIN_MODE_AVT_MCLK    (7UL)                       /**< @brief PinMux Mode AVT MCLK */
#define PINMUX_PIN_MODE_AVT_GPICLK  (8UL)                       /**< @brief PinMux Mode AVT GPICLK */
#define PINMUX_PIN_MODE_AVT_TRIGGER (9UL)                       /**< @brief PinMux Mode AVT TRIGGER */
#define PINMUX_PIN_MODE_RESET_OUT   (10UL)                      /**< @brief PinMux Mode RESET */
#define PINMUX_PIN_MODE_CAN         (11UL)                      /**< @brief PinMux Mode CAN */
#define PINMUX_PIN_MODE_UART0       (12UL)                      /**< @brief PinMux Mode UART0 */
#define PINMUX_PIN_MODE_UART1       (13UL)                      /**< @brief PinMux Mode UART1 */
#define PINMUX_PIN_MODE_UART2       (14UL)                      /**< @brief PinMux Mode UART2 */
#define PINMUX_PIN_MODE_PWM         (15UL)                      /**< @brief PinMux Mode PWM */
#define PINMUX_PIN_MODE_SDIO        (16UL)                      /**< @brief PinMux Mode SDIO */
#define PINMUX_PIN_MODE_MDIO        (17UL)                      /**< @brief PinMux Mode MDIO */
#define PINMUX_PIN_MODE_PDM         (18UL)                      /**< @brief PinMux Mode MDIO */
#define PINMUX_PIN_MODE_GPIO        (19UL)                      /**< @brief PinMux Mode GPIO */
#define PINMUX_PIN_MODE_COUNT     (PINMUX_PIN_MODE_GPIO + 1UL)    /**< @brief Number of PinMux modes */
/** @} */

/**
    @brief PinMux mode configuration

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
 */
typedef struct _PINMUX_PinModeCfgType {
    PINMUX_PinModeType pinMode;  /**< @brief Pin Mode */
    uint32_t pinModeHWConfig;    /**< @brief Pin hardware configuration
                                      (generated using #PINMUX_HW_CONFIG) */
} PINMUX_PinModeCfgType;

/**
    @name Pin direction changeable
    @{
    @brief Defines whether a PinMux pin direction is changeable at runtime or not

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
#define PINMUX_PIN_DIRECTION_NOT_CHANGEABLE    (0UL)   /**< @brief Pin direction not changeable */
#define PINMUX_PIN_DIRECTION_CHANGEABLE        (1UL)   /**< @brief Pin direction changeable */
/** @} */

/**
    @name Pin mode changeable
    @{
    @brief Defines whether a PinMux pin mode is changeable at runtime or not

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
#define PINMUX_PIN_MODE_NOT_CHANGEABLE     (0UL)   /**< @brief Pin mode not changeable */
#define PINMUX_PIN_MODE_CHANGEABLE         (1UL)   /**< @brief Pin mode changeable */
/** @} */


/**
    @name PINMUX_PinIntrTrigType
    @{
    @brief This contains details of pin interrupt trigger type

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
typedef uint32_t PINMUX_PinIntrTrigType;
#define PINMUX_PIN_INTR_TRIG_FALLING_EDG    (1UL)
#define PINMUX_PIN_INTR_TRIG_RISING_EDG     (2UL)
#define PINMUX_PIN_INTR_TRIG_DUAL_EDG       (3UL)
#define PINMUX_PIN_INTR_TRIG_LOW_LVL        (4UL)
#define PINMUX_PIN_INTR_TRIG_HIGH_LVL       (5UL)
/** @} */

/**
    @brief Configuration of an individual pin

    Structure containing configuration of an individual pin.

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
 */
typedef struct _PINMUX_PinCfgType {
    PINMUX_PinDirectionType pinDirection;                   /**< @brief Initial direction of the PinMux Pin */
    uint32_t pinDirectionChangeable;                        /**< @brief Indicates if pin direction
                                                                 is changeable during runtime
                                                                  - #PINMUX_PIN_DIRECTION_NOT_CHANGEABLE
                                                                  - #PINMUX_PIN_DIRECTION_CHANGEABLE */
    PINMUX_PinType pinID;                                   /**< @brief ID of the Pin */
    PINMUX_PinModeType pinInitialMode;                      /**< @brief Initial mode of the pin (PinMux
                                                                 driver sets the mode of this pin during
                                                                 #PinMux_Init) */
    PINMUX_PinLevelType pinLevelValue;                      /**< @brief Pin Level Value */
    uint32_t pinModeNumber;                                 /**< @brief Number of supported Pin Modes */
    const PINMUX_PinModeCfgType * const pinSupportedModes;  /**< @brief List of Pin Mode configurations */
    uint32_t pinModeChangeable;                             /**< @brief Indicates if pin mode is
                                                                 changeable during runtime
                                                                 - #PINMUX_PIN_MODE_NOT_CHANGEABLE
                                                                 - #PINMUX_PIN_MODE_CHANGEABLE */
    PINMUX_PinIntrTrigType pinIntrTrig;                     /**< @brief Pin interrupt trigger type */
} PINMUX_PinCfgType;

/**
    @brief PinMux driver configuration

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
 */
typedef struct _PINMUX_ConfigType {
    uint32_t numberOfPinMuxPins;            /**< @brief Number of pins (size of the PinMux configuration table) */
    const PINMUX_PinCfgType* const pins;    /**< @brief Pin configuration table */
} PINMUX_ConfigType;


/**
    @brief Board/Chip/Application specific Pin Mux Configuration Table

    @trace #BRCM_SWREQ_PINMUX_KERNEL_INTERFACE
*/
extern const PINMUX_ConfigType PINMUX_Config[];

/** @brief Initialize PinMux Driver

    This API initializes the PinMux driver. It reads the PinMux configuration
    table (#PINMUX_Config) and applies it.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PINMUX

    @limitations None
*/
void PINMUX_Init(void);

/** @brief Set the PinMux Pin Direction

    API to set the PinMux Pin Direction.
    This API changes pin direction, only if the pin is configured as
    #PIN_DIRECTION_CHANGEABLE. If aDirection is set as #PINMUX_PIN_DIRECTION_OUT, then the
    initial pin level value is also set as configured.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aPin        PinMux pin ID number
    @param[in]      aDirection  PinMux pin direction

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PINMUX

    @limitations None
*/
void PINMUX_SetPinDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection);

/** @brief Refresh the port direction

    This API refreshes the direction of pins that are configured as
    #PINMUX_PIN_DIRECTION_NOT_CHANGEABLE.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PINMUX

    @limitations None
*/
void PINMUX_RefreshPortDirection(void);

/** @brief Set the PinMux Pin Mode of pin at runtime

    This API sets the mode of the referenced pin, only if the pin is configured
    as #PIN_MODE_CHANGEABLE.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aPin        PinMux pin ID number
    @param[in]      aMode       New PinMux pin mode to be set on pin

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PINMUX

    @limitations Although multiple clients may use services provided by PinMux
    driver, responsibility falls on the client/user to ensure that same pins are
    not configured for different purposes by multiple clients at the same time.
*/
void PINMUX_SetPinMode(PINMUX_PinType aPin, PINMUX_PinModeType aMode);

#endif /* PINMUX_H */
/** @} */
