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
#include <stdint.h>
#include <string.h>
#include <rsa.h>
#include <sha.h>
#include <otp.h>
#include <bl_utils.h>
#include <bl_chip_config.h>
#include <bl_bcm_err.h>
#include <bl_log.h>
#include <bl_ptm.h>
#include <bl_cntrl.h>

#define BRCM_DSGN_BL_CNTRL_SEC_REPORT_ERROR_PROC    (0xD0U) /**< @brief #BL_CNTRL_SecReportError */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)

static uint8_t BL_PubKey[RSA_S_MODULUS8];

static BL_COMP_NEVER_INLINE void BL_CNTRL_SecReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_CNTRL_SEC_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static int32_t BL_CNTRL_ReadOTPKey(uint32_t aOtpAddr, uint32_t aBytesPerRow,
                                 uint8_t *const aKey, uint32_t aLen)
{
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t data;
    uint32_t i;
    uint32_t j;
    uint32_t offset = 0UL;
    int32_t bytesRemain = aLen;
    uint32_t rows;
    uint32_t loopCnt = aBytesPerRow;

    rows = aLen / aBytesPerRow;
    if ((aLen % aBytesPerRow) != 0UL) {
        rows = rows + 1UL;
    }

    for (i = 0UL; i < rows; i++) {
        ret = OTP_Read(BL_OTP_SEC_HW_ID, aOtpAddr + i, &data);
        if (ret != BL_BCM_ERR_OK) {
            break;
        }
        if (bytesRemain < aBytesPerRow) {
            loopCnt = bytesRemain;
        }
        for (j = 0; j < loopCnt; j++) {
            aKey[offset] = (uint8_t) ((data >> (j * 8UL)) & 0xFFUL);
            offset++;
        }
        bytesRemain = bytesRemain -aBytesPerRow;
    }
    return ret;
}

uint32_t BL_CNTRL_isSecureBoot(void)
{
    uint32_t data;
    int32_t ret;
    uint32_t isSecure = BL_FALSE;
    uint32_t lineNo;

    ret = OTP_Read(BL_OTP_SEC_HW_ID, BL_OTP_SEC_BOOT_EN_ROW, &data);
    if (ret == BL_BCM_ERR_OK) {
        if ((data & BL_OTP_SEC_BOOT_EN_MASK) == BL_OTP_SEC_BOOT_EN_MASK) {
            isSecure = BL_TRUE;
        }
    } else if (ret == BL_BCM_ERR_DATA_INTEG) {
        /**
         * someone tried to temper with secure OTP row?
         * force the secure boot mode
         */
        lineNo = __LINE__;
        isSecure = BL_TRUE;
    } else {
        lineNo = __LINE__;
    }
    if (ret != BL_BCM_ERR_OK) {
        BL_CNTRL_SecReportError(ret, 0UL, 0UL, 0UL, 0UL, lineNo);
    }
    return isSecure;
}

int32_t BL_CNTRL_ValidatePubKey(void)
{
    int32_t ret;
#ifdef BL_SECURE_KEY_FROM_PT
    uint32_t copyNum;
    uint32_t size = RSA_S_MODULUS8;
    uint8_t otpKey[SHA256_S_DIGEST8];
    uint8_t digest[SHA256_S_DIGEST8];

    ret = BL_PTM_LoadSecKey(BL_PubKey, &size, &copyNum);
    if (ret == BL_BCM_ERR_OK) {
        if (size == RSA_S_MODULUS8) {
            BL_BCM_MemSet(otpKey, 0, sizeof(otpKey));
            BL_BCM_MemSet(digest, 0, sizeof(digest));
            ret = BL_CNTRL_ReadOTPKey(BL_OTP_ADDR_RSA_PUB_START, BL_RSA_KEY_BYTES_PER_ROW,
                    otpKey, SHA256_S_DIGEST8);
            if (ret == BL_BCM_ERR_OK) {
                BCM_SHA256((uint32_t *const)digest, BL_PubKey, RSA_S_MODULUS);
                ret = BL_BCM_MemCmp(otpKey, digest, SHA256_S_DIGEST8);
            }
        } else {
            ret = BL_BCM_ERR_UNKNOWN;
        }
    }
#else
    ret = BL_CNTRL_ReadOTPKey(BL_OTP_ADDR_RSA_PUB_START, BL_RSA_KEY_BYTES_PER_ROW,
            BL_PubKey, RSA_S_MODULUS8);
#endif
    return ret;
}

const uint8_t * BL_CNTRL_GetPubKey(void)
{
    return BL_PubKey;
}


