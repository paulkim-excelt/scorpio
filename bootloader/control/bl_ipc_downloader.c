/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
#include <string.h>
#include <bl_bcm_err.h>
#include <bl_downloader.h>
#include <bl_ptm.h>
#include <bl_cntrl.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <bl_utils.h>
#include <bl_edc.h>
#include <bl_board.h>
#include <ipc.h>
#include <ipc_osil.h>
#include <bcm_time.h>
#include <mcu.h>
#include <otp.h>
#include <rpc_cmds.h>
#include <imgl_ipc_cmds.h>
#include <sys_ipc_cmds.h>
#include <ipc_hwif.h>

#ifdef BL_ENABLE_ETHSWT
#include <etherswt_ipc.h>
#include <bl_switch_access.h>
#endif /* BL_ENABLE_ETHSWT */

#ifdef BL_ENABLE_DBGMEM
#include <dbgmem.h>
#endif /* BL_ENABLE_DBGMEM */

/**
    @name IPC downloader API IDs
    @{
    @brief API IDs for IPC downloader
*/
#define BRCM_DSGN_BL_IPC_DWNLD_REPORT_ERROR_PROC          (0xA0U) /**< @brief #BL_IPC_DWNLD_ReportError */
/** @} */

#define BL_IPC_DWNLD_SEND_DELAY_US      (5UL)
#define BL_IPC_DWNLD_SEND_RETRIES       (700)
#define BL_IPC_DWNLD_RECV_DELAY_US      (1UL)

int32_t BL_CNTRL_ReadOtpMacAddr(uint32_t aLoc,
                                uint32_t *const aMacOct123,
                                uint32_t *const aMacOct456);


#define BL_JUMP_TO(addr) asm ("bx %[jump]" : : [jump]"r"(addr):)
#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)

#define BL_REG_READ16(aAddr)       (*((const volatile uint16_t *)(aAddr)))
#define BL_REG_WRITE16(aAddr, aVal)    \
    (*((volatile uint16_t *)(aAddr)) = (uint16_t)(aVal))

#define BL_MAC_ADDR_PACK(aDest, aSrc1, aSrc2)         \
        (aDest)[0UL] = (aSrc1 >> 16UL) & 0xFFUL;    \
        (aDest)[1UL] = (aSrc1 >> 8UL) & 0xFFUL;     \
        (aDest)[2UL] = aSrc1 & 0xFFUL;              \
        (aDest)[3UL] = (aSrc2 >> 16UL) & 0xFFUL;    \
        (aDest)[4UL] = (aSrc2 >> 8UL) & 0xFFUL;     \
        (aDest)[5UL] = aSrc2 & 0xFFUL;

#define BL_MAC_ADDR_UNPACK(aDest1, aDest2, aSrc)         \
        aDest1 = (((uint32_t)((aSrc)[0UL]) & 0xFFUL) << 16UL);  \
        aDest1 |= (((uint32_t)((aSrc)[1UL]) & 0xFFUL) << 8UL);  \
        aDest1 |= ((uint32_t)((aSrc)[2UL]) & 0xFFUL);           \
        aDest2 = (((uint32_t)((aSrc)[3UL]) & 0xFFUL) << 16UL);  \
        aDest2 |= (((uint32_t)((aSrc)[4UL]) & 0xFFUL) << 8UL);  \
        aDest2 |= ((uint32_t)((aSrc)[5UL]) & 0xFFUL);


extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];

typedef void (*BL_IPC_DWNLD_ImgEntryFuncType)(void);

#define BL_IPC_MSG_LOC_BUF_SIZE    (512UL)
static uint8_t BL_IPC_DWNLD_Buf[BL_IPC_MSG_LOC_BUF_SIZE];
static uint32_t BL_DwnldImgOffset = 0UL;
static uint32_t BL_DwnldImgTotalSize = 0UL;
static IPC_ChannIDType BL_DwnldIpcChann = 0UL;

typedef struct {
    uint32_t size;
    uint32_t addr;
    uint8_t  data[];
} BL_IPC_DWNLD_OtpInfoType;

typedef struct {
    uint32_t status;
    uint32_t size;
    uint32_t addr;
    uint8_t  data[];
} BL_IPC_DWNLD_OtpRespType;

static int32_t BL_IPC_DWNLD_RecvImageHdr(BL_DWNLD_ImgHdrType *aOut,
                                        BL_DWNLD_ImgHdrType *const aImgHdr,
                                        uint32_t aLen);
static int32_t BL_IPC_DWNLD_RecvImage(uint8_t *aOut,
                                        uint8_t *const aBuf,
                                        uint32_t aLen);

static BL_COMP_NEVER_INLINE void BL_IPC_DWNLD_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_IPC_DWNLD_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static int32_t BL_IPC_DWNLD_TrySend(IPC_ChannIDType aID, uint32_t aCmd, uint8_t *const aBuf, uint32_t aSize)
{
    int32_t ret = BL_BCM_ERR_NOMEM;
    uint32_t retries = BL_IPC_DWNLD_SEND_RETRIES;

    while ((retries-- > 0UL) && (ret == BL_BCM_ERR_NOMEM)) {
        ret = IPC_Send(aID, aCmd, aBuf, aSize);
        BCM_DelayUs(BL_IPC_DWNLD_SEND_DELAY_US);
    }
    return ret;
}

static int32_t BL_IPC_Receive_Block(uint32_t *const aCmd,
                            uint8_t *const aMsg,
                            uint32_t *const aLen,
                            uint32_t aTimeoutMs)
{
    uint32_t timeout = aTimeoutMs * 1000UL;
    int32_t ret = IPC_Receive(BL_DwnldIpcChann, aCmd, aMsg, BL_IPC_MSG_LOC_BUF_SIZE, aLen);

    while ((BL_BCM_ERR_NOMEM == ret) && (0U != aTimeoutMs)) {
        BCM_DelayUs(BL_IPC_DWNLD_RECV_DELAY_US); /* 1us */
        timeout--;
        ret = IPC_Receive(BL_DwnldIpcChann, aCmd, aMsg, BL_IPC_MSG_LOC_BUF_SIZE, aLen);
    }
    return ret;
}

static void BL_IPC_DWNLD_UpdateStatus(uint32_t aPercent)
{
    uint16_t regVal;

    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_IPC_DWNLD_FLASHING_PERCENT_MASK;
    regVal |= (((uint16_t)aPercent << BL_IPC_DWNLD_FLASHING_PERCENT_SHIFT)
               & BL_IPC_DWNLD_FLASHING_PERCENT_MASK);
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
}

static int32_t BL_IPC_DWNLD_ValidateImgEDC(BL_DWNLD_ImgHdrType *aDwnldImgHdr, uint8_t * const aImg)
{
    int32_t err;
    if (PTBL_ERR_DET_CRC == (aDwnldImgHdr->flags & PTBL_ERR_DET_CRC)) {
        err = BL_EDC_ValidateCRC(aImg, aDwnldImgHdr->imgSize, aDwnldImgHdr->errDetCode);
    } else if (PTBL_ERR_DET_CHECKSUM == (aDwnldImgHdr->flags & PTBL_ERR_DET_CHECKSUM)) {
        err = BL_EDC_ValidateChcksm(aImg, aDwnldImgHdr->imgSize, aDwnldImgHdr->errDetCode);
    } else {
        err = BL_BCM_ERR_OK;
    }
    return err;
}

static int32_t BL_IPC_DWNLD_EraseFlash(BL_FLASH_HwIDType aFlashID)
{
    uint32_t i;
    uint32_t sectorSize = 0UL;
    uint32_t sectors = 0UL;
    uint32_t addr;
    uint32_t percent = 0UL;
    int32_t err = BL_BCM_ERR_OK;

    BL_LOG_ERR("Erase flash(%d)..\n", aFlashID);
    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (aFlashID == BL_FLASH_Config[i].hwID) {
            sectorSize = BL_FLASH_Config[i].sectorSize;
            sectors = BL_FLASH_Config[i].size / sectorSize;
            break;
        }
    }

    if (0UL == sectors) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    for (i = 0UL; i < sectors; i++) {
        addr = i * sectorSize;
        err = BL_FLASH_EraseSector(aFlashID, addr, sectorSize);
        if (BL_BCM_ERR_OK != err) {
            break;
        }
        percent = ((i + 1UL) * 100UL) / sectors;
        BL_IPC_DWNLD_UpdateStatus(percent);
    }
    BL_LOG_ERR("Erase flash(%d) done\n", aFlashID);
exit:
    return err;
}

#define BL_READ_BUF_SIZE     (256UL)
uint8_t BL_ReadData[BL_READ_BUF_SIZE];

/**
 * Write data to given flash
 * This function is added for the failsafe testing purpose only.
 * This function clear the bits @ aAddr (for 4bytes).
 */
static int32_t BL_IPC_DWNLD_WriteFlash(BL_FLASH_HwIDType aFlashID,
                                       uint32_t aAddr)
{
    uint32_t i;
    uint8_t data[4] = {0};
    uint32_t pageAddr, offset, pageSize;
    int32_t err = BL_BCM_ERR_UNKNOWN;
    uint32_t lineNo = __LINE__;

    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (aFlashID == BL_FLASH_Config[i].hwID) {
            pageSize = BL_FLASH_Config[i].pageSize;
            if (BL_READ_BUF_SIZE < pageSize) {
                lineNo = __LINE__;
                err = BL_BCM_ERR_NOMEM;
                break;
            }
            pageAddr = (aAddr / pageSize) * pageSize;
            offset = (aAddr % pageSize);
            BL_LOG_ERR("pageAddr:0x%x offset:0x%x\n", pageAddr, offset);
            err = BL_FLASH_Read(aFlashID, pageAddr, BL_ReadData, pageSize);
            if (BL_BCM_ERR_OK == err) {
                if (0L != BL_BCM_MemCmp(&BL_ReadData[offset], data, 4UL)) {
                    BL_LOG_ERR("flash(%d) write 0x0 @address:0x%x\n",
                            aFlashID, aAddr);

                    BL_BCM_MemSet(&BL_ReadData[offset], 0x0L, 4UL);
                    err = BL_FLASH_Write(aFlashID, pageAddr, BL_ReadData,
                            pageSize);
                }
            } else {
                lineNo = __LINE__;
            }
            break;
        }
    }

    if (i == BL_FLASH_CfgSize) {
        lineNo = __LINE__;
        err = BL_BCM_ERR_INVAL_PARAMS;
    }

    if (BL_BCM_ERR_OK != err) {
        BL_IPC_DWNLD_ReportError(err, aFlashID, 0UL, 0UL, 0UL, lineNo);
    }

    return err;
}

static int32_t BL_IPC_DWNLD_FetchImage(const IMGL_ImgHdrType *const aPwImgHdr,
                                        uint8_t *aImg)
{
    uint32_t len = 0UL;
    uint32_t ipcCmd = 0UL;
    uint32_t lineNo = __LINE__;
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t *bufPtr32 = (uint32_t *)BL_IPC_DWNLD_Buf;
    BL_DWNLD_ImgHdrType dwnldImgHdr;

    const uint32_t recvTimeout = 8000UL; /* 8sec */
    const uint32_t imgFetchIpcCmd =
        (RPC_CMD_ASYNC_MASK) |
        (IMGL_CMDID_IMAGE_FETCH << RPC_CMD_ID_SHIFT) |
        (BCM_GROUPID_IMGL << RPC_CMD_GROUP_SHIFT);

#if defined(__BCM8953X__)
    if ((NULL == aPwImgHdr)) {
#else
    if ((NULL == aPwImgHdr) || (NULL == aImg)) {
#endif
        ret = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto exit;
    }

    /* Send a fetch request */
    BL_BCM_MemCpy(BL_IPC_DWNLD_Buf, aPwImgHdr, sizeof(IMGL_ImgHdrType));
    ret = BL_IPC_DWNLD_TrySend(BL_DwnldIpcChann, imgFetchIpcCmd, BL_IPC_DWNLD_Buf, sizeof(IMGL_ImgHdrType));
    if (BL_BCM_ERR_OK != ret) {
        lineNo = __LINE__;
        goto exit;
    }

    /*
        Now we expect the client to send a sequence of IPC commands to send us the image.
        Collect them, and respond appropriately.
    */

    /* We expect to see a DWNLD_IMG_HDR command at this point */
    ret = BL_IPC_Receive_Block(&ipcCmd, BL_IPC_DWNLD_Buf, &len, recvTimeout);
    if ((BL_BCM_ERR_OK != ret) ||
        (BCM_GROUPID_IMGL != RPC_GET_GROUPID(ipcCmd)) ||
        (IMGL_CMDID_DWNLD_IMG_HDR != RPC_GET_CMDID(ipcCmd))) {
        lineNo = __LINE__;
        ret = BL_BCM_ERR_INVAL_STATE;
        goto exit;
    }

    /* Recieve the header, send ack. */
    ret = BL_IPC_DWNLD_RecvImageHdr(&dwnldImgHdr, (BL_DWNLD_ImgHdrType*)BL_IPC_DWNLD_Buf, len);
    *bufPtr32 = ret;

    if (BL_BCM_ERR_OK != ret) {
        BL_IPC_DWNLD_TrySend(BL_DwnldIpcChann, ipcCmd, BL_IPC_DWNLD_Buf, sizeof(uint32_t));
        lineNo = __LINE__;
        goto exit;
    } else {
        ret = BL_IPC_DWNLD_TrySend(BL_DwnldIpcChann, ipcCmd, BL_IPC_DWNLD_Buf, sizeof(uint32_t));
        if (BL_BCM_ERR_OK != ret) {
            lineNo = __LINE__;
            goto exit;
        }
    }

    /* Expect to see a sequence of DWNLD_IMG commands from the client */
    while (BL_DwnldImgOffset != aPwImgHdr->imgSize) {
        ret = BL_IPC_Receive_Block(&ipcCmd, BL_IPC_DWNLD_Buf, &len, recvTimeout);
        if ((BL_BCM_ERR_OK != ret) ||
            (BCM_GROUPID_IMGL != RPC_GET_GROUPID(ipcCmd)) ||
            (IMGL_CMDID_DWNLD_IMG != RPC_GET_CMDID(ipcCmd))) {
            lineNo = __LINE__;
            goto exit;
        }

        /* Recieve the image, send ack. */
        ret = BL_IPC_DWNLD_RecvImage(aImg, BL_IPC_DWNLD_Buf, len);
        *bufPtr32 = ret;

        if (BL_BCM_ERR_OK != ret) {
            BL_IPC_DWNLD_TrySend(BL_DwnldIpcChann, ipcCmd, BL_IPC_DWNLD_Buf, sizeof(uint32_t));
            lineNo = __LINE__;
            goto exit;
        } else {
              ret = BL_IPC_DWNLD_TrySend(BL_DwnldIpcChann, ipcCmd, BL_IPC_DWNLD_Buf, sizeof(uint32_t));
            if (BL_BCM_ERR_OK != ret) {
                lineNo = __LINE__;
                goto exit;
            }
        }

    }

    /* Validate CRC */
    ret = BL_IPC_DWNLD_ValidateImgEDC(&dwnldImgHdr, aImg);
    if (BL_BCM_ERR_OK != ret) {
        lineNo = __LINE__;
        goto exit;
    }

    /* All good */
    ret = BL_BCM_ERR_OK;

exit:
    if (BL_BCM_ERR_OK != ret) {
        BL_IPC_DWNLD_ReportError(ret, 0UL, 0UL, 0UL, 0UL, lineNo);
    }

    return ret;
}

static int32_t BL_IPC_DWNLD_WriteImg(SYS_UpdateExecCmdType *const aFlsInfo)
{
    int32_t err;
    uint32_t lineNo = __LINE__;

    if (BL_FLASH_MAX_HW_ID <= aFlsInfo->flashId) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        goto err;
    }

    err = BL_CNTRL_UpdateFlashPiecewise(aFlsInfo->flashId, aFlsInfo->numImgs,
                    aFlsInfo->imgHdr, BL_IPC_DWNLD_FetchImage, BL_DWNLD_StartAddr,
                    BL_IPC_DWNLD_UpdateStatus);

err:
    if (BL_BCM_ERR_OK == err) {
        BL_LOG_ERR("Image flash successful\n");
    } else {
        BL_IPC_DWNLD_ReportError(err, aFlsInfo->flashId, 0UL, 0UL, 0UL, lineNo);
    }

    return err;
}

static int32_t BL_IPC_DWNLD_ExecuteImg(SYS_UpdateExecCmdType *const aFlsInfo)
{
#ifdef BL_ENABLE_FLASHLESS_BOOT
    return BL_CNTRL_ExecuteFW(aFlsInfo, BL_IPC_DWNLD_FetchImage);
#else
    return BL_BCM_ERR_NOSUPPORT;
#endif
}

static int32_t BL_IPC_DWNLD_ApplyXcvrCfg(SYS_UpdateExecCmdType *const aFlsInfo)
{
    ETHXCVR_ConfigType xcvrConfig;
    uint32_t imageCount = aFlsInfo->numImgs;
    IMGL_ImgHdrType *imgHdr = aFlsInfo->imgHdr;
    uint32_t i;
    int32_t err = BCM_ERR_OK;

    for (i = 0UL; i < imageCount; i++) {
        /* fetch the XCVR image apply first */
        if ((PTBL_ID_SYSCFG == imgHdr[i].pid) && (0x9000 == imgHdr[i].imgID)) {
            err = BL_IPC_DWNLD_FetchImage(&imgHdr[i], (uint8_t*)&xcvrConfig);
            if (BL_BCM_ERR_OK == err) {
                BL_CNTRL_XcvrInit(&xcvrConfig);
                break;
            }
        }
    }
    if (imageCount == i) {
        err = BL_BCM_ERR_NOT_FOUND;
    }
    return err;
}

static int32_t BL_IPC_DWNLD_OTPCmdHandler(uint32_t aCmd,
                                       uint8_t *const aBufOut,
                                       uint32_t aBufOutLen,
                                       uint8_t *const aBufIn,
                                       uint32_t aBufInLen)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t i;
    uint32_t data, data2;
    uint32_t row, row2;
    uint32_t startRow;
    uint32_t endRow;
    uint32_t keyBytes = BL_OTP_SECURE_KEY_SIZE;
    uint32_t offset = 0UL;
    uint32_t bytesPerRow = BL_RSA_KEY_BYTES_PER_ROW;
    BL_IPC_DWNLD_OtpInfoType * info;
    BL_IPC_DWNLD_OtpRespType *resp;
    uint32_t lineNo = __LINE__;
    uint32_t *temp = NULL;

    switch (aCmd) {
    case SYS_CMDID_OTP_RD:
        if ((NULL == aBufOut) || (0UL == aBufOutLen)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        info = (BL_IPC_DWNLD_OtpInfoType *) aBufOut;
        resp = (BL_IPC_DWNLD_OtpRespType *) aBufOut;
        err = OTP_Read(BL_OTP_HW_ID_0, info->addr, &data);
        resp->data[0UL] = data & 0xFFUL;
        resp->data[1UL] = (data >> 8UL) & 0xFFUL;
        resp->data[2UL] = (data >> 16UL) & 0xFFUL;
        resp->data[3UL] = (data >> 24UL) & 0xFFUL;
        lineNo = __LINE__;
        break;
    case SYS_CMDID_OTP_WR_SEC_KEY:
        if ((NULL == aBufIn) || (0UL == aBufInLen)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        info = (BL_IPC_DWNLD_OtpInfoType *) aBufIn;
        if (BL_OTP_SECURE_KEY_SIZE != info->size) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        row = BL_OTP_ADDR_RSA_PUB_START;
        while (keyBytes > 0UL) {
            data = 0UL;
            if (keyBytes < BL_RSA_KEY_BYTES_PER_ROW) {
                bytesPerRow = keyBytes;
            }
            for (i = 0UL; i < bytesPerRow; i++) {
                data |= (info->data[offset] << (i * 8UL));
                offset++;
                keyBytes--;
            }
            err = OTP_Write(BL_OTP_HW_ID_0, row, data);
            if (err != BL_BCM_ERR_OK) {
                break;
            }
            row++;
        }
        lineNo = __LINE__;
        break;
    case SYS_CMDID_OTP_ENABLE_SEC:
        err = OTP_Write(BL_OTP_HW_ID_0, BL_OTP_SEC_BOOT_EN_ROW,
                BL_OTP_SEC_BOOT_EN_MASK);
        lineNo = __LINE__;
        break;
    case SYS_CMDID_OTP_WRITE_CUST:
        if ((NULL == aBufIn) || (0UL == aBufInLen)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        info = (BL_IPC_DWNLD_OtpInfoType *) aBufIn;
        data = (uint32_t)(info->data[0UL]) & 0xFFUL;
        data |= (((uint32_t)(info->data[1UL]) & 0xFFUL) << 8UL);
        data |= (((uint32_t)(info->data[2UL]) & 0xFFUL) << 16UL);
        data |= (((uint32_t)(info->data[3UL]) & 0xFFUL) << 24UL);
        startRow = BL_OTP_ADDR_RSA_PUB_START;
        endRow = startRow + (keyBytes / BL_RSA_KEY_BYTES_PER_ROW);
        row = info->addr;
        if ((row < BL_OTP_ADDR_BRCM_REG_START) ||
                ((row >= startRow) && (row <= endRow)) ||
                (row == BL_OTP_SEC_BOOT_EN_ROW)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        err = OTP_Write(BL_OTP_HW_ID_0, row, data);
        lineNo = __LINE__;
        break;
    case SYS_CMDID_OTP_RD_MAC_ADDR:
        if ((NULL == aBufOut) || (0UL == aBufOutLen)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }
        resp = (BL_IPC_DWNLD_OtpRespType *) aBufOut;
        temp = ((uint32_t *)resp->data);
        *temp = 0x3UL;

        /* Read MAC address copy 0 */
        err = BL_CNTRL_ReadOtpMacAddr(0UL, &data, &data2);
        if ((BL_BCM_ERR_OK != err) && (BL_BCM_ERR_DATA_INTEG != err)
                && (BL_BCM_ERR_NOT_FOUND != err)) {
            lineNo = __LINE__;
            break;
        }
        if (BL_BCM_ERR_DATA_INTEG == err) {
            *temp = (*temp) & (~0x1UL);
            lineNo = __LINE__;
        }

        BL_MAC_ADDR_PACK((&(resp->data[4UL])), data, data2);

        /* Read MAC address copy 1 */
        err = BL_CNTRL_ReadOtpMacAddr(1UL, &data, &data2);
        if ((BL_BCM_ERR_OK != err) && (BL_BCM_ERR_DATA_INTEG != err)
                && (BL_BCM_ERR_NOT_FOUND != err)) {
            lineNo = __LINE__;
            break;
        }
        if (BL_BCM_ERR_DATA_INTEG == err) {
            *temp = (*temp) & (~0x2UL);
            lineNo = __LINE__;
        }

        BL_MAC_ADDR_PACK((&(resp->data[10UL])), data, data2);

        resp->size = 16UL;
        break;
    case SYS_CMDID_OTP_WR_MAC_ADDR:
        if ((NULL == aBufIn) || (0UL == aBufInLen)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }

        info = (BL_IPC_DWNLD_OtpInfoType *) aBufIn;

        BL_MAC_ADDR_UNPACK(data, data2, (&info->data[0UL]));

        /* Broadcast, multicast & universal administered MAC addressess
         * are not allowed */
        if (((0xFFFFFFUL == (data & 0xFFFFFFUL))
                    && (0xFFFFFFUL == (data2 & 0xFFFFFFUL)))
                || ((data & (0x1 << 16UL)) != 0UL)
                || ((data & (0x1 << 17UL)) == 0UL)
                || ((0x0UL == (data & 0xFFFFFFUL))
                    && (0x0UL == (data2 & 0xFFFFFFUL)))) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }

        if (0UL == info->addr) {
            row = BL_OTP_MAC_ADDR_0_OCTET123_ADDR;
            row2 = BL_OTP_MAC_ADDR_0_OCTET456_ADDR;
        } else if (1UL == info->addr) {
            row = BL_OTP_MAC_ADDR_1_OCTET123_ADDR;
            row2 = BL_OTP_MAC_ADDR_1_OCTET456_ADDR;
        } else {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
            break;
        }

        err = BL_CNTRL_ReadOtpMacAddr(info->addr, &data, &data2);
        if (BL_BCM_ERR_NOT_FOUND == err) {
            BL_MAC_ADDR_UNPACK(data, data2, (&info->data[0UL]));
            BL_LOG_ERR("Writing MAC address @loc-%d: %02x:%02x:%02x:%02x:%02x:%02x\n",
                    info->addr,
                    ((data >> 16UL) & 0xFFUL),
                    ((data >> 8UL) & 0xFFUL),
                    (data & 0xFFUL),
                    ((data2 >> 16UL) & 0xFFUL),
                    ((data2 >> 8UL) & 0xFFUL),
                    (data2 & 0xFFUL));
            err = OTP_Write(BL_OTP_HW_ID_0, row, data);
            if (BL_BCM_ERR_OK != err) {
                lineNo = __LINE__;
                break;
            }

            err = OTP_Write(BL_OTP_HW_ID_0, row2, data2);
            if (BL_BCM_ERR_OK != err) {
                lineNo = __LINE__;
            }
        } else if (BL_BCM_ERR_OK == err) {
            BL_LOG_ERR("Can't over-write MAC address\n");
            err = BL_BCM_ERR_NOPERM;
        } else {
            lineNo = __LINE__;
        }
        break;
    default:
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        break;
    }
    if (BL_BCM_ERR_OK != err) {
        BL_IPC_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    }

    return err;
}

static int32_t BL_IPC_DWNLD_RecvImageHdr(BL_DWNLD_ImgHdrType *aOut,
                                        BL_DWNLD_ImgHdrType *const aImgHdr,
                                        uint32_t aLen)
{
    int32_t err;

    /* Validate img hdr */
    if (aLen != sizeof(BL_DWNLD_ImgHdrType)) {
        err = BL_BCM_ERR_INVAL_PARAMS;
    } else if (ITBL_MAGIC != aImgHdr->magicNum) {
        err = BL_BCM_ERR_INVAL_MAGIC;
    } else if (aImgHdr->imgSize > (BL_DWNLD_EndAddr - BL_DWNLD_StartAddr)) {
        err = BL_BCM_ERR_NOMEM;
    } else {
        BL_BCM_MemCpy(aOut, aImgHdr, sizeof(BL_DWNLD_ImgHdrType));
        BL_DwnldImgOffset = 0UL;
        BL_DwnldImgTotalSize = aImgHdr->imgSize;
        err = BL_BCM_ERR_OK;
    }

    if (BL_BCM_ERR_OK != err) {
        BL_IPC_DWNLD_ReportError(err, 0UL, aLen, 0UL, 0UL, __LINE__);
    }

    return err;
}

static int32_t BL_IPC_DWNLD_RecvImage(uint8_t *aOut,
                                        uint8_t *const aBuf,
                                        uint32_t aLen)
{
    int32_t err = BL_BCM_ERR_OK;
    uint8_t *img = (uint8_t *)aOut + BL_DwnldImgOffset;

    if (BL_DwnldImgTotalSize < (BL_DwnldImgOffset + aLen)) {
        /* Image size overflow */
        BL_IPC_DWNLD_ReportError(err, 0UL, BL_DwnldImgTotalSize,
                (BL_DwnldImgOffset + aLen), 0UL, __LINE__);
        err = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        BL_BCM_MemCpy(img, aBuf, aLen);
        BL_DwnldImgOffset += aLen;
    }

    return err;
}

static int32_t BL_IPC_DWNLD_ProcessBlCmd(uint32_t aCmd,
                                       SYS_UpdateExecCmdType *const aFlsInfo)
{
    int32_t err;
    uint16_t status;
    uint16_t regVal;
    uint32_t lineNo = __LINE__;

    /* clear the percentage field since a new command is received*/
    BL_IPC_DWNLD_UpdateStatus(0UL);
    if (SYS_CMDID_EXECUTE == aCmd) {
        err = BL_IPC_DWNLD_ExecuteImg(aFlsInfo);
        lineNo = __LINE__;
    } else if (SYS_CMDID_APPLY_XCVR_CFG == aCmd) {
        err = BL_IPC_DWNLD_ApplyXcvrCfg(aFlsInfo);
    } else if (SYS_CMDID_FLASH_UPDATE == aCmd) {
        err = BL_IPC_DWNLD_WriteImg(aFlsInfo);
        lineNo = __LINE__;
    } else if (SYS_CMDID_FLASH_ERASE == aCmd) {
        err = BL_IPC_DWNLD_EraseFlash(aFlsInfo->flashId);
        lineNo = __LINE__;
    } else if (SYS_CMDID_FLASH_WRITE == aCmd) {
        err = BL_IPC_DWNLD_WriteFlash(aFlsInfo->flashId,
                                aFlsInfo->flashAddr);
        lineNo = __LINE__;
    } else {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
    }
    if (BL_BCM_ERR_OK == err) {
        status = BL_IPC_DWNLD_STATUS_SUCCESS;
    } else {
        status = BL_IPC_DWNLD_STATUS_FAILURE;
    }
    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_IPC_DWNLD_STATUS_MASK;
    regVal |= ((status << BL_IPC_DWNLD_STATUS_SHIFT)
            & BL_IPC_DWNLD_STATUS_MASK);
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);

    if (BL_BCM_ERR_OK != err) {
        BL_IPC_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    }
    return err;
}

static int32_t BL_IPC_DWNLD_ProcessCommonCmd(uint32_t aCmd,
                                             BL_IPC_RebootType *const aRebootInfo)
{
    int32_t err;
    uint16_t status;
    uint16_t regVal;
    uint32_t lineNo = __LINE__;

    /* clear the percentage field since a new command is received*/
    BL_IPC_DWNLD_UpdateStatus(0UL);
    if (SYS_CMDID_REBOOT == aCmd) {
        /* Delay is needed for the HOST to read the IPC response */
        /* for reboot command. Since we do a global reboot here  */
        /* all spare registers would get reset due to which the  */
        /* ipc_receive call at the HOST would fail. Delay        */
        /* specified by the HOST will be used here               */
        BCM_DelayUs(aRebootInfo->delayMs * 1000UL);
        if (BL_IPC_RESETMODE_CHIP == aRebootInfo->resetMode) {
            MCU_ResetReq(MCU_RESETREQ_GLOBAL);
            err = BL_BCM_ERR_OK;
            lineNo = __LINE__;
#if 0 /* NO support to reboot from BL and enter into BL mode again */
        } else if (BL_IPC_RESETMODE_CPU == aRebootInfo->resetMode) {
#if defined(__BCM89559_EVK__) || defined (__BCM89559G_EVK__)
            MCU_ResetReq(MCU_RESETREQ_LOCAL);
#endif
            err = BL_BCM_ERR_OK;
            lineNo = __LINE__;
#endif
        } else {
            err = BL_BCM_ERR_INVAL_PARAMS;
            lineNo = __LINE__;
        }
    } else {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
    }
    if (BL_BCM_ERR_OK == err) {
        status = BL_IPC_DWNLD_STATUS_SUCCESS;
    } else {
        status = BL_IPC_DWNLD_STATUS_FAILURE;
    }
    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_IPC_DWNLD_STATUS_MASK;
    regVal |= ((status << BL_IPC_DWNLD_STATUS_SHIFT)
            & BL_IPC_DWNLD_STATUS_MASK);
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);

    if (BL_BCM_ERR_OK != err) {
        BL_IPC_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    }
    return err;
}

void BL_IPC_DWNLD_Init(MCU_DwnldModeType aDwnldMode)
{
    uint16_t regVal;
    uint32_t recvCmd = 0UL;
    uint32_t recvCmdGrp = 0UL;
    uint32_t cmd = 0UL;
    int32_t err;
    int32_t ret;
    uint32_t len;
    uint32_t lineNo;
    uint32_t errInfo1, errInfo2;
    uint32_t *bufPtr32 = (uint32_t *)BL_IPC_DWNLD_Buf;
    uint32_t replySize;
    SYS_UpdateExecCmdType flsInfo;
    BL_IPC_RebootType rebootInfo;

#ifdef BL_ENABLE_ETHSWT
    ETHERSWT_RegAccessType ethSwtAccess;
#endif /* BL_ENABLE_ETHSWT */

#ifdef BL_ENABLE_DBGMEM
    SYS_MemAccessType memAccess;
#endif /* BL_ENABLE_DBGMEM */

    uint32_t i;
    IPC_ChannIDType channID;

    /* Initialize the slave IPC channel for download
     * to communicate with host
     */
    for (i = 0; i < IPC_MAX_CHANNELS; i++) {
        if (IPC_CHANN_MODE_SLAVE == IPC_ChannCfg[i].mode) {
            break;
        }
    }

    if (IPC_MAX_CHANNELS == i) {
        BL_IPC_DWNLD_ReportError(BL_BCM_ERR_INVAL_PARAMS, 0UL, IPC_MAX_CHANNELS,
                IPC_ChannCfg[0].mode, 0UL, __LINE__);
        while (1) { }
    }

    channID = IPC_ChannCfg[i].ID;
    BL_DwnldIpcChann = channID;
    IPC_Init(channID);

    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal |= BL_IPC_DWNLD_BL_READY_MASK;
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);

    while (1UL) {
        errInfo1 = 0UL;
        errInfo2 = 0UL;
        lineNo = __LINE__;
        len = BL_IPC_MSG_LOC_BUF_SIZE;
        replySize = sizeof(uint32_t);
        /* Wait for command */
        err = IPC_Receive(channID, &recvCmd, BL_IPC_DWNLD_Buf, BL_IPC_MSG_LOC_BUF_SIZE, &len);
        if (BL_BCM_ERR_OK == err) {
            recvCmdGrp = RPC_GET_GROUPID(recvCmd);
            cmd = RPC_GET_CMDID(recvCmd);
            if (BCM_GROUPID_SYS == recvCmdGrp) {
                if ((SYS_CMDID_FLASH_UPDATE == cmd)
                        || (SYS_CMDID_EXECUTE == cmd)
                        || (SYS_CMDID_FLASH_WRITE == cmd)
                        || (SYS_CMDID_FLASH_ERASE == cmd)
                        || (SYS_CMDID_APPLY_XCVR_CFG == cmd)) {
                    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
                    regVal &= ~BL_IPC_DWNLD_STATUS_MASK;
                    regVal |= ((BL_IPC_DWNLD_STATUS_BUSY << BL_IPC_DWNLD_STATUS_SHIFT)
                            & BL_IPC_DWNLD_STATUS_MASK);
                    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);

                    BL_BCM_MemCpy(&flsInfo, BL_IPC_DWNLD_Buf,
                            sizeof(SYS_UpdateExecCmdType));
                    err = BL_IPC_DWNLD_ProcessBlCmd(cmd, &flsInfo);
                } else if (SYS_CMDID_PING == cmd) {
                    err = BL_BCM_ERR_OK;
                    lineNo = __LINE__;
                    bufPtr32[1UL] = BL_PING_MODE_BL;
                    MCU_GetVersion((MCU_VersionType *)&bufPtr32[2UL]);
                    replySize = 24UL;
                } else if (SYS_CMDID_OTP_RD == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, BL_IPC_DWNLD_Buf,
                            sizeof(BL_IPC_DWNLD_Buf), NULL, 0UL);
                    replySize = 16UL;
                    lineNo = __LINE__;
                } else if (SYS_CMDID_OTP_WR_SEC_KEY == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, NULL, 0UL,
                            BL_IPC_DWNLD_Buf, len);
                    lineNo = __LINE__;
                } else if (SYS_CMDID_OTP_ENABLE_SEC == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, NULL, 0UL,
                            NULL, 0UL);
                    lineNo = __LINE__;
                } else if (SYS_CMDID_OTP_WRITE_CUST == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, NULL, 0UL,
                            BL_IPC_DWNLD_Buf, len);
                    lineNo = __LINE__;
                } else if (SYS_CMDID_OTP_WR_MAC_ADDR == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, NULL, 0UL,
                            BL_IPC_DWNLD_Buf, len);
                    lineNo = __LINE__;
                } else if (SYS_CMDID_OTP_RD_MAC_ADDR == cmd) {
                    err = BL_IPC_DWNLD_OTPCmdHandler(cmd, BL_IPC_DWNLD_Buf,
                            sizeof(BL_IPC_DWNLD_Buf), NULL, 0UL);
                    replySize = 28UL;
                    lineNo = __LINE__;
#ifdef BL_ENABLE_DBGMEM
                } else if (SYS_CMDID_MEM_WRITE == cmd) {
                    BL_BCM_MemCpy(&memAccess, BL_IPC_DWNLD_Buf, sizeof(SYS_MemAccessType));
                    err = DBGMEM_Copy(memAccess.addr, (uint32_t)(intptr_t)&memAccess.data[0],
                                        memAccess.width, memAccess.len);
                    BL_BCM_MemCpy(&(BL_IPC_DWNLD_Buf[4]), &memAccess, sizeof(SYS_MemAccessType));
                    replySize = sizeof(uint32_t) + sizeof(SYS_MemAccessType);
                    lineNo = __LINE__;
                } else if (SYS_CMDID_MEM_READ == cmd) {
                    BL_BCM_MemCpy(&memAccess, BL_IPC_DWNLD_Buf, sizeof(SYS_MemAccessType));
                    err = DBGMEM_Copy((uint32_t)(intptr_t)&memAccess.data[0], memAccess.addr,
                                        memAccess.width, memAccess.len);
                    BL_BCM_MemCpy(&(BL_IPC_DWNLD_Buf[4]), &memAccess, sizeof(SYS_MemAccessType));
                    replySize = sizeof(uint32_t) + sizeof(SYS_MemAccessType);
                    lineNo = __LINE__;
#endif /* BL_ENABLE_DBGMEM */
                } else if (SYS_CMDID_REBOOT == cmd) {
                    err = BL_BCM_ERR_OK;
                    BL_BCM_MemCpy(&rebootInfo, BL_IPC_DWNLD_Buf,
                                  sizeof(BL_IPC_RebootType));
                    lineNo = __LINE__;
                } else {
                    err = BL_BCM_ERR_INVAL_PARAMS;
                    lineNo = __LINE__;
                }
#ifdef BL_ENABLE_ETHSWT
            } else if (BCM_GROUPID_COMMS == recvCmdGrp) {
                if (ETHERSWT_CMDID_READ == cmd) {
                    BL_BCM_MemCpy(&ethSwtAccess, BL_IPC_DWNLD_Buf, sizeof(ETHERSWT_RegAccessType));
                    err = BL_IPC_EthSwtRead(&ethSwtAccess, &len);
                    BL_BCM_MemCpy(&(BL_IPC_DWNLD_Buf[4]), &ethSwtAccess, sizeof(ETHERSWT_RegAccessType));
                    replySize = sizeof(uint32_t) + sizeof(ETHERSWT_RegAccessType);
                    lineNo = __LINE__;
                } else if (ETHERSWT_CMDID_WRITE == cmd) {
                    BL_BCM_MemCpy(&ethSwtAccess, BL_IPC_DWNLD_Buf, sizeof(ETHERSWT_RegAccessType));
                    err = BL_IPC_EthSwtWrite(&ethSwtAccess, &len);
                    BL_BCM_MemCpy(&(BL_IPC_DWNLD_Buf[4]), &ethSwtAccess, sizeof(ETHERSWT_RegAccessType));
                    replySize = sizeof(uint32_t) + sizeof(ETHERSWT_RegAccessType);
                    lineNo = __LINE__;
                } else {
                    err = BL_BCM_ERR_INVAL_PARAMS;
                    lineNo = __LINE__;
                }
#endif /* BL_ENABLE_ETHSWT */
            } else {
                err = BL_BCM_ERR_NOSUPPORT;
                lineNo = __LINE__;
            }

            *bufPtr32 = err;
            errInfo1 = err;
            ret = IPC_Send(channID, recvCmd, BL_IPC_DWNLD_Buf, replySize);
            if (BL_BCM_ERR_OK != ret) {
                errInfo2 = ret;
                lineNo = __LINE__;
            }

            if ((BL_BCM_ERR_OK == err) && (BCM_GROUPID_SYS == recvCmdGrp)) {
                if (SYS_CMDID_REBOOT == cmd) {
                    IPC_DeInit(channID);
                    err = BL_IPC_DWNLD_ProcessCommonCmd(cmd, &rebootInfo);
                }
            }

        } else if (BL_BCM_ERR_NOMEM != err) {
            *bufPtr32 = err;
            errInfo1 = err;
            lineNo = __LINE__;
            ret = IPC_Send(channID, recvCmd, BL_IPC_DWNLD_Buf, sizeof(err));
            if (BL_BCM_ERR_OK != ret) {
                errInfo2 = ret;
                lineNo = __LINE__;
            }
        }
        if ((BL_BCM_ERR_OK != err) && (BL_BCM_ERR_NOMEM != err)) {
            BL_IPC_DWNLD_ReportError(err, 0UL, errInfo1, errInfo2, 0UL, lineNo);
        }
    }
}

void BL_IPC_DWNLD_ResetDwnldInfo(void)
{
    uint16_t regVal;
    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_IPC_DWNLD_BL_READY_MASK;
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
}
