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

#ifndef SGMII_H
#define SGMII_H

typedef volatile struct {
    uint16_t    MII_CTRL;                 /**< BR LRE control */
#define MII_CTRL_RESET_MASK               (0x8000U)
#define MII_CTRL_RESET_SHIFT              (15U)
#define MII_CTRL_LOOPBACK_MASK            (0x4000U)
#define MII_CTRL_LOOPBACK_SHIFT           (14U)
#define MII_CTRL_MANUAL_SPEED0_MASK       (0x2000U)
#define MII_CTRL_MANUAL_SPEED0_SHIFT      (13U)
#define MII_CTRL_AUTONEG_EN_MASK          (0x1000U)
#define MII_CTRL_AUTONEG_EN_SHIFT         (12U)
#define MII_CTRL_POWER_DOWN_MASK          (0x0800U)
#define MII_CTRL_POWER_DOWN_SHIFT         (11U)
#define MII_CTRL_FULL_DUPLEX_MASK         (0x0100U)
#define MII_CTRL_FULL_DUPLEX_SHIFT        (8U)
#define MII_CTRL_MANUAL_SPEED1_MASK       (0x0040U)
#define MII_CTRL_MANUAL_SPEED1_SHIFT      (6U)
    uint16_t    MII_STAT;                 /**< BR LRE status */
#define MII_STAT_AUTONEG_COMPLETE_MASK    (0x0020U)
#define MII_STAT_AUTONEG_COMPLETE_SHIFT   (5U)
#define MII_STAT_AUTONEG_ABILITY_MASK     (0x0008U)
#define MII_STAT_AUTONEG_ABILITY_SHIFT    (3U)
#define MII_STAT_LNK_STAT_MASK            (0x0004U)
#define MII_STAT_LNK_STAT_SHIFT           (2U)
} SGMII_CL22_IEEE_RegType;

#endif /* SGMII_H */
