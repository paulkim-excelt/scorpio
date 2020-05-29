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
    @addtogroup grp_mcudrv_ifc
    @{

    @file clk.h
    @brief Clock interface
    This header file contains the interface for Clock

    @version 0.86 Imported from docx
*/

#ifndef BCM8956X_CLK_H
#define BCM8956X_CLK_H

/**
    @name Clock API IDs
    @{
    @brief API IDs for Clock
*/
#define BRCM_SWARCH_MCU_CLK_ID_MACRO                    (0x21U) /**< @brief #MCU_CLK_ID_CPU */
#define BRCM_SWARCH_MCU_CLK_CFG_ID_MACRO                (0x22U) /**< @brief #MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ */
/** @} */

/**
    @name Clock IDs
    @{
    @brief Clock IDs for BCM8956X

    @trace #BRCM_SWREQ_MCU_INIT
 */
#define MCU_CLK_ID_CPU              (0x1UL) /**< @brief CPU clock ID */
#define MCU_CLK_ID_UART0            (0x2UL) /**< @brief UART clock ID */
#define MCU_CLK_ID_PWM              (0x3UL) /**< @brief PWM clock ID */
#define MCU_CLK_ID_IIC              (0x4UL) /**< @brief IIC clock ID */
#define MCU_CLK_ID_SPI0             (0x5UL) /**< @brief SPI0 clock ID */
#define MCU_CLK_ID_SPI1             (0x6UL) /**< @brief SPI1 clock ID */
#define MCU_CLK_ID_QSPI             (0x7UL) /**< @brief QSPI clock ID */
#define MCU_CLK_ID_MDIO             (0x8UL) /**< @brief MDIO clock ID */
/** @} */

/**
    @name Clock Config IDs
    @{
    @brief Clock config IDs for BCM8956X

    @trace #BRCM_SWREQ_MCU_INIT
 */
#define MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ           (0x30UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ           (0x31UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ           (0x32UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ           (0x33UL)
#define MCU_CLK_CFG_ID_MDIO                         (0x40UL)
/** @} */

#endif /* BCM8956X_CLK_H */

/** @} */
