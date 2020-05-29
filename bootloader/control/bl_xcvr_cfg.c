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

#include <bl_board.h>
#include <bl_compiler.h>
#include <bl_bcm_err.h>
#include <bl_cntrl.h>
#include <bl_log.h>
#include <bl_utils.h>
#include <eth_xcvr.h>
#include <otp.h>

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

#ifdef __BCM8910X__
extern void BL_BCM8910X_SetXcvrConfig(const ETHXCVR_ConfigType *const aXcvrConfig,
                                      uint8_t *const aMacAddr);
#endif


#ifdef __BCM8953X__
extern void BL_BCM8953X_SetXcvrConfig(const ETHXCVR_ConfigType *const aXcvrConfig,
                                      uint8_t *const aMacAddr);
#ifdef __BCM89559G__
void BL_BCM89881PHYConfig(uint8_t aPhy,
        ETHXCVR_BooleanType aAutoNeg,
        ETHXCVR_BooleanType aMode,
        ETHXCVR_SpeedType aSpeed);
#endif
#endif

#ifdef __BCM8956X__
extern void BL_BCM8956X_SetXcvrConfig(const ETHXCVR_ConfigType *const aXcvrConfig,
                                      uint8_t *const aMacAddr);
#endif

extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];;

int32_t BL_CNTRL_ReadOtpMacAddr(uint32_t aLoc,
                                uint32_t *const aMacOct123,
                                uint32_t *const aMacOct456)
{
    int32_t ret1, ret2;
    uint32_t row0, row1;

    if (0UL == aLoc) {
        row0 = BL_OTP_MAC_ADDR_0_OCTET123_ADDR;
        row1 = BL_OTP_MAC_ADDR_0_OCTET456_ADDR;
    } else if (1UL == aLoc){
        row0 = BL_OTP_MAC_ADDR_1_OCTET123_ADDR;
        row1 = BL_OTP_MAC_ADDR_1_OCTET456_ADDR;
    } else {
        ret1 = BL_BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    ret1 = OTP_Read(BL_OTP_HW_ID_0, row0, aMacOct123);
    ret2 = OTP_Read(BL_OTP_HW_ID_0, row1, aMacOct456);
    if ((BL_BCM_ERR_OK == ret1) && (BL_BCM_ERR_OK == ret2)) {
        if (((0UL == *aMacOct123) && (0UL == *aMacOct456))) {
            ret1 = BL_BCM_ERR_NOT_FOUND;
        }
    } else {
        ret1 = (BL_BCM_ERR_OK != ret1)? ret1 : ret2;
    }
exit:
    return ret1;
}

/**
    @code{.c}
    Load Bootloader xcvr config using PTM_LoadBLConfig()
    Load Bootloader mac config using PTM_LoadBLConfig()
    Initialize XCVR and MAC with the corresponding loaded configs
    @endcode
*/
void BL_CNTRL_XcvrInit(const ETHXCVR_ConfigType *const aConfig)
{
    uint8_t macAddr[6UL] = {0};
    uint32_t data1, data2;
    int32_t ret = BL_BCM_ERR_NOT_FOUND;

    /* Read MAC address from OTP memory */
    /* Read MAC address @ location 0 */
    ret = BL_CNTRL_ReadOtpMacAddr(0UL, &data1, &data2);

    if (BL_BCM_ERR_NOT_FOUND == ret) {
        /* Read MAC address copy @ location 1 */
        ret = BL_CNTRL_ReadOtpMacAddr(1UL, &data1, &data2);
    }

    if (BL_BCM_ERR_OK == ret) {
        macAddr[0] = (uint8_t)((data1 >> 16UL) & 0xFFUL);
        macAddr[1] = (uint8_t)((data1 >> 8UL) & 0xFFUL);
        macAddr[2] = (uint8_t)(data1 & 0xFFUL);
        macAddr[3] = (uint8_t)((data2 >> 16UL) & 0xFFUL);
        macAddr[4] = (uint8_t)((data2 >> 8UL) & 0xFFUL);
        macAddr[5] = (uint8_t)(data2 & 0xFFUL);
    } else {
        /* Read default MAC address */
        BL_BCM_MemCpy(macAddr, BL_DefaultMacAddr, 6UL);
    }
    BL_LOG_ERR("Configuring MAC address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            (uint32_t)macAddr[0], (uint32_t)macAddr[1], (uint32_t)macAddr[2],
            (uint32_t)macAddr[3], (uint32_t)macAddr[4], (uint32_t)macAddr[5]);
#ifdef __BCM8910X__
    BL_BCM8910X_SetXcvrConfig(aConfig, macAddr);
#endif

#ifdef __BCM8953X__
    BL_BCM8953X_SetXcvrConfig(aConfig, macAddr);
#ifdef __BCM89559G__
    if (aConfig->port[5UL].phy.phyMedia == ETHXCVR_PHYMEDIA_1000BASET1) {
        BL_BCM89881PHYConfig(aConfig->port[5UL].phy.slaveID,
                aConfig->port[5UL].autoNeg,
                aConfig->port[5UL].phy.masterMode,
                aConfig->port[5UL].speed);
    }
#endif
#endif

#ifdef __BCM8956X__
    BL_BCM8956X_SetXcvrConfig(aConfig, macAddr);
#endif

}
