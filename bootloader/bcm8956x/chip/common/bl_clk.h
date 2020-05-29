/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
#ifndef BL_BCM8953X_CLK_H
#define BL_BCM8953X_CLK_H

/**
 * Clock IDs
 */
#define BL_BCM_MCU_CLK_ID_CPU              (0x1UL) /**< CPU clock ID */
#define BL_BCM_MCU_CLK_ID_UART0            (0x2UL)
#define BL_BCM_MCU_CLK_ID_PWM              (0x3UL)
#define BL_BCM_MCU_CLK_ID_I2C              (0x4UL)
#define BL_BCM_MCU_CLK_ID_SPI0             (0x5UL)
#define BL_BCM_MCU_CLK_ID_SPI1             (0x6UL)
#define BL_BCM_MCU_CLK_ID_QSPI             (0x7UL) /**< QSPI clock ID */
#define BL_BCM_MCU_CLK_ID_MAX              (BL_BCM_MCU_CLK_ID_QSPI)

/**
 * Clock configuration IDs
 */

/**
 * QSPI clock settings IDs
 */
#define BL_MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ           (0x30UL)
#define BL_MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ           (0x31UL)
#define BL_MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ           (0x32UL)
#define BL_MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ           (0x33UL)

#endif /* BL_BCM8953X_CLK_H */
