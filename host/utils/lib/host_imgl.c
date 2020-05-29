/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <rpc_cmds.h>
#include "host_imgl.h"
#include "hipc.h"
#include <bcm_err.h>
#include "bl_downloader.h"
#include "bl_ipc_downloader.h"
#include <sys_ipc_cmds.h>
#include <imgl_ipc_cmds.h>
#include <host_system.h>
#include <utils.h>
#include "bl_chip_config.h"
#include <hlog.h>
#include <crc.h>

#define FLASH_PROGRAM_TIMEOUT_SEC   (5*60) /* 5minutes */
#define MSEC_PER_SEC                (1000)
#define USEC_PER_MSEC               (1000)

#define TAR_HEADER_NAME_OFFSET  (0UL)
#define TAR_HEADER_NAME_LENGTH  (100UL)
#define TAR_HEADER_SIZE_OFFSET  (124UL)
#define TAR_HEADER_SIZE_LENGTH  (12UL)
#define TAR_HEADER_MAGIC_OFFSET  (257UL)
#define TAR_HEADER_MAGIC_LENGTH  (6UL)

#define TAR_HEADER_SIZE (512UL)
#define TAR_CALLBACK_CURRENT    (0UL)
#define TAR_CALLBACK_TOTAL      (1UL)
#define TAR_CALLBACK_LAST       (2UL)

#define MSG_BUF_SIZE    (512UL - 128UL)
#define CRC32_POLY                       (0x04C11DB7UL)

typedef struct _TAR_Parsed_ImgType {
    uint32_t isInitialized;
    const uint8_t  *baseAddr;
    uint32_t numImages;
    uint32_t imgOffsets[BL_CNTRL_MAX_IMG_COUNT];
    IMGL_ImgHdrType pwImgHdrs[BL_CNTRL_MAX_IMG_COUNT];
} TAR_Parsed_ImgType;

static TAR_Parsed_ImgType parsedTarImages;


#define COMPARE_IMG_HDRS_AND_SIZE(aImgHdr, i)                        \
    ((aImgHdr->pid == parsedTarImages.pwImgHdrs[i].pid) &&           \
     (aImgHdr->imgID == parsedTarImages.pwImgHdrs[i].imgID) &&       \
     (aImgHdr->imgSize == parsedTarImages.pwImgHdrs[i].imgSize) &&   \
     (aImgHdr->isImgHdr == parsedTarImages.pwImgHdrs[i].isImgHdr))

#define COMPARE_IMG_HDRS(aImgHdr, i)                                 \
    ((aImgHdr->pid == parsedTarImages.pwImgHdrs[i].pid) &&           \
     (aImgHdr->imgID == parsedTarImages.pwImgHdrs[i].imgID) &&       \
     (aImgHdr->isImgHdr == parsedTarImages.pwImgHdrs[i].isImgHdr))

static uint16_t tar_get_pid(const uint8_t *const hdr)
{
    uint32_t i;
    uint16_t pid = 0UL;
    for (i = TAR_HEADER_NAME_OFFSET; i < (TAR_HEADER_NAME_OFFSET + 2UL); i++) {
        pid = pid * 16UL;
        if ((hdr[i] >= '0') && (hdr[i] <= '9')) {
            pid += hdr[i] - '0';
        } else if ((hdr[i] >= 'A') && (hdr[i] <= 'F')) {
            pid += hdr[i] - 'A' + 10;
        }
    }
    return pid;
}

static uint16_t tar_get_img_id(const uint8_t *const hdr)
{
    uint32_t i;
    uint16_t imgID = 0UL;
    for (i = (TAR_HEADER_NAME_OFFSET + 3UL); i < (TAR_HEADER_NAME_OFFSET + 7UL); i++) {
        imgID = imgID * 16UL;
        if ((hdr[i] >= '0') && (hdr[i] <= '9')) {
            imgID += hdr[i] - '0';
        } else if ((hdr[i] >= 'A') && (hdr[i] <= 'F')) {
            imgID += hdr[i] - 'A' + 10;
        }
    }
    return imgID;
}

static const uint8_t ImgHdrStr[] = ".bin";

static const uint8_t ImgHdrMagic[] = {0x48U, 0x47U,  0x4DU, 0x49U}; /* 'I''M''G''H'*/

static uint32_t tar_is_img_hdr(const uint8_t *const hdr)
{
    uint32_t isImgHdr = TRUE;
    uint32_t i;
    uint32_t j = 0UL;
    for (i = (TAR_HEADER_NAME_OFFSET + 2UL); i < (TAR_HEADER_NAME_OFFSET + 6UL); i++) {
        if (hdr[i] != ImgHdrStr[j++]) {
            isImgHdr = FALSE;
            break;
        }
    }

	j = 0UL;
    if (TRUE == isImgHdr) {
        for (i = TAR_HEADER_SIZE; i < (TAR_HEADER_SIZE + 4UL); i++) {
            if (hdr[i] != ImgHdrMagic[j++]) {
                isImgHdr = FALSE;
                break;
            }
        }
    }

    return isImgHdr;
}

static uint32_t tar_get_file_size(const uint8_t *const hdr)
{
    uint32_t i;
    uint32_t size = 0UL;
    for (i = TAR_HEADER_SIZE_OFFSET; i < (TAR_HEADER_SIZE_OFFSET + TAR_HEADER_SIZE_LENGTH - 1UL); i++) {
        size = size * 8UL;
        if ((hdr[i] >= '0') && (hdr[i] <= '9')) {
            size += hdr[i] - '0';
        }
    }
    return size;
}

static uint32_t tar_check_header(const uint8_t *const hdr)
{
    if ((hdr[TAR_HEADER_MAGIC_OFFSET] == 'u')
        && (hdr[TAR_HEADER_MAGIC_OFFSET + 1UL] == 's')
        && (hdr[TAR_HEADER_MAGIC_OFFSET + 2UL] == 't')
        && (hdr[TAR_HEADER_MAGIC_OFFSET + 3UL] == 'a')
        && (hdr[TAR_HEADER_MAGIC_OFFSET + 4UL] == 'r')
        && (hdr[TAR_HEADER_MAGIC_OFFSET + 5UL] == ' ')) {
            return TRUE;
    } else {
        return FALSE;
    }
}

int32_t HOST_ImglInit(const uint8_t *tar, uint32_t size)
{
    uint32_t i = 0UL;
    uint32_t offset = 0UL;
    uint32_t totalSize = 0UL;

    while ((offset + TAR_HEADER_SIZE < size) && (i < BL_CNTRL_MAX_IMG_COUNT)) {
        uint16_t pid = 0UL;
        uint32_t imageSize = 0UL;
        uint8_t  isImgHdr = FALSE;

        if (FALSE == tar_check_header(&tar[offset])) {
            break;
        }

        pid = tar_get_pid(&tar[offset]);
        isImgHdr = tar_is_img_hdr(&tar[offset]);
        imageSize = tar_get_file_size(&tar[offset]);
        totalSize = (TAR_HEADER_SIZE + imageSize + (TAR_HEADER_SIZE - 1UL)) & (~(TAR_HEADER_SIZE - 1UL));

        parsedTarImages.imgOffsets[i] = offset + TAR_HEADER_SIZE;

        parsedTarImages.pwImgHdrs[i].imgSize = imageSize;
        parsedTarImages.pwImgHdrs[i].pid = pid;
        parsedTarImages.pwImgHdrs[i].isImgHdr = isImgHdr;

        if (FALSE == isImgHdr) {
            parsedTarImages.pwImgHdrs[i].imgID = tar_get_img_id(&tar[offset]);
        } else {
            parsedTarImages.pwImgHdrs[i].imgID = 0UL;
        }

        offset += totalSize;
        i++;
    }

    parsedTarImages.numImages = i;
    parsedTarImages.baseAddr = tar;
    parsedTarImages.isInitialized = 1UL;

    return BCM_ERR_OK;
}

int32_t HOST_ImglGetBootloader(uint32_t* aOffset, uint32_t* aSize)
{
    uint32_t i;
    int32_t retval = BCM_ERR_OK;
    IMGL_ImgHdrType blImgHdr;
    IMGL_ImgHdrType *fetchImageHdr = &blImgHdr;

    blImgHdr.pid = 1;
    blImgHdr.isImgHdr = 0;
    blImgHdr.imgID = 0;
    blImgHdr.imgSize = 0;

    if ((NULL == aOffset) || (NULL == aSize)) {
        retval = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == parsedTarImages.isInitialized) {
        retval = BCM_ERR_INVAL_STATE;
    } else {
        for (i = 0UL; i < parsedTarImages.numImages; i++) {
            if (COMPARE_IMG_HDRS(fetchImageHdr, i)) {
                break;
            }
        }

        if (parsedTarImages.numImages == i) {
            retval = BCM_ERR_INVAL_PARAMS;
            HOST_Log("%s:%d status:%d %d %d %d\n", __func__, __LINE__,
                fetchImageHdr->pid, fetchImageHdr->imgID, fetchImageHdr->imgSize,
                fetchImageHdr->isImgHdr);
                goto exit;
        } else {
            *aOffset = parsedTarImages.imgOffsets[i];
            *aSize = parsedTarImages.pwImgHdrs[i].imgSize;
        }
    }

exit:
    if (BCM_ERR_OK != retval) {
        HOST_Log("%s:%d status:%d\n", __func__, __LINE__, retval);
    }
    return retval;
}


int32_t HOST_ImglGetParsedImgHdrs(SYS_UpdateExecCmdType* aInfo)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;

    if (NULL == aInfo) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == parsedTarImages.isInitialized) {
        retVal = BCM_ERR_INVAL_STATE;
    } else {
        aInfo->numImgs = uswap32(parsedTarImages.numImages);
        for (i = 0UL; i < parsedTarImages.numImages; i++) {
            aInfo->imgHdr[i].isImgHdr = parsedTarImages.pwImgHdrs[i].isImgHdr;
            aInfo->imgHdr[i].pid = parsedTarImages.pwImgHdrs[i].pid;
            aInfo->imgHdr[i].imgID = uswap16(parsedTarImages.pwImgHdrs[i].imgID);
            aInfo->imgHdr[i].imgSize = uswap32(parsedTarImages.pwImgHdrs[i].imgSize);
        }
    }
    return retVal;
}

static int32_t transfer_image(const uint8_t *const image, const uint32_t imgLen)
{
    int32_t retVal = -1;
    uint32_t recv_len;
    uint32_t cmdId;
    uint32_t i, itr, offset, cpy_sz;
    uint8_t sendMsgBuf[MSG_BUF_SIZE];

    memset(sendMsgBuf, 0, MSG_BUF_SIZE);
    BL_DWNLD_ImgHdrType *imgHdrPtr = (BL_DWNLD_ImgHdrType *)sendMsgBuf;

    /*
        We will resend the imgHdrPtr along with the image. This is expected
        to be in place by the bootloader.
    */
    const uint32_t len = imgLen;

    /* Send Download header */
    cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);

    imgHdrPtr->magicNum = uswap32(0x494D4748UL); /* ITBL_MAGIC */
    imgHdrPtr->imgSize = uswap32(len);
    imgHdrPtr->flags = uswap32(1UL);
    /* Compute CRC using BCM_CRC32() when CRC is enabled in flags */
    imgHdrPtr->errDetCode = uswap64(BCM_CRC32(image, imgLen, CRC32_POLY));

    HOST_Log("transfer_image: len: %d\n", imgLen);
    retVal = HIPC_SendRecv(cmdId, sendMsgBuf,
            sizeof(BL_DWNLD_ImgHdrType), sendMsgBuf,
            MSG_BUF_SIZE, &recv_len);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (BCM_ERR_OK != uswap32(*((int32_t *)sendMsgBuf))) {
        HOST_Log("%s status:%d\n", __func__, uswap32(*((int32_t *)sendMsgBuf)));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (recv_len != sizeof(int32_t)) {
        HOST_Log("%s len:0x%x\n", __func__, recv_len);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

    itr = len / MSG_BUF_SIZE;
    if (0UL != (len % MSG_BUF_SIZE)) {
        itr++;
    }

    offset = 0UL;
    for (i = 0UL; i < itr; i++) {
        memset(sendMsgBuf, 0, MSG_BUF_SIZE);

        cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
        /* copy image chunk */
        cpy_sz = imgLen - offset;

        if (cpy_sz > MSG_BUF_SIZE) {
            cpy_sz = MSG_BUF_SIZE;
        }

        memcpy(sendMsgBuf, &image[offset], cpy_sz);

        retVal = HIPC_SendRecv(cmdId, sendMsgBuf,
                cpy_sz, sendMsgBuf, MSG_BUF_SIZE, &recv_len);

        if (retVal != BCM_ERR_OK) {
            goto done;
        } else if (BCM_ERR_OK != uswap32(*((int32_t *)sendMsgBuf))) {
            HOST_Log("%s status:%d\n", __func__, uswap32(*((int32_t *)sendMsgBuf)));
            retVal = BCM_ERR_UNKNOWN;
            goto done;
        } else if (recv_len != sizeof(int32_t)) {
            HOST_Log("%s len:0x%x\n", __func__, recv_len);
            retVal = BCM_ERR_DATA_INTEG;
            goto done;
        }
        offset += cpy_sz;
    }
    HOST_Log("transfer_image: done\n");

done:
    return retVal;
}

static int32_t service_image_fetch_request(const IMGL_ImgHdrType *const fetchImageHdr)
{
    uint32_t i = 0UL;
    int32_t retVal = -1;

    HOST_Log("fetch request: pid: %x imgID: %x\n", fetchImageHdr->pid, fetchImageHdr->imgID);
    for (i = 0UL; i < parsedTarImages.numImages; i++) {
        if (COMPARE_IMG_HDRS_AND_SIZE(fetchImageHdr, i)) {
            break;
        }
    }

    if (parsedTarImages.numImages == i) {
        retVal = BCM_ERR_INVAL_PARAMS;
        HOST_Log("%s:%d status:%d %d %d %d\n", __func__, __LINE__,
            fetchImageHdr->pid, fetchImageHdr->imgID, fetchImageHdr->imgSize,
            fetchImageHdr->isImgHdr);
        retVal = transfer_image(NULL, 0UL);
    } else {
        retVal = transfer_image(parsedTarImages.baseAddr + parsedTarImages.imgOffsets[i], fetchImageHdr->imgSize);
    }

    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s:%d status:%d\n", __func__, __LINE__, retVal);
        goto exit;
    }

exit:
    return retVal;
}


extern int32_t HOST_ImglNotificationHandler(uint32_t currentSlave,
                BCM_CompIDType comp, uint32_t notificationId,
                uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL == msg) {
        goto done;
    }

    switch (notificationId) {
    case IMGL_CMDID_IMAGE_FETCH:
        if (sizeof(IMGL_ImgHdrType) == size) {
            IMGL_ImgHdrType hdr;
            IMGL_ImgHdrType *tmp = (IMGL_ImgHdrType*)msg;
            hdr.isImgHdr = tmp->isImgHdr;
            hdr.pid = tmp->pid;
            hdr.imgID = uswap16(tmp->imgID);
            hdr.imgSize = uswap32(tmp->imgSize);
            retVal = service_image_fetch_request(&hdr);
        }
        break;
    default:
        goto done;
    }

done:
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s failed. SPI-Id:%u comp:0x%x notificationId:0x%x msg:%p size=%d err=%d\n",
            __func__, currentSlave, comp, notificationId, msg, size, retVal);
    }

    return retVal;
}

int32_t HOST_ImglDeInit()
{
    parsedTarImages.isInitialized = 0UL;
    return BCM_ERR_OK;
}
