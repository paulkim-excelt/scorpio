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
    @file sgp_pll2_rdb.h
    @brief RDB File for SGP_PLL2

    @version 2019Jan21_rdb
*/

#ifndef SGP_PLL2_RDB_H
#define SGP_PLL2_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t SGP_PLL2_PLL_STATUS_TYPE;
#define SGP_PLL2_PLL_STATUS_VCO_RANGE_MASK (0x1fc0U)
#define SGP_PLL2_PLL_STATUS_VCO_RANGE_SHIFT (6U)
#define SGP_PLL2_PLL_STATUS_CAL_STATE_MASK (0x3cU)
#define SGP_PLL2_PLL_STATUS_CAL_STATE_SHIFT (2U)
#define SGP_PLL2_PLL_STATUS_CAL_VALID_MASK (0x2U)
#define SGP_PLL2_PLL_STATUS_CAL_VALID_SHIFT (1U)
#define SGP_PLL2_PLL_STATUS_CAL_ERROR_MASK (0x1U)
#define SGP_PLL2_PLL_STATUS_CAL_ERROR_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_CAL_TYPE;
#define SGP_PLL2_PLL_CAL_CAL_TH_MASK (0x780U)
#define SGP_PLL2_PLL_CAL_CAL_TH_SHIFT (7U)
#define SGP_PLL2_PLL_CAL_EXT_RANGE_MASK (0x7fU)
#define SGP_PLL2_PLL_CAL_EXT_RANGE_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_RANGE_TYPE;
#define SGP_PLL2_PLL_RANGE_DFS_MASK (0x7f00U)
#define SGP_PLL2_PLL_RANGE_DFS_SHIFT (8U)
#define SGP_PLL2_PLL_RANGE_OVRD_MASK (0x80U)
#define SGP_PLL2_PLL_RANGE_OVRD_SHIFT (7U)
#define SGP_PLL2_PLL_RANGE_OVRD_VAL_MASK (0x7fU)
#define SGP_PLL2_PLL_RANGE_OVRD_VAL_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_CALIB_CAP_CHARGE_TYPE;
#define SGP_PLL2_PLL_CALIB_CAP_CHARGE_TIME_MASK (0xffffU)
#define SGP_PLL2_PLL_CALIB_CAP_CHARGE_TIME_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_CALIB_DELAY_TYPE;
#define SGP_PLL2_PLL_CALIB_DELAY_TIME_MASK (0xffffU)
#define SGP_PLL2_PLL_CALIB_DELAY_TIME_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_CALIB_STEP_TYPE;
#define SGP_PLL2_PLL_CALIB_STEP_TIME_MASK (0xffffU)
#define SGP_PLL2_PLL_CALIB_STEP_TIME_SHIFT (0U)




typedef uint16_t SGP_PLL2_PLL_DFE0_TYPE;
#define SGP_PLL2_PLL_DFE0_EN_CALIB_N_MASK (0x40U)
#define SGP_PLL2_PLL_DFE0_EN_CALIB_N_SHIFT (6U)
#define SGP_PLL2_PLL_DFE0_HALFSTEP_EN_MASK (0x20U)
#define SGP_PLL2_PLL_DFE0_HALFSTEP_EN_SHIFT (5U)
#define SGP_PLL2_PLL_DFE0_CALIB_SEARCH_BIT_MASK (0x1cU)
#define SGP_PLL2_PLL_DFE0_CALIB_SEARCH_BIT_SHIFT (2U)
#define SGP_PLL2_PLL_DFE0_VCOCAL_VALID_OVRD_MASK (0x2U)
#define SGP_PLL2_PLL_DFE0_VCOCAL_VALID_OVRD_SHIFT (1U)
#define SGP_PLL2_PLL_DFE0_VCOCAL_VALID_OVRD_VAL_MASK (0x1U)
#define SGP_PLL2_PLL_DFE0_VCOCAL_VALID_OVRD_VAL_SHIFT (0U)




typedef uint8_t SGP_PLL2_RESERVED_TYPE;




typedef uint16_t SGP_PLL2_BLOCKADDRESS_TYPE;
#define SGP_PLL2_BLOCKADDRESS_BLOCKADDRESS_MASK (0x7ff0U)
#define SGP_PLL2_BLOCKADDRESS_BLOCKADDRESS_SHIFT (4U)




typedef volatile struct COMP_PACKED _SGP_PLL2_RDBType {
    SGP_PLL2_PLL_STATUS_TYPE stat0; /* OFFSET: 0x0 */
    SGP_PLL2_PLL_CAL_TYPE ctrl1; /* OFFSET: 0x2 */
    SGP_PLL2_PLL_RANGE_TYPE ctrl2; /* OFFSET: 0x4 */
    SGP_PLL2_PLL_CALIB_CAP_CHARGE_TYPE ctrl3; /* OFFSET: 0x6 */
    SGP_PLL2_PLL_CALIB_DELAY_TYPE ctrl4; /* OFFSET: 0x8 */
    SGP_PLL2_PLL_CALIB_STEP_TYPE ctrl5; /* OFFSET: 0xa */
    SGP_PLL2_PLL_DFE0_TYPE ctrl6; /* OFFSET: 0xc */
    SGP_PLL2_RESERVED_TYPE rsvd0[16]; /* OFFSET: 0xe */
    SGP_PLL2_BLOCKADDRESS_TYPE blockaddress; /* OFFSET: 0x1e */
} SGP_PLL2_RDBType;


#define SGP_PLL2_BASE                   (0x4A490300UL)



#define SGP_PLL2_MAX_HW_ID              (1UL)

#endif /* SGP_PLL2_RDB_H */
