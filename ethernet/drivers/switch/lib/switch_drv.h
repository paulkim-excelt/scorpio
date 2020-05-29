/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
#ifndef SWITCH_DRV_H
#define SWITCH_DRV_H

#include <stdint.h>

extern int SwitchDrv_Init (ETHERSWT_HwIDType aID,
        const ETHERSWT_CfgType *const aConfig,
        uint32_t *const aPort2TimeFifoMap);

extern int32_t SwitchDrv_ReadReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t *const aVal);

extern int32_t SwitchDrv_WriteReg (ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t aVal);

extern int32_t SwitchDrv_GetARLTable(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLTbl,
        uint16_t *const aTblSize);

extern int32_t SwitchDrv_GetRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_RxStatsType *const aRxStat);

extern int32_t SwitchDrv_GetTxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_TxStatsType *const aTxStat);

extern int32_t SwitchDrv_ClearRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID);

extern int32_t SwitchDrv_ClearTxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID);

extern int32_t SwitchDrv_GetPortMacAddr(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID,
        const uint8_t *const aMacAddr,
        ETHERSWT_VLANIDType aVlanID);

extern int32_t SwitchDrv_SetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode aMode);

extern int32_t SwitchDrv_GetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode *const aMode);

extern int32_t SwitchDrv_EnableVLAN(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aEnable);

extern int32_t SwitchDrv_SetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        const ETHERSWT_PortMirrorCfgType *const aConfig);

extern int32_t SwitchDrv_GetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig);

extern int32_t SwitchDrv_SetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType aState);

extern int32_t SwitchDrv_GetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType *const aState);

extern int32_t SwitchDrv_TxAdaptBuffer(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut);

extern int32_t SwitchDrv_SetMgmtInfo(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        const ETHERSWT_MgmtInfoType * const aMgmtInfo);

extern int32_t SwitchDrv_TxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut);

extern int32_t SwitchDrv_TxDoneInd (ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx, ETHERSWT_MgmtInfoType **aMgmtInfo,
        ETHER_TimestampType *aTs, ETHER_TimestampQualType *aTsQual);

extern int32_t SwitchDrv_TxDoneIndComplete (uint32_t aBufIdx);

extern int32_t SwitchDrv_RxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t **const aDataInOut,
        uint32_t * const aLenInOut,
        uint32_t *const aIsMgmtFrameOnly);

extern int32_t SwitchDrv_RxDoneInd (ETHER_HwIDType aCntrlID, uint32_t aBufIdx,
        uint8_t **aBuf, ETHERSWT_MgmtInfoType **aMgmtInfo,
        ETHER_TimestampType *aTs, ETHER_TimestampQualType *aTsQual,
        uint32_t *aTSAvailable);

extern int32_t SwitchDrv_RxDoneIndComplete (uint32_t aBufIdx);

extern int32_t SwitchDrv_EnableTxTimestamp(ETHER_HwIDType aSwtID,
        uint32_t aBufIdx,
        const ETHERSWT_MgmtInfoType * const aMgmtInfo);

extern int32_t SwitchDrv_SetTSEnabled(ETHER_HwIDType aSwtID,
                            uint32_t aBufIdx);
extern int32_t SwitchDrv_SetLedState(ETHERSWT_HwIDType aID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_LedType aLedType,
        uint32_t aTurnOn);

extern int32_t SwitchDrv_GetLedState(ETHERSWT_HwIDType aID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_LedType aLedType,
        uint32_t *const aIsStateOn);

extern int32_t SwitchDrv_SetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode);

extern int32_t SwitchDrv_GetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode);

extern int32_t SwitchDrv_SetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType aMode);

extern int32_t SwitchDrv_GetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType *const aMode);

extern int32_t SwitchDrv_LinkIRQHandler(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID);

extern int32_t SwitchDrv_LinkStatChgIndHandler(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState,
        uint32_t *const aIsLinkStateChanged);

extern int32_t SwitchDrv_GetXcvrStats(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortStatsType *const aStats);

extern int32_t SwitchDrv_GetPortLinkState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState);

extern int32_t SwitchDrv_GetVLANPorts(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t *const aPortMask,
        uint32_t *const aTagMask,
        uint32_t *const aStaticPortMask);

extern int32_t SwitchDrv_AddVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aTaggedMask);

extern int32_t SwitchDrv_RemoveVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID);

extern int32_t SwitchDrv_SetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        ETHER_PrioType aPrio);

extern int32_t SwitchDrv_GetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType *const aVlanID,
        ETHER_PrioType *const aPrio);

extern int32_t SwitchDrv_SetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType aMode);

extern int32_t SwitchDrv_GetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType *const aMode);

extern int32_t SwitchDrv_SetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t aAge);

extern int32_t SwitchDrv_GetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t *const aAge);

extern int32_t SwitchDrv_AddARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry);

extern int32_t SwitchDrv_DeleteARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry);

extern int32_t SwitchDrv_GetMirrorCapturePort(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID);

#endif /* SWITCH_DRV_H */
