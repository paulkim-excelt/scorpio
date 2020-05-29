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
    @file mdio_rdb.h
    @brief RDB File for MDIO

    @version 2019Jan21_rdb
*/

#ifndef MDIO_RDB_H
#define MDIO_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t MDIO_CTRL_TYPE;
#define MDIO_CTRL_BYP_MASK (0x400UL)
#define MDIO_CTRL_BYP_SHIFT (10UL)
#define MDIO_CTRL_EXT_MASK (0x200UL)
#define MDIO_CTRL_EXT_SHIFT (9UL)
#define MDIO_CTRL_BSY_MASK (0x100UL)
#define MDIO_CTRL_BSY_SHIFT (8UL)
#define MDIO_CTRL_PRE_MASK (0x80UL)
#define MDIO_CTRL_PRE_SHIFT (7UL)
#define MDIO_CTRL_MDCDIV_MASK (0x7fUL)
#define MDIO_CTRL_MDCDIV_SHIFT (0UL)




typedef uint32_t MDIO_CMD_TYPE;
#define MDIO_CMD_SB_MASK (0xc0000000UL)
#define MDIO_CMD_SB_SHIFT (30UL)
#define MDIO_CMD_OP_MASK (0x30000000UL)
#define MDIO_CMD_OP_SHIFT (28UL)
#define MDIO_CMD_PA_MASK (0xf800000UL)
#define MDIO_CMD_PA_SHIFT (23UL)
#define MDIO_CMD_RA_MASK (0x7c0000UL)
#define MDIO_CMD_RA_SHIFT (18UL)
#define MDIO_CMD_TA_MASK (0x30000UL)
#define MDIO_CMD_TA_SHIFT (16UL)
#define MDIO_CMD_MDATA_MASK (0xffffUL)
#define MDIO_CMD_MDATA_SHIFT (0UL)




typedef volatile struct COMP_PACKED _MDIO_RDBType {
    MDIO_CTRL_TYPE ctrl; /* OFFSET: 0x0 */
    MDIO_CMD_TYPE cmd; /* OFFSET: 0x4 */
} MDIO_RDBType;


#define MMI_BASE                        (0x4014D000UL)

#define MMI1_BASE                       (0x4014D400UL)

#define MMI2_BASE                       (0x4014D800UL)



#define MDIO_MAX_HW_ID                  (3UL)


#define MDIO_BASE0                      (MMI_BASE)


#define MDIO_BASE1                      (MMI1_BASE)


#define MDIO_BASE2                      (MMI2_BASE)


#define MDIO_CMD_OPCODE_CL22_WRITE      (1UL)


#define MDIO_CMD_OPCODE_CL22_READ       (2UL)


#define MDIO_CMD_OPCODE_CL45_WRITE_ADDR  (0UL)


#define MDIO_CMD_OPCODE_CL45_WRITE_DATA  (1UL)


#define MDIO_CMD_OPCODE_CL45_READ       (3UL)

#endif /* MDIO_RDB_H */
