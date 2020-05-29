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
    @defgroup grp_bl_ram_impl Implementation
    @ingroup grp_bl_ram

    @addtogroup grp_bl_ram_impl
    @{
    @file bl_ram_downloader.c
    @brief RAM downloader Implementation
    This source file contains the implementation of RAM downloader

    @version 0.30 Imported from docx
*/

#include <string.h>
#include <bl_bcm_err.h>
#include <bl_downloader.h>
#include <bl_ptm.h>
#include <bl_cntrl.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <bl_utils.h>
#include <bl_edc.h>
#include <bl_tar.h>
#include <otp.h>
#include <bl_board.h>
#include <compiler.h>

/**
    @name RAM downloader API IDs
    @{
    @brief API IDs for RAM downloader
*/
#define BRCM_DSGN_BL_RAM_DWNLD_VALIDATE_ARGS_PROC   (0x80U) /**< @brief #BL_RAM_DWNLD_ValidateArgs */
#define BRCM_DSGN_BL_RAM_DWNLD_EXECUTE_IMG_PROC     (0x81U) /**< @brief #BL_RAM_DWNLD_ExecuteImg */
#define BRCM_DSGN_BL_RAM_DWNLD_WRITE_IMG_PROC       (0x82U) /**< @brief #BL_RAM_DWNLD_WriteImg */
#define BRCM_DSGN_BL_RAM_DWNLD_ERASE_FLASH_PROC     (0x83U) /**< @brief #BL_RAM_DWNLD_EraseFlash */
#define BRCM_DSGN_BL_RAM_DWNLD_REPORT_ERROR_PROC    (0x84U) /**< @brief #BL_RAM_DWNLD_ReportError */
/** @} */

#define BL_JUMP_TO(addr) asm ("bx %[jump]" : : [jump]"r"(addr):)
#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)

#define BL_REG_READ16(aAddr)       (*((const volatile uint16_t *)(aAddr)))
#define BL_REG_WRITE16(aAddr, aVal)    \
    (*((volatile uint16_t *)(aAddr)) = (uint16_t)(aVal))

int32_t BL_CNTRL_ReadOtpMacAddr(uint32_t aLoc,
                                uint32_t *const aMacOct123,
                                uint32_t *const aMacOct456);

extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];
static const BL_DWNLD_ImgHdrType *const ImgHdr =
    (const BL_DWNLD_ImgHdrType *const)BL_DWNLD_StartAddr;

typedef void (*BL_RAM_DWNLD_ImgEntryFuncType)(void);

uint16_t BL_TargetInfo COMP_SECTION(".data.dwnldinfo") = 0;
uint16_t BL_HostInfo COMP_SECTION(".data.dwnldinfo") = 0;

static BL_COMP_NEVER_INLINE void BL_RAM_DWNLD_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_RAM_DWNLD_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static void BL_RAM_DWNLD_UpdateStatus(uint32_t aPercent)
{
    uint16_t regVal;

    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_RAM_DWNLD_FLASHING_PERCENT_MASK;
    regVal |= (((uint16_t)aPercent << BL_RAM_DWNLD_FLASHING_PERCENT_SHIFT)
               & BL_RAM_DWNLD_FLASHING_PERCENT_MASK);
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
}

/** @brief Validate the arguments

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Validation successful
    @retval     #BL_BCM_ERR_INVAL_MAGIC    Image header magic is invalid
    @retval     #BL_BCM_ERR_NOMEM          Image size is greater than the available memory
    @retval     others                  Errors from #EDC_ValidateCRC

    @post None

    @trace  #BRCM_DSGN_BL_RAM_DWNLD_VALIDATE_ARGS_PROC  #BRCM_ARCH_BL_RAM_DWNLD_VALIDATE_ARGS

    @limitations None

    @code{.c}
    ImgHdr = BL_DWNLD_StartAddr
    if (DWNLD_ITBL_MAGIC != ImgHdr->magicNum) and (DWNLD_TAR_MAGIC_NUM != ImgHdr->magicNum)
        return BL_BCM_ERR_INVAL_MAGIC
    if (BL_DWNLD_EndAddr - BL_DWNLD_StartAddr) < ImgHdr->imgSize
        return BL_BCM_ERR_NOMEM
    if PTBL_ERR_DET_CRC = (ImgHdr->flags & PTBL_ERR_DET_CRC)
        err = EDC_ValidateCRC((BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE), ImgHdr->imgSize, ImgHdr->errDetCode)
    else if (PTBL_ERR_DET_CHECKSUM == (ImgHdr->flags & PTBL_ERR_DET_CHECKSUM))
        err = EDC_ValidateChcksm((BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE), ImgHdr->imgSize, ImgHdr->errDetCode)
    else
        err = BL_BCM_ERR_OK
    return err;
    @endcode
*/
static int32_t BL_RAM_DWNLD_ValidateArgs(void)
{
    int32_t err;

    if (BL_DWNLD_TAR_MAGIC_NUM != ImgHdr->magicNum) {
        err = BL_BCM_ERR_INVAL_MAGIC;
        goto err;
    }
    if ((BL_DWNLD_EndAddr - BL_DWNLD_StartAddr) < ImgHdr->imgSize) {
        err = BL_BCM_ERR_NOMEM;
        goto err;
    }
    if (PTBL_ERR_DET_CRC == (ImgHdr->flags & PTBL_ERR_DET_CRC)) {
        err = BL_EDC_ValidateCRC((BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE),
                ImgHdr->imgSize, ImgHdr->errDetCode);
    } else if (PTBL_ERR_DET_CHECKSUM == (ImgHdr->flags & PTBL_ERR_DET_CHECKSUM)) {
        err = BL_EDC_ValidateChcksm((BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE),
                ImgHdr->imgSize, ImgHdr->errDetCode);
    } else {
        err = BL_BCM_ERR_OK;
    }

err:
    return err;
}

/** @brief API to erase flash

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aRegVal     Host register value

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Flash Erased Successfully
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Corresponding flash not found
    @retval     others                  Errors from #BL_FLASH_EraseSector

    @post None

    @trace  #BRCM_DSGN_BL_RAM_DWNLD_ERASE_FLASH_PROC  #BRCM_ARCH_BL_RAM_DWNLD_ERASE_FLASH

    @limitations None

    @code{.c}
    flashID = (aRegVal & BL_RAM_DWNLD_FLASH_ID_MASK) >> BL_RAM_DWNLD_FLASH_ID_SHIFT

    for i = 0 to BL_FLASH_CfgSize
        if  flashID = BL_FLASH_Config[i].hwID
            sectorSize = BL_FLASH_Config[i].sectorSize
            sectors = BL_FLASH_Config[i].size / sectorSize
            break
    if 0UL = sectors
        return BL_BCM_ERR_INVAL_PARAMS
    for i = 0 to sectors
        addr = i * sectorSize
        BL_FLASH_EraseSector(flashID, addr, sectorSize)
        percent = ((i + 1UL) * 100UL) / sectors
        BL_RAM_DWNLD_UpdateStatus(percent)

    return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_RAM_DWNLD_EraseFlash(uint32_t aRegVal)
{
    uint32_t i;
    uint32_t sectorSize = 0UL;
    uint32_t sectors = 0UL;
    BL_FLASH_HwIDType flashID;
    uint32_t addr;
    uint32_t percent = 0UL;
    int32_t err = BL_BCM_ERR_OK;

    flashID = (aRegVal & BL_RAM_DWNLD_FLASH_ID_MASK) >> BL_RAM_DWNLD_FLASH_ID_SHIFT;
    BL_LOG_ERR("Erase flash(%d)..\n", flashID);
    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (flashID == BL_FLASH_Config[i].hwID) {
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
        err = BL_FLASH_EraseSector(flashID, addr, sectorSize);
        if (BL_BCM_ERR_OK != err) {
            break;
        }
        percent = ((i + 1UL) * 100UL) / sectors;
        BL_RAM_DWNLD_UpdateStatus(percent);
    }
    BL_LOG_ERR("Erase flash(%d) completed\n", flashID);
exit:
    return err;
}

#define BL_READ_BUF_SIZE     (256UL)
static uint8_t BL_ReadData[BL_READ_BUF_SIZE];

/**
 * Write data to given flash
 * This function is added for the failsafe testing purpose only
 * This function erases sector/sub-sector of the flash and writes
 * the data to flash
 */
static int32_t BL_RAM_DWNLD_WriteFlash(uint32_t aRegVal)
{
    uint32_t i;
    BL_FLASH_HwIDType flashID;
    uint32_t addr;
    uint8_t *temp = (uint8_t *)BL_DWNLD_StartAddr;
    uint8_t data[4] = {0};
    uint32_t pageAddr, offset, pageSize;
    int32_t err = BL_BCM_ERR_UNKNOWN;

    flashID = (aRegVal & BL_RAM_DWNLD_FLASH_ID_MASK) >> BL_RAM_DWNLD_FLASH_ID_SHIFT;
    addr = *(uint32_t *)temp;

    for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
        if (flashID == BL_FLASH_Config[i].hwID) {
            pageSize = BL_FLASH_Config[i].pageSize;
            if (BL_READ_BUF_SIZE < pageSize) {
                err = BL_BCM_ERR_NOMEM;
                break;
            }
            pageAddr = (addr / pageSize) * pageSize;
            offset = (addr % pageSize);
            err = BL_FLASH_Read(flashID, pageAddr, BL_ReadData, pageSize);
            if (BL_BCM_ERR_OK == err) {
                if (0L != BL_BCM_MemCmp(&BL_ReadData[offset], data, 4UL)) {
                    BL_LOG_ERR("flash(%d) write 0x0 @ address:0x%x\n",
                            flashID, addr);
                    BL_BCM_MemSet(&BL_ReadData[offset], 0x0L, 4UL);
                    err = BL_FLASH_Write(flashID, pageAddr, BL_ReadData,
                            pageSize);
                } else {
                    BL_LOG_ERR("value @0x%x is already 0\n", addr);
                }
            }
            break;
        }
    }

    if (i == BL_FLASH_CfgSize) {
        err = BL_BCM_ERR_INVAL_PARAMS;
    }

    if (BL_BCM_ERR_OK != err) {
        BL_LOG_ERR("Writing to flash failed(%u)\n", err);
    }

    return err;
}

/** @brief Execute the image

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     others                  Errors from #BL_RAM_DWNLD_ValidateArgs

    @post None

    @trace  #BRCM_DSGN_BL_RAM_DWNLD_EXECUTE_IMG_PROC  #BRCM_ARCH_BL_RAM_DWNLD_EXECUTE_IMG

    @limitations None

    @code{.c}
    ImgHdr = BL_DWNLD_StartAddr
    err = BL_RAM_DWNLD_ValidateArgs()
    if BL_BCM_ERR_OK == err
        entry = BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE
        entry |= 1UL
        entry()
    return err;
    @endcode
*/
static int32_t BL_RAM_DWNLD_ExecuteImg(void)
{
    int32_t err = BL_BCM_ERR_OK;
#ifdef __BCM8910X__
    uint32_t entry;

    err = BL_RAM_DWNLD_ValidateArgs();
    if (BL_BCM_ERR_OK == err) {
        entry = (uint32_t)(BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE);
        entry |= 1UL;
        ((BL_RAM_DWNLD_ImgEntryFuncType)(entry))();
    }
#endif
#ifdef __BCM8953X__
    BL_JUMP_TO((uint32_t)(BL_TCM_BASE)); /* should never return back from here */
#endif
    return err;
}

/** @brief API to write the image

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aRegVal     Host register value

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Image successfully written
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Flash ID is invalid
    @retval     others                  Errors from #BL_RAM_DWNLD_ValidateArgs, #PTM_WritePT, #PTM_Init, #PTM_WriteImg

    @post None

    @trace  #BRCM_DSGN_BL_RAM_DWNLD_WRITE_IMG_PROC  #BRCM_ARCH_BL_RAM_DWNLD_WRITE_IMG

    @limitations None

    @code{.c}
    flashID = (aRegVal & BL_RAM_DWNLD_FLASH_ID_MASK) >> BL_RAM_DWNLD_FLASH_ID_SHIFT
    pid = ((ImgHdr->flags & DWNLD_FLAG_PID_MASK) >> DWNLD_FLAG_PID_SHIFT)
           | ((ImgHdr->flags & DWNLD_FLAG_COPY_ID_MASK) >> DWNLD_FLAG_COPY_ID_SHIFT) << 8UL)
    imgID = (ImgHdr->flags & DWNLD_FLAG_IMG_ID_MASK) >> DWNLD_FLAG_IMG_ID_SHIFT

    if flashID > BL_FLASH_MAX_HW_ID
        return BL_BCM_ERR_INVAL_PARAMS
    err = BL_RAM_DWNLD_ValidateArgs()
    if BL_BCM_ERR_OK = err
        if PT_IMG_PID = pid
            err = PTM_WritePT(flashID, BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE, ImgHdr->imgSize, ImgHdr->errDetCode,
            BL_RAM_DWNLD_UpdateStatus)
            if BL_BCM_ERR_OK = err
                PTM_DeInit()
                err = PTM_Init(&BL_PTM_Cfg)
        else
            if DWNLD_ITBL_MAGIC = ImgHdr->magicNum
                err = PTM_WriteImg(flashID, pid, imageID, BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE,
                    ImgHdr->imgSize, ImgHdr->errDetCode, BL_RAM_DWNLD_UpdateStatus)
            else
                err = TAR_UpdateFlash(flashID, BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE, ImgHdr->imgSize, BL_RAM_DWNLD_UpdateStatus)

    return err;
    @endcode
*/
static int32_t BL_RAM_DWNLD_WriteImg(uint16_t aRegVal)
{
    int32_t err;
    BL_FLASH_HwIDType flashID;
    uint32_t lineNo = __LINE__;

    flashID = (((uint32_t)aRegVal & (uint32_t)BL_RAM_DWNLD_FLASH_ID_MASK)
                         >> (uint32_t)BL_RAM_DWNLD_FLASH_ID_SHIFT);

    if (flashID > BL_FLASH_MAX_HW_ID) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        BL_RAM_DWNLD_ReportError(err, flashID, 0UL, 0UL, 0UL, lineNo);
        goto err;
    }
    err = BL_RAM_DWNLD_ValidateArgs();
    if (BL_BCM_ERR_OK == err) {
        err = BL_TAR_UpdateFlash(flashID, BL_DWNLD_StartAddr + BL_DWNLD_ITBL_HDR_SIZE,
                        ImgHdr->imgSize, BL_RAM_DWNLD_UpdateStatus);
    } else {
        BL_LOG_ERR("RAM downloader failed %d\n", err);
    }

err:
    if (BL_BCM_ERR_OK == err) {
        BL_LOG_ERR("Image(pid:0x%x imgId:0x%x) flashed successfully\n", 0UL, 0UL);
    } else if (BL_BCM_ERR_NOT_FOUND == err) {
        BL_LOG_ERR("Image(pid:0x%x imgId:0x%x) not found\n", 0UL, 0UL);
    } else {
        BL_LOG_ERR("Failed to flash image(pid:0x%x imgId:0x%x) err:%d\n", 0UL, 0UL, err);
    }

    return err;
}

static int32_t BL_RAM_DWNLD_OTPCmdHandler(uint16_t aCmd)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t i;
    uint32_t data, data2;
    uint32_t row, row2;
    uint32_t startRow;
    uint32_t endRow;
    uint32_t *temp = (uint32_t *)BL_DWNLD_StartAddr;
    uint32_t keyBytes = BL_OTP_SECURE_KEY_SIZE;
    uint32_t offset = 0UL;
    uint32_t bytesPerRow = BL_RSA_KEY_BYTES_PER_ROW;

    switch (aCmd) {
    case BL_RAM_DWNLD_CMD_OTP_RD:
        err = OTP_Read(BL_OTP_HW_ID_0, temp[0], &temp[1]);
        BL_LOG_ERR("OTP read: Addr: %d data:0x%x\n", temp[0], temp[1]);
        break;
    case BL_RAM_DWNLD_CMD_OTP_WR_SEC_KEY:
        BL_LOG_ERR("Write Secure key..\n");
        err = BL_RAM_DWNLD_ValidateArgs();
        if (err == BL_BCM_ERR_OK) {
            if (ImgHdr->imgSize != BL_OTP_SECURE_KEY_SIZE) {
                err = BL_BCM_ERR_INVAL_PARAMS;
                break;
            }
            row = BL_OTP_ADDR_RSA_PUB_START;
            while (keyBytes > 0UL) {
                data = 0UL;
                if (keyBytes < BL_RSA_KEY_BYTES_PER_ROW) {
                    bytesPerRow = keyBytes;
                }
                for (i = 0UL; i < bytesPerRow; i++) {
                    data |= (BL_DWNLD_StartAddr[BL_DWNLD_ITBL_HDR_SIZE + offset]
                            << (i * 8UL));
                    offset++;
                    keyBytes--;
                }
                err = OTP_Write(BL_OTP_HW_ID_0, row, data);
                if (err != BL_BCM_ERR_OK) {
                    break;
                }
                row++;
            }
        } else {
            BL_LOG_ERR("EDC validation failed\n");
        }
        break;
    case BL_RAM_DWNLD_CMD_OTP_ENABLE_SEC:
        BL_LOG_ERR("Write SEC Enable bit\n");
        err = OTP_Write(BL_OTP_HW_ID_0, BL_OTP_SEC_BOOT_EN_ROW,
                BL_OTP_SEC_BOOT_EN_MASK);
        break;
    case BL_RAM_DWNLD_CMD_OTP_WRITE_CUST:
        startRow = BL_OTP_ADDR_RSA_PUB_START;
        endRow = startRow + (keyBytes / BL_RSA_KEY_BYTES_PER_ROW);
        row = temp[0];
        data = temp[1];
        if ((row < BL_OTP_ADDR_BRCM_REG_START) ||
                ((row >= startRow) && (row <= endRow)) ||
                (row == BL_OTP_SEC_BOOT_EN_ROW)) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            break;
        }
        BL_LOG_ERR("OTP Write custom: Addr: %d data:0x%x\n", row, data);
        err = OTP_Write(BL_OTP_HW_ID_0, row, data);
        break;
    case BL_RAM_DWNLD_CMD_OTP_RD_MAC_ADDR:
        temp[0] = 0x3UL;
        /* Read MAC address copy 0 */
        err = BL_CNTRL_ReadOtpMacAddr(0UL, &data, &data2);
        if ((BL_BCM_ERR_OK != err) && (BL_BCM_ERR_DATA_INTEG != err)
                && (BL_BCM_ERR_NOT_FOUND != err)) {
            break;
        }
        if (BL_BCM_ERR_DATA_INTEG == err) {
            temp[0] = temp[0] & (~0x1UL);
        }
        temp[1] = data;
        temp[2] = data2;

        /* Read MAC address copy 1 */
        err = BL_CNTRL_ReadOtpMacAddr(1UL, &data, &data2);
        if ((BL_BCM_ERR_OK != err) && (BL_BCM_ERR_DATA_INTEG != err)
                && (BL_BCM_ERR_NOT_FOUND != err)) {
            break;
        }
        if (BL_BCM_ERR_DATA_INTEG == err) {
            temp[0] = temp[0] & (~0x2UL);
        }

        temp[3] = data;
        temp[4] = data2;
        err = BL_BCM_ERR_OK;
        break;
    case BL_RAM_DWNLD_CMD_OTP_WR_MAC_ADDR:
        data = temp[1];
        data2 = temp[2];
        /* Broadcast, multicast & universal administered MAC addressess
         * are not allowed */
        if (((0xFFFFFFUL == (data & 0xFFFFFFUL))
                    && (0xFFFFFFUL == (data2 & 0xFFFFFFUL)))
                || ((data & (0x1 << 16UL)) != 0UL)
                || ((data & (0x1 << 17UL)) == 0UL)
                || ((0x0UL == (data & 0xFFFFFFUL))
                    && (0x0UL == (data2 & 0xFFFFFFUL)))) {
            err = BL_BCM_ERR_INVAL_PARAMS;
            break;
        }

        if (0UL == temp[0]) {
            row = BL_OTP_MAC_ADDR_0_OCTET123_ADDR;
            row2 = BL_OTP_MAC_ADDR_0_OCTET456_ADDR;
        } else if (1UL == temp[0]) {
            row = BL_OTP_MAC_ADDR_1_OCTET123_ADDR;
            row2 = BL_OTP_MAC_ADDR_1_OCTET456_ADDR;
        } else {
            err = BL_BCM_ERR_INVAL_PARAMS;
            break;
        }
        err = OTP_Read(BL_OTP_HW_ID_0, row, &data);
        if (BL_BCM_ERR_OK != err) {
            break;
        }
        err = OTP_Read(BL_OTP_HW_ID_0, row2, &data2);
        if (BL_BCM_ERR_OK != err) {
            break;
        }
        if ((0UL != data) && (0UL != data2)) {
            err = BL_BCM_ERR_NOPERM;
            BL_RAM_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, __LINE__);
            break;
        } else {
            data = temp[1];
            data2 = temp[2];
            err = OTP_Write(BL_OTP_HW_ID_0, row, data);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            err = OTP_Write(BL_OTP_HW_ID_0, row2, data2);
            if (BL_BCM_ERR_OK != err) {
                break;
            }
            BL_LOG_ERR("Written MAC address @location-%d: %02x:%02x:%02x:%02x:%02x:%02x\n",
                    temp[0],
                    ((data >> 16UL) & 0xFFUL),
                    ((data >> 8UL) & 0xFFUL),
                    (data & 0xFFUL),
                    ((data2 >> 16UL) & 0xFFUL),
                    ((data2 >> 8UL) & 0xFFUL),
                    (data2 & 0xFFUL));
        }
        break;
    default:
        err = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }
    return err;
}

/**
    @code{.c}
    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG)
    regVal |= BL_RAM_DWNLD_BL_READY_MASK
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal)
    while (1UL)
        regVal = BL_REG_READ16(BL_DWNLD_HOST_SPARE_REG)
        hostSeqNo = ((regVal & BL_RAM_DWNLD_HOST_SEQ_NUM_MASK) >> BL_RAM_DWNLD_HOST_SEQ_NUM_SHIFT)
        if targetSeqNo = (1U - hostSeqNo)
            cmd = ((regVal & BL_RAM_DWNLD_CMD_MASK) >> BL_RAM_DWNLD_CMD_SHIFT)
            if BL_RAM_DWNLD_CMD_EXECUTE = cmd
                err = BL_RAM_DWNLD_ExecuteImg()
            else if BL_RAM_DWNLD_CMD_FLASH = cmd
                err = BL_RAM_DWNLD_WriteImg(regVal)
            else if BL_RAM_DWNLD_CMD_ERASE = cmd
                err = BL_RAM_DWNLD_EraseFlash(regVal)
            else
                err = BL_BCM_ERR_INVAL_PARAMS;
            if BL_BCM_ERR_OK = err
                status = BL_RAM_DWNLD_STATUS_SUCCESS
            else
                status = BL_RAM_DWNLD_STATUS_FAILURE
            targetSeqNo = (1U - targetSeqNo)
            regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG)
            regVal &= ~(BL_RAM_DWNLD_STATUS_MASK | BL_RAM_DWNLD_TARGET_SEQ_NUM_MASK)
            regVal |= ((status << BL_RAM_DWNLD_STATUS_SHIFT) & BL_RAM_DWNLD_STATUS_MASK)
                       | ((targetSeqNo << BL_RAM_DWNLD_TARGET_SEQ_NUM_SHIFT) & BL_RAM_DWNLD_TARGET_SEQ_NUM_MASK)
            BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal)
    @endcode
*/
void BL_RAM_DWNLD_Init(MCU_DwnldModeType aDwnldMode)
{
    uint16_t status;
    uint16_t regVal;
    uint16_t cmd;
    uint16_t targetSeqNo = 0U;
    uint16_t hostSeqNo;
    int32_t err;
    uint32_t lineNo = __LINE__;

    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal |= BL_RAM_DWNLD_BL_READY_MASK;
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
    while (1UL) {
        regVal = BL_REG_READ16(BL_DWNLD_HOST_SPARE_REG);
        hostSeqNo = ((regVal & BL_RAM_DWNLD_HOST_SEQ_NUM_MASK)
                     >> BL_RAM_DWNLD_HOST_SEQ_NUM_SHIFT);
        if (targetSeqNo == (1U - hostSeqNo)) {
            cmd = ((regVal & BL_RAM_DWNLD_CMD_MASK) >> BL_RAM_DWNLD_CMD_SHIFT);
            /* clear the percentage field since a new command is received*/
            BL_RAM_DWNLD_UpdateStatus(0UL);

            switch (cmd) {
            case BL_RAM_DWNLD_CMD_EXECUTE:
                err = BL_RAM_DWNLD_ExecuteImg();
                break;
            case BL_RAM_DWNLD_CMD_FLASH:
                err = BL_RAM_DWNLD_WriteImg(regVal);
                break;
            case BL_RAM_DWNLD_CMD_ERASE:
                err = BL_RAM_DWNLD_EraseFlash(regVal);
                break;
            case BL_RAM_DWNLD_CMD_FLASH_WRITE:
                err = BL_RAM_DWNLD_WriteFlash(regVal);
                break;
            case BL_RAM_DWNLD_CMD_OTP_RD:
            case BL_RAM_DWNLD_CMD_OTP_WR_SEC_KEY:
            case BL_RAM_DWNLD_CMD_OTP_ENABLE_SEC:
            case BL_RAM_DWNLD_CMD_OTP_WRITE_CUST:
            case BL_RAM_DWNLD_CMD_OTP_WR_MAC_ADDR:
            case BL_RAM_DWNLD_CMD_OTP_RD_MAC_ADDR:
                err = BL_RAM_DWNLD_OTPCmdHandler(cmd);
                break;
            default:
                err = BL_BCM_ERR_INVAL_PARAMS;
                lineNo = __LINE__;
                BL_RAM_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
                break;
            }

            if (BL_BCM_ERR_OK == err) {
                status = BL_RAM_DWNLD_STATUS_SUCCESS;
            } else {
                status = BL_RAM_DWNLD_STATUS_FAILURE;
            }
            targetSeqNo = (1U - targetSeqNo);
            regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
            regVal &= ~(BL_RAM_DWNLD_STATUS_MASK | BL_RAM_DWNLD_TARGET_SEQ_NUM_MASK);
            regVal |= ((status << BL_RAM_DWNLD_STATUS_SHIFT)
                       & BL_RAM_DWNLD_STATUS_MASK)
                       | ((targetSeqNo << BL_RAM_DWNLD_TARGET_SEQ_NUM_SHIFT)
                          & BL_RAM_DWNLD_TARGET_SEQ_NUM_MASK);
            BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
        }
    }
}

void BL_RAM_DWNLD_ResetDwnldInfo(void)
{
    uint16_t regVal;
    regVal = BL_REG_READ16(BL_DWNLD_TARGET_SPARE_REG);
    regVal &= ~BL_RAM_DWNLD_BL_READY_MASK;
    BL_REG_WRITE16(BL_DWNLD_TARGET_SPARE_REG, regVal);
}

/** @} */
