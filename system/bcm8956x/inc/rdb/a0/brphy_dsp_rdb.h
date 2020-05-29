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
/**
    @file brphy_dsp_rdb.h
    @brief RDB File for BRPHY_DSP

    @version 2019Jan21_rdb
*/

#ifndef BRPHY_DSP_RDB_H
#define BRPHY_DSP_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t BRPHY_DSP_TAP0_TYPE;
#define BRPHY_DSP_TAP0_AGC_FFE_JAM_DISABLE_MASK (0x8000U)
#define BRPHY_DSP_TAP0_AGC_FFE_JAM_DISABLE_SHIFT (15U)
#define BRPHY_DSP_TAP0_ENABLE_AGC_COARSE_FREEZE_MASK (0x4000U)
#define BRPHY_DSP_TAP0_ENABLE_AGC_COARSE_FREEZE_SHIFT (14U)
#define BRPHY_DSP_TAP0_AGC_COARSE_FREEZE_MASK (0x2000U)
#define BRPHY_DSP_TAP0_AGC_COARSE_FREEZE_SHIFT (13U)
#define BRPHY_DSP_TAP0_ENABLE_AGC_FINE_FREEZE_MASK (0x1000U)
#define BRPHY_DSP_TAP0_ENABLE_AGC_FINE_FREEZE_SHIFT (12U)
#define BRPHY_DSP_TAP0_AGC_FINE_FREEZE_MASK (0x800U)
#define BRPHY_DSP_TAP0_AGC_FINE_FREEZE_SHIFT (11U)
#define BRPHY_DSP_TAP0_BR_PGA_GAIN_MASK (0x700U)
#define BRPHY_DSP_TAP0_BR_PGA_GAIN_SHIFT (8U)
#define BRPHY_DSP_TAP0_AGC_GAIN_OV_MASK (0x80U)
#define BRPHY_DSP_TAP0_AGC_GAIN_OV_SHIFT (7U)
#define BRPHY_DSP_TAP0_AGC_GAIN_VALUE_MASK (0x7fU)
#define BRPHY_DSP_TAP0_AGC_GAIN_VALUE_SHIFT (0U)




typedef uint16_t BRPHY_DSP_TAP1_TYPE;
#define BRPHY_DSP_TAP1_DIG_GAIN_LMS_MODE_MASK (0x2000U)
#define BRPHY_DSP_TAP1_DIG_GAIN_LMS_MODE_SHIFT (13U)
#define BRPHY_DSP_TAP1_IPRF_K_OV_EN_MASK (0x1000U)
#define BRPHY_DSP_TAP1_IPRF_K_OV_EN_SHIFT (12U)
#define BRPHY_DSP_TAP1_IPRF_K_OV_VALUE_MASK (0xf80U)
#define BRPHY_DSP_TAP1_IPRF_K_OV_VALUE_SHIFT (7U)
#define BRPHY_DSP_TAP1_GBT_AGC_TARGET_LVL_MASK (0x70U)
#define BRPHY_DSP_TAP1_GBT_AGC_TARGET_LVL_SHIFT (4U)
#define BRPHY_DSP_TAP1_TX_AGC_TARGET_LVL_MASK (0xfU)
#define BRPHY_DSP_TAP1_TX_AGC_TARGET_LVL_SHIFT (0U)




typedef uint8_t BRPHY_DSP_RESERVED_TYPE;




typedef uint16_t BRPHY_DSP_TAP2_TYPE;
#define BRPHY_DSP_TAP2_MSE_MASK (0xffffU)
#define BRPHY_DSP_TAP2_MSE_SHIFT (0U)




typedef volatile struct COMP_PACKED _BRPHY_DSP_RDBType {
    BRPHY_DSP_TAP0_TYPE tap0_c0; /* OFFSET: 0x0 */
    BRPHY_DSP_TAP0_TYPE tap0_c1; /* OFFSET: 0x2 */
    BRPHY_DSP_TAP0_TYPE tap0_c2; /* OFFSET: 0x4 */
    BRPHY_DSP_TAP0_TYPE tap0_c3; /* OFFSET: 0x6 */
    BRPHY_DSP_TAP1_TYPE tap1; /* OFFSET: 0x8 */
    BRPHY_DSP_RESERVED_TYPE rsvd0[6]; /* OFFSET: 0xa */
    BRPHY_DSP_TAP2_TYPE tap2_c0; /* OFFSET: 0x10 */
    BRPHY_DSP_RESERVED_TYPE rsvd1[340]; /* OFFSET: 0x12 */
} BRPHY_DSP_RDBType;


#define BRPHY1_DSP_TAP_BASE             (0x490F2200UL)

#define BRPHY2_DSP_TAP_BASE             (0x494F2200UL)

#define BRPHY3_DSP_TAP_BASE             (0x498F2200UL)

#define BRPHY4_DSP_TAP_BASE             (0x49CF2200UL)



#define BRPHY_DSP_MAX_HW_ID             (4UL)


#define BRPHY0_DSP_TAP_CTRL_BASE        (BRPHY1_DSP_TAP_BASE)


#define BRPHY1_DSP_TAP_CTRL_BASE        (BRPHY2_DSP_TAP_BASE)


#define BRPHY2_DSP_TAP_CTRL_BASE        (BRPHY3_DSP_TAP_BASE)


#define BRPHY3_DSP_TAP_CTRL_BASE        (BRPHY4_DSP_TAP_BASE)


#define BRPHY4_DSP_TAP_CTRL_BASE        (UNDEFINED)

#endif /* BRPHY_DSP_RDB_H */
