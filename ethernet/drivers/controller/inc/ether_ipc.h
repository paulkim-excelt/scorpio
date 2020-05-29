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

/**
    @defgroup grp_eth_cntlrdrv_ipc_ifc Ethernet IPC Interface
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_ipc_ifc
    @{

    @file ethernet/drivers/controller/inc/ether_ipc.h
    @brief Ethernet Driver interface
    This header file contains the interface functions for Ethernet IPC

    @version 1.26 Imported from docx
*/

#ifndef ETHER_IPC_H
#define ETHER_IPC_H

#include <stdint.h>
#include <stdlib.h>
#include <ethernet.h>
#include <rpc_cmds.h>
#include "mdio.h"

/**
    @name Ethernet Driver IPC API IDs
    @{
    @brief API IDs for Ethernet Driver
*/
#define BRCM_SWARCH_ETHER_CMDID_TYPE           (0x00U) /**< @brief #ETHER_CmdIdType */
#define BRCM_SWARCH_ETHER_HEARTBEAT_TYPE       (0x01U) /**< @brief #ETHER_HeartBeatType */
#define BRCM_SWARCH_ETHER_PAYLOAD_TYPE         (0x02U) /**< @brief #ETHER_PayloadType */
#define BRCM_SWARCH_ETHER_MDIOACCESS_TYPE      (0x03U) /**< @brief #ETHER_MDIOAccessType */
/** @} */

/**
    @brief Firmware - Ethernet command group

    @trace #BRCM_SWREQ_ETHER_IPC_COMMUNICATION
*/
typedef uint8_t ETHER_CmdIdType;
#define ETHER_CMDID_PREPARE_SYNC               (0x01U)
#define ETHER_CMDID_PREPARE_HEARTBEAT_READ     (0x02U)
#define ETHER_CMDID_HEARTBEAT_GET              (0x03U)
#define ETHER_CMDID_TIMESYNC_ACK               (0x04U)
#define ETHER_CMDID_MDIO_READ                  (0x05U)
#define ETHER_CMDID_MDIO_WRITE                 (0x06U)

/**
    @brief Structure to maintain the HeartBeat values

    @trace #BRCM_SWREQ_ETHER_IPC_COMMUNICATION
*/
typedef struct _ETHER_HeartBeatType {
    uint64_t hb0;
    uint64_t hb1;
} ETHER_HeartBeatType;

/**
    @brief Structure to perform MDIO read/write accesses

    @trace #BRCM_SWREQ_ETHER_IPC_COMMUNICATION
*/
typedef struct _ETHER_MDIOAccessType {
    uint32_t deviceID;  /* MCU_DeviceType */
    MDIO_HwIDType mdioHwID;
    MDIO_PktType pkt;
} ETHER_MDIOAccessType;

/**
    @brief Structure defining the IPC Payload type for HeartBeat

    @trace #BRCM_SWREQ_ETHER_IPC_COMMUNICATION
*/
typedef union _ETHER_PayloadType {
    uint8_t                *u8Ptr;
    ETHER_HeartBeatType    *heartBeat;
    ETHER_MDIOAccessType   *mdioAccess;
} ETHER_PayloadType;

_Static_assert(sizeof(ETHER_HeartBeatType) <= RPC_RESPPAYLOADSZ, "ETHER_HeartBeatType does not fit in payload");

#endif /* ETHER_IPC_H */
/** @} */
