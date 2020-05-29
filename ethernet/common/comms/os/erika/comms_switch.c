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
    @defgroup grp_comms_switch Communication Subsystem platform layer for switches
    @ingroup grp_comms_sw

    @addtogroup grp_comms_switch
    @{

    @file comms_switch.c
    @brief This file implements switch specific functionality for the Communication Subsystem

    @version 0.1 Initial draft
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <bcm_osil.h>
#include <utils.h>
#include <bcm_utils.h>
#include <rpc_cmd_queue.h>
#include <rpc_async.h>
#include "switch_cfg.h"
#include "comms.h"
#include "comms_platform.h"
#include <comms_ipc.h>
#include <comms_osil.h>
#include "comms_cfg.h"
#include "mcu_ext.h"

/**
    @name Communication Subsystem switch Design IDs
    @{
    @brief Design IDs for Communication Subsystem switch layer
*/
#define BRCM_SWDSGN_COMMS_GETSWITCHCONFIG_PROC            (0x91U) /**< @brief #COMMS_GetSwitchConfig */
#define BRCM_SWDSGN_COMMS_ADDSWITCHENTRIES_PROC           (0x92U) /**< @brief #COMMS_AddSwitchEntries */
#define BRCM_SWDSGN_COMMS_ADDARLENTRIES_PART_PROC         (0x93U) /**< @brief #COMMS_AddARLEntries */
#define BRCM_SWDSGN_COMMS_ADDVLANENTRIES_PART_PROC        (0x94U) /**< @brief #COMMS_AddVLANEntries */
#define BRCM_SWDSGN_COMMS_UPDATESWITCHPORT_PART_PROC      (0x95U) /**< @brief #COMMS_UpdateSwitchPort */
#define BRCM_SWDSGN_COMMS_GETLOCALPORTMASK_PART_PROC      (0x96U) /**< @brief #COMMS_GetLocalPortMask */
#define BRCM_SWDSGN_COMMS_UPDATECFPCONFIG_PROC            (0x97U) /**< @brief #COMMS_UpdateCFPConfig */
#define BRCM_SWDSGN_COMMS_UPDATEDESTMAPFLAG_PART_PROC     (0x98U) /**< @brief #COMMS_UpdateDestMapFlag */
#define BRCM_SWDSGN_COMMS_UPDATEETSCONFIG_PROC            (0x99U) /**< @brief #COMMS_UpdateETSConfig */
#define BRCM_SWDSGN_COMMS_ETHERSWTPROCESSCOMMAND_PROC     (0x9AU) /**< @brief #COMMS_EtherSwtProcessCommand */
#define BRCM_SWDSGN_COMMS_ETHERSWT_SENDCMD_PROC           (0x9BU) /**< @brief #COMMS_EtherSwtSendCmd */
#define BRCM_SWDSGN_COMMS_ETHERSWT_CMDSTATUS_PROC         (0x9CU) /**< @brief #COMMS_EtherSwtCmdStatus */
#define BRCM_SWDSGN_COMMS_ETHERSWT_MERGERESULT_PROC       (0x9DU) /**< @brief #COMMS_EtherSwtMergeResult */
#define BRCM_SWDSGN_COMMS_ETHERSWT_ARLREFRESH_PROC        (0x9EU) /**< @brief #COMMS_EtherSwtARLRefresh */
#define BRCM_SWDSGN_COMMS_ADDSTREAMS_PROC                 (0x9FU) /**< @brief #COMMS_AddStreams */
#define BRCM_SWDSGN_COMMS_ADDSTREAM_PART_PROC             (0xA0U) /**< @brief #COMMS_AddStream */
#define BRCM_SWDSGN_COMMS_MONITORSTREAMS_PROC             (0xA2U) /**< @brief #COMMS_MonitorStreams */
#define BRCM_SWDSGN_COMMS_ADDSTREAMPOLICER_PART_PROC      (0xA3U) /**< @brief #COMMS_AddStreamPolicer */
#define BRCM_SWDSGN_COMMS_DELSTREAMPOLICER_PART_PROC      (0xA4U) /**< @brief #COMMS_DelStreamPolicer */
#define BRCM_SWDSGN_COMMS_BLOCKSTREAM_PART_PROC           (0xA5U) /**< @brief #COMMS_BlockStream */
#define BRCM_SWDSGN_COMMS_RESUMESTREAM_PART_PROC          (0xA6U) /**< @brief #COMMS_ResumeStream */
#define BRCM_SWDSGN_COMMS_ADDARLENTRY_PART_PROC           (0xA7U) /**< @brief #COMMS_AddARLEntry */
#define BRCM_SWDSGN_COMMS_DELARLENTRY_PART_PROC           (0xA8U) /**< @brief #COMMS_DelARLEntry */
#define BRCM_SWDSGN_COMMS_FINDSTREAMPOLICERIDX_PART_PROC  (0xA9U) /**< @brief #COMMS_FindStreamPolicerIdx */

/** @} */

/**
    @brief State machine for ARL refresh at master
*/
typedef uint32_t COMMS_ARLStateType;
#define COMMS_ARLSTATE_RESET        (0UL)   /**< @brief Reset state */
#define COMMS_ARLSTATE_SNAPSHOT     (1UL)   /**< @brief Get the ARL snapshot state */
#define COMMS_ARLSTATE_GET_ARL      (2UL)   /**< @brief Get entire ARL list state */
#define COMMS_ARLSTATE_MERGE        (3UL)   /**< @brief ARL merge state */

/**
    @brief Memory to store the retrieved ARL entries from switch driver
*/
#define COMMS_ARL_ENTRY_LIST_MAX  (1000UL)
static ETHERSWT_ARLEntryType COMMS_EtherSwtARLEntries[COMMS_ARL_ENTRY_LIST_MAX];
static ETHERSWT_ARLEntryType COMMS_EtherSwtStackingARLEntries[COMMS_ARL_ENTRY_LIST_MAX];
#define COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK (sizeof(uint32_t) * 8UL)
#define COMMS_ETHERSWT_MERGED_ENTRYMASK_MAX ((COMMS_ARL_ENTRY_LIST_MAX + COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK) / COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK)

/**
    @brief Read Index to ARL entry list
*/
#define COMMS_ETHERSWT_ARL_MATCH_INVALID (0xFFFFFFFFUL)

/**
    @brief Internal structure to manage ARL refresh
*/
static struct COMMS_ETHERSWT_RefreshARLType {
    COMMS_ARLStateType     ARLState;         /* Current ARL state */
    uint16_t               ARLLen;           /* Number of ARL entries at master */
    uint16_t               slaveARLLen;      /* Number of ARL entries at slave */
    const MSGQ_MsgHdrType* pHdrSlave;        /* Used to manage Msg sent from master to slave */
    uint16_t               slaveARLProcLen;  /* Num of slave ARLs that is fetched */
    uint32_t               slaveEntryMerged[COMMS_ETHERSWT_MERGED_ENTRYMASK_MAX];
                                             /* Bit map of entries that are merged into master */
    uint8_t                payload[RPC_RESPPAYLOADSZ]; /* Payload memory */
} COMMS_ETHERSWT_RefreshARL;

/**
    @brief Per stream policer data
*/
typedef struct _COMMS_StreamPolicerDataType {
    uint16_t row;
    uint16_t blocked;
    uint32_t tickCount;
} COMMS_StreamPolicerDataType;

/**
    @brief Handle to manage streams
*/
static struct COMMS_StreamDataType {
    uint32_t                    currentIntervalTicks;
    int32_t                     currentSlice;
    COMMS_StreamPolicerDataType streams[COMMS_MAX_STREAM_POLICER_ENTRIES];
} COMMS_StreamData = {
    .currentIntervalTicks = 0UL,
    .currentSlice = (int32_t)CFP_MAX_SLICES - 1L,
    .streams = {{CFP_MAX_RULES, FALSE, 0UL}}};

extern COMMS_ConfigType CommsConfig;

static void COMMS_ErrorReport(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                              uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_COM_ID & BCM_LOGMASK_USER), 0U, aApiID,
                    aErr, 4UL, values);
}

static uint16_t COMMS_GetLocalPortMask(const MCU_ExtendedInfoType *const aStackingInfo,
                                       uint16_t aInputMask)
{
    uint32_t port = 0UL;
    uint32_t isStackingPort = FALSE;
    uint16_t localPort;
    uint16_t portMask = 0U;

    for (; port < COMMS_MAX_PORTS; ++port) {
        if (0UL != ((1UL << port) & aInputMask)) {
            localPort = COMMS_ConvertToLocalPort(aStackingInfo, port, &isStackingPort);
            if (COMMS_INVALID_PORT != localPort) {
                portMask |= (1U << localPort);
            }
        }
    }
    return portMask;
}

static int32_t COMMS_AddStream(const MCU_ExtendedInfoType *const aStackInfo,
                               const COMMS_ConfigType *const aCommsCfg,
                               uint32_t aStreamIndex,
                               int32_t *const aCurrentSlice)
{
    CFP_KeyType              key;
    CFP_ActionType           action;
    int32_t                  retVal = BCM_ERR_OK;
    const COMMS_StreamPolicerConfigType *streamCfg = &aCommsCfg->streamPolicers[aStreamIndex];
    const COMMS_ARLEntryType *arlEntry = &aCommsCfg->arlTable[streamCfg->arlEntryIndex];
    uint32_t                 row;
    uint32_t                 slice;

    BCM_MemSet(&key, 0U, sizeof(key));
    BCM_MemSet(&action, 0U, sizeof(action));
    key.l2Framing = CFP_L2FRAMING_DIXV2;
    key.l3Framing = CFP_L3FRAMING_NONIP;
    key.cTagFlags = ((arlEntry->vlan << CFP_KEY_TAG_ID_SHIFT) & CFP_KEY_TAG_ID_MASK) |
        CFP_KEY_TAG_ID_VALID_MASK | CFP_KEY_TAG_VLAN_TAGGED_MASK;
    key.cTagMask  = CFP_KEY_TAG_IDMASK_MASK;
    key.sTagFlags = CFP_KEY_TAG_UN_TAGGED_MASK;
    key.ingressPortBitmap = COMMS_GetLocalPortMask(aStackInfo, streamCfg->portMask);
    key.numEnabledUDFs = 3U;
    key.udfList[0U].value = arlEntry->macAddress[1U] | (arlEntry->macAddress[0U] << 8U);
    key.udfList[0U].mask  = 0xFFFFU;
    key.udfList[0U].baseAndOffset = CFP_UDFBASE_SOP << CFP_UDF_BASE_SHIFT;
    key.udfList[1U].value = arlEntry->macAddress[3U] | (arlEntry->macAddress[2U] << 8U);
    key.udfList[1U].mask  = 0xFFFFU;
    key.udfList[1U].baseAndOffset = (CFP_UDFBASE_SOP << CFP_UDF_BASE_SHIFT) |
        ((1U << CFP_UDF_OFFSET_SHIFT) & CFP_UDF_OFFSET_MASK);
    key.udfList[2U].value = arlEntry->macAddress[5U] | (arlEntry->macAddress[4U] << 8U);
    key.udfList[2U].mask  = 0xFFFFU;
    key.udfList[2U].baseAndOffset = (CFP_UDFBASE_SOP << CFP_UDF_BASE_SHIFT) |
        ((2U << CFP_UDF_OFFSET_SHIFT) & CFP_UDF_OFFSET_MASK);
    action.meter.cirRefCnt    = streamCfg->rate / 4U;
    action.meter.cirTkBkt     = streamCfg->burstSize * 8U;
    action.meter.cirBktSize   = streamCfg->burstSize;
    action.meter.policerFlags = CFP_METER_CM_MASK | (CFP_POLICERMODE_MEF << CFP_METER_MODE_SHIFT);
    action.dstMapIBFlags      = (CFP_CHANGEFWDMAP_NON << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
    action.dstMapOBFlags      = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
    action.otherFlags         = CFP_ACTION_BYPASS_VLAN_MASK |
                                CFP_ACTION_BYPASS_EAP_MASK  |
                                CFP_ACTION_BYPASS_STP_MASK;
    while (*aCurrentSlice >= 0L) {
        row    = CFP_MAX_RULES;
        slice  = (uint32_t)*aCurrentSlice;
        retVal = CFP_AddRule(ETH_SWITCH_HW_ID, &key, &action, &row, &slice);
        if (BCM_ERR_NOMEM == retVal) {
            *aCurrentSlice  -= 1L;
        } else if (BCM_ERR_OK != retVal) {
            COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ADDSTREAM_PART_PROC, retVal,
                             (uint32_t)__LINE__, aStreamIndex, *aCurrentSlice, 0UL);
            break;
        } else {
            COMMS_StreamData.streams[aStreamIndex].row = row;
            break;
        }
    }

    return retVal;
}

void COMMS_AddStreams(const MCU_ExtendedInfoType *const aStackInfo,
                      const COMMS_ConfigType *const aCommsCfg)
{
    int32_t   retVal;
    uint32_t  i = 0UL;

    for (; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
        COMMS_StreamData.streams[i].row = CFP_MAX_RULES;
    }

    for (i = 0UL; i < aCommsCfg->numStreamPolicers; ++i) {
        retVal = COMMS_AddStream(aStackInfo, aCommsCfg, i, &COMMS_StreamData.currentSlice);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }
}

static void COMMS_AddARLEntries(const MCU_ExtendedInfoType *const aStackInfo,
                                const COMMS_ConfigType *const aCommsCfg)
{
    uint32_t              entry  = 0UL;
    int32_t               ret;
    ETHERSWT_ARLEntryType arlEntry;

    /* Iterate over all ARL entries */
    for (; entry < aCommsCfg->arlTblSz; ++entry) {

        arlEntry.vlanID = aCommsCfg->arlTable[entry].vlan;
        BCM_MemCpy(&arlEntry.macAddr, aCommsCfg->arlTable[entry].macAddress, 6U);
        arlEntry.portMask = 0U;

        arlEntry.portMask = COMMS_GetLocalPortMask(aStackInfo, aCommsCfg->arlTable[entry].portMask);
        ret = ETHERSWT_AddARLEntry(0UL, &arlEntry);
        if (BCM_ERR_OK != ret) {
            COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ADDARLENTRIES_PART_PROC, ret,
                             (uint32_t)__LINE__, entry, 0UL, 0UL);
            break;
        }
    }
}

static void COMMS_AddVLANEntries(const MCU_ExtendedInfoType *const aStackInfo,
                                 const COMMS_ConfigType *const aCommsCfg)
{
    uint32_t entry   = 0UL;
    int32_t  ret;

    /* Iterate over all VLAN entries */
    for (; entry < aCommsCfg->vidTblSz; entry++) {
        uint32_t portMask = COMMS_GetLocalPortMask(aStackInfo, aCommsCfg->vidTable[entry].portMask);
        uint32_t tagMask =  COMMS_GetLocalPortMask(aStackInfo, aCommsCfg->vidTable[entry].tagMask);

        ret = ETHERSWT_AddVLANPorts(0UL, portMask, aCommsCfg->vidTable[entry].vlan, tagMask);
        if (BCM_ERR_OK != ret) {
            COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ADDVLANENTRIES_PART_PROC, ret,
                             (uint32_t)__LINE__, entry, aCommsCfg->vidTable[entry].vlan, 0UL);
            break;
        }
    }
}

void COMMS_AddSwitchEntries(const MCU_ExtendedInfoType *const aStackingInfo,
                            const COMMS_ConfigType *const aCommsCfg)
{
    COMMS_AddVLANEntries(aStackingInfo, aCommsCfg);
    COMMS_AddARLEntries(aStackingInfo, aCommsCfg);
}

static void COMMS_UpdateSwitchPort(ETHERSWT_CfgType *const aSwitchCfg,
                                   uint8_t aStackPort,
                                   uint32_t *const aPortCnt)
{
    uint32_t             portNum = *aPortCnt;
    ETHERSWT_PortCfgType *port = &aSwitchCfg->portCfgList[portNum];
    uint32_t             i;

    if (aStackPort != 0U) {
        port->portID = aStackPort;
        port->xcvrID = aStackPort;
        port->role   = ETHERSWT_UP_LINK_PORT;
        port->enableTimeStamp = TRUE;
        port->enableJumboFrm  = TRUE;

        port->ingressCfg.defaultVLAN = 1U;
        port->ingressCfg.defaultPrio = ETHERSWT_PCP_0;
        port->ingressCfg.dropUntagged = FALSE;
        port->ingressCfg.tc = ETHERSWT_TC_INVALID;
        port->ingressCfg.policerEn = FALSE;
        for (i = 0UL; i < 8UL; ++i) {
            port->ingressCfg.pcp2tcMap[i] = ETHERSWT_TC_0 + i;
        }

        port->egressCfg.pcpRemarkEn = FALSE;
        for (i = 0UL; i < ETHERSWT_PORT_FIFO_MAX; ++i) {
            port->egressCfg.scheduler.algo[i] = ETHERSWT_SCHED_ALGO_SP;
            port->egressCfg.fifoList[i].id = i;
            port->egressCfg.fifoList[i].tc = ETHERSWT_TC_0 + i;
        }
        port->egressCfg.fifoListSz = i;

        *aPortCnt = ++portNum;
    }
}

void COMMS_GetSwitchConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                           const COMMS_ConfigType *const aCommsCfg,
                           ETHERSWT_CfgType *const aSwitchCfg)
{
    uint32_t port    = 0UL;
    uint32_t portCnt = 0UL;

    BCM_MemSet(aSwitchCfg, 0U, sizeof(ETHERSWT_CfgType));

    for (; port < aCommsCfg->portCfgListSz; ++port) {
        uint32_t isStackingPort = FALSE;
        uint32_t localPortNum = COMMS_ConvertToLocalPort(aStackingInfo,
                                                         aCommsCfg->portCfgList[port].portID,
                                                         &isStackingPort);
        if ((COMMS_INVALID_PORT != localPortNum) && (FALSE == isStackingPort)) {
            ETHERSWT_PortCfgType *destPort = &aSwitchCfg->portCfgList[portCnt];
            const COMMS_SwitchPortConfigType *srcPort = &aCommsCfg->portCfgList[port].switchPortCfg;

            destPort->portID = localPortNum;
            destPort->xcvrID = localPortNum;
            destPort->role   = srcPort->role;
            destPort->enableTimeStamp  = srcPort->enableTimeStamp;
            destPort->enableJumboFrm  = srcPort->enableJumboFrm;
            BCM_MemCpy(&destPort->ingressCfg,
                       &srcPort->ingressCfg,
                       sizeof(ETHERSWT_PortIngressCfgType));
            BCM_MemCpy(&destPort->egressCfg,
                       &srcPort->egressCfg,
                       sizeof(ETHERSWT_PortEgressCfgType));
            portCnt++;
        }
    }

    aSwitchCfg->switchType = aCommsCfg->switchType;
    if (aStackingInfo->stackingEn == 1U) {
        switch (aStackingInfo->mstSlvMode) {
            case MCU_DEVICE_MASTER:
                aSwitchCfg->switchType = ETHERSWT_SWITCH_LIGHTSTACK_MASTER;
                break;
            case MCU_DEVICE_SLAVE_1:
            case MCU_DEVICE_SLAVE_2:
                aSwitchCfg->switchType = ETHERSWT_SWITCH_LIGHTSTACK_SLAVE;
                break;
            default:
                break;
        }
        COMMS_UpdateSwitchPort(aSwitchCfg, aStackingInfo->stackPort0, &portCnt);
        COMMS_UpdateSwitchPort(aSwitchCfg, aStackingInfo->stackPort1, &portCnt);
    }
    aSwitchCfg->portCfgListSz = portCnt;
}

#ifdef ENABLE_CFP
static void COMMS_UpdateDestMapFlag(const MCU_ExtendedInfoType *const aStackingInfo,
                                    uint32_t *const aFlags,
                                    uint32_t aUnifiedToLocal)
{
    uint32_t port = 0UL;
    uint32_t isStackingPort = FALSE;
    uint16_t localPort;
    uint16_t unifiedPort;
    uint32_t portMask = 0U;
    uint32_t destMap = (*aFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT;
    uint32_t chngFwdMap = (*aFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >> CFP_ACTION_CHANGE_FWDMAP_SHIFT;

    if (CFP_CHANGEFWDMAP_NON != chngFwdMap) {

        /* CFP destination map flag is different from other portmasks because bit 7 is treated */
        /* as a special bit representing the ARM CPU port. This bit is exposed to the user     */
        for (; port < COMMS_MAX_PORTS; ++port) {
            if (0UL != ((1UL << port) & destMap)) {
                if (port == 7UL) {
                    portMask |= (1U << port);
                } else {
                    if (TRUE == aUnifiedToLocal) {
                        /* Convert unified port to local port. Make sure the port is valid */
                        localPort = COMMS_ConvertToLocalPort(aStackingInfo, port, &isStackingPort);
                        if (COMMS_INVALID_PORT != localPort) {
                            portMask |= (1U << localPort);
                        }
                    } else {
                        /* Convert local port to unified port. Remove any stacking ports */
                        unifiedPort = COMMS_ConvertToUnifiedPort(aStackingInfo, port, &isStackingPort);
                        if ((COMMS_INVALID_PORT != unifiedPort) && (FALSE == isStackingPort)) {
                            portMask |= (1U << unifiedPort);
                        }
                    }
                }
            }
        }

        *aFlags = ((portMask << CFP_ACTION_DSTMAP_SHIFT) & CFP_ACTION_DSTMAP_MASK) |
            ((chngFwdMap << CFP_ACTION_CHANGE_FWDMAP_SHIFT) & CFP_ACTION_CHANGE_FWDMAP_MASK);
    }
}

void COMMS_UpdateCFPConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                           CFP_ConfigType *const aCfg)
{
    uint16_t rule = 0UL;

    aCfg->portEnableMask = COMMS_GetLocalPortMask(aStackingInfo, aCfg->portEnableMask);

    for (; rule < aCfg->ruleListSz; ++rule) {
        aCfg->ruleList[rule].key.ingressPortBitmap = COMMS_GetLocalPortMask(aStackingInfo,
                                                                         aCfg->ruleList[rule].key.ingressPortBitmap);

        COMMS_UpdateDestMapFlag(aStackingInfo, &aCfg->ruleList[rule].action.dstMapIBFlags, TRUE);
        COMMS_UpdateDestMapFlag(aStackingInfo, &aCfg->ruleList[rule].action.dstMapOBFlags, TRUE);
    }
}
#endif

#ifdef ENABLE_ETS
void COMMS_UpdateETSConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                           ETS_ConfigType *const aCfg,
                           uint32_t aUnifiedToLocal)
{
    ETS_ConfigType scratch;
    uint32_t       localPort;
    uint16_t       unifiedPort;
    uint32_t       i                  = 0UL;
    uint32_t       isStackingPort     = FALSE;
    uint32_t       portCnt            = 0UL;
    uint32_t       stackingPortSlave  = FALSE;

    BCM_MemCpy(&scratch, aCfg, sizeof(ETS_ConfigType));
    BCM_MemSet(scratch.intfCfg, 0U, sizeof(scratch.intfCfg));

    for (; i < aCfg->numInterfaces; ++i) {
        if (TRUE == aUnifiedToLocal) {
            localPort = COMMS_ConvertToLocalPort(aStackingInfo,
                                                 aCfg->intfCfg[i].hwPortNum,
                                                 &isStackingPort);
            if ((COMMS_INVALID_PORT != localPort) && (FALSE == isStackingPort))  {
                BCM_MemCpy(&scratch.intfCfg[portCnt],
                        &aCfg->intfCfg[i],
                        sizeof(ETS_IntfConfigType));
                scratch.intfCfg[portCnt].hwPortNum = (uint8_t)localPort;
                portCnt++;
            } else if (aCfg->intfCfg[i].role == ETS_ROLE_SLAVE) {
                stackingPortSlave = TRUE;
            }
        } else {
            unifiedPort = COMMS_ConvertToUnifiedPort(aStackingInfo,
                                                     aCfg->intfCfg[i].hwPortNum,
                                                     &isStackingPort);
            if ((COMMS_INVALID_PORT != unifiedPort) && (FALSE == isStackingPort)) {
                BCM_MemCpy(&scratch.intfCfg[portCnt],
                        &aCfg->intfCfg[i],
                        sizeof(ETS_IntfConfigType));
                scratch.intfCfg[portCnt].hwPortNum = unifiedPort;
                portCnt++;
            }
        }
    }

    /* Add the stacking/internal ports */
    if ((TRUE == aUnifiedToLocal) &&
            (1U == aStackingInfo->stackingEn) &&
            (aCfg->clockMode == ETS_CLOCKMODE_SLAVE)) {
        ETS_IntfConfigType intfCfg = {
            .hwPortNum = aStackingInfo->stackPort0,
            .asCapable = ETS_ADMINMODE_ENABLE,
            .role = (stackingPortSlave == TRUE) ? ETS_ROLE_SLAVE:ETS_ROLE_MASTER,
            .initLogPdelayInterval = ETS_INITLOGPDELAY_1S,
            .operLogPdelayInterval = ETS_OPERLOGPDELAY_1S,
            .initLogSyncInterval = ETS_INITLOGSYNC_125MS,
            .operLogSyncInterval = ETS_OPERLOGSYNC_1S,
            .nbrPropDelay = 300UL,
            .nbrRateRatio = 0x40000000,
            .syncReceiptTimeout = 3UL,
            .allowedLostResponses = 3UL
        };
        BCM_MemCpy(&scratch.intfCfg[portCnt++], &intfCfg, sizeof(ETS_IntfConfigType));
    }
    scratch.numInterfaces = portCnt;
    BCM_MemCpy(aCfg, &scratch, sizeof(ETS_ConfigType));
}
#endif

static int32_t COMMS_AddARLEntry(const ETHERSWT_ARLEntryType *const aEntry)
{
    int32_t  ret                = BCM_ERR_OK;
    uint32_t i                  = 0UL;
    uint32_t emptySlot          = COMMS_MAX_ARL_ENTRIES;
    uint8_t  zeroMacAddress[6U] = {0U};

    /* Check if this entry exists */
    for (; i < COMMS_MAX_ARL_ENTRIES; ++i) {
        if ((CommsConfig.arlTable[i].vlan == aEntry->vlanID) &&
            (0UL == memcmp(aEntry->macAddr, CommsConfig.arlTable[i].macAddress, 6U))) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
        if ((emptySlot == COMMS_MAX_ARL_ENTRIES) &&
            (CommsConfig.arlTable[i].vlan == 0U) &&
            (0UL == memcmp(&zeroMacAddress, CommsConfig.arlTable[i].macAddress, 6U))) {
            emptySlot = i;
        }
    }
    if (BCM_ERR_OK == ret) {
        if (COMMS_MAX_ARL_ENTRIES == emptySlot) {
            ret = BCM_ERR_NOMEM;
        } else {
            BCM_MemCpy(&CommsConfig.arlTable[emptySlot], aEntry, sizeof(COMMS_ARLEntryType));
        }
    }

    return ret;
}

static int32_t COMMS_DelARLEntry(const ETHERSWT_ARLEntryType *const aEntry)
{
    int32_t  ret = BCM_ERR_NOT_FOUND;
    uint32_t i   = 0UL;
    uint32_t j   = 0UL;

    /* Check if this entry exists */
    for (; i < COMMS_MAX_ARL_ENTRIES; ++i) {
        if ((CommsConfig.arlTable[i].vlan == aEntry->vlanID) &&
            (0UL == memcmp(aEntry->macAddr, CommsConfig.arlTable[i].macAddress, 6U))) {
            if (i < CommsConfig.arlTblSz) {
                ret = BCM_ERR_NOPERM;
            } else {
                /* Check if any stream policers are associated */
                for (; j < COMMS_MAX_STREAM_POLICER_ENTRIES; ++j) {
                    if ((CommsConfig.streamPolicers[j].arlEntryIndex == i) &&
                        (COMMS_StreamData.streams[i].row != CFP_MAX_RULES)) {
                        ret = BCM_ERR_NOPERM;
                        break;
                    }
                }
                if (j == COMMS_MAX_STREAM_POLICER_ENTRIES) {
                    BCM_MemSet(&CommsConfig.arlTable[i], 0U, sizeof(COMMS_ARLEntryType));
                    ret = BCM_ERR_OK;
                }
            }
            break;
        }

    }

    return ret;
}

static int32_t COMMS_AddStreamPolicer(const uint8_t *const aMacAddr,
                                      uint16_t aVlan,
                                      const COMMS_StreamPolicerConfigType* const aCfg,
                                      uint32_t *const aStreamIdx)
{
    int32_t               retVal       = BCM_ERR_OK;
    uint32_t              i            = 0UL;
    uint32_t              j            = 0UL;
    uint32_t              found        = FALSE;
    uint32_t              emptySlot    = COMMS_MAX_STREAM_POLICER_ENTRIES;
    MCU_ExtendedInfoType  stackingInfo = {0U};

    if ((NULL == aMacAddr) || (NULL == aCfg) || (NULL == aStreamIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {

        retVal = MCU_GetExtendedInfo(&stackingInfo);
        if (BCM_ERR_OK != retVal) {
            COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ADDSTREAMPOLICER_PART_PROC, retVal,
                    (uint32_t)__LINE__, 0UL, 0UL, 0UL);
        } else {
            /* Find the ARL */
            for (; i < COMMS_MAX_ARL_ENTRIES; ++i) {
                if ((CommsConfig.arlTable[i].vlan == aVlan) &&
                        (0UL == memcmp(aMacAddr, CommsConfig.arlTable[i].macAddress, 6U))) {
                    found = TRUE;
                    break;
                }
            }
            if (TRUE == found) {
                /* Find place to store the policer config. Also check that a duplicate does not exist */
                found = FALSE;
                for (; j < COMMS_MAX_STREAM_POLICER_ENTRIES; ++j) {
                    if (COMMS_StreamData.streams[j].row == CFP_MAX_RULES) {
                        if (emptySlot == COMMS_MAX_STREAM_POLICER_ENTRIES) {
                            emptySlot = j;
                            found = TRUE;
                        }
                    } else if ((CommsConfig.streamPolicers[j].arlEntryIndex == i) &&
                               (0UL != (aCfg->portMask & CommsConfig.streamPolicers[j].portMask))) {
                        retVal = BCM_ERR_INVAL_PARAMS;
                        break;
                    }
                }
                if ((TRUE == found) && (BCM_ERR_OK == retVal)) {
                    BCM_MemCpy(&CommsConfig.streamPolicers[emptySlot], aCfg, sizeof(COMMS_StreamPolicerConfigType));
                    CommsConfig.streamPolicers[emptySlot].arlEntryIndex = i;
                    retVal = COMMS_AddStream(&stackingInfo, &CommsConfig, emptySlot, &COMMS_StreamData.currentSlice);
                    *aStreamIdx = emptySlot;
                    COMMS_SetStreamMonitorAlarm(&CommsConfig);
                }
            }
        }
    }
    if (FALSE == found) {
        retVal = BCM_ERR_NOMEM;
    }

    return retVal;
}

static int32_t COMMS_DelStreamPolicer(uint32_t aStreamIdx)
{
    int32_t retVal = BCM_ERR_OK;

    if (aStreamIdx >= COMMS_MAX_STREAM_POLICER_ENTRIES) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aStreamIdx < CommsConfig.numStreamPolicers) {
        retVal = BCM_ERR_NOPERM;
    } else if (COMMS_StreamData.streams[aStreamIdx].row == CFP_MAX_RULES) {
        retVal = BCM_ERR_NOT_FOUND;
    } else {
        retVal = CFP_RemoveRule(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[aStreamIdx].row);
        if (BCM_ERR_OK == retVal) {
            BCM_MemSet(&CommsConfig.streamPolicers[aStreamIdx], 0U, sizeof(COMMS_StreamPolicerConfigType));
            COMMS_StreamData.streams[aStreamIdx].row       = CFP_MAX_RULES;
            COMMS_StreamData.streams[aStreamIdx].blocked   = FALSE;
            COMMS_StreamData.streams[aStreamIdx].tickCount = 0UL;
            COMMS_SetStreamMonitorAlarm(&CommsConfig);
        }
    }

    return retVal;
}

static int32_t COMMS_FindStreamPolicerIdx(const uint8_t *const aMacAddr,
                                          uint16_t aVlan,
                                          uint32_t aPortMask,
                                          uint32_t *const aStreamIdx)
{
    int32_t  retVal = BCM_ERR_OK;
    uint32_t i      = 0UL;
    uint32_t j      = 0UL;
    uint32_t found  = FALSE;

    if ((NULL == aMacAddr) || (NULL == aStreamIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        /* Find the ARL */
        for (; i < COMMS_MAX_ARL_ENTRIES; ++i) {
            if ((CommsConfig.arlTable[i].vlan == aVlan) &&
                    (0UL == memcmp(aMacAddr, CommsConfig.arlTable[i].macAddress, 6U))) {
                found = TRUE;
                break;
            }
        }
        if (TRUE == found) {
            found = FALSE;
            /* Find corresponding stream policer with the same portMask */
            for (; j < COMMS_MAX_STREAM_POLICER_ENTRIES; ++j) {
                if ((COMMS_StreamData.streams[j].row != CFP_MAX_RULES) &&
                        (CommsConfig.streamPolicers[j].arlEntryIndex == i) &&
                        (CommsConfig.streamPolicers[j].portMask == aPortMask)) {
                    found = TRUE;
                    *aStreamIdx = j;
                    break;
                }
            }
        }
    }
    if (FALSE == found) {
        retVal = BCM_ERR_NOT_FOUND;
    }

    return retVal;
}

static int32_t COMMS_GetStreamPolicerStatus(COMMS_StreamPolicerStatusType *const aStatus)
{
    int32_t       retVal = BCM_ERR_OK;
    CFP_StatsType stats;

    if (NULL == aStatus) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aStatus->idx >= COMMS_MAX_STREAM_POLICER_ENTRIES) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (COMMS_StreamData.streams[aStatus->idx].row == CFP_MAX_RULES) {
        retVal = BCM_ERR_NOT_FOUND;
    } else {
        uint32_t arlEntryIndex = CommsConfig.streamPolicers[aStatus->idx].arlEntryIndex;
        BCM_MemCpy(aStatus->macAddress, CommsConfig.arlTable[arlEntryIndex].macAddress, 6U);
        aStatus->vlan     = CommsConfig.arlTable[arlEntryIndex].vlan;
        aStatus->portMask = CommsConfig.streamPolicers[aStatus->idx].portMask;
        aStatus->blocked  = COMMS_StreamData.streams[aStatus->idx].blocked;
        if (aStatus->idx < CommsConfig.numStreamPolicers) {
            aStatus->isStatic = TRUE;
        }
        retVal = CFP_GetStats(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[aStatus->idx].row, &stats);
        if (BCM_ERR_OK == retVal) {
            aStatus->redCntr   = stats.red;
            aStatus->greenCntr = stats.green;
        }
    }

    return retVal;
}

static int32_t COMMS_GetStreamPolicerSnapshot(COMMS_StreamPolicerSnapshotType *const aSnapshot)
{
    int32_t  retVal = BCM_ERR_OK;
    uint32_t i = 0UL;

    BCM_MemSet(aSnapshot, 0U, sizeof(COMMS_StreamPolicerSnapshotType));
    for (; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
        if (COMMS_StreamData.streams[i].row != CFP_MAX_RULES) {
            aSnapshot->policer[i] |= COMMS_STREAMPOLICERSTATE_VALID_MASK;
            if (i < CommsConfig.numStreamPolicers) {
                aSnapshot->policer[i] |= COMMS_STREAMPOLICERSTATE_STATIC_MASK;
            }
            if (COMMS_StreamData.streams[i].blocked == TRUE) {
                aSnapshot->policer[i] |= COMMS_STREAMPOLICERSTATE_BLOCKED_MASK;
            }
        }
    }

    return retVal;
}

static int32_t COMMS_BlockStream(uint32_t aStreamIdx)
{
    int32_t        retVal = BCM_ERR_OK;
    CFP_ActionType action;

    if (aStreamIdx >= COMMS_MAX_STREAM_POLICER_ENTRIES) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (COMMS_StreamData.streams[aStreamIdx].row == CFP_MAX_RULES) {
        retVal = BCM_ERR_NOT_FOUND;
    } else if (COMMS_StreamData.streams[aStreamIdx].blocked == TRUE) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        BCM_MemSet(&action, 0U, sizeof(action));
        action.dstMapIBFlags    = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
        action.dstMapOBFlags    = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
        retVal = CFP_UpdateRule(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[aStreamIdx].row, &action);
        COMMS_StreamData.streams[aStreamIdx].blocked = TRUE;
    }

    return retVal;
}

static int32_t COMMS_ResumeStream(uint32_t aStreamIdx)
{
    int32_t        retVal = BCM_ERR_OK;
    CFP_ActionType action;

    if (aStreamIdx >= COMMS_MAX_STREAM_POLICER_ENTRIES) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (COMMS_StreamData.streams[aStreamIdx].row == CFP_MAX_RULES) {
        retVal = BCM_ERR_NOT_FOUND;
    } else {
        BCM_MemSet(&action, 0U, sizeof(action));
        action.meter.cirRefCnt    = CommsConfig.streamPolicers[aStreamIdx].rate / 4U;
        action.meter.cirTkBkt     = CommsConfig.streamPolicers[aStreamIdx].burstSize * 8U;
        action.meter.cirBktSize   = CommsConfig.streamPolicers[aStreamIdx].burstSize;
        action.meter.policerFlags = CFP_METER_CM_MASK | (CFP_POLICERMODE_MEF << CFP_METER_MODE_SHIFT);
        action.dstMapIBFlags      = (CFP_CHANGEFWDMAP_NON << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
        action.dstMapOBFlags      = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
        action.otherFlags         = CFP_ACTION_BYPASS_VLAN_MASK |
            CFP_ACTION_BYPASS_EAP_MASK  |
            CFP_ACTION_BYPASS_STP_MASK;
        retVal = CFP_UpdateRule(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[aStreamIdx].row, &action);
        COMMS_StreamData.streams[aStreamIdx].blocked   = FALSE;
        COMMS_StreamData.streams[aStreamIdx].tickCount = 0UL;
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherSwtProcessCommand(ETHERSWT_CmdIdType aCmdId,
                                 int32_t * const aCmdStatus,
                                 uint8_t * const aCmdData,
                                 uint32_t * const aLen)
{
    int32_t                retVal = BCM_ERR_OK;
    uint32_t               portNum = 0UL;
    uint32_t               portMask = 0UL;
    uint32_t               tagMask = 0UL;
    uint32_t               idx = 0UL;
    ETHERSWT_PayloadType   PaylPtr;
    ETHXCVR_PortConfigType xcvrConfig;
    uint32_t               errValues[4] = {0UL, 0UL, 0UL, 0UL};
    MCU_ExtendedInfoType   stackingInfo = {0U};
    uint32_t               stackingPort = 0UL;

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWTPROCESSCOMMAND_PROC, retVal,
                         (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

    /* Error check */
    if((NULL == aCmdData) || (NULL == aCmdStatus) || (NULL == aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        errValues[0] =  __LINE__;
    } else {
        PaylPtr.u8Ptr = aCmdData;
        switch(aCmdId){
        case ETHERSWT_CMDID_PORT_INFO_GET:
            {
                ETHERSWT_PortStatsType stats;
                uint16_t pvid, prio;
                ETHXCVR_BusModeType busMode;
                ETHXCVR_PhyMediaType phyMedia;

                /* Command payload size check */
                if(sizeof(ETHERSWT_PortInfoType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portNum = COMMS_ConvertToLocalPort(&stackingInfo,
                                                   PaylPtr.portInfo->port,
                                                   &stackingPort);
                BCM_MemSet(PaylPtr.u8Ptr, 0U,
                           sizeof(ETHERSWT_PortInfoType));
                PaylPtr.portInfo->port =
                                 COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                            portNum,
                                                            &stackingPort);

                /* Query the port type */
                retVal = ETHERSWT_GetPortType(ETH_SWITCH_HW_ID, portNum, &busMode, &phyMedia);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                } else {
                    PaylPtr.portInfo->busMode  = busMode;
                    PaylPtr.portInfo->phyMedia = phyMedia;
                }

                /* Query the admin mode */
                retVal = ETHERSWT_GetPortMode(ETH_SWITCH_HW_ID, portNum,
                                              &PaylPtr.portInfo->adminMode);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->adminMode;
                    break;
                }

                /* Query the link status */
                retVal = ETHERSWT_GetPortLinkState(ETH_SWITCH_HW_ID, portNum,
                                                   &PaylPtr.portInfo->linkStatus);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->linkStatus;
                    break;
                }


                /* Query the link speed */
                retVal = ETHERSWT_GetPortSpeed(ETH_SWITCH_HW_ID, portNum,
                                               &PaylPtr.portInfo->speed);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->speed;
                    break;
                }


                if ((phyMedia == ETHXCVR_PHYMEDIA_10BASET1) ||
                    (phyMedia == ETHXCVR_PHYMEDIA_100BASET1) ||
                    (phyMedia == ETHXCVR_PHYMEDIA_1000BASET1)) {
                    /* Query the BR mode */
                    retVal = ETHERSWT_GetPortMasterMode(ETH_SWITCH_HW_ID, portNum,
                                                        &PaylPtr.portInfo->masterEnable);
                    if (BCM_ERR_OK != retVal) {
                        errValues[0] = __LINE__;
                        errValues[1] = portNum;
                        errValues[2] = PaylPtr.portInfo->masterEnable;
                        break;
                    }
                    retVal = ETHERSWT_GetSQIValue(ETH_SWITCH_HW_ID, portNum, &PaylPtr.portInfo->linkSQI);
                    /* SQI value is valid only for BRPHY ports */
                    if(BCM_ERR_OK != retVal)  {
                        errValues[0] = __LINE__;
                        errValues[1] = portNum;
                        errValues[2] = PaylPtr.portInfo->linkSQI;
                        break;
                    }
                }

                /* Query the jumbo mode */
                retVal = ETHERSWT_GetPortJumboMode(ETH_SWITCH_HW_ID, portNum,
                                                   &PaylPtr.portInfo->jumboEnable);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_NOSUPPORT != retVal)) {
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->jumboEnable;
                    break;
                }

                /* Query the loopback mode */
                retVal = ETHERSWT_GetPortLoopbackMode(ETH_SWITCH_HW_ID, portNum,
                                                      &PaylPtr.portInfo->loopbackEnable);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_NOSUPPORT != retVal)) {
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->loopbackEnable;
                    break;
                }

                /* Query the autoneg mode */
                retVal = ETHXCVR_GetPortConfig(portNum, &xcvrConfig);
                if (BCM_ERR_OK == retVal) {
                    PaylPtr.portInfo->autonegEnable = xcvrConfig.autoNeg;

                }

                /* Query the auto negotiation status */
                retVal = ETHERSWT_GetPortAutoNegStatus(ETH_SWITCH_HW_ID, portNum,
                                                       &PaylPtr.portInfo->autonegComplete);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_NOSUPPORT != retVal)) {
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->autonegComplete;
                    break;
                }

                /* Query the duplex mode */
                retVal = ETHERSWT_GetPortDuplexMode(ETH_SWITCH_HW_ID, portNum,
                                                    &PaylPtr.portInfo->duplex);
                if(BCM_ERR_OK != retVal){
                    errValues[0] =  __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->duplex;
                    break;
                }

                /* Query the LED state */
                retVal = ETHERSWT_GetLedState(ETH_SWITCH_HW_ID, portNum,
                                              ETHERSWT_LED_AVB,
                                              &PaylPtr.portInfo->led);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->led;
                    break;
                }

                /* Query the link state change count */
                retVal = ETHERSWT_GetXcvrStats(ETH_SWITCH_HW_ID, portNum, &stats);
                if(BCM_ERR_OK == retVal){
                    PaylPtr.portInfo->linkStateChangeCnt = stats.linkStateChangeCount;
                } else {
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->linkStateChangeCnt;
                    break;
                }

                /* Query the PVID and PRIO used for untagged packets */
                retVal = ETHERSWT_GetPortDefaultVlan(ETH_SWITCH_HW_ID, portNum, &pvid, &prio);
                if(BCM_ERR_OK != retVal){
                    errValues[0] =  __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = (uint32_t)pvid;
                    errValues[3] = (uint32_t)prio;
                    break;
                } else {
                    PaylPtr.portInfo->pvid = (uint32_t)pvid;
                    PaylPtr.portInfo->prio = (uint32_t)prio;
                }
                break;
            }
        case ETHERSWT_CMDID_PORT_INFO_SET:
            {
                ETHXCVR_BusModeType busMode;
                ETHXCVR_PhyMediaType phyMedia;

                /* Command payload size check */
                if(sizeof(ETHERSWT_PortInfoType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portNum = COMMS_ConvertToLocalPort(&stackingInfo,
                                                   PaylPtr.portInfo->port,
                                                   &stackingPort);

                /* Query the port type */
                retVal = ETHERSWT_GetPortType(ETH_SWITCH_HW_ID, portNum, &busMode, &phyMedia);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }

                /* Set the admin mode */
                retVal = ETHERSWT_SetPortMode(ETH_SWITCH_HW_ID, portNum,
                                              PaylPtr.portInfo->adminMode);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->adminMode;
                    break;
                }

                /* Set the link speed */
                retVal = ETHERSWT_SetPortSpeed(ETH_SWITCH_HW_ID, portNum,
                                               PaylPtr.portInfo->speed);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->speed;
                    break;
                }

                if ((phyMedia == ETHXCVR_PHYMEDIA_10BASET1) ||
                        (phyMedia == ETHXCVR_PHYMEDIA_100BASET1) ||
                        (phyMedia == ETHXCVR_PHYMEDIA_1000BASET1)) {
                    /* Set the connection mode */
                    retVal = ETHERSWT_SetPortMasterMode(ETH_SWITCH_HW_ID, portNum,
                            PaylPtr.portInfo->masterEnable);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = portNum;
                        errValues[2] = PaylPtr.portInfo->masterEnable;
                        break;
                    }
                }

                /* Set the jumbo mode */
                retVal = ETHERSWT_SetPortJumboMode(ETH_SWITCH_HW_ID, portNum,
                                                   PaylPtr.portInfo->jumboEnable);
                if(BCM_ERR_OK != retVal){
                    errValues[0] =  __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->jumboEnable;
                    break;
                }

                /* Set the loopback mode */
                retVal = ETHERSWT_SetPortLoopbackMode(ETH_SWITCH_HW_ID, portNum,
                                                      PaylPtr.portInfo->loopbackEnable);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->loopbackEnable;
                    break;
                }

                /* Set the PVID and PRIO used for untagged packets */
                retVal = ETHERSWT_SetPortDefaultVlan(ETH_SWITCH_HW_ID, portNum,
                                                     PaylPtr.portInfo->pvid,
                                                     PaylPtr.portInfo->prio);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    errValues[2] = PaylPtr.portInfo->pvid;
                    errValues[3] = PaylPtr.portInfo->prio;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_READ:
            {
                uint32_t idx = 0UL;
                uint32_t regAddr = 0UL;

                /* Command payload size check */
                if(sizeof(ETHERSWT_RegAccessType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                /* Clear the memory region where data needs to be updated */
                BCM_MemSet(PaylPtr.regAccess->data, 0U,
                           sizeof(PaylPtr.regAccess->data));

                /* Fetch the input address */
                regAddr = PaylPtr.regAccess->addr;

                /* For the specified length, read the data */
                for(idx = 0UL; idx < PaylPtr.regAccess->len; idx++){
                    uint64_t regVal;
                    retVal = ETHERSWT_ReadReg(ETH_SWITCH_HW_ID, regAddr,
                                              &regVal);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = regAddr;
                        break;
                    }

                    BCM_MemCpy(&PaylPtr.regAccess->data[idx*8UL],&regVal,
                               sizeof(uint64_t));

                }
                break;
            }
        case ETHERSWT_CMDID_WRITE:
            {
                uint32_t idx = 0UL;
                uint32_t regAddr = 0UL;

                /* Command payload size check */
                if(sizeof(ETHERSWT_RegAccessType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                regAddr = PaylPtr.regAccess->addr;

                /* For the specified length, write the data */
                for(idx = 0UL; idx < PaylPtr.regAccess->len; idx++){
                    uint64_t regVal;

                    BCM_MemCpy(&regVal, &PaylPtr.regAccess->data[idx*8UL],
                               sizeof(uint64_t));

                    retVal = ETHERSWT_WriteReg(ETH_SWITCH_HW_ID, regAddr, regVal);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = regAddr;
                        break;
                    }

                }
                break;
            }
        case ETHERSWT_CMDID_MIBS:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_MibType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }
                portNum = COMMS_ConvertToLocalPort(&stackingInfo,
                                                   PaylPtr.mib->port,
                                                   &stackingPort);
                BCM_MemSet(PaylPtr.u8Ptr, 0U,
                           sizeof(ETHERSWT_MibType));
                PaylPtr.mib->port =
                                 COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                            portNum,
                                                            &stackingPort);

                /* Fetch the Rx stats */
                retVal = ETHERSWT_GetRxStat(ETH_SWITCH_HW_ID,
                                            portNum,
                                            &PaylPtr.mib->rxStats);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    break;
                }

                /* Fetch the Tx stats */
                retVal = ETHERSWT_GetTxStat(ETH_SWITCH_HW_ID,
                                         portNum,
                                         &PaylPtr.mib->txStats);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_MIBS_CLEAR:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_MibType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portNum = COMMS_ConvertToLocalPort(&stackingInfo,
                                                   PaylPtr.mib->port,
                                                   &stackingPort);
                BCM_MemSet(PaylPtr.u8Ptr, 0U,
                           sizeof(ETHERSWT_MibType));
                PaylPtr.mib->port =
                                 COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                            portNum,
                                                            &stackingPort);

                /* Clear the Rx stats */
                retVal = ETHERSWT_ClearRxStat(ETH_SWITCH_HW_ID, portNum);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }

                /* Clear the Tx stats */
                retVal = ETHERSWT_ClearTxStat(ETH_SWITCH_HW_ID, portNum);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_SWITCH_INFO_GET:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_SwitchInfoType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                BCM_MemSet(PaylPtr.u8Ptr, 0U,
                           sizeof(ETHERSWT_SwitchInfoType));

                /* Query the FDB age time */
                retVal = ETHERSWT_GetAge(ETH_SWITCH_HW_ID,
                                         &PaylPtr.switchInfo->ageTime);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.switchInfo->ageTime;
                    break;
                }

                /* Query the dumb forward mode */
                retVal = ETHERSWT_GetDumbFwdMode(ETH_SWITCH_HW_ID,
                                                 &PaylPtr.switchInfo->dumbFwdMode);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.switchInfo->dumbFwdMode;
                    break;
                }

                /* Query ingress filter */
                retVal = ETHERSWT_GetVLANIngressFilterMode(ETH_SWITCH_HW_ID,
                                                           &PaylPtr.switchInfo->iFilter);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.switchInfo->iFilter;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_SWITCH_INFO_SET:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_SwitchInfoType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                /* Set the FDB age time */
                retVal = ETHERSWT_SetAge(ETH_SWITCH_HW_ID,
                                         PaylPtr.switchInfo->ageTime);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.switchInfo->ageTime;
                    break;
                }

                /* For a stacking slave we should not be altering the status */
                if ((0U == stackingInfo.stackingEn) ||
                    ((1U == stackingInfo.stackingEn) &&
                     (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode))) {
                    /* Set the dumb forward mode */
                    retVal = ETHERSWT_SetDumbFwdMode(ETH_SWITCH_HW_ID,
                                                     PaylPtr.switchInfo->dumbFwdMode);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = PaylPtr.switchInfo->dumbFwdMode;
                        break;
                    }
                }

                /* Set ingress filter */
                retVal = ETHERSWT_SetVLANIngressFilterMode(ETH_SWITCH_HW_ID,
                                                           PaylPtr.switchInfo->iFilter);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.switchInfo->iFilter;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_MIRROR_ENABLE:
            {
                ETHERSWT_TrafficDirType trafficDir;

                /* Command payload size check */
                if(sizeof(ETHERSWT_MirrorEnableType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portNum = COMMS_ConvertToLocalPort(&stackingInfo,
                                                   PaylPtr.mirrorEnable->probePort,
                                                   &stackingPort);
                trafficDir = PaylPtr.mirrorEnable->direction;

                PaylPtr.mirrorEnable->mirrorCfg.portMask =
                                COMMS_GetLocalPortMask(&stackingInfo,
                                                       PaylPtr.mirrorEnable->mirrorCfg.portMask);

                /* Clearing the stacking port bit mask before posting to queue */
                PaylPtr.mirrorEnable->mirrorCfg.portMask &=
                                    ~((uint16_t)(1U << stackingInfo.stackPort0));

                retVal = ETHERSWT_SetPortMirrorConfig(ETH_SWITCH_HW_ID,
                        portNum,
                        trafficDir,
                        &PaylPtr.mirrorEnable->mirrorCfg);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }

                /* Enable the port mirroring */
                retVal = ETHERSWT_SetPortMirrorState(ETH_SWITCH_HW_ID,
                                                     portNum,
                                                     ETHERSWT_PORT_MIRROR_STATE_ENABLED);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }

                PaylPtr.mirrorEnable->mirrorCfg.portMask = (uint32_t)
                         COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                        PaylPtr.mirrorEnable->mirrorCfg.portMask);
                break;
            }
        case ETHERSWT_CMDID_MIRROR_DISABLE:
        {
            /* Query the mirror capture port */
            retVal = ETHERSWT_GetMirrorCapturePort(ETH_SWITCH_HW_ID, &portNum);
            if (BCM_ERR_NOT_FOUND == retVal) {
                retVal = BCM_ERR_OK;
                break;
            } else if (BCM_ERR_OK != retVal) {
                errValues[0] = __LINE__;
                break;
            }

            /* Disable the port mirroring */
            retVal = ETHERSWT_SetPortMirrorState(ETH_SWITCH_HW_ID,
                                                 portNum,
                                                 ETHERSWT_PORT_MIRROR_STATE_DISABLED);
            if(BCM_ERR_OK != retVal){
                errValues[0] = __LINE__;
                errValues[1] = portNum;
            }
            break;
        }
        case ETHERSWT_CMDID_MIRROR_STATUS:
        {
            /* Command payload size check */
            if(sizeof(ETHERSWT_MirrorStatusType) != *aLen){
                retVal = BCM_ERR_INVAL_PARAMS;
                errValues[0] = __LINE__;
                errValues[1] = *aLen;
                break;
            }

            /* Query the mirror capture port */
            retVal = ETHERSWT_GetMirrorCapturePort(ETH_SWITCH_HW_ID, &portNum);
            if (BCM_ERR_NOT_FOUND == retVal) {
                PaylPtr.mirrorStatus->state = ETHERSWT_PORT_MIRROR_STATE_DISABLED;
                retVal = BCM_ERR_OK;
                break;
            } else if (BCM_ERR_OK != retVal) {
                errValues[0] = __LINE__;
                break;
            }

            PaylPtr.mirrorStatus->probePort = COMMS_ConvertToUnifiedPort(&stackingInfo,
                                                portNum,
                                                &stackingPort);;

            /* Query the port mirror stats */
            retVal = ETHERSWT_GetPortMirrorState(ETH_SWITCH_HW_ID,
                    portNum,
                    &PaylPtr.mirrorStatus->state);
            if(ETHERSWT_PORT_MIRROR_STATE_ENABLED ==
                    PaylPtr.mirrorStatus->state){
                /* Query the mirror configuration only if mirroring is enabled */
                retVal = ETHERSWT_GetPortMirrorConfig(ETH_SWITCH_HW_ID,
                        portNum,
                        ETHERSWT_TRAFFICDIR_INGRESS,
                        &PaylPtr.mirrorStatus->ingressMirrorCfg);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }
                retVal = ETHERSWT_GetPortMirrorConfig(ETH_SWITCH_HW_ID,
                        portNum,
                        ETHERSWT_TRAFFICDIR_EGRESS,
                        &PaylPtr.mirrorStatus->egressMirrorCfg);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portNum;
                    break;
                }
            }

            PaylPtr.mirrorStatus->ingressMirrorCfg.portMask =
                    COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                   PaylPtr.mirrorStatus->ingressMirrorCfg.portMask);
            PaylPtr.mirrorStatus->egressMirrorCfg.portMask =
                    COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                   PaylPtr.mirrorStatus->egressMirrorCfg.portMask);
            break;
        }
        case ETHERSWT_CMDID_ARL_ADD:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_ARLEntryType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_AddARLEntry(PaylPtr.arlEntry);
                if (BCM_ERR_OK != retVal) {
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.arlEntry->portMask;
                    errValues[2] = (uint32_t)PaylPtr.arlEntry->vlanID;
                    errValues[3] = (uint32_t)((PaylPtr.arlEntry->macAddr[5] << 24UL) |
                                              (PaylPtr.arlEntry->macAddr[4] << 16UL) |
                                              (PaylPtr.arlEntry->macAddr[3] << 8UL)  |
                                              (PaylPtr.arlEntry->macAddr[2] << 0UL));
                    break;
                }

                PaylPtr.arlEntry->portMask =
                            COMMS_GetLocalPortMask(&stackingInfo,
                                                   PaylPtr.arlEntry->portMask);
                /* Add the ARL entry */
                retVal = ETHERSWT_AddARLEntry(ETH_SWITCH_HW_ID,
                                              PaylPtr.arlEntry);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.arlEntry->portMask;
                    errValues[2] = (uint32_t)PaylPtr.arlEntry->vlanID;
                    errValues[3] = (uint32_t)((PaylPtr.arlEntry->macAddr[5] << 24UL) |
                                              (PaylPtr.arlEntry->macAddr[4] << 16UL) |
                                              (PaylPtr.arlEntry->macAddr[3] << 8UL)  |
                                              (PaylPtr.arlEntry->macAddr[2] << 0UL));
                    break;
                } else {
                    PaylPtr.arlEntry->portMask = COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                                PaylPtr.arlEntry->portMask);
                }

                break;
            }
        case ETHERSWT_CMDID_ARL_DELETE:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_ARLEntryType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_DelARLEntry(PaylPtr.arlEntry);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.arlEntry->portMask;
                    errValues[2] = (uint32_t)PaylPtr.arlEntry->vlanID;
                    errValues[3] = (uint32_t)((PaylPtr.arlEntry->macAddr[5] << 24UL) |
                                              (PaylPtr.arlEntry->macAddr[4] << 16UL) |
                                              (PaylPtr.arlEntry->macAddr[3] << 8UL)  |
                                              (PaylPtr.arlEntry->macAddr[2] << 0UL));
                    break;
                }

                PaylPtr.arlEntry->portMask =
                            COMMS_GetLocalPortMask(&stackingInfo,
                                                   PaylPtr.arlEntry->portMask);
                /* Remove the ARL entry */
                retVal = ETHERSWT_DeleteARLEntry(ETH_SWITCH_HW_ID,
                                                 PaylPtr.arlEntry);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.arlEntry->portMask;
                    errValues[2] = (uint32_t)PaylPtr.arlEntry->vlanID;
                    errValues[3] = (uint32_t)((PaylPtr.arlEntry->macAddr[5] << 24UL) |
                                              (PaylPtr.arlEntry->macAddr[4] << 16UL) |
                                              (PaylPtr.arlEntry->macAddr[3] << 8UL)  |
                                              (PaylPtr.arlEntry->macAddr[2] << 0UL));
                    break;
                } else {
                    PaylPtr.arlEntry->portMask = COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                                                PaylPtr.arlEntry->portMask);
                }
                break;
            }
        case ETHERSWT_CMDID_ARL_SNAPSHOT:
            {
                uint16_t arlEntrySz = COMMS_ARL_ENTRY_LIST_MAX;

                /* Command payload size check */
                if(sizeof(ETHERSWT_ARLListType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                BCM_MemSet(PaylPtr.arlList->entries, 0U,
                          sizeof(ETHERSWT_ARLEntryType)*ETHERSWT_ARL_ENTRIES_MAX);

                if ((1U == stackingInfo.stackingEn) &&
                    (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {
                    if (COMMS_ARLSTATE_RESET == COMMS_ETHERSWT_RefreshARL.ARLState) {
                        PaylPtr.arlList->num = (uint32_t)COMMS_ETHERSWT_RefreshARL.ARLLen;
                    } else {
                        retVal = BCM_ERR_EAGAIN;
                    }
                } else {
                    /* Query and populate the entire ARL into memory */
                    retVal = ETHERSWT_GetARLTable(ETH_SWITCH_HW_ID,
                                                  COMMS_EtherSwtARLEntries, &arlEntrySz);
                    if(BCM_ERR_OK == retVal){
                        COMMS_ETHERSWT_RefreshARL.ARLLen = arlEntrySz;
                        PaylPtr.arlList->num = (uint32_t)arlEntrySz;
                        for (idx = 0UL; idx < arlEntrySz; idx++) {
                            COMMS_EtherSwtARLEntries[idx].portMask =
                                        COMMS_ConvertToUnifiedPortMask(
                                                &stackingInfo,
                                                COMMS_EtherSwtARLEntries[idx].portMask);
                        }
                    } else {
                        errValues[0] = __LINE__;
                        errValues[1] = arlEntrySz;
                        break;
                    }
                }
                break;
            }
        case ETHERSWT_CMDID_ARL_GET:
            {
                /* Index from which the ARL entries need to be picked */
                uint32_t idx = 0UL;

                /* Command payload size check */
                if(sizeof(ETHERSWT_ARLListType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }
                idx = PaylPtr.arlList->num;
                /* Limit check */
                if(COMMS_ETHERSWT_RefreshARL.ARLLen < idx){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                } else {
                    /* Compute the number of entries to be copied */
                    uint32_t copyLen = ((idx + ETHERSWT_ARL_ENTRIES_MAX) < COMMS_ETHERSWT_RefreshARL.ARLLen)?
                                            ETHERSWT_ARL_ENTRIES_MAX : (COMMS_ETHERSWT_RefreshARL.ARLLen - idx);

                    /* Clear the memory where the entries are to be copied */
                    BCM_MemSet(PaylPtr.arlList->entries, 0U,
                               sizeof(ETHERSWT_ARLEntryType)*ETHERSWT_ARL_ENTRIES_MAX);

                    /* Copy the identified number of ARL entries */
                    BCM_MemCpy(PaylPtr.arlList->entries,
                               &COMMS_EtherSwtARLEntries[idx], copyLen * sizeof(ETHERSWT_ARLEntryType));

                    /* Newly added, update the number of entries being sent */
                    PaylPtr.arlList->num = copyLen;
                }
                break;
            }
        case ETHERSWT_CMDID_VLAN_PORT_ADD:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_VLANPortType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portMask = COMMS_GetLocalPortMask(&stackingInfo,
                                                  PaylPtr.vlanPort->portMask);
                tagMask = COMMS_GetLocalPortMask(&stackingInfo,
                                                 PaylPtr.vlanPort->tagMask);

                /* Enable VLAN on the mentioned port */
                retVal = ETHERSWT_AddVLANPorts(ETH_SWITCH_HW_ID,
                                               portMask,
                                               PaylPtr.vlanPort->vlan,
                                               tagMask);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portMask;
                    errValues[2] = (uint32_t)PaylPtr.vlanPort->vlan;
                    errValues[3] = tagMask;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_VLAN_PORT_REMOVE:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_VLANPortType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                portMask = COMMS_GetLocalPortMask(&stackingInfo,
                                                     PaylPtr.vlanPort->portMask);

                /* Remove VLAN on the mentioned port */
                retVal = ETHERSWT_RemoveVLANPorts(ETH_SWITCH_HW_ID,
                                                  portMask,
                                                  PaylPtr.vlanPort->vlan);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = portMask;
                    errValues[2] = (uint32_t)PaylPtr.vlanPort->vlan;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_VLAN_GET:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_VLANPortType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                /* Query the port and untag mask for specified VLAN */
                retVal = ETHERSWT_GetVLANPorts(ETH_SWITCH_HW_ID,
                                               PaylPtr.vlanPort->vlan,
                                               &PaylPtr.vlanPort->portMask,
                                               &PaylPtr.vlanPort->tagMask,
                                               &PaylPtr.vlanPort->staticPortMask);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = (uint32_t)PaylPtr.vlanPort->vlan;
                    break;
                } else {
                    PaylPtr.vlanPort->portMask = (uint32_t)
                                COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                               PaylPtr.vlanPort->portMask);
                    PaylPtr.vlanPort->tagMask = (uint32_t)
                                COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                               PaylPtr.vlanPort->tagMask);
                    PaylPtr.vlanPort->staticPortMask = (uint32_t)
                                COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                               PaylPtr.vlanPort->staticPortMask);
                }
                break;
            }
        case ETHERSWT_CMDID_STREAMPOLICER_ADD:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_StreamPolicerType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_AddStreamPolicer(PaylPtr.streamPolicerEntry->macAddr,
                                                PaylPtr.streamPolicerEntry->vlanID,
                                                &PaylPtr.streamPolicerEntry->policerParams,
                                                &PaylPtr.streamPolicerEntry->streamIdx);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = (uint32_t)((PaylPtr.streamPolicerEntry->macAddr[3] << 0UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[2] << 8UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[1] << 16UL)  |
                                              (PaylPtr.streamPolicerEntry->macAddr[0] << 24UL));
                    errValues[2] = (uint32_t)((PaylPtr.streamPolicerEntry->macAddr[5] << 0UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[4] << 8UL));
                    errValues[3] = (uint32_t)PaylPtr.streamPolicerEntry->vlanID;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_STREAMPOLICER_FIND:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_StreamPolicerType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_FindStreamPolicerIdx(PaylPtr.streamPolicerEntry->macAddr,
                                                    PaylPtr.streamPolicerEntry->vlanID,
                                                    PaylPtr.streamPolicerEntry->policerParams.portMask,
                                                    &PaylPtr.streamPolicerEntry->streamIdx);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = (uint32_t)((PaylPtr.streamPolicerEntry->macAddr[3] << 0UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[2] << 8UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[1] << 16UL)  |
                                              (PaylPtr.streamPolicerEntry->macAddr[0] << 24UL));
                    errValues[2] = (uint32_t)((PaylPtr.streamPolicerEntry->macAddr[5] << 0UL) |
                                              (PaylPtr.streamPolicerEntry->macAddr[4] << 8UL));
                    errValues[3] = (uint32_t)PaylPtr.streamPolicerEntry->vlanID;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_STREAMPOLICER_GET:
            {
                /* Command payload size check */
                if(sizeof(COMMS_StreamPolicerStatusType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_GetStreamPolicerStatus(PaylPtr.streamPolicerStatus);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.streamPolicerStatus->idx;
                    break;
                } else {
                    PaylPtr.streamPolicerStatus->portMask = COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                                PaylPtr.streamPolicerStatus->portMask);
                }
                break;
            }
        case ETHERSWT_CMDID_STREAMPOLICER_SNAPSHOT:
            {
                /* Command payload size check */
                if(sizeof(COMMS_StreamPolicerSnapshotType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_GetStreamPolicerSnapshot(PaylPtr.streamPolicerSnapshot);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_STREAMPOLICER_DEL:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_StreamPolicerType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_DelStreamPolicer(PaylPtr.streamPolicerEntry->streamIdx);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.streamPolicerEntry->streamIdx;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_BLOCKSTREAM:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_StreamPolicerType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_BlockStream(PaylPtr.streamPolicerEntry->streamIdx);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.streamPolicerEntry->streamIdx;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_RESUMESTREAM:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_StreamPolicerType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = COMMS_ResumeStream(PaylPtr.streamPolicerEntry->streamIdx);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.streamPolicerEntry->streamIdx;
                    break;
                }
                break;
            }
#if defined(ENABLE_CFP)
        case ETHERSWT_CMDID_CFP_ADDRULE:
            {
                uint32_t row   = 0UL;
                uint32_t slice = 0UL;
                /* Command payload size check */
                if(sizeof(CFP_RuleType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                PaylPtr.cfpRule->key.ingressPortBitmap = COMMS_GetLocalPortMask(&stackingInfo,
                                                            PaylPtr.cfpRule->key.ingressPortBitmap);

                COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapIBFlags, TRUE);
                COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapOBFlags, TRUE);

                row   = (PaylPtr.cfpRule->rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT ;
                slice = (PaylPtr.cfpRule->rowAndSlice & CFP_ROWANDSLICE_SLICE_MASK) >> CFP_ROWANDSLICE_SLICE_SHIFT;

                retVal = CFP_AddRule(ETH_SWITCH_HW_ID, &PaylPtr.cfpRule->key,
                                     &PaylPtr.cfpRule->action, &row, &slice);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = row;
                    errValues[2] = slice;
                    break;
                }
                PaylPtr.cfpRule->rowAndSlice = ((row << CFP_ROWANDSLICE_ROW_SHIFT) & CFP_ROWANDSLICE_ROW_MASK) |
                                               ((slice << CFP_ROWANDSLICE_SLICE_SHIFT) & CFP_ROWANDSLICE_SLICE_MASK);
                break;
            }
        case ETHERSWT_CMDID_CFP_REMOVERULE:
            {
                uint32_t row   = 0UL;
                /* Command payload size check */
                if(sizeof(CFP_RuleType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                row = (PaylPtr.cfpRule->rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT;
                retVal = CFP_RemoveRule(ETH_SWITCH_HW_ID, row);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = row;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_CFP_UPDATERULE:
            {
                uint32_t row   = 0UL;
                /* Command payload size check */
                if(sizeof(CFP_RuleType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapIBFlags, TRUE);
                COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapOBFlags, TRUE);

                row = (PaylPtr.cfpRule->rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT;
                retVal = CFP_UpdateRule(ETH_SWITCH_HW_ID,
                                        row,
                                        &PaylPtr.cfpRule->action);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = row;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_CFP_GETSTATS:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_CFPStatsType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                retVal = CFP_GetStats(ETH_SWITCH_HW_ID,
                                      PaylPtr.cfpStats->row,
                                      &PaylPtr.cfpStats->stats);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.cfpStats->row;
                    break;
                }
                break;
            }
        case ETHERSWT_CMDID_CFP_GETROWCONFIG:
            {
                uint32_t row   = 0UL;
                /* Command payload size check */
                if(sizeof(CFP_RuleType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    errValues[2] = sizeof(CFP_RuleType);
                    break;
                }

                row = (PaylPtr.cfpRule->rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT;
                retVal = CFP_GetRowConfig(ETH_SWITCH_HW_ID,
                                          row,
                                          PaylPtr.cfpRule);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    errValues[1] = row;
                    break;
                } else {
                    PaylPtr.cfpRule->key.ingressPortBitmap = COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                                     PaylPtr.cfpRule->key.ingressPortBitmap);

                    COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapIBFlags, FALSE);
                    COMMS_UpdateDestMapFlag(&stackingInfo, &PaylPtr.cfpRule->action.dstMapOBFlags, FALSE);
                }
                break;
            }
        case ETHERSWT_CMDID_CFP_GETSNAPSHOT:
            {
                /* Command payload size check */
                if(sizeof(CFP_TableSnapshotType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    errValues[2] = sizeof(CFP_TableSnapshotType);
                    break;
                }

                retVal = CFP_GetSnapshot(ETH_SWITCH_HW_ID,
                                         PaylPtr.cfpSnapshot);
                if(BCM_ERR_OK != retVal){
                    errValues[0] = __LINE__;
                    break;
                } else {
                    PaylPtr.cfpSnapshot->portEnableMask =
                                COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                               PaylPtr.cfpSnapshot->portEnableMask);
                }
                break;
            }
        case ETHERSWT_CMDID_CFP_SETPORTMODE:
            {
                /* Command payload size check */
                if(sizeof(ETHERSWT_CFPPortModeType) != *aLen){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = *aLen;
                    break;
                }

                PaylPtr.cfpPortMode->port =
                                COMMS_ConvertToLocalPort(&stackingInfo,
                                                         PaylPtr.cfpPortMode->port,
                                                         &stackingPort);

                if (PaylPtr.cfpPortMode->mode == 1UL) {
                    retVal = CFP_EnablePort(ETH_SWITCH_HW_ID,
                                            PaylPtr.cfpPortMode->port);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = PaylPtr.cfpPortMode->port;
                    }
                } else if (PaylPtr.cfpPortMode->mode == 0UL) {
                    retVal = CFP_DisablePort(ETH_SWITCH_HW_ID,
                                             PaylPtr.cfpPortMode->port);
                    if(BCM_ERR_OK != retVal){
                        errValues[0] = __LINE__;
                        errValues[1] = PaylPtr.cfpPortMode->port;
                        break;
                    }
                } else {
                    retVal = BCM_ERR_INVAL_PARAMS;
                    errValues[0] = __LINE__;
                    errValues[1] = PaylPtr.cfpPortMode->mode;
                    break;
                }
                break;
            }
#endif
        default:
            retVal = BCM_ERR_NOSUPPORT;
            *aLen = 0UL;
            break;
        }
    }

    /* Update the error status */
    *aCmdStatus = retVal;

    if(BCM_ERR_OK != retVal){
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWTPROCESSCOMMAND_PROC,
                          retVal,errValues[0], errValues[1], errValues[2], errValues[3]);
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherSwtSendCmd(uint8_t aCmdID,
                         BCM_CompIDType aCompID,
                         uint8_t *aCmd, uint32_t aLen,
                         const uint32_t aClientMask,
                         const MSGQ_MsgHdrType** const aHdr,
                         uint32_t *aForwardToSlaves)
{
    int32_t              retVal = BCM_ERR_OK;
    ETHERSWT_PayloadType PaylPtr;
    ETHERSWT_PortIDType  port = 0UL;
    uint32_t             addToCmdQ = TRUE;
    uint32_t             isStackingPort = FALSE;
    uint16_t             lclPortMask = 0U;
    MCU_ExtendedInfoType stackingInfo = {0U};
    COMMS_CmdRespType    cmdResp;

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_SENDCMD_PROC, retVal,
                          (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

    /* Set to default */
    *aForwardToSlaves = FALSE;
    if (aLen < RPC_MSG_PAYLOAD_SZ) {
        BCM_MemCpy(cmdResp.payload, aCmd, aLen);
        PaylPtr.u8Ptr = cmdResp.payload;
        cmdResp.cmdID = aCmdID;
        cmdResp.compID = aCompID;
        cmdResp.ret = BCM_ERR_UNKNOWN;
        cmdResp.payloadLen = aLen;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto error;
    }

    switch (aCmdID) {
        case ETHERSWT_CMDID_PORT_INFO_SET:
        case ETHERSWT_CMDID_PORT_INFO_GET:
            port = COMMS_ConvertToLocalPort(&stackingInfo,
                                            PaylPtr.portInfo->port,
                                            &isStackingPort);
            if (COMMS_INVALID_PORT == port) {
                retVal = BCM_ERR_INVAL_PARAMS;
                addToCmdQ = FALSE;
            } else if (TRUE == isStackingPort) {
                *aForwardToSlaves = TRUE;
                addToCmdQ = FALSE;
            } else {
                /* Nothing */
            }
            break;
        case ETHERSWT_CMDID_MIBS:
        case ETHERSWT_CMDID_MIBS_CLEAR:
            port = COMMS_ConvertToLocalPort(&stackingInfo,
                                            PaylPtr.mib->port,
                                            &isStackingPort);
            if (COMMS_INVALID_PORT == port) {
                retVal = BCM_ERR_INVAL_PARAMS;
                addToCmdQ = FALSE;
            } else if (TRUE == isStackingPort) {
                *aForwardToSlaves = TRUE;
                addToCmdQ = FALSE;
            } else {
                /* Nothing */
            }
            break;
        case ETHERSWT_CMDID_MIRROR_ENABLE:
            lclPortMask = COMMS_GetLocalPortMask(&stackingInfo,
                                                 PaylPtr.mirrorEnable->mirrorCfg.portMask);

            port = COMMS_ConvertToLocalPort(&stackingInfo,
                                            PaylPtr.mirrorEnable->probePort,
                                            &isStackingPort);
            if (COMMS_INVALID_PORT == port) {
                retVal = BCM_ERR_INVAL_PARAMS;
                goto error;
            /* If probe port is at slave OR if the slave portMask is set */
            } else if ((TRUE == isStackingPort) ||
                       ((PaylPtr.mirrorEnable->mirrorCfg.portMask | lclPortMask) != lclPortMask)) {
                *aForwardToSlaves = TRUE;
            } else {
                /* Nothing */
            }

            /* In case when the portMask has only slave ports, then only */
            /* stacking ports would be set. If the probe port is         */
            /* also at slave, then we dont post command to master.       */
            if (((1U << stackingInfo.stackPort0) != lclPortMask) ||
                (FALSE == isStackingPort)) {
                addToCmdQ = TRUE;
            } else {
                addToCmdQ = FALSE;
            }
            break;
        case ETHERSWT_CMDID_WRITE:
        case ETHERSWT_CMDID_READ:
            if ((0U == stackingInfo.stackingEn) ||
                ((1U == stackingInfo.stackingEn) &&
                 (stackingInfo.mstSlvMode == PaylPtr.regAccess->deviceID))) {
                *aForwardToSlaves = FALSE;
                addToCmdQ = TRUE;
            } else {
                *aForwardToSlaves = TRUE;
                addToCmdQ = FALSE;
            }
            break;
        case ETHERSWT_CMDID_CFP_SETPORTMODE:
            port = COMMS_ConvertToLocalPort(&stackingInfo,
                                            PaylPtr.cfpPortMode->port,
                                            &isStackingPort);
            if (TRUE == isStackingPort) {
                *aForwardToSlaves = TRUE;
                addToCmdQ = FALSE;
            }
            break;
        case ETHERSWT_CMDID_SWITCH_INFO_GET:
        case ETHERSWT_CMDID_SWITCH_INFO_SET:
        case ETHERSWT_CMDID_VLAN_GET:
        case ETHERSWT_CMDID_MIRROR_DISABLE:
        case ETHERSWT_CMDID_MIRROR_STATUS:
        case ETHERSWT_CMDID_CFP_REMOVERULE:
        case ETHERSWT_CMDID_CFP_GETSTATS:
        case ETHERSWT_CMDID_CFP_GETROWCONFIG:
        case ETHERSWT_CMDID_CFP_GETSNAPSHOT:
        case ETHERSWT_CMDID_STREAMPOLICER_DEL:
        case ETHERSWT_CMDID_BLOCKSTREAM:
        case ETHERSWT_CMDID_RESUMESTREAM:
        case ETHERSWT_CMDID_STREAMPOLICER_SNAPSHOT:
        case ETHERSWT_CMDID_STREAMPOLICER_GET:
        case ETHERSWT_CMDID_ARL_ADD:
        case ETHERSWT_CMDID_ARL_DELETE:
        case ETHERSWT_CMDID_CFP_ADDRULE:
        case ETHERSWT_CMDID_CFP_UPDATERULE:
        case ETHERSWT_CMDID_VLAN_PORT_ADD:
        case ETHERSWT_CMDID_VLAN_PORT_REMOVE:
        case ETHERSWT_CMDID_STREAMPOLICER_ADD:
            *aForwardToSlaves = TRUE;
            break;
        case ETHERSWT_CMDID_ARL_SNAPSHOT:
        case ETHERSWT_CMDID_ARL_GET:
        case ETHERSWT_CMDID_STREAMPOLICER_FIND:
            *aForwardToSlaves = FALSE;
            break;
        default:
            retVal = BCM_ERR_NOSUPPORT;
            break;
    }

    if (TRUE == addToCmdQ) {
        retVal = COMMS_AddToCmdQ(&cmdResp, aClientMask, aHdr);
    }

error:
    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherSwtCmdStatus(const MSGQ_MsgHdrType* const aHdr,
                                uint8_t *const aCmdID,
                                const BCM_CompIDType aCompID,
                                uint8_t *aReply,
                                uint32_t *aReplyLen)
{
    int32_t              retVal = BCM_ERR_OK;
    COMMS_CmdRespType    cmdResp;
    MCU_ExtendedInfoType stackingInfo = {0U};

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_CMDSTATUS_PROC, retVal,
                          (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

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
    } else if (BCM_ERR_OK != cmdResp.ret) {
        retVal = cmdResp.ret;
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_EtherSwtMergeResult(uint8_t aCmdID, BCM_CompIDType aCompID, uint32_t aLen,
                             uint8_t *aOutResponse,
                             int32_t *aOutResult,
                             const uint8_t *aInResponse1,
                             int32_t aInResult1,
                             const uint8_t *aInResponse2,
                             int32_t aInResult2)
{
    int32_t              retVal = BCM_ERR_OK;
    ETHERSWT_PayloadType ptr1;
    ETHERSWT_PayloadType ptr2;
    ETHERSWT_PayloadType outPtr;
    MCU_ExtendedInfoType stackingInfo = {0U};

    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_MERGERESULT_PROC, retVal,
                          (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

    if ((NULL == aOutResponse) ||
        (NULL == aOutResult) ||
        (NULL == aInResponse1) ||
        (NULL == aInResponse2)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto error;
    }

    ptr1.u8ConstPtr = aInResponse1;
    ptr2.u8ConstPtr = aInResponse2;
    outPtr.u8Ptr = aOutResponse;

    if ((BCM_ERR_OK != aInResult1) || (BCM_ERR_OK != aInResult2)) {
        *aOutResult = (aInResult1 == BCM_ERR_OK) ? aInResult2 : aInResult1;
        goto error;
    }

    switch (aCmdID) {
        case ETHERSWT_CMDID_SWITCH_INFO_GET:
            /* Configuration is expected to be same between master and slave. */
            /* dumbFwdMode is not expected to same between master and slave.  */
            if ((ptr1.switchInfo->ageTime != ptr2.switchInfo->ageTime) ||
                (ptr1.switchInfo->iFilter != ptr2.switchInfo->iFilter)) {
                *aOutResult = BCM_ERR_INVAL_STATE;
            } else {
                BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_SwitchInfoType));
                *aOutResult = BCM_ERR_OK;
            }
            break;
        case ETHERSWT_CMDID_SWITCH_INFO_SET:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_SwitchInfoType));
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_VLAN_GET:
        case ETHERSWT_CMDID_VLAN_PORT_ADD:
        case ETHERSWT_CMDID_VLAN_PORT_REMOVE:
            outPtr.vlanPort->vlan = ptr1.vlanPort->vlan;
            outPtr.vlanPort->portMask = ptr1.vlanPort->portMask | ptr2.vlanPort->portMask;
            outPtr.vlanPort->tagMask = ptr1.vlanPort->tagMask | ptr2.vlanPort->tagMask;
            outPtr.vlanPort->staticPortMask = ptr1.vlanPort->staticPortMask | ptr2.vlanPort->staticPortMask;
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_MIRROR_ENABLE:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_MirrorEnableType));
            outPtr.mirrorEnable->mirrorCfg.portMask = ptr1.mirrorEnable->mirrorCfg.portMask |
                                                      ptr2.mirrorEnable->mirrorCfg.portMask;
            if (COMMS_INVALID_PORT == outPtr.mirrorStatus->probePort) {
                outPtr.mirrorEnable->probePort = ptr2.mirrorEnable->probePort;
            }
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_MIRROR_DISABLE:
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_MIRROR_STATUS:
            if ((ptr1.mirrorStatus->state == ETHERSWT_PORT_MIRROR_STATE_ENABLED) &&
                (ptr2.mirrorStatus->state == ETHERSWT_PORT_MIRROR_STATE_ENABLED)) {

                BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_MirrorStatusType));

                if (COMMS_INVALID_PORT == outPtr.mirrorStatus->probePort) {
                    outPtr.mirrorStatus->probePort = ptr2.mirrorStatus->probePort;
                }

                outPtr.mirrorStatus->ingressMirrorCfg.portMask = ptr1.mirrorStatus->ingressMirrorCfg.portMask |
                                                                 ptr2.mirrorStatus->ingressMirrorCfg.portMask;
                outPtr.mirrorStatus->egressMirrorCfg.portMask = ptr1.mirrorStatus->egressMirrorCfg.portMask |
                                                                 ptr2.mirrorStatus->egressMirrorCfg.portMask;
            } else if (ptr1.mirrorStatus->state == ETHERSWT_PORT_MIRROR_STATE_ENABLED) {
                BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_MirrorStatusType));
            } else if (ptr2.mirrorStatus->state == ETHERSWT_PORT_MIRROR_STATE_ENABLED) {
                BCM_MemCpy(aOutResponse, aInResponse2, sizeof(ETHERSWT_MirrorStatusType));
            } else {
               outPtr.mirrorStatus->state = ETHERSWT_PORT_MIRROR_STATE_DISABLED;
            }
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_ARL_ADD:
        case ETHERSWT_CMDID_ARL_DELETE:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_MirrorStatusType));
            outPtr.arlEntry->portMask = ptr1.arlEntry->portMask | ptr2.arlEntry->portMask;
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_CFP_ADDRULE:
            if (ptr1.cfpRule->rowAndSlice == ptr2.cfpRule->rowAndSlice) {
                BCM_MemCpy(aOutResponse, aInResponse1, sizeof(CFP_RuleType));
                *aOutResult = BCM_ERR_OK;
            } else {
                *aOutResult = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case ETHERSWT_CMDID_CFP_REMOVERULE:
        case ETHERSWT_CMDID_CFP_UPDATERULE:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(CFP_RuleType));
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_CFP_GETSTATS:
            {
                uint64_t stats1 = ptr1.cfpStats->stats.green +
                                  ptr1.cfpStats->stats.red +
                                  ptr1.cfpStats->stats.yellow;
                uint64_t stats2 = ptr2.cfpStats->stats.green +
                                  ptr2.cfpStats->stats.red +
                                  ptr2.cfpStats->stats.yellow;
                if (stats1 > stats2) {
                    BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_CFPStatsType));
                } else {
                    BCM_MemCpy(aOutResponse, aInResponse2, sizeof(ETHERSWT_CFPStatsType));
                }
                *aOutResult = BCM_ERR_OK;
            }
            break;
        case ETHERSWT_CMDID_CFP_GETROWCONFIG:
            if (ptr1.cfpRule->rowAndSlice != ptr2.cfpRule->rowAndSlice) {
                *aOutResult = BCM_ERR_INVAL_PARAMS;
            } else {
                BCM_MemCpy(aOutResponse, aInResponse1, sizeof(CFP_RuleType));
                outPtr.cfpRule->key.ingressPortBitmap |= ptr2.cfpRule->key.ingressPortBitmap;
                outPtr.cfpRule->action.dstMapIBFlags  |= ptr2.cfpRule->action.dstMapIBFlags;
                outPtr.cfpRule->action.dstMapOBFlags  |= ptr2.cfpRule->action.dstMapOBFlags;
                *aOutResult = BCM_ERR_OK;
            }
            break;
        case ETHERSWT_CMDID_CFP_GETSNAPSHOT:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(CFP_TableSnapshotType));
            outPtr.cfpSnapshot->portEnableMask |= ptr2.cfpSnapshot->portEnableMask;
            *aOutResult = BCM_ERR_OK;
        case ETHERSWT_CMDID_STREAMPOLICER_ADD:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_StreamPolicerType));
            outPtr.streamPolicerEntry->policerParams.portMask = ptr1.streamPolicerEntry->policerParams.portMask |
                                                                ptr2.streamPolicerEntry->policerParams.portMask;
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_STREAMPOLICER_DEL:
        case ETHERSWT_CMDID_BLOCKSTREAM:
        case ETHERSWT_CMDID_RESUMESTREAM:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(ETHERSWT_StreamPolicerType));
            *aOutResult = BCM_ERR_OK;
            break;
        case ETHERSWT_CMDID_STREAMPOLICER_SNAPSHOT:
            {
                uint32_t i = 0UL;
                for (; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
                    outPtr.streamPolicerSnapshot->policer[i] = ptr1.streamPolicerSnapshot->policer[i] |
                        ptr2.streamPolicerSnapshot->policer[i];
                }
                *aOutResult = BCM_ERR_OK;
            }
            break;
        case ETHERSWT_CMDID_STREAMPOLICER_GET:
            BCM_MemCpy(aOutResponse, aInResponse1, sizeof(COMMS_StreamPolicerStatusType));
            outPtr.streamPolicerStatus->portMask = ptr1.streamPolicerStatus->portMask |
                                                   ptr2.streamPolicerStatus->portMask;
            *aOutResult = BCM_ERR_OK;
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }

error:
    return retVal;
}

/**
    API used to check if an ARL entry is present in other (master/slave)
    ARL list.

    @code{.unparsed}
    @endcode
*/
static uint32_t COMMS_EtherSwtCheckARLPresent(ETHERSWT_ARLEntryType* keyARL,
                                              ETHERSWT_ARLEntryType* arlList,
                                              uint32_t arlListLen)
{
    uint32_t count = COMMS_ETHERSWT_ARL_MATCH_INVALID;
    uint32_t idx;

    for (idx = 0UL; idx < arlListLen; idx++) {
        if ((0L == memcmp(keyARL->macAddr, arlList[idx].macAddr, 6UL)) &&
            (keyARL->vlanID == arlList[idx].vlanID)) {
            count = idx;
            COMMS_ETHERSWT_RefreshARL.slaveEntryMerged[idx/COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK] |=
                                (1UL << (idx % COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK));
            break;
        }
    }
    return count;
}

/**
    @code{.unparsed}
    @endcode
*/
void COMMS_EtherSwtARLMerge()
{
    uint32_t idx = 0UL;
    uint32_t stackingIdx = 0UL;
    int32_t  retVal;
    MCU_ExtendedInfoType stackingInfo = {0U};
    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_SENDCMD_PROC, retVal,
                          (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    BCM_MemSet(COMMS_ETHERSWT_RefreshARL.slaveEntryMerged, 0U,
               sizeof(COMMS_ETHERSWT_RefreshARL.slaveEntryMerged));

    /* For all the entries present on Master, check slave stacking list */
    /* and update the port mask.                                        */
    for (idx = 0UL ; idx < COMMS_ETHERSWT_RefreshARL.ARLLen; idx++) {
        /* Convert Master port mask to unified view */
        COMMS_EtherSwtARLEntries[idx].portMask =
                    COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                                   COMMS_EtherSwtARLEntries[idx].portMask);
        /* Check for multicast MAC address */
        if (1U == (COMMS_EtherSwtStackingARLEntries[idx].macAddr[0] & 0x1U)) {
            stackingIdx =
                COMMS_EtherSwtCheckARLPresent(&COMMS_EtherSwtARLEntries[idx],
                                              COMMS_EtherSwtStackingARLEntries,
                                              COMMS_ETHERSWT_RefreshARL.slaveARLLen);
            if (COMMS_ETHERSWT_ARL_MATCH_INVALID != stackingIdx) {
                COMMS_EtherSwtARLEntries[idx].portMask |=
                        COMMS_EtherSwtStackingARLEntries[stackingIdx].portMask;
            } else {
                /* If for a multicast addr, entry is not found at slave then  */
                /* setting portMask at master to 0 for it to be deleted below */
                COMMS_EtherSwtARLEntries[idx].portMask = 0UL;
            }
        /* For a unicast address, if the port mask at master is 0, then */
        /* search at slave. Unicast address have either master or       */
        /* slave port bit mask set.                                     */
        } else if (0UL == COMMS_EtherSwtARLEntries[idx].portMask) {
            stackingIdx =
                COMMS_EtherSwtCheckARLPresent(&COMMS_EtherSwtARLEntries[idx],
                                              COMMS_EtherSwtStackingARLEntries,
                                              COMMS_ETHERSWT_RefreshARL.slaveARLLen);
            if (COMMS_ETHERSWT_ARL_MATCH_INVALID != stackingIdx) {
                COMMS_EtherSwtARLEntries[idx].portMask =
                        COMMS_EtherSwtStackingARLEntries[stackingIdx].portMask;
            }
        } else {
            /* It is unicast and port mask is valid, no need to check at slave */
        }
    }

    /* For port masks which is zero, remove the entry from the master list */
    idx = 0UL;
    while (idx < COMMS_ETHERSWT_RefreshARL.ARLLen) {
        if (0UL == COMMS_EtherSwtARLEntries[idx].portMask) {
            if (idx != (COMMS_ETHERSWT_RefreshARL.ARLLen - 1U)) {
                BCM_MemMove(&COMMS_EtherSwtARLEntries[idx],
                            &COMMS_EtherSwtARLEntries[idx + 1UL],
                            COMMS_ETHERSWT_RefreshARL.ARLLen - idx - 1UL);
            }
            COMMS_ETHERSWT_RefreshARL.ARLLen--;
        } else {
            idx++;
        }
    }

    /* For entries present only at slave, check for non-zero port mask and add to master list */
    for (idx = 0UL;
        ((idx < COMMS_ETHERSWT_RefreshARL.slaveARLLen) &&
         ((COMMS_ETHERSWT_RefreshARL.ARLLen + 1UL) < COMMS_ARL_ENTRY_LIST_MAX));
         idx++) {
        if ((0UL == (COMMS_ETHERSWT_RefreshARL.slaveEntryMerged[idx/COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK]
                            & (1UL << (idx % COMMS_ETHERSWT_ARL_ENTRIES_PER_MASK)))) &&
            (1U != (COMMS_EtherSwtStackingARLEntries[idx].macAddr[0] & 0x1U)) &&
            (0UL != COMMS_EtherSwtStackingARLEntries[idx].portMask)) {
            BCM_MemCpy(&COMMS_EtherSwtARLEntries[COMMS_ETHERSWT_RefreshARL.ARLLen],
                       &COMMS_EtherSwtStackingARLEntries[idx], sizeof(ETHERSWT_ARLEntryType));
            COMMS_ETHERSWT_RefreshARL.ARLLen++;
        }
    }

    return;
}

/**
    API used to fetch the ARL entries from the slave and merge them
    into master ARL list.

    @code{.unparsed}
    @endcode
*/
void COMMS_EtherSwtARLRefresh()
{
    int32_t              retVal = BCM_ERR_OK;
    int32_t              status = BCM_ERR_OK;
    ETHERSWT_PayloadType PaylPtr;
    uint32_t             size = 0UL;
    uint16_t             arlEntrySz = 0U;

    MCU_ExtendedInfoType stackingInfo = {0U};
    retVal = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_SENDCMD_PROC, retVal,
                          (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    /* This unified list is managed at Master */
    if ((1U == stackingInfo.stackingEn) &&
        (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {

        PaylPtr.u8Ptr = COMMS_ETHERSWT_RefreshARL.payload;

        switch (COMMS_ETHERSWT_RefreshARL.ARLState) {
            case COMMS_ARLSTATE_RESET:
                /* Get the count of ARL entries at slave */
                retVal = RPC_CmdQSendCmd(BCM_GROUPID_COMMS, BCM_SWT_ID,
                                         ETHERSWT_CMDID_ARL_SNAPSHOT,
                                         COMMS_ETHERSWT_RefreshARL.payload,
                                         sizeof(ETHERSWT_ARLListType),
                                         ETHERSWT_ARL_REFRESH_EVENT,
                                         &COMMS_ETHERSWT_RefreshARL.pHdrSlave);
                if (BCM_ERR_OK == retVal) {
                    COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_SNAPSHOT;
                }
                break;
            case COMMS_ARLSTATE_SNAPSHOT:
                retVal = RPC_CmdQGetCmdStatus(COMMS_ETHERSWT_RefreshARL.pHdrSlave, &status,
                                              COMMS_ETHERSWT_RefreshARL.payload,
                                              &size);
                if (BCM_ERR_OK != retVal) {
                    if (BCM_ERR_BUSY == retVal) {
                        retVal = BCM_ERR_OK;
                    }
                    goto error;
                } else if (BCM_ERR_OK == status) {
                    if ((size == sizeof(ETHERSWT_ARLListType) &&
                        (0UL < PaylPtr.arlList->num))) {
                        COMMS_ETHERSWT_RefreshARL.slaveARLProcLen = 0UL;
                        COMMS_ETHERSWT_RefreshARL.slaveARLLen = PaylPtr.arlList->num;
                    } else {
                        /* If slave has no ARL, then fetch master ARL list */
                        COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_RESET;
                        COMMS_ETHERSWT_RefreshARL.slaveARLProcLen = 0UL;
                        COMMS_ETHERSWT_RefreshARL.slaveARLLen = 0UL;
                        arlEntrySz = COMMS_ARL_ENTRY_LIST_MAX;
                        /* Refresh the ARL table at master */
                        retVal = ETHERSWT_GetARLTable(ETH_SWITCH_HW_ID,
                                                      COMMS_EtherSwtARLEntries,
                                                      &arlEntrySz);
                        if (BCM_ERR_OK != retVal) {
                            goto error;
                        } else {
                            COMMS_ETHERSWT_RefreshARL.ARLLen = arlEntrySz;
                        }
                        goto error;
                    }

                    PaylPtr.arlList->num =
                                    COMMS_ETHERSWT_RefreshARL.slaveARLProcLen;

                    /* Issue command to get the ARLs from slave */
                    retVal = RPC_CmdQSendCmd(BCM_GROUPID_COMMS, BCM_SWT_ID,
                                             ETHERSWT_CMDID_ARL_GET,
                                             COMMS_ETHERSWT_RefreshARL.payload,
                                             sizeof(ETHERSWT_ARLListType),
                                             ETHERSWT_ARL_REFRESH_EVENT,
                                             &COMMS_ETHERSWT_RefreshARL.pHdrSlave);
                    if (BCM_ERR_OK == retVal) {
                        COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_GET_ARL;
                    } else {
                        COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_RESET;
                        goto error;
                    }
                }
                break;
            case COMMS_ARLSTATE_GET_ARL:
                if (NULL != COMMS_ETHERSWT_RefreshARL.pHdrSlave) {
                    /* Use COMMS_ETHERSWT_RefreshARL.pHdrSlave and check if command is previously issued */
                    retVal = RPC_CmdQGetCmdStatus(COMMS_ETHERSWT_RefreshARL.pHdrSlave, &status,
                                                  COMMS_ETHERSWT_RefreshARL.payload,
                                                  &size);
                    if (BCM_ERR_OK != retVal) {
                        if (BCM_ERR_BUSY == retVal) {
                            retVal = BCM_ERR_OK;
                        }
                        goto error;
                    } else if ((BCM_ERR_OK == status) &&
                               (PaylPtr.arlList->num > 0UL)) {
                        /* Accumulate the ARL entries from slave */
                        BCM_MemCpy(&COMMS_EtherSwtStackingARLEntries[COMMS_ETHERSWT_RefreshARL.slaveARLProcLen],
                                   &PaylPtr.arlList->entries[0],
                                   PaylPtr.arlList->num * sizeof(ETHERSWT_ARLEntryType));
                        COMMS_ETHERSWT_RefreshARL.slaveARLProcLen +=
                                   PaylPtr.arlList->num;
                        COMMS_ETHERSWT_RefreshARL.pHdrSlave = NULL;
                    }
                }

                /* Until all ARLs are received, re-issue command with updated ARL list index */
                if ((COMMS_ETHERSWT_RefreshARL.slaveARLProcLen <
                                COMMS_ETHERSWT_RefreshARL.slaveARLLen) &&
                    (COMMS_ETHERSWT_RefreshARL.pHdrSlave == NULL)) {

                    PaylPtr.arlList->num = COMMS_ETHERSWT_RefreshARL.slaveARLProcLen;
                    size = sizeof(ETHERSWT_ARLListType);

                    retVal = RPC_CmdQSendCmd(BCM_GROUPID_COMMS, BCM_SWT_ID,
                                             ETHERSWT_CMDID_ARL_GET,
                                             COMMS_ETHERSWT_RefreshARL.payload,
                                             sizeof(ETHERSWT_ARLListType),
                                             ETHERSWT_ARL_REFRESH_EVENT,
                                             &COMMS_ETHERSWT_RefreshARL.pHdrSlave);
                    if (BCM_ERR_OK != retVal) {
                        goto error;
                    }
                }

                /* If all the entries have been retrieved */
                if (COMMS_ETHERSWT_RefreshARL.slaveARLProcLen ==
                                    COMMS_ETHERSWT_RefreshARL.slaveARLLen) {
                    arlEntrySz = COMMS_ARL_ENTRY_LIST_MAX;
                    /* Get the ARL table at master */
                    retVal = ETHERSWT_GetARLTable(ETH_SWITCH_HW_ID,
                                                  COMMS_EtherSwtARLEntries,
                                                  &arlEntrySz);
                    if (BCM_ERR_OK != retVal) {
                        goto error;
                    } else {
                        COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_MERGE;
                        COMMS_ETHERSWT_RefreshARL.ARLLen = arlEntrySz;
                        /* Merge entries of master and slave ARL lists */
                        COMMS_EtherSwtARLMerge();
                    }
                }
                break;
            default:
                goto error;
                break;
        }

        if (COMMS_ARLSTATE_MERGE == COMMS_ETHERSWT_RefreshARL.ARLState) {
            COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_RESET;
            /* After merge we need to check if any ARL command from   */
            /* host is pending. We would have blocked ARL snapshot &  */
            /* ARL get command from host if ARL refresh is in         */
            /* progress. If yes, trigger CMD task                     */
            (void)BCM_SetEvent(COMMS_COMMAND_PROC_TASK,
                               COMMS_MSGQ_CMD_EVENT);
        }
    }

error:
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ETHERSWT_ARLREFRESH_PROC,
                          retVal, __LINE__, COMMS_ETHERSWT_RefreshARL.ARLState,
                          COMMS_ETHERSWT_RefreshARL.ARLLen,
                          COMMS_ETHERSWT_RefreshARL.slaveARLLen);
        COMMS_ETHERSWT_RefreshARL.ARLState = COMMS_ARLSTATE_RESET;
        COMMS_ETHERSWT_RefreshARL.slaveARLProcLen = 0UL;
    }
    return;
}

#if defined(ENABLE_CFP)
void COMMS_MonitorStreams(const COMMS_ConfigType *const aCommsCfg)
{
    uint32_t                      i = 0UL;
    CFP_StatsType                 stats;
    CFP_ActionType                action;
    int32_t                       ret;
    ETHERSWT_PayloadType          notifPayload;
    COMMS_StreamPolicerStatusType status;
    MCU_ExtendedInfoType          stackingInfo = {0U};

    notifPayload.streamPolicerStatus = &status;
    BCM_MemSet(&action, 0U, sizeof(action));
    action.dstMapIBFlags    = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);
    action.dstMapOBFlags    = (CFP_CHANGEFWDMAP_REP << CFP_ACTION_CHANGE_FWDMAP_SHIFT);

    ret = MCU_GetExtendedInfo(&stackingInfo);
    if (BCM_ERR_OK == ret) {
        for(; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
            if ((CFP_MAX_RULES != COMMS_StreamData.streams[i].row) &&
                (FALSE == COMMS_StreamData.streams[i].blocked) &&
                (aCommsCfg->streamPolicers[i].action != 0U)) {

                COMMS_StreamData.streams[i].tickCount += COMMS_StreamData.currentIntervalTicks;
                if (COMMS_StreamData.streams[i].tickCount >= aCommsCfg->streamPolicers[i].monitoringInterval) {
                    COMMS_StreamData.streams[i].tickCount = 0UL;
                    ret = CFP_GetStats(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[i].row, &stats);
                    if (BCM_ERR_OK != ret) {
                        COMMS_ErrorReport(BRCM_SWARCH_COMMS_MONITORSTREAMS_PROC, ret,
                                (uint32_t)__LINE__, i, 0UL, 0UL);
                        break;
                    }
                    if (stats.red > aCommsCfg->streamPolicers[i].dropThreshold) {
                        if (0U != (aCommsCfg->streamPolicers[i].action & COMMS_STREAMPOLICERACTION_BLOCK_MASK)) {
                            ret = CFP_UpdateRule(ETH_SWITCH_HW_ID, COMMS_StreamData.streams[i].row, &action);
                            if (BCM_ERR_OK != ret) {
                                COMMS_ErrorReport(BRCM_SWARCH_COMMS_MONITORSTREAMS_PROC, ret,
                                        (uint32_t)__LINE__, i, stats.red, aCommsCfg->streamPolicers[i].dropThreshold);
                                break;
                            } else {
                                COMMS_StreamData.streams[i].blocked = TRUE;
                            }
                        }
                        if (0U != (aCommsCfg->streamPolicers[i].action & COMMS_STREAMPOLICERACTION_REPORT_MASK)) {
                            status.vlan = CommsConfig.arlTable[CommsConfig.streamPolicers[i].arlEntryIndex].vlan;
                            BCM_MemCpy(status.macAddress,
                                    CommsConfig.arlTable[CommsConfig.streamPolicers[i].arlEntryIndex].macAddress,
                                    sizeof(6U));
                            status.portMask = COMMS_ConvertToUnifiedPortMask(&stackingInfo,
                                    CommsConfig.streamPolicers[i].portMask);
                            status.idx = i;
                            status.blocked = COMMS_StreamData.streams[i].blocked;

                            ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_SWT_ID,
                                    ETHERSWT_EVENT_STREAM_EXCEEDED,
                                    notifPayload.u8Ptr,
                                    sizeof(COMMS_StreamPolicerStatusType));
                            if (BCM_ERR_OK != ret) {
                                COMMS_ErrorReport(BRCM_SWARCH_COMMS_MONITORSTREAMS_PROC , ret,
                                        (uint32_t)__LINE__, i, stats.red, aCommsCfg->streamPolicers[i].dropThreshold);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void COMMS_SetStreamMonitorAlarm(const COMMS_ConfigType *const aCommsCfg)
{
    uint32_t interval = 0UL;
    uint32_t i = 0UL;

    for (; i < COMMS_MAX_STREAM_POLICER_ENTRIES; ++i) {
        if ((aCommsCfg->streamPolicers[i].action != 0U) &&
            (0UL != aCommsCfg->streamPolicers[i].monitoringInterval) &&
            (CFP_MAX_RULES != COMMS_StreamData.streams[i].row)) {
            if (0UL == interval) {
                interval = aCommsCfg->streamPolicers[i].monitoringInterval;
            } else {
                interval = BCM_GCD(interval, aCommsCfg->streamPolicers[i].monitoringInterval);
            }
        }
    }

    if (interval != COMMS_StreamData.currentIntervalTicks) {
        if (0UL != COMMS_StreamData.currentIntervalTicks) {
            BCM_CancelAlarm(StreamMonitor);
        }
        if (0UL != interval) {
            /* Start periodic alarm with the GCD as the cycleTime */
            BCM_SetRelAlarm(StreamMonitor, interval, interval);
        }
        COMMS_StreamData.currentIntervalTicks = interval;
    }
}
#endif
/** @} */
