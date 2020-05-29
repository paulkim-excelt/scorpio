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
    @defgroup grp_gpiodrv_impl GPIO Client Design
    @ingroup grp_gpiodrv

    @addtogroup grp_gpiodrv_impl
    @{

    @file gpio.c

    @version 0.1 Initial version
*/

#include <string.h>
#include <stddef.h>
#include <gpio.h>
#include <gpio_osil.h>
#include <bcm_err.h>
#include <chip_config.h>
#include <compiler.h>
#include <utils.h>

/**
    @name GPIO driver Design IDs
    @{
    @brief Design IDs for GPIO driver
*/
#define BRCM_SWDSGN_GPIO_READ_CHANNEL_PROC            (0x80U) /**< @brief #GPIO_ReadChannel */
#define BRCM_SWDSGN_GPIO_WRITE_CHANNEL_PROC           (0x81U) /**< @brief #GPIO_WriteChannel */
#define BRCM_SWDSGN_GPIO_READ_PORT_PROC               (0x82U) /**< @brief #GPIO_ReadPort */
#define BRCM_SWDSGN_GPIO_WRITE_PORT_PROC              (0x83U) /**< @brief #GPIO_WritePort */
#define BRCM_SWDSGN_GPIO_READ_CHANNEL_GROUP_PROC      (0x84U) /**< @brief #GPIO_ReadChannelGroup */
#define BRCM_SWDSGN_GPIO_WRITE_CHANNEL_GROUP_PROC     (0x85U) /**< @brief #GPIO_WriteChannelGroup */
#define BRCM_SWDSGN_GPIO_FLIP_CHANNEL_PROC            (0x86U) /**< @brief #GPIO_FlipChannel */
#define BRCM_SWDSGN_GPIO_ENABLE_INTERRUPT_PROC        (0x87U) /**< @brief #GPIO_EnableInterrupt */
#define BRCM_SWDSGN_GPIO_DISABLE_INTERRUPT_PROC       (0x88U) /**< @brief #GPIO_DisableInterrupt */
/** @} */


/** @brief

   Read value of an individual GPIO Channel

   @param[in]    aChannelId      ID of Channel to be read

   @return       Level of GPIO Channel

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_PROC
*/
GPIO_LevelType GPIO_ReadChannel(GPIO_ChannelType aChannelId)
{
    int32_t ret;
    GPIO_IOType gpioIO;
    GPIO_LevelType level = GPIO_LEVEL_LOW;

    gpioIO.ChannelId = aChannelId;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    ret = GPIO_SysCmdReq(GPIO_CMD_READ_CHANNEL, &gpioIO);
    if (BCM_ERR_OK == ret) {
        level = gpioIO.ChannelLevel;
    }

    return level;
}

/** @brief

   Write value of an individual GPIO Channel

   @param[in]    aChannelId      ID of Channel to write to
   @param[in]    aLevel          GPIO_LEVEL_LOW OR GPIO_HIGH
   @return       void

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_PROC
*/
void GPIO_WriteChannel(GPIO_ChannelType aChannelId, GPIO_LevelType aLevel)
{
    GPIO_IOType gpioIO;

    gpioIO.ChannelId = aChannelId;
    gpioIO.ChannelLevel = aLevel;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    (void)GPIO_SysCmdReq(GPIO_CMD_WRITE_CHANNEL, &gpioIO);
}

/** @brief Read value of a GPIO Port

   @param[in]    aChannelId          ID of Port to be read

   @return       Level of GPIO Port

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_READ_PORT_PROC
*/
GPIO_PortLevelType GPIO_ReadPort(GPIO_PortType aPortId)
{
    int32_t ret;
    GPIO_IOType gpioIO;
    GPIO_PortLevelType level = GPIO_LEVEL_LOW;

    gpioIO.PortId = aPortId;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    ret = GPIO_SysCmdReq(GPIO_CMD_READ_PORT, &gpioIO);
    if (BCM_ERR_OK == ret) {
        level = gpioIO.PortLevel;
    }

    return level;
}

/** @brief Write value of a GPIO Port

   @param[in]    aPortId         ID of Port to write to
   @param[in]    aLevel          Level of GPIO Port

   @return       void

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_WRITE_PORT_PROC
*/
void GPIO_WritePort(GPIO_PortType aPortId, GPIO_PortLevelType aLevel)
{
    GPIO_IOType gpioIO;

    gpioIO.PortId = aPortId;
    gpioIO.PortLevel = aLevel;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    (void)GPIO_SysCmdReq(GPIO_CMD_WRITE_PORT, &gpioIO);
}

/** @brief Read value of a GPIO Channel Group

   @param[in]    aChannelGroupIdPtr  Pointer to Channel Group Id

   @return       Level of GPIO Port Channel Group

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_READ_CHANNEL_GROUP_PROC
*/
GPIO_PortLevelType GPIO_ReadChannelGroup(const GPIO_ChannelGroupType *const aChannelGroupIdPtr)
{
    int32_t ret;
    GPIO_IOType gpioIO;
    GPIO_PortLevelType level = GPIO_LEVEL_LOW;

    if (NULL != aChannelGroupIdPtr) {
        gpioIO.ChannelGroupIdPtr = aChannelGroupIdPtr;
        gpioIO.retVal = BCM_ERR_UNKNOWN;
        ret = GPIO_SysCmdReq(GPIO_CMD_READ_CHANNEL_GROUP, &gpioIO);
        if (BCM_ERR_OK == ret) {
            level = gpioIO.PortLevel;
        }
    }

    return level;
}

/** @brief Write value of a GPIO Port

   @param[in]    aChannelGroupIdPtr   Pointer to Channel Group Id
   @param[in]    aLevel               Level of GPIO Port Channel Group

   @return       void

   @trace #BRCM_SWREQ_GPIO
   @trace #BRCM_SWARCH_GPIO_WRITE_CHANNEL_GROUP_PROC
*/
void GPIO_WriteChannelGroup(const GPIO_ChannelGroupType *const aChannelGroupIdPtr,
        GPIO_PortLevelType aLevel)
{
    GPIO_IOType gpioIO;

    if (NULL != aChannelGroupIdPtr) {
        gpioIO.ChannelGroupIdPtr = aChannelGroupIdPtr;
        gpioIO.PortLevel = aLevel;
        gpioIO.retVal = BCM_ERR_UNKNOWN;
        (void)GPIO_SysCmdReq(GPIO_CMD_WRITE_CHANNEL_GROUP, &gpioIO);
    }
}

/** @brief Flip value of an individual GPIO Channel

    @param[in]    aChannelId      ID of Channel to be read

    @return       Level of GPIO Channel
    @trace #BRCM_SWREQ_GPIO
    @trace #BRCM_SWARCH_GPIO_FLIP_CHANNEL_PROC
*/
GPIO_LevelType GPIO_FlipChannel(GPIO_ChannelType aChannelId)
{
    int32_t ret;
    GPIO_IOType gpioIO;
    GPIO_LevelType level = GPIO_LEVEL_LOW;

    gpioIO.ChannelId = aChannelId;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    ret = GPIO_SysCmdReq(GPIO_CMD_FLIP_CHANNEL, &gpioIO);
    if (BCM_ERR_OK == ret) {
        level = gpioIO.ChannelLevel;
    }

    return level;
}


/** @brief Enables interrupt for an individual GPIO Channel

    @param[in]    aChannelId      ID of Channel

    @return       result of operation.
    @trace #BRCM_SWREQ_GPIO
    @trace #BRCM_SWARCH_GPIO_ENABLE_INTERRUPT_PROC
*/
int32_t GPIO_EnableInterrupt(GPIO_ChannelType aChannelId)
{
    int32_t ret;
    GPIO_IOType gpioIO;

    gpioIO.ChannelId = aChannelId;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    ret = GPIO_SysCmdReq(GPIO_CMD_ENABLE_INTERRUPT, &gpioIO);

    return ret;
}

/** @brief Disables interrupt for an individual GPIO Channel

    @param[in]    aChannelId      ID of Channel

    @return       result of operation.
    @trace #BRCM_SWREQ_GPIO
    @trace #BRCM_SWARCH_GPIO_DISABLE_INTERRUPT_PROC
*/
int32_t GPIO_DisableInterrupt(GPIO_ChannelType aChannelId)
{
    int32_t ret;
    GPIO_IOType gpioIO;

    gpioIO.ChannelId = aChannelId;
    gpioIO.retVal = BCM_ERR_UNKNOWN;
    ret = GPIO_SysCmdReq(GPIO_CMD_DISABLE_INTERRUPT, &gpioIO);

    return ret;
}
/** @} */
