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

#include <stdint.h>
#include <bl_utils.h>
#include <bl_bcm_err.h>
#include <bl_cntrl.h>
#include <eth_xcvr.h>

#ifdef __BCM89559G__
#include <bl_mmi.h>
#endif

void BL_BCM89881PHYConfig(uint8_t aPhy,
        ETHXCVR_BooleanType aAutoNeg,
        ETHXCVR_BooleanType aMode,
        ETHXCVR_SpeedType aSpeed)
{
    uint16_t regVal16;

    /* Enable SGMII_MDIO */
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x1E, 0x0010, 0x8001);

    /* Set SGMII PLL registers */
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x05, 0x8000, 0x0C2F);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x05, 0x8183, 0x3D09);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x05, 0x8184, 0x00FA);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x05, 0x8185, 0x09C4);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x05, 0x8000, 0x2C2F);
    /* Disable SGMII_MDIO */
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x1E, 0x0010, 0x0001);

    /* Enable DSP RDB clock */
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x900B, 0x0000);

    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8100, 0xe2e1);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8101, 0x0002);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8102, 0x0001);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8103, 0x0006);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8104, 0x0005);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8105, 0x9fbe);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8106, 0x25bf);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8110, 0x7a7b);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8111, 0x8f80);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8112, 0x8000);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8113, 0x0006);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8114, 0x0001);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8115, 0x8fbe);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8116, 0x25bf);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8120, 0x0095);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8121, 0x8007);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8122, 0x0000);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8123, 0x0406);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8124, 0x0001);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8125, 0x8fbe);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8126, 0x25bf);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8030, 0xe005);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8031, 0x0079);
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x01, 0x8032, 0x0094);

    if (aAutoNeg == ETHXCVR_BOOLEAN_FALSE) {
        regVal16 = 0x8000;
        if (aMode == ETHXCVR_BOOLEAN_TRUE) {
            regVal16 |= (1 << 14);
        }
        if ((aSpeed & ETHXCVR_SPEED_1000MBPS) != 0) {
            regVal16 |= (1 << 0);
        }
        BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x1, 0x0834, regVal16);
        BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x07, 0x0200, 0x200);
    } else { /* SYSCFG_XCVR_PORT_AUTONEG_EN */
        regVal16 = 0x0020;

        if (aMode == ETHXCVR_BOOLEAN_TRUE) {
            regVal16 |= (1 << 4);
        }
        if ((aSpeed & ETHXCVR_SPEED_1000MBPS) != 0) {
            regVal16 |= (1 << 7);
        }

        /* Write to Autoneg Adv register */
        BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x07, 0x0203, regVal16);
        BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x07, 0x0202, 0x0001);
        /* Enable AutoNeg */
        BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, aPhy, 0x07, 0x0200, 0x1200);
    }
}

