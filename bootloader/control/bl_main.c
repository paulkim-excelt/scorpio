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

#include <stdint.h>
#include <bl_chip_config.h>
#include <bl_board.h>
#include <bl_utils.h>
#include <bl_log.h>
#include <bl_uart.h>
#include <bl_flash.h>
#include <bl_test.h>
#include <bl_ptm.h>
#include <bl_bcm_err.h>
#include <bcm_time.h>
#include <mcu.h>
#include <otp.h>
#include <bcm_comp.h>
#if defined(BL_ENABLE_DDR)
#include <memc.h>
#endif
#include <bl_downloader.h>
#include <cache.h>
#if defined( __CORTEX_MX__)
#include <cortex_mx.h>
#endif
#include <bl_cntrl.h>
#include <gpt.h>

#ifdef BL_ENABLE_ETH_AMAC
#include <bl_ethernet.h>
#endif
#include <eth_xcvr.h>

#ifdef BL_ENABLE_SECURE_BOOT
#include <rsa.h>
#endif

#ifdef BL_ENABLE_SLAVE_BOOT
#include <mcu_ext.h>
#endif

/**
    @name BL Main API IDs
    @{
    @brief API IDs for BL Main
*/
#define BRCM_DSGN_BL_CNTRL_MAIN_REPORT_ERROR_PROC          (0xB0U) /**< @brief #BL_CNTRL_MainReportError */
/** @} */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

#define REG_READ(addr)         \
        (*((const volatile uint32_t *) (addr)))

#define BL_NS_TO_MS(ns)     ((ns) / 1000000ULL)

typedef uint32_t BL_CNTRL_ModeType;
#define BL_CNTRL_MODE_ENTER_FW            (0UL)
#define BL_CNTRL_MODE_ENTER_TFTP_DWNLD    (1UL)
#define BL_CNTRL_MODE_ENTER_RAM_DWNLD     (2UL)
#define BL_CNTRL_MODE_ENTER_IPC_DWNLD     (3UL)
#define BL_CNTRL_MODE_ENTER_RAM_DUMP      (4UL)

extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];;
typedef void (*BL_PTM_ImgEntryFuncType)(void);


static BL_COMP_NEVER_INLINE void BL_CNTRL_MainReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_CNTRL_MAIN_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static BL_CNTRL_ModeType BL_MCUResetModeToCNTRLMode(MCU_ResetModeType aMode)
{
    BL_CNTRL_ModeType mode = BL_CNTRL_MODE_ENTER_FW;
    switch (aMode) {
    case MCU_RESETMODE_DWNLD_TFTP:
        mode = BL_CNTRL_MODE_ENTER_TFTP_DWNLD;
        break;
    case MCU_RESETMODE_DWNLD_RAM:
        mode = BL_CNTRL_MODE_ENTER_RAM_DWNLD;
        break;
    case MCU_RESETMODE_DWNLD_IPC:
        mode = BL_CNTRL_MODE_ENTER_IPC_DWNLD;
        break;
    case MCU_RESETMODE_RAMDUMP:
        mode = BL_CNTRL_MODE_ENTER_RAM_DUMP;
        break;
    default:
        break;
    }
    return mode;
}

static uint32_t BL_isDownloadMode(BL_CNTRL_ModeType aMode)
{
    uint32_t isDwnldMode = BL_FALSE;

    if ((aMode == BL_CNTRL_MODE_ENTER_TFTP_DWNLD) ||
            (aMode == BL_CNTRL_MODE_ENTER_RAM_DWNLD) ||
            (aMode == BL_CNTRL_MODE_ENTER_IPC_DWNLD)) {
        isDwnldMode = BL_TRUE;
    }
    return isDwnldMode;
}

int BL_main(void)
{
    int32_t ret = BL_BCM_ERR_OK;
    int32_t ptmInitRet;
    uint32_t entry;
    uint32_t i;
    uint64_t time = 0ULL;
    uint64_t t1 = 0ULL;
    uint64_t t2 = 0ULL;
    MCU_BootTimeInfoType bootTime = {0U};
    MCU_ResetReasonType resetReason;
    MCU_ResetModeType resetMode;
    MCU_DwnldModeType dwnldMode;
    MCU_ResetModeType defaultResetMode = MCU_GetDefaultResetMode();
#if defined(BL_ENABLE_RAMDUMP)
    uint32_t ramdump = BL_FALSE;;
#endif
    uint32_t fwCopyNum = 0UL;
    uint32_t sysCfgCopyNum = 0UL;
    uint32_t flashID;
    MCU_FWBootInfoType fwBootInfo = 0UL;
    MCU_BLBootInfoType blBootInfo;
    BL_CNTRL_ModeType defaultDwnldMode = BL_MCUResetModeToCNTRLMode(defaultResetMode);
    BL_CNTRL_ModeType cntrlMode = BL_CNTRL_MODE_ENTER_FW;
    ETHXCVR_ConfigType xcvrConfig;
    uint32_t size = (uint32_t)sizeof(xcvrConfig);

#if defined(BL_ENABLE_SLAVE_BOOT)
    MCU_ExtendedInfoType stackInfo;
#endif

#ifdef BL_ENABLE_GPT
    /* Some of the ROMs starts the TIM0 is free running mode
     *
     * Fetch the timer value before calling the MCU_Init() which
     * resets the timer block. Save this raw value in ms format
     * into the space register which will be later used by the
     * firwmare for profiling purpose
     */
    time = BL_NS_TO_MS(TIM_GetTimeRaw(BL_TIM_CHANID_0));
    bootTime.blBootTime = time;
#endif

#if defined(BL_ENABLE_DDR)
    MEMC_StateType aState = MEMC_STATE_UNINIT;
#endif
    uint32_t noFlash = BL_FALSE;

    /* Reset the FW execution status. For example, in case FW is     */
    /* running and ERASE command is issued, control stays in BL mode */
    /* after erasing, but the below register is not reset.           */
    /* For clean handling, this register is RESET as soon as         */
    /* bootloader starts execution                                   */
    fwBootInfo = 0UL;
    MCU_DrvSetFWBootInfo(fwBootInfo);

    /**
     * Initialize the MCU
     */
    MCU_Init(&BL_MCU_Config);

#ifdef BL_ENABLE_GPT
    TIM_Init(BL_TIM_CHANID_0, &BL_TimeCfg);
#endif

    t1 = BCM_GetTimeNs();

#ifdef BL_ENABLE_GPIO
    /* initialize Pinmux */
    PINMUX_Init();
#endif

#if defined(BL_ENABLE_UART_PL011)
    BL_UART_Init(BL_LOG_UART_HW_ID, &BL_UART_Config);
#endif
    OTP_Init(BL_OTP_HW_ID_0);

#if defined(BL_ENABLE_TEST)
    BL_TestMain();
#endif

#if defined(BL_ENABLE_DDR)
    MEMC_Init(BL_MEMC_HW_ID_0);
    ret = MEMC_GetState(BL_MEMC_HW_ID_0, &aState);
    if (ret != BCM_ERR_OK) {
        BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }
    if (aState != MEMC_STATE_INIT) {
        BL_CNTRL_MainReportError(BCM_ERR_UNINIT, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }
#endif

#if defined(BL_ENABLE_SLAVE_BOOT)
    /* boot all the slaves */
    if (BL_BCM_ERR_OK == MCU_GetExtendedInfo(&stackInfo)) {
        if (MCU_DEVICE_MASTER == stackInfo.mstSlvMode) {
            BL_CNTRL_BootSlaves();
        } else {
            cntrlMode = defaultDwnldMode;
            noFlash = BL_TRUE;
        }
    }
#endif

    if (BL_FALSE == noFlash) {
#if defined(BL_ENABLE_FLASH)
        for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
            ret = MCU_ClkInit(BL_FLASH_Config[i].clkCfgID);
            if (BL_BCM_ERR_OK != ret) {
                BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
            }
            BL_FLASH_Init(BL_FLASH_Config[i].hwID, &BL_FLASH_Config[i]);
        }
#endif

        ret = MCU_GetBLBootInfo(&blBootInfo);
        if (BL_BCM_ERR_OK == ret) {
            if (blBootInfo == 0UL) {
                cntrlMode = defaultDwnldMode;
                BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
            }
        } else if (BL_BCM_ERR_NOSUPPORT != ret) {
            BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
        }

#if defined(BL_ENABLE_PTM)
        ptmInitRet = BL_PTM_Init(&BL_PTM_Cfg);
        if (BL_BCM_ERR_OK != ptmInitRet) {
            cntrlMode = defaultDwnldMode;
        } else {
            BL_LOG_INFO("PTM_Init successful\n");
        }
#endif


#if defined(BL_ENABLE_SECURE_BOOT)
        if (BL_TRUE == BL_CNTRL_isSecureBoot()) {
            BL_LOG_ERR("Secure boot is enabled\n");
            ret = BL_CNTRL_ValidatePubKey();
            if (BL_BCM_ERR_OK != ret) {
                BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
                cntrlMode = defaultDwnldMode;
            }
        }
#endif

#if defined(BL_ENABLE_PTM)
        if (BL_FALSE == BL_isDownloadMode(cntrlMode)) {
            ret = BL_PTM_GetPTValidFlashID(&flashID);
            if (BL_BCM_ERR_OK == ret) {
                fwBootInfo |= (flashID << MCU_FW_BOOT_INFO_FLASH_ID_SHIFT) & MCU_FW_BOOT_INFO_FLASH_ID_MASK;
                ret = BL_PTM_ValidatePartition(PTBL_ID_SYSCFG, &sysCfgCopyNum);
                if (BL_BCM_ERR_OK == ret) {
                    fwBootInfo |= ((sysCfgCopyNum << MCU_FW_BOOT_INFO_SYSCFG_COPY_ID_SHIFT)
                            & MCU_FW_BOOT_INFO_SYSCFG_COPY_ID_MASK);
                    size = (uint32_t)sizeof(xcvrConfig);
                    BL_BCM_MemSet(&xcvrConfig, 0U, size);
                    ret = BL_PTM_LoadSysCfgImg(0x9000, (BL_PTM_AddrType)&xcvrConfig, &size);
                    if (BL_BCM_ERR_OK != ret) {
                        BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
                    }
                } else {
                    BL_CNTRL_MainReportError(ret, 0UL, 0UL, 0UL, 0UL, __LINE__);
                    cntrlMode = defaultDwnldMode;
                }
            }
        }
#endif
    }

#if defined(BL_ENABLE_SWR_CONFIG)
    BL_CNTRL_SWRConfigure();
#endif
    /**
     * check reset reason of the MCU:
     * 1) if reset its soft reset with download
     * mode bit set, enter into download mode
     * directly
     * 2) its reset is due to watchdog or crash
     * enter into ramdump mode
     */
    resetReason = MCU_GetResetReason();
    resetMode   = MCU_GetResetMode();
    dwnldMode   = MCU_GetDwnldMode();
    switch (resetReason) {
    case MCU_RESETREASON_POWER_ON:
        break;
    case MCU_RESETREASON_SW:
        cntrlMode = BL_MCUResetModeToCNTRLMode(resetMode);
        break;
    case MCU_RESETREASON_WD:
#if defined(ENABLE_RAMDUMP)
        cntrlMode = BL_CNTRL_MODE_ENTER_RAM_DUMP;
#endif
        break;
    default:
        BL_CNTRL_MainReportError(BL_BCM_ERR_UNKNOWN, 0UL, 0UL, 0UL, 0UL, __LINE__);
        break;
    }

    if ((cntrlMode == BL_CNTRL_MODE_ENTER_RAM_DWNLD) ||
            (cntrlMode == BL_CNTRL_MODE_ENTER_IPC_DWNLD)) {
        goto end;
    } else if (cntrlMode == BL_CNTRL_MODE_ENTER_TFTP_DWNLD) {
        BL_CNTRL_XcvrInit(&xcvrConfig);
#ifdef BL_ENABLE_ETH_AMAC
        BL_ETHER_Init(0UL, 0UL);
#endif
        goto end;
    } else {
        BL_CNTRL_XcvrInit(&xcvrConfig);
#ifdef BL_ENABLE_ETH_AMAC
        BL_ETHER_Init(0UL, 0UL);
#endif
    }

#if defined(ENABLE_RAMDUMP)
    if (BL_CNTRL_MODE_ENTER_RAM_DUMP == cntrlMode) {
    }
#endif

    t2 = BCM_GetTimeNs();
    bootTime.xcvrInitTime = BL_NS_TO_MS(t2 - t1) + bootTime.blBootTime;

#if defined(BL_ENABLE_PTM)
    if (BL_FALSE == BL_isDownloadMode(cntrlMode)) {
        ret = BL_PTM_LoadFirmware(&entry, &fwCopyNum);
        if (ret == BL_BCM_ERR_OK) {
            fwBootInfo |= ((fwCopyNum << MCU_FW_BOOT_INFO_FW_COPY_ID_SHIFT) &
                            MCU_FW_BOOT_INFO_FW_COPY_ID_MASK);
            fwBootInfo |= MCU_FW_BOOT_INFO_FW_EXECUTED_MASK;
            MCU_DrvSetFWBootInfo(fwBootInfo);
            BL_LOG_INFO("Found FW[copy:0x%x] image\n", fwBootInfo);
#ifdef BL_CORTEX_M7
            entry |= 0x1UL;
#endif
        } else {
            cntrlMode = defaultDwnldMode;
        }
    }
#endif

end:
    MCU_SetBootTimeInfo(&bootTime);
#ifdef BL_ENABLE_DCACHE
#if defined( __CORTEX_MX__)
    CORTEX_MX_DCacheDisable();
#endif
#endif
#ifdef BL_ENABLE_ICACHE
#if defined( __CORTEX_MX__)
    CORTEX_MX_ICacheDisable();
#endif
#endif

    switch(cntrlMode) {
    case BL_CNTRL_MODE_ENTER_FW:
#if defined(BL_ENABLE_RAM_DWNLD_MODE)
        BL_RAM_DWNLD_ResetDwnldInfo();
#endif
#if defined(BL_ENABLE_IPC_DWNLD_MODE)
        BL_IPC_DWNLD_ResetDwnldInfo();
#endif
        ((BL_PTM_ImgEntryFuncType)(entry))();
        break;
#if defined(BL_ENABLE_RAM_DWNLD_MODE)
    case BL_CNTRL_MODE_ENTER_RAM_DWNLD:
        BL_LOG_ERR("Enter RAM download mode\n");
        BL_RAM_DWNLD_Init(dwnldMode);
        break;
#endif
#if defined(BL_ENABLE_TFTP_DWNLD_MODE)
    case BL_CNTRL_MODE_ENTER_TFTP_DWNLD:
        BL_TFTP_DWNLD_Init(dwnldMode);
        break;
#endif
#if defined(BL_ENABLE_IPC_DWNLD_MODE)
    case BL_CNTRL_MODE_ENTER_IPC_DWNLD:
        BL_LOG_ERR("Enter IPC download mode\n");
        BL_IPC_DWNLD_Init(dwnldMode);
        break;
#endif
    default:
        BL_CNTRL_MainReportError(ret, 0UL, cntrlMode, 0UL, 0UL, __LINE__);
        break;
    }
    return -1;
}
