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

#ifndef BL_MMI_DRV_H
#define BL_MMI_DRV_H

typedef volatile struct {
    uint32_t CTRL;
#define MMI_CTRL_BUSY                   (0x100UL)
    uint32_t CMD;
#define MMI_CMD_START_MASK              (0xC0000000UL)
#define MMI_CMD_START_SHIFT             (30UL)
#define MMI_CMD_OP_CODE_MASK            (0x30000000UL)
#define MMI_CMD_OP_CODE_SHIFT           (28UL)
#define MMI_CMD_OPCODE_CL22_WRITE       (1UL)
#define MMI_CMD_OPCODE_CL22_READ        (2UL)
#define MMI_CMD_OPCODE_CL45_WRITE_ADDR  (0UL)
#define MMI_CMD_OPCODE_CL45_WRITE_DATA  (1UL)
#define MMI_CMD_OPCODE_CL45_READ        (3UL)
#define MMI_CMD_PHY_ADDR_MASK           (0x0F800000UL)
#define MMI_CMD_PHY_ADDR_SHIFT          (23UL)
#define MMI_CMD_REG_ADDR_MASK           (0x007C0000UL)
#define MMI_CMD_REG_ADDR_SHIFT          (18UL)
#define MMI_CMD_TA_MASK                 (0x00030000UL)
#define MMI_CMD_TA_SHIFT                (16UL)
#define MMI_CMD_DATA_MASK               (0xFFFFUL)
} BL_MMI_RegType;

#endif /* BL_MMI_DRV_H */
