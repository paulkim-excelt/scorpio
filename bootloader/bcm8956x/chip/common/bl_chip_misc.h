/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
 * @file bl_chip_misc.h
 *
 * @brief BCM8953x chip misc block
 *
 * This file defines BCM8953x chip misc block
 */

#ifndef BL_BCM8953X_CHIP_MISC_H
#define BL_BCM8953X_CHIP_MISC_H


typedef volatile struct {
    uint16_t MODEL_REV_NUM;
    uint16_t DEVICEID_LO;
    uint16_t DEVICEID_HI;
    uint16_t SWITCH_MISC_CTRL;
    uint16_t RSVD[124];
    uint16_t LDO_PWRDN;
    uint16_t LDO_VREGCNTL_1;
    uint16_t LDO_VREGCNTL_2;
    uint16_t LDO_VREGCNTLEN;
    uint16_t RSVD1[4];
    uint16_t SWREG_CTRL_REG0;
    uint16_t SWREG_CTRL_REG1;
    uint16_t SWREG_CTRL_REG2;
    uint16_t SWREG_CTRL_REG3;
    uint16_t SWREG_CTRL_REG4;
    uint16_t SWREG_CTRL_REG5;
    uint16_t SWREG_CTRL_REG6;
    uint16_t SWREG_CTRL_REG7;
    uint16_t SWREG_CTRL_REG8;
    uint16_t SWREG_CTRL_REG9;
    uint16_t SWREG_STAT_REG12;
    uint16_t SWREG_STAT_REG13;
    uint16_t SWREG_STAT_REG14;
    uint16_t SWREG_STAT_REG15;
    uint16_t SWREG_ACCESS_CTRL_1;
    uint16_t SWREG_ACCESS_CTRL_2;
    uint16_t SWREG_CONTROL_STATUS;
    uint16_t RSVD2[7];
    uint16_t SGMII_PWRDWN;
    uint16_t SGMII_HW_RST_DLY_VAL;
    uint16_t SGMII_MDIO_RST_DLY_VAL;
    uint16_t SGMII_PLL_RST_DLY_VAL;
    uint16_t SGMII_EXT_CTL;
    uint16_t SGMII_AN0;
    uint16_t SGMII_BASE_PAGE0;
    uint16_t SGMII_AN1;
    uint16_t SGMII_BASE_PAGE1;
    uint16_t SGMII_AN2;
    uint16_t SGMII_BASE_PAGE2;
    uint16_t SGMII_MDIO_CTL;
    uint16_t RSVD3[12];
    uint16_t OTP_CPU_CMD;
    uint16_t OTP_CPU_WRDATA_H;
    uint16_t OTP_CPU_WRDATA_L;
    uint16_t OTP_CFG;
    uint16_t OTP_ADDR;
    uint16_t OTP_STATUS_1;
    uint16_t OTP_STATUS_0;
    uint16_t RSVD4[1];
    uint16_t OTP_RDATA_H;
    uint16_t OTP_RDATA_L;
    uint16_t BISR_STATUS;
    uint16_t RSVD5[189];
    uint16_t F1_IMAGE_STATUS;
    uint16_t F1_IMAGE_VERSION;
    uint16_t F2_IMAGE_STATUS;
    uint16_t F2_IMAGE_VERSION;
    uint16_t SPARE_HW_REG4;
    uint16_t SPARE_HW_REG5;
    uint16_t SPARE_HW_REG6;
    uint16_t SPARE_HW_REG7;
    uint16_t SPARE_HW_REG8;
    uint16_t SPARE_HW_REG9;
    uint16_t SPARE_HW_REG10;
    uint16_t SPARE_HW_REG11;
    uint16_t SPARE_HW_REG12;
    uint16_t SPARE_HW_REG13;
    uint16_t SPARE_HW_REG14;
    uint16_t SPARE_HW_REG15;
    uint16_t SPARE_SW_REG0;
    uint16_t SPARE_SW_REG1;
    uint16_t SPARE_SW_REG2;
    uint16_t SPARE_SW_REG3;
    uint16_t SPARE_SW_REG4;
    uint16_t SPARE_SW_REG5;
    uint16_t SPARE_SW_REG6;
    uint16_t SPARE_SW_REG7;
    uint16_t SPARE_SW_REG8;
    uint16_t SPARE_SW_REG9;
    uint16_t SPARE_SW_REG10;
    uint16_t SPARE_SW_REG11;
    uint16_t SPARE_SW_REG12;
    uint16_t SPARE_SW_REG13;
    uint16_t SPARE_SW_REG14;
    uint16_t SPARE_SW_REG15;
    uint16_t CPUSYS_MISC;
    uint16_t BRPHYS_CLEAR_ON_READ_REG;
    uint16_t RSVD6[1629];
    uint16_t SCRATCH_REG;
} BL_CHIPMISC_RegsType;

#endif /* BL_BCM8953X_CHIP_MISC_H */
