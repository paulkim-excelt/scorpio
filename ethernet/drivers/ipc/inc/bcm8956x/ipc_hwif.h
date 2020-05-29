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

/******************************************************************************
 File Name:  ipc_hwif.h
 Descritpion: Defines the macros/structures exported to HOST
 ******************************************************************************/

#ifndef IPC_HWIF_H
#define IPC_HWIF_H

#include <spi1_rdb.h>
#include <chipmisc_rdb.h>

static CHIPMISC_RDBType *const CHIPMISC_REGS = (CHIPMISC_RDBType *)CHIPMISC_BASE;
#define IPC_BUFF_INFO_REG           (&(CHIPMISC_REGS->spare_sw_reg7))
#define IPC_BUFF_INFO2_REG          (&(CHIPMISC_REGS->spare_sw_reg10))
#define IPC_TARGET_STAT_REG         (&(CHIPMISC_REGS->spare_sw_reg8))
#define IPC_HOST_STAT_REG           (&(CHIPMISC_REGS->spare_sw_reg9))
#define MISC_CPUSYS_MISC            (&(CHIPMISC_REGS->cpusys_misc))
#define MISC_SPARE_SW_REG4          (&(CHIPMISC_REGS->spare_sw_reg4))
#define MISC_DEVICEID_LO            (&(CHIPMISC_REGS->deviceid_lo))
#define MISC_DEVICEID_HI            (&(CHIPMISC_REGS->deviceid_hi))
#define MISC_MODEL_REV_NUM          (&(CHIPMISC_REGS->model_rev_num))

/* TODO: replace MISC_CPUSYS_MISC_SOFT_INTR_MASK in c file itself */
#define MISC_CPUSYS_MISC_SOFT_INTR_MASK (CHIPMISC_CPUSYS_MISC_SOFT_INTR_MASK)

#define IPC_MAX_PTR_MASK                (0xFU)
#define IPC_BUFF_INFO_BASE_ALIGN_SHIFT  (11UL)
#define IPC_BUFF_INFO2_BASE_ALIGN_SHIFT (19UL)
#define IPC_INTR_NUM                    (7UL)

#define IPC_BUFF_INFO_BASE_SHIFT        (8UL)
#define IPC_BUFF_INFO_BASE_MASK         (0xFF00U)
#define IPC_BUFF_INFO_SZ_SHIFT          (4UL)
#define IPC_BUFF_INFO_SZ_MASK           (0x00F0U)
#define IPC_BUFF_INFO_CNT_SHIFT         (2UL)
#define IPC_BUFF_INFO_CNT_MASK          (0x000CU)
#define IPC_BUFF_INFO_RSVD_SHIFT        (1UL)
#define IPC_BUFF_INFO_RSVD_MASK         (0x0002U)
#define IPC_BUFF_INFO_PAR_SHIFT         (0UL)
#define IPC_BUFF_INFO_PAR_MASK          (0x0001U)

#define IPC_BUFF_INFO2_BASE_SHIFT       (1UL)
#define IPC_BUFF_INFO2_BASE_MASK        (0x3FFEU)
#define IPC_BUFF_INFO2_RSVD_SHIFT       (14UL)
#define IPC_BUFF_INFO2_RSVD_MASK        (0xC000U)
#define IPC_BUFF_INFO2_PAR_SHIFT        (0UL)
#define IPC_BUFF_INFO2_PAR_MASK         (0x0001U)

#define IPC_TARGET_STAT_RSVD_SHIFT      (11UL)
#define IPC_TARGET_STAT_RSVD_MASK       (0xF100U)
#define IPC_TARGET_STAT_PRI_SHIFT       (8UL)
#define IPC_TARGET_STAT_PRI_MASK        (0x0700U)
#define IPC_TARGET_STAT_PRI_REBOOT      (7U)
#define IPC_TARGET_STAT_PRI_6           (6U)
#define IPC_TARGET_STAT_PRI_5           (5U)
#define IPC_TARGET_STAT_PRI_4           (4U)
#define IPC_TARGET_STAT_PRI_3           (3U)
#define IPC_TARGET_STAT_PRI_2           (2U)
#define IPC_TARGET_STAT_PRI_1           (1U)
#define IPC_TARGET_STAT_PRI_PAUSE       (0U)
#define IPC_TARGET_STAT_WR_SHIFT        (4UL)
#define IPC_TARGET_STAT_WR_MASK         (IPC_MAX_PTR_MASK << IPC_TARGET_STAT_WR_SHIFT)
#define IPC_TARGET_STAT_RD_SHIFT        (0UL)
#define IPC_TARGET_STAT_RD_MASK         (IPC_MAX_PTR_MASK << IPC_TARGET_STAT_RD_SHIFT)

#define IPC_HOST_STAT_RSVD_SHIFT        (8UL)
#define IPC_HOST_STAT_RSVD_MASK         (0xFF00U)
#define IPC_HOST_STAT_WR_SHIFT          (4UL)
#define IPC_HOST_STAT_WR_MASK           (IPC_MAX_PTR_MASK << IPC_HOST_STAT_WR_SHIFT)
#define IPC_HOST_STAT_RD_SHIFT          (0UL)
#define IPC_HOST_STAT_RD_MASK           (IPC_MAX_PTR_MASK << IPC_HOST_STAT_RD_SHIFT)

#define IPC_COMMAND_MAGIC           (0xa5a5a5a5UL)
#define IPC_REPLY_MAGIC             (0x5a5a5a5aUL)

#define IPC_HDR_MAGIC_INDEX        (0UL)
#define IPC_HDR_CHKSUM_INDEX       (1UL)
#define IPC_HDR_COMMAND_INDEX      (2UL)
#define IPC_HDR_LENGTH_INDEX       (3UL)
#define IPC_HDR_LAST_INDEX         (4UL)
#endif /* IPC_HWIF_H */
