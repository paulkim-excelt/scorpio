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
    @defgroup grp_bl_ptm_impl Implementation
    @ingroup grp_bl_ptm

    @addtogroup grp_bl_ptm_impl
    @{
    @file bl_ptm.c
    @brief PTM Implementation
    This source file contains the implementation of Partition Table Manager

    @version 0.30 Imported from docx
*/

#include <stdint.h>
#include <string.h>
#include <bl_flash.h>
#include <bl_bcm_err.h>
#include <bl_ptm.h>
#include <nvm_pt.h>
#include <bl_utils.h>
#include <bl_chip_config.h>
#include <bl_edc.h>
#include <bl_log.h>
#include <bl_board.h>
#include <bl_cntrl.h>

#ifdef BL_ENABLE_SECURE_BOOT
#include <rsa.h>
#include <sha.h>
#endif

/**
    @name PTM API IDs
    @{
    @brief API IDs for PTM
*/
#define BRCM_DSGN_BL_PTM_STATE_TYPE                         (0x80U) /**< @brief #BL_PTM_StateType */
#define BRCM_DSGN_BL_PTM_IMG_TABLE_SIZE_MACRO               (0x81U) /**< @brief #BL_IMG_TABLE_SIZE */
#define BRCM_DSGN_BL_PTM_PT_TABLE_SIZE_MACRO                (0x82U) /**< @brief #BL_PT_TABLE_SIZE */
#define BRCM_DSGN_BL_PTM_COMP_TYPE                          (0x83U) /**< @brief #BL_PTM_CompType */
#define BRCM_DSGN_BL_PTM_CMDS_MACRO                         (0x84U) /**< @brief #FLASH_CMD_READ */
#define BRCM_DSGN_BL_PTM_FLASH_ERASE_PROC                   (0x85U) /**< @brief #BL_PTM_FlashErase */
#define BRCM_DSGN_BL_PTM_IS_CFG_IMG_PRESENT_PROC            (0x86U) /**< @brief #BL_PTM_IsCfgImgPresent */
#define BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_IMG_PROC         (0x87U) /**< @brief #BL_PTM_ReadAndValidateImg */
#define BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_IMG_ENTRIES_PROC (0x88U) /**< @brief #BL_PTM_ReadAndValidateImgEntries */
#define BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_IMG_HDR_PROC     (0x89U) /**< @brief #BL_PTM_ReadAndValidateImgHdr */
#define BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_PT_PROC          (0x8AU) /**< @brief #BL_PTM_ReadAndValidatePT */
#define BRCM_DSGN_BL_PTM_VALIDATE_SYS_CFG_PARTITION_PROC    (0x8BU) /**< @brief #BL_PTM_ValidateSysCfgPartition */
#define BRCM_DSGN_BL_PTM_UPDATE_IMG_PROC                    (0x8CU) /**< @brief #BL_PTM_UpdateImg */
#define BRCM_DSGN_BL_PTM_UPDATE_PT_PROC                     (0x8DU) /**< @brief #BL_PTM_UpdatePT */
/** @} */

#define BL_GetModuleLogLevel()     (BL_LOG_LVL_ERROR)

typedef ITBL_HdrType BL_PTM_ImgHdrType;
typedef ITBL_EntryType BL_PTM_ImgEntryType;
typedef PTBL_HdrType BL_PTM_PTHdrType;
typedef PTBL_EntryType BL_PTM_PTEntryType;

/**
    @name BL_PTM_StateType
    @{
    @brief PTM State type

    @trace #BRCM_DSGN_BL_PTM_STATE_TYPE #BRCM_REQ_BL_PTM_STATE
 */
typedef uint32_t BL_PTM_StateType;         /**< @brief typedef for PTM states */
#define BL_PTM_STATE_UNINIT        (0UL)   /**< @brief PTM status uninitialized */
#define BL_PTM_STATE_IDLE          (1UL)   /**< @brief PTM status idle */
#define BL_PTM_STATE_ERROR         (2UL)   /**< @brief PTM status error */
/** @} */

#define BL_READ_BUF_SIZE           (256UL)

/**
    @brief Img hdr + img entries (max size)

    @trace #BRCM_DSGN_BL_PTM_IMG_TABLE_SIZE_MACRO #BRCM_REQ_BL_PTM_BL_IMG_TABLE_SIZE
*/
#define BL_IMG_TABLE_SIZE          (4UL * 1024UL)

/**
    @brief Maximum size of the PT image

    @trace #BRCM_DSGN_BL_PTM_PT_TABLE_SIZE_MACRO #BRCM_REQ_BL_PTM_BL_PT_TABLE_SIZE
*/
#define BL_PT_TABLE_SIZE   (4UL * 1024UL)

/**
    @brief PTM component type

    @trace #BRCM_DSGN_BL_PTM_COMP_TYPE #BRCM_REQ_BL_BL_PTM_Comp
 */
typedef struct {
    BL_PTM_LookupTblEntType lookupTbl[BL_FLASH_MAX_HW_ID]; /**< @brief lookup table */
    uint32_t lookupTblSize;                          /**< @brief lookup table size */
    uint32_t lookupIdx;                              /**< @brief This is the index of lookupTbl where valid PT is found */
    uint32_t sysCfgCopyIdx;                              /**< @brief Copy ID of the valid system configuration partition */
    BL_PTM_StateType state;                             /**< @brief state of the PTM */
    uint8_t ptTbl[PTBL_SIZE];                          /**< @brief PT table (PT hdr + PT entries) */
    uint8_t imgTbl[BL_IMG_TABLE_SIZE];                  /**< @brief Image table ( Img hdr + img entries) */
    uint8_t * tempBuf;                              /**< @brief pointer to scratch buffer */
    uint32_t tempBufSize;                           /**< @brief Size of scratch buffer */
} BL_PTM_CompType;

static BL_PTM_CompType BL_PTM_Comp =
{
    .state = BL_PTM_STATE_UNINIT,
};

static const BL_PTM_PTHdrType *const BL_PTHdr =
    (const BL_PTM_PTHdrType *const)BL_PTM_Comp.ptTbl;
static BL_PTM_ImgHdrType *const BL_ImgHdr = (BL_PTM_ImgHdrType *const)BL_PTM_Comp.imgTbl;

static BL_COMP_NEVER_INLINE void BL_PTM_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_PTM_ID, aInstanceID, 0x0, aErr, 4UL, values);
}

static int32_t BL_PTM_GetFlashInfo(BL_PTM_FlashIDType aFlashID,
                                   uint32_t *const aPageSize,
                                   uint32_t *const aMinEraseSize,
                                   uint32_t *const aIsSubSectorSupported)
{
    uint32_t i;
    int32_t err = BL_BCM_ERR_NOT_FOUND;

    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (aFlashID == BL_FLASH_Config[i].hwID) {
            *aPageSize = BL_FLASH_Config[i].pageSize;
            if (BL_TRUE == BL_FLASH_Config[i].subSectorSupported) {
                *aIsSubSectorSupported = BL_TRUE;
                *aMinEraseSize = BL_FLASH_Config[i].subSectorSize;
            } else {
                *aIsSubSectorSupported = BL_FALSE;
                *aMinEraseSize = BL_FLASH_Config[i].sectorSize;
            }
            err = BL_BCM_ERR_OK;
            break;
        }
    }

    return err;
}

static int32_t BL_PTM_ValidateEDC(uint32_t aEDCFlag,
                               uint8_t *const aStartAddr,
                               uint32_t aSize,
                               uint64_t aErrDetCode)
{
    int32_t err;

    switch (aEDCFlag) {
    case PTBL_ERR_DET_NOCODE:
        err = BL_BCM_ERR_OK;
        break;
    case PTBL_ERR_DET_CRC:
        err = BL_EDC_ValidateCRC(aStartAddr, aSize, aErrDetCode);
        break;
    case PTBL_ERR_DET_CHECKSUM:
        err = BL_EDC_ValidateChcksm(aStartAddr, aSize, aErrDetCode);
        break;
    default:
        err = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }

    return err;
}

/** @brief Function to erase flash starting from aAddr for size aLen

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID                Flash ID
    @param[in]      aAddr                   Flash address from where it has to be erased
    @param[in]      aLen                    Size of the flash to be erased
    @param[in]      aIsSubSectorSupported   BL_TRUE - subsector erase supported, BL_FALSE - subsector erase not supported

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Flash erased successfully
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Given flash controller not present
    @retval     others                  Errors from BL_FLASH_EraseSector/BL_FLASH_EraseSubSector

    @post None

    @trace  #BRCM_DSGN_BL_PTM_FLASH_ERASE_PROC  #BRCM_ARCH_BL_PTM_FLASH_ERASE

    @limitations None

    @code{.c}
    for i = 0 to BL_FLASH_CfgSize
        if aFlashID = BL_FLASH_Config[i].hwID
            if BL_TRUE = BL_FLASH_Config[i].subSectorSupported
                BL_FLASH_EraseSubSector(aFlashID, aAddr, aLen)
            else
                BL_FLASH_EraseSector(aFlashID, aAddr, aLen)
            break
    if i = BL_FLASH_CfgSize
        return BL_BCM_ERR_INVAL_PARAMS
    else
        return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_PTM_FlashErase(BL_FLASH_HwIDType aFlashID,
                                 BL_FLASH_AddrType aAddr,
                                 BL_FLASH_LenType aLen,
                                 uint32_t aIsSubSectorSupported)
{
    int32_t err = BL_BCM_ERR_OK;

    if (BL_TRUE == aIsSubSectorSupported) {
        err = BL_FLASH_EraseSubSector(aFlashID, aAddr, aLen);
    } else {
        err = BL_FLASH_EraseSector(aFlashID, aAddr, aLen);
    }
    return err;
}

/** @brief Function to get the image entry corresponding to aImgID

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aImgID  Image ID

    Return values are documented in reverse-chronological order
    @retval     #BL_PTM_ImgEntryType       Image entry corresponding to aImgID
    @retval     NULL                    Corresponding image entry is not found

    @post None

    @trace  #BRCM_DSGN_BL_PTM_IS_CFG_IMG_PRESENT_PROC  #BRCM_ARCH_BL_PTM_IS_CFG_IMG_PRESENT

    @limitations None

    @code{.c}
    imgEntry = BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE
    BL_ImgHdr = BL_PTM_Comp.imgTbl

    for i = 0 to BL_ImgHdr->numImgs
        if imgEntry->imgType = aImgID
            break
        imgEntry++
    if i = BL_ImgHdr->numImgs
        imgEntry = NULL

    return imgEntry
    @endcode
*/
static BL_PTM_ImgEntryType *BL_PTM_IsCfgImgPresent(BL_PTM_ImgIDType aImgID)
{
    uint32_t i;
    BL_PTM_ImgEntryType *imgEntry =
        (BL_PTM_ImgEntryType *)(BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE);

    for (i = 0UL; i < BL_ImgHdr->numImgs; i++) {
        if (imgEntry->imgType == aImgID) {
            break;
        }
        imgEntry++;
    }
    if (i == BL_ImgHdr->numImgs) {
        imgEntry = NULL;
    }

    return imgEntry;
}

/** @brief This function updates the image where its header points to, updates the header and if aCallback is not NULL,
    callback is given to the client with the percentage of process completed

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash ID
    @param[in]      aPTEntry        PT entry corresponding to aImgID
    @param[in]      aImgID          Image ID
    @param[in]      aImgSize        Size of the image
    @param[in]      aAddr           Address from where image has to read
    @param[in]      aErrDetCode     EDC of the image
    @param[in]      aCallback       Callback function pointer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully updated the image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   - Given flash controller is not present
                                        - Max size of the image is not a multiple of the erasable size
    @retval     #BL_BCM_ERR_NOMEM          aImgSize is greater than the max size allocated for that image
    @retval     #BL_BCM_ERR_NOT_FOUND      Image is not found
    @retval     others                  errors from Flash APIs

    @post None

    @trace  #BRCM_DSGN_BL_PTM_UPDATE_IMG_PROC  #BRCM_ARCH_BL_PTM_UPDATE_IMG

    @limitations None

    @code{.c}
    BL_ImgHdr = BL_PTM_Comp.imgTbl

    for i = 0 to BL_FLASH_CfgSize
        if aFlashID = BL_FLASH_Config[i].hwID
            pageSize = BL_FLASH_Config[i].pageSize
            if BL_TRUE = BL_FLASH_Config[i].subSectorSupported
                eraseSize = BL_FLASH_Config[i].subSectorSize
            else
                eraseSize = BL_FLASH_Config[i].sectorSize
            break
    if (0UL = pageSize) or (0UL = eraseSize)
        return BL_BCM_ERR_INVAL_PARAMS

    pages = aImgSize / pageSize
    rem = aImgSize % pageSize
    imgEntry = BL_PTM_IsCfgImgPresent(aImgID)
    if NULL = imgEntry
        return BL_BCM_ERR_NOT_FOUND
    if imgEntry->maxSize < aImgSize
        return BL_BCM_ERR_NOMEM
    if (imgEntry->maxSize % eraseSize) != 0
        return BL_BCM_ERR_INVAL_PARAMS

    totalSize = aImgSize + BL_IMG_TABLE_SIZE + imgEntry->maxSize + BL_IMG_TABLE_SIZE
    flashAddr = aPTEntry->addr + imgEntry->flashOffset
    sectors = imgEntry->maxSize / eraseSize
    for i = 0 to sectors
        BL_PTM_FlashErase(aFlashID, flashAddr, eraseSize)
        flashAddr += eraseSize
        flashedSize += eraseSize
        percent = (flashedSize * 100) / totalSize
        if NULL != aCallback
            aCallback(percent)

    flashAddr = aPTEntry->addr + imgEntry->flashOffset
    for k = 0 to pages
        BL_FLASH_Write(aFlashID, flashAddr, aAddr, pageSize)
        aAddr += pageSize
        flashAddr += pageSize
        flashedSize += pageSize
        percent = (flashedSize * 100) / totalSize
        if NULL != aCallback
            aCallback(percent)
    if rem > 0
        BL_FLASH_Write(aFlashID, flashAddr, aAddr, rem)
        flashedSize += rem
        percent = (flashedSize * 100) / totalSize
        if NULL != aCallback
            aCallback(percent)

    imgEntry->errDetCode = aErrDetCode
    imgEntry->actualSize = aImgSize
    BL_ImgHdr->errDetCode = BL_EDC_CalculateCRC((BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE),
                (BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE))
    BL_PTM_FlashErase(aFlashID, aPTEntry->addr, eraseSize)
    BL_FLASH_Write(aFlashID, aPTEntry->addr, BL_PTM_Comp.imgTbl, BL_IMG_TABLE_SIZE)
    flashedSize += (2UL * BL_IMG_TABLE_SIZE)
    percent = (flashedSize * 100UL) / totalSize
    if NULL != aCallback
        aCallback(percent)

    return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_PTM_UpdateImg(BL_PTM_FlashIDType aFlashID,
                                const BL_PTM_PTEntryType *const aPTEntry,
                                BL_PTM_ImgIDType aImgID,
                                BL_PTM_SizeType aImgSize,
                                BL_PTM_AddrType aAddr,
                                uint64_t aErrDetCode,
                                BL_PTM_ProgressCbType aCallback)
{
    int32_t err;
    uint32_t pages = 0UL;
    uint32_t sectors = 0UL;
    uint32_t pageSize = 0UL;
    uint32_t eraseSize = 0UL;
    uint32_t totalSize;
    uint32_t flashedSize = 0UL;
    uint32_t rem = 0UL;
    uint32_t percent;
    uint32_t i, k;
    uint32_t crc, lineNo = 0UL;
    BL_PTM_ImgEntryType *imgEntry;
    BL_PTM_FlashAddrType flashAddr;
    uint32_t isSubSecSupport = BL_FALSE;
    uint8_t rdBuf[BL_READ_BUF_SIZE];
    BL_PTM_AddrType addr = aAddr;
    uint64_t errDetCode = aErrDetCode;

    err = BL_PTM_GetFlashInfo(aFlashID, &pageSize, &eraseSize, &isSubSecSupport);
    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        goto err;
    }

#if defined(__BCM89559_EVK__) || defined(__BCM89559G_EVK__) || defined(__BCM8956X__)
#if defined(BL_ENABLE_SECURE_BOOT)
    if (BL_FALSE == BL_CNTRL_isSecureBoot()) {
        if (PTBL_ID_BL == (aPTEntry->pid & PTBL_ID_TYPE_MASK)) {
            aImgSize -= RSA_S_MODULUS8;
            addr += RSA_S_MODULUS8;
        }
    }
#endif
#endif

    errDetCode = BL_EDC_CalculateCRC(addr, aImgSize,
            BL_EDC_CRC_INIT_SEED);

    pages = aImgSize / pageSize;
    rem = aImgSize % pageSize;
    imgEntry = BL_PTM_IsCfgImgPresent(aImgID);
    if (NULL == imgEntry) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_NOT_FOUND;
        goto err;
    }
    if (imgEntry->maxSize < aImgSize) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_NOMEM;
        goto err;
    }

    if (pageSize > sizeof(rdBuf)) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_NOMEM;
        goto err;
    }

    if ((imgEntry->maxSize % eraseSize) != 0UL) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    totalSize = aImgSize + BL_IMG_TABLE_SIZE + imgEntry->maxSize + BL_IMG_TABLE_SIZE;
    flashAddr = aPTEntry->addr + imgEntry->flashOffset;
    BL_LOG_ERR("Erasing sectors..\n");
    sectors = imgEntry->maxSize / eraseSize;
    for (i = 0UL; i < sectors; i++) {
        err = BL_PTM_FlashErase(aFlashID, flashAddr, eraseSize, isSubSecSupport);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            break;
        }
        flashAddr += eraseSize;
        flashedSize += eraseSize;
        percent = (flashedSize * 100UL) / totalSize;
        if (NULL != aCallback) {
            aCallback(percent);
        }
    }
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

    flashAddr = aPTEntry->addr + imgEntry->flashOffset;
    BL_LOG_ERR("Writing image@0x%x\n", flashAddr);
    crc = BL_EDC_CRC_INIT_SEED;
    for (k = 0UL; k < pages; k++) {
        err = BL_FLASH_Write(aFlashID, flashAddr, addr, pageSize);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            break;
        }

        /* Verify the data written in flash */
        err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, pageSize);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            break;
        }

        crc = BL_EDC_CalculateCRC(rdBuf, pageSize, crc);

        addr += pageSize;
        flashAddr += pageSize;
        flashedSize += pageSize;
        percent = (flashedSize * 100UL) / totalSize;
        if (NULL != aCallback) {
            aCallback(percent);
        }
    }
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

    if (rem > 0UL) {
        err = BL_FLASH_Write(aFlashID, flashAddr, addr, rem);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            goto err;
        }

        /* Verify the data written in flash */
        err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, rem);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            goto err;
        }

        crc = BL_EDC_CalculateCRC(rdBuf, rem, crc);

        flashedSize += rem;
        percent = (flashedSize * 100UL) / totalSize;
        if (NULL != aCallback) {
            aCallback(percent);
        }
    }

    if (crc != errDetCode) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_DATA_INTEG;
        goto err;
    }

    /* Update Img hdr and entry */
    imgEntry->errDetCode = errDetCode;
    imgEntry->actualSize = aImgSize;
    BL_ImgHdr->errDetCode = BL_EDC_CalculateCRC((BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE),
                (BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE),
                BL_EDC_CRC_INIT_SEED);
    BL_ImgHdr->hdrErrDetCode = BL_EDC_CalculateCRC(BL_PTM_Comp.imgTbl,
                                        ITBL_HDR_SIZE - sizeof(uint32_t),
                                        BL_EDC_CRC_INIT_SEED);
    err = BL_PTM_FlashErase(aFlashID, aPTEntry->addr, eraseSize, isSubSecSupport);
    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        goto err;
    }
    err = BL_FLASH_Write(aFlashID, aPTEntry->addr, BL_PTM_Comp.imgTbl, BL_IMG_TABLE_SIZE);
    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        goto err;
    }

    err = BL_FLASH_Read(aFlashID, aPTEntry->addr, rdBuf, ITBL_HDR_SIZE);
    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        goto err;
    }

    if (0L != BL_BCM_MemCmp(BL_PTM_Comp.imgTbl, rdBuf, ITBL_HDR_SIZE)) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_DATA_INTEG;
        goto err;
    }

    pages = ((BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE) / pageSize);
    rem = ((BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE) % pageSize);
    flashAddr = (aPTEntry->addr + ITBL_HDR_SIZE);

    crc = BL_EDC_CRC_INIT_SEED;
    for (k = 0UL; k < pages; k++) {
        err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, pageSize);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            break;
        }
        crc = BL_EDC_CalculateCRC(rdBuf, pageSize, crc);
        flashAddr += pageSize;
    }
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

    if (rem > 0UL) {
        err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, rem);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            goto err;
        }
        crc = BL_EDC_CalculateCRC(rdBuf, rem, crc);
    }

    if (crc != BL_ImgHdr->errDetCode) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_DATA_INTEG;
        goto err;
    }

    flashedSize += (2UL * BL_IMG_TABLE_SIZE);
    percent = (flashedSize * 100UL) / totalSize;
    if (NULL != aCallback) {
        aCallback(percent);
    }

err:
    if (BL_BCM_ERR_OK != err) {
        BL_PTM_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    }
    return err;
}

/** @brief Function to read the image and validate it

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aPTEntry        PT entry corresponding to aImgID
    @param[in]      aIsAppCfg       Flag to tell whether its an application cfg image
    @param[in]      aImgID          Image ID
    @param[inout]   aImgSize        Size of the image
    @param[out]     aAddr           Address where image shall be loaded.
    @param[in]      aImgEntry       Image entry

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully read and validated  image
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aImgSize is NULL
    @retval     #BL_BCM_ERR_NOMEM          Image size is greater than the size of the buffer provided
    @retval     #BL_BCM_ERR_DATA_INTEG     EDC validation failed
    @retval     #BL_BCM_ERR_NOT_FOUND      Image not found
    @retval     Others                  Errors from #BL_FLASH_Read

    @post None

    @trace  #BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_IMG_PROC  #BRCM_ARCH_BL_PTM_READ_AND_VALIDATE_IMG

    @limitations None

    @code{.c}
    if BL_TRUE = aIsAppCfg
        imgEntry = BL_PTM_IsCfgImgPresent(aImgID)
        if NULL != imgEntry
            if 0UL = imgEntry->actualSize
                return BL_BCM_ERR_NOT_FOUND
            if imgEntry->actualSize > *aImgSize
                return BL_BCM_ERR_NOMEM
        else
            return BL_BCM_ERR_NOT_FOUND
    else
        imgEntry = BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE

    flashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID
    addr = aPTEntry->addr + imgEntry->flashOffset
    if BL_TRUE = aIsAppCfg
        loadAddr = aAddr
        *aImgSize = imgEntry->actualSize
    else
        loadAddr = (uint8_t *)imgEntry->loadAddr
    BL_FLASH_Read(flashID, addr, loadAddr, imgEntry->actualSize)
    edcFlag = ((imgEntry->flags & ITBL_ERR_DET_ENTRY_MASK) >> ITBL_ERR_DET_ENTRY_SHIFT);
    if PTBL_ERR_DET_CRC = edcFlag
        err = BL_EDC_ValidateCRC(loadAddr, imgEntry->actualSize, imgEntry->errDetCode)
    else if PTBL_ERR_DET_CHECKSUM = edcFlag
        err = BL_EDC_ValidateChcksm(loadAddr, imgEntry->actualSize, imgEntry->errDetCode)
    else
        err = BL_BCM_ERR_OK;

    return err;
    @endcode
*/
static int32_t BL_PTM_ReadAndValidateImages(const BL_PTM_PTEntryType *const aPTEntry)
{
    BL_PTM_FlashAddrType flashAddr;
    uint32_t endIdx;
    uint32_t edcFlag;
    uint32_t tempSz;
    uint32_t readSz;
    int32_t i;
    uint8_t *loadAddr;
    uint32_t isExeImg;
    uint32_t offset;
    uint32_t imgSize = 0UL;
    uint32_t totalSize = 0UL;
    uint32_t crc = BL_EDC_CRC_INIT_SEED;
    BL_PTM_FlashIDType flashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID;
    BL_PTM_ImgEntryType *imgEntry =
        (BL_PTM_ImgEntryType *)(BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE);
    uint16_t pid = aPTEntry->pid & PTBL_ID_TYPE_MASK;
    uint32_t shaByteRemain = 0UL;
#if defined(BL_ENABLE_SECURE_BOOT)
    uint32_t isSecureChip = BL_FALSE;
    uint32_t bitsRemaining = 0UL;
    uint8_t *signature = NULL;
    RSA_Status status;
    uint8_t digest[SHA256_S_DIGEST8];
    const uint8_t *pubKey = NULL;
#endif
    int32_t err = BL_BCM_ERR_OK;

#if defined(BL_ENABLE_SECURE_BOOT)
    isSecureChip = BL_CNTRL_isSecureBoot();
    if ((BL_TRUE == isSecureChip) && (PTBL_ID_SECURE_KEY != pid)) {
        SHA_Init256((uint32_t *)digest);
        /* signature of the image is right after image entries */
        signature = ((uint8_t *)imgEntry) + (BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE);
        pubKey = BL_CNTRL_GetPubKey();
        if (NULL == pubKey) {
            err = BL_BCM_ERR_UNKNOWN;
            goto end;
        }
    }
#endif
    endIdx = BL_ImgHdr->numImgs;

    /**
     * Read each images in 4KB chunks in temp buffer
     * compute the CRC or digest (in case of secure boot)
     * Copy back the executable images to their respective
     * load addresses
     */
    for (i = 0UL; i < endIdx; i++) {

        if ((0UL == imgEntry[i].actualSize) && (0UL == imgEntry[i].maxSize)) {
            continue;
        }

        flashAddr = aPTEntry->addr + imgEntry[i].flashOffset;
        imgSize = imgEntry[i].actualSize;
        tempSz = imgSize;
        if ((PTBL_ID_FW == pid) &&
                (imgEntry[i].imgType <= ITBL_ID_EXE_MAX)) {
            loadAddr = (uint8_t *)imgEntry[i].loadAddr;
            isExeImg = BL_TRUE;
            /* Ensure that  load address is not overlapping with tempBuf */
            if (!((loadAddr > (BL_PTM_Comp.tempBuf + BL_PTM_Comp.tempBufSize))
                        || ((loadAddr + imgSize) < BL_PTM_Comp.tempBuf))) {
                err = BL_BCM_ERR_NOMEM;
                goto end;
            }
        } else {
            loadAddr = NULL;
            isExeImg = BL_FALSE;
        }

        if (imgSize == 0UL) {
            err = BL_BCM_ERR_DATA_INTEG;
            break;
        }

        offset = 0UL;
        crc = BL_EDC_CRC_INIT_SEED;
        totalSize += imgSize;
        /**
         * Read the image in 4KB chunks and compute the SHA
         * digest (in case of secure boot) or compute the
         * CRC (in case of non-secure boot)
         *
         * if SHA returns unsed bits, copy the un-used bits
         * at the start of the temp buffer and load the remaining
         * bits from the next image
         */
        while (tempSz > 0UL) {
            readSz = BL_MIN(tempSz, BL_PTM_Comp.tempBufSize - shaByteRemain);
            err = BL_FLASH_Read(flashID, flashAddr, BL_PTM_Comp.tempBuf + shaByteRemain, readSz);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            /* copy the image data from temp buffer to load address */
            if (BL_TRUE == isExeImg) {
                BL_BCM_MemCpy(loadAddr + offset, BL_PTM_Comp.tempBuf + shaByteRemain, readSz);
            }
#if defined(BL_ENABLE_SECURE_BOOT)
            if ((BL_TRUE == isSecureChip) && (PTBL_ID_SECURE_KEY != pid)) {
                bitsRemaining = SHA_Update256((uint32_t *)digest,
                        (uint8_t *const)BL_PTM_Comp.tempBuf, (readSz + shaByteRemain) * 8UL);
                uint32_t bytesUsed = (((readSz + shaByteRemain) * 8UL) - bitsRemaining) / 8UL;
                shaByteRemain = bitsRemaining / 8UL;
                /**
                 * copy the remaining bits from SHA digest to start of the temp
                 * buffer
                 */
                if (0UL < shaByteRemain) {
                    BL_BCM_MemMove(BL_PTM_Comp.tempBuf, &BL_PTM_Comp.tempBuf[bytesUsed], shaByteRemain);
                }
            } else {
#endif
                edcFlag = ((imgEntry[i].flags & ITBL_ERR_DET_ENTRY_MASK) >>
                        ITBL_ERR_DET_ENTRY_SHIFT);
                if (PTBL_ERR_DET_NOCODE == edcFlag) {
                    err = BL_BCM_ERR_OK;
                } else if (PTBL_ERR_DET_CRC == edcFlag) {
                    crc = BL_EDC_CalculateCRC(BL_PTM_Comp.tempBuf, readSz, crc);
                    if ((tempSz - readSz) == 0UL) {
                        if (crc != imgEntry[i].errDetCode) {
                            err = BL_BCM_ERR_DATA_INTEG;
                            break;
                        }
                    }
                } else {
                    err = BL_BCM_ERR_NOSUPPORT;
                }
#if defined(BL_ENABLE_SECURE_BOOT)
            }
#endif
            tempSz -= readSz;
            flashAddr += readSz;
            offset += readSz;
        }
        if (BL_BCM_ERR_OK != err) {
            break;
        }
    }
    if (BL_BCM_ERR_OK != err) {
        goto end;
    }
#if defined(BL_ENABLE_SECURE_BOOT)
    if ((BL_TRUE == isSecureChip) && (PTBL_ID_SECURE_KEY != pid)) {
        SHA_Final256((uint32_t *)digest, BL_PTM_Comp.tempBuf, totalSize * 8UL, bitsRemaining);
        status = Sec_verifyDigest((uint32_t const *)signature, RSA_S_MODULUS8,
                (uint32_t const*)pubKey, RSA_S_MODULUS8, digest,
                sizeof(digest));
        if (RSA_SUCCESS != status) {
            BL_LOG_ERR("RSA verification failed for pid: %d\n", pid);
            err = BL_BCM_ERR_DATA_INTEG;
        }
    }
#endif

end:
    return err;
}

/** @brief Function to read the image header and entries and validate it

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash Controller ID
    @param[in]      aPTEntry        PT entry whose image header and entries are to be read and validated

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully read and validated img header and entries
    @retVal     #BL_BCM_ERR_CANCELLED      PT header skip flag set
    @retVal     #BL_BCM_ERR_INVAL_MAGIC    Header magic not valid
    @retval     #BL_BCM_ERR_DATA_INTEG     number of images in the Partition is 0
    @retval     #BL_BCM_ERR_NOMEM          Mismatch in the size
    @retval     #BL_BCM_ERR_DATA_INTEG     EDC validation failed
    @retval     Others                     Errors from #BL_FLASH_Read

    @post None

    @trace  #BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_IMG_ENTRIES_PROC  #BRCM_ARCH_BL_PTM_READ_AND_VALIDATE_IMG_ENTRIES

    @limitations None

    @code{.c}
    if pt entry skip flag is set
         return BL_BCM_ERR_CANCELLED
     if BL_ImgHdr->magicNumber != ITBL_MAGIC
        return BL_BCM_ERR_INVAL_MAGIC
    if BL_ImgHdr->numImgs == 0UL
        return BL_BCM_ERR_DATA_INTEG
    size = BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE
    if size > (BL_IMG_TABLE_SIZE - ITBL_HDR_SIZE)
        return BL_BCM_ERR_NOMEM
    BL_FLASH_Read(aFlashID, (aPTEntry->addr + ITBL_HDR_SIZE), &BL_PTM_Comp.imgTbl[ITBL_HDR_SIZE], size)
    edcFlag = ((BL_ImgHdr->flags & ITBL_ERR_DET_HDR_MASK) >> ITBL_ERR_DET_HDR_SHIFT)
    if PTBL_ERR_DET_CRC = edcFlag
        err = BL_EDC_ValidateCRC((BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE), size, BL_ImgHdr->errDetCode)
    else if PTBL_ERR_DET_CHECKSUM == edcFlag
        err = BL_EDC_ValidateChcksm((BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE), size, BL_ImgHdr->errDetCode)
    else
        err = BL_BCM_ERR_OK
    return err
    @endcode
*/
static int32_t BL_PTM_ReadAndValidateImgTbl(BL_PTM_FlashIDType aFlashID,
                                         const BL_PTM_PTEntryType *const aPTEntry)
{
    int32_t err;
    uint32_t size;
    uint32_t edcFlag;

    if ((aPTEntry->flags & PTBL_ID_SKIP_MASK) ==
            PTBL_ID_SKIP_MASK) {
        err = BL_BCM_ERR_CANCELLED;
        goto err;
    }
    /* Read Img Hdr and entries */
    err = BL_FLASH_Read(aFlashID, aPTEntry->addr, BL_PTM_Comp.imgTbl, BL_IMG_TABLE_SIZE);
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }
    /* Validate Img Hdr */
    if (ITBL_MAGIC != BL_ImgHdr->magicNumber) {
        err = BL_BCM_ERR_INVAL_MAGIC;
        goto err;
    }
    /* we don't expect any partition with 0 entries */
    if (BL_ImgHdr->numImgs == 0UL) {
        err = BL_BCM_ERR_DATA_INTEG;
        goto err;
    }
    size = (BL_ImgHdr->numImgs * ITBL_ENTRY_SIZE);
    if (size > (BL_IMG_TABLE_SIZE - ITBL_HDR_SIZE)) {
            err = BL_BCM_ERR_NOMEM;
        goto err;
    }
    err = BL_PTM_ValidateEDC(PTBL_ERR_DET_CRC, BL_PTM_Comp.imgTbl,
            ITBL_HDR_SIZE - sizeof(uint32_t), BL_ImgHdr->hdrErrDetCode);
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }
    /* Validate Img Entries */
    edcFlag = ((BL_ImgHdr->flags & ITBL_ERR_DET_HDR_MASK) >>
            ITBL_ERR_DET_HDR_SHIFT);
    err = BL_PTM_ValidateEDC(edcFlag, &BL_PTM_Comp.imgTbl[ITBL_HDR_SIZE], size,
            BL_ImgHdr->errDetCode);

err:
    return err;
}

/** @brief Function to read the PT and validate it

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      flashID         Flash Controller ID

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully read and validated PT
    @retval     #BL_BCM_ERR_NOT_FOUND      Valid PT not found
    @retval     Others                  Errors from #BL_FLASH_Read

    @post None

    @trace  #BRCM_DSGN_BL_PTM_READ_AND_VALIDATE_PT_PROC  #BRCM_ARCH_BL_PTM_READ_AND_VALIDATE_PT

    @limitations None

    @code{.c}
    flashAddr = BL_PTM_Comp.lookupTbl[aFlashID].flashAddr
    BL_PTHdr = BL_PTM_Comp.ptTbl

    for i = 0 to PTBL_NUM_COPIES
        BL_FLASH_Read(aFlashID, flashAddr, BL_PTM_Comp.ptTbl, PTBL_HDR_SIZE)
        if PTBL_MAGIC = BL_PTHdr->magicNumber
            size = BL_PTHdr->numEntries * PTBL_ENTRY_SIZE
            if size = (BL_PTHdr->totalSize - PTBL_HDR_SIZE)
                BL_FLASH_Read(aFlashID, flashAddr, &BL_PTM_Comp.ptTbl[PTBL_HDR_SIZE], size);
                edcFlag = ((BL_PTHdr->flags & PTBL_ERR_DET_MASK) >> PTBL_ERR_DET_SHIFT)
                if PTBL_ERR_DET_CRC = edcFlag
                    err = BL_EDC_ValidateCRC(&BL_PTM_Comp.ptTbl[PTBL_HDR_SIZE], size, BL_PTHdr->errDetCode)
                else if PTBL_ERR_DET_CHECKSUM = edcFlag
                    err = BL_EDC_ValidateChcksm(&BL_PTM_Comp.ptTbl[PTBL_HDR_SIZE], size, BL_PTHdr->errDetCode)
                else
                    err = BL_BCM_ERR_OK
                if BL_BCM_ERR_OK = err
                    break
        flashAddr += PTBL_SIZE

    if PTBL_NUM_COPIES = i
        return BL_BCM_ERR_NOT_FOUND;
    else
        return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_PTM_ReadAndValidatePT(BL_PTM_FlashIDType aFlashID,
                                     uint32_t aFlashAddr)
{
    int32_t err;
    uint32_t size;
    uint32_t edcFlag;

    /* Read PT Hdr and Entries */
    err = BL_FLASH_Read(aFlashID, aFlashAddr, BL_PTM_Comp.ptTbl, PTBL_SIZE);
    if (BL_BCM_ERR_OK != err) {
        goto end;
    }
    /* Validate PT Hdr */
    if (PTBL_MAGIC != BL_PTHdr->magicNumber) {
        err = BL_BCM_ERR_INVAL_MAGIC;
        goto end;
    }
    size = BL_PTHdr->numEntries * PTBL_ENTRY_SIZE;
    if (size != (BL_PTHdr->totalSize - PTBL_HDR_SIZE)) {
        err = BL_BCM_ERR_NOMEM;
        goto end;
    }
    err = BL_PTM_ValidateEDC(PTBL_ERR_DET_CRC, BL_PTM_Comp.ptTbl,
            PTBL_HDR_SIZE - sizeof(uint32_t), BL_PTHdr->hdrErrDetCode);
    if (BL_BCM_ERR_OK != err) {
        goto end;
    }

    /* validate PT entries */
    edcFlag = ((BL_PTHdr->flags & PTBL_ERR_DET_MASK) >>
            PTBL_ERR_DET_SHIFT);
    err = BL_PTM_ValidateEDC(edcFlag, &BL_PTM_Comp.ptTbl[PTBL_HDR_SIZE], size,
            BL_PTHdr->errDetCode);

end:
    return err;
}

int32_t BL_PTM_LoadSysCfgImg(BL_PTM_ImgIDType aImgID,
                             BL_PTM_AddrType const aAddr,
                             BL_PTM_SizeType *const aImgSize)
{
    int32_t err = BL_BCM_ERR_UNKNOWN;
    uint32_t i, j;
    uint32_t flashAddr;
    uint32_t imgSize;
    uint16_t pid = (BL_PTM_Comp.sysCfgCopyIdx << 8UL) | PTBL_ID_SYSCFG;
    BL_PTM_FlashIDType flashID;
    BL_PTM_ImgEntryType *imgEntry;
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);

    if (BL_PTM_STATE_UNINIT == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_UNINIT;
        goto err;
    }
    if (BL_PTM_STATE_ERROR == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_INVAL_STATE;
        goto err;
    }
    if (NULL == aImgSize) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    flashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID;
    for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
        if (pid != ptEntry[i].pid) {
            err = BL_BCM_ERR_NOT_FOUND;
            continue;
        }
        err = BL_PTM_ReadAndValidateImgTbl(flashID, &ptEntry[i]);
        if (BL_BCM_ERR_OK != err) {
            continue;
        }
        imgEntry = (BL_PTM_ImgEntryType *)(BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE);
        for (j = 0UL; j < BL_ImgHdr->numImgs; j++) {
            if (aImgID != imgEntry[j].imgType) {
                err = BL_BCM_ERR_NOT_FOUND;
                continue;
            }
            flashAddr = ptEntry[i].addr + imgEntry[j].flashOffset;
            imgSize = imgEntry[j].actualSize;
            if (*aImgSize < imgSize) {
                err = BL_BCM_ERR_NOMEM;
                continue;
            }
            err = BL_FLASH_Read(flashID, flashAddr, aAddr, imgSize);
            if (BL_BCM_ERR_OK == err) {
                *aImgSize = imgSize;
                break;
            }
        }
        if (BL_BCM_ERR_OK == err) {
            break;
        }
    }

err:
    return err;
}

int32_t BL_PTM_ValidatePartition(uint16_t aPID, uint32_t *const aCopyNum)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t i;
    BL_PTM_FlashIDType flashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID;
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);

    if (BL_PTM_STATE_UNINIT == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_UNINIT;
        goto err;
    }
    if (BL_PTM_STATE_ERROR == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_INVAL_STATE;
        goto err;
    }

    for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
        if (aPID != (ptEntry[i].pid & PTBL_ID_TYPE_MASK)) {
            continue;
        }
        err = BL_PTM_ReadAndValidateImgTbl(flashID, &ptEntry[i]);
        if (BL_BCM_ERR_OK != err) {
            continue;
        }
        err = BL_PTM_ReadAndValidateImages(&ptEntry[i]);
        if (BL_BCM_ERR_OK == err) {
            break;
        }
    }
    if (i == BL_PTHdr->numEntries) {
        err = BL_BCM_ERR_NOT_FOUND;
    } else {
        if (PTBL_ID_SYSCFG == aPID) {
            BL_PTM_Comp.sysCfgCopyIdx = ((ptEntry[i].pid & PTBL_ID_COPY_MASK)
                    >> PTBL_ID_COPY_SHIFT);
        }
        if (NULL != aCopyNum) {
            *aCopyNum = ((ptEntry[i].pid & PTBL_ID_COPY_MASK)
                    >> PTBL_ID_COPY_SHIFT);
        }
    }
err:
    return err;
}


int32_t BL_PTM_LoadFirmware(uint32_t *const aImgEntryPt, uint32_t *const aCopyNum)
{
    int32_t err;

    if ((NULL == aImgEntryPt) || (NULL == aCopyNum)) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    err = BL_PTM_ValidatePartition(PTBL_ID_FW, aCopyNum);
    if (BL_BCM_ERR_OK == err) {
        if (NULL != aImgEntryPt) {
            *aImgEntryPt = BL_ImgHdr->entryPoint;
        }
    }
err:
    return err;
}

int32_t BL_PTM_LoadSecKey(BL_PTM_AddrType const aAddr, BL_PTM_SizeType *const aSize,
                       uint32_t *const aCopyNum)
{
    uint32_t i;
    uint32_t flashAddr;
    uint32_t imgSize;
    BL_PTM_FlashIDType flashID;
    BL_PTM_ImgEntryType *imgEntry;
    uint16_t pid = PTBL_ID_SECURE_KEY;
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);
    int32_t err = BL_BCM_ERR_OK;

    if ((NULL != aAddr) && (NULL != aSize)) {
        err = BL_PTM_ValidatePartition(PTBL_ID_SECURE_KEY, aCopyNum);
        if (BL_BCM_ERR_OK == err) {
            pid = pid | (*aCopyNum << 8U);
            for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
                if (pid == ptEntry[i].pid) {
                    flashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID;
                    imgEntry = (BL_PTM_ImgEntryType *)(BL_PTM_Comp.imgTbl + ITBL_HDR_SIZE);
                    flashAddr = ptEntry[i].addr + imgEntry[0].flashOffset;
                    imgSize = imgEntry[0].actualSize;
                    if (imgSize <= *aSize) {
                        err = BL_FLASH_Read(flashID, flashAddr, aAddr, imgSize);
                        if (BL_BCM_ERR_OK == err) {
                            *aSize = imgSize;
                            break;
                        }
                    } else {
                        err = BL_BCM_ERR_NOMEM;
                        break;
                    }
                }
            }
            if (i == BL_PTHdr->numEntries) {
                err = BL_BCM_ERR_NOT_FOUND;
            }
        }
    } else {
        err = BL_BCM_ERR_INVAL_PARAMS;
    }
    return err;
}



/** @brief Function to update PT

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID        Flash Controller ID
    @param[in]      aAddr           Address from where PT has to read
    @param[in]      aPTEntry        PT Entry corresponding to PT image

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successfully updated PT
    @retval     #BL_BCM_ERR_NOT_FOUND      Matching PT entry not found
    @retval     Others                  errors from Flash APIs

    @post None

    @trace  #BRCM_DSGN_BL_PTM_UPDATE_PT_PROC  #BRCM_ARCH_BL_PTM_UPDATE_PT

    @limitations None

    @code{.c}
    ptHdr = aAddr
    ptEntry = aAddr + PTBL_HDR_SIZE
    addr = aAddr + ptHdr->totalSize
    flashAddr = aPTEntry->addr

    for i = 0 to ptHdr->numEntries
        imgHdr = addr
        size = ITBL_HDR_SIZE + (imgHdr->numImgs * ITBL_ENTRY_SIZE)
        if memcmp(aPTEntry, ptEntry, sizeof(BL_PTM_PTEntryType) = 0UL)
            break
        addr += size
        ptEntry++
    if i = ptHdr->numEntries
        return BL_BCM_ERR_NOT_FOUND

    if BL_TRUE = BL_FLASH_Config[aFlashID].subSectorSupported
        eraseSize = BL_FLASH_Config[aFlashID].subSectorSize
    else
        eraseSize = BL_FLASH_Config[aFlashID].sectorSize

    BL_PTM_FlashErase(aFlashID, flashAddr, eraseSize)
    BL_FLASH_Write(aFlashID, flashAddr, addr, size)

    return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_PTM_UpdatePT(BL_PTM_FlashIDType aFlashID,
                               const BL_PTM_AddrType const aAddr,
                               const BL_PTM_PTEntryType *const aPTEntry)
{
    BL_PTM_PTHdrType *ptHdr;
    BL_PTM_PTEntryType *ptEntry;
    BL_PTM_ImgHdrType *imgHdr;
    uint32_t flashAddr;
    uint32_t eraseSize = 0UL;
    uint32_t size = 0UL;
    uint32_t i;
    uint8_t *addr;
    int32_t err = BL_BCM_ERR_OK;
    uint32_t pages = 0UL;
    uint32_t pageSize = 0UL;
    uint32_t rem = 0UL;
    uint32_t isSubSecSupport = BL_FALSE;
    uint8_t rdBuf[BL_READ_BUF_SIZE];
    ptHdr = (BL_PTM_PTHdrType *)aAddr;
    ptEntry = (BL_PTM_PTEntryType *)(aAddr + PTBL_HDR_SIZE);

    addr = aAddr + ptHdr->totalSize;
    flashAddr = aPTEntry->addr;

    /* find the correspoding pt entry from backup copy */
    for (i = 0UL; i < ptHdr->numEntries; i++) {
        imgHdr = (BL_PTM_ImgHdrType *)addr;
        size = ITBL_HDR_SIZE + (imgHdr->numImgs * ITBL_ENTRY_SIZE);
        if (BL_BCM_MemCmp(aPTEntry, ptEntry, sizeof(BL_PTM_PTEntryType)) == 0L) {
            break;
        }
        addr += size;
        ptEntry++;
    }
    if (i == ptHdr->numEntries) {
        err = BL_BCM_ERR_NOT_FOUND;
        goto exit;
    }

    err = BL_PTM_GetFlashInfo(aFlashID, &pageSize, &eraseSize, &isSubSecSupport);
    if (BL_BCM_ERR_OK != err) {
        goto exit;
    }

    /* erase the flash sector/subsector */
    err = BL_PTM_FlashErase(aFlashID, flashAddr, eraseSize, isSubSecSupport);
    if (BL_BCM_ERR_OK == err) {
        /* Flash Img Hdr and Img entries */
        err = BL_FLASH_Write(aFlashID, flashAddr, addr, size);
        if (BL_BCM_ERR_OK != err) {
            goto exit;
        }

        pages = (size / pageSize);
        rem = (size % pageSize);

        for (i = 0UL; i < pages; i++) {
            err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, pageSize);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            if (0L != BL_BCM_MemCmp(&addr[i * pageSize], rdBuf, pageSize)) {
                err = BL_BCM_ERR_DATA_INTEG;
                break;
            }
            flashAddr += pageSize;
        }
        if (BL_BCM_ERR_OK != err) {
            goto exit;
        }
        if (rem > 0UL) {
            err = BL_FLASH_Read(aFlashID, flashAddr, rdBuf, rem);
            if (BL_BCM_ERR_OK != err) {
                goto exit;
            }
            if (0L != BL_BCM_MemCmp(&addr[i * pageSize], rdBuf, rem)) {
                err = BL_BCM_ERR_DATA_INTEG;
                goto exit;
            }
        }
    }

exit:
    return err;
}

/**
    @image html bl_ptm_flowchart_write_img.jpg
*/
int32_t BL_PTM_WriteImg(BL_PTM_FlashIDType aFlashID,
                        uint16_t aPID,
                        BL_PTM_ImgIDType aImgID,
                        const BL_PTM_AddrType const aAddr,
                        BL_PTM_SizeType aImgSize,
                        uint64_t aErrDetCode,
                        BL_PTM_ProgressCbType aCallback)
{
    uint32_t i;
    int32_t err = BL_BCM_ERR_NOT_FOUND;
    uint16_t ptmFactCopyImgID = 0xFFFFU;
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);

    if ((NULL == aAddr) || (0UL == aImgSize)) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (BL_PTM_STATE_UNINIT == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_UNINIT;
        goto err;
    }
    if (BL_PTM_STATE_ERROR == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_INVAL_STATE;
        goto err;
    }
    /* update all the copies of the images except the factory copy */
    for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
        if (ptEntry->pid == aPID) {
            /**
             * Read and validate the image header and entries
             * if its corrupted, we have to use the factory copy
             * of PT and update the img hdr and entries
             */
            err = BL_PTM_ReadAndValidateImgTbl(aFlashID, ptEntry);
            if (BL_BCM_ERR_OK != err) {
                uint32_t size;
                size = BL_PTM_Comp.tempBufSize;
                /* load the factory copy of the PTM */
                err = BL_PTM_LoadSysCfgImg(ptmFactCopyImgID, BL_PTM_Comp.tempBuf,&size);
                if (BL_BCM_ERR_OK != err) {
                    break;
                }
                err = BL_PTM_UpdatePT(aFlashID, (BL_PTM_AddrType)BL_PTM_Comp.tempBuf,
                        ptEntry);
                if (BL_BCM_ERR_OK != err) {
                    break;
                }
            }
            /* Take a copy of image hdr and entries */
            err = BL_FLASH_Read(aFlashID, ptEntry->addr, BL_PTM_Comp.imgTbl,
                    BL_IMG_TABLE_SIZE);
            if (BL_BCM_ERR_OK != err) {
                break;
            }

            err = BL_PTM_UpdateImg(aFlashID, ptEntry, aImgID, aImgSize,
                        aAddr, aErrDetCode, aCallback);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
        }
        ptEntry++;
    }

    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

err:
    return err;
}

int32_t BL_PTM_WriteImgHdr(BL_PTM_FlashIDType aFlashID,
                           uint16_t aPID,
                           const BL_PTM_AddrType const aAddr,
                           BL_PTM_SizeType aSize,
                           BL_PTM_ProgressCbType aCallback)
{
    uint32_t i = BL_PTHdr->numEntries;
    int32_t err = BL_BCM_ERR_OK;

    /* find the entry in PT */
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);

    if (BL_PTM_STATE_IDLE == BL_PTM_Comp.state) {
        for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
            if (ptEntry[i].pid == aPID) {
                break;
            }
        }
        if (i == BL_PTHdr->numEntries) {
            err = BL_BCM_ERR_NOT_FOUND;
        } else {
            ptEntry = &ptEntry[i];
        }
    } else {
        err = BL_BCM_ERR_INVAL_STATE;
    }

    if (BL_BCM_ERR_OK == err) {
        uint32_t eraseSize;
        BL_FLASH_AddrType flashAddr = 0UL;
        uint32_t isSubSecSupport = BL_FALSE;
        uint32_t pages = 0UL;
        uint32_t pageSize = 0UL;
        uint32_t rem = 0UL;
        uint8_t rdBuf[BL_READ_BUF_SIZE];

        flashAddr = ptEntry->addr;

        err = BL_PTM_GetFlashInfo(aFlashID, &pageSize, &eraseSize, &isSubSecSupport);
        if (BL_BCM_ERR_OK != err) {
            goto err;
        }

        err = BL_PTM_FlashErase(aFlashID, flashAddr, eraseSize, isSubSecSupport);
        if (BL_BCM_ERR_OK != err) {
            goto err;
        }

        /* Flash Img Hdr and Img entries */
        err = BL_FLASH_Write(aFlashID, flashAddr, aAddr, aSize);
        if (BL_BCM_ERR_OK != err) {
            goto err;
        }
        pages = aSize / pageSize;
        rem = aSize % pageSize;
        for (i = 0UL; i < pages; i++) {
            err = BL_FLASH_Read(aFlashID, flashAddr + (i * pageSize),
                    rdBuf, pageSize);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            if (0L != BL_BCM_MemCmp(&aAddr[i * pageSize], rdBuf, pageSize)) {
                err = BL_BCM_ERR_DATA_INTEG;
                break;
            }
        }

        if (BL_BCM_ERR_OK != err) {
            goto err;
        }

        if (rem > 0UL) {
            err = BL_FLASH_Read(aFlashID, flashAddr + (i * pageSize),
                    rdBuf, rem);
            if (BL_BCM_ERR_OK != err) {
                goto err;
            }
            if (0L != BL_BCM_MemCmp(&aAddr[i * pageSize], rdBuf, rem)) {
                err = BL_BCM_ERR_DATA_INTEG;
                goto err;
            }
        }

        if (NULL != aCallback) {
            aCallback(100UL);
        }
    }

err:
    return err;
}

/** @brief Function to check if the partition is valid or not

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID    Flash Controller ID
    @param[in]      aPID        Partition ID

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Valid partition found
    @retval     #BL_BCM_ERR_NOT_FOUND      Valid partition not found
    @retval     Others                  Errors from #BL_PTM_ReadAndValidatePT

    @post None

    @trace  #BRCM_DSGN_BL_PTM_IS_VALID_PARTITION_PROC  #BRCM_ARCH_BL_PTM_IS_VALID_PARTITION

    @limitations None

    @code{.c}
    found = BL_BCM_ERR_NOT_FOUND
    ptEntry = BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE

    err = BL_PTM_ReadAndValidatePT(aFlashID)
    if BL_BCM_ERR_OK = err
        for i = 0 to BL_PTHdr->numEntries
            if ptEntry[i].pid = aPID
                found = BL_BCM_ERR_OK
                break

    return found
    @endcode
*/
int32_t BL_PTM_IsValidPartition(BL_PTM_FlashIDType aFlashID,
                                uint16_t aPID)
{
    uint32_t i;
    int32_t found = BL_BCM_ERR_NOT_FOUND;
    BL_PTM_PTEntryType *ptEntry =
        (BL_PTM_PTEntryType *)(BL_PTM_Comp.ptTbl + PTBL_HDR_SIZE);

    if (BL_PTM_STATE_IDLE == BL_PTM_Comp.state) {
        for (i = 0UL; i < BL_PTHdr->numEntries; i++) {
            if (ptEntry[i].pid == aPID) {
                found = BL_BCM_ERR_OK;
                break;
            }
        }
    }
    return found;
}

/**
    @image html bl_ptm_flowchart_write_pt.jpg
*/
int32_t BL_PTM_WritePT(BL_PTM_FlashIDType aFlashID,
                       const BL_PTM_AddrType const aAddr,
                       BL_PTM_SizeType aImgSize,
                       uint64_t aErrDetCode,
                       BL_PTM_ProgressCbType aCallback)
{
    int32_t err;
    uint32_t i, j;
    uint32_t percent;
    uint32_t totalSize;
    uint32_t eraseSize;
    uint32_t flashedSize = 0UL;
    BL_PTM_PTHdrType *ptHdr;
    BL_FLASH_AddrType flashAddr = 0UL;
    BL_PTM_FlashAddrType ptmFlashAddr = 0UL;
    uint32_t pages = 0UL;
    uint32_t pageSize = 0UL;
    uint32_t rem = 0UL;
    uint32_t isSubSecSupport = BL_FALSE;
    uint8_t rdBuf[BL_READ_BUF_SIZE];

    ptHdr = (BL_PTM_PTHdrType *)aAddr;

    /* is the PTM configured for this flash? */
    for (i = 0UL; i < BL_PTM_Comp.lookupTblSize; i++) {
        if (aFlashID == BL_PTM_Comp.lookupTbl[i].flashID) {
            break;
        }
    }
    if (i == BL_PTM_Comp.lookupTblSize) {
        err = BL_BCM_ERR_NOT_FOUND;
        goto err;
    }

    err = BL_PTM_GetFlashInfo(aFlashID, &pageSize, &eraseSize, &isSubSecSupport);
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

    /* For percentage calculation:
     * Total size = 1 sector erase for PT + Write 4 copies of PT +
     *              n sectors erase for PT entries + write of PT entries
     */
    totalSize = (ptHdr->totalSize * PTBL_NUM_COPIES) + eraseSize;

    ptmFlashAddr = BL_PTM_Comp.lookupTbl[i].flashAddr;
    /**
     * erase the sector where PTM shall be present
     * User shall ensure that PTM lookTable flash address
     * is aligned to sector/subsector bounary
     */
    err = BL_PTM_FlashErase(aFlashID,  ptmFlashAddr, eraseSize, isSubSecSupport);
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

    flashedSize += eraseSize;
    percent = (flashedSize * 100UL) / totalSize;
    if (NULL != aCallback) {
        aCallback(percent);
    }
    for (i = 0UL; i < PTBL_NUM_COPIES; i++) {
        flashAddr = ptmFlashAddr + (i * PTBL_SIZE);
        BL_LOG_ERR("Writing PT image@0x%x\n",flashAddr);
        /* Flash PT Hdr and PT entries */
        err = BL_FLASH_Write(aFlashID, flashAddr, aAddr, ptHdr->totalSize);
        if (BL_BCM_ERR_OK != err) {
            break;
        }
        pages = ptHdr->totalSize / pageSize;
        rem = ptHdr->totalSize % pageSize;
        for (j = 0UL; j < pages; j++) {
            err = BL_FLASH_Read(aFlashID, flashAddr + (j * pageSize),
                    rdBuf, pageSize);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            if (0L != BL_BCM_MemCmp(&aAddr[j * pageSize], rdBuf, pageSize)) {
                err = BL_BCM_ERR_DATA_INTEG;
                break;
            }
        }
        if (BL_BCM_ERR_OK != err) {
            break;
        }

        if (rem > 0UL) {
            err = BL_FLASH_Read(aFlashID, flashAddr + (j * pageSize),
                    rdBuf, rem);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            if (0L != BL_BCM_MemCmp(&aAddr[j * pageSize], rdBuf, rem)) {
                err = BL_BCM_ERR_DATA_INTEG;
                break;
            }
        }

        flashedSize += ptHdr->totalSize;
        percent = (flashedSize * 100UL) / totalSize;
        if (NULL != aCallback) {
            aCallback(percent);
        }
    }
    if (BL_BCM_ERR_OK != err) {
        goto err;
    }

err:
    return err;
}

int32_t BL_PTM_GetPTValidFlashID(BL_FLASH_HwIDType *aFlashID)
{
    int32_t err = BL_BCM_ERR_OK;

    if (BL_PTM_STATE_UNINIT == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_UNINIT;
        goto err_exit;
    }
    if (BL_PTM_STATE_ERROR == BL_PTM_Comp.state) {
        err = BL_BCM_ERR_INVAL_STATE;
        goto err_exit;
    }
    if (NULL == aFlashID) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    *aFlashID = BL_PTM_Comp.lookupTbl[BL_PTM_Comp.lookupIdx].flashID;

err_exit:
    return err;
}

/**
    @code{.c}
    if BL_PTM_STATE_UNINIT != BL_PTM_Comp.state
        BL_BCM_MemSet(BL_PTM_Comp.ptTbl, 0, PTBL_SIZE)
        BL_BCM_MemSet(BL_PTM_Comp.imgTbl, 0, BL_IMG_TABLE_SIZE)
        BL_PTM_Comp.state = BL_PTM_STATE_UNINIT
        BL_PTM_Comp.lookupIdx = 0UL
    @endcode
*/
void BL_PTM_DeInit(void)
{
    if (BL_PTM_STATE_UNINIT != BL_PTM_Comp.state) {
        BL_BCM_MemSet(BL_PTM_Comp.ptTbl, 0, PTBL_SIZE);
        BL_BCM_MemSet(BL_PTM_Comp.imgTbl, 0, BL_IMG_TABLE_SIZE);
        BL_PTM_Comp.state = BL_PTM_STATE_UNINIT;
        BL_PTM_Comp.lookupIdx = 0UL;
        BL_PTM_Comp.sysCfgCopyIdx = 0UL;
    }
}

/**
    @image html bl_ptm_flowchart_init.jpg
*/
int32_t BL_PTM_Init(const BL_PTM_CfgType *const aConfig)
{
    uint32_t flashAddr;
    uint32_t i = 0UL, j;
    uint32_t scratchMemSize;
    int32_t err = BL_BCM_ERR_UNKNOWN;
    const BL_PTM_LookupTblEntType *cfgtbl;
    BL_PTM_LookupTblEntType *lookupTbl = &BL_PTM_Comp.lookupTbl[0];

    if (NULL == aConfig) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto end;
    }

    scratchMemSize = (aConfig->scratchMemEndAddr - aConfig->scratchMemStartAddr);
    if ((NULL == aConfig->scratchMemStartAddr)
            || (NULL == aConfig->scratchMemEndAddr)
            || (BL_PT_TABLE_SIZE > scratchMemSize)) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto end;
    }
    if (BL_PTM_STATE_UNINIT != BL_PTM_Comp.state) {
        err = BL_BCM_ERR_INVAL_STATE;
        goto end;
    }

    BL_BCM_MemSet(aConfig->scratchMemStartAddr, 0L, scratchMemSize);
    BL_PTM_Comp.tempBuf = aConfig->scratchMemStartAddr;
    BL_PTM_Comp.tempBufSize = scratchMemSize;

    if (NULL != aConfig->lookupTbl) {
        cfgtbl = aConfig->lookupTbl;
        for (i = 0UL; i < aConfig->lookupTblSize; i++) {
            lookupTbl[i].flashID = cfgtbl[i].flashID;
            lookupTbl[i].flashAddr = cfgtbl[i].flashAddr;
        }
        BL_PTM_Comp.lookupTblSize = aConfig->lookupTblSize;
    } else {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto end;
    }
    /**
     * Search through all the 4 redundant copies to find a valid partition table
     */
    for (i = 0UL; i < BL_PTM_Comp.lookupTblSize; i++) {
        flashAddr = lookupTbl[i].flashAddr;
        for (j = 0UL; j < PTBL_NUM_COPIES; j++) {
            err = BL_PTM_ReadAndValidatePT(lookupTbl[i].flashID, flashAddr);
            if (BL_BCM_ERR_OK == err) {
                BL_PTM_Comp.state = BL_PTM_STATE_IDLE;
                break;
            }
            flashAddr += PTBL_SIZE;
        }
        if (BL_BCM_ERR_OK == err) {
            break;
        }
        BL_PTM_Comp.lookupIdx++;
    }

end:
    if (i == BL_PTM_Comp.lookupTblSize) { /* No valid PT header found !! */
        BL_PTM_Comp.state = BL_PTM_STATE_ERROR;
        err = BL_BCM_ERR_NOT_FOUND;
    }
    return err;
}

/** @} */
