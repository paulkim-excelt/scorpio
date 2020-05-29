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
    @defgroup grp_pinmuxdrv_drv_impl PinMux Driver Design
    @ingroup grp_pinmuxdrv

    @addtogroup grp_pinmuxdrv_drv_impl
    @{

    @file pinmux_drv.c

    @version 0.1 Initial version
*/
#include <string.h>
#include <bcm_err.h>
#include <stddef.h>
#include <compiler.h>
#include <pinmux.h>
#include <pinmux_osil.h>
#include <gio_rdb.h>

/**
    @name PINMUX driver Design IDs
    @{
    @brief Design IDs for PINMUX driver
*/
#define BRCM_SWDSGN_PINMUX_IS_INITIALIZED_GLOBAL           (0x50U) /**< @brief #PINMUX_IsInitialized            */
#define BRCM_SWDSGN_PINMUX_REGS_GLOBAL                     (0x51U) /**< @brief #PINMUX_Regs                     */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_PROC                (0x52U) /**< @brief #PINMUX_DrvSetPin                */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_DIRECTION_PROC      (0x53U) /**< @brief #PINMUX_DrvSetPinDirection       */
#define BRCM_SWDSGN_PINMUX_DRV_SET_DRIVE_POWER_PROC        (0x54U) /**< @brief #PINMUX_DrvSetDrivePower         */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_HWMODE_PROC         (0x55U) /**< @brief #PINMUX_DrvSetPinHWMode          */
#define BRCM_SWDSGN_PINMUX_DRV_INIT_PROC                   (0x56U) /**< @brief #PINMUX_DrvInit                  */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_VAL_DIRECTION_PROC  (0x57U) /**< @brief #PINMUX_DrvSetPinValueDirection  */
#define BRCM_SWDSGN_PINMUX_DRV_REFRESH_PORT_DIRECTION_PROC (0x58U) /**< @brief #PINMUX_DrvRefreshPortDirection  */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_MODE_PROC           (0x59U) /**< @brief #PINMUX_DrvSetPinMode            */
#define BRCM_SWDSGN_PINMUX_SVCIO_TYPE                      (0x5AU) /**< @brief #PINMUX_SVCIOType                */
#define BRCM_SWDSGN_PINMUX_SYS_CMD_HANDLER_PROC            (0x5BU) /**< @brief #PINMUX_SysCmdHandler            */
#define BRCM_SWDSGN_PINMUX_DRV_SET_PIN_INTR_CFG_PROC       (0x5CU) /**< @brief #PINMUX_DrvSetPinIntrCfg         */
/** @} */

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static uint32_t PINMUX_IsInitialized COMP_SECTION(".data.drivers") = 0UL;

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static GIO_RDBType *const PINMUX_Regs[GIO_MAX_HW_ID] = {
    (GIO_RDBType *)GIO0_BASE,
#if (GIO_MAX_HW_ID > 1)
    (GIO_RDBType *)GIO1_BASE,
#endif /* GIO_MAX_HW_ID > 1 */
#if (GIO_MAX_HW_ID > 2)
#error "GIO_MAX_HW_ID > 2"
#endif /* GIO_MAX_HW_ID > 2 */
};


/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static void PINMUX_DrvSetPin(PINMUX_PinType aPin, PINMUX_PinLevelType aLevel)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        PINMUX_Regs[port]->dout = (PINMUX_PIN_LEVEL_LOW == aLevel) ?
                    (PINMUX_Regs[port]->dout & (~(0x01UL << pin_offset))) :
                    (PINMUX_Regs[port]->dout | (0x01UL << pin_offset));
    }
}

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static void PINMUX_DrvSetPinDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        PINMUX_Regs[port]->drv_en = (PINMUX_PIN_DIRECTION_IN == aDirection) ?
                                  (PINMUX_Regs[port]->drv_en & (~(1UL << pin_offset))) :
                                  (PINMUX_Regs[port]->drv_en | (1UL << pin_offset));
    }
}

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static void PINMUX_DrvSetDrivePower(PINMUX_PinType aPin, PINMUX_PinDrvPwrType aPwrSel)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;
        const uint32_t values[4] = {aPin, aPwrSel, 0UL, 0UL};
        switch (aPwrSel) {
        case PINMUX_PIN_HW_CFG_DRIVE_DEFAULT:
            /* Do Nothing */
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_2MA:
            PINMUX_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_4MA:
            PINMUX_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_6MA:
            PINMUX_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_8MA:
            PINMUX_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_10MA:
            PINMUX_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_12MA:
            PINMUX_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_14MA:
            PINMUX_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_16MA:
            PINMUX_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            PINMUX_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        default:
            /* Invalid drive power. Do nothing. */
            BCM_ReportError(BCM_PMX_ID, 0U, BRCM_SWDSGN_PINMUX_DRV_SET_DRIVE_POWER_PROC,
                                BCM_ERR_INVAL_PARAMS, 4UL, values);
            break;
        }
    }
}

/**
    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
static void PINMUX_DrvSetPinHWMode(PINMUX_PinType aPin,
                      const PINMUX_PinModeCfgType* const aConfigPtr)
{
    uint32_t modeFlags;
    GPIO_PortType port;
    uint32_t pin_offset;
    PINMUX_PinModeType mode;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        modeFlags = aConfigPtr->pinModeHWConfig;
        mode = aConfigPtr->pinMode;

        /* Check whether GPIO Pad resister is to be enabled.
         * If yes, set the resistor type depending on the configuration passed.
         */
        if (PINMUX_PIN_HW_CFG_RES_EN == (PINMUX_HW_CONFIG_GET_PADRES_EN(modeFlags))) {
            /* Enable GPIO Pad Resistor */
            PINMUX_Regs[port]->pad_resis_en |= (GIO_PAD_RES_EN << pin_offset);

            if (PINMUX_PIN_HW_CFG_RES_PULL_UP == (PINMUX_HW_CONFIG_GET_PADRES_SEL(modeFlags))) {
                /* Pull-Up select */
                PINMUX_Regs[port]->pad_res |= (GIO_PAD_RES_PULL_UP << pin_offset);
            } else {
                /* Pull-Down select */
                PINMUX_Regs[port]->pad_res &= (~(GIO_PAD_RES_PULL_UP << pin_offset));
            }
        }

        /* Check for Drive Power configuration.
         * NOTE: If no Drive Power configuration is mentioned, then the default
         * hardware configuration shall apply.
         */
        PINMUX_DrvSetDrivePower(aPin, PINMUX_HW_CONFIG_GET_DRV_PWR(modeFlags));

        /* Slew-Rate select. */
        if (PINMUX_PIN_HW_CFG_SLEW_RATE_FAST == PINMUX_HW_CONFIG_GET_SLEW_RATE(modeFlags)) {
            /* Fast slew-rate */
            PINMUX_Regs[port]->slew_ctrl &= (~(GIO_SLEW_NORMAL_SPEED << pin_offset));
        } else {
            /* Normal slew-rate */
            PINMUX_Regs[port]->slew_ctrl |= (GIO_SLEW_NORMAL_SPEED << pin_offset);
        }

        if (PINMUX_PIN_MODE_GPIO == mode) {
            /* Set AUXSEL register to GPIO mode */
            PINMUX_Regs[port]->aux_sel &= (~(GIO_AUXSEL_AUX << pin_offset));
        } else {
            /* Auxiliary Mode selected.
             * Check if the Pin mode is supported on the pin. If yes, then set the
             * pin for Auxiliary functionality.
             */
            if (BCM_ERR_OK == PINMUX_SetPadCfg(aPin, mode)) {
                PINMUX_Regs[port]->aux_sel |= (GIO_AUXSEL_AUX << pin_offset);
            }
        }
    }
}

/** @brief
 *
 * Configures interrupt typr for an individual GPIO Channel
 *
 * param[in]    aPin            ID of Pin
 *              aIntrConfig     Interrupt configuration as per GPIO_INTR_CONFIG
 *
 * return       None

    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWREQ_PINMUX
 */
static void PINMUX_DrvSetPinIntrCfg(PINMUX_PinType aPin, uint32_t aPinIntrTrig)
{
    GPIO_PortType port;
    uint32_t pinMask;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aPin % GIO_PINS_PER_PORT);

        /* Disable and clear the interrupt by default */
        PINMUX_Regs[port]->int_msk &= ~pinMask;
        PINMUX_Regs[port]->int_clr = pinMask;

        /* Unmask interrupt for A or B type interrupt for once and ever */
        if (GPIO_CHANNEL_31 >= aPin) {
            PINMUX_Regs[port]->int_msk_a |= pinMask;
        } else {
            PINMUX_Regs[port]->int_msk_b |= pinMask;
        }

        if (PINMUX_PIN_INTR_TRIG_LOW_LVL > aPinIntrTrig) {
            PINMUX_Regs[port]->int_typ &= pinMask;              /* Edge Triggered */
        } else {
            PINMUX_Regs[port]->int_typ |= pinMask;              /* Level Triggered */
        }

        if ((PINMUX_PIN_INTR_TRIG_FALLING_EDG == aPinIntrTrig) ||
            (PINMUX_PIN_INTR_TRIG_LOW_LVL     == aPinIntrTrig)) {
            PINMUX_Regs[port]->int_edg_lvl_sel &= ~pinMask;     /* Falling Edge or Low Level */
        } else {
            PINMUX_Regs[port]->int_edg_lvl_sel |= pinMask;      /* Rising Edge or High Level */
        }

        if (PINMUX_PIN_INTR_TRIG_DUAL_EDG == aPinIntrTrig) {
            PINMUX_Regs[port]->int_du_edg |= pinMask;           /* Dual Edge Enabled */
        } else {
            PINMUX_Regs[port]->int_du_edg &= ~pinMask;          /* Dual Edge Disabled */
        }
    }
}


/** @brief

    Initializes the PinMux pins

    @param[in]    None

    @return       void

    @trace #BRCM_SWARCH_PINMUX_INIT_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
 */
static void PINMUX_DrvInit(void)
{
    PINMUX_PinType pin;

    for (pin = 0UL; pin < PINMUX_Config[0].numberOfPinMuxPins; pin++) {
        uint32_t mode;
        PINMUX_PinLevelType pinLevel;

        pinLevel = PINMUX_Config[0].pins[pin].pinLevelValue;
        /* Set Pin Initial Mode */
        for (mode = 0UL; mode < PINMUX_Config[0].pins[pin].pinModeNumber; mode++) {
            if (NULL != PINMUX_Config[0].pins[pin].pinSupportedModes) {
                if (PINMUX_Config[0].pins[pin].pinSupportedModes[mode].pinMode == PINMUX_Config[0].pins[pin].pinInitialMode) {
                    PINMUX_DrvSetPinHWMode(PINMUX_Config[0].pins[pin].pinID, &(PINMUX_Config[0].pins[pin].pinSupportedModes[mode]));
                }
            }
        }
        /* Set Initial Pin Direction and Value */
        PINMUX_DrvSetPinDirection(PINMUX_Config[0].pins[pin].pinID, PINMUX_Config[0].pins[pin].pinDirection);

        /* Sets Initial Port Pin Level Value */
        if (PINMUX_PIN_DIRECTION_OUT == PINMUX_Config[0].pins[pin].pinDirection) {
            if (PINMUX_PIN_LEVEL_LOW == pinLevel){
                PINMUX_DrvSetPin(PINMUX_Config[0].pins[pin].pinID, PINMUX_PIN_LEVEL_LOW);
            } else {
                PINMUX_DrvSetPin(PINMUX_Config[0].pins[pin].pinID, PINMUX_PIN_LEVEL_HIGH);
            }
        }

        /* Set Pin Interrupt Configuration only if pin initial mode is GPIO and direction is not changeble and
           interrupt trigger type is valid */
        if ((PINMUX_PIN_MODE_GPIO == PINMUX_Config[0].pins[pin].pinInitialMode) &&
            (PINMUX_PIN_DIRECTION_NOT_CHANGEABLE == PINMUX_Config[0].pins[pin].pinDirectionChangeable) &&
            (PINMUX_PIN_INTR_TRIG_FALLING_EDG <= PINMUX_Config[0].pins[pin].pinIntrTrig) &&
            (PINMUX_PIN_INTR_TRIG_HIGH_LVL >= PINMUX_Config[0].pins[pin].pinIntrTrig)) {
            PINMUX_DrvSetPinIntrCfg(PINMUX_Config[0].pins[pin].pinID, PINMUX_Config[0].pins[pin].pinIntrTrig);
        }
    }
    PINMUX_IsInitialized = 1UL;
}

/** @brief
 *
 * Sets the Pin Direction and Initial level for output pin
 *
 * @param[in]    aPin             Pin ID Number
 * @param[in]    aDirection       Pin Direction
 * @return       void

    @trace #BRCM_SWARCH_PINMUX_SET_PIN_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
 */
static void PINMUX_DrvSetPinValueDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection)
{
    PINMUX_PinType p;

    if (GPIO_MAX_CHANNELS > aPin) {
        /* Pin Look-up */
        for (p = 0UL;( p < PINMUX_Config[0].numberOfPinMuxPins ) &&
                   ( PINMUX_Config[0].pins[p].pinID != aPin ); p++) {
            ;
        }

        if ((p < PINMUX_Config[0].numberOfPinMuxPins)
            && (PINMUX_PIN_DIRECTION_CHANGEABLE == PINMUX_Config[0].pins[p].pinDirectionChangeable)) {
            if (PINMUX_PIN_DIRECTION_OUT == aDirection ) {
                PINMUX_DrvSetPin(PINMUX_Config[0].pins[p].pinID, PINMUX_Config[0].pins[p].pinLevelValue);
            }
            PINMUX_DrvSetPinDirection(aPin, aDirection);
        }
    }
}

/** @brief
 *
 * Service to refresh the ports direction
 *
 * @param[in]    void
 *
 * @return       void
    @trace #BRCM_SWARCH_PINMUX_REFRESH_PORT_DIRECTION_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
 */
static void PINMUX_DrvRefreshPortDirection(void)
{
    PINMUX_PinType pin;

    for (pin = 0UL; pin < PINMUX_Config[0].numberOfPinMuxPins; pin++ ) {
        if (PINMUX_PIN_DIRECTION_CHANGEABLE != PINMUX_Config[0].pins[pin].pinDirectionChangeable) {
            PINMUX_DrvSetPinDirection(PINMUX_Config[0].pins[pin].pinID,
                                PINMUX_Config[0].pins[pin].pinDirection);
            }
    }
}

/** @brief
 *
 * Service to set the Pin Mode of referenced pin during runtime
 *
 * @param[in]    aPin         Pin ID number
 * @param[in]    aMode        New Pin mode to be set on PinMux pin
 * @return       void
    @trace #BRCM_SWARCH_PINMUX_SET_PIN_MODE_PROC
    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
 */
static void PINMUX_DrvSetPinMode(PINMUX_PinType aPin, PINMUX_PinModeType aMode)
{
    PINMUX_PinType p;
    uint32_t mode;

    if (GPIO_MAX_CHANNELS > aPin) {
        /* Pin Look-up */
        for (p = 0UL;( p < PINMUX_Config[0].numberOfPinMuxPins ) &&
                   ( PINMUX_Config[0].pins[p].pinID != aPin ); p++) {
            ;
        }

        if ((p < PINMUX_Config[0].numberOfPinMuxPins)
            && (PINMUX_PIN_MODE_CHANGEABLE == PINMUX_Config[0].pins[p].pinModeChangeable)) {
            /* PinModes Look-up */
            for (mode = 0UL; (mode < PINMUX_Config[0].pins[p].pinModeNumber) &&
                          (PINMUX_Config[0].pins[p].pinSupportedModes[mode].pinMode !=
                           aMode); mode++) {
                ;
            }

            if (mode < PINMUX_Config[0].pins[p].pinModeNumber) {
                /* Change Pin Mode */
                PINMUX_DrvSetPinHWMode(PINMUX_Config[0].pins[p].pinID,
                        &(PINMUX_Config[0].pins[p].pinSupportedModes[mode]));
            }
        }
    }
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX
*/
typedef union _PINMUX_SVCIOType {
    uint8_t *data;
    PINMUX_IOType *io;
} PINMUX_SVCIOType;

/**
    @code{.c}
    if pinmuxIO is not NULL
        if aMagicID is BCM_PMX_SVC_MAGIC_ID
            pinmuxIO->retVal = PINMUX_CmdHandler(aCmd, pinmuxIO)
        else
            pinmuxIO->retVal = BCM_ERR_INVAL_MAGIC
    @endcode

    @trace #BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PINMUX_KERNEL_HANDLER
*/
void PINMUX_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    PINMUX_SVCIOType pinmux;
    pinmux.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_PMX_ID == aMagicID) {
            if (PINMUX_CMD_INIT == aCmd) {
                PINMUX_DrvInit();
                retVal = BCM_ERR_OK;
            } else if (1UL == PINMUX_IsInitialized) {
                retVal = BCM_ERR_OK;
                switch (aCmd) {
                case PINMUX_CMD_SET_PIN_DIRECTION:
                    PINMUX_DrvSetPinValueDirection(pinmux.io->pin, pinmux.io->direction);
                    break;
                case PINMUX_CMD_REFRESH_PORT_DIRECTION:
                    PINMUX_DrvRefreshPortDirection();
                    break;
                case PINMUX_CMD_SET_PIN_MODE:
                    PINMUX_DrvSetPinMode(pinmux.io->pin, pinmux.io->mode);
                    break;
                default:
                    retVal = BCM_ERR_INVAL_PARAMS;
                    break;
                }
            } else {
                retVal = BCM_ERR_UNINIT;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        pinmux.io->retVal = retVal;
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_PMX_ID, 0U, BRCM_SWARCH_PINMUX_SYS_CMD_HANDLER_PROC, retVal,
                4UL, values);
    }
}

/** @} */
