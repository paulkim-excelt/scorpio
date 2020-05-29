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

#ifndef BCM89564G_EVK_BOARD_H
#define BCM89564G_EVK_BOARD_H

#include <bcm8956x/board/common/board.h>

#define FLASH0_SIZE                     (4UL * 1024UL * 1024UL)
#define FLASH0_SUBSECTOR_SIZE           (4UL * 1024UL)
#define FLASH0_SPEED                    (FLASH_SPEED_50M)
#define FLASH0_READ_LANE_CFG            (FLASH_READ_LANE_SINGLE)
#define FLASH0_FLASH_ID                 (FLASH_FLSID_MACRONIX_MX25L32)
#define FLASH0_FLSABLE_SEC_START_ADDR   (1024UL * 1024UL)

#if (FLASH0_SPEED == FLASH_SPEED_80M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_62M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_50M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_25M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ)
#else
#error "Invalid flash speed"
#endif

#define ETHER_TIMESYNC_DEFAULT        ETHER_TIMESYNC_INTERNAL
#define PT_LOOKUP_FLASH_ADDR           (0x0UL)

#endif /* BCM89564G_EVK_BOARD_H */
