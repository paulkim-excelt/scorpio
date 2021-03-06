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

#ifndef BCM8956X_IRQS_H
#define BCM8956X_IRQS_H

#if 1

#ifdef  EE_CORTEX_MX_QSPI0_ISR
#define EE_CORTEX_MX_INT_09_ISR     EE_CORTEX_MX_QSPI0_ISR
#ifdef  EE_CORTEX_MX_QSPI0_ISR_PRI
#define EE_CORTEX_MX_INT_09_ISR_PRI EE_CORTEX_MX_QSPI0_ISR_PRI
#endif  /* EE_CORTEX_MX_QSPI0_ISR_PRI */
#endif  /* EE_CORTEX_MX_QSPI0_ISR */

#ifdef  EE_CORTEX_MX_UART0_ISR
#define EE_CORTEX_MX_INT_0A_ISR     EE_CORTEX_MX_UART0_ISR
#ifdef  EE_CORTEX_MX_UART0_ISR_PRI
#define EE_CORTEX_MX_INT_0A_ISR_PRI EE_CORTEX_MX_UART0_ISR_PRI
#endif  /* EE_CORTEX_MX_UART0_ISR_PRI */
#endif  /* EE_CORTEX_MX_UART0_ISR */

#ifdef  EE_CORTEX_MX_SPI1_ISR
#define EE_CORTEX_MX_INT_11_ISR     EE_CORTEX_MX_SPI1_ISR
#ifdef  EE_CORTEX_MX_SPI1_ISR_PRI
#define EE_CORTEX_MX_INT_11_ISR_PRI EE_CORTEX_MX_SPI1_ISR_PRI
#endif  /* EE_CORTEX_MX_SPI1_ISR_PRI */
#endif  /* EE_CORTEX_MX_SPI1_ISR */

#ifdef  EE_CORTEX_MX_TIMER0_ISR
#define EE_CORTEX_MX_INT_0C_ISR     EE_CORTEX_MX_TIMER0_ISR
#ifdef  EE_CORTEX_MX_TIMER0_ISR_PRI
#define EE_CORTEX_MX_INT_0C_ISR_PRI EE_CORTEX_MX_TIMER0_ISR_PRI
#endif  /* EE_CORTEX_MX_TIMER0_ISR_PRI */
#endif  /* EE_CORTEX_MX_TIMER0_ISR */

#ifdef  EE_CORTEX_MX_TIMER1_ISR
#define EE_CORTEX_MX_INT_0D_ISR     EE_CORTEX_MX_TIMER1_ISR
#ifdef  EE_CORTEX_MX_TIMER1_ISR_PRI
#define EE_CORTEX_MX_INT_0D_ISR_PRI EE_CORTEX_MX_TIMER1_ISR_PRI
#endif  /* EE_CORTEX_MX_TIMER1_ISR_PRI */
#endif  /* EE_CORTEX_MX_TIMER1_ISR */

#ifdef  EE_CORTEX_MX_ETH_ISR
#define EE_CORTEX_MX_INT_17_ISR     EE_CORTEX_MX_ETH_ISR
#ifdef  EE_CORTEX_MX_ETH_ISR_PRI
#define EE_CORTEX_MX_INT_17_ISR_PRI EE_CORTEX_MX_ETH_ISR_PRI
#endif  /* EE_CORTEX_MX_AMAC_ISR_PRI */
#endif  /* EE_CORTEX_MX_AMAC_ISR */

#ifdef  EE_CORTEX_MX_IPC_ISR
#define EE_CORTEX_MX_INT_01_ISR     EE_CORTEX_MX_IPC_ISR
#ifdef  EE_CORTEX_MX_IPC_ISR_PRI
#define EE_CORTEX_MX_INT_01_ISR_PRI EE_CORTEX_MX_IPC_ISR_PRI
#endif  /* EE_CORTEX_MX_IPC_ISR_PRI */
#endif  /* EE_CORTEX_MX_IPC_ISR */

#ifdef  EE_CORTEX_MX_ETHTIME_ISR
#define EE_CORTEX_MX_INT_07_ISR     EE_CORTEX_MX_ETHTIME_ISR
#ifdef  EE_CORTEX_MX_ETHTIME_ISR_PRI
#define EE_CORTEX_MX_INT_07_ISR_PRI EE_CORTEX_MX_ETHTIME_ISR_PRI
#endif  /* EE_CORTEX_MX_ETHTIME_ISR_PRI */
#endif  /* EE_CORTEX_MX_ETHTIME_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK0_ISR
#define EE_CORTEX_MX_INT_1B_ISR     EE_CORTEX_MX_SWITCH_LINK0_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK0_ISR_PRI
#define EE_CORTEX_MX_INT_1B_ISR_PRI EE_CORTEX_MX_SWITCH_LINK0_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK0_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK0_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK1_ISR
#define EE_CORTEX_MX_INT_1C_ISR     EE_CORTEX_MX_SWITCH_LINK1_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK1_ISR_PRI
#define EE_CORTEX_MX_INT_1C_ISR_PRI EE_CORTEX_MX_SWITCH_LINK1_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK1_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK1_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK2_ISR
#define EE_CORTEX_MX_INT_1D_ISR     EE_CORTEX_MX_SWITCH_LINK2_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK2_ISR_PRI
#define EE_CORTEX_MX_INT_1D_ISR_PRI EE_CORTEX_MX_SWITCH_LINK2_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK2_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK2_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK3_ISR
#define EE_CORTEX_MX_INT_1E_ISR     EE_CORTEX_MX_SWITCH_LINK3_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK3_ISR_PRI
#define EE_CORTEX_MX_INT_1E_ISR_PRI EE_CORTEX_MX_SWITCH_LINK3_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK3_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK3_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK4_ISR
#define EE_CORTEX_MX_INT_1F_ISR     EE_CORTEX_MX_SWITCH_LINK4_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK4_ISR_PRI
#define EE_CORTEX_MX_INT_1F_ISR_PRI EE_CORTEX_MX_SWITCH_LINK4_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK4_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK4_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK5_ISR
#define EE_CORTEX_MX_INT_20_ISR     EE_CORTEX_MX_SWITCH_LINK5_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK5_ISR_PRI
#define EE_CORTEX_MX_INT_20_ISR_PRI EE_CORTEX_MX_SWITCH_LINK5_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK5_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK5_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK6_ISR
#define EE_CORTEX_MX_INT_21_ISR     EE_CORTEX_MX_SWITCH_LINK6_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK6_ISR_PRI
#define EE_CORTEX_MX_INT_21_ISR_PRI EE_CORTEX_MX_SWITCH_LINK6_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK6_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK6_ISR */

#ifdef  EE_CORTEX_MX_SWITCH_LINK8_ISR
#define EE_CORTEX_MX_INT_22_ISR     EE_CORTEX_MX_SWITCH_LINK8_ISR
#ifdef  EE_CORTEX_MX_SWITCH_LINK8_ISR_PRI
#define EE_CORTEX_MX_INT_22_ISR_PRI EE_CORTEX_MX_SWITCH_LINK8_ISR_PRI
#endif /* EE_CORTEX_MX_SWITCH_LINK8_ISR_PRI */
#endif /* EE_CORTEX_MX_SWITCH_LINK8_ISR */

#ifdef  EE_CORTEX_MX_GPIO1_ISR
#define EE_CORTEX_MX_INT_14_ISR     EE_CORTEX_MX_GPIO1_ISR
#ifdef  EE_CORTEX_MX_GPIO1_ISR_PRI
#define EE_CORTEX_MX_INT_14_ISR_PRI EE_CORTEX_MX_GPIO1_ISR_PRI
#endif  /* EE_CORTEX_MX_GPIO1_ISR_PRI */
#endif  /* EE_CORTEX_MX_GPIO1_ISR */

#else
#ifdef  EE_CORTEX_MX_VMP_GP_ISR
#define EE_CORTEX_MX_INT_00_ISR     EE_CORTEX_MX_VMP_GP_ISR
#ifdef  EE_CORTEX_MX_VMP_GP_ISR_PRI
#define EE_CORTEX_MX_INT_00_ISR_PRI EE_CORTEX_MX_VMP_GP_ISR_PRI
#endif  /* EE_CORTEX_MX_VMP_GP_ISR_PRI */
#endif  /* EE_CORTEX_MX_VMP_GP_ISR */

#ifdef  EE_CORTEX_MX_VMP_ECC_ISR
#define EE_CORTEX_MX_INT_01_ISR     EE_CORTEX_MX_VMP_ECC_ISR
#ifdef  EE_CORTEX_MX_VMP_ECC_ISR_PRI
#define EE_CORTEX_MX_INT_01_ISR_PRI EE_CORTEX_MX_VMP_ECC_ISR_PRI
#endif  /* EE_CORTEX_MX_VMP_ECC_ISR_PRI */
#endif  /* EE_CORTEX_MX_VMP_ECC_ISR */

#ifdef  EE_CORTEX_MX_VMP_WD_ISR
#define EE_CORTEX_MX_INT_02_ISR     EE_CORTEX_MX_VMP_WD_ISR
#ifdef  EE_CORTEX_MX_VMP_WD_ISR_PRI
#define EE_CORTEX_MX_INT_02_ISR_PRI EE_CORTEX_MX_VMP_WD_ISR_PRI
#endif  /* EE_CORTEX_MX_VMP_WD_ISR_PRI */
#endif  /* EE_CORTEX_MX_VMP_WD_ISR */

#ifdef  EE_CORTEX_MX_TIMER0_ISR
#define EE_CORTEX_MX_INT_34_ISR     EE_CORTEX_MX_TIMER0_ISR
#ifdef  EE_CORTEX_MX_TIMER0_ISR_PRI
#define EE_CORTEX_MX_INT_34_ISR_PRI EE_CORTEX_MX_TIMER0_ISR_PRI
#endif  /* EE_CORTEX_MX_TIMER0_ISR_PRI */
#endif  /* EE_CORTEX_MX_TIMER0_ISR */


#ifdef  EE_CORTEX_MX_TIMER1_ISR
#define EE_CORTEX_MX_INT_35_ISR     EE_CORTEX_MX_TIMER1_ISR
#ifdef  EE_CORTEX_MX_TIMER1_ISR_PRI
#define EE_CORTEX_MX_INT_35_ISR_PRI EE_CORTEX_MX_TIMER1_ISR_PRI
#endif  /* EE_CORTEX_MX_TIMER1_ISR_PRI */
#endif  /* EE_CORTEX_MX_TIMER1_ISR */


#ifdef  EE_CORTEX_MX_ISP_ISR
#define EE_CORTEX_MX_INT_03_ISR     EE_CORTEX_MX_ISP_ISR
#ifdef  EE_CORTEX_MX_ISP_ISR_PRI
#define EE_CORTEX_MX_INT_03_ISR_PRI EE_CORTEX_MX_ISP_ISR_PRI
#endif  /* EE_CORTEX_MX_ISP_ISR_PRI */
#endif  /* EE_CORTEX_MX_ISP_ISR */

#ifdef  EE_CORTEX_MX_IMG_CAM_ISR
#define EE_CORTEX_MX_INT_05_ISR        EE_CORTEX_MX_IMG_CAM_ISR
#ifdef  EE_CORTEX_MX_IMG_CAM_ISR_PRI
#define EE_CORTEX_MX_INT_05_ISR_PRI    EE_CORTEX_MX_IMG_CAM_ISR_PRI
#endif  /*EE_CORTEX_MX_IMG_CAM_ISR_PRI */
#endif  /*EE_CORTEX_MX_IMG_CAM_ISR */

#ifdef  EE_CORTEX_MX_IMG_CAM_ERR_ISR
#define EE_CORTEX_MX_INT_06_ISR        EE_CORTEX_MX_IMG_CAM_ERR_ISR
#ifdef  EE_CORTEX_MX_IMG_CAM_ERR_ISR_PRI
#define EE_CORTEX_MX_INT_06_ISR_PRI    EE_CORTEX_MX_IMG_CAM_ERR_ISR_PRI
#endif  /*EE_CORTEX_MX_IMG_CAM_ISR_PRI */
#endif  /*EE_CORTEX_MX_IMG_CAM_ISR */


#ifdef  EE_CORTEX_MX_UART0_ISR
#define EE_CORTEX_MX_INT_41_ISR     EE_CORTEX_MX_UART0_ISR
#ifdef  EE_CORTEX_MX_UART0_ISR_PRI
#define EE_CORTEX_MX_INT_41_ISR_PRI EE_CORTEX_MX_UART0_ISR_PRI
#endif  /* EE_CORTEX_MX_UART0_ISR_PRI */
#endif  /* EE_CORTEX_MX_UART0_ISR */

#ifdef  EE_CORTEX_MX_UART1_ISR
#define EE_CORTEX_MX_INT_42_ISR     EE_CORTEX_MX_UART1_ISR
#ifdef  EE_CORTEX_MX_UART1_ISR_PRI
#define EE_CORTEX_MX_INT_42_ISR_PRI EE_CORTEX_MX_UART1_ISR_PRI
#endif  /* EE_CORTEX_MX_UART1_ISR_PRI */
#endif  /* EE_CORTEX_MX_UART1_ISR */

#ifdef  EE_CORTEX_MX_UART2_ISR
#define EE_CORTEX_MX_INT_43_ISR     EE_CORTEX_MX_UART2_ISR
#ifdef  EE_CORTEX_MX_UART2_ISR_PRI
#define EE_CORTEX_MX_INT_43_ISR_PRI EE_CORTEX_MX_UART2_ISR_PRI
#endif  /* EE_CORTEX_MX_UART2_ISR_PRI */
#endif  /* EE_CORTEX_MX_UART2_ISR */

#ifdef  EE_CORTEX_MX_I2C02VIC_ISR
#define EE_CORTEX_MX_INT_3A_ISR     EE_CORTEX_MX_I2C02VIC_ISR
#ifdef  EE_CORTEX_MX_I2C02VIC_ISR_PRI
#define EE_CORTEX_MX_INT_3A_ISR_PRI EE_CORTEX_MX_I2C02VIC_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3A_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2C02VIC_ISR */

#ifdef  EE_CORTEX_MX_I2C12VIC_ISR
#define EE_CORTEX_MX_INT_3B_ISR     EE_CORTEX_MX_I2C12VIC_ISR
#ifdef  EE_CORTEX_MX_I2C12VIC_ISR_PRI
#define EE_CORTEX_MX_INT_3B_ISR_PRI EE_CORTEX_MX_I2C12VIC_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3B_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2C12VIC_ISR */

#ifdef  EE_CORTEX_MX_I2C22VIC_ISR
#define EE_CORTEX_MX_INT_3C_ISR     EE_CORTEX_MX_I2C22VIC_ISR
#ifdef  EE_CORTEX_MX_I2C22VIC_ISR_PRI
#define EE_CORTEX_MX_INT_3C_ISR_PRI EE_CORTEX_MX_I2C22VIC_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3C_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2C22VIC_ISR */

#ifdef  EE_CORTEX_MX_I2C32VIC_ISR
#define EE_CORTEX_MX_INT_3D_ISR     EE_CORTEX_MX_I2C32VIC_ISR
#ifdef  EE_CORTEX_MX_I2C32VIC_ISR_PRI
#define EE_CORTEX_MX_INT_3D_ISR_PRI EE_CORTEX_MX_I2C32VIC_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3D_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2C32VIC_ISR */

#ifdef  EE_CORTEX_MX_SPI0_ISR
#define EE_CORTEX_MX_INT_3E_ISR     EE_CORTEX_MX_SPI0_ISR
#ifdef  EE_CORTEX_MX_SPI0_ISR_PRI
#define EE_CORTEX_MX_INT_3E_ISR_PRI EE_CORTEX_MX_SPI0_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3E_ISR_PRI */
#endif  /* EE_CORTEX_MX_SPI0_ISR */

#ifdef  EE_CORTEX_MX_SPI1_ISR
#define EE_CORTEX_MX_INT_3F_ISR     EE_CORTEX_MX_SPI1_ISR
#ifdef  EE_CORTEX_MX_SPI1_ISR_PRI
#define EE_CORTEX_MX_INT_3F_ISR_PRI EE_CORTEX_MX_SPI1_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_3F_ISR_PRI */
#endif  /* EE_CORTEX_MX_SPI1_ISR */

#ifdef  EE_CORTEX_MX_SPI2_ISR
#define EE_CORTEX_MX_INT_40_ISR     EE_CORTEX_MX_SPI2_ISR
#ifdef  EE_CORTEX_MX_SPI2_ISR_PRI
#define EE_CORTEX_MX_INT_40_ISR_PRI EE_CORTEX_MX_SPI2_ISR_PRI
#endif  /* EE_CORTEX_MX_INT_40_ISR_PRI */
#endif  /* EE_CORTEX_MX_SPI2_ISR */

#ifdef EE_CORTEX_MX_AVT_SYS_TS_ISR
#define EE_CORTEX_MX_INT_2E_ISR     EE_CORTEX_MX_AVT_SYS_TS_ISR
#ifdef EE_CORTEX_MX_AVT_SYS_TS_ISR_PRI
#define EE_CORTEX_MX_INT_2E_ISR_PRI EE_CORTEX_MX_AVT_SYS_TS_ISR_PRI
#endif /* EE_CORTEX_MX_AVT_SYS_SP_ISR_PRI */
#endif /* EE_CORTEX_MX_AVT_SYS_TS_ISR_PRI */

#ifdef  EE_CORTEX_MX_AVT_SYS_SP_ISR
#define EE_CORTEX_MX_INT_2F_ISR     EE_CORTEX_MX_AVT_SYS_SP_ISR
#ifdef EE_CORTEX_MX_AVT_SYS_SP_ISR_PRI
#define EE_CORTEX_MX_INT_2F_ISR_PRI EE_CORTEX_MX_AVT_SYS_SP_ISR_PRI
#endif /* EE_CORTEX_MX_AVT_SYS_SP_ISR_PRI */
#endif /* EE_CORTEX_MX_AVT_SYS_SP_ISR */

#ifdef  EE_CORTEX_MX_GMAC_ISR
#define EE_CORTEX_MX_INT_20_ISR     EE_CORTEX_MX_GMAC_ISR
#ifdef  EE_CORTEX_MX_GMAC_ISR_PRI
#define EE_CORTEX_MX_INT_20_ISR_PRI EE_CORTEX_MX_GMAC_ISR_PRI
#endif  /* EE_CORTEX_MX_GMAC_ISR_PRI */
#endif  /* EE_CORTEX_MX_GMAC_ISR */

#ifdef  EE_CORTEX_MX_QSPI0_ISR
#define EE_CORTEX_MX_INT_2A_ISR     EE_CORTEX_MX_QSPI0_ISR
#ifdef  EE_CORTEX_MX_QSPI0_ISR_PRI
#define EE_CORTEX_MX_INT_2A_ISR_PRI EE_CORTEX_MX_QSPI0_ISR_PRI
#endif  /* EE_CORTEX_MX_QSPI0_ISR_PRI */
#endif  /* EE_CORTEX_MX_QSPI0_ISR */

#ifdef  EE_CORTEX_MX_QSPI1_ISR
#define EE_CORTEX_MX_INT_2B_ISR     EE_CORTEX_MX_QSPI1_ISR
#ifdef  EE_CORTEX_MX_QSPI1_ISR_PRI
#define EE_CORTEX_MX_INT_2B_ISR_PRI EE_CORTEX_MX_QSPI1_ISR_PRI
#endif  /* EE_CORTEX_MX_QSPI1_ISR_PRI */
#endif  /* EE_CORTEX_MX_QSPI1_ISR */

#ifdef  EE_CORTEX_MX_SD_HOST_ISR
#define EE_CORTEX_MX_INT_2C_ISR         EE_CORTEX_MX_SD_HOST_ISR
#ifdef  EE_CORTEX_MX_SD_HOST_ISR_PRI
#define EE_CORTEX_MX_INT_2C_ISR_PRI     EE_CORTEX_MX_SD_HOST_ISR_PRI
#endif /* EE_CORTEX_MX_SD_HOST_ISR_PRI */
#endif /* EE_CORTEX_MX_SD_HOST_ISR */

#ifdef  EE_CORTEX_MX_SD_HOST_WKUP_ISR
#define EE_CORTEX_MX_INT_2D_ISR         EE_CORTEX_MX_SD_HOST_WKUP_ISR
#ifdef  EE_CORTEX_MX_SD_HOST_WKUP_ISR_PRI
#define EE_CORTEX_MX_INT_2D_ISR_PRI     EE_CORTEX_MX_SD_HOST_WKUP_ISR_PRI
#endif /* EE_CORTEX_MX_SD_HOST_WKUP_ISR_PRI */
#endif /* EE_CORTEX_MX_SD_HOST_WKUP_ISR */

#ifdef  EE_CORTEX_MX_CAN0_ISR
#define EE_CORTEX_MX_INT_37_ISR     EE_CORTEX_MX_CAN0_ISR
#ifdef  EE_CORTEX_MX_CAN0_ISR_PRI
#define EE_CORTEX_MX_INT_37_ISR_PRI EE_CORTEX_MX_CAN0_ISR_PRI
#endif  /* EE_CORTEX_MX_CAN0_ISR_PRI */
#endif  /* EE_CORTEX_MX_CAN0_ISR */

#ifdef  EE_CORTEX_MX_CAN1_ISR
#define EE_CORTEX_MX_INT_38_ISR     EE_CORTEX_MX_CAN1_ISR
#ifdef  EE_CORTEX_MX_CAN1_ISR_PRI
#define EE_CORTEX_MX_INT_38_ISR_PRI EE_CORTEX_MX_CAN1_ISR_PRI
#endif  /* EE_CORTEX_MX_CAN1_ISR_PRI */
#endif  /* EE_CORTEX_MX_CAN1_ISR */

#ifdef  EE_CORTEX_MX_BRPHY0_ISR
#define EE_CORTEX_MX_INT_11_ISR        EE_CORTEX_MX_BRPHY0_ISR
#ifdef  EE_CORTEX_MX_BRPHY0_ISR_PRI
#define EE_CORTEX_MX_INT_11_ISR_PRI    EE_CORTEX_MX_BRPHY0_ISR_PRI
#endif  /*EE_CORTEX_MX_BRPHY0_ISR_PRI */
#endif  /*EE_CORTEX_MX_BRPHY0_ISR */

#ifdef  EE_CORTEX_MX_BRPHY1_ISR
#define EE_CORTEX_MX_INT_12_ISR        EE_CORTEX_MX_BRPHY1_ISR
#ifdef  EE_CORTEX_MX_BRPHY1_ISR_PRI
#define EE_CORTEX_MX_INT_12_ISR_PRI    EE_CORTEX_MX_BRPHY1_ISR_PRI
#endif  /*EE_CORTEX_MX_BRPHY1_ISR_PRI */
#endif  /*EE_CORTEX_MX_BRPHY1_ISR */

#ifdef  EE_CORTEX_MX_DISP_PWR_DOWN_ISR
#define EE_CORTEX_MX_INT_07_ISR        EE_CORTEX_MX_DISP_PWR_DOWN_ISR
#ifdef  EE_CORTEX_MX_DISP_PWR_DOWN_ISR_PRI
#define EE_CORTEX_MX_INT_07_ISR_PRI    EE_CORTEX_MX_DISP_PWR_DOWN_ISR_PRI
#endif  /* EE_CORTEX_MX_DISP_PWR_DOWN_ISR_PRI */
#endif  /* EE_CORTEX_MX_DISP_PWR_DOWN_ISR */

#ifdef  EE_CORTEX_MX_DISP_BVB_ISR
#define EE_CORTEX_MX_INT_08_ISR        EE_CORTEX_MX_DISP_BVB_ISR
#ifdef  EE_CORTEX_MX_DISP_BVB_ISR_PRI
#define EE_CORTEX_MX_INT_08_ISR_PRI    EE_CORTEX_MX_DISP_BVB_ISR_PRI
#endif  /* EE_CORTEX_MX_DISP_BVB_ISR_PRI */
#endif  /* EE_CORTEX_MX_DISP_BVB_ISR */

#ifdef  EE_CORTEX_MX_DISP_DSI_ISR
#define EE_CORTEX_MX_INT_09_ISR        EE_CORTEX_MX_DISP_DSI_ISR
#ifdef  EE_CORTEX_MX_DISP_DSI_ISR_PRI
#define EE_CORTEX_MX_INT_09_ISR_PRI    EE_CORTEX_MX_DISP_DSI_ISR_PRI
#endif  /* EE_CORTEX_MX_DISP_DSI_ISR_PRI */
#endif  /* EE_CORTEX_MX_DISP_DSI_ISR */

#ifdef  EE_CORTEX_MX_DISP_PV_ISR
#define EE_CORTEX_MX_INT_0A_ISR        EE_CORTEX_MX_DISP_PV_ISR
#ifdef  EE_CORTEX_MX_DISP_PV_ISR_PRI
#define EE_CORTEX_MX_INT_0A_ISR_PRI    EE_CORTEX_MX_DISP_PV_ISR_PRI
#endif  /* EE_CORTEX_MX_DISP_PV_ISR_PRI */
#endif  /* EE_CORTEX_MX_DISP_PV_ISR */

#ifdef  EE_CORTEX_MX_DISP_EOT_ISR
#define EE_CORTEX_MX_INT_0B_ISR        EE_CORTEX_MX_DISP_EOT_ISR
#ifdef  EE_CORTEX_MX_DISP_EOT_ISR_PRI
#define EE_CORTEX_MX_INT_0B_ISR_PRI    EE_CORTEX_MX_DISP_EOT_ISR_PRI
#endif  /* EE_CORTEX_MX_DISP_EOT_ISR_PRI */
#endif  /* EE_CORTEX_MX_DISP_EOT_ISR */

#ifdef  EE_CORTEX_MX_VTMON_ISR
#define EE_CORTEX_MX_INT_48_ISR        EE_CORTEX_MX_VTMON_ISR
#ifdef  EE_CORTEX_MX_VTMON_ISR_PRI
#define EE_CORTEX_MX_INT_48_ISR_PRI    EE_CORTEX_MX_VTMON_ISR_PRI
#endif  /* EE_CORTEX_MX_VTMON_ISR_PRI */
#endif  /* EE_CORTEX_MX_VTMON_ISR */

#ifdef  EE_CORTEX_MX_TMR0_CH0_ISR
#define EE_CORTEX_MX_INT_24_ISR        EE_CORTEX_MX_TMR0_CH0_ISR
#ifdef  EE_CORTEX_MX_TMR0_CH0_ISR_PRI
#define EE_CORTEX_MX_INT_24_ISR_PRI    EE_CORTEX_MX_TMR0_CH0_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_CH0_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_CH0_ISR */

#ifdef  EE_CORTEX_MX_TMR0_CH1_ISR
#define EE_CORTEX_MX_INT_25_ISR        EE_CORTEX_MX_TMR0_CH1_ISR
#ifdef  EE_CORTEX_MX_TMR0_CH1_ISR_PRI
#define EE_CORTEX_MX_INT_25_ISR_PRI    EE_CORTEX_MX_TMR0_CH1_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_CH1_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_CH1_ISR */

#ifdef  EE_CORTEX_MX_TMR0_CH2_ISR
#define EE_CORTEX_MX_INT_26_ISR        EE_CORTEX_MX_TMR0_CH2_ISR
#ifdef  EE_CORTEX_MX_TMR0_CH2_ISR_PRI
#define EE_CORTEX_MX_INT_26_ISR_PRI    EE_CORTEX_MX_TMR0_CH2_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_CH2_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_CH2_ISR */

#ifdef  EE_CORTEX_MX_TMR0_CH3_ISR
#define EE_CORTEX_MX_INT_27_ISR        EE_CORTEX_MX_TMR0_CH3_ISR
#ifdef  EE_CORTEX_MX_TMR0_CH3_ISR_PRI
#define EE_CORTEX_MX_INT_27_ISR_PRI    EE_CORTEX_MX_TMR0_CH3_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_CH3_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_CH3_ISR */

#ifdef  EE_CORTEX_MX_TMR0_UDC_ISR
#define EE_CORTEX_MX_INT_28_ISR        EE_CORTEX_MX_TMR0_UDC_ISR
#ifdef  EE_CORTEX_MX_TMR0_UDC_ISR_PRI
#define EE_CORTEX_MX_INT_28_ISR_PRI    EE_CORTEX_MX_TMR0_UDC_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_UDC_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_UDC_ISR */

#ifdef  EE_CORTEX_MX_TMR0_ETR_ISR
#define EE_CORTEX_MX_INT_29_ISR        EE_CORTEX_MX_TMR0_ETR_ISR
#ifdef  EE_CORTEX_MX_TMR0_ETR_ISR_PRI
#define EE_CORTEX_MX_INT_29_ISR_PRI    EE_CORTEX_MX_TMR0_ETR_ISR_PRI
#endif  /* EE_CORTEX_MX_TMR0_ETR_ISR_PRI */
#endif  /* EE_CORTEX_MX_TMR0_ETR_ISR */

#ifdef  EE_CORTEX_MX_DMA330_0_ISR
#define EE_CORTEX_MX_INT_15_ISR        EE_CORTEX_MX_DMA330_0_ISR
#ifdef  EE_CORTEX_MX_DMA330_0_ISR_PRI
#define EE_CORTEX_MX_INT_15_ISR_PRI    EE_CORTEX_MX_DMA330_0_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_0_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_0_ISR */

#ifdef  EE_CORTEX_MX_DMA330_1_ISR
#define EE_CORTEX_MX_INT_16_ISR        EE_CORTEX_MX_DMA330_1_ISR
#ifdef  EE_CORTEX_MX_DMA330_1_ISR_PRI
#define EE_CORTEX_MX_INT_16_ISR_PRI    EE_CORTEX_MX_DMA330_1_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_1_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_1_ISR */

#ifdef  EE_CORTEX_MX_DMA330_2_ISR
#define EE_CORTEX_MX_INT_17_ISR        EE_CORTEX_MX_DMA330_2_ISR
#ifdef  EE_CORTEX_MX_DMA330_2_ISR_PRI
#define EE_CORTEX_MX_INT_17_ISR_PRI    EE_CORTEX_MX_DMA330_2_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_2_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_2_ISR */

#ifdef  EE_CORTEX_MX_DMA330_3_ISR
#define EE_CORTEX_MX_INT_18_ISR        EE_CORTEX_MX_DMA330_3_ISR
#ifdef  EE_CORTEX_MX_DMA330_3_ISR_PRI
#define EE_CORTEX_MX_INT_18_ISR_PRI    EE_CORTEX_MX_DMA330_3_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_3_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_3_ISR */

#ifdef  EE_CORTEX_MX_DMA330_4_ISR
#define EE_CORTEX_MX_INT_19_ISR        EE_CORTEX_MX_DMA330_4_ISR
#ifdef  EE_CORTEX_MX_DMA330_4_ISR_PRI
#define EE_CORTEX_MX_INT_19_ISR_PRI    EE_CORTEX_MX_DMA330_4_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_4_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_4_ISR */

#ifdef  EE_CORTEX_MX_DMA330_5_ISR
#define EE_CORTEX_MX_INT_1A_ISR        EE_CORTEX_MX_DMA330_5_ISR
#ifdef  EE_CORTEX_MX_DMA330_5_ISR_PRI
#define EE_CORTEX_MX_INT_1A_ISR_PRI    EE_CORTEX_MX_DMA330_5_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_5_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_5_ISR */

#ifdef  EE_CORTEX_MX_DMA330_6_ISR
#define EE_CORTEX_MX_INT_1B_ISR        EE_CORTEX_MX_DMA330_6_ISR
#ifdef  EE_CORTEX_MX_DMA330_6_ISR_PRI
#define EE_CORTEX_MX_INT_1B_ISR_PRI    EE_CORTEX_MX_DMA330_6_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_6_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_6_ISR */

#ifdef  EE_CORTEX_MX_DMA330_7_ISR
#define EE_CORTEX_MX_INT_1C_ISR        EE_CORTEX_MX_DMA330_7_ISR
#ifdef  EE_CORTEX_MX_DMA330_7_ISR_PRI
#define EE_CORTEX_MX_INT_1C_ISR_PRI    EE_CORTEX_MX_DMA330_7_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_7_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_7_ISR */

#ifdef  EE_CORTEX_MX_DMA330_ABORT_ISR
#define EE_CORTEX_MX_INT_1F_ISR        EE_CORTEX_MX_DMA330_ABORT_ISR
#ifdef  EE_CORTEX_MX_DMA330_ABORT_ISR_PRI
#define EE_CORTEX_MX_INT_1F_ISR_PRI    EE_CORTEX_MX_DMA330_ABORT_ISR_PRI
#endif  /* EE_CORTEX_MX_DMA330_ABORT_ISR_PRI */
#endif  /* EE_CORTEX_MX_DMA330_ABORT_ISR */

#ifdef  EE_CORTEX_MX_I2S0_ISR
#define EE_CORTEX_MX_INT_44_ISR        EE_CORTEX_MX_I2S0_ISR
#ifdef  EE_CORTEX_MX_I2S0_ISR_PRI
#define EE_CORTEX_MX_INT_44_ISR_PRI    EE_CORTEX_MX_I2S0_ISR_PRI
#endif  /* EE_CORTEX_MX_I2S0_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2S0_ISR */

#ifdef  EE_CORTEX_MX_I2S1_ISR
#define EE_CORTEX_MX_INT_45_ISR        EE_CORTEX_MX_I2S1_ISR
#ifdef  EE_CORTEX_MX_I2S1_ISR_PRI
#define EE_CORTEX_MX_INT_45_ISR_PRI    EE_CORTEX_MX_I2S1_ISR_PRI
#endif  /* EE_CORTEX_MX_I2S1_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2S1_ISR */

#ifdef  EE_CORTEX_MX_I2S2_ISR
#define EE_CORTEX_MX_INT_46_ISR        EE_CORTEX_MX_I2S2_ISR
#ifdef  EE_CORTEX_MX_I2S2_ISR_PRI
#define EE_CORTEX_MX_INT_46_ISR_PRI    EE_CORTEX_MX_I2S2_ISR_PRI
#endif  /* EE_CORTEX_MX_I2S2_ISR_PRI */
#endif  /* EE_CORTEX_MX_I2S2_ISR */

#ifdef  EE_CORTEX_MX_ADC_ISR
#define EE_CORTEX_MX_INT_39_ISR     EE_CORTEX_MX_ADC_ISR
#ifdef  EE_CORTEX_MX_ADC_ISR_PRI
#define EE_CORTEX_MX_INT_39_ISR_PRI EE_CORTEX_MX_ADC_ISR_PRI
#endif  /* EE_CORTEX_MX_ADC_ISR_PRI */
#endif  /* EE_CORTEX_MX_ADC_ISR */

#ifdef  EE_CORTEX_MX_ISP_NLINES_ISR
#define EE_CORTEX_MX_INT_4B_ISR     EE_CORTEX_MX_ISP_NLINES_ISR
#ifdef  EE_CORTEX_MX_ISP_NLINES_ISR_PRI
#define EE_CORTEX_MX_INT_4B_ISR_PRI EE_CORTEX_MX_ISP_NLINES_ISR_PRI
#endif  /* EE_CORTEX_MX_ISP_NLINES_ISR_PRI */
#endif  /* EE_CORTEX_MX_ISP_NLINES_ISR */
#endif

#endif /* BCM8956X_IRQS_H */
