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

/******************************************************************************
 File Name: gpio_gio_v1.c
 Descritpion: This file implements the gpio driver.
******************************************************************************/

/* Includes */
#include <string.h>
#include <bcm_err.h>
#include <stddef.h>
#include <compiler.h>
#include <gio_rdb.h>

#include <utils.h>
#include <gpio_osil.h>
#include <pinmux_osil.h>

/* GPIO Interrupt Config Mask as per DEFINE_GPIO_INTR_CONFIG */
#define GPIO_INTR_EDGE_LVL_MASK         (0x00000001UL)
#define GPIO_INTR_POS_NEG_MASK          (0x00000002UL)
#define GPIO_INTR_DUAL_EDGE_MASK        (0x00000004UL)

static GIO_CHAN_GRP_RDBType *const GPIO_Regs[GIO_MAX_PORTS] = {
    (GIO_CHAN_GRP_RDBType *)GIO_BASE,
#if (GIO_MAX_PORTS > 1)
    (GIO_CHAN_GRP_RDBType *)GIO1_BASE,
#endif
#if (GIO_MAX_PORTS > 2)
    (GIO_CHAN_GRP_RDBType *)GIO2_BASE,
#endif
#if (GIO_MAX_PORTS > 3)
#error "GIO_MAX_PORTS > 3) not supported"
#endif
};

uint32_t PINMUX_IsInitialized COMP_SECTION(".data.drivers") = 0UL;

extern const PINMUX_ConfigType PINMUX_Config[];
static const PINMUX_ConfigType * PinMuxConfigPtr;

COMP_INLINE GPIO_LevelType GIO_GetPin(GPIO_ChannelType aPin)
{
    GPIO_LevelType rdata;
    GPIO_PortType port;
    uint32_t pin_offset;

    if (aPin < GIO_MAX_CHANNELS) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        rdata = (((GPIO_Regs[port]->din >> pin_offset) & 0x01UL) > 0UL ?
                GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    } else {
        rdata = GPIO_LEVEL_LOW;
    }

    return rdata;
}

COMP_INLINE GPIO_PortLevelType GIO_GetPort(GPIO_PortType aPort)
{
    GPIO_PortLevelType rdata;

    rdata = (GPIO_PortLevelType)(GPIO_Regs[aPort]->din);

    return rdata;
}

COMP_INLINE void GIO_SetPin(GPIO_ChannelType aPin, GPIO_LevelType aLevel)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (aPin < GIO_MAX_CHANNELS) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        GPIO_Regs[port]->dout = (GPIO_LEVEL_LOW == aLevel) ?
                    (GPIO_Regs[port]->dout & (~(0x01UL << pin_offset))) :
                    (GPIO_Regs[port]->dout | (0x01UL << pin_offset));
    }
}

COMP_INLINE void GIO_SetPort(GPIO_PortType aPort, GPIO_PortLevelType aLevel)
{
    GPIO_Regs[aPort]->dout = aLevel;
}


COMP_INLINE void GIO_SetPinDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (aPin < GIO_MAX_CHANNELS) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        GPIO_Regs[port]->drv_en = (PINMUX_PIN_DIRECTION_IN == aDirection) ?
                                  (GPIO_Regs[port]->drv_en & (~(GIO_DRV_OUT << pin_offset))) :
                                  (GPIO_Regs[port]->drv_en | (GIO_DRV_OUT << pin_offset));
    }
}

static void GIO_SetPinIntrConfig(PINMUX_PinType aPin, uint32_t aPinIntrTrig)
{
    GPIO_PortType port;
    uint32_t pinMask;

    if (GIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aPin % GIO_PINS_PER_PORT);

        /* Disable and clear the interrupt by default */
        GPIO_Regs[port]->int_msk &= ~pinMask;
        GPIO_Regs[port]->int_clr = pinMask;

        if (PINMUX_PIN_INTR_TRIG_LOW_LVL > aPinIntrTrig) {
            GPIO_Regs[port]->int_typ &= pinMask;              /* Edge Triggered */
        } else {
            GPIO_Regs[port]->int_typ |= pinMask;              /* Level Triggered */
        }

        if ((PINMUX_PIN_INTR_TRIG_FALLING_EDG == aPinIntrTrig) ||
            (PINMUX_PIN_INTR_TRIG_LOW_LVL     == aPinIntrTrig)) {
            GPIO_Regs[port]->int_edg_lvl_sel &= ~pinMask;     /* Falling Edge or Low Level */
        } else {
            GPIO_Regs[port]->int_edg_lvl_sel |= pinMask;      /* Rising Edge or High Level */
        }

        if (PINMUX_PIN_INTR_TRIG_DUAL_EDG == aPinIntrTrig) {
            GPIO_Regs[port]->int_du_edg |= pinMask;           /* Dual Edge Enabled */
        } else {
            GPIO_Regs[port]->int_du_edg &= ~pinMask;          /* Dual Edge Disabled */
        }
    }
}

static void GIO_SetDrivePower(PINMUX_PinType aPin, uint32_t aPwrSel)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (aPin < GIO_MAX_CHANNELS) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        switch(aPwrSel) {
        case PINMUX_PIN_HW_CFG_DRIVE_2MA:
            GPIO_Regs[port]->drv_sel_0 |=  (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_4MA:
            GPIO_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 &= ~(0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_6MA:
            GPIO_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_8MA:
            GPIO_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_10MA:
            GPIO_Regs[port]->drv_sel_0 &= ~(0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        case PINMUX_PIN_HW_CFG_DRIVE_12MA:
            GPIO_Regs[port]->drv_sel_0 |= (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_1 |= (0x01UL << pin_offset);
            GPIO_Regs[port]->drv_sel_2 |= (0x01UL << pin_offset);
            break;
        default:
            /* Invalid drive power. Do nothing. */
            break;
        }
    }
}

static void GIO_SetPinHWMode(PINMUX_PinType aPin,
                      const PINMUX_PinModeCfgType* const aConfigPtr)
{
    uint32_t modeFlags, drvPwrSel;
    GPIO_PortType port;
    uint32_t pin_offset;
    PINMUX_PinModeType mode;

    if (aPin < GIO_MAX_CHANNELS) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        modeFlags = aConfigPtr->pinModeHWConfig;
        mode = aConfigPtr->pinMode;

        /* Check whether GPIO Pad resister is to be enabled.
         * If yes, set the resistor type depending on the configuration passed.
         */
        if (0UL != (modeFlags & PINMUX_PIN_RES_EN_BIT_MASK)) {
            /* Enable GPIO Pad Resistor */
            GPIO_Regs[port]->pad_resis_en |= (GIO_PAD_RES_EN << pin_offset);

            if (0UL != (modeFlags & PINMUX_PIN_RES_SEL_BIT_MASK)) {
                /* Pull-Up select */
                GPIO_Regs[port]->pad_res |= (GIO_PAD_RES_PULL_UP << pin_offset);
            } else {
                /* Pull-Down select */
                GPIO_Regs[port]->pad_res &= (~(GIO_PAD_RES_PULL_UP << pin_offset));
            }
        }

        /* Check for Drive Power configuration.
         * NOTE: If no Drive Power configuration is mentioned, then the default
         * hardware configuration shall apply.
         */
        drvPwrSel = ((modeFlags & PINMUX_PIN_DRV_PWR_BIT_MASK) >> 2UL);
        if (0UL != drvPwrSel) {
            GIO_SetDrivePower(aPin, drvPwrSel);
        }

        /* Slew-Rate select. */
        if (0UL != (modeFlags & PINMUX_PIN_SLEW_RATE_BIT_MASK)) {
            /* Fast slew-rate */
            GPIO_Regs[port]->slew_ctrl &= (~(GIO_SLEW_NORMAL_SPEED << pin_offset));
        } else {
            /* Normal slew-rate */
            GPIO_Regs[port]->slew_ctrl |= (GIO_SLEW_NORMAL_SPEED << pin_offset);
        }

        if (PINMUX_PIN_MODE_GPIO == mode) {
            /* Set AUXSEL register to GPIO mode */
            GPIO_Regs[port]->aux_sel &= (~(GIO_AUXSEL_AUX << pin_offset));
        } else {
            /* Auxiliary Mode selected. */
            GPIO_Regs[port]->aux_sel |= (GIO_AUXSEL_AUX << pin_offset);
        }
    }
}

static void GIO_InitPortPin(const PINMUX_PinCfgType* const aConfigPtr)
{
    uint32_t mode;
    PINMUX_PinLevelType pinLevel;

    pinLevel = aConfigPtr->pinLevelValue;
    /* Set Pin Initial Mode */
    for (mode = 0UL; mode < aConfigPtr->pinModeNumber; mode++) {
        if (aConfigPtr->pinSupportedModes != NULL) {
            if (aConfigPtr->pinSupportedModes[mode].pinMode == aConfigPtr->pinInitialMode) {
                GIO_SetPinHWMode(aConfigPtr->pinID, &(aConfigPtr->pinSupportedModes[mode]));
            }
        }
    }
    /* Set Initial Pin Direction and Value */
    GIO_SetPinDirection(aConfigPtr->pinID, aConfigPtr->pinDirection);

    /* Sets Initial Port Pin Level Value */
    if (PINMUX_PIN_DIRECTION_OUT == aConfigPtr->pinDirection) {
        switch (pinLevel) {
        case PINMUX_PIN_LEVEL_LOW:
            GIO_SetPin(aConfigPtr->pinID, GPIO_LEVEL_LOW);
            break;
        case PINMUX_PIN_LEVEL_HIGH:
            GIO_SetPin(aConfigPtr->pinID, GPIO_LEVEL_HIGH);
            break;
        default:
            break;
        }
    }

    /* Set Pin Interrupt Configuration only if pin initial mode is GPIO and direction is not changeble and
       interrupt trigger type is valid */
    if ((PINMUX_PIN_MODE_GPIO == aConfigPtr->pinInitialMode) &&
        (PINMUX_PIN_DIRECTION_NOT_CHANGEABLE == aConfigPtr->pinDirectionChangeable) &&
        (PINMUX_PIN_INTR_TRIG_FALLING_EDG <= aConfigPtr->pinIntrTrig) &&
        (PINMUX_PIN_INTR_TRIG_HIGH_LVL >= aConfigPtr->pinIntrTrig)) {
        GIO_SetPinIntrConfig(aConfigPtr->pinID, aConfigPtr->pinIntrTrig);
    }
}

/** @brief
 *
 * IRQ Handler for GPIO PORT 1, Channel 0 to 31
 *
 * @param[in]    None
 *
 * @return       None
 */
void GPIO_DrvIRQHandler(GPIO_PortType aPort)
{
    volatile uint32_t irq_status;
    uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
    GPIO_ChannelType channelId = GIO_PINS_PER_PORT * aPort;

    if (GIO_MAX_PORTS > aPort) {
        /* Get the current IRQ status */
        irq_status = GPIO_Regs[aPort]->int_msk_sts & GIO_GPIO_G1_INT_MSK_STS_STS_MASK;

        /* Disable the interrupt for level type only */
        GPIO_Regs[aPort]->int_msk &= ~(irq_status & GPIO_Regs[aPort]->int_typ);

        /* Clear the identified interrupt */
        GPIO_Regs[aPort]->int_clr = irq_status;

        /* Check and call the valid call back functions */
        while (0UL != irq_status) {
            if (1UL == (irq_status & 0x1UL)) {
                if (NULL != GPIO_IntrCbTbl[channelId]) {
                    GPIO_IntrCbTbl[channelId](channelId);
                } else {
                    values[3] = channelId;
                    BCM_ReportError(BCM_GIO_ID, 0UL, BRCM_SWARCH_GPIO_DRV_IRQHANDLER_PROC, BCM_ERR_UNKNOWN,
                                    4UL, values);
                }
            }
            channelId++;
            irq_status = irq_status >> 1;
        }
    } else {
        values[3] = aPort;
        BCM_ReportError(BCM_GIO_ID, 0UL, BRCM_SWARCH_GPIO_DRV_IRQHANDLER_PROC, BCM_ERR_INVAL_PARAMS,
                        4UL, values);
    }
}

/** @brief
 *
 * Enables interrupt of an individual GPIO Channel
 *
 * @param[in]    aChannelId      ID of Channel
 *
 * @return       Error type
 */
int32_t GIO_EnableInterrupt(GPIO_ChannelType aChannelId)
{
    GPIO_PortType port;
    uint32_t pinMask;
    int32_t ret = BCM_ERR_OK;

    if (aChannelId < GIO_MAX_CHANNELS) {
        port = aChannelId >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aChannelId % GIO_PINS_PER_PORT);

        /* Check if pin direction is input and callback function exists */
        if ((0UL == (pinMask & GPIO_Regs[port]->drv_en)) &&
            (NULL != GPIO_IntrCbTbl[aChannelId])) {
            GPIO_Regs[port]->int_msk |= pinMask;
        } else {
            ret = BCM_ERR_NOSUPPORT;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/** @brief
 *
 * Disables interrupt of an individual GPIO Channel. Clears the status too.
 *
 * @param[in]    aChannelId      ID of Channel
 *
 * @return       Error type
 */
int32_t GIO_DisableInterrupt(GPIO_ChannelType aChannelId)
{
    GPIO_PortType port;
    uint32_t pinMask;
    int32_t ret = BCM_ERR_OK;

    if (aChannelId < GIO_MAX_CHANNELS) {
        port = aChannelId >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aChannelId % GIO_PINS_PER_PORT);
        GPIO_Regs[port]->int_msk &= ~pinMask;
        GPIO_Regs[port]->int_clr = pinMask;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/** @brief
 *
 * Read value of an individual GPIO Channel
 *
 * @param[in]    aChannelId      ID of Channel to be read
 *
 * @return       Level of GPIO Channel
 */
GPIO_LevelType GIO_ReadChannel(GPIO_ChannelType aChannelId)
{
    GPIO_LevelType level;

    if (aChannelId >= GIO_MAX_CHANNELS) {
        level = GPIO_LEVEL_LOW;
    } else {
        level = GIO_GetPin(aChannelId);
    }

    return level;
}

/** @brief
 *
 * Write value of an individual GPIO Channel
 *
 * @param[in]    aChannelId      ID of Channel to write to
 * @param[in]    aLevel          GPIO_LEVEL_LOW OR GPIO_LEVEL_HIGH
 * @return       void
 */
void GIO_WriteChannel(GPIO_ChannelType aChannelId, GPIO_LevelType aLevel)
{
    if ((aChannelId < GIO_MAX_CHANNELS) && (aLevel <= GPIO_LEVEL_HIGH)) {
        GIO_SetPin(aChannelId, aLevel);
    }
}

/** @brief
 *
 * Read value of a GPIO Port
 *
 * @param[in]    aConfigPtr       Pointer to Configuration Structure
 * @param[in]    aPortId          ID of Port to be read
 *
 * @return       Level of GPIO Port
 */
GPIO_PortLevelType GIO_ReadPort(GPIO_PortType aPortId)
{
    GPIO_PortLevelType lvl;

    if (aPortId >= GIO_MAX_PORTS) {
        lvl = GPIO_LEVEL_LOW;
    } else {
        lvl = (GPIO_PortLevelType)GIO_GetPort(aPortId);
    }

    return lvl;
}


/** @brief
 *
 * Write value of a GPIO Port
 *
 * @param[in]    aConfigPtr      Pointer to Configuration Structure
 * @param[in]    aPortId         ID of Port to write to
 * @param[in]    aLevel          Level of GPIO Port
 * @return       void
 */
void GIO_WritePort(GPIO_PortType aPortId, GPIO_PortLevelType aLevel)
{
    if (aPortId < GIO_MAX_PORTS) {
        GIO_SetPort(aPortId, aLevel);
    }
}


/** @brief
 *
 * Read value of a GPIO Channel Group
 *
 * @param[in]    aConfigPtr          Pointer to Configuration Structure
 * @param[in]    aChannelGroupIdPtr  Pointer to Channel Group Id
 *
 * @return       Level of GPIO Port Channel Group
 */
GPIO_PortLevelType GIO_ReadChannelGroup(const GPIO_ChannelGroupType* aChannelGroupIdPtr)
{
    GPIO_PortLevelType lvl;

    if (aChannelGroupIdPtr == NULL) {
        lvl = GPIO_LEVEL_LOW;
        goto err;
    }

    if ((aChannelGroupIdPtr->port >= GIO_MAX_PORTS)
        || (aChannelGroupIdPtr-> offset >= GIO_PINS_PER_PORT)) {
        lvl = GPIO_LEVEL_LOW;
        goto err;
    }

    lvl = (( GIO_GetPort(aChannelGroupIdPtr->port) & aChannelGroupIdPtr->mask ) >>
            aChannelGroupIdPtr->offset);

err:
  return lvl;
}

/** @brief
 *
 * Write value of a GPIO Port
 *
 * @param[in]    aConfigPtr          Pointer to Configuration Structure
 * @param[in]    aChannelGroupIdPtr   Pointer to Channel Group Id
 * @param[in]    aLevel               Level of GPIO Port Channel Group
 * @return       void
 */
void GIO_WriteChannelGroup(const GPIO_ChannelGroupType* aChannelGroupIdPtr,
                           GPIO_PortLevelType aLevel )
{
    GPIO_PortLevelType currentLvl;
    if (aChannelGroupIdPtr == NULL) {
        goto err;
    }

    if ((aChannelGroupIdPtr->port >= GIO_MAX_PORTS)
        || (aChannelGroupIdPtr->offset >= GIO_PINS_PER_PORT)) {
        goto err;
    }

    currentLvl = GIO_GetPort(aChannelGroupIdPtr->port);
    currentLvl &= ~(aChannelGroupIdPtr->mask);
    currentLvl |= ((aLevel << aChannelGroupIdPtr->offset) & aChannelGroupIdPtr->mask);

    GIO_SetPort(aChannelGroupIdPtr->port, currentLvl);
err:
    return;
}

/** @brief
 *
 * Flip value of an individual GPIO Channel
 *
 * param[in]    aChannelId      ID of Channel to be read
 *
 * return       GPIO_LevelType   Level of GPIO Channel
 */
GPIO_LevelType GIO_FlipChannel(GPIO_ChannelType aChannelId)
{
    volatile GPIO_LevelType lvl;
    GPIO_LevelType newLvl;

    if (aChannelId >= GIO_MAX_CHANNELS) {
        lvl = GPIO_LEVEL_LOW;
    } else {
        lvl = GIO_GetPin(aChannelId);
        newLvl = ( lvl == GPIO_LEVEL_LOW ) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        GIO_SetPin(aChannelId, newLvl);

    lvl = GIO_GetPin(aChannelId);
    }

    return lvl;
}

/** @brief
 *
 * Initializes the PinMux pins
 *
 * @param[in]    None
 *
 * @return       void
 */
void GIO_MuxInit(void)
{
    PINMUX_PinType pin;
    PinMuxConfigPtr = &PINMUX_Config[0];

    for (pin = 0UL; pin < PinMuxConfigPtr->numberOfPinMuxPins; pin++) {
        GIO_InitPortPin(&(PinMuxConfigPtr->pins[pin]));
    }
    PINMUX_IsInitialized = TRUE;
}

/** @brief
 *
 * Sets the Pin Direction and Initial level for output pin
 *
 * @param[in]    aPin             Pin ID Number
 * @param[in]    aDirection       Pin Direction
 * @return       void
 */
void GIO_SetPinValueDirection(PINMUX_PinType aPin, PINMUX_PinDirectionType aDirection)
{
    PINMUX_PinType p;

    if (aPin < GIO_MAX_CHANNELS) {
        /* Pin Look-up */
        for (p = 0UL;( p < PinMuxConfigPtr->numberOfPinMuxPins ) &&
                   ( PinMuxConfigPtr->pins[p].pinID != aPin ); p++) {
            ;
        }

        if ((p < PinMuxConfigPtr->numberOfPinMuxPins)
            && (PINMUX_PIN_DIRECTION_CHANGEABLE == PinMuxConfigPtr->pins[p].pinDirectionChangeable)) {
            if (PINMUX_PIN_DIRECTION_OUT == aDirection ) {
                GIO_SetPin(PinMuxConfigPtr->pins[p].pinID, PinMuxConfigPtr->pins[p].pinLevelValue);
            }
            GIO_SetPinDirection(aPin, aDirection);
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
 */
void GIO_RefreshPortDirection(void)
{
    PINMUX_PinType pin;

    for (pin = 0UL; pin < PinMuxConfigPtr->numberOfPinMuxPins; pin++ ) {
        if (PinMuxConfigPtr->pins[pin].pinDirectionChangeable !=
                      PINMUX_PIN_DIRECTION_CHANGEABLE) {
            GIO_SetPinDirection(PinMuxConfigPtr->pins[pin].pinID,
                                PinMuxConfigPtr->pins[pin].pinDirection);
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
 */
void GIO_SetPinMode(PINMUX_PinType aPin, PINMUX_PinModeType aMode)
{
    PINMUX_PinType p;
    uint32_t mode;

    if (aPin < GIO_MAX_CHANNELS) {
        /* Pin Look-up */
        for (p = 0UL;( p < PinMuxConfigPtr->numberOfPinMuxPins ) &&
                   ( PinMuxConfigPtr->pins[p].pinID != aPin ); p++) {
            ;
        }

        if ((p < PinMuxConfigPtr->numberOfPinMuxPins)
            && (PINMUX_PIN_MODE_CHANGEABLE == PinMuxConfigPtr->pins[p].pinModeChangeable)) {
            /* PinModes Look-up */
            for (mode = 0UL; (mode < PinMuxConfigPtr->pins[p].pinModeNumber) &&
                          (PinMuxConfigPtr->pins[p].pinSupportedModes[mode].pinMode !=
                           aMode); mode++) {
                ;
            }

            if (mode < PinMuxConfigPtr->pins[p].pinModeNumber) {
                /* Change Pin Mode */
                GIO_SetPinHWMode(PinMuxConfigPtr->pins[p].pinID,
                        &(PinMuxConfigPtr->pins[p].pinSupportedModes[mode]));
            }
        }
    }
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _PINMUX_SVCIOType {
    uint8_t *data;
    PINMUX_IOType *io;
} PINMUX_SVCIOType;

void PINMUX_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    PINMUX_SVCIOType pinmux;
    pinmux.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_PMX_ID == aMagicID) {
            if (PINMUX_CMD_INIT == aCmd) {
                GIO_MuxInit();
                retVal = BCM_ERR_OK;
            } else if (1UL == PINMUX_IsInitialized) {
                retVal = BCM_ERR_OK;
                switch(aCmd) {
                case PINMUX_CMD_SET_PIN_DIRECTION:
                    GIO_SetPinValueDirection(pinmux.io->pin, pinmux.io->direction);
                    break;
                case PINMUX_CMD_REFRESH_PORT_DIRECTION:
                    GIO_RefreshPortDirection();
                    break;
                case PINMUX_CMD_SET_PIN_MODE:
                    GIO_SetPinMode(pinmux.io->pin, pinmux.io->mode);
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

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _GPIO_SVCIOType {
    uint8_t *data;
    GPIO_IOType *io;
} GPIO_SVCIOType;


void GPIO_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    GPIO_SVCIOType gpio;
    gpio.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_GIO_ID == aMagicID) {
            retVal = BCM_ERR_OK;
            switch (aCmd) {
            case GPIO_CMD_READ_CHANNEL:
                gpio.io->ChannelLevel = GIO_ReadChannel(gpio.io->ChannelId);
                break;
            case GPIO_CMD_WRITE_CHANNEL:
                GIO_WriteChannel(gpio.io->ChannelId, gpio.io->ChannelLevel);
                break;
            case GPIO_CMD_READ_PORT:
                gpio.io->PortLevel = GIO_ReadPort(gpio.io->PortId);
                break;
            case GPIO_CMD_WRITE_PORT:
                GIO_WritePort(gpio.io->PortId, gpio.io->PortLevel);
                break;
            case GPIO_CMD_READ_CHANNEL_GROUP:
                gpio.io->PortLevel = GIO_ReadChannelGroup(gpio.io->ChannelGroupIdPtr);
                break;
            case GPIO_CMD_WRITE_CHANNEL_GROUP:
                GIO_WriteChannelGroup(gpio.io->ChannelGroupIdPtr, gpio.io->PortLevel);
                break;
            case GPIO_CMD_FLIP_CHANNEL:
                gpio.io->ChannelLevel = GIO_FlipChannel(gpio.io->ChannelId);
                break;
            case GPIO_CMD_ENABLE_INTERRUPT:
                retVal = GIO_EnableInterrupt(gpio.io->ChannelId);
                break;
            case GPIO_CMD_DISABLE_INTERRUPT:
                retVal = GIO_DisableInterrupt(gpio.io->ChannelId);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        gpio.io->retVal = retVal;
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_GIO_ID, 0U, BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC, retVal,
                4UL, values);
    }
}
