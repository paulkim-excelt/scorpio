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
    @defgroup grp_comms_platform_ifc Communication Subsystem Platform interface
    @ingroup grp_comms_sw

    @addtogroup grp_comms_platform_ifc
    @{

    @limitations None

    @file comms_platform.h
    @brief Interface for Communication Subsystem Platform layer
    This header file contains the interfaces for Communication Subsystem Platform layer
    @version 0.1 Initial Version
 */

#ifndef COMMS_PLATFORM_H
#define COMMS_PLATFORM_H

#include <stdint.h>
#include <ether_ipc.h>
#include <etherswt_ipc.h>
#include <eth_switch.h>
#include <cfp.h>
#include <comms_cfg.h>
#include <mcu_ext.h>
#include <msg_queue.h>

/**
    @name Communication Subsystem API IDs
    @{
    @brief API IDs for Communication Subsystem
*/
#define BRCM_SWARCH_COMMS_INITXCVRSTACKINGPORTS_PROC    (0x01U) /**< @brief #COMMS_InitXcvrStackingPorts */
#define BRCM_SWARCH_COMMS_GETSWITCHCONFIG_PROC          (0x02U) /**< @brief #COMMS_GetSwitchConfig */
#define BRCM_SWARCH_COMMS_ADDSWITCHENTRIES_PROC         (0x03U) /**< @brief #COMMS_AddSwitchEntries */
#define BRCM_SWARCH_COMMS_UPDATECFPCONFIG_PROC          (0x04U) /**< @brief #COMMS_UpdateCFPConfig */
#define BRCM_SWARCH_COMMS_ETHERPROCESSCOMMAND_PROC      (0x05U) /**< @brief #COMMS_EtherProcessCommand */
#define BRCM_SWARCH_COMMS_ETHERSWTPROCESSCOMMAND_PROC   (0x06U) /**< @brief #COMMS_EtherSwtProcessCommand */
#define BRCM_SWARCH_COMMS_ADDSTREAMS                    (0x07U) /**< @brief #COMMS_AddStreams */
#define BRCM_SWARCH_COMMS_SETSTREAMMONITORALARM_PROC    (0x08U) /**< @brief #COMMS_SetStreamMonitorAlarm */
#define BRCM_SWARCH_COMMS_MONITORSTREAMS_PROC           (0x09U) /**< @brief #COMMS_MonitorStreams */
/** @} */

void COMMS_InitXcvrStackingPorts(const MCU_ExtendedInfoType *const aStackInfo);

/** @brief API to process a ethernet driver command from HOST

    This command handler interface shall handle all the commands
    requested from the comms task for ethernet commands.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmdId      Command ID value
    @param[in]      aCmdStatus  Command processing status
    @param[inout]   aCmdData    Pointer to command payload
    @param[inout]   aLen        Payload size in bytes

    @return     BCM_ERR_OK              On success
    @return     BCM_ERR_INVAL_PARAMS    Invalid parameters
    @return     BCM_ERR_NOSUPPORT       Command not supported

    @post None

    @note To be implemented.

    @limitations None
*/
int32_t COMMS_EtherProcessCommand(ETHER_CmdIdType aCmdId,
                                  int32_t * const aCmdStatus,
                                  uint8_t * const aCmdData,
                                  uint32_t * const aLen);

/** @brief Send commands to ethernet component

    This API queues the commands to Component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Command information
    @param[in]      aLen                Length of the command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     aHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves (if exist)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command posted successfully
    @retval     #BCM_ERR_NOMEM          Queue is Full and cannot be added to queue
    @retval     #BCM_ERR_INVAL_PARAMS   aCmd is NULL or
                                        aHdr is NULL or
                                        aForwardToSlaves is NULL or
                                        aClientMask is zero or
                                        aCompID does not belong to this group

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherSendCmd(uint8_t aCmdID,
                           BCM_CompIDType aCompID,
                           uint8_t *aCmd, uint32_t aLen,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr,
                           uint32_t *aForwardToSlaves);

/** @brief Retreive status of queued commands from ethernet component

    This API queries the status of completed commands which were previously issued to
    component via COMMS_EtherSendCmd

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr        Pointer to the message queue header used to retrieve the status
    @param[out]     aCmdID      Pointer to the command id to be filled by Component
    @param[in]      aCompID     Component ID
    @param[out]     aReply      Pointer to reply message to be filled by Component
    @param[out]     aReplyLen   Pointer to length of reply message to be filled by Component

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_BUSY       Command is still being processed
    @retval     #BCM_ERR_INVAL_PARAMS   aHdr is NULL or
                                        aCmdID is NULL or
                                        aCompID is NULL or
                                        aReply is NULL or
                                        aReplyLen is NULL

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherCmdStatus(const MSGQ_MsgHdrType* const aHdr,
                             uint8_t *const aCmdID,
                             const BCM_CompIDType aCompID,
                             uint8_t *aReply,
                             uint32_t *aReplyLen);

/** @brief Merge results of ether component into one response to Host

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID          command id
    @param[in]      aCompID         component id
    @param[in]      aLen            length of response
    @param[out]     aOutResponse    Pointer to merged response (of aLen) to be filled by Component
    @param[out]     aOutResult      Pointer to merged result (int32_t) to be filled by Component
    @param[in]      aInResponse1    Pointer to first response message to be merged
    @param[in]      aInResult1      Result1 to be merged (of BCM Error types)
    @param[in]      aInResponse2    Pointer to second response message to be merged
    @param[in]      aInResult2      Result2 to be merged (of BCM Error types)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aOutResponse is NULL or
                                        aOutResult is NULL or
                                        aInResponse1 is NULL or
                                        aInResponse2 is NULL or
                                        aCompID is invalid

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherMergeResult(uint8_t aCmdID, BCM_CompIDType aCompID, uint32_t aLen,
                               uint8_t *aOutResponse,
                               int32_t *aOutResult,
                               const uint8_t *aInResponse1,
                               int32_t aInResult1,
                               const uint8_t *aInResponse2,
                               int32_t aInResult2);

/** @brief API to process a switch driver command from HOST

    This command handler interface shall handle all the commands
    requested from the comms task for switch driver commands.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmdId           Command ID value
    @param[out]     aCmdStatus       Command status
    @param[inout]   aCmdData         Pointer to payload where the input
                                     configuration or the output data
                                     is to be populated.
    @param[inout]   aLen             Payload size in bytes

    @return     BCM_ERR_OK           On success
    @return     BCM_ERR_NOMEM        In case the configuration structure memory
                                     exceeds the expected value
    @return     BCM_ERR_NOSUPPORT    In case of an unsupported command

    @post TBD

    @trace  #TBD

    @limitations None
*/
int32_t COMMS_EtherSwtProcessCommand(ETHERSWT_CmdIdType aCmdId,
                                     int32_t * const aCmdStatus,
                                     uint8_t * const aCmdData,
                                     uint32_t * const aLen);

/** @brief Send commands to switch driver

    This API queues the commands to Component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Command information
    @param[in]      aLen                Length of the command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     aHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves (if exist)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command posted successfully
    @retval     #BCM_ERR_NOMEM          Queue is Full and cannot be added to queue
    @retval     #BCM_ERR_INVAL_PARAMS   aCmd is NULL or
                                        aHdr is NULL or
                                        aForwardToSlaves is NULL or
                                        aClientMask is zero or
                                        aCompID does not belong to this group

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherSwtSendCmd(uint8_t aCmdID,
                              BCM_CompIDType aCompID,
                              uint8_t *aCmd, uint32_t aLen,
                              const uint32_t aClientMask,
                              const MSGQ_MsgHdrType** const aHdr,
                              uint32_t *aForwardToSlaves);

/** @brief Retreive status of queued commands for switch commands

    This API queries the status of completed commands which were previously issued to
    component via COMMS_EtherSwtSendCmd

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr        Pointer to the message queue header used to retrieve the status
    @param[out]     aCmdID      Pointer to the command id to be filled by Component
    @param[in]      aCompID     Component ID
    @param[out]     aReply      Pointer to reply message to be filled by Component
    @param[out]     aReplyLen   Pointer to length of reply message to be filled by Component

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_BUSY       Command is still being processed
    @retval     #BCM_ERR_INVAL_PARAMS   aHdr is NULL or
                                        aCmdID is NULL or
                                        aCompID is NULL or
                                        aReply is NULL or
                                        aReplyLen is NULL

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherSwtCmdStatus(const MSGQ_MsgHdrType* const aHdr,
                                uint8_t *const aCmdID,
                                const BCM_CompIDType aCompID,
                                uint8_t *aReply,
                                uint32_t *aReplyLen);


/** @brief Merge results from master/slaves into one response to Host
    for switch commands

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID          command id
    @param[in]      aCompID         component id
    @param[in]      aLen            length of response
    @param[out]     aOutResponse    Pointer to merged response (of aLen) to be filled by Component
    @param[out]     aOutResult      Pointer to merged result (int32_t) to be filled by Component
    @param[in]      aInResponse1    Pointer to first response message to be merged
    @param[in]      aInResult1      Result1 to be merged (of BCM Error types)
    @param[in]      aInResponse2    Pointer to second response message to be merged
    @param[in]      aInResult2      Result2 to be merged (of BCM Error types)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aOutResponse is NULL or
                                        aOutResult is NULL or
                                        aInResponse1 is NULL or
                                        aInResponse2 is NULL or
                                        aCompID is invalid

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_EtherSwtMergeResult(uint8_t aCmdID, BCM_CompIDType aCompID, uint32_t aLen,
                                  uint8_t *aOutResponse,
                                  int32_t *aOutResult,
                                  const uint8_t *aInResponse1,
                                  int32_t aInResult1,
                                  const uint8_t *aInResponse2,
                                  int32_t aInResult2);

void COMMS_EtherSwtARLRefresh();

#ifdef ENABLE_ETH_SWITCH
/** @brief Get updated switch config
    Get updated switch config after conversion of unified ports/masks to
    local ports/masks

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aStackInfo          MCU extended info containing stacking info
    @param[in]      aCommsCfg           Communication Subsystem configuration
    @param[inout]   aSwitchCfg          Switch config to be updated

    @retval         void

    @post None
*/
void COMMS_GetSwitchConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                           const COMMS_ConfigType *const aCommsCfg,
                           ETHERSWT_CfgType *const aSwitchCfg);

/** @brief Add VLAN & ARL entries

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aStackInfo          MCU extended info containing stacking info
    @param[in]      aCommsCfg           Communication Subsystem configuration

    @retval         void

    @post None
*/
void COMMS_AddSwitchEntries(const MCU_ExtendedInfoType *const aStackingInfo,
                           const COMMS_ConfigType *const aCommsCfg);
#ifdef ENABLE_CFP
/** @brief Add streams

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aStackInfo          MCU extended info containing stacking info
    @param[in]      aCommsCfg           Communication Subsystem configuration

    @retval         Monitoring interval in system ticks

    @post None
*/
void COMMS_AddStreams(const MCU_ExtendedInfoType *const aStackInfo,
                      const COMMS_ConfigType *const aCommsCfg);

/** @brief Set alarm monitoring interval for policed streams

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCommsCfg           Communication Subsystem configuration

    @retval         void

    @post None
*/
void COMMS_SetStreamMonitorAlarm(const COMMS_ConfigType *const aCommsCfg);

/** @brief Monitor policed streams
    Poll the statistics for the policed streams and take block/report actions as configured

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCommsCfg           Communication Subsystem configuration

    @retval         void

    @post None
*/
void COMMS_MonitorStreams(const COMMS_ConfigType *const aCommsCfg);

/** @brief Get updated CFP config
    Get updated CFP config after conversion of unified ports/masks to
    local ports/masks

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aStackInfo          MCU extended info containing stacking info
    @param[inout]   aCfg                CFP configuration to be updated

    @retval         void

    @post None
*/
void COMMS_UpdateCFPConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                           CFP_ConfigType *const aCfg);
#endif
#endif

#endif /* COMMS_PLATFORM_H */

/** @} */
