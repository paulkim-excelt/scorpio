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
/**
 * @file bl_dmu.h
 *
 * @brief BCM8956X DMU block
 *
 * This file defines BCM8956X DMU block
 */

#ifndef BL_BCM8956X_DMU_H
#define BL_BCM8956X_DMU_H

#define BL_MHZ(x)          (x * 1000000UL)

/**
 * CPU clock source selection
 */
typedef uint32_t BL_DMU_CPUCLKSrcType;
#define BL_DMU_CPUCLK_SRC_REFCLK           (0x0UL) /**< 25Mhz ref clock */
#define BL_DMU_CPUCLK_SRC_TOP_PLL          (0x1UL) /**< TOP PLL (250Mhz) */
#define BL_DMU_CPUCLK_SRC_TOP_PLL_BY_2     (0x2UL) /**< TOP PLL / 2 */
#define BL_DMU_CPUCLK_SRC_TOP_PLL_BY_4     (0x3UL) /**< TOP PLL / 4 */

/**
 * HCLK divisor type
 */
typedef uint32_t BL_DMU_HCLKDivType;
#define BL_DMU_HCLK_DIV_1                  (0x0UL) /**< 1 x CPUCLK */
#define BL_DMU_HCLK_DIV_2                  (0x1UL) /**< CPUCLK / 2 */
#define BL_DMU_HCLK_DIV_3                  (0x2UL) /**< CPUCLK / 3 */
#define BL_DMU_HCLK_DIV_4                  (0x3UL) /**< CPUCLK / 4 */

/**
 * PCLK divisor type
 */
typedef uint32_t BL_DMU_PCLKDivType;
#define BL_DMU_PCLK_DIV_1                  (0x0UL) /**< 1 x HCLK */
#define BL_DMU_PCLK_DIV_2                  (0x1UL) /**< HCLK / 2 */
#define BL_DMU_PCLK_DIV_3                  (0x2UL) /**< HCLK / 3 */

/**
 * QCLK (QSPI clk) divisor type
 */
typedef uint32_t BL_DMU_QCLKDivType;
#define BL_DMU_QCLK_DIV_3                  (0x0UL) /**< CPUCLK / 3 */
#define BL_DMU_QCLK_DIV_4                  (0x1UL) /**< CPUCLK / 4 */
#define BL_DMU_QCLK_DIV_5                  (0x2UL) /**< CPUCLK / 5 */
#define BL_DMU_QCLK_DIV_10                 (0x3UL) /**< CPUCLK / 10 */

/**
 * DMU registers structure
 */
typedef volatile struct {
    uint32_t STATUS;
    uint32_t CLK_SEL;
#define CLK_SEL_CPUCLK_SEL_MASK             (0x00000003UL)
#define CLK_SEL_CPUCLK_SEL_SHIFT            (0UL)
#define CLK_SEL_CPUCLK_SRC_REFCLK           (0x0UL)
#define CLK_SEL_CPUCLK_SRC_TOP_PLL          (0x1UL)
#define CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_2     (0x2UL)
#define CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_4     (0x3UL)
#define CLK_SEL_QCLK_SEL_MASK               (0x0000000cUL)
#define CLK_SEL_QCLK_SEL_SHIFT              (2UL)
#define CLK_SEL_QCLK_DIV_3                  (0x0UL)
#define CLK_SEL_QCLK_DIV_4                  (0x1UL)
#define CLK_SEL_QCLK_DIV_5                  (0x2UL)
#define CLK_SEL_QCLK_DIV_10                 (0x3UL)
#define CLK_SEL_HCLK_SEL_MASK               (0x00000030UL)
#define CLK_SEL_HCLK_SEL_SHIFT              (4UL)
#define CLK_SEL_HCLK_DIV_1                  (0x0UL)
#define CLK_SEL_HCLK_DIV_2                  (0x1UL)
#define CLK_SEL_HCLK_DIV_3                  (0x2UL)
#define CLK_SEL_HCLK_DIV_4                  (0x3UL)
#define CLK_SEL_PCLK_SEL_MASK               (0x000000c0UL)
#define CLK_SEL_PCLK_SEL_SHIFT              (6UL)
#define CLK_SEL_PCLK_DIV_1                  (0x0UL)
#define CLK_SEL_PCLK_DIV_2                  (0x1UL)
#define CLK_SEL_PCLK_DIV_3                  (0x2UL)

    uint32_t CLK_OUT_SEL;
    uint32_t TIMER_ENABLE_SEL;
    uint32_t PM;
    uint32_t RSVD[27];
    uint32_t PWD_BLK1;
    uint32_t PWD_BLK2;
} BL_DMU_RegsType;

void BL_DMU_PeriphPowerUpAll(void);
int32_t BL_DMU_SetCPUClk(BL_DMU_CPUCLKSrcType aSrc);
int32_t BL_DMU_SetHClk(BL_DMU_HCLKDivType aDiv);
int32_t BL_DMU_SetPClk(BL_DMU_PCLKDivType aDiv);
int32_t BL_DMU_SetQClk(BL_DMU_QCLKDivType aDiv);
uint32_t BL_DMU_GetCPUClk(void);
#endif /* BL_BCM8956X_DMU_H */
