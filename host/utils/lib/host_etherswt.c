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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "hipc.h"
#include <bcm_err.h>
#include <string.h>
#include <ether_ipc.h>
#include <rpc_cmds.h>
#include <host_etherswt.h>
#include <utils.h>
#include <hlog.h>

#define MAX_SYNC_PULSES 8
#define MGMT_ETHER_ADDR_LEN                    6

int32_t HOST_EtherSwtMib(MgmtInfoType *info, int32_t port, ETHERSWT_MibType *mib)
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

    if ((info == NULL) || (mib == NULL)) {
        HOST_Log("%s :: Invalid input param(info:%p mib:%p)\n",
                __FUNCTION__, info, mib);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.mib->port = uswap32(port);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_MIBS);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
                sizeof(ETHERSWT_MibType), (uint8_t *)&resp,
                RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_MibType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        mib->port = uswap32(respHdl.mib->port);
        mib->rxStats.gdPkts = uswap32(respHdl.mib->rxStats.gdPkts);
        mib->rxStats.octetsLow = uswap32(respHdl.mib->rxStats.octetsLow);
        mib->rxStats.octetsHigh = uswap32(respHdl.mib->rxStats.octetsHigh);
        mib->rxStats.allPkts = uswap32(respHdl.mib->rxStats.allPkts);
        mib->rxStats.brdCast = uswap32(respHdl.mib->rxStats.brdCast);
        mib->rxStats.mutCast = uswap32(respHdl.mib->rxStats.mutCast);
        mib->rxStats.uniCast = uswap32(respHdl.mib->rxStats.uniCast);
        mib->rxStats.pkts64 = uswap32(respHdl.mib->rxStats.pkts64);
        mib->rxStats.pkts65_127 = uswap32(respHdl.mib->rxStats.pkts65_127);
        mib->rxStats.pkts128_255 = uswap32(respHdl.mib->rxStats.pkts128_255);
        mib->rxStats.pkts256_511 = uswap32(respHdl.mib->rxStats.pkts256_511);
        mib->rxStats.pkts512_1023 = uswap32(respHdl.mib->rxStats.pkts512_1023);
        mib->rxStats.pkts1024_MAX = uswap32(respHdl.mib->rxStats.pkts1024_MAX);
        mib->rxStats.pkts1024_1522 = uswap32(respHdl.mib->rxStats.pkts1024_1522);
        mib->rxStats.pkts1523_2047 = uswap32(respHdl.mib->rxStats.pkts1523_2047);
        mib->rxStats.pkts2048_4095 = uswap32(respHdl.mib->rxStats.pkts2048_4095);
        mib->rxStats.pkts4096_8191 = uswap32(respHdl.mib->rxStats.pkts4096_8191);
        mib->rxStats.pkts8192_MAX = uswap32(respHdl.mib->rxStats.pkts8192_MAX);
        mib->rxStats.pktsJabber = uswap32(respHdl.mib->rxStats.pktsJabber);
        mib->rxStats.pktsOvrSz = uswap32(respHdl.mib->rxStats.pktsOvrSz);
        mib->rxStats.pktsFrag = uswap32(respHdl.mib->rxStats.pktsFrag);
        mib->rxStats.pktsRxDrop = uswap32(respHdl.mib->rxStats.pktsRxDrop);
        mib->rxStats.pktsCrcAlignErr = uswap32(respHdl.mib->rxStats.pktsCrcAlignErr);
        mib->rxStats.pktsUndSz = uswap32(respHdl.mib->rxStats.pktsUndSz);
        mib->rxStats.pktsCrcErr = uswap32(respHdl.mib->rxStats.pktsCrcErr);
        mib->rxStats.pktsRxDiscard = uswap32(respHdl.mib->rxStats.pktsRxDiscard);
        mib->rxStats.rxPause = uswap32(respHdl.mib->rxStats.rxPause);

        mib->txStats.octets = uswap32(respHdl.mib->txStats.octets);
        mib->txStats.brdCast = uswap32(respHdl.mib->txStats.brdCast);
        mib->txStats.mutCast = uswap32(respHdl.mib->txStats.mutCast);
        mib->txStats.uniCast = uswap32(respHdl.mib->txStats.uniCast);
        mib->txStats.txDropped = uswap32(respHdl.mib->txStats.txDropped);
        mib->txStats.txDroppedErr = uswap32(respHdl.mib->txStats.txDroppedErr);
        mib->txStats.txCollision = uswap32(respHdl.mib->txStats.txCollision);
        mib->txStats.txCollisionSingle = uswap32(respHdl.mib->txStats.txCollisionSingle);
        mib->txStats.txCollisionMulti = uswap32(respHdl.mib->txStats.txCollisionMulti);
        mib->txStats.txLateCollision = uswap32(respHdl.mib->txStats.txLateCollision);
        mib->txStats.txExcessiveCollision = uswap32(respHdl.mib->txStats.txExcessiveCollision);
        mib->txStats.txDeferredTransmit = uswap32(respHdl.mib->txStats.txDeferredTransmit);
        mib->txStats.txFrameInDiscard = uswap32(respHdl.mib->txStats.txFrameInDiscard);
        mib->txStats.txPause = uswap32(respHdl.mib->txStats.txPause);
        mib->txStats.txQ0 = uswap32(respHdl.mib->txStats.txQ0);
        mib->txStats.txQ1 = uswap32(respHdl.mib->txStats.txQ1);
        mib->txStats.txQ2 = uswap32(respHdl.mib->txStats.txQ2);
        mib->txStats.txQ3 = uswap32(respHdl.mib->txStats.txQ3);
        mib->txStats.txQ4 = uswap32(respHdl.mib->txStats.txQ4);
        mib->txStats.txQ5 = uswap32(respHdl.mib->txStats.txQ5);
        mib->txStats.txQ6 = uswap32(respHdl.mib->txStats.txQ6);
        mib->txStats.txQ7 = uswap32(respHdl.mib->txStats.txQ7);
        mib->txStats.pkts64 = uswap32(respHdl.mib->txStats.pkts64);
        mib->txStats.pkts65_127 = uswap32(respHdl.mib->txStats.pkts65_127);
        mib->txStats.pkts128_255 = uswap32(respHdl.mib->txStats.pkts128_255);
        mib->txStats.pkts256_511 = uswap32(respHdl.mib->txStats.pkts256_511);
        mib->txStats.pkts512_1023 = uswap32(respHdl.mib->txStats.pkts512_1023);
        mib->txStats.pkts1024_MAX = uswap32(respHdl.mib->txStats.pkts1024_MAX);
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtMibClear(MgmtInfoType *info, int32_t port)
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

    if (info == NULL) {
        HOST_Log("%s :: Invalid input param info:%p\n", __FUNCTION__, info);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.mib->port = uswap32(port);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_MIBS_CLEAR);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
                sizeof(ETHERSWT_MibType), (uint8_t *)&resp,
                RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_MibType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}
/*
 * @api
 * HOST_EtherSwtAgeTimeGet
 *
 * @brief
 * Get the bridge age time
 *
 * @param=info - management info
 * @param=switch age time in seconds
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtAgeTimeGet(MgmtInfoType *info, uint32_t *ageTime)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if ((info == NULL) || (ageTime == NULL)) {
        HOST_Log("%s :: Invalid input param(info =%p and ageTime = %p)\n",
                __FUNCTION__, info, ageTime);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (retVal == BCM_ERR_OK) {
        *ageTime = switchInfo.ageTime;
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtDumbFwdModeGet
 *
 * @brief
 * Get the dumbfwd mode
 *
 * @param=info - management info
 * @param=dumbFwdMode - to get the information
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtDumbFwdModeGet(MgmtInfoType *info, ETHERSWT_DumbFwdModeType *dumbFwdMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if ((info == NULL) || (dumbFwdMode == NULL)) {
        HOST_Log("%s :: Invalid input parameters(info=%p and dumbFwdMode=%p \n",
                __FUNCTION__, info, dumbFwdMode);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (retVal == BCM_ERR_OK) {
        *dumbFwdMode = switchInfo.dumbFwdMode;
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtMirrorEnable
 *
 * @brief
 * Enable mirroring
 *
 * @param=info - management info
 * @param=portMask - port bitmap
 * @param=probePort - probe port
 * @param=direction - traffic direction
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtMirrorEnable(MgmtInfoType *info, uint16_t portMask,
                                uint32_t probePort, ETHERSWT_TrafficDirType direction)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((direction != ETHERSWT_TRAFFICDIR_INGRESS)
            && (direction != ETHERSWT_TRAFFICDIR_EGRESS)) {
        HOST_Log("%s :: Invalid mirroring direction %d\n", __FUNCTION__, direction);
        goto done;
    }

    /* make sure the port bitmap is non-zero and
       don't allow the probe port in the port bitmap
     */
    if (((portMask == 0x0) && (FALSE == HIPC_IsStacked()))
            || ((portMask & (1 << probePort)) != 0x0)) {
        HOST_Log("Invalid port bitmap or conflicting probe port\n");
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.mirrorEnable->probePort = uswap32(probePort);
    cmdHdl.mirrorEnable->mirrorCfg.portMask = uswap16(portMask);
    cmdHdl.mirrorEnable->direction = direction;

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_MIRROR_ENABLE);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_MirrorEnableType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_MirrorEnableType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtMirrorDisable
 *
 * @brief
 * Disable mirroring
 *
 * @param=info - management info
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtMirrorDisable(MgmtInfoType *info)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_MIRROR_DISABLE);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            0UL, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

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

/*
 * @api
 * HOST_EtherSwtMirrorStatus
 *
 * @brief
 * Get the status of mirroring
 *
 * @param=info - management info
 * @param=mode -     enabled or disabled
 * @param=portMask- bitmap of ports to mirror
 * @param=probePort  -   probe port
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtMirrorStatus(MgmtInfoType *info, ETHERSWT_PortMirrorStateType *state,
        uint16_t *ingressPortMask, uint16_t *egressPortMask, uint32_t *probePort)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_MIRROR_STATUS);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_MirrorStatusType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_MirrorStatusType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else  {
        *state = uswap32(respHdl.mirrorStatus->state);
        *ingressPortMask = uswap16(respHdl.mirrorStatus->ingressMirrorCfg.portMask);
        *egressPortMask = uswap16(respHdl.mirrorStatus->egressMirrorCfg.portMask);
        *probePort = uswap32(respHdl.mirrorStatus->probePort);
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtAgeTimeSet
 *
 * @brief
 * Set the bridge age time
 *
 * @param=info - management info
 * @param=switch age time in seconds
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtAgeTimeSet(MgmtInfoType *info, uint32_t ageTime)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (BCM_ERR_OK == retVal) {
        switchInfo.ageTime = ageTime;
        retVal = HOST_EtherSwtSwitchInfoSet(info, &switchInfo);
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtSwitchInfoGet(MgmtInfoType *info, ETHERSWT_SwitchInfoType *switchInfo)
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

    if ((info == NULL) || (switchInfo == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and switchInfo = %p)\n",
                __FUNCTION__, info, switchInfo);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_SWITCH_INFO_GET);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_SwitchInfoType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_SwitchInfoType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        switchInfo->ageTime = uswap32(respHdl.switchInfo->ageTime);
        switchInfo->dumbFwdMode = uswap32(respHdl.switchInfo->dumbFwdMode);
        switchInfo->iFilter = uswap32(respHdl.switchInfo->iFilter);
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtSwitchInfoSet(MgmtInfoType *info, ETHERSWT_SwitchInfoType *switchInfo)
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

    if ((info == NULL) || (switchInfo == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and switchInfo = %p)\n",
                __FUNCTION__, info, switchInfo);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.switchInfo->ageTime = uswap32(switchInfo->ageTime);
    cmdHdl.switchInfo->dumbFwdMode = uswap32(switchInfo->dumbFwdMode);
    cmdHdl.switchInfo->iFilter = uswap32(switchInfo->iFilter);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_SWITCH_INFO_SET);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_SwitchInfoType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_SwitchInfoType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtPortInfoGet
 *
 * @brief
 * Get the port information
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=portInfo - structure to fill port information
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtPortInfoGet(MgmtInfoType *info, uint32_t port, ETHERSWT_PortInfoType *portInfo)
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

    if ((info == NULL) || (portInfo == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and portInfo = %p)\n",
                __FUNCTION__, info, portInfo);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.portInfo->port = uswap32(port);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_PORT_INFO_GET);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_PortInfoType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_PortInfoType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        portInfo->port = uswap32(respHdl.portInfo->port);
        portInfo->adminMode = respHdl.portInfo->adminMode;
        portInfo->linkStatus = respHdl.portInfo->linkStatus;
        portInfo->speed = respHdl.portInfo->speed;
        portInfo->masterEnable = respHdl.portInfo->masterEnable;
        portInfo->jumboEnable = respHdl.portInfo->jumboEnable;
        portInfo->loopbackEnable = respHdl.portInfo->loopbackEnable;
        portInfo->autonegEnable = respHdl.portInfo->autonegEnable;
        portInfo->autonegComplete = respHdl.portInfo->autonegComplete;
        portInfo->duplex= respHdl.portInfo->duplex;
        portInfo->led = uswap32 (respHdl.portInfo->led);
        portInfo->linkStateChangeCnt = uswap32 (respHdl.portInfo->linkStateChangeCnt);
        portInfo->busMode = respHdl.portInfo->busMode;
        portInfo->phyMedia = respHdl.portInfo->phyMedia;
        portInfo->linkSQI = uswap32(respHdl.portInfo->linkSQI);
        portInfo->pvid = uswap32(respHdl.portInfo->pvid);
        portInfo->prio = uswap32(respHdl.portInfo->prio);
    }

done:
    return retVal;
}


/*
 * @api
 * HOST_EtherSwtPortInfoSet
 *
 * @brief
 * Get the port information
 *
 * @param=info - management info
 * @param=portInfo - structure with port information
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtPortInfoSet(MgmtInfoType *info, ETHERSWT_PortInfoType *portInfo)
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

    if ((info == NULL) || (portInfo == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and portInfo = %p)\n",
                __FUNCTION__, info, portInfo);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.portInfo->port = uswap32(portInfo->port);
    cmdHdl.portInfo->adminMode = portInfo->adminMode;
    cmdHdl.portInfo->linkStatus = portInfo->linkStatus;
    cmdHdl.portInfo->speed = portInfo->speed;
    cmdHdl.portInfo->masterEnable = portInfo->masterEnable;
    cmdHdl.portInfo->jumboEnable = portInfo->jumboEnable;
    cmdHdl.portInfo->loopbackEnable = portInfo->loopbackEnable;
    cmdHdl.portInfo->autonegEnable = portInfo->autonegEnable;
    cmdHdl.portInfo->autonegComplete = portInfo->autonegComplete;
    cmdHdl.portInfo->duplex= portInfo->duplex;
    cmdHdl.portInfo->led = uswap32 (portInfo->led);
    cmdHdl.portInfo->linkStateChangeCnt = uswap32 (portInfo->linkStateChangeCnt);
    cmdHdl.portInfo->busMode = portInfo->busMode;
    cmdHdl.portInfo->phyMedia = portInfo->phyMedia;
    cmdHdl.portInfo->linkSQI = uswap32(portInfo->linkSQI);
    cmdHdl.portInfo->pvid = uswap32(portInfo->pvid);
    cmdHdl.portInfo->prio = uswap32(portInfo->prio);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_PORT_INFO_SET);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_PortInfoType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_PortInfoType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtRegRead(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                             uint32_t len, uint64_t *data)
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
    int i;

    if ((info == NULL) || (len > 16UL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and len = %u)\n",
                __FUNCTION__, info, len);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.regAccess->addr = uswap32(addr);
    cmdHdl.regAccess->len = uswap32(len);
    cmdHdl.regAccess->deviceID = uswap32(destn);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_READ);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_RegAccessType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_RegAccessType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        for(i = 0; i < len; i++) {
           data[i] = uswap64(*(uint64_t *)&respHdl.regAccess->data[i*8]);
        }
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtRegWrite(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                              uint32_t len, uint64_t *data)
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
    int i;

    if ((info == NULL) || (len > 16UL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p and len = %u)\n",
                __FUNCTION__, info, len);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.regAccess->addr = uswap32(addr);
    cmdHdl.regAccess->len = uswap32(len);
    cmdHdl.regAccess->deviceID = uswap32(destn);

    for(i = 0; i < len; i++) {
        *(uint64_t *)&cmdHdl.regAccess->data[i*8] = uswap64(data[i]);
    }

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_WRITE);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_RegAccessType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_RegAccessType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtVlanGet
 *
 * @brief
 * Get the VLAN info
 *
 * @param=info - management info
 * @param=vlan - vlan id
 * @param=portMask - VLAN port membership
 * @param=tagMask - tagged bitmap
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanGet(MgmtInfoType *info, uint16_t vlan, uint32_t *portMask,
        uint32_t *tagMask, uint32_t *staticPortMask)
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

    if ((info == NULL) || (tagMask == NULL) || (portMask == NULL)
            || (staticPortMask == NULL)) {
        HOST_Log("%s :: Invalid input param(info = %p, tagMask = %p, "
                "portMask = %p, staticPortMask = %p)\n",
                __FUNCTION__, info, tagMask, portMask, staticPortMask);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.vlanPort->vlan = uswap16(vlan);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_VLAN_GET);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETHERSWT_VLANPortType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_VLANPortType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *portMask = uswap32(respHdl.vlanPort->portMask);
        *tagMask = uswap32(respHdl.vlanPort->tagMask);
        *staticPortMask = uswap32(respHdl.vlanPort->staticPortMask);
    }
done:
    return retVal;
}


/*
 * @api
 * HOST_EtherSwtVlanPortAdd
 *
 * @brief
 * Add the given port to the given VLAN, and enable/disable tagging on it
 *
 * @param=info - management info
 * @param=vlan - vlan id
 * @param=portMask - port bitmask to add
 * @param=tagMask - tagged port bitMask
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanPortAdd(MgmtInfoType *info, uint16_t vlan, uint32_t portMask, uint32_t tagMask)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_VLAN_PORT_ADD);
    cmdHdl.vlanPort->vlan = uswap16(vlan);
    cmdHdl.vlanPort->portMask = uswap32(portMask);
    cmdHdl.vlanPort->tagMask = uswap32(tagMask);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_VLANPortType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_VLANPortType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtVlanPortDel
 *
 * @brief
 * Delete the given port from the given VLAN
 *
 * @param=info - management info
 * @param=vlan - vlan id to create
 * @param=portMask - portMask to remove
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanPortDel(MgmtInfoType *info, uint16_t vlan, uint32_t portMask)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_VLAN_PORT_REMOVE);
    cmdHdl.vlanPort->vlan = uswap16(vlan);
    cmdHdl.vlanPort->portMask = uswap32(portMask);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_VLANPortType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_VLANPortType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtVlanPvidSet
 *
 * @brief
 * Configure the untagged VLAN for the given port.
 *
 * @param=info - management info
 * @param=port - port number
 * @param=pvid - pvid
 * @param=prio - priority
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanPvidSet(MgmtInfoType *info, uint32_t port, uint32_t pvid, uint32_t prio)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        portInfo.pvid = pvid;
        portInfo.prio = prio;
        retVal = HOST_EtherSwtPortInfoSet(info, &portInfo);
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtVlanIfilterSet
 *
 * @brief
 * Enable/Disbale VLAN ingress filtering mode for all ports
 *
 * @param=info - management info
 * @param=iFilter - iFilter mode
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanIfilterSet(MgmtInfoType *info, ETHERSWT_VLANIngressFilterModeType iFilter)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    if ((iFilter != ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED) &&
            (iFilter != ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED)) {
        HOST_Log("Invalid mode for ingress filtering %d\n", iFilter);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (BCM_ERR_OK == retVal) {
        switchInfo.iFilter = iFilter;
        retVal = HOST_EtherSwtSwitchInfoSet(info, &switchInfo);
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtVlanIfilterGet
 *
 * @brief
 * Get VLAN ingress filtering mode for all ports
 *
 * @param=info - management info
 * @param=iFilter - iFilter mode
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtVlanIfilterGet(MgmtInfoType *info, ETHERSWT_VLANIngressFilterModeType *iFilter)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if ((info == NULL) || (iFilter == NULL)){
        HOST_Log("%s: Invalid parameters info:%p iFilter:%p\n", __FUNCTION__, info, iFilter);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (retVal == BCM_ERR_OK) {
        *iFilter = switchInfo.iFilter;
    }

done:
    return retVal;
}

/*@api
 * HOST_EtherSwtConvertMac
 *
 * @brief
 * Print the contents of a buffer to stdout. For debugging.
 *
 * @param=mac_addr - pointer to mac address string
 * @param=data - pointer to the mac address hex
 * @param=status - status of the operation
 *
 * @returns void
 *
 * @desc
 */
void HOST_EtherSwtConvertMac(char *mac_addr, unsigned char *mac_hex, int32_t *status)
{
    uint8_t i;
    uint32_t octet;
    char *ptr = mac_addr;

    *status = BCM_ERR_INVAL_PARAMS;

    if (ptr == NULL) {
        HOST_Log("%s :: mac_addr ptr is null\n", __FUNCTION__);
        return ;
    }

    if (mac_hex == NULL) {
        HOST_Log("%s :: mac_hex ptr is null\n", __FUNCTION__);
        return ;
    }

    if (strlen(mac_addr) != 17) {
        HOST_Log("Macaddr length mismatch, should be of the format xx:xx:xx:xx:xx:xx\n");
        return;
    }

    for (i = 0; i < 17; i++) {
        if ((i % 3) == 2) {
            if (mac_addr[i] != ':') {
                HOST_Log("Invalid characters in macaddr input\n");
                return;
            }
        } else {
            if ((mac_addr[i] < '0')
                    || ((mac_addr[i] > '9') && (mac_addr[i] < 'A'))
                    || ((mac_addr[i] > 'F') && (mac_addr[i] < 'a'))
                    || (mac_addr[i] > 'f')) {
                HOST_Log("Invalid characters in macaddr input\n");
                return;
            }
        }
    }

    for (i = 0; i < MGMT_ETHER_ADDR_LEN; ++i) {
        if (!i) {
            octet = strtol(mac_addr, &ptr, 16);
        } else {
            octet = strtol(ptr+1, &ptr, 16);
        }

        if (i == (MGMT_ETHER_ADDR_LEN - 1)) {
            if ((*ptr != '\0') || (octet > 255)) {
                HOST_Log("%s :: Invalid mac address format\n", __FUNCTION__);
                return;
            }
        } else {
            if ((*ptr != ':') || (octet > 255)) {
                HOST_Log("%s :: Invalid macaddress\n", __FUNCTION__);
                return;
            }
        }

        mac_hex[i] = octet;
    }

    if ((mac_hex[0] == 0) && (mac_hex[1] == 0) && (mac_hex[2] == 0) &&
            (mac_hex[3] == 0) && (mac_hex[4] == 0) && (mac_hex[5] == 0)) {
        HOST_Log("\nInvalid MAC address\n");
        return;   /* Invalid mac address */
    }

    if ((mac_hex[0] == 0xff) && (mac_hex[1] == 0xff) && (mac_hex[2] == 0xff) &&
            (mac_hex[3] == 0xff) && (mac_hex[4] == 0xff) && (mac_hex[5] == 0xff)) {
        HOST_Log("\nBroadcast MAC address\n");
        return;   /* Broad cast mac address */
    }

    *status = BCM_ERR_OK;
}

int32_t HOST_EtherSwtARLAdd(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan, uint32_t portMask)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_ARL_ADD);
    cmdHdl.arlEntry->vlanID = uswap16(vlan);
    cmdHdl.arlEntry->portMask = uswap32(portMask);
    memcpy(&cmdHdl.arlEntry->macAddr[0], mac_addr, 6UL);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_ARLEntryType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_ARLEntryType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtARLDelete(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan)
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

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_ARL_DELETE);
    cmdHdl.arlEntry->vlanID = uswap16(vlan);
    memcpy(&cmdHdl.arlEntry->macAddr[0], mac_addr, 6UL);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_ARLEntryType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_ARLEntryType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtARLSnapshot(MgmtInfoType *info, uint32_t *count)
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

    if (info == NULL){
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_ARL_SNAPSHOT);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_ARLListType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_ARLListType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *count = uswap32(respHdl.arlList->num);
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtARLGetInt(MgmtInfoType *info, ETHERSWT_ARLEntryType *entry,
                                uint32_t index, uint32_t *count)
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
    uint32_t idx;

    if ((info == NULL) || (entry == NULL) || (NULL == count) || (0UL == *count) || (*count > ETHERSWT_ARL_ENTRIES_MAX)) {
        HOST_Log("%s :: Invalid parameters info:%p entry:%p count:%u\n", __FUNCTION__, info, entry, *count);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_ARL_GET);
    cmdHdl.arlList->num = uswap32(index);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_ARLListType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_ARLListType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *count = uswap32(respHdl.arlList->num);
        for (idx = 0; idx < *count; idx++) {
            memcpy(&entry[idx].macAddr[0], &respHdl.arlList->entries[idx].macAddr[0], 6UL);
            entry[idx].vlanID = uswap16(respHdl.arlList->entries[idx].vlanID);
            entry[idx].portMask = uswap32(respHdl.arlList->entries[idx].portMask);
            entry[idx].reserved = uswap16(respHdl.arlList->entries[idx].reserved);
        }
    }

done:
    return retVal;
}

/* count is an inout parameter. Indicates maximum entries that the caller can
 * accept and the maximum entries that the target can provide */
int32_t HOST_EtherSwtARLGet(MgmtInfoType *info, ETHERSWT_ARLEntryType *entry, uint32_t *count)
{
    uint32_t idx = 0UL;
    uint32_t thisCount;
    uint32_t available = 0UL;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((info == NULL) || (entry == NULL) || (count == NULL) || (0UL == *count)) {
        HOST_Log("%s :: Invalid parameters info:%p entry:%p count:%p\n", __FUNCTION__, info, entry, count);
        goto done;
    }

    retVal = HOST_EtherSwtARLSnapshot(info, &available);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Failed to buffer ARL entries\n");
        goto done;
    }
    if (*count < available) {
        available = *count;
    }

    memset((uint8_t *)entry, 0UL, sizeof(ETHERSWT_ARLEntryType) * (*count));

    while (idx < available) {
        if ((available - idx) >= ETHERSWT_ARL_ENTRIES_MAX) {
            thisCount = ETHERSWT_ARL_ENTRIES_MAX;
        } else {
            thisCount = available - idx;
        }
        retVal = HOST_EtherSwtARLGetInt(info, &entry[idx], idx, &thisCount);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        idx += thisCount;
    }

    *count = available;

done:
    return retVal;
}

/*
 * @api
 * mgmt_admin_mode_get
 *
 * @brief
 * Get admin mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=admin_mode - information to get
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrAdminModeGet(MgmtInfoType *info, uint32_t port, ETHXCVR_ModeType *adminMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    if (adminMode == NULL) {
        HOST_Log("Invalid adminMode pointer\n");
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *adminMode = portInfo.adminMode;
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrSpeedGet
 *
 * @brief
 * Get the speed
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=speed - information to get
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrSpeedGet(MgmtInfoType *info, uint32_t port, ETHXCVR_SpeedType *speed)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    if (speed == NULL) {
        HOST_Log("Invalid speed pointer\n");
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *speed = portInfo.speed;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrMasterSlaveGet
 *
 * @brief
 * Get the BR master/slave mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=master_slave - information to get
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrMasterSlaveGet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType *masterSlave)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    if (masterSlave == NULL) {
        HOST_Log("Invalid masterSlave pointer\n");
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *masterSlave = portInfo.masterEnable;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrPhyLbGet
 *
 * @brief
 * Get the phy_lb mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=phy_lb - information to get
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrPhyLbGet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType *loopbackEnable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    if (loopbackEnable == NULL) {
        HOST_Log("Invalid loopbackEnable pointer\n");
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *loopbackEnable = portInfo.loopbackEnable;
    }
done:
    return retVal;
}


/*
 * @api
 * HOST_EtherSwtXcvrLinkSQIGet
 *
 * @brief
 * Get the link signal quality index
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=sqi_reg_val - pointer to fill the sqi value
 *
 * @returns 0 on success, !0 on error
 */

int32_t HOST_EtherSwtXcvrLinkSQIGet(MgmtInfoType *info, uint32_t port, uint32_t *sqi)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if ((info == NULL) || (sqi == NULL)) {
        HOST_Log("%s :: Atleast one of the input pointer argument is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *sqi = portInfo.linkSQI;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrJumboFrameModeGet
 *
 * @brief
 * Get the port jumbo frame mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=jumboe frame mode  0=disable, 1=enable
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrJumboFrameModeGet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType* jumboEnable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if ((info == NULL) || (jumboEnable == NULL)) {
        HOST_Log("%s :: Atleast one of the input pointer argument is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *jumboEnable = portInfo.jumboEnable;
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrTypeGet
 *
 * @brief
 * get port type
 *
 * @param=info - pointer to the MgmtInfoType
 * @port = port number
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrTypeGet(MgmtInfoType *info, uint32_t port, ETHXCVR_BusModeType *busMode, ETHXCVR_PhyMediaType *phyMedia)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if ((info == NULL) || (busMode == NULL) || (phyMedia == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p,busMode = %p, phyMedia = %p)\n", __FUNCTION__, info, busMode, phyMedia);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        *busMode  = portInfo.busMode;
        *phyMedia = portInfo.phyMedia;
    }
done:
    return retVal;
}



/*
 * @api
 * HOST_EtherSwtDumbFwdModeSet
 *
 * @brief
 * Set the dumbfwd mode
 *
 * @param=info - management info
 * @param=dumbfwd_mode - to set dumbfwd mode
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtDumbFwdModeSet(MgmtInfoType *info, ETHERSWT_DumbFwdModeType dumbFwdMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_SwitchInfoType switchInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((dumbFwdMode != ETHERSWT_DUMBFWD_ENABLE) && (dumbFwdMode != ETHERSWT_DUMBFWD_DISABLE)) {
        HOST_Log("%s :: Invalid value for dumbFwdMode\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_EtherSwtSwitchInfoGet(info, &switchInfo);
    if (BCM_ERR_OK == retVal) {
        switchInfo.dumbFwdMode = dumbFwdMode;
        retVal = HOST_EtherSwtSwitchInfoSet(info, &switchInfo);
    }

done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrAdminModeSet
 *
 * @brief
 * Set the port admin mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=admin_mode - 1=disable, 2=enable
 *
 * @returns 0 on success, !0 on error
 */

int32_t HOST_EtherSwtXcvrAdminModeSet(MgmtInfoType *info, uint32_t port,
                                    ETHXCVR_ModeType adminMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((adminMode != ETHXCVR_MODE_ACTIVE) && (adminMode != ETHXCVR_MODE_DOWN)) {
        HOST_Log("%s :: Invalid setting for port admin mode %u\n", __FUNCTION__,
                adminMode);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        portInfo.adminMode = adminMode;
        retVal = HOST_EtherSwtPortInfoSet(info, &portInfo);
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrMasterSlaveSet
 *
 * @brief
 * Set the BR master/slave mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=brMode - 0=slave, 1=master
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrMasterSlaveSet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType masterEnable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((masterEnable != ETHXCVR_BOOLEAN_TRUE) && (masterEnable != ETHXCVR_BOOLEAN_FALSE)) {
        HOST_Log("%s :: Invalid brMode configuration %u\n", __FUNCTION__, masterEnable);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        portInfo.masterEnable = masterEnable;
        retVal = HOST_EtherSwtPortInfoSet(info, &portInfo);
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrPhyLbSet
 *
 * @brief
 * Set the port phy_lb mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=loopBackMode - ETHXCVR_BOOLEAN_FALSE=disable, ETHXCVR_BOOLEAN_TRUE=enable
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrPhyLbSet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType loopbackEnable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((loopbackEnable != ETHXCVR_BOOLEAN_TRUE) && (loopbackEnable != ETHXCVR_BOOLEAN_FALSE)) {
        HOST_Log("%s :: Invalid loopbackEnable configuration %u\n", __FUNCTION__, loopbackEnable);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        portInfo.loopbackEnable = loopbackEnable;
        retVal = HOST_EtherSwtPortInfoSet(info, &portInfo);
    }
done:
    return retVal;
}

/*
 * @api
 * HOST_EtherSwtXcvrJumboFrameModeSet
 *
 * @brief
 * Set the port jumbo frame mode
 *
 * @param=info - management info
 * @param=port - port to get the information
 * @param=jumbo enable  0=disable, 1=enable
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_EtherSwtXcvrJumboFrameModeSet(MgmtInfoType *info, uint32_t port, ETHXCVR_BooleanType jumboEnable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PortInfoType portInfo;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if ((jumboEnable != ETHXCVR_BOOLEAN_TRUE) && (jumboEnable != ETHXCVR_BOOLEAN_FALSE)) {
        HOST_Log("%s :: Invalid jumboFrameMode configuration %u \n", __FUNCTION__, jumboEnable);
        goto done;
    }

    retVal = HOST_EtherSwtPortInfoGet(info, port, &portInfo);
    if (retVal == BCM_ERR_OK) {
        portInfo.jumboEnable = jumboEnable;
        retVal = HOST_EtherSwtPortInfoSet(info, &portInfo);
    }
done:
    return retVal;
}


int32_t HOST_EtherSwtNotificationHandler(uint32_t currentSlave,
            EtherSwt_EventType notificationId, uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETHERSWT_PayloadType notificationHdl;

    if (NULL == msg) {
        HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x msg:%p size=%d\n",
            __func__, currentSlave, notificationId, msg, size);
        goto done;
    }

    notificationHdl.u8Ptr = msg;

    switch (notificationId) {
    case ETHERSWT_EVENT_PORT_LINK_INFO:
        if (size == sizeof(ETHERSWT_PortLinkInfoType)) {
            HOST_Log("%s SPI-Id:%u Interface:%u Status:%s ChangeCount:%u\n",
                    __func__, currentSlave,
                    uswap32(notificationHdl.portLinkInfo->port),
                    (uswap32(notificationHdl.portLinkInfo->linkState) ==
                    ETHXCVR_LINKSTATE_ACTIVE) ? "Active" : "Down",
                    uswap32(notificationHdl.portLinkInfo->portStats.linkStateChangeCount));
            retVal = BCM_ERR_OK;
        } else {

            HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x size=%d\n",
                __func__, currentSlave, notificationId, size);
        }
        break;
    case ETHERSWT_EVENT_STREAM_EXCEEDED:
        if (size == sizeof(COMMS_StreamPolicerStatusType)) {
            HOST_Log("%s SPI-Id:%u Stream: %u exceeded, blocked %u\n",
                    __func__, currentSlave,
                    uswap32(notificationHdl.streamPolicerStatus->idx),
                    uswap32(notificationHdl.streamPolicerStatus->blocked));
            retVal = BCM_ERR_OK;
        } else {

            HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x size=%d\n",
                __func__, currentSlave, notificationId, size);
        }
        break;
    default:
        HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x size=%d\n",
            __func__, currentSlave, notificationId, size);
        break;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtStreamPolicerAdd(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan,
                                      uint32_t rate, uint32_t burst, uint32_t srcMask,
                                      uint32_t threshold, uint32_t interval, uint32_t report,
                                      uint32_t block, uint32_t *const streamIdx)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_STREAMPOLICER_ADD);
    cmdHdl.streamPolicerEntry->vlanID    = uswap16(vlan);
    memcpy(&cmdHdl.streamPolicerEntry->macAddr[0], mac_addr, 6UL);
    cmdHdl.streamPolicerEntry->policerParams.portMask           = uswap32(srcMask);
    cmdHdl.streamPolicerEntry->policerParams.rate               = uswap32(rate);
    cmdHdl.streamPolicerEntry->policerParams.burstSize          = uswap32(burst);
    cmdHdl.streamPolicerEntry->policerParams.dropThreshold      = uswap32(threshold);
    cmdHdl.streamPolicerEntry->policerParams.monitoringInterval = uswap32(interval);
    cmdHdl.streamPolicerEntry->policerParams.action             = (block & COMMS_STREAMPOLICERACTION_BLOCK_MASK) |
                                                                  ((report << COMMS_STREAMPOLICERACTION_REPORT_SHIFT) &
                                                                  COMMS_STREAMPOLICERACTION_REPORT_MASK);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_StreamPolicerType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_StreamPolicerType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

    *streamIdx = uswap32(respHdl.streamPolicerEntry->streamIdx);
done:
    return retVal;
}

int32_t HOST_EtherSwtStreamPolicerDel(MgmtInfoType *info, uint32_t streamIdx)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_STREAMPOLICER_DEL);
    cmdHdl.streamPolicerEntry->streamIdx = uswap32(streamIdx);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_StreamPolicerType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_StreamPolicerType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtBlockStream(MgmtInfoType *info, uint32_t streamIdx)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_BLOCKSTREAM);
    cmdHdl.streamPolicerEntry->streamIdx = uswap32(streamIdx);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_StreamPolicerType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_StreamPolicerType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_EtherSwtResumeStream(MgmtInfoType *info, uint32_t streamIdx)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_RESUMESTREAM);
    cmdHdl.streamPolicerEntry->streamIdx = uswap32(streamIdx);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_StreamPolicerType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_StreamPolicerType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

int32_t HOST_EtherSwtStreamPolicerFindIdx(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan,
                                          uint32_t srcMask, uint32_t *const streamIdx)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_STREAMPOLICER_FIND);
    memcpy(&cmdHdl.streamPolicerEntry->macAddr[0], mac_addr, 6UL);
    cmdHdl.streamPolicerEntry->vlanID                 = uswap16(vlan);
    cmdHdl.streamPolicerEntry->policerParams.portMask = uswap32(srcMask);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_StreamPolicerType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_StreamPolicerType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

    *streamIdx = uswap32(respHdl.streamPolicerEntry->streamIdx);
done:
    return retVal;
}

int32_t HOST_EtherSwtStreamPolicerGetStatus(MgmtInfoType *info, COMMS_StreamPolicerStatusType *const aStatus)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_STREAMPOLICER_GET);
    cmdHdl.streamPolicerStatus->idx = uswap32(aStatus->idx);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(COMMS_StreamPolicerStatusType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(COMMS_StreamPolicerStatusType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

    memcpy(aStatus->macAddress, respHdl.streamPolicerStatus->macAddress, 6U);
    aStatus->idx       = uswap32(respHdl.streamPolicerStatus->idx);
    aStatus->vlan      = uswap16(respHdl.streamPolicerStatus->vlan);
    aStatus->portMask  = uswap32(respHdl.streamPolicerStatus->portMask);
    aStatus->blocked   = uswap32(respHdl.streamPolicerStatus->blocked);
    aStatus->isStatic  = uswap32(respHdl.streamPolicerStatus->isStatic);
    aStatus->greenCntr = uswap32(respHdl.streamPolicerStatus->greenCntr);
    aStatus->redCntr   = uswap32(respHdl.streamPolicerStatus->redCntr);
done:
    return retVal;
}

int32_t HOST_EtherSwtStreamPolicerSnapshot(MgmtInfoType *info, COMMS_StreamPolicerSnapshotType *const aSnapshot)
{
    int32_t              retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    RPC_ResponseType     resp;
    ETHERSWT_PayloadType respHdl;
    uint32_t             replyLen;
    uint32_t             cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];
    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_STREAMPOLICER_SNAPSHOT);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(COMMS_StreamPolicerSnapshotType),
             (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(COMMS_StreamPolicerSnapshotType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

    memcpy(aSnapshot, respHdl.streamPolicerSnapshot, sizeof(COMMS_StreamPolicerSnapshotType));
done:
    return retVal;
}
