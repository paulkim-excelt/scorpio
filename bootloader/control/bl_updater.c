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
    @defgroup grp_bl_bcs_impl Implementation
    @ingroup grp_bl_bcs

    @addtogroup grp_bl_bcs_impl
    @{
    @file bl_updater.c
    @brief BL_CNTRL Implementation
    This source file contains the implementation of BL_CNTRL

    @version 0.30 Imported from docx
*/

#include <stdint.h>
#include <bl_edc.h>
#include <bl_tar.h>
#include <bl_cntrl.h>
#include <bl_downloader.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#include <bl_board.h>
#include <bl_log.h>

/**
    @name BL_CNTRL API IDs
    @{
    @brief API IDs for BL_CNTRL
*/
#define BRCM_DSGN_BL_CNTRL_CONTEXT_TYPE                 (0x80U) /**< @brief #BL_CNTRL_ContextType           */
#define BRCM_DSGN_BL_CNTRL_GLOBAL_CONTEXT_VAR           (0x81U) /**< @brief #BL_CNTRL_Context               */
#define BRCM_DSGN_BL_CNTRL_TAR_PARSED_IMAGES_VAR        (0x82U) /**< @brief #BL_CNTRL_TAR_Parsed_Images     */
#define BRCM_DSGN_BL_CNTRL_PROGRESS_CB_PROC             (0x82U) /**< @brief #BL_CNTRL_ProgressCb            */
#define BRCM_DSGN_BL_CNTRL_GET_NUM_BL_PARTITIONS_PROC   (0x83U) /**< @brief #BL_CNTRL_GetNumBLPartitions    */
#define BRCM_DSGN_BL_CNTRL_GET_NUM_FW_PARTITIONS_PROC   (0x83U) /**< @brief #BL_CNTRL_GetNumFWPartitions    */
#define BRCM_DSGN_BL_CNTRL_UPDATE_FLASH_PROC            (0x84U) /**< @brief #BL_CNTRL_UpdateFlash           */
#define BRCM_DSGN_BL_CNTRL_UPDATE_FLASH_PIECEWISE_PROC  (0x85U) /**< @brief #BL_CNTRL_UpdateFlashPiecewise  */
#define BRCM_DSGN_BL_CNTRL_UPDATE_REPORT_ERROR_PROC     (0x86U) /**< @brief #BL_CNTRL_Updater_ReportError   */
/** @} */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)

/**
    @brief BCM Context structure

    @trace #BRCM_ARCH_BL_CNTRL_CONTEXT_TYPE #BRCM_REQ_BL_CNTRL_CONTEXT
*/
typedef struct _BL_CNTRL_ContextType {
    BL_PTM_ProgressCbType cb;              /**< @brief Flashing progress callback */
    uint32_t totalSize;                    /**< @brief Total size of the tar file */
    uint32_t updatedSize;                  /**< @brief variable to keep track of the flashed size */
    uint32_t curIdx;                       /**< @brief Current index */
    IMGL_ImgHdrType pwImgHdrs[BL_CNTRL_MAX_IMG_COUNT]; /**< @brief Array of pwImgHdrs to be written to flash */
} BL_CNTRL_ContextType;

/**
    @brief Global context for BL_CNTRL_ContextType

    Global structure to hold the current context of the BL_CNTRL.

    @trace  #BRCM_DSGN_BL_CNTRL_GLOBAL_CONTEXT_VAR  #BRCM_ARCH_BL_CNTRL_GLOBAL_CONTEXT_VAR
*/
BL_CNTRL_ContextType BL_CNTRL_Context;

static BL_COMP_NEVER_INLINE void BL_CNTRL_Updater_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_CNTRL_UPDATE_REPORT_ERROR_PROC, aErr, 4UL, values);
}

/** @brief Callback to send the flashing percentage to the client

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aPercent    Completed flashing percentage

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_PROGRESS_CB_PROC  #BRCM_ARCH_BL_CNTRL_PROGRESS_CB

    @limitations None

    @code{.c}
    if (NULL != BL_CNTRL_Context.cb) and (0UL != BL_CNTRL_Context.totalSize)
        p = aPercent * BL_CNTRL_Context.pwImgHdrs[BL_CNTRL_Context.curIdx].imgSize / 100UL
        p += BL_CNTRL_Context.updatedSize
        p = (100UL*p)/BL_CNTRL_Context.totalSize
        BL_CNTRL_Context.cb(p)
    @endcode
*/
static void BL_CNTRL_ProgressCb(uint32_t aPercent)
{
    if ((NULL != BL_CNTRL_Context.cb) && (BL_CNTRL_Context.totalSize != 0UL)) {
        uint32_t p = aPercent * BL_CNTRL_Context.pwImgHdrs[BL_CNTRL_Context.curIdx].imgSize / 100UL;
        p += BL_CNTRL_Context.updatedSize;
        p = (100UL*p)/BL_CNTRL_Context.totalSize;
        BL_CNTRL_Context.cb(p);
    }
}

/** @brief Function to get the number of BL partitions

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID    Flash ID

    Return values are documented in reverse-chronological order
    @retval     uint32_t    Number of BL partitions

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_GET_NUM_BL_PARTITIONS_PROC  #BRCM_ARCH_BL_CNTRL_GET_NUM_BL_PARTITIONS

    @limitations None

    @code{.c}
    ret = 0

    for i = 1UL to PTBL_MAX_NUM_ENTRIES
        if BL_BCM_ERR_OK = PTM_IsValidPartition(aFlashID, (i << 8UL)| PT_ENTRY_PID_IMG_ID_BL_IMAGE)
            ret++

    return ret
    @endcode
*/
static uint32_t BL_CNTRL_GetNumBLPartitions(uint32_t aFlashID)
{
    uint32_t i;
    uint32_t ret = 0UL;
    for (i = 1UL; i <= PTBL_MAX_NUM_ENTRIES; i++) {
        if (BL_BCM_ERR_OK == BL_PTM_IsValidPartition(aFlashID, (i << 8UL)| PTBL_ID_BL)) {
            ret++;
        }
    }
    return ret;
}

/** @brief Function to get the number of FW partitions

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID    Flash ID

    Return values are documented in reverse-chronological order
    @retval     uint32_t    Number of FW partitions

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_GET_NUM_FW_PARTITIONS_PROC  #BRCM_ARCH_BL_CNTRL_GET_NUM_FW_PARTITIONS

    @limitations None

    @code{.c}
    ret = 0

    for i = 1UL to PTBL_MAX_NUM_ENTRIES
        if BL_BCM_ERR_OK = PTM_IsValidPartition(aFlashID, (i << 8UL)| PT_ENTRY_PID_IMG_ID_FW_IMAGE)
            ret++

    return ret
    @endcode
*/
static uint32_t BL_CNTRL_GetNumFWPartitions(uint32_t aFlashID)
{
    uint32_t i;
    uint32_t ret = 0UL;
    for (i = 1UL; i <= PTBL_MAX_NUM_ENTRIES; i++) {
        if (BL_BCM_ERR_OK == BL_PTM_IsValidPartition(aFlashID, (i << 8UL)| PTBL_ID_FW)) {
            ret++;
        }
    }
    return ret;
}

static uint32_t BL_CNTRL_GetNumSYSCFGPartitions(uint32_t aFlashID)
{
    uint32_t i;
    uint32_t ret = 0UL;
    for (i = 1UL; i <= PTBL_MAX_NUM_ENTRIES; i++) {
        if (BL_BCM_ERR_OK == BL_PTM_IsValidPartition(aFlashID, (i << 8UL)| PTBL_ID_SYSCFG)) {
            ret++;
        }
    }
    return ret;
}

#if (BOOTROM_SUPPORTS_PT == 0)
typedef struct _BL_ROM_ImgHdrType {
    uint32_t magic;
#define BL_IMG_MAGIC                   (0x900DC0DEUL)
    uint32_t cfg;
#define BL_CFG_FLAG_IMG_CNT_MASK       (0xFUL << 26UL)
#define BL_CFG_FLAG_IMG_CNT_SHIFT      (26UL)
#define BL_CFG_FLAG_IMG_FINAL_MASK     (0x1UL << 25UL)
#define BL_CFG_FLAG_IMG_FINAL_SHIFT    (25UL)
    uint32_t tcmStart;
    uint32_t tcmEnd;
} BL_ROM_ImgHdrType;

static int32_t BL_CNTRL_FlashErase(BL_FLASH_HwIDType aFlashID,
                                   BL_FLASH_AddrType aAddr,
                                   BL_FLASH_LenType aLen)
{
    uint32_t i;
    int32_t err = BL_BCM_ERR_OK;

    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (aFlashID == BL_FLASH_Config[i].hwID) {
            if (BL_TRUE == BL_FLASH_Config[i].subSectorSupported) {
                err = BL_FLASH_EraseSubSector(aFlashID, aAddr, aLen);
            } else {
                err = BL_FLASH_EraseSector(aFlashID, aAddr, aLen);
            }
            break;
        }
    }
    if (i == BL_FLASH_CfgSize) {
        err = BL_BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

static int32_t BL_CNTRL_WriteBlImage(BL_FLASH_HwIDType aFlashID,
                                     const uint8_t *const aAddr,
                                     uint32_t aImgSize)
{
    uint32_t i;
    int32_t err = BL_BCM_ERR_OK;
    uint32_t flashAddr = 0UL;
    BL_ROM_ImgHdrType *hdr;
    uint32_t totalSz;
    uint32_t currSz = 0UL;
    uint32_t percent = 0UL;
    uint32_t lineNo = 0UL;

    hdr = (BL_ROM_ImgHdrType *)aAddr;
    totalSz = (aImgSize * BL_NUM_COPIES) + BL_PTM_Cfg.lookupTbl->flashAddr;

    if (BL_IMG_MAGIC != hdr->magic) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto exit;
    }

    /**
     * check if All the BL copies will fix into
     * 128KB region
     */

    if ((BL_NUM_COPIES * aImgSize) >= BL_PTM_Cfg.lookupTbl->flashAddr) {
        err = BL_BCM_ERR_NOMEM;
        lineNo = __LINE__;
        goto exit;
    }

    /**
     * Erase complete top 128KB reserved for BL copies
     * (PT image is right after the BL copies)
     */
    err = BL_CNTRL_FlashErase(aFlashID, flashAddr,
            BL_PTM_Cfg.lookupTbl->flashAddr);

    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        goto exit;
    }
    currSz =  BL_PTM_Cfg.lookupTbl->flashAddr;
    percent = (currSz * 100UL) / totalSz;
    BL_CNTRL_ProgressCb(percent);

    for (i = 0UL; i < BL_NUM_COPIES; i++) {
        /* increment the image count number in the image cfg */
        hdr->cfg &= ~BL_CFG_FLAG_IMG_CNT_MASK;
        hdr->cfg |= (i << BL_CFG_FLAG_IMG_CNT_SHIFT);

        /* set the final flag if its last copy */
        if ((BL_NUM_COPIES - 1UL) == i) {
            hdr->cfg |= BL_CFG_FLAG_IMG_FINAL_MASK;
        }
        BL_LOG_ERR("Writing BL image@0x%x\n", flashAddr);
        err = BL_FLASH_Write(aFlashID, flashAddr, aAddr, aImgSize);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            break;
        }
        flashAddr += aImgSize;
        currSz += aImgSize;
        percent = (currSz * 100UL) / totalSz;
        BL_CNTRL_ProgressCb(percent);
    }

exit:
    if (BL_BCM_ERR_OK != err) {
        BL_CNTRL_Updater_ReportError(err, aFlashID, 0UL, 0UL, 0UL, lineNo);
    }
    return err;
}
#endif
extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];;


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
    @param[in]  aCallback           Callback for the flash progress

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Update successful
    @retval     #BL_BCM_ERR_INVAL_PARAMS    If the arguments are invalid

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_UPDATE_FLASH_PIECEWISE_PROC  #BRCM_REQ_BL_BCS_UPDATE_FLASH

    @limitations None
*/
int32_t BL_CNTRL_UpdateFlashPiecewise(
    const uint32_t aFlashID,
    const uint32_t aImageCount,
    const IMGL_ImgHdrType aPwImgHdrs[],
    BL_CNTRL_ImageFetchProcType aImageFetchProc,
    uint8_t* aDwnldAddress,
    BL_PTM_ProgressCbType aCallback)
{
    uint32_t i, j, start;
    uint32_t numBL, numFW, numSYSCFG;
    uint8_t *img = aDwnldAddress;
    int32_t lineNo = 0UL;
    int32_t ret = BL_BCM_ERR_OK;

    if ((0UL == aImageCount) ||
        (BL_CNTRL_MAX_IMG_COUNT < aImageCount) ||
        (NULL == aPwImgHdrs) ||
        (NULL == aImageFetchProc)) {
        lineNo = __LINE__;
        ret = BL_BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    BL_CNTRL_Context.cb = aCallback;
    BL_CNTRL_Context.totalSize = 0UL;
    BL_CNTRL_Context.updatedSize = 0UL;
    BL_CNTRL_Context.curIdx = 0UL;
    BL_BCM_MemCpy(BL_CNTRL_Context.pwImgHdrs, aPwImgHdrs, aImageCount * sizeof(IMGL_ImgHdrType));

    #define CALL_FUNC_AND_CHECK_ERROR(fn, ...)  \
        ret = (fn)(__VA_ARGS__);                \
        if (BL_BCM_ERR_OK != ret) {             \
            lineNo = __LINE__;                  \
            goto exit;                          \
        }

    start = 0UL;
    /* See if the partition table needs to be updated */
    if (BL_PT_IMG_PID == BL_CNTRL_Context.pwImgHdrs[0UL].pid) {
        CALL_FUNC_AND_CHECK_ERROR(aImageFetchProc, &BL_CNTRL_Context.pwImgHdrs[0UL], img);
        CALL_FUNC_AND_CHECK_ERROR(BL_PTM_WritePT, aFlashID, img, BL_CNTRL_Context.pwImgHdrs[0UL].imgSize, 0UL, BL_CNTRL_ProgressCb);

        /* Reinitialize the PT */
        BL_PTM_DeInit();
        CALL_FUNC_AND_CHECK_ERROR(BL_PTM_Init, &BL_PTM_Cfg);

        start++;
    } else {
        /* Do nothing */
        /* Flow comes here in case of partial update */
    }

    /* Compute total size to be written to flash. For progress computations. */
    numBL = BL_CNTRL_GetNumBLPartitions(aFlashID);
    numFW = BL_CNTRL_GetNumFWPartitions(aFlashID);
    numSYSCFG = BL_CNTRL_GetNumSYSCFGPartitions(aFlashID);
    for (i = 0UL; i < aImageCount; i++) {
        if (PTBL_ID_BL == BL_CNTRL_Context.pwImgHdrs[i].pid) {
            BL_CNTRL_Context.totalSize += BL_CNTRL_Context.pwImgHdrs[i].imgSize * numBL;
        } else if ((PTBL_ID_FW == BL_CNTRL_Context.pwImgHdrs[i].pid)
                || (PTBL_ID_USER_DATA == BL_CNTRL_Context.pwImgHdrs[i].pid)) {
            BL_CNTRL_Context.totalSize += BL_CNTRL_Context.pwImgHdrs[i].imgSize * numFW;
        } else if (PTBL_ID_SYSCFG == BL_CNTRL_Context.pwImgHdrs[i].pid) {
            BL_CNTRL_Context.totalSize += BL_CNTRL_Context.pwImgHdrs[i].imgSize * numSYSCFG;
        } else {
            /* Do Nothing */
        }
    }

    for (i = 1UL; i <= PTBL_MAX_NUM_COPIES; i++) {
        for (j = start; j < aImageCount; j++) {

            uint32_t pid, imgID;

            BL_CNTRL_Context.curIdx = j;
            imgID = BL_CNTRL_Context.pwImgHdrs[j].imgID;

#if (BOOTROM_SUPPORTS_PT == 0)
            /*
                BootROM on some chips does not understand the partiion table defined in the SDK,
                and expects to find the BL at a particular location in the flash. This path handles
                such a case.
            */
            if ((PTBL_ID_BL == BL_CNTRL_Context.pwImgHdrs[j].pid) && (0UL == imgID) && (BL_FALSE == BL_CNTRL_Context.pwImgHdrs[j].isImgHdr)) {
                if (1UL == i) {
                    /* Fetch the BL image and update */
                    CALL_FUNC_AND_CHECK_ERROR(aImageFetchProc, &BL_CNTRL_Context.pwImgHdrs[j], img);
                    CALL_FUNC_AND_CHECK_ERROR(BL_CNTRL_WriteBlImage, aFlashID, img, BL_CNTRL_Context.pwImgHdrs[j].imgSize);
                    BL_CNTRL_Context.updatedSize += numBL * BL_CNTRL_Context.pwImgHdrs[j].imgSize;
                }
            } else {
#endif
            pid = (i << 8UL) | (BL_CNTRL_Context.pwImgHdrs[j].pid);

            if (BL_BCM_ERR_OK == BL_PTM_IsValidPartition(aFlashID, pid)) {

                CALL_FUNC_AND_CHECK_ERROR(aImageFetchProc, &BL_CNTRL_Context.pwImgHdrs[j], img);

                if (BL_TRUE == BL_CNTRL_Context.pwImgHdrs[j].isImgHdr) {
                    CALL_FUNC_AND_CHECK_ERROR(BL_PTM_WriteImgHdr, aFlashID, pid,
                        img, BL_CNTRL_Context.pwImgHdrs[j].imgSize, BL_CNTRL_ProgressCb);
                } else {
                    uint32_t edc = BL_EDC_CalculateCRC(img, BL_CNTRL_Context.pwImgHdrs[j].imgSize, BL_EDC_CRC_INIT_SEED);
                    BL_LOG_ERR("Flashing image ID @ [%x] = (%x, %x) \n", BL_CNTRL_Context.pwImgHdrs[j].imgSize, pid, imgID);
                    CALL_FUNC_AND_CHECK_ERROR(BL_PTM_WriteImg, aFlashID, pid, imgID,
                        img, BL_CNTRL_Context.pwImgHdrs[j].imgSize, edc, BL_CNTRL_ProgressCb);
                }
                BL_CNTRL_Context.updatedSize += BL_CNTRL_Context.pwImgHdrs[j].imgSize;
            }

#if (BOOTROM_SUPPORTS_PT == 0)
            }
#endif
        }
    }

    ret = BL_BCM_ERR_OK;

exit:

    if (BL_BCM_ERR_OK != ret) {
        BL_CNTRL_Updater_ReportError(ret, aFlashID, 0UL, 0UL, 0UL, lineNo);
    }

    return ret;
}

/** @} */
