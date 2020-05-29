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

#ifndef BL_BCM8956X_CONFIG_H
#define BL_BCM8956X_CONFIG_H

#include <xgxsblk0_rdb.h>
#include <chipmisc_rdb.h>
#include <qspi_rdb.h>
#include <uart_rdb.h>

static CHIPMISC_RDBType *const BL_CHIPMISC_REGS = (CHIPMISC_RDBType *)CHIPMISC_BASE;

#define BL_CPU_CLOCK           (400000000UL)   /* 400 MHz */
#define BL_NS_PER_CPU_CYCLE    (1000000000UL / BL_CPU_CLOCK)

#define BL_CNTRL_MAX_IMG_COUNT (32UL)

/* TCM */
#define BL_TCM_BASE            (0x00000000UL)
/* DMU */
#define BL_DMU_BASE            (DMU_DMU_STATUS)

#define BL_SRAM_REGION_START   (0x20000000)
/* UART */
#define BL_UART_MAX_HW_ID      (UART_MAX_HW_ID)
#define BL_URT0_BASE           (UART_BASE)
#define BL_LOG_UART_HW_ID      (0UL)

/* Timer */
#define BL_TIM_MAX_CHAN_ID     (2UL)
#define BL_SP804_MAX_CHAN_ID   (2UL)
#define BL_SP804_0_BASE        (TIM0_TIMER1LOAD)
#define BL_SP804_1_BASE        (TIM0_TIMER2LOAD)
#define BL_TIMER_CLOCK         (6250000UL)
#define BL_TIM_CHANID_0        (0UL)
#define BL_TIM_LOAD_REG_OFFSET (0UL)
#define BL_TIM_CURR_REG_OFFSET (4UL)
#define BL_TIM_CTRL_REG_OFFSET (8UL)
#define BL_SP804_LOAD_TIME_REG (BL_SP804_0_BASE + BL_TIM_LOAD_REG_OFFSET)
#define BL_SP804_CURR_TIME_REG (BL_SP804_0_BASE + BL_TIM_CURR_REG_OFFSET)
#define BL_SP804_CTRL_REG      (BL_SP804_0_BASE + BL_TIM_CTRL_REG_OFFSET)

/* CFG */
/* TODO: fix CFG block base address */
#define BL_CFG_BASE            (CFG_SR)

/* BRPHY */
/* TODO: Use RDB */
#define BRPHY4_BR_CTRL_EXP90      0x49cf2600 /* BroadReach LRE Misc Control */
#define BRPHY4_CL45DEV7_AN_AD     0x49ce0020 /* Auto Neg AD(0x0010) (REG 7.16) */
#define BRPHY4_CL45DEV7_AN_CTRL   0x49ce0000 /* Auto Neg Extended Next Page Control     (0x0000) (REG 7.0) */

#define BL_BRPHY1_CL22_BASE    (BRPHY1_BR_CL22_IEEE_MII_CTRL)

/* CRG */
#define BL_CRG_BASE            (CRG_XTAL_CONFIG)

/* QSPI */
#define BL_QSPI_MAX_HW_ID      (QSPI_MAX_HW_ID)
#define BL_QSPI0_BASE          (QSPI0_BASE)

/* FLASH */
#define BL_FLASH_MAX_HW_ID     (1UL)

/* MISC */
#define BL_CHIP_MISC_BASE            (&(BL_CHIPMISC_REGS->model_rev_num))

/* DOWNLOADER */
#define BL_DWNLD_TARGET_SPARE_REG    (&(BL_CHIPMISC_REGS->spare_sw_reg1))
#define BL_BOOT_MODE_SPARE_REG       (&(BL_CHIPMISC_REGS->spare_sw_reg0))
#define BL_DWNLD_HOST_SPARE_REG      (&(BL_CHIPMISC_REGS->spare_sw_reg2))

/* PLL2 */
/* TODO: Use RDB */
#define PLL2_STAT0                     0x4a410300 /* Analog PLL Status0 Register */
#define PLL2_CTRL1                     0x4a410302 /* Analog PLL Control1 Register */
#define PLL2_CTRL2                     0x4a410304 /* Analog PLL Control2 Register */
#define PLL2_CTRL3                     0x4a410306 /* Analog PLL Control3 Register */
#define PLL2_CTRL4                     0x4a410308 /* Analog PLL Control4 Register */
#define PLL2_CTRL5                     0x4a41030a /* Analog PLL Control5 Register */
#define PLL2_CTRL6                     0x4a41030c /* Analog PLL Control6 Register */
#define PLL2_BLOCKADDRESS              0x4a41031e /* Block Address register */


/* OTP */
#define BL_OTP_HW_ID_MAX                   (1UL)
#define BL_OTP_BASE                        (OTP_CPU_COMMAND)
#define BL_OTP_ADDR_START                  (0UL)
#define BL_OTP_ADDR_END                    (255UL)
#define BL_OTP_DATA_MASK_WITH_ECC          (0xFFFFFFFUL)
#define BL_OTP_ADDR_BRCM_REG_START         (12UL)
#define BL_OTP_ADDR_BRCM_REG_END           (63UL)
#define BL_OTP_ADDR_CUST_REG_START         (64UL)
#define BL_OTP_ADDR_CUST_REG_END           (BL_OTP_ADDR_END)
#define BL_OTP_SEC_BOOT_EN_ROW             (BL_OTP_ADDR_CUST_REG_START)
#define BL_OTP_SEC_BOOT_EN_MASK            (0x1UL)
#define BL_OTP_ADDR_RSA_PUB_START          (BL_OTP_ADDR_CUST_REG_START + 1UL)
#define BL_OTP_ECC_BITS                    (6UL)

/* Security */
#define BL_OTP_SEC_HW_ID                   (0UL)
#define BL_OTP_SEC_BOOT_EN_ROW             (BL_OTP_ADDR_CUST_REG_START)
#define BL_OTP_SEC_BOOT_EN_MASK            (0x1UL)
#define BL_OTP_ADDR_RSA_PUB_START          (BL_OTP_ADDR_CUST_REG_START + 1UL)
/** RSA KEY BITS:
 * Each row has 32 OTP bits out of which 6 bits are reserved
 * for ECC (only rows starting after BRCM region)
 * Another bits are reserved to make it aligned at
 * byte boundary
 */
#define BL_RSA_KEY_BYTES_PER_ROW           (3UL)

/**
 * 2048 bits secure RSA key
 */
#define BL_OTP_SECURE_KEY_SIZE             (256UL)

#define BL_OTP_HW_ID_0                     (0UL)
#define BL_OTP_MAC_ADDR_0_OCTET123_ADDR    (151UL)
#define BL_OTP_MAC_ADDR_0_OCTET456_ADDR    (152UL)
#define BL_OTP_MAC_ADDR_1_OCTET123_ADDR    (153UL)
#define BL_OTP_MAC_ADDR_1_OCTET456_ADDR    (154UL)

#endif /* BL_BCM8956X_CONFIG_H */
