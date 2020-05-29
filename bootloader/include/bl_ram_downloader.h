/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_bl_ram RAM Downloader
    @ingroup grp_bl_downloader

    @addtogroup grp_bl_ram
    @{
    @file bl_ram_downloader.h
    @brief RAM Downloader APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_RAM_DOWNLOADER_H
#define BL_RAM_DOWNLOADER_H

/**
    @name RAM Downloader API IDs
    @{
    @brief API IDs for RAM Downloader
*/
#define BRCM_ARCH_BL_RAM_DWNLD_TARGET_SIDE_MACRO        (0x00U) /**< @brief #BL_RAM_DWNLD_BL_READY_MASK */
#define BRCM_ARCH_BL_RAM_DWNLD_HOST_SIDE_MACRO          (0x01U) /**< @brief #BL_RAM_DWNLD_FLASH_ID_MASK */
#define BRCM_ARCH_BL_RAM_DWNLD_STATUS_MACRO             (0x02U) /**< @brief #BL_RAM_DWNLD_STATUS_SUCCESS */
#define BRCM_ARCH_BL_RAM_DWNLD_CMDS_MACRO               (0x03U) /**< @brief #BL_RAM_DWNLD_CMD_FLASH */
/** @} */

/**
    @name Target side macros
    @{
    @brief Target side macros

    @trace #BRCM_ARCH_BL_RAM_DWNLD_TARGET_SIDE_MACRO #BRCM_REQ_BL_RAM_DWNLD_TARGET_SIDE
*/
#define BL_RAM_DWNLD_BL_READY_MASK             (0x0001U) /**< @brief Mask for BL ready bit */
#define BL_RAM_DWNLD_BL_READY_SHIFT            (0U)      /**< @brief Shift for BL ready bit */
#define BL_RAM_DWNLD_FLASHING_PERCENT_MASK     (0x00FEU) /**< @brief Mask for flashing percentage bits */
#define BL_RAM_DWNLD_FLASHING_PERCENT_SHIFT    (1U)      /**< @brief Shift for flashing percentage bits */
#define BL_RAM_DWNLD_STATUS_MASK               (0x0100U) /**< @brief Mask for status bit */
#define BL_RAM_DWNLD_STATUS_SHIFT              (8U)      /**< @brief Shift for status bit */
#define BL_RAM_DWNLD_TARGET_SEQ_NUM_MASK       (0x1E00U) /**< @brief Mask for target sequence number */
#define BL_RAM_DWNLD_TARGET_SEQ_NUM_SHIFT      (9U)      /**< @brief Shift for target sequence number */
/** @} */

/**
    @name Host side macros
    @{
    @brief Host side macros

    @trace #BRCM_ARCH_BL_RAM_DWNLD_HOST_SIDE_MACRO #BRCM_REQ_BL_RAM_DWNLD_HOST_SIDE
*/
#define BL_RAM_DWNLD_FLASH_ID_MASK             (0x0001U) /**< @brief Mask for Flash ID */
#define BL_RAM_DWNLD_FLASH_ID_SHIFT            (0U)      /**< @brief Shift for Flash ID */
#define BL_RAM_DWNLD_HOST_SEQ_NUM_MASK         (0x0F00U) /**< @brief Mask for host sequence number */
#define BL_RAM_DWNLD_HOST_SEQ_NUM_SHIFT        (8U)      /**< @brief Shift for host sequence number */
#define BL_RAM_DWNLD_CMD_MASK                  (0xF000U) /**< @brief Mask for command */
#define BL_RAM_DWNLD_CMD_SHIFT                 (12U)     /**< @brief Shift for command */
/** @} */

/**
    @name Download Status macros
    @{
    @brief Download Status macros

    @trace #BRCM_ARCH_BL_RAM_DWNLD_STATUS_MACRO #BRCM_REQ_BL_RAM_DWNLD_STATUS
*/
#define BL_RAM_DWNLD_STATUS_SUCCESS            (0U) /**< @brief Status Success */
#define BL_RAM_DWNLD_STATUS_FAILURE            (1U) /**< @brief Status Failure */
/** @} */

/**
    @name Download commands
    @{
    @brief Download commands

    @trace #BRCM_ARCH_BL_RAM_DWNLD_CMDS_MACRO #BRCM_REQ_BL_RAM_DWNLD_CMDS
*/
#define BL_RAM_DWNLD_CMD_FLASH                 (0U)  /**< @brief Flash command */
#define BL_RAM_DWNLD_CMD_EXECUTE               (1U)  /**< @brief Execute command */
#define BL_RAM_DWNLD_CMD_ERASE                 (2U)  /**< @brief Erase command */
#define BL_RAM_DWNLD_CMD_FLASH_WRITE           (3UL) /**< @brief Flash Write command */
#define BL_RAM_DWNLD_CMD_OTP_RD                (4UL) /**< @brief OTP read command */
#define BL_RAM_DWNLD_CMD_OTP_WR_SEC_KEY        (5UL) /**< @brief OTP write secure key command */
#define BL_RAM_DWNLD_CMD_OTP_ENABLE_SEC        (6UL) /**< @brief OTP Enable security command */
#define BL_RAM_DWNLD_CMD_OTP_WRITE_CUST        (7UL) /**< @brief Write to customer region command */
#define BL_RAM_DWNLD_CMD_OTP_WR_MAC_ADDR       (8UL) /**< @brief Write MAC address command */
#define BL_RAM_DWNLD_CMD_OTP_RD_MAC_ADDR       (9UL) /**< @brief Read MAC address command */
/** @} */

#endif /* BL_RAM_DOWNLOADER_H */

/** @} */
