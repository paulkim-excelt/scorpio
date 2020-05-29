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
#include <bl_chip_config.h>
#include <bl_board.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#ifdef ENABLE_IPC
#include <ipc.h>
#include <ipc_osil.h>
#endif

#define BL_FREQ_MHZ(x)         (1000000UL * (x))

extern uint8_t BL_Scratch_StartAddr[];
extern uint8_t BL_Scratch_EndAddr[];

const uint8_t BL_DefaultMacAddr[6UL] = {0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};

static const MCU_ClkCfgType BL_ClkCfgTbl[] = {
    {
        .cfgID = BL_MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ,   /* QSPI @ 83MHZ */
        .clkRef = {
            .clkID = BL_BCM_MCU_CLK_ID_QSPI,
            .freq = BL_FREQ_MHZ(83),
            .cntrl = {[0] = 0x3, [1] = 0x4},
        },
    },
    {
        .cfgID = BL_MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ,   /* QSPI @ 62MHZ */
        .clkRef = {
            .clkID = BL_BCM_MCU_CLK_ID_QSPI,
            .freq = BL_FREQ_MHZ(62),
            .cntrl = {[0] = 0x3, [1] = 0x5},
        },
    },
    {
        .cfgID = BL_MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ,   /* QSPI @ 50MHZ */
        .clkRef = {
            .clkID = BL_BCM_MCU_CLK_ID_QSPI,
            .freq = BL_FREQ_MHZ(50),
            .cntrl = {[0] = 0x3, [1] = 0x7},
        },
    },
    {
        .cfgID = BL_MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ,   /* QSPI @ 25MHZ */
        .clkRef = {
            .clkID = BL_BCM_MCU_CLK_ID_QSPI,
            .freq = BL_FREQ_MHZ(25),
            .cntrl = {[0] = 0x2, [1] = 0x7},
        },
    },
};

const MCU_ConfigType BL_MCU_Config = {
    .clkSrcFailNotification = MCU_CLK_SRC_NOTIFICATION_DIS,
    .clkCfgTbl = BL_ClkCfgTbl,
    .clkCfgTblSize = (sizeof(BL_ClkCfgTbl) / sizeof(MCU_ClkCfgType)),
    .ramCfgTbl = NULL,
    .ramCfgTblSize = 0UL,
};

const BL_UART_ConfigType BL_UART_Config = {
    .baud = BL_UART_BAUD_115200,
    .stopBits = BL_UART_STOP1,
    .parity = BL_UART_PARITY_NONE,
};

/* Board flash configurations */
#define BL_FLASH_SPEED    (BL_FLASH_SPI_SPEED_50M)

#if (BL_FLASH_SPEED == BL_FLASH_SPI_SPEED_80M)
#define BL_FLASH_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ)
#elif (BL_FLASH_SPEED == BL_FLASH_SPI_SPEED_62M)
#define BL_FLASH_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ)
#elif (BL_FLASH_SPEED == BL_FLASH_SPI_SPEED_50M)
#define BL_FLASH_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ)
#elif (BL_FLASH_SPEED == BL_FLASH_SPI_SPEED_25M)
#define BL_FLASH_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ)
#else
#error "Invalid flash speed"
#endif

const BL_FLASH_CfgType BL_FLASH_Config[] = {
    {
        .hwID = BL_FLASH_HW_ID_0,
        .size = BL_FLASH_SIZE,
        .pageSize = 256UL,
        .sectorSize = BL_FLASH_SECTOR_SIZE,
        .subSectorSupported = BL_IS_SUB_SECTOR_SUPPORT,
        .subSectorSize = BL_FLASH_SUBSECTOR_SIZE,
        .SPIMode = BL_FLASH_SPI_MODE_3,
        .speed = BL_FLASH_SPEED,
        .readLane = BL_FLASH_READ_LANE_SINGLE,
        .clkCfgID = BL_FLASH_CLK_CFG_ID,
    },
};

const uint32_t BL_FLASH_CfgSize = sizeof(BL_FLASH_Config)/ sizeof(BL_FLASH_CfgType);

#ifdef BL_ENABLE_PTM
static const BL_PTM_LookupTblEntType BL_LookupTbl[] = {
    {
        .flashID = BL_FLASH_HW_ID_0,
        .flashAddr = BL_PT_LOOKUP_FLASH_ADDR,
    },
};

const BL_PTM_CfgType BL_PTM_Cfg = {
    .lookupTbl = BL_LookupTbl,
    .lookupTblSize = (sizeof(BL_LookupTbl)/sizeof(BL_PTM_LookupTblEntType)),
    .scratchMemStartAddr = BL_Scratch_StartAddr,
    .scratchMemEndAddr = BL_Scratch_EndAddr,
};

#endif /* BL_ENABLE_PTM */

#ifdef BL_ENABLE_GPT
const TIM_ConfigType BL_TimeCfg = {
    .prescale = TIM_PRESCALE_DIV_1,
    .width = TIM_WIDTH_32_BIT,
    .sysTimeEn = BL_TRUE,
    .chanID = BL_TIM_CHANID_0,
    .chanMode = TIM_CHAN_MODE_FREE_RUNNING,
    .cb = NULL,
};
#endif

#ifdef ENABLE_IPC
#ifdef __BCM89564G__
extern const IPC_BusFnTblType IPC_SpiFnTbl;
#endif

const IPC_ChannCfgType IPC_ChannCfg[IPC_MAX_CHANNELS] = {
    {
        .ID = 0UL,
        .mode = IPC_CHANN_MODE_SLAVE,
        .sizeLog2 = 9U,
        .cntLog2 = 3U,
        .busInfo = {
            .hwID = 0UL,
            .busType = IPC_BUS_MEMMAP,
            .slaveID = 0UL,
            .fnTbl = NULL,
        },
    },
#ifdef __BCM89564G__
    {
        .ID = 1UL,
        .mode = IPC_CHANN_MODE_MASTER,
        .sizeLog2 = 9U,
        .cntLog2 = 3U,
        .busInfo = {
            .hwID = 1UL,
            .busType = IPC_BUS_SPI,
            .slaveID = 0UL,
            .fnTbl = &IPC_SpiFnTbl,
        },
    },
#endif
};
#endif
