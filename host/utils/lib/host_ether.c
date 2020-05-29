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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "hipc.h"
#include <bcm_err.h>
#include "stdint.h"
#include <string.h>
#include <etherswt_ipc.h>
#include <ether_ipc.h>
#include <rpc_cmds.h>
#include <host_ether.h>
#include <host_etherswt.h>
#include <utils.h>
#include <hlog.h>

#define MAX_SYNC_PULSES 8

static int32_t HOST_EtherBroadcastMessage(MgmtInfoType *info, uint32_t *spiId, uint32_t count, uint32_t id)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETHERSWT_PayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETHERSWT_PayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;
    uint32_t i;
    uint32_t retry;

    if ((info == NULL) || (spiId == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p, spiId = %p)\n",
            __FUNCTION__, info, spiId);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = id;

    /* Send out message to all active devices */
    for (i = 0; i < count; i++) {
        retVal = HIPC_SetSlave(spiId[i]);
        if (BCM_ERR_OK != retVal) {
            HOST_Log("%s Failed to set slave ID err:%x\n", __func__, retVal);
            goto done;
        }
        retry = 5;
        do {
            retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
                    0UL, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);
            if (retVal == BCM_ERR_OK) {
                if (uswap32(resp.status) != BCM_ERR_OK) {
                    HOST_Log("%s spiId:%u status:%d\n", __func__, spiId[i], uswap32(resp.status));
                    retVal = BCM_ERR_UNKNOWN;
                } else if (replyLen != MGMT_STATUS_LEN) {
                    HOST_Log("%s spiId:%u len:0x%x\n", __func__, spiId[i], replyLen);
                    retVal = BCM_ERR_DATA_INTEG;
                } else {
                    retry = 0;
                }
                break;
            } if (retVal == BCM_ERR_BUSY) {
                retry--;
                continue;
            } else {
                HOST_Log("%s HIPC_SendRecv failed for ID:%u err:0x%x\n", __func__,spiId[i], retVal);
                break;
            }
        } while (retry != 0);
    }
done:
    return retVal;
}

/* Send an IPC message and retrieve heartbeat values */
static int32_t HOST_EtherRead1588Heartbeat(MgmtInfoType *info, uint64_t* const heartbeat0, uint64_t* const heartbeat1)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETHER_PayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETHER_PayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (heartbeat0 == NULL) || (heartbeat1 == NULL)) {
        HOST_Log("%s :: Invalid input info = %p, hb0 = %p, hb1 = %p\n",
            __FUNCTION__, info, heartbeat0, heartbeat1);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_HEARTBEAT_GET);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHER_HeartBeatType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHER_HeartBeatType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *heartbeat0 = uswap64(respHdl.heartBeat->hb0);
        *heartbeat1 = uswap64(respHdl.heartBeat->hb1);
    }
done:
    return retVal;
}

static void UpdateMinMax(uint64_t *val, uint64_t *min, uint64_t *max)
{
    if (*val > *max) {
        *max = *val;
    }
    if (*val < *min) {
        *min = *val;
    }
}

static int32_t validate_1588_sync(MgmtInfoType *info, uint32_t *spiId, uint32_t count)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t i;
    uint64_t heartbeat1, heartbeat2;
    uint64_t maxheartbeat = 0ULL, minheartbeat = ~(0ULL);
    uint64_t diff;

    for (i = 0; i < count; i++) {
        retVal = HIPC_SetSlave(spiId[i]);
        if (BCM_ERR_OK != retVal) {
            HOST_Log("Failed to set slave ID err:%x\n", retVal);
            goto done;
        }
        retVal = HOST_EtherRead1588Heartbeat(info, &heartbeat1, &heartbeat2);
        if (retVal != BCM_ERR_OK) {
            HOST_Log("Failed to read heartbeat, spiId:%u err:%d\n",
                spiId[i], retVal);
            break;
        }

        UpdateMinMax(&heartbeat1, &minheartbeat, &maxheartbeat);
        UpdateMinMax(&heartbeat2, &minheartbeat, &maxheartbeat);
    }

    if (retVal == BCM_ERR_OK) {
        diff = maxheartbeat - minheartbeat;
        if (diff <= 8ULL) {
            retVal = BCM_ERR_OK;
        } else {
            HOST_Log("Difference in min and max heartbeats: %"PRIu64"\n", diff);
            retVal = BCM_ERR_EAGAIN;
        }
    }

done:
    return retVal;
}

static int32_t HOST_EtherPrepareSync(MgmtInfoType *info, uint32_t *spiId, uint32_t count)
{
    return HOST_EtherBroadcastMessage(info, spiId, count,
            RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_PREPARE_SYNC));
}

static int32_t HOST_EtherHeartbeatRead(MgmtInfoType *info, uint32_t *spiId, uint32_t count)
{
    return HOST_EtherBroadcastMessage(info, spiId, count,
            RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_PREPARE_HEARTBEAT_READ));
}

static int32_t HOST_EtherTimeSyncAcknowledge(MgmtInfoType *info, uint32_t *spiId, uint32_t count)
{
    return HOST_EtherBroadcastMessage(info, spiId, count,
            RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_TIMESYNC_ACK));
}

int32_t HOST_EtherStackSync(MgmtInfoType *info)
{
    uint32_t originalSpiId;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t max_tries = 0;
    uint32_t spiId[4UL];
    uint32_t count = 4UL;

    retVal = HIPC_GetSlave(&originalSpiId);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to get slave ID err:%x\n", __func__, retVal);
        goto err_getslave;
    }

    retVal = HIPC_GetActiveSlaves(&spiId[0], &count);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to get Actives slaves err:%x\n", __func__, retVal);
        goto err_getslave;
    }

    if (info == NULL) {
        HOST_Log("%s Invalid parameters info:%p\n", __func__, info);
        goto err_invalParams;
    }

    if ((count <= 1UL)
        || ((count == 2UL) && (spiId[0] == spiId[1]))
        || ((count == 3UL) && ((spiId[0] == spiId[1]) || (spiId[0] == spiId[2]) || (spiId[1] == spiId[2])))) {
        /*
        HOST_Log("%s Invalid parameters spiId1:%d spiId2:%d spiId3:%d count:%d\n",
                __func__, spiId[0], spiId[1], spiId[2], count);
        */
        retVal = BCM_ERR_NOSUPPORT;
        goto err_invalParams;
    }

    do {
        /* Communicate with target devices before starting sync */
        retVal = HOST_EtherPrepareSync(info, spiId, count);
        if (retVal != BCM_ERR_OK) {
            HOST_Log("Failed to prepare devices for sync, err:%d\n", retVal);
            break;
        }

        /* Pulse SYNC_OUT */
        HIPC_GenerateFrameSyncPulse();

        /* Acknowledge target devices of generating sync pulse and prepare them
         * for reading heartbeat to verify */
        retVal = HOST_EtherHeartbeatRead(info, spiId, count);
        if (retVal != BCM_ERR_OK) {
            HOST_Log("Failed to prepare devices for reading heartbeat, err:%d\n", retVal);
            break;
        }

        /* Pulse SYNC_OUT */
        HIPC_GenerateFrameSyncPulse();

        /* Read HeartBeat from each of the target devices and ascertain if
         * synchronisation has been achieved */
        retVal = validate_1588_sync(info, spiId, count);

        max_tries++;
    } while ((retVal != BCM_ERR_OK) && (max_tries < MAX_SYNC_PULSES));

    if (retVal == BCM_ERR_OK) {
        HOST_Log("Successfully synchronised after %u tries\n", max_tries);

        retVal = HOST_EtherTimeSyncAcknowledge(info, spiId, count);
        if (retVal != BCM_ERR_OK) {
            HOST_Log("Failed to acknowledge devices of sync, err:%d\n", retVal);
        }
    } else {
        HOST_Log("Failed to sync devices after %u tries\n", max_tries);
    }

    /* Restore the original SPI id  */
    if (BCM_ERR_OK != HIPC_SetSlave(originalSpiId)) {
        HOST_Log("Failed to set slave ID\n");
    }
err_invalParams:
err_getslave:
    return retVal;
}

int32_t HOST_MDIORead(MgmtInfoType *info, MDIO_RegAccessType access,
                      MDIO_HwIDType mdioHwID, MDIO_SlaveAddrType phy,
                      MDIO_DeviceType dev, uint16_t reg, uint16_t *val,
                      MCU_DeviceType destn)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETHER_PayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETHER_PayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (val == NULL)) {
        HOST_Log("%s Invalid parameters info:%p val:%p\n", __func__, info, val);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_MDIO_READ);

    cmdHdl.mdioAccess->pkt.regAccMode = access;
    cmdHdl.mdioAccess->pkt.slaveAddr = phy;
    cmdHdl.mdioAccess->pkt.deviceType = dev;
    cmdHdl.mdioAccess->pkt.regAddr = uswap16(reg);
    cmdHdl.mdioAccess->deviceID = uswap32(destn);
    cmdHdl.mdioAccess->mdioHwID = uswap32(mdioHwID);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHER_MDIOAccessType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHER_MDIOAccessType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *val = (uint16_t)uswap16(respHdl.mdioAccess->pkt.regData);
    }

done:
    return retVal;
}

int32_t HOST_MDIOWrite(MgmtInfoType *info, MDIO_RegAccessType access,
                       MDIO_HwIDType mdioHwID, MDIO_SlaveAddrType phy,
                       MDIO_DeviceType dev, uint16_t reg, uint16_t val,
                       MCU_DeviceType destn)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETHER_PayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETHER_PayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s Invalid parameters info:%p\n", __func__, info);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_AMC_ID, ETHER_CMDID_MDIO_WRITE);

    cmdHdl.mdioAccess->pkt.regAccMode = access;
    cmdHdl.mdioAccess->pkt.slaveAddr = phy;
    cmdHdl.mdioAccess->pkt.deviceType = dev;
    cmdHdl.mdioAccess->pkt.regAddr = uswap16(reg);
    cmdHdl.mdioAccess->pkt.regData = uswap16(val);
    cmdHdl.mdioAccess->deviceID = uswap32(destn);
    cmdHdl.mdioAccess->mdioHwID = uswap32(mdioHwID);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHER_MDIOAccessType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != MGMT_STATUS_LEN) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherNotificationHandler(uint32_t currentSlave,
            uint8_t notificationId, uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    return retVal;
}

