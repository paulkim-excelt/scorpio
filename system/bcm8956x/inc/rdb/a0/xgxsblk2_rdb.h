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
    @file xgxsblk2_rdb.h
    @brief RDB File for XGXSBLK2

    @version 2019Jan21_rdb
*/

#ifndef XGXSBLK2_RDB_H
#define XGXSBLK2_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t xgxsBlk2_RESERVED_TYPE;




typedef uint16_t xgxsBlk2_INDCOMBCTRL_TYPE;
#define xgxsBlk2_INDCOMBCTRL_TX_POWERDOWN_OVER_MASK (0x4U)
#define xgxsBlk2_INDCOMBCTRL_TX_POWERDOWN_OVER_SHIFT (2U)
#define xgxsBlk2_INDCOMBCTRL_MASTERLN_INDX_MASK (0x3U)
#define xgxsBlk2_INDCOMBCTRL_MASTERLN_INDX_SHIFT (0U)




typedef uint16_t xgxsBlk2_TESTMODELANE_TYPE;
#define xgxsBlk2_TESTMODELANE_RESCAL_SEL_MASK (0x40U)
#define xgxsBlk2_TESTMODELANE_RESCAL_SEL_SHIFT (6U)
#define xgxsBlk2_TESTMODELANE_TOGGLE_FRACN_SEL_MASK (0x20U)
#define xgxsBlk2_TESTMODELANE_TOGGLE_FRACN_SEL_SHIFT (5U)
#define xgxsBlk2_TESTMODELANE_TM_SLICE_MASK (0x3U)
#define xgxsBlk2_TESTMODELANE_TM_SLICE_SHIFT (0U)




typedef uint16_t xgxsBlk2_TESTMODECOMBO_TYPE;
#define xgxsBlk2_TESTMODECOMBO_TEST_MONITOR_MODE2_MASK (0xfc0U)
#define xgxsBlk2_TESTMODECOMBO_TEST_MONITOR_MODE2_SHIFT (6U)
#define xgxsBlk2_TESTMODECOMBO_TEST_MONITOR_MODE1_MASK (0x3fU)
#define xgxsBlk2_TESTMODECOMBO_TEST_MONITOR_MODE1_SHIFT (0U)




typedef uint16_t xgxsBlk2_TESTMODEMUX_TYPE;
#define xgxsBlk2_TESTMODEMUX_TMUX_SEL_MASK (0xeU)
#define xgxsBlk2_TESTMODEMUX_TMUX_SEL_SHIFT (1U)
#define xgxsBlk2_TESTMODEMUX_TMUX_EN_MASK (0x1U)
#define xgxsBlk2_TESTMODEMUX_TMUX_EN_SHIFT (0U)




typedef uint16_t xgxsBlk2_CX4SIGDETCNT_TYPE;
#define xgxsBlk2_CX4SIGDETCNT_CX4SIGDETCNT_MASK (0xffffU)
#define xgxsBlk2_CX4SIGDETCNT_CX4SIGDETCNT_SHIFT (0U)




typedef uint16_t xgxsBlk2_MISCRESET_TYPE;
#define xgxsBlk2_MISCRESET_RESET_MDIO_MASK (0x8000U)
#define xgxsBlk2_MISCRESET_RESET_MDIO_SHIFT (15U)
#define xgxsBlk2_MISCRESET_RESET_PLL_MASK (0x100U)
#define xgxsBlk2_MISCRESET_RESET_PLL_SHIFT (8U)




typedef uint16_t xgxsBlk2_QSGMII_TYPE;
#define xgxsBlk2_QSGMII_DISPERROR_EN_FORCE_MASK (0x8000U)
#define xgxsBlk2_QSGMII_DISPERROR_EN_FORCE_SHIFT (15U)
#define xgxsBlk2_QSGMII_DISPERROR_EN_VAL_MASK (0x4000U)
#define xgxsBlk2_QSGMII_DISPERROR_EN_VAL_SHIFT (14U)
#define xgxsBlk2_QSGMII_DISPERROR_SYNC_VAL_MASK (0x2000U)
#define xgxsBlk2_QSGMII_DISPERROR_SYNC_VAL_SHIFT (13U)
#define xgxsBlk2_QSGMII_DISPARITY_EN_VAL_MASK (0x1000U)
#define xgxsBlk2_QSGMII_DISPARITY_EN_VAL_SHIFT (12U)
#define xgxsBlk2_QSGMII_QSGMII_K28P1_LN_SEL_MASK (0xc0U)
#define xgxsBlk2_QSGMII_QSGMII_K28P1_LN_SEL_SHIFT (6U)
#define xgxsBlk2_QSGMII_QSGMII_AUTO_PDRST_LN123_DISABLE_MASK (0x20U)
#define xgxsBlk2_QSGMII_QSGMII_AUTO_PDRST_LN123_DISABLE_SHIFT (5U)
#define xgxsBlk2_QSGMII_QSGMII_ADJ_EN_FORCE_MASK (0x10U)
#define xgxsBlk2_QSGMII_QSGMII_ADJ_EN_FORCE_SHIFT (4U)
#define xgxsBlk2_QSGMII_QSGMII_ADJ_EN_VAL_MASK (0x8U)
#define xgxsBlk2_QSGMII_QSGMII_ADJ_EN_VAL_SHIFT (3U)
#define xgxsBlk2_QSGMII_QSGMII_SYNC_FORCE_MASK (0x4U)
#define xgxsBlk2_QSGMII_QSGMII_SYNC_FORCE_SHIFT (2U)
#define xgxsBlk2_QSGMII_QSGMII_PIN_DISABLE_MASK (0x2U)
#define xgxsBlk2_QSGMII_QSGMII_PIN_DISABLE_SHIFT (1U)
#define xgxsBlk2_QSGMII_QSGMII_EN_MASK (0x1U)
#define xgxsBlk2_QSGMII_QSGMII_EN_SHIFT (0U)




typedef uint16_t xgxsBlk2_BLOCKADDRESS_TYPE;
#define xgxsBlk2_BLOCKADDRESS_BLOCKADDRESS_MASK (0x7ff0U)
#define xgxsBlk2_BLOCKADDRESS_BLOCKADDRESS_SHIFT (4U)




typedef volatile struct COMP_PACKED _xgxsBlk2_RDBType {
    XGXSBLK2_RESERVED_TYPE rsvd0[10]; /* OFFSET: 0x0 */
    XGXSBLK2_INDCOMBCTRL_TYPE indcombctrl; /* OFFSET: 0xa */
    XGXSBLK2_TESTMODELANE_TYPE testmodelane; /* OFFSET: 0xc */
    XGXSBLK2_TESTMODECOMBO_TYPE testmodecombo; /* OFFSET: 0xe */
    XGXSBLK2_TESTMODEMUX_TYPE testmodemux; /* OFFSET: 0x10 */
    XGXSBLK2_CX4SIGDETCNT_TYPE cx4sigdetcnt; /* OFFSET: 0x12 */
    XGXSBLK2_MISCRESET_TYPE miscreset; /* OFFSET: 0x14 */
    XGXSBLK2_QSGMII_TYPE qsgmii; /* OFFSET: 0x16 */
    XGXSBLK2_RESERVED_TYPE rsvd1[6]; /* OFFSET: 0x18 */
    XGXSBLK2_BLOCKADDRESS_TYPE blockaddress; /* OFFSET: 0x1e */
} xgxsBlk2_RDBType;


#define BLK2_BASE                       (0x4A410200UL)



#define XGXSBLK2_MAX_HW_ID              (1UL)

#endif /* xgxsBlk2_RDB_H */
