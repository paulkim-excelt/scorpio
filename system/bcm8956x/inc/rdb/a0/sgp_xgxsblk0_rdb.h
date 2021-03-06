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
    @file sgp_xgxsblk0_rdb.h
    @brief RDB File for SGP_XGXSBLK0

    @version 2019Jan21_rdb
*/

#ifndef SGP_XGXSBLK0_RDB_H
#define SGP_XGXSBLK0_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t SGP_XGXSBLK0_XGXSCONTROL_TYPE;
#define SGP_XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK (0x2000U)
#define SGP_XGXSBLK0_XGXSCONTROL_START_SEQUENCER_SHIFT (13U)
#define SGP_XGXSBLK0_XGXSCONTROL_MODE_MASK (0xf00U)
#define SGP_XGXSBLK0_XGXSCONTROL_MODE_SHIFT (8U)
#define SGP_XGXSBLK0_XGXSCONTROL_HSTL_MASK (0x20U)
#define SGP_XGXSBLK0_XGXSCONTROL_HSTL_SHIFT (5U)
#define SGP_XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK (0x10U)
#define SGP_XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_SHIFT (4U)
#define SGP_XGXSBLK0_XGXSCONTROL_CDET_EN_MASK (0x8U)
#define SGP_XGXSBLK0_XGXSCONTROL_CDET_EN_SHIFT (3U)
#define SGP_XGXSBLK0_XGXSCONTROL_EDEN_MASK (0x4U)
#define SGP_XGXSBLK0_XGXSCONTROL_EDEN_SHIFT (2U)
#define SGP_XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK (0x2U)
#define SGP_XGXSBLK0_XGXSCONTROL_AFRST_EN_SHIFT (1U)
#define SGP_XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK (0x1U)
#define SGP_XGXSBLK0_XGXSCONTROL_TXCKO_DIV_SHIFT (0U)




typedef uint8_t SGP_XGXSBLK0_RESERVED_TYPE;




typedef uint16_t SGP_XGXSBLK0_MMDSELECT_TYPE;
#define SGP_XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK (0x8000U)
#define SGP_XGXSBLK0_MMDSELECT_MULTIPRTS_EN_SHIFT (15U)
#define SGP_XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK (0x4000U)
#define SGP_XGXSBLK0_MMDSELECT_MULTIMMDS_EN_SHIFT (14U)
#define SGP_XGXSBLK0_MMDSELECT_PRTAD_BCST_DISABLE_MASK (0x2000U)
#define SGP_XGXSBLK0_MMDSELECT_PRTAD_BCST_DISABLE_SHIFT (13U)
#define SGP_XGXSBLK0_MMDSELECT_DEVAN_EN_MASK (0x8U)
#define SGP_XGXSBLK0_MMDSELECT_DEVAN_EN_SHIFT (3U)
#define SGP_XGXSBLK0_MMDSELECT_DEVPMD_EN_MASK (0x4U)
#define SGP_XGXSBLK0_MMDSELECT_DEVPMD_EN_SHIFT (2U)
#define SGP_XGXSBLK0_MMDSELECT_DEVDEVAD_EN_MASK (0x2U)
#define SGP_XGXSBLK0_MMDSELECT_DEVDEVAD_EN_SHIFT (1U)
#define SGP_XGXSBLK0_MMDSELECT_DEVCL22_EN_MASK (0x1U)
#define SGP_XGXSBLK0_MMDSELECT_DEVCL22_EN_SHIFT (0U)




typedef uint16_t SGP_XGXSBLK0_MISCCONTROL1_TYPE;
#define SGP_XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK (0x2U)
#define SGP_XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_SHIFT (1U)
#define SGP_XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK (0x1U)
#define SGP_XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_SHIFT (0U)




typedef uint16_t SGP_XGXSBLK0_BLOCKADDRESS_TYPE;
#define SGP_XGXSBLK0_BLOCKADDRESS_BLOCKADDRESS_MASK (0x7ff0U)
#define SGP_XGXSBLK0_BLOCKADDRESS_BLOCKADDRESS_SHIFT (4U)




typedef volatile struct COMP_PACKED _SGP_XGXSBLK0_RDBType {
    SGP_XGXSBLK0_XGXSCONTROL_TYPE xgxscontrol; /* OFFSET: 0x0 */
    SGP_XGXSBLK0_RESERVED_TYPE rsvd0[24]; /* OFFSET: 0x2 */
    SGP_XGXSBLK0_MMDSELECT_TYPE mmdselect; /* OFFSET: 0x1a */
    SGP_XGXSBLK0_MISCCONTROL1_TYPE misccontrol1; /* OFFSET: 0x1c */
    SGP_XGXSBLK0_BLOCKADDRESS_TYPE blockaddress; /* OFFSET: 0x1e */
} SGP_XGXSBLK0_RDBType;


#define SGP_BLK0_BASE                   (0x4A490000UL)



#define SGP_XGXSBLK0_MAX_HW_ID          (1UL)

#endif /* SGP_XGXSBLK0_RDB_H */
