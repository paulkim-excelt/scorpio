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
    @defgroup grp_gpiodrv_v2_impl GPIO Driver Design
    @ingroup grp_gpiodrv

    @addtogroup grp_gpiodrv_v2_impl
    @{

    @file gpio_drv_v2.c

    @version 0.1 Initial version
*/
#include <string.h>
#include <bcm_err.h>
#include <stddef.h>
#include <compiler.h>
#include <gpio.h>
#include <gpio_osil.h>
#include <gio_rdb.h>
#include <pinmux.h>

/**
    @name GPIO driver Design IDs
    @{
    @brief Design IDs for GPIO driver
*/
#define BRCM_SWDSGN_GPIO_DRV_REGS_GLOBAL                (0x80U) /**< @brief #GPIO_Regs                  */
#define BRCM_SWDSGN_GPIO_DRV_GET_PIN_PROC               (0x81U) /**< @brief #GPIO_DrvGetPin             */
#define BRCM_SWDSGN_GPIO_DRV_GET_PORT_PROC              (0x82U) /**< @brief #GPIO_DrvGetPort            */
#define BRCM_SWDSGN_GPIO_DRV_SET_PIN_PROC               (0x83U) /**< @brief #GPIO_DrvSetPin             */
#define BRCM_SWDSGN_GPIO_DRV_SET_PORT_PROC              (0x84U) /**< @brief #GPIO_DrvSetPort            */
#define BRCM_SWDSGN_GPIO_DRV_READ_CHANNEL_PROC          (0x85U) /**< @brief #GPIO_DrvReadChannel        */
#define BRCM_SWDSGN_GPIO_DRV_WRITE_CHANNEL_PROC         (0x86U) /**< @brief #GPIO_DrvWriteChannel       */
#define BRCM_SWDSGN_GPIO_DRV_READ_PORT_PROC             (0x87U) /**< @brief #GPIO_DrvReadPort           */
#define BRCM_SWDSGN_GPIO_DRV_WRITE_PORT_PROC            (0x88U) /**< @brief #GPIO_DrvWritePort          */
#define BRCM_SWDSGN_GPIO_DRV_READ_CHANNEL_GROUP_PROC    (0x89U) /**< @brief #GPIO_DrvReadChannelGroup   */
#define BRCM_SWDSGN_GPIO_DRV_WRITE_CHANNEL_GROUP_PROC   (0x8AU) /**< @brief #GPIO_DrvWriteChannelGroup  */
#define BRCM_SWDSGN_GPIO_DRV_FLIP_CHANNEL_PROC          (0x8BU) /**< @brief #GPIO_DrvFlipChannel        */
#define BRCM_SWDSGN_GPIO_SVCIO_TYPE                     (0x8CU) /**< @brief #GPIO_SVCIOType             */
#define BRCM_SWDSGN_GPIO_SYS_CMD_HANDLER_PROC           (0x8DU) /**< @brief #GPIO_SysCmdHandler         */
#define BRCM_SWDSGN_GPIO_DRV_IRQHANDLER_PROC            (0x8EU) /**< @brief #GPIO_DrvIRQHandler         */
#define BRCM_SWDSGN_GPIO_DRV_ENABLE_INTERRUPT_PROC      (0x8FU) /**< @brief #GPIO_DrvEnableInterrupt    */
#define BRCM_SWDSGN_GPIO_DRV_DISABLE_INTERRUPT_PROC     (0x90U) /**< @brief #GPIO_DrvDisableInterrupt   */
/** @} */

/**
    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_READ_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_WRITE_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
static GIO_RDBType *const GPIO_Regs[GIO_MAX_HW_ID] = {
    (GIO_RDBType *)GIO0_BASE,
#if (GIO_MAX_HW_ID > 1)
    (GIO_RDBType *)GIO1_BASE,
#endif /* GIO_MAX_HW_ID > 1 */
#if (GIO_MAX_HW_ID > 2)
#error "GIO_MAX_HW_ID > 2"
#endif /* GIO_MAX_HW_ID > 2 */
};

/**
    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
static GPIO_LevelType GPIO_DrvGetPin(GPIO_ChannelType aPin)
{
    GPIO_LevelType rdata;
    GPIO_PortType port;
    uint32_t pin_offset;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        rdata = (((GPIO_Regs[port]->din >> pin_offset) & 0x01UL) > 0UL ?
                GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    } else {
        rdata = GPIO_LEVEL_LOW;
    }

    return rdata;
}

/**
    @trace #BRCM_SWARCH_GPIO_READ_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
static GPIO_PortLevelType GPIO_DrvGetPort(GPIO_PortType aPort)
{
    GPIO_PortLevelType rdata;

    rdata = (GPIO_PortLevelType)(GPIO_Regs[aPort]->din);

    return rdata;
}

/**
    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
static void GPIO_DrvSetPin(GPIO_ChannelType aPin, GPIO_LevelType aLevel)
{
    GPIO_PortType port;
    uint32_t pin_offset;

    if (GPIO_MAX_CHANNELS > aPin) {
        port = aPin >> GIO_PORT_SHIFT;
        pin_offset = aPin % GIO_PINS_PER_PORT;

        GPIO_Regs[port]->dout = (GPIO_LEVEL_LOW == aLevel) ?
                    (GPIO_Regs[port]->dout & (~(0x01UL << pin_offset))) :
                    (GPIO_Regs[port]->dout | (0x01UL << pin_offset));
    }
}

/**
    @trace #BRCM_SWARCH_GPIO_WRITE_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
static void GPIO_DrvSetPort(GPIO_PortType aPort, GPIO_PortLevelType aLevel)
{
    GPIO_Regs[aPort]->dout = aLevel;
}


/** @brief
 *
 * Read value of an individual GPIO Channel
 *
 * @param[in]    aChannelId      ID of Channel to be read
 *
 * @return       Level of GPIO Channel

    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static GPIO_LevelType GPIO_DrvReadChannel(GPIO_ChannelType aChannelId)
{
    GPIO_LevelType level;

    if (GPIO_MAX_CHANNELS <= aChannelId) {
        level = GPIO_LEVEL_LOW;
    } else {
        level = GPIO_DrvGetPin(aChannelId);
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

    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static void GPIO_DrvWriteChannel(GPIO_ChannelType aChannelId, GPIO_LevelType aLevel)
{
    if ((GPIO_MAX_CHANNELS > aChannelId) && (GPIO_LEVEL_HIGH >= aLevel)) {
        GPIO_DrvSetPin(aChannelId, aLevel);
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

    @trace #BRCM_SWARCH_GPIO_READ_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static GPIO_PortLevelType GPIO_DrvReadPort(GPIO_PortType aPortId)
{
    GPIO_PortLevelType lvl;

    if (GIO_MAX_HW_ID <= aPortId) {
        lvl = GPIO_LEVEL_LOW;
    } else {
        lvl = (GPIO_PortLevelType)GPIO_DrvGetPort(aPortId);
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

    @trace #BRCM_SWARCH_GPIO_WRITE_PORT_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static void GPIO_DrvWritePort(GPIO_PortType aPortId, GPIO_PortLevelType aLevel)
{
    if (GIO_MAX_HW_ID > aPortId) {
        GPIO_DrvSetPort(aPortId, aLevel);
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

    @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static GPIO_PortLevelType GPIO_DrvReadChannelGroup(const GPIO_ChannelGroupType* aChannelGroupIdPtr)
{
    GPIO_PortLevelType lvl;

    if (NULL == aChannelGroupIdPtr) {
        lvl = GPIO_LEVEL_LOW;
        goto err;
    }

    if ((GIO_MAX_HW_ID <= aChannelGroupIdPtr->port)
        || (GIO_PINS_PER_PORT <= aChannelGroupIdPtr-> offset)) {
        lvl = GPIO_LEVEL_LOW;
        goto err;
    }

    lvl = (( GPIO_DrvGetPort(aChannelGroupIdPtr->port) & aChannelGroupIdPtr->mask ) >>
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

    @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static void GPIO_DrvWriteChannelGroup(const GPIO_ChannelGroupType* aChannelGroupIdPtr,
                           GPIO_PortLevelType aLevel )
{
    GPIO_PortLevelType currentLvl;
    if (NULL == aChannelGroupIdPtr) {
        goto err;
    }

    if ((GIO_MAX_HW_ID <= aChannelGroupIdPtr->port)
        || (GIO_PINS_PER_PORT <= aChannelGroupIdPtr->offset)) {
        goto err;
    }

    currentLvl = GPIO_DrvGetPort(aChannelGroupIdPtr->port);
    currentLvl &= ~(aChannelGroupIdPtr->mask);
    currentLvl |= ((aLevel << aChannelGroupIdPtr->offset) & aChannelGroupIdPtr->mask);

    GPIO_DrvSetPort(aChannelGroupIdPtr->port, currentLvl);
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

    @trace #BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC
    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static GPIO_LevelType GPIO_DrvFlipChannel(GPIO_ChannelType aChannelId)
{
    volatile GPIO_LevelType lvl;
    GPIO_LevelType newLvl;

    if (GPIO_MAX_CHANNELS <= aChannelId) {
        lvl = GPIO_LEVEL_LOW;
    } else {
        lvl = GPIO_DrvGetPin(aChannelId);
        newLvl = (GPIO_LEVEL_LOW == lvl) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        GPIO_DrvSetPin(aChannelId, newLvl);

    lvl = GPIO_DrvGetPin(aChannelId);
    }

    return lvl;
}

/**
    @brief Dummy IRQ Handler

    @trace #BRCM_SWARCH_GPIO_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
void GPIO_DrvIRQHandler(GPIO_PortType aPort)
{
    volatile uint32_t irq_status;
    uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
    GPIO_ChannelType channelId = GIO_PINS_PER_PORT * aPort;

    if (GIO_MAX_HW_ID > aPort) {
        /* Get the current IRQ status */
        irq_status = GPIO_Regs[aPort]->int_msk_sts & GIO_GPIO_INT_STS_FULL_ACCESS_STS_MASK;

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
 * Enables interrupt for an individual GPIO Channel
 *
 * param[in]    aChannelId      ID of Channel
 *
 * return       Error type

    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static int32_t GPIO_DrvEnableInterrupt(GPIO_ChannelType aChannelId)
{
    GPIO_PortType port;
    uint32_t pinMask;
    int32_t ret = BCM_ERR_OK;

    if (aChannelId < GPIO_MAX_CHANNELS) {
        port = aChannelId >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aChannelId % GIO_PINS_PER_PORT);
        if ((0U == (pinMask & GPIO_Regs[port]->drv_en)) &&
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
 * Disable and clear interrupt for an individual GPIO Channel
 *
 * param[in]    aChannelId      ID of Channel
 *
 * return       Error type

    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
 */
static int32_t GPIO_DrvDisableInterrupt(GPIO_ChannelType aChannelId)
{
    GPIO_PortType port;
    uint32_t pinMask;
    int32_t ret = BCM_ERR_OK;

    if (aChannelId < GPIO_MAX_CHANNELS) {
        port = aChannelId >> GIO_PORT_SHIFT;
        pinMask = 0x01UL << (aChannelId % GIO_PINS_PER_PORT);
        GPIO_Regs[port]->int_msk &= ~pinMask;
        GPIO_Regs[port]->int_clr = pinMask;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}


/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
typedef union _GPIO_SVCIOType {
    uint8_t *data;
    GPIO_IOType *io;
} GPIO_SVCIOType;

/**
    @code{.c}
    if aSysIO.gpioIO is not NULL
        if aMagicID is SVC_MAGIC_GIO_ID
            aSysIO.gpioIO->retVal = GPIO_CmdHandler(aCmd, aSysIO.gpioIO)
        else
            gpioIO->retVal = BCM_ERR_INVAL_MAGIC
    @endcode

    @trace #BRCM_SWARCH_GPIO_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_GPIO
*/
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
                gpio.io->ChannelLevel = GPIO_DrvReadChannel(gpio.io->ChannelId);
                break;
            case GPIO_CMD_WRITE_CHANNEL:
                GPIO_DrvWriteChannel(gpio.io->ChannelId, gpio.io->ChannelLevel);
                break;
            case GPIO_CMD_READ_PORT:
                gpio.io->PortLevel = GPIO_DrvReadPort(gpio.io->PortId);
                break;
            case GPIO_CMD_WRITE_PORT:
                GPIO_DrvWritePort(gpio.io->PortId, gpio.io->PortLevel);
                break;
            case GPIO_CMD_READ_CHANNEL_GROUP:
                gpio.io->PortLevel = GPIO_DrvReadChannelGroup(gpio.io->ChannelGroupIdPtr);
                break;
            case GPIO_CMD_WRITE_CHANNEL_GROUP:
                GPIO_DrvWriteChannelGroup(gpio.io->ChannelGroupIdPtr, gpio.io->PortLevel);
                break;
            case GPIO_CMD_FLIP_CHANNEL:
                gpio.io->ChannelLevel = GPIO_DrvFlipChannel(gpio.io->ChannelId);
                break;
            case GPIO_CMD_ENABLE_INTERRUPT:
                GPIO_DrvEnableInterrupt(gpio.io->ChannelId);
                break;
            case GPIO_CMD_DISABLE_INTERRUPT:
                GPIO_DrvDisableInterrupt(gpio.io->ChannelId);
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
/** @} */
