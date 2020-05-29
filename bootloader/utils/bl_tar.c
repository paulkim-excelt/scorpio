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
    @defgroup grp_bl_tar_impl Implementation
    @ingroup grp_bl_tar

    @addtogroup grp_bl_tar_impl
    @{
    @file bl_tar.c
    @brief TAR Implementation
    This source file contains the implementation of TAR

    @version 0.30 Imported from docx
*/

#include <stdint.h>
#include <bl_edc.h>
#include <bl_tar.h>
#include <bl_downloader.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#include <bl_board.h>
#include <bl_cntrl.h>
#include <compiler.h>
#include <bl_log.h>

/**
    @name TAR API IDs
    @{
    @brief API IDs for TAR
*/
#define BRCM_DSGN_BL_TAR_HEADER_INFO_MACRO      (0x80U) /**< @brief #BL_TAR_HEADER_NAME_OFFSET */
#define BRCM_DSGN_BL_TAR_HEADER_SIZE_MACRO      (0x81U) /**< @brief #BL_TAR_HEADER_SIZE */
#define BRCM_DSGN_BL_TAR_GET_PID_PROC           (0x82U) /**< @brief #BL_TAR_GetPID */
#define BRCM_DSGN_BL_TAR_GET_IMG_ID_PROC        (0x83U) /**< @brief #BL_TAR_GetImgID */
#define BRCM_DSGN_BL_TAR_GET_FILE_SIZE_PROC     (0x84U) /**< @brief #BL_TAR_GetFileSize */
#define BRCM_DSGN_BL_TAR_CHECK_HEADER_PROC      (0x85U) /**< @brief #BL_TAR_CheckHeader */
/** @} */

/**
    @name TAR Header Info
    @{
    @brief TAR header bit fields info
*/
#define BL_TAR_HEADER_NAME_OFFSET      (0UL)   /**< @brief File name offset */
#define BL_TAR_HEADER_NAME_LENGTH      (100UL) /**< @brief File name max length */
#define BL_TAR_HEADER_SIZE_OFFSET      (124UL) /**< @brief Size offset */
#define BL_TAR_HEADER_SIZE_LENGTH      (12UL)  /**< @brief Size length (in bits) */
#define BL_TAR_HEADER_MAGIC_OFFSET     (257UL) /**< @brief Magic offset */
#define BL_TAR_HEADER_MAGIC_LENGTH     (6UL)   /**< @brief Magic length (in bits) */
/** @} */

/**
    @brief Structure describing the flashable images in a TAR file

    @trace #BRCM_ARCH_BL_CNTRL_IMG_TYPE #BRCM_REQ_BL_BCS_IMG
*/
typedef struct _BL_TAR_Parsed_ImgType {
    uint8_t  *baseAddr;                                         /* @brief Base address of the TAR   */
    uint32_t numImages;                                         /* @brief Number of images          */
    uint32_t imgOffsets[BL_CNTRL_MAX_IMG_COUNT];                /* @brief Image offsets             */
    IMGL_ImgHdrType pwImgHdrs[BL_CNTRL_MAX_IMG_COUNT];    /* @brief Pw Image headers          */
} BL_TAR_Parsed_ImgType;

/**
    @brief Global to hold the parsed TAR image description

    @trace  #BRCM_DSGN_BL_CNTRL_TAR_IMAGES_VAR  #BRCM_ARCH_BL_CNTRL_TAR_IMAGES_VAR
*/
BL_TAR_Parsed_ImgType BL_TAR_Parsed_Images;

/**
    @brief Size of the TAR header

    @trace #BRCM_DSGN_BL_BL_TAR_HEADER_SIZE_MACRO #BRCM_REQ_BL_BL_TAR_HEADER_SIZE
*/
#define BL_TAR_HEADER_SIZE (512UL)

static uint8_t BL_ImgHdrStr[] = ".bin";

static uint8_t BL_ImgHdrMagic[] = {0x48U, 0x47U,  0x4DU, 0x49U}; /* 'I''M''G''H'*/

static uint32_t BL_TAR_isImgHdr(const uint8_t *const aHdr)
{

    uint32_t isImgHdr = BL_TRUE;
    uint32_t i;
    uint32_t j = 0UL;
    for (i = (BL_TAR_HEADER_NAME_OFFSET + 2UL); i < (BL_TAR_HEADER_NAME_OFFSET + 6UL); i++) {
        if (aHdr[i] != BL_ImgHdrStr[j++]) {
            isImgHdr = BL_FALSE;
            break;
        }
    }

    j = 0UL;
    if (BL_TRUE == isImgHdr) {
        for (i = BL_TAR_HEADER_SIZE; i < (BL_TAR_HEADER_SIZE + 4UL); i++) {
            if (aHdr[i] != BL_ImgHdrMagic[j++]) {
                isImgHdr = BL_FALSE;
                break;
            }
        }
    }

    return isImgHdr;
}

/** @brief Function to get the PID from the header

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr    TAR header

    Return values are documented in reverse-chronological order
    @retval     uint16_t    PID

    @post None

    @trace  #BRCM_DSGN_BL_TAR_GET_PID_PROC  #BRCM_ARCH_BL_TAR_GET_PID

    @limitations None

    @code{.c}
    for i = BL_TAR_HEADER_NAME_OFFSET to (BL_TAR_HEADER_NAME_OFFSET + 2UL)
        pid = pid * 16
        if (aHdr[i] >= '0') and (aHdr[i] <= '9')
            pid += aHdr[i] - '0'
        else if (aHdr[i] >= 'A') and (aHdr[i] <= 'F')
            pid += aHdr[i] - 'A' + 10

    return pid
    @endcode
*/
static uint16_t BL_TAR_GetPID(const uint8_t *const aHdr)
{
    uint32_t i;
    uint16_t pid = 0UL;
    for (i = BL_TAR_HEADER_NAME_OFFSET; i < (BL_TAR_HEADER_NAME_OFFSET + 2UL); i++) {
        pid = pid * 16UL;
        if ((aHdr[i] >= '0') && (aHdr[i] <= '9')) {
            pid += aHdr[i] - '0';
        } else if ((aHdr[i] >= 'A') && (aHdr[i] <= 'F')) {
            pid += aHdr[i] - 'A' + 10;
        }
    }
    return pid;
}

/** @brief Function to get the Image ID from the header

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr    TAR header

    Return values are documented in reverse-chronological order
    @retval     uint16_t    Image ID

    @post None

    @trace  #BRCM_DSGN_BL_TAR_GET_IMG_ID_PROC  #BRCM_ARCH_BL_TAR_GET_IMG_ID

    @limitations None

    @code{.c}
    for i = (BL_TAR_HEADER_NAME_OFFSET + 3UL) to (BL_TAR_HEADER_NAME_OFFSET + 7UL)
        imgID = imgID * 16
        if (aHdr[i] >= '0') and (aHdr[i] <= '9')
            imgID += aHdr[i] - '0'
        else if (aHdr[i] >= 'A') and (aHdr[i] <= 'F')
            imgID += aHdr[i] - 'A' + 10

    return imgID
    @endcode
*/
static uint16_t BL_TAR_GetImgID(const uint8_t *const aHdr)
{
    uint32_t i;
    uint16_t imgID = 0UL;
    for (i = (BL_TAR_HEADER_NAME_OFFSET + 3UL); i < (BL_TAR_HEADER_NAME_OFFSET + 7UL); i++) {
        imgID = imgID * 16UL;
        if ((aHdr[i] >= '0') && (aHdr[i] <= '9')) {
            imgID += aHdr[i] - '0';
        } else if ((aHdr[i] >= 'A') && (aHdr[i] <= 'F')) {
            imgID += aHdr[i] - 'A' + 10;
        }
    }
    return imgID;
}

/** @brief Function to get the file size from the header

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr    TAR header

    Return values are documented in reverse-chronological order
    @retval     uint32_t    File size

    @post None

    @trace  #BRCM_DSGN_BL_TAR_GET_FILE_SIZE_PROC  #BRCM_ARCH_BL_TAR_GET_FILE_SIZE

    @limitations None

    @code{.c}
    for i = BL_TAR_HEADER_SIZE_OFFSET to (BL_TAR_HEADER_SIZE_OFFSET + BL_TAR_HEADER_SIZE_LENGTH - 1UL)
        size = size * 8
        if (aHdr[i] >= '0') and (aHdr[i] <= '9')
            size += aHdr[i] - '0'

    return size;
    @endcode
*/
static uint32_t BL_TAR_GetFileSize(const uint8_t *const aHdr)
{
    uint32_t i;
    uint32_t size = 0UL;
    for (i = BL_TAR_HEADER_SIZE_OFFSET; i < (BL_TAR_HEADER_SIZE_OFFSET + BL_TAR_HEADER_SIZE_LENGTH - 1UL); i++) {
        size = size * 8UL;
        if ((aHdr[i] >= '0') && (aHdr[i] <= '9')) {
            size += aHdr[i] - '0';
        }
    }
    return size;
}

/** @brief Function to get the file size from the header

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr    TAR header

    Return values are documented in reverse-chronological order
    @retval     uint32_t    File size

    @post None

    @trace  #BRCM_DSGN_BL_TAR_CHECK_HEADER_PROC  #BRCM_ARCH_BL_TAR_CHECK_HEADER

    @limitations None

    @code{.c}
    if (aHdr[BL_TAR_HEADER_MAGIC_OFFSET] = 'u')
        and (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 1UL] = 's')
        and (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 2UL] = 't')
        and (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 3UL] = 'a')
        and (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 4UL] = 'r')
        and (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 5UL] = ' '))
            return BL_TRUE
    else
        return BL_FALSE
    @endcode
*/
static uint32_t BL_TAR_CheckHeader(const uint8_t *const aHdr)
{
    if ((aHdr[BL_TAR_HEADER_MAGIC_OFFSET] == 'u')
        && (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 1UL] == 's')
        && (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 2UL] == 't')
        && (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 3UL] == 'a')
        && (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 4UL] == 'r')
        && (aHdr[BL_TAR_HEADER_MAGIC_OFFSET + 5UL] == ' ')) {
            return BL_TRUE;
    } else {
        return BL_FALSE;
    }
}

/** @brief Flashing TAR content

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aTarFile    Input image
    @param[in]      aSize       Size of the tar file
    @param[in]      aImages     Image Info

    Return values are documented in reverse-chronological order
    @retval       #BL_BCM_ERR_OK           CRC matches
    @retval       #BL_BCM_ERR_DATA_INTEG   If Data Integrity checks failed
    @retval       #BL_BCM_ERR_INVAL_PARAMS If TAR file content is incorrect

    @post None

    @trace  #BRCM_ARCH_BL_DWNLD_TAR_PARSE_PROC  #BRCM_REQ_BL_DWNLD_TAR_PARSE
*/
static int32_t BL_TAR_Parse(uint8_t *const aTarFile,
                      uint32_t aSize,
                      BL_TAR_Parsed_ImgType *aImages)
{
    uint32_t i = 0UL;
    uint32_t offset = 0UL;
    uint32_t totalSize = 0UL;

    while ((offset + BL_TAR_HEADER_SIZE < aSize) && (i < BL_CNTRL_MAX_IMG_COUNT)) {
        if (BL_FALSE == BL_TAR_CheckHeader(&aTarFile[offset])) {
            break;
        }

        aImages->imgOffsets[i] = offset + BL_TAR_HEADER_SIZE;

        aImages->pwImgHdrs[i].imgSize = BL_TAR_GetFileSize(&aTarFile[offset]);
        aImages->pwImgHdrs[i].pid = BL_TAR_GetPID(&aTarFile[offset]);
        aImages->pwImgHdrs[i].isImgHdr = BL_TAR_isImgHdr(&aTarFile[offset]);
        if (BL_FALSE == aImages->pwImgHdrs[i].isImgHdr) {
            aImages->pwImgHdrs[i].imgID = BL_TAR_GetImgID(&aTarFile[offset]);
        } else {
            aImages->pwImgHdrs[i].imgID = 0UL;
        }

        totalSize = (BL_TAR_HEADER_SIZE + aImages->pwImgHdrs[i].imgSize + (BL_TAR_HEADER_SIZE - 1UL)) & (~(BL_TAR_HEADER_SIZE - 1UL));
        offset += totalSize;

        i++;
    }

    aImages->numImages = i;
    aImages->baseAddr = aTarFile;

    return BL_BCM_ERR_OK;
}

static int32_t BL_TAR_FetchImage(const IMGL_ImgHdrType *const aPwImgHdr,
                                uint8_t *aImg)
{
    int32_t ret;
    uint32_t i;

    if ((NULL == aPwImgHdr) || (NULL == aImg)) {
        ret = BL_BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    #define COMPARE_IMG_DESCS(idx)                                                      \
        ((aPwImgHdr->pid == BL_TAR_Parsed_Images.pwImgHdrs[i].pid) &&             \
         (aPwImgHdr->imgID == BL_TAR_Parsed_Images.pwImgHdrs[i].imgID) &&         \
         (aPwImgHdr->imgSize == BL_TAR_Parsed_Images.pwImgHdrs[i].imgSize) &&     \
         (aPwImgHdr->isImgHdr == BL_TAR_Parsed_Images.pwImgHdrs[i].isImgHdr))

    for (i = 0UL; i < BL_TAR_Parsed_Images.numImages; i++) {
        if (COMPARE_IMG_DESCS(i)) {
            BL_BCM_MemCpy(aImg, BL_TAR_Parsed_Images.baseAddr + BL_TAR_Parsed_Images.imgOffsets[i], aPwImgHdr->imgSize);
            break;
        }
    }

    if (BL_TAR_Parsed_Images.numImages == i) {
        ret = BL_BCM_ERR_NOT_FOUND;
    } else {
        ret = BL_BCM_ERR_OK;
    }

done:
    return ret;
}

static uint8_t BL_TAR_ScratchMemory[512*1024] COMP_SECTION(".noload.data.tar_scratch");

/** @brief Function to update the flash with one TAR file as an input

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID    Flash ID
                    aTarFile    Tar file packing all the images to be flashed
                    aSize       Tar file size
                    aCallback   Progress callback

    Return values are documented in reverse-chronological order
    @retval       #BL_BCM_ERR_OK             Flashing completed
    @retval       #BL_BCM_ERR_INVAL_PARAMS   If the arguments are invalid

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_GET_NUM_FW_PARTITIONS_PROC  #BRCM_ARCH_BL_CNTRL_GET_NUM_FW_PARTITIONS

    @limitations None
*/
int32_t BL_TAR_UpdateFlash(uint32_t aFlashID,
                             uint8_t *const aTarFile,
                             uint32_t aSize,
                             BL_PTM_ProgressCbType aCallback)
{
    int32_t ret = BL_BCM_ERR_OK;

    if (NULL == aTarFile) {
        ret = BL_BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    ret = BL_TAR_Parse(aTarFile, aSize, &BL_TAR_Parsed_Images);
    if (BL_BCM_ERR_OK != ret) {
        goto exit;
    }

    ret = BL_CNTRL_UpdateFlashPiecewise(aFlashID, BL_TAR_Parsed_Images.numImages,
                                        BL_TAR_Parsed_Images.pwImgHdrs,
                                        BL_TAR_FetchImage, BL_TAR_ScratchMemory, aCallback);

exit:
    return ret;
}

/** @} */
