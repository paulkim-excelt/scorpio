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
    @defgroup grp_ets_rpc ETS RPC Integration Layer
    @addtogroup grp_ets_rpc
    @ingroup grp_ets
    @{
    @file ets_rpc.c
    @brief ETS RPC Integration Layer
    This source file contains the integration of ETS to RPC
    @version 0.1 Initial version
*/

#include <utils.h>
#include <ets_ipc.h>
#include <bcm_err.h>
#include <comms.h>
#include <ulog.h>
#define GetModuleLogLevel() ULOG_LVL_ERROR

int32_t ETS_CmdHandler(ETS_CmdType aCmdID, BCM_CompIDType aCompID,
        uint8_t *aCmd, uint32_t aLen,
        const uint32_t aClientMask,
        const MSGQ_MsgHdrType** const pHdr,
        uint32_t *aForwardToSlaves)
{
    ETS_CmdRespPayloadType ipc;
    ETS_CmdRespType        cmdResp;
    int32_t                retVal = BCM_ERR_OK;
    uint32_t               sendCmd = TRUE;
    MCU_ExtendedInfoType   stackingInfo = {0U};

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK == retVal) {
        /* Map the incoming request */
        ipc.u8Ptr = aCmd;

        cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
        cmdResp.cmdId            = aCmdID;
        cmdResp.ret              = BCM_ERR_UNKNOWN;

        *aForwardToSlaves = TRUE;
        switch (aCmdID) {
            case ETS_CMD_GETCONFIG:
            case ETS_CMD_GETGLOBALSTATUS:
                /* Fall through intentional */
                break;
            case ETS_CMD_GETINTFSTATUS:
                {
                    uint32_t isStackingPort;
                    uint32_t port = COMMS_ConvertToLocalPort(&stackingInfo,
                                                             ipc.portStatus->num,
                                                             &isStackingPort);
                    if (port == COMMS_INVALID_PORT) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                    } else if (TRUE == isStackingPort) {
                        sendCmd = FALSE;
                    } else {
                        cmdResp.payloadPtr.portStatus->num = port;
                        *aForwardToSlaves = FALSE;
                    }
                }
                break;
            case ETS_CMD_SETCONFIG:
                if (aLen != sizeof(ETS_ConfigType)) {
                    retVal = BCM_ERR_INVAL_PARAMS;
                } else {
                    BCM_MemCpy(cmdResp.payloadPtr.config, ipc.config, sizeof(ETS_ConfigType));
                    /* Convert from unified port number to local port number */
                    COMMS_UpdateETSConfig(&stackingInfo, cmdResp.payloadPtr.config, TRUE);
                }
                break;
            case ETS_CMD_RESETSTATS:
                {
                    uint32_t isStackingPort;
                    uint32_t port = COMMS_ConvertToLocalPort(&stackingInfo,
                                                             *ipc.port,
                                                             &isStackingPort);
                    if (port == COMMS_INVALID_PORT) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                    } else if (TRUE == isStackingPort) {
                        sendCmd = FALSE;
                    } else {
                        *cmdResp.payloadPtr.port = port;
                        *aForwardToSlaves = FALSE;
                    }
                }
                break;
            case ETS_CMD_SETGLOBALTIME:
                if (aLen != sizeof(ETS_TimestampType)) {
                    retVal = BCM_ERR_NOMEM;
                } else if (TRUE == stackingInfo.stackingEn) {
                    retVal = BCM_ERR_NOSUPPORT;
                } else {
                    *aForwardToSlaves = FALSE;
                    BCM_MemCpy(cmdResp.payloadPtr.time, ipc.time, sizeof(ETS_TimestampType));
                }
                break;
            case ETS_CMD_NOTIFY_RECORD:
                *cmdResp.payloadPtr.isNotifyingRecord = *ipc.isNotifyingRecord;
                break;
            default:
                *aForwardToSlaves = FALSE;
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }

        if ((BCM_ERR_OK == retVal) && (TRUE == sendCmd)) {
            retVal = ETS_SendMsg(&cmdResp, aClientMask, pHdr);
        }
    }

    return retVal;
}

int32_t ETS_GetCmdStatus(const MSGQ_MsgHdrType* const pHdr,
                         BCM_CompIDType aCompID,
                         ETS_CmdType* const aCmdID,
                         uint8_t *aReply,
                         uint32_t *aReplyLen)
{
    ETS_CmdRespType       cmdResp;
    int32_t               retVal = BCM_ERR_OK;
    MCU_ExtendedInfoType  stackingInfo = {0U};

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if ((pHdr != NULL) && (aReply != NULL) && (aReplyLen != NULL) && (BCM_ERR_OK == retVal)) {
        retVal = ETS_RecvResp(pHdr, &cmdResp);
        if (BCM_ERR_OK == retVal) {
            cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
            *aCmdID  = cmdResp.cmdId;
            switch (cmdResp.cmdId) {
                case ETS_CMD_GETCONFIG:
                    /* Convert from local port number to unified port number */
                    COMMS_UpdateETSConfig(&stackingInfo, cmdResp.payloadPtr.config, FALSE);
                    BCM_MemCpy(aReply, cmdResp.payloadPtr.config, sizeof(ETS_ConfigType));
                    *aReplyLen = sizeof(ETS_ConfigType);
                    break;
                case ETS_CMD_SETCONFIG:
                    BCM_MemSet(aReply, 0U, sizeof(ETS_ConfigType));
                    *aReplyLen = sizeof(ETS_ConfigType);
                    break;
                case ETS_CMD_GETGLOBALSTATUS:
                    {
                        uint32_t isStackingPort = FALSE;
                        uint32_t portNum = COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                                      cmdResp.payloadPtr.globalStatus->slavePort,
                                                                      &isStackingPort);
                        cmdResp.payloadPtr.globalStatus->slavePort = (TRUE == isStackingPort) ? 0xFFFFUL:portNum;
                        BCM_MemCpy(aReply, cmdResp.payloadPtr.globalStatus, sizeof(ETS_GlobalStatusType));
                        *aReplyLen = sizeof(ETS_GlobalStatusType);
                    }
                    break;
                case ETS_CMD_GETINTFSTATUS:
                    {
                        uint32_t isStackingPort = FALSE;
                        cmdResp.payloadPtr.portStatus->num = COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                                                        cmdResp.payloadPtr.portStatus->num,
                                                                                        &isStackingPort);
                        BCM_MemCpy(aReply, cmdResp.payloadPtr.portStatus, sizeof(ETS_PortStatsAndStatusType));
                        *aReplyLen = sizeof(ETS_PortStatsAndStatusType);
                    }
                    break;
                case ETS_CMD_RESETSTATS:
                    BCM_MemSet(aReply, 0U, sizeof(ETS_PortStatsAndStatusType));
                    *aReplyLen = sizeof(ETS_PortStatsAndStatusType);
                    break;
                case ETS_CMD_SETGLOBALTIME:
                    *aReplyLen = sizeof(ETS_TimestampType);
                    break;
                case ETS_CMD_NOTIFY_RECORD:
                    *aReplyLen = 0UL;
                    break;
                default:
                    retVal = BCM_ERR_NOSUPPORT;
                    break;
            }
            if (BCM_ERR_BUSY != cmdResp.ret) {
                retVal = cmdResp.ret;
            } else {
                retVal = BCM_ERR_UNKNOWN;
            }
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

int32_t ETS_MergeResult (uint8_t aCmdID,
                         BCM_CompIDType aCompID,
                         uint32_t aLen,
                         uint8_t *aOutResponse,
                         int32_t *aOutResult,
                         const uint8_t *aInResponse1,
                         int32_t aInResult1,
                         const uint8_t *aInResponse2,
                         int32_t aInResult2)
{
    int32_t                retVal = BCM_ERR_OK;
    ETS_CmdRespPayloadType output;
    ETS_CmdRespPayloadType input1;
    ETS_CmdRespPayloadType input2;

    if ((BCM_ETS_ID != aCompID) ||
        (NULL == aOutResponse) ||
        (NULL == aOutResult) ||
        (NULL == aInResponse1) ||
        (NULL == aInResponse2)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        input1.u8Ptr = (uint8_t*)aInResponse1;
        input2.u8Ptr = (uint8_t*)aInResponse2;
        output.u8Ptr = aOutResponse;

        switch (aCmdID) {
            case ETS_CMD_GETCONFIG:
                {
                    uint32_t numInterfaces = 0UL;
                    uint32_t i = 0UL;

                    if ((sizeof(ETS_ConfigType) != aLen) ||
                        (BCM_ERR_OK != aInResult1) ||
                        (BCM_ERR_OK != aInResult2)) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                    } else {
                        BCM_MemCpy(output.config,
                                   input1.config,
                                   sizeof(ETS_ConfigType));
                        numInterfaces = input1.config->numInterfaces;
                        for (; i < input2.config->numInterfaces; ++i) {
                            BCM_MemCpy(&output.config->intfCfg[numInterfaces],
                                       &input2.config->intfCfg[i],
                                       sizeof(ETS_IntfConfigType));
                            numInterfaces++;
                        }
                    }
                    output.config->numInterfaces = numInterfaces;
                    *aOutResult = BCM_ERR_OK;
                }
                break;
            case ETS_CMD_GETGLOBALSTATUS:
                if ((sizeof(ETS_GlobalStatusType) != aLen) ||
                    (BCM_ERR_OK != aInResult1) ||
                    (BCM_ERR_OK != aInResult2)) {
                    retVal = BCM_ERR_INVAL_PARAMS;
                } else {
                    if (input1.globalStatus->slavePort != 0xFFFFUL) {
                        BCM_MemCpy(output.globalStatus,
                                   input1.globalStatus,
                                   sizeof(ETS_GlobalStatusType));
                    } else if (input2.globalStatus->slavePort != 0xFFFFUL) {
                        BCM_MemCpy(output.globalStatus,
                                   input2.globalStatus,
                                   sizeof(ETS_GlobalStatusType));
                    } else {
                        /* Must be in GM mode. Pick the response with a better clockState */
                        if (input1.globalStatus->clockState > input2.globalStatus->clockState) {
                            BCM_MemCpy(output.globalStatus,
                                       input1.globalStatus,
                                       sizeof(ETS_GlobalStatusType));
                        } else {
                            BCM_MemCpy(output.globalStatus,
                                       input2.globalStatus,
                                       sizeof(ETS_GlobalStatusType));
                        }
                    }
                    *aOutResult = BCM_ERR_OK;
                }
                break;
            case ETS_CMD_GETINTFSTATUS:
                if ((sizeof(ETS_PortStatsAndStatusType) != aLen) ||
                    (aInResult1 == aInResult2)) {
                    retVal = BCM_ERR_INVAL_PARAMS;
                } else {
                    if (BCM_ERR_OK == aInResult1) {
                        BCM_MemCpy(output.portStatus,
                                   input1.portStatus,
                                   sizeof(ETS_PortStatsAndStatusType));
                        *aOutResult = BCM_ERR_OK;
                    } else if (BCM_ERR_OK == aInResult2) {
                        BCM_MemCpy(output.portStatus,
                                   input2.portStatus,
                                   sizeof(ETS_PortStatsAndStatusType));
                        *aOutResult = BCM_ERR_OK;
                    } else {
                        *aOutResult = BCM_ERR_INVAL_PARAMS;
                    }
                }
                break;
            case ETS_CMD_SETCONFIG:
                if (sizeof(ETS_ConfigType) != aLen) {
                    retVal = BCM_ERR_INVAL_PARAMS;
                } else {
                    if ((BCM_ERR_OK != aInResult1) ||
                       (BCM_ERR_OK != aInResult2)) {
                        *aOutResult = (BCM_ERR_OK != aInResult1) ? aInResult2: aInResult1;
                    } else {
                        *aOutResult = BCM_ERR_OK;
                    }
                }
                break;
            case ETS_CMD_RESETSTATS:
                if ((sizeof(ETS_PortStatsAndStatusType) != aLen) ||
                    (aInResult1 == aInResult2)) {
                    retVal = BCM_ERR_INVAL_PARAMS;
                } else {
                    *aOutResult = BCM_ERR_OK;
                }
                break;
            default:
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }
    }

    return retVal;
}

/** @} */
