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
    @addtogroup grp_nif_impl
    @{
    @file nif_switch.c
    @brief NIF switch Implementation file
    This source file contains the implementation of NIF switch layer
    @version 0.1 Initial version
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <system.h>
#include <utils.h>
#include <atomic.h>
#include <nif.h>
#include <nif_switch.h>
#include <eth_osil.h>
#include <eth_switch_osil.h>
#include "nif_internal.h"

/**
    @name NIF Design IDs
    @{
    @brief Design IDs for NIF
*/
#define BRCM_SWDSGN_ETHERSWT_RXMGMTINFOIND_PROC          (0xC0U) /**< @brief #ETHERSWT_MgmtInfoInd */
#define BRCM_SWDSGN_ETHERSWT_TXTSIND_PROC                (0xC1U) /**< @brief #ETHERSWT_TxTSInd */
#define BRCM_SWDSGN_ETHERSWT_RXTSIND_PROC                (0xC2U) /**< @brief #ETHERSWT_RxTSInd */
#define BRCM_SWDSGN_ETHERSWT_LINKSTATECHANGEIND_PROC     (0xC3U) /**< @brief #ETHERSWT_LinkStateChangeInd */
#define BRCM_SWDSGN_NIF_SWTSETMGMTINFO_PROC              (0xC4U) /**< @brief #NIF_SwtSetMgmtInfo */
/** @} */

extern NIF_CompType NIF_Comp;
extern const NIF_CtrlCfgType *NIF_CtrlCfg;

/**
    Set the management information

    @trace #BRCM_SWARCH_NIF_SWTSETMGMTINFO_PROC
    @trace #BRCM_SWREQ_NIF_SWITCH_MANAGEMENT_INFO

    @code{.c}
    @endcode
*/
int32_t NIF_SwtSetMgmtInfo(NIF_CntrlIDType aCtrlIdx,
                           uint32_t aBufIdx,
                           const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    int32_t retVal = BCM_ERR_OK;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if (NULL == ethFn) {
        goto done;
    }

    if (NIF_BUF_COUNT <= aBufIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SWTSETMGMTINFO_PROC, retVal,
                        aBufIdx, __LINE__);
        goto done;
    }

    if (NIF_BUFSTATE_ALLOC != NIF_Comp.txBufInfo[aBufIdx].bufState) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SWTSETMGMTINFO_PROC, retVal,
                        aBufIdx, NIF_Comp.txBufInfo[aBufIdx].bufState);
        goto done;
    }

    NIF_Comp.txBufInfo[aBufIdx].mgmtInfo = *aMgmtInfo;
    retVal = ETHERSWT_SetMgmtInfo(ethCtrlIdx, aBufIdx,
                    &(NIF_Comp.txBufInfo[aBufIdx].mgmtInfo));
done:
    return retVal;
}

/**
    Set the management information

    @trace #BRCM_SWARCH_NIF_GETLINKSTATE_PROC
    @trace #BRCM_SWREQ_NIF_QUERY

    @code{.c}
    @endcode
*/
int32_t NIF_GetLinkState(uint32_t aIdx,
                        ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t retVal = BCM_ERR_OK;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        retVal = ETHERSWT_GetPortLinkState(0UL, aIdx, aLinkState);
    }

    return retVal;
}

/**
    Switch Management information call back

    @trace #BRCM_SWARCH_ETHERSWT_MGMTINFOIND_PROC
    @trace #BRCM_SWREQ_NIF_SWITCH_MANAGEMENT_INFO

    @code{.c}
    @endcode
*/
void ETHERSWT_MgmtInfoInd(ETHER_HwIDType aCntrlID,
                          const uint8_t *const aData,
                          const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
    uint32_t i;
    NIF_CntrlIDType ctrlIdx;
    const uint8_t *buf = NIF_Comp.rxBufInfo.clientBuf;
    ETHERSWT_VLANIDType vlanID = NIF_Comp.rxBufInfo.vlanID;
    ETHER_EtherType etherType = NIF_Comp.rxBufInfo.etherType;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_MGMT_INFO_IND_PROC, BCM_ERR_UNINIT,
                        NIF_Comp.state, __LINE__);
        goto done;
    }

    if (aCntrlID != NIF_CtrlCfg->hwIdx) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_MGMT_INFO_IND_PROC,
                        BCM_ERR_INVAL_PARAMS, aCntrlID, __LINE__);
        goto done;
    }

    if ((NULL != aData) && (NULL != aMgmtInfo)) {
        /* Save Rx driver buffer pointer */
        if (aData != NIF_Comp.rxBufInfo.drvBuf) {
            NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_MGMT_INFO_IND_PROC,
                    BCM_ERR_INVAL_PARAMS, (uint32_t)aData,
                    (uint32_t)NIF_Comp.rxBufInfo.drvBuf);
            goto done;
        }

        for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
            if (etherType == NIF_CtrlCfg->cbTbl[i].etherType) {
                ctrlIdx = NIF_CTRL(vlanID,
                                   NIF_CtrlCfg->cbTbl[i].clientID,
                                   NIF_CtrlCfg->cbTbl[i].cntrlIndex);
                if (NIF_CtrlCfg->cbTbl[i].rxMgmtInfoCb != NULL) {
                    NIF_CtrlCfg->cbTbl[i].rxMgmtInfoCb(ctrlIdx, buf, aMgmtInfo);
                }
                break;
            }
        }
    }

done:
    return ;
}

/**
    Callback from switch driver to provide timestamp of TX packet

    @trace #BRCM_SWARCH_ETHERSWT_TXTSIND_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
void ETHERSWT_TxTSInd(ETHER_HwIDType aCntrlID,
                      uint32_t aBufIdx,
                      const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                      const ETHER_TimestampType *const aTS,
                      const ETHER_TimestampQualType* const aTSQual)
{
    uint32_t i;
    NIF_TxBufInfoType     *txBufInfo = &NIF_Comp.txBufInfo[aBufIdx];
    ETHER_EtherType       etherType;
    NIF_ClientIDType      clientID;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_TX_TS_IND_PROC,
                        BCM_ERR_UNINIT, NIF_Comp.state, __LINE__);
        goto done;
    }

    if (aCntrlID != NIF_CtrlCfg->hwIdx) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_TX_TS_IND_PROC,
                        BCM_ERR_INVAL_PARAMS, aCntrlID,  __LINE__);
        goto done;
    }

    NIF_GetClientID(txBufInfo->ctrlIdx, &clientID);
    etherType = txBufInfo->etherType;

    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        if ((etherType == NIF_CtrlCfg->cbTbl[i].etherType) &&
            (clientID == NIF_CtrlCfg->cbTbl[i].clientID)) {
            if (TRUE == txBufInfo->isTSed) {
                txBufInfo->isTSed = FALSE;
                if (NIF_CtrlCfg->cbTbl[i].txTSCb != NULL) {
                    NIF_CtrlCfg->cbTbl[i].txTSCb(txBufInfo->ctrlIdx,
                            aBufIdx,
                            &(txBufInfo->mgmtInfo),
                            aTS, aTSQual);
                }
            }
            break;
        }
    }
done:
    return ;
}

/**
    Callback from switch driver to provide timestamp of RX packet

    @trace #BRCM_SWARCH_ETHERSWT_RXTSIND_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
void ETHERSWT_RxTSInd(ETHER_HwIDType aCntrlID,
                      const uint8_t *const aData,
                      const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                      const ETHER_TimestampType *const aTS,
                      const ETHER_TimestampQualType* const aTSQual)
{
    uint32_t i;
    NIF_CntrlIDType ctrlIdx;
    const uint8_t *buf = NIF_Comp.rxBufInfo.clientBuf;
    ETHERSWT_VLANIDType vlanID = NIF_Comp.rxBufInfo.vlanID;
    ETHER_EtherType etherType = NIF_Comp.rxBufInfo.etherType;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_RX_TS_IND_PROC, BCM_ERR_UNINIT,
                        NIF_Comp.state, __LINE__);
        goto done;
    }

    if (aCntrlID != NIF_CtrlCfg->hwIdx) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_RX_TS_IND_PROC,
                        BCM_ERR_INVAL_PARAMS, aCntrlID, __LINE__);
        goto done;
    }

    if ((NULL != aData) && (NULL != aMgmtInfo)) {
        /* Save Rx driver buffer pointer */
        if (aData != NIF_Comp.rxBufInfo.drvBuf) {
            NIF_ReportError(0ULL, BRCM_SWARCH_ETHERSWT_IL_RX_TS_IND_PROC,
                            BCM_ERR_INVAL_PARAMS, (uint32_t)aData,
                           (uint32_t)NIF_Comp.rxBufInfo.drvBuf);
            goto done;
        }

        for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
            if (etherType == NIF_CtrlCfg->cbTbl[i].etherType) {
                if (NIF_CtrlCfg->cbTbl[i].rxTSCb != NULL) {
                    ctrlIdx = NIF_CTRL(vlanID,
                                       NIF_CtrlCfg->cbTbl[i].clientID,
                                       NIF_CtrlCfg->cbTbl[i].cntrlIndex);
                    NIF_CtrlCfg->cbTbl[i].rxTSCb(ctrlIdx, buf, aMgmtInfo,
                            aTS, aTSQual);
                }
                break;
            }
        }
    }

done:
    return ;
}

/**
    Port link state change indication callback.

    @trace #BRCM_SWARCH_ETHERSWT_LINKSTATECHANGEIND_PROC
    @trace #BRCM_SWREQ_NIF_CONFIG

    @code{.c}
    @endcode
*/
void ETHERSWT_LinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
                                 ETHERSWT_PortIDType aPortID,
                                 ETHXCVR_LinkStateType aLinkState)
{
    uint32_t i, j;
    NIF_CntrlIDType ctrlIdx;
    ETHERSWT_VLANIDType vlanID;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        goto done;
    }

    NIF_ReportError(0ULL, 0U, /* @todo */
                    BCM_ERR_OK, aPortID, aLinkState);
    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        vlanID = 0U;
        ctrlIdx = NIF_CTRL(vlanID,
                           NIF_CtrlCfg->cbTbl[i].clientID,
                           NIF_CtrlCfg->cbTbl[i].cntrlIndex);
        if (NIF_CtrlCfg->cbTbl[i].linkStateChgCb != NULL) {
            NIF_CtrlCfg->cbTbl[i].linkStateChgCb(ctrlIdx, aLinkState);
        }
        for (j = 0UL; j < NIF_CtrlCfg->vlanArraySize; j++) {
            ctrlIdx = NIF_CTRL(NIF_CtrlCfg->vlanArray[j],
                               NIF_CtrlCfg->cbTbl[i].clientID,
                               NIF_CtrlCfg->cbTbl[i].cntrlIndex);
            if (NIF_CtrlCfg->cbTbl[i].linkStateChgCb != NULL) {
                NIF_CtrlCfg->cbTbl[i].linkStateChgCb(ctrlIdx, aLinkState);
            }
        }
    }
done:
    return ;
}

/** @} */
