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
    @addtogroup grp_mcudrv_impl
    @{

    @limitations None

    @file mcu_drv.c
    @brief MCU driver low level implementation
    This header file contains the MCU driver implementation

    @version 0.86 Imported from docx
*/

#include <inttypes.h>
#include <string.h>
#include <compiler.h>
#include <utils.h>
#include <bcm_err.h>
#include <mcu.h>
#include <mcu_osil.h>
#include <clk.h>
#include <system.h>
#include <crg_rdb.h>
#include <chipmisc_rdb.h>
#include <brphy_mii_rdb.h>
#include <cfg_rdb.h>
#include <dmu_rdb.h>
#include <io_rdb.h>
#include "clk.h"
#include <cortex_mx.h>

/**
    @name MCU Design IDs
    @{
    @brief Design IDs for MCU
*/
#define BRCM_SWDSGN_MCU_MHZ_MACRO                (0x80U) /**< @brief #MCU_MHZ */
#define BRCM_SWDSGN_MCU_DMUQCLKDIV_TYPE          (0x81U) /**< @brief #MCU_DMUQCLKDivType */
#define BRCM_SWDSGN_MCU_DMUQCLKDIV3_MACRO     (0x82U) /**< @brief #MCU_DMUQCLKDIV3 */
#define BRCM_SWDSGN_MCU_DMUQCLKDIV4_MACRO     (0x83U) /**< @brief #MCU_DMUQCLKDIV4 */
#define BRCM_SWDSGN_MCU_DMUQCLKDIV5_MACRO     (0x84U) /**< @brief #MCU_DMUQCLKDIV5 */
#define BRCM_SWDSGN_MCU_DMUQCLKDIV10_MACRO    (0x85U) /**< @brief #MCU_DMUQCLKDIV10 */
#define BRCM_SWDSGN_MCU_DEV_TYPE                 (0x86U) /**< @brief #MCU_DevType */
#define BRCM_SWDSGN_MCU_SVCIO_TYPE               (0x87U) /**< @brief #MCU_SVCIOType */
#define BRCM_SWDSGN_MCU_CHIPMISC_REGS_GLOBAL     (0x88U) /**< @brief #MCU_CHIPMISC_REGS */
#define BRCM_SWDSGN_MCU_CRG_REGS_GLOBAL          (0x89U) /**< @brief #MCU_CRG_REGS */
#define BRCM_SWDSGN_MCU_BRPHY_MII_REGS_GLOBAL    (0x8AU) /**< @brief #MCU_BRPHY_MII_REGS */
#define BRCM_SWDSGN_MCU_DMU_REGS_GLOBAL          (0x8BU) /**< @brief #MCU_DMU_REGS */
#define BRCM_SWDSGN_MCU_CFG_REGS_GLOBAL          (0x8CU) /**< @brief #MCU_CFG_REGS */
#define BRCM_SWDSGN_MCU_CFGENABLEALLQSPIINTERRUPT_PROC      (0x8DU) /**< @brief #MCU_CFGEnableAllQSPIInterrupt */
#define BRCM_SWDSGN_MCU_CFGDISABLEALLQSPIINTERRUPT_PROC     (0x8EU) /**< @brief #MCU_CFGDisableAllQSPIInterrupt */
#define BRCM_SWDSGN_MCU_CFGCLEARSWITCHINTERRUPT_PROC        (0x8FU) /**< @brief #MCU_CFGClearSwitchInterrupt */
#define BRCM_SWDSGN_MCU_DMUPERIPHPOWERUPALL_PROC            (0x90U) /**< @brief #MCU_DMUPeriphPowerUpAll */
#define BRCM_SWDSGN_MCU_DMUGETCPUCLK_PROC                   (0x91U) /**< @brief #MCU_DMUGetCPUClk */
#define BRCM_SWDSGN_MCU_DMUSETQCLK_PROC                     (0x92U) /**< @brief #MCU_DMUSetQClk */
#define BRCM_SWDSGN_MCU_ISINITIALIZED_PROC                  (0x93U) /**< @brief #MCU_IsInitialized */
#define BRCM_SWDSGN_MCU_QSPICLKINIT_PROC                    (0x94U) /**< @brief #MCU_QspiClkInit */
#define BRCM_SWDSGN_MCU_INTCLKINIT_PROC                     (0x95U) /**< @brief #MCU_IntClkInit */
#define BRCM_SWDSGN_MCU_DRV_PERIPHINIT_PROC                 (0x96U) /**< @brief #MCU_DrvPeriphInit */
#define BRCM_SWDSGN_MCU_DRV_GETRESETREASON_PROC             (0x97U) /**< @brief #MCU_DrvGetResetReason */
#define BRCM_SWDSGN_MCU_DRV_GETRESETRAWVALUE_PROC           (0x98U) /**< @brief #MCU_DrvGetResetRawValue */
#define BRCM_SWDSGN_MCU_DRV_GETRESETMODE_PROC               (0x99U) /**< @brief #MCU_DrvGetResetMode */
#define BRCM_SWDSGN_MCU_DRV_SETRESETMODE_PROC               (0x9AU) /**< @brief #MCU_DrvSetResetMode */
#define BRCM_SWDSGN_MCU_DRV_GETFWBOOTINFO_PROC              (0x9BU) /**< @brief #MCU_DrvGetFWBootInfo */
#define BRCM_SWDSGN_MCU_DRV_RESETREQ_PROC                   (0x9CU) /**< @brief #MCU_DrvResetReq */
#define BRCM_SWDSGN_MCU_GETPLLSTATUS_PROC                   (0x9DU) /**< @brief #MCU_GetPllStatus */
#define BRCM_SWDSGN_MCU_ACTIVATEPLLCLOCKS_PROC              (0x9EU) /**< @brief #MCU_ActivatePllClocks */
#define BRCM_SWDSGN_MCU_CLKINIT_PROC                        (0x9FU) /**< @brief #MCU_ClkInit */
#define BRCM_SWDSGN_MCU_RAMSECTIONINIT_PROC                 (0xA0U) /**< @brief #MCU_RamSectionInit */
#define BRCM_SWDSGN_MCU_DRVGETVERSION_PROC                 (0xA1U) /**< @brief #MCU_DrvGetVersion */
#define BRCM_SWDSGN_MCU_INIT_PROC                           (0xA2U) /**< @brief #MCU_Init */
#define BRCM_SWDSGN_MCU_GETCLKFREQ_PROC                 (0xA3U) /**< @brief #MCU_GetClkFreq */
#define BRCM_SWDSGN_MCU_SYSCMDHANDLER_PROC                  (0xA4U) /**< @brief #MCU_SysCmdHandler */
#define BRCM_SWDSGN_MCU_DEV_GLOBAL                          (0xA5U) /**< @brief #MCU_Dev */
#define BRCM_SWDSGN_MCU_CFGDISABLEQSPIERRINTERRUPT_PROC     (0xA6U) /**< @brief #MCU_CFGDisableQSPIErrInterrupt */
#define BRCM_SWDSGN_MCU_DRVINTGETPLLSTATUS_PROC             (0xA7U) /**< @brief #MCU_DrvGetPllStatus */
#define BRCM_SWDSGN_MCU_DRVGETSWITCHPORT2TIMEFIFOMAP_PROC   (0xA8U) /**< @brief #MCU_DrvGetSwitchPort2TimeFifoMap */
#define BRCM_SWDSGN_MCU_DRVENABLESWITCHCPUPORT_PROC         (0xA9U) /**< @brief #MCU_DrvEnableSwitchCPUPort */
#define BRCM_SWDSGN_MCU_DRVDISABLESWITCHCPUPORT_PROC        (0xAAU) /**< @brief #MCU_DrvDisableSwitchCPUPort */
#define BRCM_SWDSGN_MCU_IO_REGS_GLOBAL                      (0xABU) /**< @brief #MCU_IO_REGS */
#define BRCM_SWDSGN_MCU_SCRATCH_RAMDUMP_MODE_MASK_MACRO     (0x95U) /**< @brief #MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK */
#define BRCM_SWDSGN_MCU_GETBLBOOTINFO_PROC                  (0x96U) /**< @brief #MCU_GetBLBootInfo */
#define BRCM_SWDSGN_MCU_GETDEFAULTRESET_PROC                (0x97U) /**< @brief #MCU_GetDefaultResetMode */
#define BRCM_SWDSGN_MCU_GETDWNLDMODE_PROC                   (0x98U) /**< @brief #MCU_GetDwnldMode */
#define BRCM_SWDSGN_MCU_DRV_SETFWBOOTINFO_PROC              (0x99U) /**< @brief #MCU_DrvSetFWBootInfo */
#define BRCM_SWDSGN_MCU_DRV_GETSTACKINGINFO_PROC            (0x9AU) /**< @brief #MCU_DrvGetStackingInfo */
#define BRCM_SWDSGN_MCU_MISC_SPARE_SW_REG4_ADDR_MACRO       (0x9BU) /**< @brief #MCU_MISC_SPARE_SW_REG4_ADDR */
#define BRCM_SWDSGN_MCU_DRV_GETBOOTTIMEINFO_PROC            (0x9CU) /**< @brief #MCU_DrvGetBootTimeInfo */
#define BRCM_SWDSGN_MCU_SETBOOTTIMEINFO_PROC                (0x9DU) /**< @brief #MCU_SetBootTimeInfo */

/** @} */

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
#define MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK       (0x80000000UL) /**< Ramdump mode mask */

/**
   QCLK (QSPI clk) divisor type
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
#define MCU_MHZ(x)          (x * 1000000UL)

/**
    TODO: Use RDB instead of hard coding the address.
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
 */
#define MCU_MISC_SPARE_SW_REG4_ADDR    (0x4a800328) /**< Reference: /system/bcm8956x/inc/sysmap.h */

/**
   @name MCU DMU clock division type
   @{
   QCLK (QSPI clk) divisor type
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
typedef uint32_t MCU_DMUQCLKDivType;
#define MCU_DMUQCLKDIV3                  (DMU_QCLK_DIV_3_VAL) /**< SPI_ROOT_CLK / 3 */
#define MCU_DMUQCLKDIV4                  (DMU_QCLK_DIV_4_VAL) /**< SPI_ROOT_CLK / 4 */
#define MCU_DMUQCLKDIV5                  (DMU_QCLK_DIV_5_VAL) /**< SPI_ROOT_CLK / 5 */
#define MCU_DMUQCLKDIV10                 (DMU_QCLK_DIV_10_VAL) /**< SPI_ROOT_CLK / 10 */
/** @} */

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
   @trace #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
   @trace #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
typedef struct _MCU_DevType {
    uint32_t init;  /**< driver Initialized */
    const MCU_ConfigType *config;
} MCU_DevType;

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static CHIPMISC_RDBType* const MCU_CHIPMISC_REGS = (CHIPMISC_RDBType *)CHIPMISC_BASE;

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static CRG_RDBType * const MCU_CRG_REGS = (CRG_RDBType *)CRG_BASE;

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static BRPHY_MII_RDBType * const MCU_BRPHY_MII_REGS = (BRPHY_MII_RDBType *)BRPHY1_BR_CL22_IEEE_BASE;

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static DMU_RDBType * const MCU_DMU_REGS = (DMU_RDBType *)DMU_BASE;

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static CFG_RDBType *const MCU_CFG_REGS = (CFG_RDBType *)CFG_BASE;

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static IO_RDBType *const MCU_IO_REGS = (IO_RDBType *)IO_BASE;

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
   @trace #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
   @trace #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static MCU_DevType MCU_Dev COMP_SECTION(".data.drivers") = {
    .init = FALSE,
};

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static void MCU_CFGEnableAllQSPIInterrupt(void)
{
    uint32_t mask = CFG_QSPI_IO_CONTROL_ENABLE_MSPI_HALT_SET_TRANSACTION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_MSPI_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_OVERREAD_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_SESSION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_IMPATIENT_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_TRUNCATED_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_FULLNESS_REACHED_MASK;

    MCU_CFG_REGS->qspi_io_control |= mask;
}

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static void MCU_CFGDisableQSPIErrInterrupt(void)
{
    uint32_t mask = CFG_QSPI_IO_CONTROL_ENABLE_MSPI_HALT_SET_TRANSACTION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_OVERREAD_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_IMPATIENT_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_TRUNCATED_MASK;

    MCU_CFG_REGS->qspi_io_control &= ~mask;
}

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static void MCU_CFGClearSwitchInterrupt(void)
{
    MCU_CFG_REGS->sw_intr_clr = CFG_SW_INTR_CLR_CLR_MASK;
}

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static void MCU_DMUPeriphPowerUpAll(void)
{
    MCU_DMU_REGS->pwd_blk1 = 0x0UL;
    MCU_DMU_REGS->pwd_blk2 = 0x0UL;
}

/**
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static int32_t MCU_DMUSetQClk(MCU_DMUQCLKDivType aDiv)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t clkSel = MCU_DMU_REGS->clk_sel;
    clkSel &= ~(DMU_CLK_SEL_QCLK_SEL_MASK);

    switch (aDiv) {
    case MCU_DMUQCLKDIV3:
        clkSel |= (MCU_DMUQCLKDIV3 << DMU_CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case MCU_DMUQCLKDIV4:
        clkSel |= (MCU_DMUQCLKDIV4 << DMU_CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case MCU_DMUQCLKDIV5:
        clkSel |= (MCU_DMUQCLKDIV5 << DMU_CLK_SEL_QCLK_SEL_SHIFT);
        break;
    case MCU_DMUQCLKDIV10:
        clkSel |= (MCU_DMUQCLKDIV10 << DMU_CLK_SEL_QCLK_SEL_SHIFT);
        break;
    default:
        ret = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (ret == BCM_ERR_OK) {
        MCU_DMU_REGS->clk_sel = clkSel;
    }
    return ret;
}

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
   @trace #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
   @trace #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static uint32_t MCU_IsInitialized(void)
{
    return MCU_Dev.init;
}

/**
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static int32_t MCU_QspiClkInit(const MCU_ClkCfgType *const aCfg)
{
    MCU_DMUQCLKDivType div;
    int32_t err = BCM_ERR_OK;

    switch(aCfg->cfgID) {
        case MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ:
            div = MCU_DMUQCLKDIV3;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ:
            div = MCU_DMUQCLKDIV4;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ:
            div = MCU_DMUQCLKDIV5;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ:
            div = MCU_DMUQCLKDIV10;
            break;
        default:
            err = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (err == BCM_ERR_OK) {
        err = MCU_DMUSetQClk(div);
    }
    return err;
}

/**
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static int32_t MCU_IntClkInit(const MCU_ClkCfgType *const aCfg)
{
    int32_t err = BCM_ERR_OK;

    switch (aCfg->clkRef.clkID) {
        case MCU_CLK_ID_QSPI:
        err = MCU_QspiClkInit(aCfg);
        break;
    default:
        err = BCM_ERR_INVAL_PARAMS;
        break;
    }
    return err;
}

/**
   @trace #BRCM_SWARCH_MCU_INIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
static void MCU_DrvPeriphInit(void)
{
    /* Power up all peripherals */
    MCU_DMUPeriphPowerUpAll();

    MCU_CFGEnableAllQSPIInterrupt();
    MCU_CFGDisableQSPIErrInterrupt();
    MCU_CFGClearSwitchInterrupt();

    /* Enable SPIM (SPI1) and MDIO PAD */
    MCU_IO_REGS->io_ctl &= (~IO_CTL_SPIM_DISABLE_MASK & ~IO_CTL_MDIOM_DISABLE_MASK);

    /* Enable reset through watchdog */
    MCU_CRG_REGS->reset_config |= CRG_RESET_CONFIG_WDG_EN_MASK;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static MCU_ResetReasonType MCU_DrvGetResetReason(void)
{
    MCU_ResetReasonType reason = MCU_RESETREASON_UNDEFINED;
    uint32_t reg = MCU_CHIPMISC_REGS->spare_sw_reg0;
    uint32_t resetConfigVal = MCU_CRG_REGS->reset_config;

    if (MCU_IsInitialized() == TRUE) {
        if ((resetConfigVal & CRG_RESET_CONFIG_WDG_STATUS_MASK) != 0UL) {
            reason = MCU_RESETREASON_WD;
        } else if (reg == 0UL) {
            reason = MCU_RESETREASON_POWER_ON;
        } else if ((reg & MCU_FW_BOOT_INFO_SW_RESET_MASK) != 0UL) {
            reason = MCU_RESETREASON_SW;
        } else {
        }
    }
    return reason;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static uint32_t MCU_DrvGetResetRawValue(void)
{
    return MCU_CHIPMISC_REGS->spare_sw_reg0;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static MCU_ResetModeType MCU_DrvGetResetMode(void)
{
    MCU_ResetModeType resetMode;
    uint32_t dwnldMode;
    uint32_t reg =  MCU_CHIPMISC_REGS->spare_sw_reg0;

    dwnldMode = ((reg & MCU_FW_BOOT_INFO_DWNLD_MODE_MASK) >> MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT);
    if ((reg & MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK) != 0UL) {
        resetMode = MCU_RESETMODE_RAMDUMP;
    } else if (dwnldMode != 0UL) {
        switch (dwnldMode) {
            case MCU_FW_BOOT_INFO_DWNLD_MODE_TFTP:
                resetMode = MCU_RESETMODE_DWNLD_TFTP;
                break;
            case MCU_FW_BOOT_INFO_DWNLD_MODE_RAM:
                resetMode = MCU_RESETMODE_DWNLD_RAM;
                break;
            case MCU_FW_BOOT_INFO_DWNLD_MODE_IPC:
                resetMode = MCU_RESETMODE_DWNLD_IPC;
                break;
            default:
                resetMode = MCU_RESETMODE_NONE;
                break;
        }
    } else {
        resetMode = MCU_RESETMODE_NONE;
    }

    return resetMode;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8956X
 */
static void MCU_DrvSetResetMode(MCU_ResetModeType aResetMode)
{
    switch (aResetMode) {
    case MCU_RESETMODE_DWNL:
        MCU_CHIPMISC_REGS->spare_sw_reg0 |= MCU_FW_BOOT_INFO_SW_RESET_MASK
                | (MCU_FW_BOOT_INFO_DWNLD_MODE_IPC << MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT);
        break;
    case MCU_RESETMODE_RAMDUMP:
        MCU_CHIPMISC_REGS->spare_sw_reg0 |= MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK;
        break;
    default:
        break;
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_GETBLBOOTINFO_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/

int32_t MCU_GetBLBootInfo(MCU_BLBootInfoType * const aBootInfo)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t reg;

    if (NULL == aBootInfo) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        reg = MCU_CHIPMISC_REGS->f1_image_status;
        *aBootInfo = (((uint32_t)reg) & MCU_BL_BOOT_INFO_COPY_ID_MASK)
                            >> MCU_BL_BOOT_INFO_COPY_ID_SHIFT;
    }
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETDEFAULTRESETMODE_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/
MCU_ResetModeType MCU_GetDefaultResetMode(void)
{
    return MCU_RESETMODE_DWNLD_IPC;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETDWNLDMODE_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/
MCU_DwnldModeType MCU_GetDwnldMode(void)
{
    MCU_DwnldModeType mode;
    uint32_t reg = MCU_CHIPMISC_REGS->spare_sw_reg0;
    if ((reg & MCU_FW_BOOT_INFO_DWNLD_MODE_PARTIAL_MASK) != 0UL) {
        mode = MCU_DWNLD_MODE_PARTIAL;
    } else {
        mode = MCU_DWNLD_MODE_VIRGIN;
    }

    return mode;
}

/**
  TODO:
  @trace  #BRCM_SWARCH_MCU_DRVSETFWBOOTINFO_PROC
  @trace  #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/
void MCU_DrvSetFWBootInfo(MCU_FWBootInfoType aBootInfo)
{
    MCU_CHIPMISC_REGS->spare_sw_reg3 = (aBootInfo & MCU_SET_FW_BOOT_INFO_MASK);
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static int32_t MCU_DrvGetFWBootInfo(MCU_FWBootInfoType * const aBootInfo)
{
    int32_t err = BCM_ERR_INVAL_PARAMS;
    if (NULL != aBootInfo) {
        *aBootInfo = MCU_CHIPMISC_REGS->spare_sw_reg3;
        err = BCM_ERR_OK;
    }
    return err;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8956X
 */
static void MCU_DrvResetReq(MCU_ResetReqType aResetReq)
{
    if (MCU_IsInitialized() == TRUE) {
        switch (aResetReq) {
        case MCU_RESETREQ_GLOBAL:
            MCU_CRG_REGS->reset_config |= (CRG_RESET_CONFIG_GLOBAL_SRST_EN_MASK
                                        | CRG_RESET_CONFIG_SRST_CHIP_MASK);
            break;
        case MCU_RESETREQ_LOCAL:
            MCU_CFG_REGS->cfg_cpusys_misc |= CFG_CPUSYS_MISC_CPU_SYSRSTREQ_RST_EN_MASK;
            MCU_CHIPMISC_REGS->spare_sw_reg0 |= MCU_FW_BOOT_INFO_SW_RESET_MASK;
            CORTEX_MX_SystemReset();
            break;
        default:
            break;
        }
    }
}

/**
   @trace #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
   @trace #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static MCU_PllStatusType MCU_DrvGetPllStatus(void)
{
    return MCU_PLLSTATUS_LOCKED;
}

/**
   @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
static void MCU_DrvGetVersion(MCU_VersionType *const aVersion)
{
    uint16_t model_rev =  MCU_CHIPMISC_REGS->model_rev_num;
    uint16_t devid_lo =  MCU_CHIPMISC_REGS->deviceid_lo;
    uint16_t devid_hi =  MCU_CHIPMISC_REGS->deviceid_hi;
    uint32_t lsb = MCU_BRPHY_MII_REGS->phy_id_lsb;
    uint32_t msb = MCU_BRPHY_MII_REGS->phy_id_msb;

    aVersion->rev =
        (uint32_t)((model_rev & CHIPMISC_MODEL_REV_NUM_REV_NUM_MASK) >>
            CHIPMISC_MODEL_REV_NUM_REV_NUM_SHIFT);
    aVersion->manuf = (msb & BRPHY_MII_PHY_ID_MSB_OUI_MSB_MASK);
    aVersion->manuf |= ((lsb & BRPHY_MII_PHY_ID_LSB_OUI_LSB_MASK) >>
            BRPHY_MII_PHY_ID_LSB_OUI_LSB_SHIFT) << 19U;

    aVersion->model = (devid_hi << 12UL) | devid_lo;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
static int32_t MCU_DrvGetSwitchPort2TimeFifoMap(uint32_t *const aPort2TimeFifoMap)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL == aPort2TimeFifoMap) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
         aPort2TimeFifoMap[0UL] = 0x00UL;
         aPort2TimeFifoMap[1UL] = 0x01UL;
         aPort2TimeFifoMap[2UL] = 0x02UL;
         aPort2TimeFifoMap[3UL] = 0x03UL;
         aPort2TimeFifoMap[4UL] = 0x04UL;
         aPort2TimeFifoMap[5UL] = 0x05UL;
         aPort2TimeFifoMap[6UL] = 0x06UL;
         aPort2TimeFifoMap[7UL] = 0x10UL;
         aPort2TimeFifoMap[8UL] = 0x07UL;
         aPort2TimeFifoMap[9UL] = 0x08UL;
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
static int32_t MCU_DrvEnableSwitchCPUPort(void)
{
    /* Enable P7(CPU) port - Enable Tx & Rx pause frames, link status,
     * 1G speed */
    MCU_IO_REGS->cpu_gmii_ctl = (IO_CPU_GMII_CTL_RX_PAUSE_MASK
            | IO_CPU_GMII_CTL_TX_PAUSE_MASK
            | IO_CPU_GMII_CTL_LINK_MASK
            | (0x1UL & IO_CPU_GMII_CTL_SPD_MASK));
    return BCM_ERR_OK;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
static int32_t MCU_DrvDisableSwitchCPUPort(void)
{
    /* Disable P7(CPU) port - Clear link status bit */
    MCU_IO_REGS->cpu_gmii_ctl &= ~IO_CPU_GMII_CTL_LINK_MASK;
    return BCM_ERR_OK;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
static int32_t MCU_DrvGetStackingInfo(uint32_t * const aStackingInfo)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    if (NULL != aStackingInfo) {
        *aStackingInfo = *(volatile uint16_t *)MCU_MISC_SPARE_SW_REG4_ADDR;
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
	@trace  #BRCM_SWARCH_MCU_GETBOOTTIMEINFO_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/
static int32_t MCU_DrvGetBootTimeInfo(MCU_BootTimeInfoType *const aBootTime)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aBootTime) {
        aBootTime->blBootTime = MCU_CHIPMISC_REGS->spare_hw_reg4 & 0xFFU;
        aBootTime->xcvrInitTime = (MCU_CHIPMISC_REGS->spare_hw_reg4 & 0xFF00U) >> 8U;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MCU_SETBOOTTIMEINFO_PROC
    @trace #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8956X
*/
int32_t MCU_SetBootTimeInfo(const MCU_BootTimeInfoType *const aBootTime)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aBootTime) {
        MCU_CHIPMISC_REGS->spare_hw_reg4 = ((aBootTime->blBootTime & 0xFFU) |
                ((aBootTime->xcvrInitTime & 0xFFU) << 8U));
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
    @brief Union to avoid MISRA Required error for Type conversion

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef union _MCU_SVCIOType {
    uint8_t *data;
    MCU_IOType *io;
} MCU_SVCIOType;

/**
    @code{.c}
    if aSysIO.mcuIO is not NULL
        if aMagicID is MCU_SVC_MAGIC_ID
            aSysIO.mcuIO.retVal = MCU_CmdHandler(aCmd, aSysIO.mcuIO)
        else
            aSysIO.mcuIO.retVal = BCM_ERR_INVAL_MAGIC
    @endcode

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_KERNEL_HANDLER_BCM8956X
*/
void MCU_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    MCU_SVCIOType mcu;
    mcu.data = aSysIO;
    int32_t ret = BCM_ERR_OK;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_MCU_ID) {
            switch (aCmd) {
            case MCU_CMD_RESET_REQ:
                MCU_DrvResetReq(mcu.io->resetReq);
                break;
            case MCU_CMD_GET_RESET_REASON:
                mcu.io->resetReason = MCU_DrvGetResetReason();
                break;
            case MCU_CMD_GET_RAW_VAL:
                mcu.io->resetRaw = MCU_DrvGetResetRawValue();
                break;
            case MCU_CMD_SET_LPM_MODE:
                break;
            case MCU_CMD_GET_VERSION:
                MCU_DrvGetVersion(mcu.io->version);
                break;
            case MCU_CMD_GET_RESET_MODE:
                mcu.io->resetMode = MCU_DrvGetResetMode();
                break;
            case MCU_CMD_SET_RESET_MODE:
                MCU_DrvSetResetMode(mcu.io->resetMode);
                break;
            case MCU_CMD_GET_FW_BOOT_INFO:
                ret = MCU_DrvGetFWBootInfo(mcu.io->bootInfo);
                break;
            case MCU_CMD_GET_SWT_PORT_2_TIME_FIFO_MAP:
                ret = MCU_DrvGetSwitchPort2TimeFifoMap(mcu.io->swtPort2TimeFifoMap);
                break;
            case MCU_CMD_ENABLE_SWT_CPU_PORT:
                ret = MCU_DrvEnableSwitchCPUPort();
                break;
            case MCU_CMD_DISABLE_SWT_CPU_PORT:
                ret = MCU_DrvDisableSwitchCPUPort();
                break;
            case MCU_CMD_GET_STACKING_INFO:
                ret = MCU_DrvGetStackingInfo(&(mcu.io->stackingInfo));
                break;
            case MCU_CMD_GET_BOOT_TIME:
                ret = MCU_DrvGetBootTimeInfo(mcu.io->bootTime);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            mcu.io->retVal = ret;
        } else {
            mcu.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != ret) {
        const uint32_t values[4UL] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_MCU_ID, 0U, BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC, ret, 4UL, values);
    }
}

/**
    @limitations
    It is assumed that this API is called from priviledged context with
    interrupt disabled

    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8956X
*/
void MCU_Init(const MCU_ConfigType *const aConfig)
{
    if (MCU_IsInitialized() == FALSE) {
        if (aConfig != NULL) {
            MCU_Dev.config = aConfig;
            MCU_DrvPeriphInit();
            MCU_Dev.init = TRUE;
        }
    }
}

/**
   @trace #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
void MCU_RamSectionInit(MCU_RamSectionIDType aID)
{
    if (MCU_IsInitialized() == TRUE) {
        if (aID < MCU_Dev.config->ramCfgTblSize) {
            BCM_MemSet(MCU_Dev.config->ramCfgTbl[aID].baseAddr,
                        (int32_t)MCU_Dev.config->ramCfgTbl[aID].defaultVal,
                        MCU_Dev.config->ramCfgTbl[aID].size);
        }
    }
}

/**
   PLLs are already activate by hardware during initialization
   This is just a dummy implementation which returns success

   @trace #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
   @trace #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8956X
 */
int32_t MCU_ActivatePllClocks(void)
{
    int32_t err = BCM_ERR_OK;

    if (MCU_IsInitialized() == TRUE) {
        if (MCU_DrvGetPllStatus() != MCU_PLLSTATUS_LOCKED) {
            err = BCM_ERR_INVAL_STATE;
        }
    } else {
        err = BCM_ERR_UNINIT;
    }
    return err;
}

/**
   @trace #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
MCU_PllStatusType MCU_GetPllStatus(void)
{
    return MCU_DrvGetPllStatus();
}

/**
   @trace #BRCM_SWARCH_MCU_CLKINIT_PROC
   @trace #BRCM_SWREQ_MCU_INIT_BCM8956X
 */
int32_t MCU_ClkInit(MCU_ClkCfgIDType aCfgID)
{
    int err = BCM_ERR_OK;
    uint32_t i;

    if (MCU_IsInitialized() == TRUE) {
        /* lookup if aCfgID is present
         * in MCU configuration data
         */
        for (i = 0UL; i < MCU_Dev.config->clkCfgTblSize; i++) {
            if (MCU_Dev.config->clkCfgTbl[i].cfgID == aCfgID) {
                err = MCU_IntClkInit(&MCU_Dev.config->clkCfgTbl[i]);
                break;
            }
        }
    } else {
        err = BCM_ERR_UNINIT;
    }
    return err;
}

/**
   @trace #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
   @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
 */
uint32_t MCU_GetClkFreq(MCU_ClkCfgIDType aCfgID)
{
    uint32_t i;
    uint32_t clkFreq;

    clkFreq = 0UL;

    if (MCU_IsInitialized() != TRUE) {
        goto err;
    }

    for (i = 0UL; i < MCU_Dev.config->clkCfgTblSize; ++i) {
        if (MCU_Dev.config->clkCfgTbl[i].cfgID == aCfgID) {
            clkFreq = MCU_Dev.config->clkCfgTbl[i].clkRef.freq;
            break;
        }
    }

err:
    return clkFreq;
}
/** @} */

