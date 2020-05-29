/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_bl_ptm PTM
    @ingroup grp_bootloader

    @addtogroup grp_bl_ptm
    @{
    @section sec_bl_ptm_fsm State Machine
    @image html bl_ptm_state_machine.jpg "PTM State Machine"
    - #BL_PTM_STATE_UNINIT: Uninitialized state. This is the state of PTM when not initialized.
    - #BL_PTM_STATE_IDLE: Valid PT is found and PTM is initialized.
    - #BL_PTM_STATE_ERROR: Error state. PTM enters into this state if no valid PT is found in the flash. In this state,
    PTM does not process any requests.

    @file bl_ptm.h
    @brief Interface for Partition Table Manager
    This header file defines interface for Partition Table Manager

    @version 0.30 Imported from docx
*/

#ifndef BL_PTM_H
#define BL_PTM_H

#include <bl_flash.h>

/**
    @name PTM API IDs
    @{
    @brief API IDs for PTM
 */
#define BRCM_ARCH_BL_PTM_ADDR_TYPE                  (0x00U) /**< @brief #BL_PTM_AddrType */
#define BRCM_ARCH_BL_PTM_SIZE_TYPE                  (0x01U) /**< @brief #BL_PTM_SizeType */
#define BRCM_ARCH_BL_PTM_IMG_ID_TYPE                (0x02U) /**< @brief #BL_PTM_ImgIDType */
#define BRCM_ARCH_BL_PTM_FLASH_ADDR_TYPE            (0x03U) /**< @brief #BL_PTM_FlashAddrType */
#define BRCM_ARCH_BL_PTM_FLASH_ID_TYPE              (0x04U) /**< @brief #BL_PTM_FlashIDType */
#define BRCM_ARCH_BL_PTM_LOOKUP_TBL_ENT_TYPE        (0x05U) /**< @brief #BL_PTM_LookupTblEntType */
#define BRCM_ARCH_BL_PTM_CFG_TYPE                   (0x06U) /**< @brief #BL_PTM_CfgType */
#define BRCM_ARCH_BL_PTM_LOAD_SYS_CFG_IMG_PROC      (0x07U) /**< @brief #BL_PTM_LoadSysCfgImg */
#define BRCM_ARCH_BL_PTM_LOAD_FIRMWARE_PROC         (0x08U) /**< @brief #BL_PTM_LoadFirmware */
#define BRCM_ARCH_BL_PTM_INIT_PROC                  (0x09U) /**< @brief #BL_PTM_Init */
#define BRCM_ARCH_BL_PTM_DEINIT_PROC                (0x0AU) /**< @brief #BL_PTM_DeInit */
#define BRCM_ARCH_BL_PTM_PROGRESS_CB_TYPE_PROC      (0x0BU) /**< @brief #BL_PTM_ProgressCbType */
#define BRCM_ARCH_BL_PTM_WRITE_IMG_PROC             (0x0CU) /**< @brief #BL_PTM_WriteImg */
#define BRCM_ARCH_BL_PTM_WRITE_PT_PROC              (0x0DU) /**< @brief #BL_PTM_WritePT */
#define BRCM_ARCH_BL_PTM_IS_VALID_PARTITION_PROC    (0x0EU) /**< @brief #BL_PTM_IsValidPartition */
/** @} */

/**
    @brief PTM Address Type

    @trace #BRCM_ARCH_BL_PTM_ADDR_TYPE #BRCM_REQ_PTM_ADDR
*/
typedef uint8_t *BL_PTM_AddrType;

/**
    @brief PTM size Type

    @trace #BRCM_ARCH_BL_PTM_SIZE_TYPE #BRCM_REQ_PTM_SIZE
*/
typedef uint32_t BL_PTM_SizeType;

/**
    @brief Partition table image ID types

    @trace #BRCM_ARCH_BL_PTM_IMG_ID_TYPE #BRCM_REQ_PTM_IMG_ID
*/
typedef uint16_t BL_PTM_ImgIDType;

/**
    @brief PTM Flash address type

    @trace #BRCM_ARCH_BL_PTM_FLASH_ADDR_TYPE #BRCM_REQ_PTM_FLASH_ADDR
*/
typedef BL_FLASH_AddrType BL_PTM_FlashAddrType;

/**
    @brief PTM Flash ID type

    @trace #BRCM_ARCH_BL_PTM_FLASH_ID_TYPE #BRCM_REQ_PTM_FLASH_ID
*/
typedef BL_FLASH_HwIDType BL_PTM_FlashIDType;

/**
    @brief Lookup table entry structure consisting of flash id and address of the flash (flashAddr) where Partition
    table shall be searched by PTM

    @trace #BRCM_ARCH_BL_PTM_LOOKUP_TBL_ENT_TYPE #BRCM_REQ_PTM_LOOKUP_TBL_ENT
 */
typedef struct {
    BL_PTM_FlashIDType flashID;        /**< @brief Flash device ID */
    BL_PTM_FlashAddrType flashAddr;    /**< @brief Address of the flash where PT can be found */
} BL_PTM_LookupTblEntType;

/**
    @brief PTM Configuration structure

    @trace #BRCM_ARCH_BL_PTM_CFG_TYPE #BRCM_REQ_PTM_CFG
 */
typedef struct {
    const BL_PTM_LookupTblEntType *const lookupTbl; /**< @brief Lookup table address (Partition table manager shall search
                                                     for a valid partition table in the flash using this table) */
    uint32_t lookupTblSize;                         /**< @brief No of entries in the lookup table */
    uint8_t *const scratchMemStartAddr;             /**< @brief Pointer to scratch memory start */
    uint8_t *const scratchMemEndAddr;               /**< @brief Pointer to scratch memory end */
} BL_PTM_CfgType;

/** @brief Load the BL Config image

    Request to load an image from flash

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aImgID      Image ID
    @param[in]      aAddr       Address where image shall be loaded.
    @param[inout]   aImgSize    Size of the buffer. Will be updated with the size of the actual image by
                                #PTM_LoadSysCfgImg

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully loaded the image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aAddr is NULL or aImgSize is NULL or aImgEntryPt is NULL
    @retval     #BL_BCM_ERR_UNINIT         PTM is not initialized
    @retval     #BL_BCM_ERR_INVAL_STATE    PTM is in error state
    @retval     #BL_BCM_ERR_NOT_FOUND      Image not found

    @post None

    @trace  #BRCM_ARCH_BL_PTM_LOAD_SYS_CFG_IMG_PROC  #BRCM_REQ_PTM_LOAD_SYS_CFG_IMG

    @limitations None
*/
extern int32_t BL_PTM_LoadSysCfgImg(BL_PTM_ImgIDType aImgID,
                                    BL_PTM_AddrType const aAddr,
                                    BL_PTM_SizeType *const aImgSize);

/** @brief Load Firmware

    Request to load firmware from flash

    @behavior Sync, Re-entrant

    @pre None

    @param[out]     aImgEntryPt     Entry point of the image loaded
    @param[out]     aCopyNum        copy number which is loaded

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully loaded firmware image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aCopyNum is NULL or aImgEntryPt is NULL
    @retval     #BL_BCM_ERR_UNINIT         PTM is not initialized
    @retval     #BL_BCM_ERR_INVAL_STATE    PTM is in error state
    @retval     #BL_BCM_ERR_NOT_FOUND      Image not found

    @post None

    @trace  #BRCM_ARCH_BL_PTM_LOAD_FIRMWARE_PROC  #BRCM_REQ_PTM_LOAD_FIRMWARE

    @limitations None
*/
extern int32_t BL_PTM_LoadFirmware(uint32_t *const aImgEntryPt,
                                uint32_t *aCopyNum);

/** @brief Load Secure key

    Request to load secure key from flash

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr           Buffer where key shall be loaded
    @param[inout]   aSize           input: maximum size of the input address buffer
                                    output: Actual lenght of the key loaded from the flash
    @param[out]     aCopyNum        copy number which is loaded

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully loaded firmware image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aCopyNum is NULL or aImgEntryPt is NULL
    @retval     #BL_BCM_ERR_UNINIT         PTM is not initialized
    @retval     #BL_BCM_ERR_INVAL_STATE    PTM is in error state
    @retval     #BL_BCM_ERR_NOT_FOUND      Image not found

    @post None

    @trace  #BRCM_ARCH_BL_PTM_LOAD_FIRMWARE_PROC  #BRCM_REQ_PTM_LOAD_FIRMWARE

    @limitations None
*/
extern int32_t BL_PTM_LoadSecKey(BL_PTM_AddrType const aAddr,
                              BL_PTM_SizeType *const aSize,
                              uint32_t *const aCopyNum);


/** @brief Validate a partition

    Request to validate a partition from the flash

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aPID        partition id
    @param[out]     aCopyNum    copy number which is valid

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully loaded firmware image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aCopyNum is NULL or aImgEntryPt is NULL
    @retval     #BL_BCM_ERR_UNINIT         PTM is not initialized
    @retval     #BL_BCM_ERR_INVAL_STATE    PTM is in error state
    @retval     #BL_BCM_ERR_NOT_FOUND      Image not found

    @post None

    @trace  #BRCM_ARCH_BL_PTM_LOAD_FIRMWARE_PROC  #BRCM_REQ_PTM_LOAD_FIRMWARE

    @limitations None
*/
extern int32_t BL_PTM_ValidatePartition(uint16_t aPID,
                                     uint32_t *const aCopyNum);
/** @brief PTM initialization

    Initialize Partition Table Manager

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aConfig         PTM Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             PTM initialized successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
                                        aConfig->lookupTbl is NULL
    @retval     #BCM_ERR_INVAL_STATE    Interface not called in PTM_STATE_UNINIT state
    @retval     #BCM_ERR_NOT_FOUND      No valid PT found

    @post None

    @trace  #BRCM_ARCH_BL_PTM_INIT_PROC  #BRCM_REQ_PTM_INIT

    @limitations None
*/
extern int32_t BL_PTM_Init(const BL_PTM_CfgType *const aConfig);

/** @brief PT Manager De-initialization

    This API de-initializes the Partition Table Manager

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_PTM_DEINIT_PROC  #BRCM_REQ_PTM_DEINIT

    @limitations None
*/
void BL_PTM_DeInit(void);

/** @brief PT Manager Get the valid flash ID

    This API returns the ID of the flash where a valid PT is found

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_PTM_DEINIT_PROC  #BRCM_REQ_PTM_DEINIT

    @limitations None
*/
extern int32_t BL_PTM_GetPTValidFlashID(BL_FLASH_HwIDType *const aFlashID);

/** @brief PTM Flash/Erase progress callback

    This is a callback to the client with the progress status (Percentage of process completed)

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aPercent    Percentage of process completed

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_PTM_PROGRESS_CB_TYPE_PROC  #BRCM_REQ_PTM_PROGRESS_CB

    @limitations None
*/
typedef void (*BL_PTM_ProgressCbType)(uint32_t aPercent);

/** @brief PTM write image

    This API searches the PT for the matching entry and writes/updates the image where its header points to, updates the
    header and if aCallback is not NULL, callback is given to the client with the percentage of process completed

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash ID
    @param[in]      aPID            Partition ID
    @param[in]      aImgID          Image ID
    @param[in]      aAddr           Address from where image has to read
    @param[in]      aImgSize        Size of the image
    @param[in]      aErrDetCode     EDC of the image
    @param[in]      aCallback       Callback function pointer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully written the image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aAddr is NULL
                                           aImgSize is 0
    @retval     #BL_BCM_ERR_NOT_FOUND      If Valid PT is not found or
                                           matching PT entry not found or
                                           matching image entry not found
    @retval     #BL_BCM_ERR_NOMEM          aImgSize is greater than the max size allocated for that image
    @retval     others                     errors from Flash APIs

    @post None

    @trace  #BRCM_ARCH_BL_PTM_WRITE_IMG_PROC  #BRCM_REQ_PTM_WRITE_IMG

    @limitations None
*/
extern int32_t BL_PTM_WriteImg(BL_PTM_FlashIDType aFlashID,
                               uint16_t aPID,
                               BL_PTM_ImgIDType aImgID,
                               const BL_PTM_AddrType const aAddr,
                               BL_PTM_SizeType aImgSize,
                               uint64_t aErrDetCode,
                               BL_PTM_ProgressCbType aCallback);

/** @brief PTM write PT

    This API writes the PT to the flash aFlashID. The given PT image layout is assumed as follows:
    PT hdr -- PT entries -- Image header1 -- Image entries -- Image header2 -- image entries and so on..with no gaps in
    between. If aCallback is not NULL, callback is given to the client with the percentage of process completed

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash ID
    @param[in]      aAddr           Address from where image has to read
    @param[in]      aImgSize        Size of the image
    @param[in]      aCallback       Callback function pointer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully written the PT
    @retval     others                  errors from Flash APIs

    @post None

    @trace  #BRCM_ARCH_BL_PTM_WRITE_PT_PROC  #BRCM_REQ_PTM_WRITE_PT

    @limitations None
*/
extern int32_t BL_PTM_WritePT(BL_PTM_FlashIDType aFlashID,
                              const BL_PTM_AddrType const aAddr,
                              BL_PTM_SizeType aImgSize,
                              uint64_t aErrDetCode,
                              BL_PTM_ProgressCbType aCallback);

extern int32_t BL_PTM_WriteImgHdr(BL_PTM_FlashIDType aFlashID,
                                  uint16_t aPID,
                                  const BL_PTM_AddrType const aAddr,
                                  BL_PTM_SizeType aImgHdrSz,
                                  BL_PTM_ProgressCbType aCallback);
/** @brief Check if PID is present in PT

    This API checks if PID is present in PT

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash ID
    @param[in]      aPID            16-bit PID

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             PID is present
    @retval     #BL_BCM_ERR_NOT_FOUND      PID is not present

    @post None

    @trace  #BRCM_ARCH_BL_PTM_IS_VALID_PARTITION_PROC  #BRCM_REQ_PTM_IS_VALID_PARTITION

    @limitations None
*/
extern int32_t BL_PTM_IsValidPartition(BL_PTM_FlashIDType aFlashID,
                                       uint16_t aPID);
#endif /* BL_PTM_H */

/** @} */
