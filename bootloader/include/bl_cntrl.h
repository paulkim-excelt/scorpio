/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited. All rights reserved.

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
    @defgroup grp_bl_bcs Control Software
    @ingroup grp_bootloader

    @addtogroup grp_bl_bcs
    @{
    Bootloader enters BCS after bootstrapping. BCS executes firmware image if present in flash. If Firmware is not
    present in flash, BCS will wait for commands over Ethernet/IPC.

    @file bl_cntrl.h
    @brief Control Software APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_CNTRL_H
#define BL_CNTRL_H

#include <bl_ptm.h>
#include <bl_downloader.h>
#include <nvm_pt.h>
#include <imgl_ipc_cmds.h>
#include <sys_ipc_cmds.h>
#include <eth_xcvr.h>

/**
    @name Control Software API IDs
    @{
    @brief API IDs for Control Software
*/
#define BRCM_ARCH_BL_CNTRL_XCVR_INIT_PROC               (0x01U) /**< @brief #BL_CNTRL_XcvrInit */
#define BRCM_ARCH_BL_CNTRL_SWR_CONFIGURE_PROC           (0x02U) /**< @brief #BL_CNTRL_SWRConfigure */
#define BRCM_ARCH_BL_CNTRL_UPDATE_FLASH_PIECEWISE_PROC  (0x04U) /**< @brief #BL_CNTRL_UpdateFlashPiecewise */
#define BRCM_ARCH_BL_CNTRL_IS_SECURE_BOOT_PROC          (0x05U) /**< @brief #BL_CNTRL_isSecureBoot*/
#define BRCM_ARCH_BL_CNTRL_VALIDATE_KEY_PROC            (0x06U) /**< @brief #BCS_CNTRL_ValidateKey */
#define BRCM_ARCH_BL_CNTRL_GET_PUB_KEY_PROC             (0x07U) /**< @brief #BCS_CNTRL_GetPubKey */
/** @} */

/** @brief Initialize Transceiver

    This function initializes the transceiver and MAC with the corresponding configurations.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_XCVR_INIT_PROC  #BRCM_REQ_BL_BCS_XCVR_INIT

    @limitations None
*/
void BL_CNTRL_XcvrInit(const ETHXCVR_ConfigType *const aConfig);

/** @brief Configure Internal Regulators

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_SWR_CONFIGURE_PROC  #BRCM_REQ_BL_BCS_SWR_CONFIGURE

    @limitations None
*/
void BL_CNTRL_SWRConfigure(void);


/** @brief Interface to fetch a flashable image.

    The BL_CNTRL_UpdateFlashPiecewise API accepts a function that implements
    the interface defined here as an argument. Such a function would fetch the
    image contents correspnding to the aPwImgHdr argument passed to it.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aPwImgHdr     Header identifying the image to be fetched
    @param[inout]  aImg          Address where the image need to be loaded

    @return                     #BCM_ERR_OK on success. Any other BCM_ERR_* enum on failure

    @post None

    @trace  #BRCM_REQ_BL_BCS_UPDATE_FLASH

    @limitations None
*/
typedef int32_t (*BL_CNTRL_ImageFetchProcType)(const IMGL_ImgHdrType *const aPwImgHdr,
                                                uint8_t *aImg);

/** @brief Update flash piecewise

    This API's goal is the same as the BL_CNTRL_UpdateFlash API.
    But, unlike the said API which recieves all the images in a TAR
    image, this API fetches the images as and when it needs them.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  aFlashID            Flash ID
    @param[in]  aImageCount         Number of images to be written to flash
    @param[in]  aPwImgHdrs          Headers describing the images that are to be
                                    fetched and written to flash
    @param[in]  aImageFetchProc     Pointer to a function that fetches the images
                                    to be written to flash
    @param[out] aDwnldAddress       Scratch memory to download content (if applicable)
    @param[in]  aCallback           Callback for the flash progress

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Update successful
    @retval     #BL_BCM_ERR_INVAL_PARAMS    If the arguments are invalid

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_UPDATE_FLASH_PIECEWISE_PROC  #BRCM_REQ_BL_BCS_UPDATE_FLASH

    @limitations None
*/
extern int32_t BL_CNTRL_UpdateFlashPiecewise(const uint32_t aFlashID,
                                             const uint32_t aImageCount,
                                             const IMGL_ImgHdrType aPwImgHdrs[],
                                             BL_CNTRL_ImageFetchProcType aImageFetchProc,
                                             uint8_t* aDwnldAddress,
                                             BL_PTM_ProgressCbType aCallback);

/** @brief Returns if chip has secure boot enabled or not

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  void

    Return values are documented in reverse-chronological order
    @retval     #BL_TRUE             Chip has secure boot enabled
    @retval     #BL_FALSE            Chip does not have secure boot enabled


    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_IS_SECURE_BOOT_PROC  #BRCM_REQ_BL_BCS_SECURE_BOOT

    @limitations None
*/
extern uint32_t BL_CNTRL_isSecureBoot(void);

/** @brief Validate the secure key

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  void

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK       Secure is valid

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_VALIDATE_KEY_PROC  #BRCM_REQ_BL_BCS_SECURE_BOOT

    @limitations None
*/
extern int32_t BL_CNTRL_ValidatePubKey(void);

/** @brief Returns the public key

    @behavior Sync, Re-entrant

    @pre BL_CNTRL_ValidatePubKey() must be called

    @param[in]  void

    Return values are documented in reverse-chronological order
    @retval     pointer to public key

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_GET_PUB_KEY_PROC  #BRCM_REQ_BL_BCS_SECURE_BOOT

    @limitations None
*/
extern const uint8_t * BL_CNTRL_GetPubKey(void);

/** @brief Boot the slave devices
    Boot all the slave devices connected on the bus

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  void

    Return values are documented in reverse-chronological order
    @retval     None

    @post None

    @trace None

    @limitations None
*/
extern void BL_CNTRL_BootSlaves();

/** @brief Execute firmware in flashless mode

    @behavior Sync, Non-Reentrant

    @pre None

    @param[in]  aImageCount         Number of images to be written to flash
    @param[in]  aPwImgHdrs          Headers describing the images that are to be
                                    fetched
    @param[in]  aImageFetchProc     Pointer to a function that fetches the images
                                    to be executed/applied

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Update successful
    @retval     #BL_BCM_ERR_INVAL_PARAMS    If the arguments are invalid

    @post None

    @trace  #BRCM_ARCH_BL_CNTRL_UPDATE_FLASH_PIECEWISE_PROC  #BRCM_REQ_BL_BCS_UPDATE_FLASH

    @limitations None
*/
extern int32_t BL_CNTRL_ExecuteFW(SYS_UpdateExecCmdType *const aFlsInfo,
        BL_CNTRL_ImageFetchProcType aImageFetchProc);

#endif /* BCS_H */

/** @} */
