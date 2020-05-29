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
    @file xgxsblk1_rdb.h
    @brief RDB File for XGXSBLK1

    @version 2019Jan21_rdb
*/

#ifndef XGXSBLK1_RDB_H
#define XGXSBLK1_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t xgxsBlk1_RESERVED_TYPE;




typedef uint16_t xgxsBlk1_LANECTRL3_TYPE;
#define xgxsBlk1_LANECTRL3_FORCE_PLL_LOCK_MASK (0x1000U)
#define xgxsBlk1_LANECTRL3_FORCE_PLL_LOCK_SHIFT (12U)
#define xgxsBlk1_LANECTRL3_PWRDWN_FORCE_MASK (0x800U)
#define xgxsBlk1_LANECTRL3_PWRDWN_FORCE_SHIFT (11U)
#define xgxsBlk1_LANECTRL3_LOCK_REF_EN_MASK (0x400U)
#define xgxsBlk1_LANECTRL3_LOCK_REF_EN_SHIFT (10U)
#define xgxsBlk1_LANECTRL3_PWRDWN10G_PLL_DIS_MASK (0x200U)
#define xgxsBlk1_LANECTRL3_PWRDWN10G_PLL_DIS_SHIFT (9U)
#define xgxsBlk1_LANECTRL3_PWRDWN_PLL_MASK (0x100U)
#define xgxsBlk1_LANECTRL3_PWRDWN_PLL_SHIFT (8U)




typedef uint16_t xgxsBlk1_LANETEST_TYPE;
#define xgxsBlk1_LANETEST_TMUX_SEL_MASK (0xf000U)
#define xgxsBlk1_LANETEST_TMUX_SEL_SHIFT (12U)
#define xgxsBlk1_LANETEST_PWRDN_EXT_DIS_MASK (0x400U)
#define xgxsBlk1_LANETEST_PWRDN_EXT_DIS_SHIFT (10U)
#define xgxsBlk1_LANETEST_PWRDN_SAFE_DIS_MASK (0x200U)
#define xgxsBlk1_LANETEST_PWRDN_SAFE_DIS_SHIFT (9U)
#define xgxsBlk1_LANETEST_PWRDWN_CLKS_EN_MASK (0x100U)
#define xgxsBlk1_LANETEST_PWRDWN_CLKS_EN_SHIFT (8U)
#define xgxsBlk1_LANETEST_RXSEQSTART_EXT_DIS_MASK (0x80U)
#define xgxsBlk1_LANETEST_RXSEQSTART_EXT_DIS_SHIFT (7U)
#define xgxsBlk1_LANETEST_PLL_LOCK_RSTB_R_MASK (0x40U)
#define xgxsBlk1_LANETEST_PLL_LOCK_RSTB_R_SHIFT (6U)
#define xgxsBlk1_LANETEST_LFCK_BYPASS_MASK (0x20U)
#define xgxsBlk1_LANETEST_LFCK_BYPASS_SHIFT (5U)
#define xgxsBlk1_LANETEST_RX_SNOOP_EN_MASK (0x10U)
#define xgxsBlk1_LANETEST_RX_SNOOP_EN_SHIFT (4U)
#define xgxsBlk1_LANETEST_MODE_10G_SNOOP_MASK (0xfU)
#define xgxsBlk1_LANETEST_MODE_10G_SNOOP_SHIFT (0U)




typedef uint16_t xgxsBlk1_BLOCKADDRESS_TYPE;
#define xgxsBlk1_BLOCKADDRESS_BLOCKADDRESS_MASK (0x7ff0U)
#define xgxsBlk1_BLOCKADDRESS_BLOCKADDRESS_SHIFT (4U)




typedef volatile struct COMP_PACKED _xgxsBlk1_RDBType {
    XGXSBLK1_RESERVED_TYPE rsvd0[16]; /* OFFSET: 0x0 */
    XGXSBLK1_LANECTRL3_TYPE lanectrl3; /* OFFSET: 0x10 */
    XGXSBLK1_RESERVED_TYPE rsvd1[2]; /* OFFSET: 0x12 */
    XGXSBLK1_LANETEST_TYPE lanetest; /* OFFSET: 0x14 */
    XGXSBLK1_RESERVED_TYPE rsvd2[8]; /* OFFSET: 0x16 */
    XGXSBLK1_BLOCKADDRESS_TYPE blockaddress; /* OFFSET: 0x1e */
} xgxsBlk1_RDBType;


#define BLK1_BASE                       (0x4A410020UL)



#define XGXSBLK1_MAX_HW_ID              (1UL)

#endif /* xgxsBlk1_RDB_H */
