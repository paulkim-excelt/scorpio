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
    @defgroup grp_comms_ether Communication Subsystem platform layer for ethernet
    @ingroup grp_comms_sw

    @addtogroup grp_comms_ether
    @{

    @file comms_ether.c
    @brief This file implements ethernet specific functionality for the Communication Subsystem

    @version 0.1 Initial draft
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <utils.h>
#include "comms_platform.h"
#include "comms_cfg.h"
#include "comms_osil.h"
#include "ulog.h"
#include <ether_ipc.h>
#include <osil/eth_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_ERROR

/**
    @name Communication Subsystem switch Design IDs
    @{
    @brief Design IDs for Communication Subsystem switch layer
*/
#define BRCM_SWDSGN_COMMS_ETHERPROCESSCOMMAND_PROC        (0x91U) /**< @brief #COMMS_EtherProcessCommand */
/** @} */

extern uint32_t COMMS_EtherTimeInit0Done;

static MDIO_JobIDType MdioJobID;
static MDIO_JobIDType MdioJobIDValid = FALSE;

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherProcessCommand(ETHER_CmdIdType aCmdId,
                                  int32_t * const aCmdStatus,
                                  uint8_t * const aCmdData,
                                  uint32_t * const aLen)
{
    int32_t    retVal = BCM_ERR_OK;
    ETHER_TimeInitOutType timeOut;
    ETHER_PayloadType PaylPtr;

    /* Error check */
    if((NULL == aCmdData) ||
       (NULL == aCmdStatus)){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        PaylPtr.u8Ptr = aCmdData;
        switch(aCmdId){
        case ETHER_CMDID_PREPARE_SYNC:{
            if (TRUE == COMMS_EtherTimeInit0Done) {
                ETHER_TimeInitStateHandler(ETHER_HW_ID_0,
                                           ETHER_TIMEINITCMD_PREP_SYNC,
                                           0ULL, NULL);
            } else {
                retVal = BCM_ERR_BUSY;
            }
            /* No response payload */
            *aLen = 0UL;
            break;
        }
        case ETHER_CMDID_PREPARE_HEARTBEAT_READ:{
            if (TRUE == COMMS_EtherTimeInit0Done) {
                ETHER_TimeInitStateHandler(ETHER_HW_ID_0,
                                           ETHER_TIMEINITCMD_PREP_HB_READ,
                                           0ULL, NULL);
            } else {
                retVal = BCM_ERR_BUSY;
            }
            /* No response payload */
            *aLen = 0UL;
            break;
        }
        case ETHER_CMDID_HEARTBEAT_GET:{
            if (TRUE == COMMS_EtherTimeInit0Done) {
                ETHER_TimeInitStateHandler(ETHER_HW_ID_0,
                                           ETHER_TIMEINITCMD_GET_HB,
                                           0ULL, &timeOut);
                PaylPtr.heartBeat->hb0 = (((uint64_t)timeOut.heartBeat0[3] << 48U) |
                                          ((uint64_t)timeOut.heartBeat0[2] << 32U) |
                                          ((uint64_t)timeOut.heartBeat0[1] << 16U) |
                                          ((uint64_t)timeOut.heartBeat0[0] << 00U));
                PaylPtr.heartBeat->hb1 = (((uint64_t)timeOut.heartBeat1[3] << 48U) |
                                          ((uint64_t)timeOut.heartBeat1[2] << 32U) |
                                          ((uint64_t)timeOut.heartBeat1[1] << 16U) |
                                          ((uint64_t)timeOut.heartBeat1[0] << 00U));
            } else {
                retVal = BCM_ERR_BUSY;
            }
            /* Fill size of heartbeat response */
            *aLen = sizeof(ETHER_HeartBeatType);
            break;
        }
        case ETHER_CMDID_TIMESYNC_ACK:{
            if (TRUE == COMMS_EtherTimeInit0Done) {
                ETHER_TimeInitStateHandler(ETHER_HW_ID_0,
                        ETHER_TIMEINITCMD_1, 0ULL, NULL);
#if defined(ENABLE_ETH) && defined(ENABLE_ETH_BRPHY)
                ETHER_TimeInitStateHandler(ETHER_HW_ID_0, ETHER_TIMEINITCMD_2, 0ULL, NULL);
#endif
#if defined(ENABLE_ETS)
                COMMS_InitETS();
#endif  /* ENABLE_ETS */
            } else {
                retVal = BCM_ERR_BUSY;
            }
            /* No response payload */
            *aLen = 0UL;
            break;
        }
        case ETHER_CMDID_MDIO_READ:
            *aLen = 0UL;
            if (TRUE == MdioJobIDValid) {
                retVal = ETHER_MdioCheckStatus(PaylPtr.mdioAccess->mdioHwID, MdioJobID, &PaylPtr.mdioAccess->pkt);
                if (BCM_ERR_BUSY == retVal) {
                    retVal = BCM_ERR_EAGAIN;
                } else {
                    MdioJobIDValid = FALSE;
                    /* read register value will be present in pkt.regData */
                    *aLen = sizeof(ETHER_MDIOAccessType);
                }
            } else {
                retVal = ETHER_MdioRead(PaylPtr.mdioAccess->mdioHwID, &PaylPtr.mdioAccess->pkt, &MdioJobID);
                if (BCM_ERR_OK == retVal) {
                    /* Read request accepted but not complete. So caller needs to check back again later */
                    MdioJobIDValid = TRUE;
                    retVal = BCM_ERR_EAGAIN;
                }
            }
            break;
        case ETHER_CMDID_MDIO_WRITE:
            *aLen = 0UL;
            if (TRUE == MdioJobIDValid) {
                retVal = ETHER_MdioCheckStatus(PaylPtr.mdioAccess->mdioHwID, MdioJobID, &PaylPtr.mdioAccess->pkt);
                if (BCM_ERR_BUSY == retVal) {
                    /* Write request accepted but not complete. So caller needs to check back again later */
                    retVal = BCM_ERR_EAGAIN;
                } else {
                    MdioJobIDValid = FALSE;
                }
            } else {
                retVal = ETHER_MdioWrite(PaylPtr.mdioAccess->mdioHwID, &PaylPtr.mdioAccess->pkt, &MdioJobID);
                if (BCM_ERR_OK == retVal) {
                    retVal = BCM_ERR_EAGAIN;
                    MdioJobIDValid = TRUE;
                }
            }
            break;
        default:
            retVal = BCM_ERR_NOSUPPORT;
            *aLen = 0UL;
            break;
        }
    }

    /* Update the command processing status */
    *aCmdStatus = retVal;

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherSendCmd(uint8_t aCmdID,
                           BCM_CompIDType aCompID,
                           uint8_t *aCmd, uint32_t aLen,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr,
                           uint32_t *aForwardToSlaves)
{
    int32_t            retVal;
    COMMS_CmdRespType  cmdResp;
    BCM_MemCpy(cmdResp.payload, aCmd, aLen);
    cmdResp.cmdID = aCmdID;
    cmdResp.compID = aCompID;
    cmdResp.ret = BCM_ERR_UNKNOWN;
    cmdResp.payloadLen = aLen;
    uint32_t sendCmd;
    MCU_ExtendedInfoType   stackingInfo = {0U};

    ETHER_PayloadType paylPtr;
    paylPtr.u8Ptr = cmdResp.payload;

    retVal = MCU_GetExtendedInfo(&stackingInfo);

    if ((ETHER_CMDID_MDIO_READ == aCmdID)
            || (ETHER_CMDID_MDIO_WRITE == aCmdID)) {
        if ((0U == stackingInfo.stackingEn) ||
                ((1U == stackingInfo.stackingEn) &&
                 (stackingInfo.mstSlvMode == paylPtr.mdioAccess->deviceID))) {
            *aForwardToSlaves = FALSE;
            sendCmd = TRUE;
        } else {
            *aForwardToSlaves = TRUE;
            sendCmd = FALSE;
        }
    } else {
        *aForwardToSlaves = FALSE;
        sendCmd = TRUE;
    }

    if (TRUE == sendCmd) {
        retVal = COMMS_AddToCmdQ(&cmdResp, aClientMask, aHdr);
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherCmdStatus(const MSGQ_MsgHdrType* const aHdr,
                             uint8_t *const aCmdID,
                             const BCM_CompIDType aCompID,
                             uint8_t *aReply,
                             uint32_t *aReplyLen)
{
    int32_t retVal;
    COMMS_CmdRespType cmdResp;

    retVal = COMMS_RecvResp(aHdr, &cmdResp);
    if (BCM_ERR_OK == retVal) {
        *aCmdID  = cmdResp.cmdID;

        if(RPC_RESPPAYLOADSZ > cmdResp.payloadLen){
            /* Copy the response payload */
            BCM_MemCpy(aReply, cmdResp.payload, cmdResp.payloadLen);
            *aReplyLen = cmdResp.payloadLen;
        } else {
            /* Case when the response payload is incorrect */
            cmdResp.ret = BCM_ERR_INVAL_PARAMS;
        }

        /* Copy the response status */
        if (BCM_ERR_BUSY != cmdResp.ret) {
            retVal = cmdResp.ret;
        } else {
            retVal = BCM_ERR_UNKNOWN;
        }
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherMergeResult(uint8_t aCmdID, BCM_CompIDType aCompID, uint32_t aLen,
                               uint8_t *aOutResponse,
                               int32_t *aOutResult,
                               const uint8_t *aInResponse1,
                               int32_t aInResult1,
                               const uint8_t *aInResponse2,
                               int32_t aInResult2)
{
    return BCM_ERR_OK;
}

/** @} */
