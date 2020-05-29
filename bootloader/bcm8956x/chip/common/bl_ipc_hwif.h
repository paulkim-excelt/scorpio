/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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

/******************************************************************************
 File Name:  bl_ipc_hwif.h
 Description: Defines the macros/structures exported to HOST
 ******************************************************************************/

#ifndef BL_IPC_HWIF_H
#define BL_IPC_HWIF_H

#define BL_IPC_MAX_PTR_MASK                (0xFU)
#define BL_IPC_BUFF_INFO_BASE_ALIGN_SHIFT  (11UL)
#define BL_IPC_BUFF_INFO2_BASE_ALIGN_SHIFT (19UL)
#define BL_IPC_INTR_NUM                    (7UL)

#define BL_IPC_BUFF_INFO_BASE_SHIFT        (8UL)
#define BL_IPC_BUFF_INFO_BASE_MASK         (0xFF00U)
#define BL_IPC_BUFF_INFO_SZ_SHIFT          (4UL)
#define BL_IPC_BUFF_INFO_SZ_MASK           (0x00F0U)
#define BL_IPC_BUFF_INFO_CNT_SHIFT         (2UL)
#define BL_IPC_BUFF_INFO_CNT_MASK          (0x000CU)
#define BL_IPC_BUFF_INFO_RSVD_SHIFT        (1UL)
#define BL_IPC_BUFF_INFO_RSVD_MASK         (0x0002U)
#define BL_IPC_BUFF_INFO_PAR_SHIFT         (0UL)
#define BL_IPC_BUFF_INFO_PAR_MASK          (0x0001U)

#define BL_IPC_TARGET_STAT_REG             ((volatile uint16_t *)MISC_SPARE_SW_REG8)
#define BL_IPC_TARGET_STAT_RSVD_SHIFT      (13UL)
#define BL_IPC_TARGET_STAT_RSVD_MASK       (0xE000U)
#define BL_IPC_TARGET_STAT_ERROR_SHIFT     (12UL)
#define BL_IPC_TARGET_STAT_ERROR_MASK      (0x1000U)
#define BL_IPC_TARGET_STAT_READY_SHIFT     (11UL)
#define BL_IPC_TARGET_STAT_READY_MASK      (0x0800U)
#define BL_IPC_TARGET_STAT_PRI_SHIFT       (8UL)
#define BL_IPC_TARGET_STAT_PRI_MASK        (0x0700U)
#define BL_IPC_TARGET_STAT_PRI_REBOOT      (7U)
#define BL_IPC_TARGET_STAT_PRI_6           (6U)
#define BL_IPC_TARGET_STAT_PRI_5           (5U)
#define BL_IPC_TARGET_STAT_PRI_4           (4U)
#define BL_IPC_TARGET_STAT_PRI_3           (3U)
#define BL_IPC_TARGET_STAT_PRI_2           (2U)
#define BL_IPC_TARGET_STAT_PRI_1           (1U)
#define BL_IPC_TARGET_STAT_PRI_PAUSE       (0U)
#define BL_IPC_TARGET_STAT_WR_SHIFT        (4UL)
#define BL_IPC_TARGET_STAT_WR_MASK         (BL_IPC_MAX_PTR_MASK << BL_IPC_TARGET_STAT_WR_SHIFT)
#define BL_IPC_TARGET_STAT_RD_SHIFT        (0UL)
#define BL_IPC_TARGET_STAT_RD_MASK         (BL_IPC_MAX_PTR_MASK << BL_IPC_TARGET_STAT_RD_SHIFT)

#define BL_IPC_HOST_STAT_REG               ((volatile uint16_t *)MISC_SPARE_SW_REG9)
#define BL_IPC_HOST_STAT_RSVD_SHIFT        (8UL)
#define BL_IPC_HOST_STAT_RSVD_MASK         (0xFF00U)
#define BL_IPC_HOST_STAT_WR_SHIFT          (4UL)
#define BL_IPC_HOST_STAT_WR_MASK           (BL_IPC_MAX_PTR_MASK << BL_IPC_HOST_STAT_WR_SHIFT)
#define BL_IPC_HOST_STAT_RD_SHIFT          (0UL)
#define BL_IPC_HOST_STAT_RD_MASK           (BL_IPC_MAX_PTR_MASK << BL_IPC_HOST_STAT_RD_SHIFT)

#define BL_IPC_BUFF_INFO2_BASE_SHIFT       (1UL)
#define BL_IPC_BUFF_INFO2_BASE_MASK        (0x3FFEU)
#define BL_IPC_BUFF_INFO2_RSVD_SHIFT       (14UL)
#define BL_IPC_BUFF_INFO2_RSVD_MASK        (0xC000U)
#define BL_IPC_BUFF_INFO2_PAR_SHIFT        (0UL)
#define BL_IPC_BUFF_INFO2_PAR_MASK         (0x0001U)

#define BL_IPC_COMMAND_MAGIC           (0xa5a5a5a5UL)
#define BL_IPC_REPLY_MAGIC             (0x5a5a5a5aUL)

#define BL_IPC_HDR_MAGIC_INDEX        (0UL)
#define BL_IPC_HDR_CHKSUM_INDEX       (1UL)
#define BL_IPC_HDR_COMMAND_INDEX      (2UL)
#define BL_IPC_HDR_LENGTH_INDEX       (3UL)
#define BL_IPC_HDR_LAST_INDEX         (4UL)

#endif /* BL_IPC_HWIF_H */
