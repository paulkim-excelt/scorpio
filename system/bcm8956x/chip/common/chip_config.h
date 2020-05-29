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
#ifndef BCM8956X_CONFIG_H
#define BCM8956X_CONFIG_H

#include <ieee_cl22_rdb.h>
#include <cpu_indirect_rdb.h>
#include <switch_rdb.h>

#define CPU_CLOCK                 (400000000UL)  /**< 400 MHz */
#define NS_PER_CPU_CYCLE          (1000000000UL / CPU_CLOCK)
#define ETHERSWT_HW_ID_MAX        (1UL)
#define ETH_SWT_REGS_MAX_PAGES    (0xB0UL)
#define ETH_SWT_REGS_PAGE_SIZE    (256UL)
#define ETHERSWT_LIGHTSTACK_UPLINK_PORT_CNT (2UL)
#define ETHERSWT_LIGHTSTACK_SLAVES_PORT_TO_MASTER (8UL)
#define ETHERSWT_LIGHTSTACK_HOST_PORT (8UL)
#define ETH_SWT_BASE              (SWITCH_BASE)
#define ETHERSWT_INT_CLR_BASE     (CFG_SW_INTR_CLR)
#define ETHERSWT_INTERNAL_CPU_PORT (7UL)
#define IND_ACC_BASE              (CPU_INDIRECT_BASE)
#define SGMII_MAX_HW_ID           (5UL)
#define SGMII0_CL22_IEEE_BASE     (SG0_CL22_B0_BASE)
#define SGMII1_CL22_IEEE_BASE     (SG1_CL22_B0_BASE)
#define SGMII2_CL22_IEEE_BASE     (SG2_CL22_B0_BASE)
#define SGMII3_CL22_IEEE_BASE     (SG3_CL22_B0_BASE)
#define SGMII4_CL22_IEEE_BASE     (SGP_CL22_B0_BASE)
#define GPIO_MAX_CHANNELS         (9UL)
#define GPIO_MAX_PORTS            (3UL)
#define GPIO_GIO0_G0_BASE         (GIO0_GPIO_G0_DIN)
#define GPIO_GIO0_G1_BASE         (GIO0_GPIO_G1_DIN)
#define GPIO_GIO0_G2_BASE         (GIO0_FLASH_CS_DIN)

#define OTP_HW_ID_0                     (0UL)
#define OTP_MAC_ADDR_0_OCTET123_ADDR    (151UL)
#define OTP_MAC_ADDR_0_OCTET456_ADDR    (152UL)
#define OTP_MAC_ADDR_1_OCTET123_ADDR    (153UL)
#define OTP_MAC_ADDR_1_OCTET456_ADDR    (154UL)
#endif /* BCM8956X_CONFIG_H */
