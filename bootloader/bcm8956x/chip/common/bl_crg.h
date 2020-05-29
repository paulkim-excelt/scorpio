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

#ifndef BL_CRG_H
#define BL_CRG_H

typedef volatile struct {
    uint16_t XTAL_CONFIG;
    uint16_t PLL_CONFIG1;
    uint16_t PLL_CONFIG2;
    uint16_t PLL_NDIV;
    uint16_t PLL_CTRL0;
    uint16_t PLL_CTRL1;
    uint16_t PLL_CTRL2;
    uint16_t PLL_CTRL3;
    uint16_t PLL_MDIV_CH01;
    uint16_t PLL_MDIV_CH23;
    uint16_t PLL_SSC_CONFIG1;
    uint16_t PLL_SSC_CONFIG2;
    uint16_t PLL_SSC_STEP;
    uint16_t PLL_STATUS;
    uint16_t CLOCK_CONFIG1;
    uint16_t CLOCK_CONFIG2;
    uint16_t IDDQ_CHIP;
    uint16_t IDDQ_CONFIG;
    uint16_t RESET_CONFIG;
#define RESET_CONFIG_SRST_CHIP_MASK             (0x0001U)
#define RESET_CONFIG_SRST_CHIP_SHIFT            (0U)
#define RESET_CONFIG_SRST_SOC_MASK              (0x0008U)
#define RESET_CONFIG_SRST_SOC_SHIFT             (3U)
#define RESET_CONFIG_GLOBAL_SRST_EN_MASK        (0x8000U)
#define RESET_CONFIG_GLOBAL_SRST_EN_SHIFT       (15U)
    uint16_t RSVD[2028];
    uint16_t SCRATCH_REG;
} BL_CRG_RegsType;

#endif /* BL_CRG_H */
