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
    @addtogroup grp_bl_downloader
    @{
    @section sec_bl_downloader_overview Overview
    BL enters download mode when there is no valid FW image in the flash.  A soft reset will put BL into download mode
    forcibly even if there is a valid FW found in the flash. The reset mode decides the type of download mode BL has to
    enter.

    @file bl_downloader.h
    @brief Downloader APIs

    @version 0.30 Imported from docx
*/
#ifndef BL_DOWNLOADER_H
#define BL_DOWNLOADER_H

#include <bl_flash.h>
#if defined(BL_ENABLE_RAM_DWNLD_MODE)
#include <bl_ram_downloader.h>
#endif

#if defined(BL_ENABLE_IPC_DWNLD_MODE)
#include <bl_ipc_downloader.h>
#endif

/**
    @name Downloader API IDs
    @{
    @brief API IDs for Downloader
*/
#define BRCM_ARCH_BL_DWNLD_IMG_HDR_TYPE                 (0x00U) /**< @brief #BL_DWNLD_ImgHdrType */
#define BRCM_ARCH_BL_DWNLD_ITBL_HDR_SIZE_MACRO          (0x01U) /**< @brief #BL_DWNLD_ITBL_HDR_SIZE */
#define BRCM_ARCH_BL_DWNLD_TFTP_INIT_PROC               (0x02U) /**< @brief #BL_TFTP_DWNLD_Init */
#define BRCM_ARCH_BL_DWNLD_RAM_INIT_PROC                (0x03U) /**< @brief #BL_RAM_DWNLD_Init */
#define BRCM_ARCH_BL_DWNLD_RAM_RESET_DWNLD_INFO_PROC    (0x04U) /**< @brief #BL_RAM_DWNLD_ResetDwnldInfo */
#define BRCM_ARCH_BL_DWNLD_IPC_INIT_PROC                (0x05U) /**< @brief #BL_IPC_DWNLD_Init */
#define BRCM_ARCH_BL_DWNLD_IPC_RESET_DWNLD_INFO_PROC    (0x06U) /**< @brief #BL_IPC_DWNLD_ResetDwnldInfo */
/** @} */

#pragma pack(push, 4)
/**
    @brief Download image header

    @trace #BRCM_ARCH_BL_DWNLD_IMG_HDR_TYPE #BRCM_REQ_DWNLD_IMG_HDR
 */
typedef struct {
    uint32_t magicNum;      /**< @brief Magic number [IMGH] */
#define BL_DWNLD_TAR_MAGIC_NUM     (0x54415248UL)
    uint32_t imgSize;       /**< @brief Size of the image */
    uint32_t flags;         /**< @brief bits[1:0]: edc flag (0 for no edc / 1 for CRC / 2 for checksum)
                                  bit[2]: flash id (0 for internal flash or 1 for external flash)
                                  bits[10:3]: PID (Partition ID)
                                  bits[26:11]: Image ID (used for config image)
                                  bits[31:25]: reserved for future use */
#define BL_DWNLD_FLAG_EDC_MASK             (0x3UL)
#define BL_DWNLD_FLAG_EDC_NONE             (0x0UL)
#define BL_DWNLD_FLAG_EDC_CRC              (0x1UL)
#define BL_DWNLD_FLAG_EDC_CHECKSUM         (0x2UL)
#define BL_DWNLD_FLAG_EDC_SHIFT            (0UL)
#define BL_DWNLD_FLAG_FLASH_ID_MASK        (0x4UL)
#define BL_DWNLD_FLAG_FLASH_ID_SHIFT       (2UL)
#define BL_DWNLD_FLAG_PID_MASK             (0x7F8UL)
#define BL_DWNLD_FLAG_PID_SHIFT            (3UL)
#define BL_DWNLD_FLAG_IMG_ID_MASK          (0x7FFF800UL)
#define BL_DWNLD_FLAG_IMG_ID_SHIFT         (11UL)
#define BL_DWNLD_FLAG_COPY_ID_MASK         (0x18000000UL)
#define BL_DWNLD_FLAG_COPY_ID_SHIFT        (27UL)

#define BL_PT_IMG_PID                      (0xFFU)
#define BL_ALL_IN_ONE_IMG_ID               (0xFFFFU)
    uint64_t errDetCode;    /**< @brief Error detection code */
} BL_DWNLD_ImgHdrType;


/**
    @brief Download Image header structure size

    @trace #BRCM_ARCH_BL_DWNLD_ITBL_HDR_SIZE_MACRO #BRCM_REQ_BL_DWNLD_ITBL_HDR_SIZE
 */
#define BL_DWNLD_ITBL_HDR_SIZE         (sizeof(BL_DWNLD_ImgHdrType))
#pragma pack(pop)

#if defined(BL_ENABLE_TFTP_DWNLD_MODE)
/** @brief Initialize TFTP downloader

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aDwnldMode  Type of download mode

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_TFTP_INIT_PROC  #BRCM_REQ_BL_DWNLD_TFTP_INIT

    @limitations None
*/
extern void BL_TFTP_DWNLD_Init(MCU_DwnldModeType aDwnldMode);
#endif

#if defined(BL_ENABLE_RAM_DWNLD_MODE)
/** @brief Initialize RAM downloader

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aDwnldMode  Type of download mode

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_RAM_INIT_PROC  #BRCM_REQ_BL_DWNLD_RAM_INIT

    @limitations None
*/
extern void BL_RAM_DWNLD_Init(MCU_DwnldModeType aDwnldMode);

/** @brief Reset RAM download Info

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_RAM_RESET_DWNLD_INFO_PROC  #BRCM_REQ_BL_DWNLD_RESET_DWNLD_INFO

    @limitations None
*/
extern void BL_RAM_DWNLD_ResetDwnldInfo(void);
#endif

#if defined(BL_ENABLE_IPC_DWNLD_MODE)
/** @brief Initialize IPC downloader

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aDwnldMode  Type of download mode

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_IPC_INIT_PROC  #BRCM_REQ_BL_DWNLD_IPC_INIT

    @limitations None
*/
extern void BL_IPC_DWNLD_Init(MCU_DwnldModeType aDwnldMode);

/** @brief Reset IPC download Info

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_IPC_RESET_DWNLD_INFO_PROC  #BRCM_REQ_BL_DWNLD_RESET_DWNLD_INFO

    @limitations None
*/
extern void BL_IPC_DWNLD_ResetDwnldInfo(void);
#endif
#endif /* BL_DOWNLOADER_H */

/** @} */
