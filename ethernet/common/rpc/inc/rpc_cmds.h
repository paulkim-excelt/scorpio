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


#ifndef RPC_CMDS_H
#define RPC_CMDS_H

#include <ipc.h>

#define MGMT_STATUS_LEN         (sizeof(uint32_t))

#define RPC_CMD_ASYNC_SHIFT     (31UL)
#define RPC_CMD_ASYNC_MASK      (1UL << RPC_CMD_ASYNC_SHIFT)

#define RPC_CMD_GROUP_SHIFT     (24UL)
#define RPC_CMD_GROUP_MASK      (0x7FUL << RPC_CMD_GROUP_SHIFT)

#define RPC_CMD_COMP_SHIFT      (8UL)
#define RPC_CMD_COMP_MASK       (0xFFFFUL << RPC_CMD_COMP_SHIFT)

#define RPC_CMD_ID_SHIFT        (0UL)
#define RPC_CMD_ID_MASK         (0xFFUL << RPC_CMD_ID_SHIFT)

#define RPC_ASYNCID(grp, comp, id) ((RPC_CMD_ASYNC_MASK) | \
                                   ((grp << RPC_CMD_GROUP_SHIFT) & RPC_CMD_GROUP_MASK) | \
                                   ((comp << RPC_CMD_COMP_SHIFT) & RPC_CMD_COMP_MASK) | \
                                   ((id << RPC_CMD_ID_SHIFT) & RPC_CMD_ID_MASK))

#define RPC_CMDID(grp, comp, id)   (((grp << RPC_CMD_GROUP_SHIFT) & RPC_CMD_GROUP_MASK) | \
                                   ((comp << RPC_CMD_COMP_SHIFT) & RPC_CMD_COMP_MASK) | \
                                   ((id << RPC_CMD_ID_SHIFT) & RPC_CMD_ID_MASK))

#define RPC_GET_GROUPID(aCmd)   ((aCmd & RPC_CMD_GROUP_MASK) \
                                    >> RPC_CMD_GROUP_SHIFT)

#define RPC_GET_COMP(aCmd)     ((aCmd & RPC_CMD_COMP_MASK) \
                                        >> RPC_CMD_COMP_SHIFT)

#define RPC_GET_CMDID(aCmd)     ((aCmd & RPC_CMD_ID_MASK) \
                                        >> RPC_CMD_ID_SHIFT)

#define RPC_CMDPAYLOADSZ   (RPC_MSG_PAYLOAD_SZ)
#define RPC_ASYNCPAYLOADSZ (RPC_MSG_PAYLOAD_SZ)
#define RPC_RESPPAYLOADSZ  (RPC_MSG_PAYLOAD_SZ - sizeof(uint32_t))

typedef struct _RPC_ResponseType {
    int32_t status;
    uint8_t payload[RPC_RESPPAYLOADSZ];
} RPC_ResponseType;

#endif /* RPC_CMDS_H */
