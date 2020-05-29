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
/******************************************************************************
 File Name: bl_mmi.c
 Description: This file implements the MMI driver.
******************************************************************************/
#include <inttypes.h>
#include <stdlib.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <bl_mmi.h>
#include <bl_compiler.h>
#include "bl_mmi_drv.h"

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)
/* MMI Register */
static BL_MMI_RegType *const BL_MMI_REGS = (BL_MMI_RegType *const)BL_MMI_BASE;

static BL_COMP_NEVER_INLINE void BL_MMI_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_MIO_ID, aInstanceID, 0x0, aErr, 4UL, values);
}

void BL_MMI_Init(void)
{
#ifdef __BCM8910X__
    *(volatile uint32_t *)(CFG_MISC_CTRL) = 0x00000001; // Diverge MMI MDC/MDIO to top level MDC/MDIO lines (VREG and BRPHY)
    *(volatile uint16_t *)(CHIP_MISC_CHIP_TEST_MODE) = 0x00000100; // Configure CFG_MISC_CTRL module to drive BRPHY mdio_busy to value 1 	91
    BL_MMI_REGS->CTRL = 0x00000281; // Configure MMI module with preamble, MDC = 25MHz
#endif
#ifdef __BCM89559G__
    BL_MMI_REGS->CTRL = 0x00000283;
    BL_MMI_Write(BL_MMI_ACCESSMODE_CL45, 0x0U, 0x1, 0xA011, 0x1030); // Set MDIO to be 4mA driver in Denali
#endif
}

void BL_MMI_DeInit(void)
{
#ifdef __BCM8910X__
    *(volatile uint32_t *)(CFG_MISC_CTRL) = 0x00000000; // Diverge MMI MDC/MDIO to top level MDC/MDIO lines (VREG and BRPHY)
    *(volatile uint16_t *)(CHIP_MISC_CHIP_TEST_MODE) = 0x00000000; // Configure CFG_MISC_CTRL module to drive BRPHY mdio_busy to value 1 	91
#endif
#ifdef __BCM89559G__
    BL_MMI_REGS->CTRL = 0;
#endif
}

void BL_MMI_Write(BL_MMI_AccessModeType aMode,
                  uint8_t aPhy,
                  uint8_t aDev,
                  uint16_t aReg,
                  uint16_t aValue)
{
    uint32_t regVal;
    if (BL_MMI_ACCESSMODE_CL22 == aMode) {
        BL_LOG_VERB("mmi wr: phyaddr: %02x raddr: %02x val: %04x\n", aPhy, aReg, aValue);
        BL_MMI_REGS->CMD = ((1UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL22_WRITE << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((aPhy << MMI_CMD_PHY_ADDR_SHIFT) & MMI_CMD_PHY_ADDR_MASK)
            | (((uint8_t)aReg << MMI_CMD_REG_ADDR_SHIFT) & MMI_CMD_REG_ADDR_MASK)
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK)
            | (aValue & MMI_CMD_DATA_MASK);
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}
    } else if (BL_MMI_ACCESSMODE_CL45 == aMode) {
        BL_LOG_VERB("mmi wr: phyaddr: %02x devaddr: %02x raddr: %04x val: %04x\n", aPhy, aDev, aReg, aValue);
        regVal = ((0UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL45_WRITE_ADDR << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((((uint32_t)aPhy & (MMI_CMD_PHY_ADDR_MASK >> MMI_CMD_PHY_ADDR_SHIFT)) << MMI_CMD_PHY_ADDR_SHIFT))
            | ((((uint32_t)aDev & (MMI_CMD_REG_ADDR_MASK >> MMI_CMD_REG_ADDR_SHIFT)) << MMI_CMD_REG_ADDR_SHIFT))
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK)
            | (aReg & MMI_CMD_DATA_MASK);
        BL_MMI_REGS->CMD = regVal;
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}

        regVal = ((0UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL45_WRITE_DATA << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((((uint32_t)aPhy & (MMI_CMD_PHY_ADDR_MASK >> MMI_CMD_PHY_ADDR_SHIFT)) << MMI_CMD_PHY_ADDR_SHIFT))
            | ((((uint32_t)aDev & (MMI_CMD_REG_ADDR_MASK >> MMI_CMD_REG_ADDR_SHIFT)) << MMI_CMD_REG_ADDR_SHIFT))
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK)
            | (aValue & MMI_CMD_DATA_MASK);
        BL_MMI_REGS->CMD = regVal;
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}
    } else {
        BL_MMI_ReportError(aMode, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }
}

uint16_t BL_MMI_Read(BL_MMI_AccessModeType aMode,
                     uint8_t aPhy,
                     uint8_t aDev,
                     uint16_t aReg)
{
    uint32_t regVal;

    if (BL_MMI_ACCESSMODE_CL22 == aMode) {
        BL_MMI_REGS->CMD = ((1UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL22_READ << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((aPhy << MMI_CMD_PHY_ADDR_SHIFT) & MMI_CMD_PHY_ADDR_MASK)
            | (((uint8_t)aReg << MMI_CMD_REG_ADDR_SHIFT) & MMI_CMD_REG_ADDR_MASK)
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK);
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}
    } else if (BL_MMI_ACCESSMODE_CL45 == aMode) {
        regVal = ((0UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL45_WRITE_ADDR << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((((uint32_t)aPhy & (MMI_CMD_PHY_ADDR_MASK >> MMI_CMD_PHY_ADDR_SHIFT)) << MMI_CMD_PHY_ADDR_SHIFT))
            | ((((uint32_t)aDev & (MMI_CMD_REG_ADDR_MASK >> MMI_CMD_REG_ADDR_SHIFT)) << MMI_CMD_REG_ADDR_SHIFT))
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK)
            | (aReg & MMI_CMD_DATA_MASK);
        BL_MMI_REGS->CMD = regVal;
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}

        regVal = ((0UL << MMI_CMD_START_SHIFT) & MMI_CMD_START_MASK)
            | ((MMI_CMD_OPCODE_CL45_READ << MMI_CMD_OP_CODE_SHIFT) & MMI_CMD_OP_CODE_MASK)
            | ((((uint32_t)aPhy & (MMI_CMD_PHY_ADDR_MASK >> MMI_CMD_PHY_ADDR_SHIFT)) << MMI_CMD_PHY_ADDR_SHIFT))
            | ((((uint32_t)aDev & (MMI_CMD_REG_ADDR_MASK >> MMI_CMD_REG_ADDR_SHIFT)) << MMI_CMD_REG_ADDR_SHIFT))
            | ((0x2UL << MMI_CMD_TA_SHIFT) & MMI_CMD_TA_MASK);
        BL_MMI_REGS->CMD = regVal;
        while (0UL != (MMI_CTRL_BUSY & BL_MMI_REGS->CTRL)) {}
    } else {
        BL_MMI_ReportError(aMode, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }

    return ((uint16_t)(BL_MMI_REGS->CMD & MMI_CMD_DATA_MASK));
}
