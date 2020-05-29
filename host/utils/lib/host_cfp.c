/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "hipc.h"
#include <bcm_err.h>
#include <rpc_cmds.h>
#include <host_cfp.h>
#include <etherswt_ipc.h>
#include <hlog.h>

int32_t HOST_CFPGetStats(MgmtInfoType *info,
                       uint32_t row,
                       CFP_StatsType *stats)
{
    int32_t                retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    HOST_Log("CFPGetStats invoked for %u\n", row);

    if ((info == NULL) || (stats == NULL) || (row >= CFP_MAX_RULES)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_GETSTATS);

    cmdHdl.cfpStats->row = uswap32(row);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_CFPStatsType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_CFPStatsType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        stats->green  = uswap32(respHdl.cfpStats->stats.green);
        stats->yellow = uswap32(respHdl.cfpStats->stats.yellow);
        stats->red    = uswap32(respHdl.cfpStats->stats.red);
    }
done:
    return retVal;
}

int32_t HOST_CFPGetRowConfig(MgmtInfoType *info, uint32_t row, CFP_RuleType* config)
{
    int32_t                retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;
    uint32_t             i;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    if ((info == NULL) || (config == NULL) || (row >= CFP_MAX_RULES)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_GETROWCONFIG);

    cmdHdl.cfpRule->rowAndSlice = uswap16((row << CFP_ROWANDSLICE_ROW_SHIFT) & CFP_ROWANDSLICE_ROW_MASK);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(CFP_RuleType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(CFP_RuleType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        config->rowAndSlice           = uswap16(respHdl.cfpRule->rowAndSlice);
        config->key.flags             = uswap32(respHdl.cfpRule->key.flags);
        config->key.flagsMask         = uswap32(respHdl.cfpRule->key.flagsMask);
        config->key.ingressPortBitmap = uswap16(respHdl.cfpRule->key.ingressPortBitmap);
        config->key.cTagFlags         = uswap32(respHdl.cfpRule->key.cTagFlags);
        config->key.sTagFlags         = uswap32(respHdl.cfpRule->key.sTagFlags);
        config->key.cTagMask          = uswap16(respHdl.cfpRule->key.cTagMask);
        config->key.sTagMask          = uswap16(respHdl.cfpRule->key.sTagMask);
        config->key.l2Framing         = respHdl.cfpRule->key.l2Framing;
        config->key.l3Framing         = respHdl.cfpRule->key.l3Framing;
        config->key.numEnabledUDFs    = respHdl.cfpRule->key.numEnabledUDFs;
        for (i = 0UL; i < respHdl.cfpRule->key.numEnabledUDFs; ++i) {
            config->key.udfList[i].value         = uswap16(respHdl.cfpRule->key.udfList[i].value);
            config->key.udfList[i].mask          = uswap16(respHdl.cfpRule->key.udfList[i].mask);
            config->key.udfList[i].baseAndOffset = respHdl.cfpRule->key.udfList[i].baseAndOffset;
        }

        config->action.dstMapIBFlags      = uswap32(respHdl.cfpRule->action.dstMapIBFlags);
        config->action.dstMapOBFlags      = uswap32(respHdl.cfpRule->action.dstMapOBFlags);
        config->action.tosIBFlags         = respHdl.cfpRule->action.tosIBFlags;
        config->action.tosOBFlags         = respHdl.cfpRule->action.tosOBFlags;
        config->action.tcFlags            = respHdl.cfpRule->action.tcFlags;
        config->action.reasonCode         = respHdl.cfpRule->action.reasonCode;
        config->action.otherFlags         = respHdl.cfpRule->action.otherFlags;
        config->action.colorFlags         = respHdl.cfpRule->action.colorFlags;
        config->action.chainID            = respHdl.cfpRule->action.chainID;
        config->action.meter.policerFlags = respHdl.cfpRule->action.meter.policerFlags;
        config->action.meter.cirTkBkt     = uswap32(respHdl.cfpRule->action.meter.cirTkBkt);
        config->action.meter.cirRefCnt    = uswap32(respHdl.cfpRule->action.meter.cirRefCnt);
        config->action.meter.cirBktSize   = uswap32(respHdl.cfpRule->action.meter.cirBktSize);
        config->action.meter.eirTkBkt     = uswap32(respHdl.cfpRule->action.meter.eirTkBkt);
        config->action.meter.eirRefCnt    = uswap32(respHdl.cfpRule->action.meter.eirRefCnt);
        config->action.meter.eirBktSize   = uswap32(respHdl.cfpRule->action.meter.eirBktSize);
    }
done:
    return retVal;
}

int32_t HOST_CFPGetSnapshot(MgmtInfoType *info, CFP_TableSnapshotType* snapShot)
{
    int32_t              retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    if ((info == NULL) || (snapShot == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_GETSNAPSHOT);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(CFP_TableSnapshotType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(CFP_TableSnapshotType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        snapShot->numValidEntries     = uswap32(respHdl.cfpSnapshot->numValidEntries);
        memcpy(snapShot->entry, respHdl.cfpSnapshot->entry, sizeof(CFP_EntrySnapshotType) * CFP_MAX_RULES);
        memcpy(&snapShot->udfList, &respHdl.cfpSnapshot->udfList, sizeof(snapShot->udfList));
        snapShot->portEnableMask      = uswap16(respHdl.cfpSnapshot->portEnableMask);
    }
done:
    return retVal;
}
int32_t HOST_CFPAddRule(MgmtInfoType *info, CFP_RuleType *config)
{
    int32_t              retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;
    uint32_t             i;
    uint16_t             row;
    uint16_t             slice;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    if ((info == NULL) || (config == NULL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    /* Perform basic validation */
    row   = (config->rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT;
    slice = (config->rowAndSlice & CFP_ROWANDSLICE_SLICE_MASK) >> CFP_ROWANDSLICE_SLICE_SHIFT;
    if ((row >= CFP_MAX_RULES) || (slice >= CFP_MAX_SLICES)) {
        HOST_Log("CFP row number: [0,%lu] slice number: [0, %lu]\n", CFP_MAX_RULES-1UL, CFP_MAX_SLICES-1UL);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_ADDRULE);

    cmdHdl.cfpRule->rowAndSlice            = uswap16(config->rowAndSlice);
    cmdHdl.cfpRule->key.flags              = uswap32(config->key.flags);
    cmdHdl.cfpRule->key.flagsMask          = uswap32(config->key.flagsMask);
    cmdHdl.cfpRule->key.ingressPortBitmap  = uswap16(config->key.ingressPortBitmap);
    cmdHdl.cfpRule->key.cTagFlags          = uswap32(config->key.cTagFlags);
    cmdHdl.cfpRule->key.sTagFlags          = uswap32(config->key.sTagFlags);
    cmdHdl.cfpRule->key.cTagMask           = uswap16(config->key.cTagMask);
    cmdHdl.cfpRule->key.sTagMask           = uswap16(config->key.sTagMask);
    cmdHdl.cfpRule->key.l2Framing          = config->key.l2Framing;
    cmdHdl.cfpRule->key.l3Framing          = config->key.l3Framing;
    cmdHdl.cfpRule->key.numEnabledUDFs     = config->key.numEnabledUDFs;
    for (i = 0UL; i < cmdHdl.cfpRule->key.numEnabledUDFs; ++i) {
        cmdHdl.cfpRule->key.udfList[i].value         = uswap16(config->key.udfList[i].value);
        cmdHdl.cfpRule->key.udfList[i].mask          = uswap16(config->key.udfList[i].mask);
        cmdHdl.cfpRule->key.udfList[i].baseAndOffset = config->key.udfList[i].baseAndOffset;
    }

    cmdHdl.cfpRule->action.dstMapIBFlags      = uswap32(config->action.dstMapIBFlags);
    cmdHdl.cfpRule->action.dstMapOBFlags      = uswap32(config->action.dstMapOBFlags);
    cmdHdl.cfpRule->action.tosIBFlags         = config->action.tosIBFlags;
    cmdHdl.cfpRule->action.tosOBFlags         = config->action.tosOBFlags;
    cmdHdl.cfpRule->action.tcFlags            = config->action.tcFlags;
    cmdHdl.cfpRule->action.reasonCode         = config->action.reasonCode;
    cmdHdl.cfpRule->action.otherFlags         = config->action.otherFlags;
    cmdHdl.cfpRule->action.colorFlags         = config->action.colorFlags;
    cmdHdl.cfpRule->action.chainID            = config->action.chainID;
    cmdHdl.cfpRule->action.meter.policerFlags = config->action.meter.policerFlags;
    cmdHdl.cfpRule->action.meter.cirTkBkt     = uswap32(config->action.meter.cirTkBkt);
    cmdHdl.cfpRule->action.meter.cirRefCnt    = uswap32(config->action.meter.cirRefCnt);
    cmdHdl.cfpRule->action.meter.cirBktSize   = uswap32(config->action.meter.cirBktSize);
    cmdHdl.cfpRule->action.meter.eirTkBkt     = uswap32(config->action.meter.eirTkBkt);
    cmdHdl.cfpRule->action.meter.eirRefCnt    = uswap32(config->action.meter.eirRefCnt);
    cmdHdl.cfpRule->action.meter.eirBktSize   = uswap32(config->action.meter.eirBktSize);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(CFP_RuleType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(CFP_RuleType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

int32_t HOST_CFPDeleteRule(MgmtInfoType *info, uint32_t row)
{
    int32_t                retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    if ((info == NULL) || (row >= CFP_MAX_RULES)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_REMOVERULE);

    cmdHdl.cfpRule->rowAndSlice = uswap16((row << CFP_ROWANDSLICE_ROW_SHIFT) & CFP_ROWANDSLICE_ROW_MASK);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(CFP_RuleType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(CFP_RuleType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

int32_t HOST_CFPUpdateRule(MgmtInfoType *info, uint32_t row, CFP_ActionType *action)
{
    int32_t                retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    if ((info == NULL) || (action == NULL) || (row >= CFP_MAX_RULES)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_UPDATERULE);

    cmdHdl.cfpRule->rowAndSlice               = uswap16((row << CFP_ROWANDSLICE_ROW_SHIFT) & CFP_ROWANDSLICE_ROW_MASK);
    cmdHdl.cfpRule->action.dstMapIBFlags      = uswap32(action->dstMapIBFlags);
    cmdHdl.cfpRule->action.dstMapOBFlags      = uswap32(action->dstMapOBFlags);
    cmdHdl.cfpRule->action.tosIBFlags         = action->tosIBFlags;
    cmdHdl.cfpRule->action.tosOBFlags         = action->tosOBFlags;
    cmdHdl.cfpRule->action.tcFlags            = action->tcFlags;
    cmdHdl.cfpRule->action.reasonCode         = action->reasonCode;
    cmdHdl.cfpRule->action.otherFlags         = action->otherFlags;
    cmdHdl.cfpRule->action.colorFlags         = action->colorFlags;
    cmdHdl.cfpRule->action.chainID            = action->chainID;
    cmdHdl.cfpRule->action.meter.policerFlags = action->meter.policerFlags;
    cmdHdl.cfpRule->action.meter.cirTkBkt     = uswap32(action->meter.cirTkBkt);
    cmdHdl.cfpRule->action.meter.cirRefCnt    = uswap32(action->meter.cirRefCnt);
    cmdHdl.cfpRule->action.meter.cirBktSize   = uswap32(action->meter.cirBktSize);
    cmdHdl.cfpRule->action.meter.eirTkBkt     = uswap32(action->meter.eirTkBkt);
    cmdHdl.cfpRule->action.meter.eirRefCnt    = uswap32(action->meter.eirRefCnt);
    cmdHdl.cfpRule->action.meter.eirBktSize   = uswap32(action->meter.eirBktSize);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(CFP_RuleType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(CFP_RuleType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

int32_t HOST_CFPSetPortMode(MgmtInfoType *info, uint32_t port, uint32_t enable)
{
    int32_t                retVal = BCM_ERR_OK;
    uint8_t              cmdPayload[RPC_CMDPAYLOADSZ];
    ETHERSWT_PayloadType cmdHdl;
    ETHERSWT_PayloadType respHdl;
    RPC_ResponseType     resp;
    uint32_t             replyLen;
    uint32_t             cmdId;

    cmdHdl.u8Ptr = &cmdPayload[0];
    respHdl.u8Ptr = &resp.payload[0];

    /*FIXME: Validate port */
    if ((info == NULL) || ((enable != 0UL) && (enable != 1UL))){
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_COMMS, BCM_SWT_ID, ETHERSWT_CMDID_CFP_SETPORTMODE);

    cmdHdl.cfpPortMode->port = uswap32(port);
    cmdHdl.cfpPortMode->mode = uswap32(enable);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
             sizeof(ETHERSWT_CFPPortModeType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(ETHERSWT_CFPPortModeType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }
done:
    return retVal;
}

int32_t HOST_CFPNotificationHandler(uint32_t currentSlave,
            uint8_t notificationId, uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    return retVal;
}

