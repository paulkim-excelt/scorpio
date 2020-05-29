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
#include <inttypes.h>
#include <bl_chip_config.h>
#include <bl_bcm_err.h>
#include <bl_clk.h>
#include "bl_cfg.h"
#include "bl_dmu.h"

static BL_DMU_RegsType * const BL_DMU_REGS = (BL_DMU_RegsType *)BL_DMU_BASE;

void BL_DMU_PeriphPowerUpAll(void)
{
    BL_DMU_REGS->PWD_BLK1 = 0x0UL;
    BL_DMU_REGS->PWD_BLK2 = 0x0UL;
}

uint32_t BL_DMU_GetCPUClk(void)
{
    uint32_t freq = BL_MHZ(25UL);
    uint32_t clkSel = BL_DMU_REGS->CLK_SEL;
    clkSel &= (CLK_SEL_CPUCLK_SEL_MASK);
    clkSel >>= CLK_SEL_CPUCLK_SEL_SHIFT;

    switch (clkSel) {
        case CLK_SEL_CPUCLK_SRC_REFCLK:
            break;
        case CLK_SEL_CPUCLK_SRC_TOP_PLL:
            freq = BL_TOP_PLL_CLOCK;
            break;
        case CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_2:
            freq = BL_TOP_PLL_CLOCK / 2UL;
            break;
        case CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_4:
            freq = BL_TOP_PLL_CLOCK / 4UL;
            break;
        default:
            break;
    }
    return freq;
}

int32_t BL_DMU_SetCPUClk(BL_DMU_CPUCLKSrcType aSrc)
{
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t clkSel = BL_DMU_REGS->CLK_SEL;
    clkSel &= ~(CLK_SEL_CPUCLK_SEL_MASK);

    switch (aSrc) {
        case BL_DMU_CPUCLK_SRC_REFCLK:
            clkSel |= (CLK_SEL_CPUCLK_SRC_REFCLK << CLK_SEL_CPUCLK_SEL_SHIFT);
            break;
        case BL_DMU_CPUCLK_SRC_TOP_PLL:
            clkSel |= (CLK_SEL_CPUCLK_SRC_TOP_PLL << CLK_SEL_CPUCLK_SEL_SHIFT);
            break;
        case BL_DMU_CPUCLK_SRC_TOP_PLL_BY_2:
            clkSel |= (CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_2 << CLK_SEL_CPUCLK_SEL_SHIFT);
            break;
        case BL_DMU_CPUCLK_SRC_TOP_PLL_BY_4:
            clkSel |= (CLK_SEL_CPUCLK_SRC_TOP_PLL_BY_4 << CLK_SEL_CPUCLK_SEL_SHIFT);
            break;
        default:
            ret = BL_BCM_ERR_INVAL_PARAMS;
            break;
    }
    if (ret == BL_BCM_ERR_OK) {
         BL_DMU_REGS->CLK_SEL = clkSel;
    }

    return ret;
}

int32_t BL_DMU_SetQClk(BL_DMU_QCLKDivType aDiv)
{
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t clkSel = BL_DMU_REGS->CLK_SEL;
    clkSel &= ~(CLK_SEL_QCLK_SEL_MASK);

    switch (aDiv) {
    case BL_DMU_QCLK_DIV_3:
        clkSel |= (CLK_SEL_QCLK_DIV_3 << CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case BL_DMU_QCLK_DIV_4:
        clkSel |= (CLK_SEL_QCLK_DIV_4 << CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case BL_DMU_QCLK_DIV_5:
        clkSel |= (CLK_SEL_QCLK_DIV_5 << CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case BL_DMU_QCLK_DIV_10:
        clkSel |= (CLK_SEL_QCLK_DIV_10 << CLK_SEL_QCLK_SEL_SHIFT);
        break;
    default:
        ret = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (ret == BL_BCM_ERR_OK) {
        BL_DMU_REGS->CLK_SEL = clkSel;
    }
    return ret;
}

int32_t BL_DMU_SetHClk(BL_DMU_HCLKDivType aDiv)
{
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t clkSel = BL_DMU_REGS->CLK_SEL;
    clkSel &= ~(CLK_SEL_HCLK_SEL_MASK);

    switch (aDiv) {
    case BL_DMU_HCLK_DIV_1:
        clkSel |= (CLK_SEL_HCLK_DIV_1 << CLK_SEL_HCLK_SEL_SHIFT);
        break;
    case BL_DMU_HCLK_DIV_2:
        clkSel |= (CLK_SEL_HCLK_DIV_2 << CLK_SEL_HCLK_SEL_SHIFT);
        break;
    case BL_DMU_HCLK_DIV_3:
        clkSel |= (CLK_SEL_HCLK_DIV_3 << CLK_SEL_HCLK_SEL_SHIFT);
        break;
    case BL_DMU_HCLK_DIV_4:
        clkSel |= (CLK_SEL_HCLK_DIV_4 << CLK_SEL_HCLK_SEL_SHIFT);
        break;
    default:
        ret = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }
    if (ret == BL_BCM_ERR_OK) {
        BL_DMU_REGS->CLK_SEL = clkSel;
    }

    return ret;
}

int32_t BL_DMU_SetPClk(BL_DMU_PCLKDivType aDiv)
{
    int32_t ret = BL_BCM_ERR_OK;
    uint32_t clkSel = BL_DMU_REGS->CLK_SEL;
    clkSel &= ~(CLK_SEL_PCLK_SEL_MASK);

    switch (aDiv) {
    case BL_DMU_PCLK_DIV_1:
        clkSel |= (CLK_SEL_PCLK_DIV_1 << CLK_SEL_PCLK_SEL_SHIFT);
        break;
    case BL_DMU_PCLK_DIV_2:
        clkSel |= (CLK_SEL_PCLK_DIV_2 << CLK_SEL_PCLK_SEL_SHIFT);
        break;
    case BL_DMU_PCLK_DIV_3:
        clkSel |= (CLK_SEL_PCLK_DIV_3 << CLK_SEL_PCLK_SEL_SHIFT);
        break;
    default:
        ret = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }
    if (ret == BL_BCM_ERR_OK) {
        BL_DMU_REGS->CLK_SEL = clkSel;
    }
    return ret;
}
