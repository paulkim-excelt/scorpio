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
#include <bl_utils.h>
#include <bl_log.h>
#include <bl_bcm_err.h>
#include <bl_cntrl.h>
#include <eth_xcvr.h>
#include <bl_chip_config.h>
#include <bcm_time.h>
#include <mcu_ext.h>
#include <brphy_cl45ven_rdb.h>
#include <chipmisc_rdb.h>
#include <io_rdb.h>
#include <ieee_cl22_rdb.h>
#include <crg_rdb.h>
#include <sgp_pll2_rdb.h>
#include <sgp_xgxsblk0_rdb.h>

#define BRPHY_CL45VEN_0x102REG_MANUAL_FORCE_MASK   (0x8000U)
#define BRPHY_CL45VEN_0x102REG_MASTER_MASK         (0x4000U)
#define BRPHY_CL45VEN_0x102REG_100MBPS_MASK        (0x0800U)

static XGXSBLK0_RDBType *const BL_XGXSBLK0_RDB_REG = (XGXSBLK0_RDBType *)BLK0_BASE;
static SGP_XGXSBLK0_RDBType *const SGP_XGXSBLK0_REG = (SGP_XGXSBLK0_RDBType *)SGP_BLK0_BASE;
static IO_RDBType *const BL_IO_RDB_REG = (IO_RDBType *)IO_BASE;
static CRG_RDBType *const BL_CRG_RDB_REG = (CRG_RDBType *)CRG_BASE;
static SGP_PLL2_RDBType *const SGP_PLL2_REG = (SGP_PLL2_RDBType *)SGP_PLL2_BASE;

#define BL_REG_READ32(addr)         \
    (*((const volatile uint32_t *) (addr)))

#define BL_REG_WRITE32(addr, val)   \
    (*((volatile uint32_t *) (addr)) = (uint32_t)(val))

#define BL_REG_READ16(addr)         \
    (*((const volatile uint16_t *) (addr)))

#define BL_REG_WRITE16(addr, val)   \
    (*((volatile uint16_t *) (addr)) = (uint16_t)(val))

#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)
static BL_COMP_NEVER_INLINE void BL_XCVR_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_XVR_ID, aInstanceID, 0UL, aErr, 4UL, values);
}

static void BL_DelayUs(uint32_t aDelay)
{
    BCM_CpuNDelay(aDelay * 1000UL);
}

void BL_XCVR_SetPhyMasterMode(uint32_t aPortIdx, ETHXCVR_BooleanType aMasterMode)
{
    uint32_t base = 0UL;
    uint32_t offset = 0x102;

    switch (aPortIdx) {
    case 1:
        base = BRPHY1_CL45VEN_BASE;
        break;
    case 2:
        base = BRPHY2_CL45VEN_BASE;
        break;
    case 3:
        base = BRPHY3_CL45VEN_BASE;
        break;
    case 4:
        base = BRPHY4_CL45VEN_BASE;
        break;
    default:
        break;
    }
    if (0U != base) {
        switch (aMasterMode) {
        case ETHXCVR_BOOLEAN_TRUE:
            BL_REG_WRITE16(base + offset, BRPHY_CL45VEN_0x102REG_MANUAL_FORCE_MASK |\
                    BRPHY_CL45VEN_0x102REG_MASTER_MASK |\
                    BRPHY_CL45VEN_0x102REG_100MBPS_MASK);
            break;
        case ETHXCVR_BOOLEAN_FALSE:
            BL_REG_WRITE16(base + offset, BRPHY_CL45VEN_0x102REG_MANUAL_FORCE_MASK |\
                    BRPHY_CL45VEN_0x102REG_100MBPS_MASK);
            break;
        default:
            break;
        }
    }
}

static void BL_XCVR_SGBPowerUp(void)
{
    uint16_t reg;
    reg = BL_CHIPMISC_REGS->sgmii_pcie_pwrdwn;

    if (0UL != (reg & CHIPMISC_SGMII_PCIE_PWRDWN_PWRDWN_MASK)) {
        /* Power up the B0 serdes */
        BL_CHIPMISC_REGS->sgmii_pcie_pwrdwn = 0UL;

        BL_CHIPMISC_REGS->sgmii_pcie_ext_ctl = 0x20U;
        BL_DelayUs(1000UL);

        SGP_XGXSBLK0_REG->xgxscontrol = 0x0C2FU;
        SGP_PLL2_REG->ctrl3 = 0x3D09U;
        SGP_PLL2_REG->ctrl4 = 0x00FAU;
        SGP_PLL2_REG->ctrl5 = 0x09C4U;

        /* enable the PLL Sequencer */
        SGP_XGXSBLK0_REG->xgxscontrol = 0x2C2FU;
        BL_DelayUs(1000UL);
    }
}

static void BL_XCVR_SGAPowerUp(void)
{
    uint16_t reg;

    /* check if SGMII is already powered up
     * and PLL sequence is locked
     */
    reg = BL_CHIPMISC_REGS->sgmii_pwrdwn;
    if (0U != (reg & CHIPMISC_SGMII_PWRDWN_PWRDWN_MASK)) {
        /* Power up the SGMII */
        BL_CHIPMISC_REGS->sgmii_pwrdwn = 0x0;
        BL_DelayUs(1000UL);

        /* Workaround for PLL lock issue
         * PLL2_ctrl3 - 16.h3D09 //setup PLL contrl parameters
         * PLL2_ctrl4 - 16.h00FA
         * PLL2_ctrl5 - 16.h09C4
         */
        BL_REG_WRITE16(PLL2_CTRL3, 0x3D09);
        BL_REG_WRITE16(PLL2_CTRL4, 0xFA);
        BL_REG_WRITE16(PLL2_CTRL5, 0x9C4);
        /* configure the TCXO:
         * txcko divide by 2;
         * Automatic 10+G Tx input fifo reset enable;
         * 8B/10B encoder/decoder enable for 4-lane XGXS/XAUI modes;
         * Comma detect enable;
         * Operational mode:IndLane_OS4
         */
        BL_XGXSBLK0_RDB_REG->xgxscontrol = 0x62F;

        /* Enable Pll sequencer */
        BL_XGXSBLK0_RDB_REG->xgxscontrol = 0x262F;

        /* wait for PLL sequnece to finish */
        do {
            reg = BL_XGXSBLK0_RDB_REG->xgxsstatus;
        } while (0UL == (reg & XGXSBLK0_XGXSSTATUS_SEQUENCER_DONE_MASK));

        /* check if sequnece has passed */
        if ((XGXSBLK0_XGXSSTATUS_SEQUENCER_PASS_MASK & reg) != XGXSBLK0_XGXSSTATUS_SEQUENCER_PASS_MASK) {
            BL_XCVR_ReportError(BL_BCM_ERR_TIME_OUT, 0xFF, 0UL, 0UL, 0UL, __LINE__);
        }
    }
}

static void BL_XCVR_ConfigSGMII(uint32_t aPortIdx, ETHXCVR_SpeedType aSpeed,
                                ETHXCVR_BooleanType aAutoNegEnable)
{
    uint16_t reg;
    uint32_t sgmiiBase = 0UL;
    uint32_t digitalBase = 0UL;
    uint32_t digital5Base = 0UL;
    int32_t ret = BL_BCM_ERR_OK;

    reg = BL_IO_RDB_REG->sgmii_rgmii_ctl;
    switch (aPortIdx) {
        case 0UL:
            sgmiiBase = SG1_CL22_B0_BASE;
            break;
        case 1UL:
            reg |= IO_SGMII_RGMII_CTL_SEL_P1_MASK;
            sgmiiBase = SG2_CL22_B0_BASE;
            break; 
        case 5UL:
            sgmiiBase = SG3_CL22_B0_BASE;
            reg &= ~IO_SGMII_RGMII_CTL_SEL_RGMII2_MASK;
            break;
        case 6UL:
            reg &= ~IO_SGMII_RGMII_CTL_SEL_P8_P6_PCIE_MASK;
            sgmiiBase = SG0_CL22_B0_BASE;
            break;
        case 8UL:
            reg |= IO_SGMII_RGMII_CTL_SEL_P8_P6_PCIE_MASK;
            reg &= ~IO_SGMII_RGMII_CTL_SEL_RGMII1_MASK;
            sgmiiBase = SG0_CL22_B0_BASE;
            break;
        default:
            ret = BL_BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (BL_BCM_ERR_OK != ret) {
        goto err_exit;
    }

    /* Power up SGMII block and enable the PLL sequencer:
     * All the A0 based serdes have common sequence
     * B0 serdes (SGMII/PCIE serdes) have different sequence
     */
    if (SG0_CL22_B0_BASE == sgmiiBase) {
        BL_XCVR_SGBPowerUp();
    } else {
        BL_XCVR_SGAPowerUp();
    }

    digitalBase = sgmiiBase + 0x10600;
    digital5Base = digitalBase + 0x80;

    /* configure top level mux */
    BL_IO_RDB_REG->sgmii_rgmii_ctl = reg;
    /* enable crc checker;disable comma detection */
    BL_REG_WRITE16(digitalBase, 0x100);   /* SGx_Digital_Control1000X1 = 0x0 */

    if (ETHXCVR_SPEED_100MBPS == aSpeed) {
        /* osr mode = osx5 */
        BL_REG_WRITE16(digital5Base + 0x14, 0x3U);
        /* SGx_CL22_B0_MIICntl: full-duplex, autoneg enabled, 100M */
        BL_REG_WRITE16(sgmiiBase, 0x2100U);
        BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x2100);
    } else if (ETHXCVR_SPEED_1000MBPS == aSpeed) {
        /* osr mode = osx5 */
        BL_REG_WRITE16(digital5Base + 0x14, 0x3U);
        /* SGx_CL22_B0_MIICntl: full-duplex, autoneg enabled, 1000M */
        if (ETHXCVR_BOOLEAN_TRUE == aAutoNegEnable) {
            BL_REG_WRITE16(sgmiiBase, 0x1140U);
            BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x1140);
        } else {
            BL_REG_WRITE16(sgmiiBase, 0x140U);
            BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x140);
        }
    } else if (ETHXCVR_SPEED_2500MBPS == aSpeed) {
        BL_REG_WRITE16(digitalBase + 0x1E, 0x8300);
        /* dr_2500BRCM_X1 : 0x10 */
        BL_REG_WRITE16(digitalBase + 0x10, 0x10);
        BL_REG_WRITE16(digital5Base + 0x1E, 0x8340);
        /*  osr mode = osx2 */
        BL_REG_WRITE16(digital5Base + 0x14, 0x1);
        /* set block address */
        BL_XGXSBLK0_RDB_REG->blockaddress = 0x0;
        if (ETHXCVR_BOOLEAN_TRUE == aAutoNegEnable) {
            BL_REG_WRITE16(sgmiiBase, 0x1140U);
            BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x1140);
        } else {
            BL_REG_WRITE16(sgmiiBase, 0x140U);
            BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x140);
        }
    } else if (ETHXCVR_SPEED_5000MBPS == aSpeed) {
        /* SG0_DIGITAL_BlockAddress */
        BL_REG_WRITE16(digitalBase + 0x1E, 0x8300);
        /* SG0_DIGITAL_Control1000X1 - enable crc checker;disable comma detection */
        BL_REG_WRITE16(digitalBase + 0x00, 0x0180);
        /* SG0_DIGITAL_Misc1 : dr_5000BRCM_X1 = 0x11 */
        BL_REG_WRITE16(digitalBase + 0x10, 0x0011);
        /* SG0_DIGITAL5_BlockAddress */
        BL_REG_WRITE16(digital5Base + 0x1E, 0x8340);
        /* SG0_DIGITAL5_Misc8 : osr mode = osx5 */
        BL_REG_WRITE16(digital5Base + 0x14, 0x0000);
        /* BLK0_BlockAddress */
        BL_XGXSBLK0_RDB_REG->blockaddress = 0x0000;
        /* SG0_CL22_B0_MIICntl : SGMII 1000 Mb/s;full duplex */
        BL_REG_WRITE16(sgmiiBase, 0x140U);
        /* SG0_COMBO_IEEE0_MIICntl */
        BL_REG_WRITE16(sgmiiBase + 0x1FFC0, 0x140);
    }
    else {
        ret = BL_BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

err_exit:
    return;
}

static void BL_XCVR_ConfigRGMII(uint32_t aPortIdx, ETHXCVR_SpeedType aSpeed)
{
    uint16_t sgmiiRgmiiCtl;
    uint16_t rgmiiCtl;
    uint16_t rgmiiSpd = 0U;
    uint32_t line = __LINE__;
    int32_t ret = BL_BCM_ERR_OK;

    sgmiiRgmiiCtl = BL_IO_RDB_REG->sgmii_rgmii_ctl;;
    switch (aSpeed) {
    case ETHXCVR_SPEED_10MBPS:
        rgmiiSpd = 0U;
        break;
    case ETHXCVR_SPEED_100MBPS:
        rgmiiSpd = 1U;
        break;
    case ETHXCVR_SPEED_1000MBPS:
        rgmiiSpd = 2U;
        break;
    case ETHXCVR_SPEED_2500MBPS:
        break;
    default:
        ret = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (BL_BCM_ERR_OK == ret) {
        switch (aPortIdx) {
        case 5:
            sgmiiRgmiiCtl |= IO_SGMII_RGMII_CTL_SEL_RGMII2_MASK;
            BL_IO_RDB_REG->sgmii_rgmii_ctl = sgmiiRgmiiCtl;
            rgmiiCtl = BL_IO_RDB_REG->rgmii2_gmii_ctl;
            rgmiiCtl &= ~IO_RGMII2_GMII_CTL_RGMII_SPD_MASK;
            rgmiiCtl |= (rgmiiSpd << IO_RGMII2_GMII_CTL_RGMII_SPD_SHIFT);
            rgmiiCtl |= IO_RGMII2_GMII_CTL_RGMII_LINK_MASK;
            BL_IO_RDB_REG->rgmii2_gmii_ctl = rgmiiCtl;
            /* RGMII pad configuration */
            BL_IO_RDB_REG->mii2_config = 0x800E;
            BL_IO_RDB_REG->rgmii2_ctl = 0x00U;  
            break;
        case 8:
            sgmiiRgmiiCtl |= IO_SGMII_RGMII_CTL_SEL_RGMII1_MASK;
            BL_IO_RDB_REG->sgmii_rgmii_ctl = sgmiiRgmiiCtl;
            rgmiiCtl = BL_IO_RDB_REG->rgmii1_gmii_ctl;
            rgmiiCtl &= ~IO_RGMII1_GMII_CTL_RGMII_SPD_MASK;
            rgmiiCtl |= (rgmiiSpd << IO_RGMII1_GMII_CTL_RGMII_SPD_SHIFT);
            rgmiiCtl |= IO_RGMII1_GMII_CTL_RGMII_LINK_MASK;
            BL_IO_RDB_REG->rgmii1_gmii_ctl = rgmiiCtl;
            BL_IO_RDB_REG->mii1_config = 0x800E;
            BL_IO_RDB_REG->rgmii1_ctl = 0x08U;
            break;
        default:
            line = __LINE__;
            ret = BL_BCM_ERR_INVAL_PARAMS;
            break;
        }
    } else {
        line = __LINE__;
    }
    if (BL_BCM_ERR_OK != ret) {
        BL_XCVR_ReportError(ret, aPortIdx, 0UL, 0UL, 0UL, line);
    }
}

static void BL_XCVR_ConfigPCIE(uint32_t aPortIdx, ETHXCVR_SpeedType aSpeed)
{
    uint16_t reg;
    uint16_t ioStrapVal;    

    /* reduce the BAR size.   
     * 0   : bar1Enabled - 0x1
     * 3:1 : bar1EncodedSize
     * 0x0 is 1M, 0x1 is 2M and so on 0x7 is 128M
     */
    BL_REG_WRITE32(0x4C1028C8, 0x00000803);    

#ifdef BL_ENABLE_PCIE_GEN1
    /* Advertize GEN1 speed */  
    BL_REG_WRITE32(0x4C102808, 0x9B7E15BFUL);
#endif

    /* Advertize MSI only (no MSI-X) */
    BL_REG_WRITE32(0x4c102880, 0xd08e8440UL);

    BL_CRG_RDB_REG->reset_config2 = 0x10;

    reg = BL_IO_RDB_REG->sgmii_rgmii_ctl;  
    if (6UL == aPortIdx) {
        reg |= IO_SGMII_RGMII_CTL_SEL_P8_P6_PCIE_MASK;
        ioStrapVal = BL_IO_RDB_REG->sw_ovrd[aPortIdx];
        ioStrapVal &= ~IO_SW_OVRD_SPD_VAL_MASK;
        ioStrapVal |= 0x2U;    /* 1G speed */
        ioStrapVal |= IO_SW_OVRD_LINK_VAL_MASK;
        ioStrapVal |= IO_SW_OVRD_SPD_SEL_MASK;
        ioStrapVal |= IO_SW_OVRD_LINK_SEL_MASK;
    } else if (8UL == aPortIdx) {
        reg &= ~IO_SGMII_RGMII_CTL_SEL_P8_P6_PCIE_MASK; /* P8: Pcie serd, p6: SGMII0 serd */
        reg &= ~IO_SGMII_RGMII_CTL_SEL_RGMII1_MASK;     /* P8: pcie serd (not in RGMII) */
        ioStrapVal = BL_IO_RDB_REG->sw_ovrd[aPortIdx];
        ioStrapVal &= ~IO_SW_OVRD_SPD_VAL_MASK;
        ioStrapVal |= 0x2U;    /* 1G speed */
        ioStrapVal |= IO_SW_OVRD_LINK_VAL_MASK;
        ioStrapVal |= IO_SW_OVRD_SPD_SEL_MASK;
        ioStrapVal |= IO_SW_OVRD_LINK_SEL_MASK;
    } else {
        BL_XCVR_ReportError(BL_BCM_ERR_INVAL_PARAMS, aPortIdx, 0UL, 0UL, 0UL, __LINE__);
        goto err_exit;
    }
    BL_IO_RDB_REG->sgmii_rgmii_ctl = reg;
    BL_IO_RDB_REG->sw_ovrd[aPortIdx] = ioStrapVal;
    /* power up the PCIE */

    BL_CHIPMISC_REGS->sgmii_pcie_pwrdwn = 0x0U;

err_exit:
    return;
}

uint32_t BL_BCM8956X_ConvertToLocalPort(const MCU_ExtendedInfoType *const aStackInfo,
        uint32_t aPortNum,
        uint32_t *const aDoesPortBelongToMe)
{
    uint32_t localPortNum = aPortNum;

    if (aStackInfo->stackingEn == 0U) {
        if (aPortNum <= 8UL) {
            *aDoesPortBelongToMe = BL_TRUE;
        } else {
            *aDoesPortBelongToMe = BL_FALSE;
        }
    } else {
        *aDoesPortBelongToMe = BL_FALSE;
#ifdef __BCM89564G__
        if (aPortNum <= 8UL) {
            switch (aStackInfo->mstSlvMode) {
                case MCU_DEVICE_MASTER:
                    *aDoesPortBelongToMe = BL_TRUE;
                    break;
                default:
                    localPortNum = 8UL;
                    break;
            }
        } else if (aPortNum <= 15UL) {
            switch (aStackInfo->mstSlvMode) {
                case MCU_DEVICE_MASTER:
                    localPortNum = aStackInfo->stackPort0;
                    break;
                case MCU_DEVICE_SLAVE_1:
                    localPortNum = aPortNum - 9UL;
                    *aDoesPortBelongToMe = BL_TRUE;
                    break;
                case MCU_DEVICE_SLAVE_2:
                    localPortNum = aStackInfo->stackPort1;
                    break;
                default:
                    break;
            }
        } else if (aPortNum <= 23UL) {
            switch (aStackInfo->mstSlvMode) {
                case MCU_DEVICE_MASTER:
                    localPortNum = aStackInfo->stackPort1;
                    break;
                case MCU_DEVICE_SLAVE_1:
                    localPortNum = aStackInfo->stackPort1;
                    break;
                case MCU_DEVICE_SLAVE_2:
                    localPortNum = aPortNum - 18UL;
                    *aDoesPortBelongToMe = BL_TRUE;
                    break;
                default:
                    break;
            }
        }
#endif
    }
    return localPortNum;
}

#ifdef __BCM89564G__
void BL_BCM8956X_AddStackingPort(uint8_t aMstSlvMode,
                                 uint32_t aStackPort,
                                 ETHXCVR_ConfigType *const aCfg,
                                 uint32_t *aNumPorts)
{
    ETHXCVR_PortConfigType config = {
        .id         = 0U,
        .portEnable = ETHXCVR_BOOLEAN_TRUE,
        .speed      = ETHXCVR_SPEED_1000MBPS,
        .autoNeg    = ETHXCVR_BOOLEAN_FALSE,
        .duplex     = ETHXCVR_DUPLEXMODE_FULL,
        .flowControl= ETHXCVR_FLOWCONTROL_NONE,
        .jumbo      = ETHXCVR_BOOLEAN_TRUE,
        .busMode    = ETHXCVR_BUSMODE_SGMII,
        .bus = {
            .cntrlID = 0U,
            .instID  = 0U,
            .driverParams = {{0U}},
        },
        .phy = {
            .phyMedia = ETHXCVR_PHYMEDIA_NONE,
            .driverParams = {{0U}},
        }
    };

    switch (aStackPort) {
        case 5U:
            config.id = 5U;
            config.bus.cntrlID = 3U;
            config.speed = ETHXCVR_SPEED_2500MBPS;
            break;
        case 6U:
            config.id = 6U;
            config.bus.cntrlID = 0U;
            config.speed = ETHXCVR_SPEED_5000MBPS;
            break;
        case 8U:
            config.id = 8U;
            config.bus.cntrlID = 0U;
            if (aMstSlvMode == MCU_DEVICE_SLAVE_1) {
                config.speed = ETHXCVR_SPEED_5000MBPS;
            } else {
                config.speed = ETHXCVR_SPEED_2500MBPS;
            }
            break;
        default:
            break;
    }
    BL_BCM_MemCpy(&aCfg->port[*aNumPorts], &config, sizeof(ETHXCVR_PortConfigType));
    *aNumPorts = *aNumPorts + 1UL;
}
#endif

static void BL_BCM8956X_UpdateXcvrConfig(ETHXCVR_ConfigType *const aOutCfg,
                                       const ETHXCVR_ConfigType *const aInCfg)
{
    uint32_t             inPort  = 0UL;
    uint32_t             outPort = 0UL;
    int32_t              retVal  = BCM_ERR_OK;
    MCU_ExtendedInfoType stackingInfo;
    uint32_t             portNum;
    uint32_t             doesPortBelongToMe;

    /* Retrieve stacking information */
    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (retVal == BCM_ERR_OK) {
        for (; inPort < aInCfg->numPorts; inPort++) {
            portNum = BL_BCM8956X_ConvertToLocalPort(&stackingInfo, aInCfg->port[inPort].id, &doesPortBelongToMe);

            if (BL_TRUE == doesPortBelongToMe) {
                BL_BCM_MemCpy(&aOutCfg->port[outPort], &aInCfg->port[inPort], sizeof(ETHXCVR_PortConfigType));
                aOutCfg->port[outPort].id = portNum;
                outPort++;
            }
        }
        /* Add the stacking ports */
#ifdef __BCM89564G__
        if (stackingInfo.stackingEn == 1U) {
            BL_BCM8956X_AddStackingPort(stackingInfo.mstSlvMode, stackingInfo.stackPort0, aOutCfg, &outPort);
            if (stackingInfo.stackPort1 != 0U) {
                BL_BCM8956X_AddStackingPort(stackingInfo.mstSlvMode, stackingInfo.stackPort1, aOutCfg, &outPort);
            }
        }

#endif
        aOutCfg->numPorts = outPort;
    } else {
        BL_BCM_MemCpy(aOutCfg, aInCfg, sizeof(ETHXCVR_ConfigType));
    }
}

void BL_BCM8956X_SetXcvrConfig(ETHXCVR_ConfigType * const aCfg,
                               uint8_t * const aMacAddr)
{
    uint32_t i;
    uint32_t numPorts;
    ETHXCVR_ConfigType config;

    ETHXCVR_PortConfigType *portCfg = NULL;
    if (NULL != aCfg) {

        BL_BCM8956X_UpdateXcvrConfig(&config, aCfg);

        numPorts = config.numPorts;
        /* Workaround for HWSCORPIO-569:
         * sgmii3 is broadcast SGMII which overrides
         * the settings of all other SGMII blocks
         * so if P5 is configured in SGMII mode,
         * first configure sgmii3 and then other
         * sgmii blocks
         */
        if (numPorts > 6UL) {
            portCfg = &config.port[5];
            if (ETHXCVR_BUSMODE_SGMII == portCfg->busMode) {
                BL_XCVR_ConfigSGMII(5U, portCfg->speed, portCfg->autoNeg);
            }
        }
        for (i = 0; i < numPorts; i++) {
            portCfg = &config.port[i];
            /* configure the BR Ports */
            switch (portCfg->busMode) {
            case ETHXCVR_BUSMODE_INTG:
                if (ETHXCVR_PHYMEDIA_100BASET1 == portCfg->phy.phyMedia) {
                    BL_XCVR_SetPhyMasterMode(i, portCfg->phy.masterMode);
                } else if (ETHXCVR_PHYMEDIA_100BASETX == portCfg->phy.phyMedia) {
                    /* only port 4 is allowed in BR/100-TX mode */
                    if ((4UL == portCfg->id) && (ETHXCVR_BOOLEAN_TRUE == portCfg->autoNeg)) {
                        /* disable the BR */
                        BL_REG_WRITE16(BRPHY4_BR_CTRL_EXP90, 0x0U);
                        /* set autoneg parameters for 100-TX */
                        BL_REG_WRITE16(BRPHY4_CL45DEV7_AN_AD, 0x2DE1U);
                        /* restart auto-neg */
                        BL_REG_WRITE16(BRPHY4_CL45DEV7_AN_CTRL, 0x1200U);
                    } else {
                        BL_XCVR_ReportError(BL_BCM_ERR_INVAL_PARAMS, portCfg->id, 0UL, 0UL, 0UL, __LINE__);
                    }
                }
                break;
            case ETHXCVR_BUSMODE_SGMII:
                if (5UL != portCfg->id) {
                    BL_XCVR_ConfigSGMII(portCfg->id, portCfg->speed, portCfg->autoNeg);
                }
                break;
            case ETHXCVR_BUSMODE_RGMII:
                BL_XCVR_ConfigRGMII(portCfg->id, portCfg->speed);
                break;
            case ETHXCVR_BUSMODE_PCIE:
                BL_XCVR_ConfigPCIE(portCfg->id, portCfg->speed);
                break;
            default:
                break;
            }
        }
    }
}

