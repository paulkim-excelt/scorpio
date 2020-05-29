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
#include <ets_osil.h>
#include <rpc_cmds.h>
#include <host_ets.h>
#include <hlog.h>

#define HOST_ETS_ASYNC_PREFIX  "ETS Notification:"

extern uint32_t CurrentSpiId;

#ifdef ENABLE_RECORD_NOTIFICATION
static ETS_RecordType portRecords[ETS_MAX_INTERFACES][ETS_NUM_RECORDS_PER_INTF];
static ETS_ConfigType config;
#endif

uint32_t HOST_ETSPortToIndex(uint32_t port, ETS_IntfConfigType *intfCfg)
{
    uint32_t idx;

    for (idx = 0; idx < ETS_MAX_INTERFACES; idx++) {
        if (intfCfg->hwPortNum == port) {
            break;
        }
        intfCfg++;
    }

    return idx;
}

int32_t HOST_ETSConfigGet(MgmtInfoType *info, ETS_ConfigType *config)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];
    int i;

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if (config == NULL) {
        HOST_Log("%s :: ETS_ConfigType pointer is null\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_GETCONFIG);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETS_ConfigType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_ConfigType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        config->magicId = uswap32(respHdl.config->magicId);
        config->clockMode = respHdl.config->clockMode;
        config->avnuSyncAbsenceTimeout = respHdl.config->avnuSyncAbsenceTimeout;
        config->adminMode = respHdl.config->adminMode;
        config->boundaryModeEnable = respHdl.config->boundaryModeEnable;
        config->gmRateRatio= uswap32(respHdl.config->gmRateRatio);
        config->numInterfaces = uswap32(respHdl.config->numInterfaces);
        for (i = 0; i < ETS_MAX_INTERFACES; i++) {
            config->intfCfg[i].hwPortNum = respHdl.config->intfCfg[i].hwPortNum;
            config->intfCfg[i].role = respHdl.config->intfCfg[i].role;
            config->intfCfg[i].asCapable = respHdl.config->intfCfg[i].asCapable;
            config->intfCfg[i].nbrPropDelay = uswap32(respHdl.config->intfCfg[i].nbrPropDelay);
            config->intfCfg[i].initLogPdelayInterval = (int8_t)uswap32(respHdl.config->intfCfg[i].initLogPdelayInterval);
            config->intfCfg[i].initLogSyncInterval = (int8_t)uswap32(respHdl.config->intfCfg[i].initLogSyncInterval);
            config->intfCfg[i].operLogPdelayInterval = (int8_t)uswap32(respHdl.config->intfCfg[i].operLogPdelayInterval);
            config->intfCfg[i].operLogSyncInterval = (int8_t)uswap32(respHdl.config->intfCfg[i].operLogSyncInterval);
            config->intfCfg[i].syncReceiptTimeout = respHdl.config->intfCfg[i].syncReceiptTimeout;
            config->intfCfg[i].allowedLostResponses = uswap16(respHdl.config->intfCfg[i].allowedLostResponses);
            config->intfCfg[i].nbrRateRatio = uswap32(respHdl.config->intfCfg[i].nbrRateRatio);
        }
    }

done:
    return retVal;
}

int32_t HOST_ETSConfigSet(MgmtInfoType *info, ETS_ConfigType *config)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];
    int i;

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if (config == NULL) {
        HOST_Log("%s :: ETS_ConfigType pointer is null\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_SETCONFIG);
    cmdHdl.config->magicId = uswap32(config->magicId);
    cmdHdl.config->clockMode = config->clockMode;
    cmdHdl.config->avnuSyncAbsenceTimeout = config->avnuSyncAbsenceTimeout;
    cmdHdl.config->adminMode = config->adminMode;
    cmdHdl.config->boundaryModeEnable = config->boundaryModeEnable;
    cmdHdl.config->gmRateRatio = uswap32(config->gmRateRatio);
    cmdHdl.config->numInterfaces = uswap32(config->numInterfaces);

    for (i = 0; i < ETS_MAX_INTERFACES; i++) {
        cmdHdl.config->intfCfg[i].hwPortNum = config->intfCfg[i].hwPortNum;
        cmdHdl.config->intfCfg[i].role = config->intfCfg[i].role;
        cmdHdl.config->intfCfg[i].asCapable = config->intfCfg[i].asCapable;
        cmdHdl.config->intfCfg[i].nbrPropDelay = uswap32(config->intfCfg[i].nbrPropDelay);
        cmdHdl.config->intfCfg[i].initLogPdelayInterval = uswap32(config->intfCfg[i].initLogPdelayInterval);
        cmdHdl.config->intfCfg[i].initLogSyncInterval = uswap32(config->intfCfg[i].initLogSyncInterval);
        cmdHdl.config->intfCfg[i].operLogPdelayInterval = uswap32(config->intfCfg[i].operLogPdelayInterval);
        cmdHdl.config->intfCfg[i].operLogSyncInterval = uswap32(config->intfCfg[i].operLogSyncInterval);
        cmdHdl.config->intfCfg[i].syncReceiptTimeout = config->intfCfg[i].syncReceiptTimeout;
        cmdHdl.config->intfCfg[i].allowedLostResponses = uswap16(config->intfCfg[i].allowedLostResponses);
        cmdHdl.config->intfCfg[i].nbrRateRatio = uswap32(config->intfCfg[i].nbrRateRatio);
    }

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETS_ConfigType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_ConfigType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}


int32_t HOST_ETSGlobalStatus(MgmtInfoType *info, ETS_GlobalStatusType *globalStatus)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (globalStatus == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info = %p, globalStatus = %p)\n",
            __FUNCTION__, info, globalStatus);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_GETGLOBALSTATUS);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETS_GlobalStatusType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_GlobalStatusType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        globalStatus->magicId = uswap32(respHdl.globalStatus->magicId);
        globalStatus->gmStatus = uswap32(respHdl.globalStatus->gmStatus);
        globalStatus->rateRatioIn = uswap32(respHdl.globalStatus->rateRatioIn);
        globalStatus->gmRateRatio = uswap32(respHdl.globalStatus->gmRateRatio);
        globalStatus->slavePort = (int8_t)uswap32(respHdl.globalStatus->slavePort);
        globalStatus->signalingTxSeqId = uswap16(respHdl.globalStatus->signalingTxSeqId);
        globalStatus->reqSyncLogInterval = uswap32(respHdl.globalStatus->reqSyncLogInterval);
        globalStatus->isSignalingTimerStarted = uswap32(respHdl.globalStatus->isSignalingTimerStarted);;
        globalStatus->isSyncReceived = uswap32(respHdl.globalStatus->isSyncReceived);
        globalStatus->clockState = uswap32(respHdl.globalStatus->clockState);
        memcpy(&globalStatus->clockId.id[0], &respHdl.globalStatus->clockId.id[0], sizeof(ETS_ClockIdentityType));
        globalStatus->networkTime.seconds = uswap64(respHdl.globalStatus->networkTime.seconds);
        globalStatus->networkTime.nanoseconds = uswap32(respHdl.globalStatus->networkTime.nanoseconds);
        globalStatus->networkTimeState = uswap32(respHdl.globalStatus->networkTimeState);
    }

done:
    return retVal;
}

int32_t HOST_ETSPortStatus(MgmtInfoType *info, uint32_t port, ETS_PortStatsAndStatusType *portStatus)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (portStatus == NULL)) {
        HOST_Log("%s :: Invalid input parameter is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.portStatus->num = uswap32(port);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_GETINTFSTATUS);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETS_PortStatsAndStatusType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_PortStatsAndStatusType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        portStatus->num = uswap32(respHdl.portStatus->num);
        portStatus->status.magicId = uswap32(respHdl.portStatus->status.magicId);
        portStatus->status.nbrPropDelay = uswap32(respHdl.portStatus->status.nbrPropDelay);      /* Current peer delay */
        portStatus->status.nbrRateRatio = uswap32(respHdl.portStatus->status.nbrRateRatio);  /* Current neighbor rate ratio */
        portStatus->status.isMeasuringPdelay = uswap32(respHdl.portStatus->status.isMeasuringPdelay); /* Is port measuring PDELAY? */
        portStatus->status.isAVnuPdelayConfigSaved = uswap32(respHdl.portStatus->status.isAVnuPdelayConfigSaved);
        portStatus->status.currentLogSyncInterval = uswap32(respHdl.portStatus->status.currentLogSyncInterval);   /* Current SYNC TX interval */
        portStatus->status.currentLogPdelayInterval = uswap32(respHdl.portStatus->status.currentLogPdelayInterval);   /* Current PDELAY TX interval */
        portStatus->status.syncReceiptTimeoutInterval = uswap64(respHdl.portStatus->status.syncReceiptTimeoutInterval);  /* Current SYNC RX timeout */
        memcpy(&portStatus->status.partnerClockId.id[0],
                &respHdl.portStatus->status.partnerClockId.id[0],
                sizeof(portStatus->status.partnerClockId));
        portStatus->status.partnerPortId = uswap16(respHdl.portStatus->status.partnerPortId);
        portStatus->status.pDelayTxSeqId = uswap16(respHdl.portStatus->status.pDelayTxSeqId);
        portStatus->status.syncTxSeqId = uswap16(respHdl.portStatus->status.syncTxSeqId);
        portStatus->status.syncInfoAvailable = uswap32(respHdl.portStatus->status.syncInfoAvailable);
        portStatus->status.syncLastRxSeqId = uswap16(respHdl.portStatus->status.syncLastRxSeqId);
        portStatus->status.rxFollowupCorrection = uswap64(respHdl.portStatus->status.rxFollowupCorrection);
        portStatus->status.txFollowupCorrection = uswap64(respHdl.portStatus->status.txFollowupCorrection);
        portStatus->status.rxPduInterval = respHdl.portStatus->status.rxPduInterval;
        portStatus->status.rxPOT.seconds = uswap64(respHdl.portStatus->status.rxPOT.seconds);
        portStatus->status.rxPOT.nanoseconds = uswap32(respHdl.portStatus->status.rxPOT.nanoseconds);
        portStatus->status.numPdelayRespLost = uswap32(respHdl.portStatus->status.numPdelayRespLost);
        portStatus->status.pDelayReqState = uswap32(respHdl.portStatus->status.pDelayReqState);
        portStatus->status.pDelayRespState = uswap32(respHdl.portStatus->status.pDelayRespState);
        portStatus->status.syncTxState = uswap32(respHdl.portStatus->status.syncTxState);
        portStatus->status.syncRxState = uswap32(respHdl.portStatus->status.syncRxState);
        portStatus->status.syncTxTimestamp.seconds = uswap64(respHdl.portStatus->status.syncTxTimestamp.seconds);
        portStatus->status.syncTxTimestamp.nanoseconds = uswap32(respHdl.portStatus->status.syncTxTimestamp.nanoseconds);
        portStatus->status.syncRxTimestamp.seconds = uswap64(respHdl.portStatus->status.syncRxTimestamp.seconds);
        portStatus->status.syncRxTimestamp.nanoseconds = uswap32(respHdl.portStatus->status.syncRxTimestamp.nanoseconds);
        portStatus->status.pDelayT1.seconds = uswap64(respHdl.portStatus->status.pDelayT1.seconds);
        portStatus->status.pDelayT1.nanoseconds = uswap32(respHdl.portStatus->status.pDelayT1.nanoseconds);
        portStatus->status.pDelayT2.seconds = uswap64(respHdl.portStatus->status.pDelayT2.seconds);
        portStatus->status.pDelayT2.nanoseconds = uswap32(respHdl.portStatus->status.pDelayT2.nanoseconds);
        portStatus->status.pDelayT3.seconds = uswap64(respHdl.portStatus->status.pDelayT3.seconds);
        portStatus->status.pDelayT3.nanoseconds = uswap32(respHdl.portStatus->status.pDelayT3.nanoseconds);
        portStatus->status.pDelayT4.seconds = uswap64(respHdl.portStatus->status.pDelayT4.seconds);
        portStatus->status.pDelayT4.nanoseconds = uswap32(respHdl.portStatus->status.pDelayT4.nanoseconds);

        portStatus->stats.magicId = uswap32(respHdl.portStatus->stats.magicId);
        portStatus->stats.syncTxCount = uswap32(respHdl.portStatus->stats.syncTxCount);
        portStatus->stats.syncRxCount = uswap32(respHdl.portStatus->stats.syncRxCount);
        portStatus->stats.syncTransmitTimeouts = uswap32(respHdl.portStatus->stats.syncTransmitTimeouts);
        portStatus->stats.syncRxDiscards = uswap32(respHdl.portStatus->stats.syncRxDiscards);
        portStatus->stats.followUpTxCount = uswap32(respHdl.portStatus->stats.followUpTxCount);
        portStatus->stats.followUpRxCount = uswap32(respHdl.portStatus->stats.followUpRxCount);
        portStatus->stats.followUpRxDiscards = uswap32(respHdl.portStatus->stats.followUpRxDiscards);
        portStatus->stats.signalingTxCount = uswap32(respHdl.portStatus->stats.signalingTxCount);
        portStatus->stats.signalingRxCount = uswap32(respHdl.portStatus->stats.signalingRxCount);
        portStatus->stats.signalingRxDiscards = uswap32(respHdl.portStatus->stats.signalingRxDiscards);
        portStatus->stats.pDelayReqTxCount = uswap32(respHdl.portStatus->stats.pDelayReqTxCount);
        portStatus->stats.pDelayReqRxCount = uswap32(respHdl.portStatus->stats.pDelayReqRxCount);
        portStatus->stats.pDelayReqRxDiscards = uswap32(respHdl.portStatus->stats.pDelayReqRxDiscards);
        portStatus->stats.pDelayRespTxCount = uswap32(respHdl.portStatus->stats.pDelayRespTxCount);
        portStatus->stats.pDelayRespRxCount = uswap32(respHdl.portStatus->stats.pDelayRespRxCount);
        portStatus->stats.pDelayRespRxDiscards = uswap32(respHdl.portStatus->stats.pDelayRespRxDiscards);
        portStatus->stats.pDelayRespFollowUpTxCount = uswap32(respHdl.portStatus->stats.pDelayRespFollowUpTxCount);
        portStatus->stats.pDelayRespFollowUpRxCount = uswap32(respHdl.portStatus->stats.pDelayRespFollowUpRxCount);
        portStatus->stats.syncReceiptTimeouts = uswap32(respHdl.portStatus->stats.syncReceiptTimeouts);
        portStatus->stats.followupReceiptTimeouts = uswap32(respHdl.portStatus->stats.followupReceiptTimeouts);
        portStatus->stats.pDelayReceiptTimeouts = uswap32(respHdl.portStatus->stats.pDelayReceiptTimeouts);
        portStatus->stats.badPdelayValues = uswap32(respHdl.portStatus->stats.badPdelayValues);
        portStatus->stats.txErrors = uswap32(respHdl.portStatus->stats.txErrors);
        portStatus->stats.tsErrors = uswap32(respHdl.portStatus->stats.tsErrors);
        portStatus->stats.ptpDiscardCount = uswap32(respHdl.portStatus->stats.ptpDiscardCount);
        portStatus->stats.parseFailed = uswap32(respHdl.portStatus->stats.parseFailed);
        portStatus->stats.txConf = uswap32(respHdl.portStatus->stats.txConf);
        portStatus->stats.pDelayLostResponsesExceeded = uswap32(respHdl.portStatus->stats.pDelayLostResponsesExceeded);
    }

done:
    return retVal;
}

/*
   @api
 * mgmt_stats_clear
 *
 * @brief
 * Clear the 802.1AS stats for a port
 *
 * @param=info - management info
 * @param=port - port number
 *
 * @returns 0 on success, !0 on error
 */
int32_t HOST_ETSPortStatsClear(MgmtInfoType *info, uint32_t port)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.portStatus->num = uswap32(port);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_RESETSTATS);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(ETS_PortStatsAndStatusType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_PortStatsAndStatusType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_ETSTimeSet(MgmtInfoType *info, uint32_t sec_high, uint32_t sec_low, uint32_t nanosec)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_SETGLOBALTIME);

    cmdHdl.time->seconds = uswap64((((uint64_t)sec_high << 32) | sec_low));
    cmdHdl.time->nanoseconds = uswap32(nanosec);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETS_TimestampType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETS_TimestampType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_ETSGlobalAdminModeSet(MgmtInfoType *info, ETS_AdminModeType adminMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    } else if ((adminMode != ETS_ADMINMODE_DISABLE) && (adminMode != ETS_ADMINMODE_ENABLE)) {
        HOST_Log("%s invalid mode:%d\n", __FUNCTION__, adminMode);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    config.adminMode = adminMode;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSClockModeSet(MgmtInfoType *info, ETS_ClockModeType mode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    } else if ((mode != ETS_CLOCKMODE_GM) && (mode != ETS_CLOCKMODE_SLAVE)) {
        HOST_Log("%s invalid mode:%d\n", __FUNCTION__, mode);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    config.clockMode = mode;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSSyncAbsenceTimeoutSet(MgmtInfoType *info, uint32_t timeout)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    config.avnuSyncAbsenceTimeout = timeout;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}


int32_t HOST_ETSAsCapableModeSet(MgmtInfoType *info, uint32_t port, ETS_AdminModeType asCapable)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    } else if ((asCapable != ETS_ADMINMODE_DISABLE) && (asCapable != ETS_ADMINMODE_ENABLE)) {
        HOST_Log("%s invalid mode:%d\n", __FUNCTION__, asCapable);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }
    HOST_Log("port %u idx %u numInterfaces %u\n", port, idx, config.numInterfaces);

    config.intfCfg[idx].asCapable = asCapable;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSPortRoleSet(MgmtInfoType *info, uint32_t port, ETS_RoleType role)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    } else if ((role != ETS_ROLE_MASTER) && (role != ETS_ROLE_SLAVE) && (role != ETS_ROLE_STACKING)) {
        HOST_Log("%s invalid role:%d\n", __FUNCTION__, role);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].role = role;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSInitPdelayIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].initLogPdelayInterval = interval;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSOperPdelayIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].operLogPdelayInterval = interval;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSInitSyncIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval)
{
    uint32_t idx;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].initLogSyncInterval = interval;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSNbrPdelaySet(MgmtInfoType *info, uint32_t port, uint32_t pdelay)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].nbrPropDelay = pdelay;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

int32_t HOST_ETSNumLostRespSet(MgmtInfoType *info, uint32_t port, uint32_t numLostResp)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx;
    ETS_ConfigType config;

    /* validate parameters */
    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    retVal = HOST_ETSConfigGet(info, &config);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    idx = HOST_ETSPortToIndex(port, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    config.intfCfg[idx].allowedLostResponses = numLostResp;

    retVal = HOST_ETSConfigSet(info, &config);

done:
    return retVal;
}

#ifdef ENABLE_RECORD_NOTIFICATION
int32_t HOST_ETSStartStopSendingRecord(MgmtInfoType *info, uint8_t isStart)
{
    int32_t retVal = -1;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    ETS_CmdRespPayloadType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    ETS_CmdRespPayloadType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    retVal = HOST_ETSConfigGet(info, &config);
    if ((retVal != BCM_ERR_OK) || (ETS_CONFIG_MAGIC_ID != config.magicId)) {
        HOST_Log("%s :: Invalid return value of ETS Config: %lu\n", __FUNCTION__, retVal);
        goto done;
    }
    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_CMD_NOTIFY_RECORD);
    *cmdHdl.isNotifyingRecord = uswap32(isStart);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(*cmdHdl.isNotifyingRecord), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

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

    if (0U == isStart) {
        memset(portRecords, 0U, sizeof(portRecords));
    }
done:
    return retVal;
}

int32_t HOST_ETSGetRecord(uint32_t portIdx, ETS_RecordType *records, uint32_t size)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t idx;

    /* validate parameters */
    if (records == NULL)  {
        HOST_Log("%s :: ETS_RecordType pointer is NULL\n", __FUNCTION__);
        goto done;
    }

    if (size > ETS_NUM_RECORDS_PER_INTF)  {
        HOST_Log("%s :: invalid size:%d\n", __FUNCTION__, size);
        goto done;
    }

    if(ETS_CONFIG_MAGIC_ID != config.magicId) {
        retVal = BCM_ERR_UNINIT;
        HOST_Log("%s :: ETS config is not valid \n", __FUNCTION__);
        goto done;
    }
    idx = HOST_ETSPortToIndex(portIdx, &config.intfCfg[0]);
    if (idx >= ETS_MAX_INTERFACES) {
        HOST_Log("%s :: idx is: %lu , which is greater than max allowed value: %lu \n", __FUNCTION__, idx, (ETS_MAX_INTERFACES - 1));
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memcpy(records, &portRecords[idx] , size * sizeof(ETS_RecordType));
done:
    return retVal;
}

static int32_t HOST_ETSMsgToIndex(uint32_t isTX, ETS_MsgType msgType, uint32_t* msgIdx)
{
    int32_t retVal = BCM_ERR_OK;
    switch (msgType) {
        case ETS_MSG_SYNC:
            *msgIdx = 0UL;
            break;
        case ETS_MSG_FOLLOWUP:
            *msgIdx = 1UL;
            break;
        case ETS_MSG_PDELAY_REQ:
            *msgIdx = (isTX == 1UL) ? 4UL: 2UL ;
            break;
        case ETS_MSG_PDELAY_RESP:
            *msgIdx = (isTX == 1UL) ? 5UL: 3UL ;
            break;
        case ETS_MSG_PDELAY_RESP_FOLLOWUP:
            *msgIdx = (isTX == 1UL) ? 7UL: 6UL ;
            break;
        default:
            *msgIdx = 0xFFFFFFFF;
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }
    return retVal;
}
#endif

int32_t HOST_ETSNotificationHandler(uint32_t currentSlave, ETS_EventType notificationId, uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    ETS_CmdRespPayloadType notificationHdl;

    if (NULL == msg) {
        HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x msg:%p size=%d\n",
            __func__, currentSlave, notificationId, msg, size);
        goto done;
    }

    notificationHdl.u8Ptr = msg;

    switch (notificationId) {
    case ETS_EVENT_IF_UP:
        HOST_Log("%s SPI-Id:%u Interface:%u Up\n", HOST_ETS_ASYNC_PREFIX,
                currentSlave, uswap32(*notificationHdl.port));
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_IF_DOWN:
        HOST_Log("%s SPI-Id:%u Interface:%u Down\n", HOST_ETS_ASYNC_PREFIX,
                currentSlave, uswap32(*notificationHdl.port));
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_SYNC:
        HOST_Log("%s SPI-Id:%u Sync\n", HOST_ETS_ASYNC_PREFIX, currentSlave);
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_SYNC_LOST:
        HOST_Log("%s SPI-Id:%u Sync Lost\n", HOST_ETS_ASYNC_PREFIX, currentSlave);
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_IF_LOST_RESP_EXCEEDED:
        HOST_Log("%s SPI-Id:%u Interface:%u Lost response exceeded\n",
                HOST_ETS_ASYNC_PREFIX, currentSlave, uswap32(*notificationHdl.port));
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_IF_LOST_RESP_RECOVERED:
        HOST_Log("%s SPI-Id:%u Interface:%u Lost response recovered\n",
                HOST_ETS_ASYNC_PREFIX, currentSlave, uswap32(*notificationHdl.port));
        retVal = BCM_ERR_OK;
        break;
    case ETS_EVENT_IF_PDELAY_CHANGED:
        HOST_Log("%s SPI-Id:%u Interface:%u PDelay changed to %u NBRR 0x%x\n",
                HOST_ETS_ASYNC_PREFIX, currentSlave, uswap32(notificationHdl.pDelayNBRR->port),
                uswap32(notificationHdl.pDelayNBRR->pDelay), uswap32(notificationHdl.pDelayNBRR->nbrr));
        retVal = BCM_ERR_OK;
        break;
#ifdef ENABLE_RECORD_NOTIFICATION
    case ETS_EVENT_IF_RECORD:
        {
            uint32_t portIdx;
            uint32_t i;
            uint32_t validRecords;
            uint32_t msgIdx;
            uint32_t idx;
            uint32_t flag;
            uint32_t portAndRecNum;
            if(ETS_CONFIG_MAGIC_ID != config.magicId) {
                retVal = BCM_ERR_UNINIT;
                HOST_Log("%s :: ETS config is not valid \n", __FUNCTION__);
                goto done;
            }
            validRecords = uswap32(notificationHdl.records->numRecords);
            for (i = 0UL; i < validRecords; ++i) {
                    /* Cache the latest record for each port */
                    portAndRecNum = uswap32(notificationHdl.records->records[i].portAndRecNum);
                    portIdx = (portAndRecNum & ETS_RECORD_PORT_NUM_MASK) >> ETS_RECORD_PORT_NUM_SHIFT;
                    idx = HOST_ETSPortToIndex(portIdx, &config.intfCfg[0]);
                    if (idx >= ETS_MAX_INTERFACES) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                        HOST_Log("%s :: idx is: %lu , which is greater than max allowed value: %lu \n", __FUNCTION__, idx, (ETS_MAX_INTERFACES - 1));
                        goto done;
                    }
                    flag = uswap32(notificationHdl.records->records[i].flag);
                    retVal = HOST_ETSMsgToIndex((flag & ETS_RECORD_IS_TX_MASK) >> ETS_RECORD_IS_TX_SHIFT,
                                        (flag & ETS_RECORD_PACKET_TYPE_MASK) >> ETS_RECORD_PACKET_TYPE_SHIFT,
                                        &msgIdx);
                    if (retVal != BCM_ERR_OK) {
                        HOST_Log("%s :: msgIdx invalid: %lu\n", __FUNCTION__, msgIdx);
                        goto done;
                    }
                    portRecords[idx][msgIdx].gmTime.s           = uswap32(notificationHdl.records->records[i].gmTime.s);
                    portRecords[idx][msgIdx].gmTime.ns          = uswap32(notificationHdl.records->records[i].gmTime.ns);
                    portRecords[idx][msgIdx].localTime.s        = uswap32(notificationHdl.records->records[i].localTime.s);
                    portRecords[idx][msgIdx].localTime.ns       = uswap32(notificationHdl.records->records[i].localTime.ns);
                    portRecords[idx][msgIdx].portAndRecNum      = portAndRecNum;
                    portRecords[idx][msgIdx].flag               = flag;
            }
            retVal = BCM_ERR_OK;
        }
        break;
#endif
    default:
        HOST_Log("%s SPI-Id:%u Invalid parameter notificationId:0x%x size=%d\n",
            __func__, currentSlave, notificationId, size);
        break;
    }

done:
    return retVal;
}

