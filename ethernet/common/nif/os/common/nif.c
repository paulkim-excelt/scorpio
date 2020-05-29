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

/**
    @addtogroup grp_nif_impl
    @{
    @section sec_nif_overview NIF Overview

    Network Interface Layer (NIF) is an abstraction layer for the
    underlying network drivers such as Ethernet driver, Switch driver,
    transciever driver and the wirless driver. This layer abstracts all
    the client modules from the underlying network drivers.

    @file nif.c
    @brief NIF Implementation file
    This source file contains the implementation of NIF layer
    @version 0.1 Initial version
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <system.h>
#include <utils.h>
#include <atomic.h>
#include <nif.h>
#include <eth_osil.h>
#include <eth_switch_osil.h>
#include <ethxcvr_osil.h>
#if defined(ENABLE_WLAN)
#include <wl_eth.h>
#include <wl_eth_xcvr.h>
#endif
#include <ulog.h>
#if defined (ENABLE_FQTSS)
#include <fqtss.h>
#endif
#include "nif_internal.h"

/**
    @name NIF Design IDs
    @{
    @brief Design IDs for NIF
*/
#define BRCM_SWDSGN_NIF_INIT_PROC                        (0x80U) /**< @brief #NIF_Init */
#define BRCM_SWDSGN_NIF_GETCTRLIDX_PROC                  (0x81U) /**< @brief #NIF_GetCtrlIdx */
#define BRCM_SWDSGN_NIF_SETCTRLMODE_PROC                 (0x82U) /**< @brief #NIF_SetCtrlMode */
#define BRCM_SWDSGN_NIF_GETCTRLMODE_PROC                 (0x83U) /**< @brief #NIF_GetCtrlMode */
#define BRCM_SWDSGN_NIF_GETMACADDR_PROC                  (0x84U) /**< @brief #NIF_GetMacAddr */
#define BRCM_SWDSGN_NIF_GETVLANID_PROC                   (0x85U) /**< @brief #NIF_GetVlanID */
#define BRCM_SWDSGN_NIF_GETTXBUFFER_PROC                 (0x86U) /**< @brief #NIF_GetTxBuffer */
#define BRCM_SWDSGN_NIF_SEND_PROC                        (0x87U) /**< @brief #NIF_Send */
#define BRCM_SWDSGN_NIF_ENABLETXTIMESTAMP_PROC           (0x88U) /**< @brief #NIF_EnableTxTimestamp */
#define BRCM_SWDSGN_NIF_GETTIME_PROC                     (0x89U) /**< @brief #NIF_GetTime */
#define BRCM_SWDSGN_NIF_SETTIME_PROC                     (0x8AU) /**< @brief #NIF_SetTime */
#define BRCM_SWDSGN_NIF_SETCORRECTIONTIME_PROC           (0x8BU) /**< @brief #NIF_SetCorrectionTime */
#define BRCM_SWDSGN_NIF_GETLINKSTATE_PROC                (0x8CU) /**< @brief #NIF_GetLinkState */
#define BRCM_SWDSGN_NIF_XCVRGETSQIVALUE_PROC             (0x8DU) /**< @brief #NIF_XcvrGetSQIValue */
#define BRCM_SWDSGN_NIF_ENABLEAVBSHAPING_PROC            (0x8EU) /**< @brief #NIF_EnableAVBShaping */
#define BRCM_SWDSGN_NIF_DISABLEAVBSHAPING_PROC           (0x8FU) /**< @brief #NIF_DisableAVBShaping */
#define BRCM_SWDSGN_NIF_PROCESSETHERNETTIMER_PROC        (0x90U) /**< @brief #NIF_ProcessEthernetTimer */
#define BRCM_SWDSGN_NIF_ARECLIENTSWAITINGFORBUFFERS_PROC (0x91U) /**< @brief #NIF_AreClientsWaitingForBuffers */
#define BRCM_SWDSGN_ETHER_RXIND_PROC                     (0x92U) /**< @brief #ETHER_RxInd */
#define BRCM_SWDSGN_ETHER_TXDONEIND_PROC                 (0x93U) /**< @brief #ETHER_TxDoneInd */
#define BRCM_SWDSGN_ETHXCVR_LINKSTATECHANGEIND_PROC      (0x94U) /**< @brief #ETHXCVR_LinkStateChangeInd */
#define BRCM_SWDSGN_NIF_MAX_CLIENTS_MACRO                (0x95U) /**< @brief #NIF_MAX_CLIENTS */
#define BRCM_SWDSGN_NIF_CTRL_ETHER_CTRL_MASK_MACRO       (0x96U) /**< @brief #NIF_CTRL_ETHER_CTRL_MASK */
#define BRCM_SWDSGN_NIF_CTRL_ETHER_CTRL_SHIFT_MACRO      (0x97U) /**< @brief #NIF_CTRL_ETHER_CTRL_SHIFT */
#define BRCM_SWDSGN_NIF_CTRL_VLAN_ID_MASK_MACRO          (0x98U) /**< @brief #NIF_CTRL_VLAN_ID_MASK */
#define BRCM_SWDSGN_NIF_CTRL_VLAN_ID_SHIFT_MACRO         (0x99U) /**< @brief #NIF_CTRL_VLAN_ID_SHIFT */
#define BRCM_SWDSGN_NIF_CTRL_INST_ID_MASK_MACRO          (0x9AU) /**< @brief #NIF_CTRL_INST_ID_MASK */
#define BRCM_SWDSGN_NIF_CTRL_INST_ID_SHIFT_MACRO         (0x9BU) /**< @brief #NIF_CTRL_INST_ID_SHIFT */
#define BRCM_SWDSGN_NIF_CTRL_COMP_ID_MASK_MACRO          (0x9CU) /**< @brief #NIF_CTRL_COMP_ID_MASK */
#define BRCM_SWDSGN_NIF_CTRL_COMP_ID_SHIFT_MACRO         (0x9DU) /**< @brief #NIF_CTRL_COMP_ID_SHIFT */
#define BRCM_SWDSGN_NIF_VLAN_TAG_SIZE_MACRO              (0x9EU) /**< @brief #NIF_VLAN_TAG_SIZE */
#define BRCM_SWDSGN_NIF_ETHERTYPE_SIZE_MACRO             (0x9FU) /**< @brief #NIF_ETHERTYPE_SIZE */
#define BRCM_SWDSGN_NIF_TXBUFINFO_GLOBAL                 (0xAAU) /**< @brief #NIF_TxBufInfo */
#define BRCM_SWDSGN_NIF_COMP_GLOBAL                      (0xABU) /**< @brief #NIF_Comp */
#define BRCM_SWDSGN_NIF_CTRL_MACRO                       (0xACU) /**< @brief #NIF_CTRL */
#define BRCM_SWDSGN_NIF_CTRLCONFIG_GLOBAL                (0xADU) /**< @brief #NIF_CtrlConfig */
#define BRCM_SWDSGN_NIF_CTRLCFG_GLOBAL                   (0xAEU) /**< @brief #NIF_CtrlCfg */
#define BRCM_SWDSGN_NIF_REPORTERROR_PROC                 (0xAFU) /**< @brief #NIF_ReportError */
#define BRCM_SWDSGN_NIF_CHECKCONFIG_PROC                 (0xB0U) /**< @brief #NIF_CheckConfig */
#define BRCM_SWDSGN_NIF_GETCLIENTID_PROC                 (0xB1U) /**< @brief #NIF_GetClientID */
#define BRCM_SWDSGN_NIF_FINDSTREAMIDX_PROC               (0xB2U) /**< @brief #NIF_FindStreamIdx */
#define BRCM_SWDSGN_NIF_GETETHCONTROLLERID_PROC          (0xB3U) /**< @brief #NIF_GetEthControllerID */
/** @} */

/**
    @brief Maximum supported NIF clients

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_MAX_CLIENTS         (16UL)

/**
    @brief NIF VLAN tag size

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_VLAN_TAG_SIZE               (4UL)

/**
    @brief NIF ethertype size

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_ETHERTYPE_SIZE              (2UL)

/**
    @brief Allocate Tx buffer memory

    Allocate Tx buffer memory

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWARCH_NIF_TXBUFINFO_TYPE
    @trace #BRCM_SWREQ_NIF_INIT
*/
static NIF_TxBufInfoType NIF_TxBufInfo[NIF_BUF_COUNT] COMP_SECTION(".data.nif") = {{0}};

/**
    @brief NIF context memory

    NIF context memory

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWARCH_NIF_COMP_TYPE
    @trace #BRCM_SWREQ_NIF_INIT
*/
NIF_CompType NIF_Comp COMP_SECTION(".data.nif") = {
    .state = NIF_STATE_UNINIT,
    .txBufInfo = NIF_TxBufInfo,
    .isWaitingForBuffer = 0UL,
    .currentTimerPeriod = 0xFFFFFFFFUL,
    .totalAVBReserved = 0UL,
};

/**
    @brief NIF configuration memory

    NIF configuration memory

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWREQ_NIF_INIT
*/
extern const NIF_CtrlCfgType NIF_CtrlConfig;

/**
    @brief NIF configuration

    NIF configuration

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWREQ_NIF_INIT
*/
const NIF_CtrlCfgType *NIF_CtrlCfg = &NIF_CtrlConfig;

/** @brief NIF report error

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC
    @trace #BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC
    @trace #BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC
    @trace #BRCM_SWREQ_NIF_INIT

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.
    @image html comp_flowchart.jpg

    @code{.unparsed}
    @endcode
*/
void  NIF_ReportError(NIF_CntrlIDType aCtrlIdx,
                      uint8_t aApiID, int32_t aError,
                      uint32_t aVal0, uint32_t aVal1)
{
    const uint32_t values[4] = {(uint32_t)(aCtrlIdx >> 32ULL),
                                (uint32_t)aCtrlIdx, aVal0, aVal1};
    BCM_ReportError(BCM_NIF_ID, 0U, aApiID, aError, 4UL, values);
}

/** @brief NIF check configuration

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWREQ_NIF_INIT

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.
    @image html comp_flowchart.jpg

    @code{.unparsed}
    @endcode
*/
static int32_t NIF_CheckConfig(const NIF_CfgType *const aConfig)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;

    /* Check controller config */
    if (NULL == NIF_CtrlCfg) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    if ((NULL == NIF_CtrlCfg->vlanArray) &&
        (0UL != NIF_CtrlCfg->vlanArraySize)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    for (i = 0UL; i < NIF_CtrlCfg->vlanArraySize; i++) {
        if ((NIF_CtrlCfg->vlanArray[i] < ETHER_VLANID_MIN)
                || (NIF_CtrlCfg->vlanArray[i] > ETHER_VLANID_MAX)) {
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    if ((NULL == NIF_CtrlCfg->cbTbl) && (0UL != NIF_CtrlCfg->cbTblSize)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    if (NIF_CtrlCfg->cbTblSize > NIF_MAX_CLIENTS) {
        retVal = BCM_ERR_NOMEM;
        goto done;
    }

    if (((NULL == NIF_CtrlCfg->ethIntfTbl) && (0UL != NIF_CtrlCfg->ethIntfTblSize)) ||
        ((NULL == NIF_CtrlCfg->wlEthIntfTbl) && (0UL != NIF_CtrlCfg->wlEthIntfTblSize)) ||
        ((NULL == NIF_CtrlCfg->xcvrIntfTbl) && (0UL != NIF_CtrlCfg->xcvrIntfTblSize)) ||
        ((NULL == NIF_CtrlCfg->wlXcvrIntfTbl) && (0UL != NIF_CtrlCfg->wlXcvrIntfTblSize)) ||
        ((NULL == NIF_CtrlCfg->swtIntfTbl) && (0UL != NIF_CtrlCfg->swtIntfTblSize))){
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }
done:
    return retVal;
}

/** @brief NIF get client ID

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC
    @trace #BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC
    @trace #BRCM_SWARCH_NIF_TXDONECLIENTCB_TYPE
    @trace #BRCM_SWARCH_NIF_TXTSCLIENTCB_TYPE
    @trace #BRCM_SWREQ_NIF_INIT

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.
    @image html comp_flowchart.jpg

    @code{.unparsed}
    @endcode
*/
void  NIF_GetClientID(NIF_CntrlIDType aCtrlIdx,
                      NIF_ClientIDType *const aClientID)
{
    uint16_t compID;
    uint8_t instID;

    if (aClientID != NULL) {
        compID = ((uint16_t)(((aCtrlIdx) & NIF_CTRLVAL_COMP_ID_MASK)
                              >> NIF_CTRLVAL_COMP_ID_SHIFT));
        instID = ((uint8_t)(((aCtrlIdx) & NIF_CTRLVAL_INST_ID_MASK)
                              >> NIF_CTRLVAL_INST_ID_SHIFT));
        *aClientID = NIF_CLIENT_ID(compID, instID);
    }
}

/** @brief NIF get controller ID

    @trace #BRCM_SWARCH_NIF_SETCTRLMODE_PROC
    @trace #BRCM_SWARCH_NIF_GETCTRLMODE_PROC
    @trace #BRCM_SWARCH_NIF_GETMACADDR_PROC
    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC
    @trace #BRCM_SWARCH_NIF_GETTIME_PROC
    @trace #BRCM_SWARCH_NIF_SETTIME_PROC
    @trace #BRCM_SWARCH_NIF_SETCORRECTIONTIME_PROC
    @trace #BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC
    @trace #BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC
    @trace #BRCM_SWREQ_NIF_INIT

    @code{.unparsed}
    @endcode
*/
const NIF_EthIntfType* NIF_GetEthControllerID(NIF_CntrlIDType aCtrlIdx)

{
    NIF_EthCntrlrType ctrlType;
    NIF_EthCntrlrIdxType ctrlIndex;
    const NIF_EthIntfType *funcPtr = NULL;

    ctrlType = ((uint32_t)(((aCtrlIdx) & NIF_CTRLVAL_CNTRLRTYPE_MASK)
                                    >> NIF_CTRLVAL_CNTRLRTYPE_SHIFT));
    ctrlIndex = ((uint32_t)(((aCtrlIdx) & NIF_CTRLVAL_CNTRLRID_MASK)
                                    >> NIF_CTRLVAL_CNTRLRID_SHIFT));

    /* Check for configured type and index to be valid values and avoid */
    /* out of bound access                                              */
    if(((NIF_ETHCNTRLR_WIRED == ctrlType) &&
        (NIF_CtrlCfg->ethIntfTblSize <= ctrlIndex)) ||
       ((NIF_ETHCNTRLR_WIRELESS == ctrlType) &&
        (NIF_CtrlCfg->wlEthIntfTblSize <= ctrlIndex))){
       goto done;
    }

    switch(ctrlType){
        case NIF_ETHCNTRLR_WIRED:
            funcPtr = &NIF_CtrlCfg->ethIntfTbl[ctrlIndex];
            break;
        case NIF_ETHCNTRLR_WIRELESS:
            funcPtr = &NIF_CtrlCfg->wlEthIntfTbl[ctrlIndex];
            break;
        default:
            break;
    }

done:
    return funcPtr;
}

/** @brief NIF find stream index

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC
    @trace #BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC
    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.
    @image html comp_flowchart.jpg

    @code{.unparsed}
    @endcode
*/
static int32_t  NIF_FindStreamIdx(NIF_ClientIDType aClientID,
                                  uint32_t *const aStreamIdx)
{
    int32_t   retVal = BCM_ERR_NOT_FOUND;
    uint32_t  i = 0UL;

    for (i = 0UL; i < NIF_CtrlCfg->streamTblSize; ++i) {
        if (aClientID == NIF_CtrlCfg->streamTbl[i]) {
            *aStreamIdx = i;
            retVal = BCM_ERR_OK;
            break;
        }
    }

    return retVal;
}

/**
    NIF Initialization function

    @trace #BRCM_SWARCH_NIF_INIT_PROC
    @trace #BRCM_SWREQ_NIF_INIT

    @code{.c}
    @endcode
*/
int32_t NIF_Init(const NIF_CfgType *const aConfig)
{
    int32_t retVal;
    uint32_t i;

    if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    if (NIF_STATE_UNINIT != NIF_Comp.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto done;
    }

    /* Check config parameters */
    retVal = NIF_CheckConfig(aConfig);
    if (BCM_ERR_OK == retVal) {

        for (i = 0UL; i < NIF_BUF_COUNT; i++) {
            NIF_Comp.txBufInfo[i].bufState = NIF_BUFSTATE_FREE;
            NIF_Comp.txBufInfo[i].etherType = 0U;
            NIF_Comp.txBufInfo[i].size      = 0UL;
            NIF_Comp.txBufInfo[i].ctrlIdx   = 0ULL;
        }

        /* Change the state to INIT */
        NIF_Comp.state = NIF_STATE_INIT;
    }

done:
    return retVal;
}

/**
    Get NIF controller index

    @trace #BRCM_SWARCH_NIF_GETCTRLIDX_PROC
    @trace #BRCM_SWREQ_NIF_TRANSMIT

    @code{.c}
    @endcode
*/
int32_t NIF_GetCtrlIdx(ETHER_VLANIDType aVlanID,
                       uint16_t aCompID,
                       uint8_t aInstID,
                       NIF_CntrlIDType *const aCtrlIdx)
{
    int32_t retVal = BCM_ERR_OK;
    NIF_ClientIDType clientID = NIF_CLIENT_ID(aCompID, aInstID);
    NIF_CntrlIDType ctrlIdx;
    NIF_CntrlrValType ethCtrlIdx = NIF_CNTRLRVAL_INVALID;
    uint32_t i;

    /* Find the ethernet controller for the client */
    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        if (clientID == NIF_CtrlCfg->cbTbl[i].clientID) {
            ethCtrlIdx = NIF_CtrlCfg->cbTbl[i].cntrlIndex;
            break;
        }
    }

    ctrlIdx = NIF_CTRL(aVlanID, clientID, ethCtrlIdx);

    if ((NULL == aCtrlIdx) || (ethCtrlIdx == NIF_CNTRLRVAL_INVALID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(ctrlIdx, BRCM_SWARCH_NIF_GETCTRLIDX_PROC, retVal, 0UL, __LINE__);
        goto done;
    }

    if (NIF_STATE_INIT != NIF_Comp.state) {
        retVal = BCM_ERR_UNINIT;
        goto done;
    }

    *aCtrlIdx = ctrlIdx;

done:
    return retVal;
}

/**
    Set NIF controller mode

    @trace #BRCM_SWARCH_NIF_SETCTRLMODE_PROC
    @trace #BRCM_SWREQ_NIF_INIT

    @code{.c}
    @endcode
*/
int32_t NIF_SetCtrlMode(NIF_CntrlIDType aCtrlIdx,
                        ETHER_ModeType aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL != ethFn){
        retVal = ethFn->setMode(ethCtrlIdx, aMode);
    }
    return retVal;
}

/**
    Get NIF controller mode

    @trace #BRCM_SWARCH_NIF_GETCTRLMODE_PROC
    @trace #BRCM_SWREQ_NIF_QUERY

    @code{.c}
    @endcode
*/
int32_t NIF_GetCtrlMode(NIF_CntrlIDType aCtrlIdx,
                        ETHER_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    if (NULL == aMode) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ethFn = NIF_GetEthControllerID(aCtrlIdx);
        if(NULL != ethFn){
            retVal = ethFn->getMode(ethCtrlIdx, aMode);
        }
    }
    return retVal;
}

/**
    Get MAC address

    @trace #BRCM_SWARCH_NIF_GETMACADDR_PROC
    @trace #BRCM_SWREQ_NIF_QUERY

    @code{.c}
    @endcode
*/
int32_t NIF_GetMacAddr(NIF_CntrlIDType aCtrlIdx,
                       uint8_t *const aMacAddr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL != ethFn){
        retVal = ethFn->getMacAddr(ethCtrlIdx, aMacAddr);
    }
    return retVal;
}

/**
    Get VLAN ID

    @trace #BRCM_SWARCH_NIF_GETVLANID_PROC
    @trace #BRCM_SWREQ_NIF_QUERY

    @code{.c}
    @endcode
*/
int32_t NIF_GetVlanID(NIF_CntrlIDType aCtrlIdx,
                      ETHER_VLANIDType *const aVlanID)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;

    if (NULL == aVlanID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    if (NIF_STATE_INIT != NIF_Comp.state) {
        retVal = BCM_ERR_UNINIT;
        goto done;
    }

    *aVlanID = ((ETHER_VLANIDType)(((aCtrlIdx) & NIF_CTRLVAL_VLAN_ID_MASK)
                                        >> NIF_CTRLVAL_VLAN_ID_SHIFT));

    if (0U != (*aVlanID)) {
        for (i = 0; i < NIF_CtrlCfg->vlanArraySize; i++) {
            if (NIF_CtrlCfg->vlanArray[i] == (*aVlanID)) {
                break;
            }
        }

        if (i == NIF_CtrlCfg->vlanArraySize) {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

done:
    return retVal;
}

/**
    Get empty transmit buffer

    @trace #BRCM_SWARCH_NIF_GETTXBUFFER_PROC
    @trace #BRCM_SWREQ_NIF_TRANSMIT

    @code{.c}
    @endcode
*/
int32_t NIF_GetTxBuffer(NIF_CntrlIDType aCtrlIdx,
                        ETHER_EtherType aEtherType,
                        ETHER_PrioType aPriority,
                        uint32_t *const aBufIdx,
                        uint8_t **const aBuf,
                        uint32_t *const aLenInOut)
{
    int32_t retVal = BCM_ERR_OK;
    ETHER_VLANIDType vlanID;
    ETHER_PrioType priority = (aPriority & 0x7U);
    uint32_t i = NIF_CtrlCfg->cbTblSize;
    NIF_ClientIDType clientID;
#if defined (ENABLE_FQTSS)
    uint32_t streamIdx = FQTSS_MAX_STREAMS;
    uint32_t fqtssBytes;
#endif
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL == ethFn){
        goto done;
    }


    retVal = NIF_GetVlanID(aCtrlIdx, &vlanID);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    NIF_GetClientID(aCtrlIdx, &clientID);

    /* Check that the Ethertype is allocated to the client */
    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        if ((clientID == NIF_CtrlCfg->cbTbl[i].clientID) &&
            (aEtherType == NIF_CtrlCfg->cbTbl[i].etherType)) {
            break;
        }
    }

    if (i == NIF_CtrlCfg->cbTblSize) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    if (aPriority > 7U) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

#if defined (ENABLE_FQTSS)
    /* Check for available credits - in case shaping is enabled */
    if (BCM_ERR_OK == NIF_FindStreamIdx(clientID, &streamIdx)) {
        if (BCM_ERR_OK != FQTSS_GetBytes(streamIdx, aLenInOut)) {
            if ((NIF_Comp.totalAVBReserved > 0UL) &&
                (*aLenInOut > ((100UL - NIF_Comp.totalAVBReserved) * 15UL))) {
                *aLenInOut = (100UL - NIF_Comp.totalAVBReserved) * 15UL;
            }
        }
    } else {
        /* Unshaped/unreserved stream */
        if ((NIF_Comp.totalAVBReserved > 0UL) &&
            (*aLenInOut > ((100UL - NIF_Comp.totalAVBReserved) * 15UL))) {
            *aLenInOut = (100UL - NIF_Comp.totalAVBReserved) * 15UL;
        }
    }
    fqtssBytes = *aLenInOut;
#endif

    if ((vlanID >= ETHER_VLANID_MIN)
            && (vlanID <= ETHER_VLANID_MAX)){
        /* Space for VLAN tag */
        *aLenInOut += NIF_VLAN_TAG_SIZE;
    }
    /* Space for Ether type */
    *aLenInOut += NIF_ETHERTYPE_SIZE;

    if (*aLenInOut > 0UL) {
        retVal = ethFn->getTxBuf(ethCtrlIdx, priority, aBufIdx,
                                 aBuf, aLenInOut);
        if (BCM_ERR_OK != retVal) {
#if defined (ENABLE_FQTSS)
            if (streamIdx < FQTSS_MAX_STREAMS) {
                (void)FQTSS_ConsumeBytes(streamIdx, fqtssBytes, 0UL);
            }
#endif
            goto done;
        }

        if ((vlanID >= ETHER_VLANID_MIN)
                && (vlanID <= ETHER_VLANID_MAX)){
            if (*aLenInOut > NIF_VLAN_TAG_SIZE) {
                *aLenInOut -= NIF_VLAN_TAG_SIZE;
                /* Insert VLAN tag */
                *((ETHER_EtherType *)(*aBuf)) = Host2BE16(ETHER_ETHERTYPE_VLANTAG);
                *aBuf += 2UL;
                *((ETHER_VLANIDType *)(*aBuf)) =
                    Host2BE16(0U | (priority << 13UL) | (vlanID & 0xFFFU));
                *aBuf += 2UL;
            } else {
                retVal = ethFn->sendTxBuf(ethCtrlIdx, FALSE, *aBufIdx, 0UL, NULL);
                if (BCM_ERR_OK != retVal) {
                    NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_GETTXBUFFER_PROC, retVal,
                                    *aBufIdx, __LINE__);
                }
                retVal = BCM_ERR_NOMEM;
#if defined (ENABLE_FQTSS)
                (void)FQTSS_ConsumeBytes(streamIdx, fqtssBytes, 0UL);
#endif
                goto done;
            }
        }

        if (*aLenInOut > NIF_ETHERTYPE_SIZE) {
            *aLenInOut -= NIF_ETHERTYPE_SIZE;
            *((ETHER_EtherType *)(*aBuf)) = Host2BE16(aEtherType);
            *aBuf += NIF_ETHERTYPE_SIZE;
        } else {
            retVal = ethFn->sendTxBuf(ethCtrlIdx, FALSE, *aBufIdx, 0UL, NULL);
            if (BCM_ERR_OK != retVal) {
                NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_GETTXBUFFER_PROC, retVal,
                        *aBufIdx, __LINE__);
            }
            retVal = BCM_ERR_NOMEM;
#if defined (ENABLE_FQTSS)
            (void)FQTSS_ConsumeBytes(streamIdx, fqtssBytes, 0UL);
#endif
            goto done;
        }

        /* Update the state of the buffer */
        if (NIF_BUF_COUNT <= (*aBufIdx)) {
            NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_GETTXBUFFER_PROC, BCM_ERR_NOMEM,
                    *aBufIdx, __LINE__);
        } else {
            if (NIF_BUFSTATE_FREE != NIF_Comp.txBufInfo[*aBufIdx].bufState) {
                NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_GETTXBUFFER_PROC,
                        BCM_ERR_INVAL_BUF_STATE,
                        *aBufIdx, NIF_Comp.txBufInfo[*aBufIdx].bufState);
            } else {
                NIF_Comp.txBufInfo[*aBufIdx].bufState = NIF_BUFSTATE_ALLOC;
                NIF_Comp.txBufInfo[*aBufIdx].etherType = aEtherType;
                NIF_Comp.txBufInfo[*aBufIdx].size = *aLenInOut;
                NIF_Comp.txBufInfo[*aBufIdx].ctrlIdx = aCtrlIdx;
                ATOMIC_ClrBit(&NIF_Comp.isWaitingForBuffer, i);
            }
        }
#if defined (ENABLE_FQTSS)
    } else {
        retVal = BCM_ERR_NOMEM;
#endif
    }
done:
    if ((BCM_ERR_OK != retVal) && (i != NIF_CtrlCfg->cbTblSize)) {
        ATOMIC_SetBit(&NIF_Comp.isWaitingForBuffer, i);
    }
    return retVal;
}

/**
    Send Ethernet packet

    @trace #BRCM_SWARCH_NIF_SEND_PROC
    @trace #BRCM_SWREQ_NIF_TRANSMIT

    @code{.c}
    @endcode
*/
int32_t NIF_Send(NIF_CntrlIDType aCtrlIdx,
                 ETHER_EtherType aEtherType,
                 uint32_t aIsTxConfEnable,
                 uint32_t aBufIdx,
                 uint32_t aLen,
                 const uint8_t *const aDestMacAddr)
{
    int32_t retVal = BCM_ERR_OK;
    ETHER_VLANIDType vlanID;
    uint32_t len = aLen;
#if defined (ENABLE_FQTSS)
    uint32_t streamIdx = FQTSS_MAX_STREAMS;
    uint32_t i;
    uint32_t clientID;

    NIF_GetClientID(aCtrlIdx, &clientID);
#endif
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if (NULL == ethFn) {
        goto done;
    }

    if (NIF_STATE_INIT != NIF_Comp.state) {
        retVal = BCM_ERR_UNINIT;
        goto done;
    }

    retVal = NIF_GetVlanID(aCtrlIdx, &vlanID);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    if (NIF_BUF_COUNT <= aBufIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal, aBufIdx, __LINE__);
        goto done;
    }

    if (NIF_BUFSTATE_ALLOC != NIF_Comp.txBufInfo[aBufIdx].bufState) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal, aBufIdx,
                        NIF_Comp.txBufInfo[aBufIdx].bufState);
        goto done;
    }

    if (aEtherType != NIF_Comp.txBufInfo[aBufIdx].etherType) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal, aBufIdx, __LINE__);
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal,
                        NIF_Comp.txBufInfo[aBufIdx].etherType, aEtherType);
        goto done;
    }

    if (aCtrlIdx != NIF_Comp.txBufInfo[aBufIdx].ctrlIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal, 0UL, __LINE__);
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal,
                       (uint32_t)(NIF_Comp.txBufInfo[aBufIdx].ctrlIdx >> 32UL),
                       (uint32_t)NIF_Comp.txBufInfo[aBufIdx].ctrlIdx);
        goto done;
    }

    if (len > NIF_Comp.txBufInfo[aBufIdx].size) {
        retVal = BCM_ERR_INVAL_PARAMS;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal, aBufIdx, __LINE__);
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_SEND_PROC, retVal,
                        NIF_Comp.txBufInfo[aBufIdx].size, len);
        goto done;
    }

    if (TRUE == aIsTxConfEnable) {
        NIF_Comp.txBufInfo[aBufIdx].bufState = NIF_BUFSTATE_TX;
    } else {
        NIF_Comp.txBufInfo[aBufIdx].bufState = NIF_BUFSTATE_FREE;
    }

    /* If len is zero free-up the packet */
    if (len != 0UL) {
#if defined (ENABLE_FQTSS)
        if (BCM_ERR_OK == NIF_FindStreamIdx(clientID, &streamIdx)) {
            FQTSS_ConsumeBytes(streamIdx, NIF_Comp.txBufInfo[aBufIdx].size, len);
        }
#endif

        if (vlanID >= ETHER_VLANID_MIN) {
            len += NIF_VLAN_TAG_SIZE + NIF_ETHERTYPE_SIZE; /* VLAN + EtherType */
        } else {
            len += NIF_ETHERTYPE_SIZE; /* EtherType */
        }
    } else {
        /* Free the NIF buf state */
        NIF_Comp.txBufInfo[aBufIdx].bufState = NIF_BUFSTATE_FREE;
#if defined (ENABLE_FQTSS)
        if (BCM_ERR_OK == NIF_FindStreamIdx(clientID, &streamIdx)) {
            /* Release the bytes back to shaper */
            (void)FQTSS_ConsumeBytes(streamIdx, NIF_Comp.txBufInfo[aBufIdx].size, 0UL);
        }
        for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
            if ((aEtherType == NIF_CtrlCfg->cbTbl[i].etherType) &&
                (clientID == NIF_CtrlCfg->cbTbl[i].clientID)) {
                ATOMIC_SetBit(&NIF_Comp.isWaitingForBuffer, i);
                break;
            }
        }
#endif
    }

    retVal = ethFn->sendTxBuf(ethCtrlIdx, aIsTxConfEnable,
                              aBufIdx, len, aDestMacAddr);
done:
    return retVal;
}

/**
    Enable Timestamping for transmit packet

    @trace #BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
int32_t NIF_EnableTxTimestamp(NIF_CntrlIDType aCtrlIdx,
                              uint32_t aBufIdx)
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
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC, retVal,
                        aBufIdx, __LINE__);
        goto done;
    }

    if (NIF_BUFSTATE_ALLOC != NIF_Comp.txBufInfo[aBufIdx].bufState) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC, retVal,
                        aBufIdx, NIF_Comp.txBufInfo[aBufIdx].bufState);
        goto done;
    }

#if defined(ENABLE_ETH_SWITCH)
    retVal = NIF_CtrlCfg->swtIntfTbl[0].enableTxTs(ethCtrlIdx, aBufIdx,
                               &(NIF_Comp.txBufInfo[aBufIdx].mgmtInfo));
#else
    retVal = ethFn->enableTs(ethCtrlIdx, aBufIdx);
#endif

    if (BCM_ERR_OK == retVal) {
        NIF_Comp.txBufInfo[aBufIdx].isTSed = TRUE;
    }
done:
    return retVal;
}

/**
    Retrieve current Ethernet time

    @trace #BRCM_SWARCH_NIF_GETTIME_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
int32_t NIF_GetTime(NIF_CntrlIDType aCtrlIdx,
                    ETHER_TimestampType* const aTS,
                    ETHER_TimestampQualType* const aTSQual)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL != ethFn){
        retVal = ethFn->getTime(ethCtrlIdx, aTS, aTSQual);
    }
    return retVal;
}

/**
    Set Ethernet time

    @trace #BRCM_SWARCH_NIF_SETTIME_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
int32_t NIF_SetTime(NIF_CntrlIDType aCtrlIdx,
                    const ETHER_TimestampType* const aTS)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL != ethFn){
        retVal = ethFn->setTime(ethCtrlIdx, aTS);
    }
    return retVal;
}

/**
    Adjust Ethernet time

    @trace #BRCM_SWARCH_NIF_SETCORRECTIONTIME_PROC
    @trace #BRCM_SWREQ_NIF_TIME

    @code{.c}
    @endcode
*/
int32_t NIF_SetCorrectionTime(NIF_CntrlIDType aCtrlIdx,
                              const ETHER_TimestampDiffType* const aTD,
                              const ETHER_RateRatioType* const aRR)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if(NULL != ethFn){
        retVal = ethFn->setCorrectionTime(ethCtrlIdx, aTD, aRR);
    }
    return retVal;
}

#if !defined(ENABLE_ETH_SWITCH)
/**
    Get Ethernet transceiver link state

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
#if defined(ENABLE_WLAN)
    } else if (NIF_CtrlCfg->wlXcvrIntfTblSize <= aIdx){
#else
    } else if (NIF_CtrlCfg->xcvrIntfTblSize <= aIdx){
#endif
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
#if defined(ENABLE_WLAN)
        retVal = NIF_CtrlCfg->wlXcvrIntfTbl[aIdx].getLinkState(aIdx, aLinkState);
#else
        retVal = NIF_CtrlCfg->xcvrIntfTbl[aIdx].getLinkState(aIdx, aLinkState);
#endif
    }

    return retVal;
}
#endif /* !defined(ENABLE_ETH_SWITCH) */

#if !defined(ENABLE_WLAN)
/**
    Get SQI Value

    @trace #BRCM_SWARCH_NIF_XCVRGETSQIVALUE_PROC
    @trace #BRCM_SWREQ_NIF_QUERY

    @code{.c}
    @endcode
*/
int32_t NIF_XcvrGetSQIValue(uint32_t aXcvrIdx,
                            uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_OK;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        retVal = BCM_ERR_UNINIT;
    } else if (NIF_CtrlCfg->xcvrIntfTblSize <= aXcvrIdx){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = NIF_CtrlCfg->xcvrIntfTbl[aXcvrIdx].getSQIValue(aXcvrIdx, aSQIValue);
    }

    return retVal;
}
#endif

/**
    Enables AVB shaping for a stream

    @trace #BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC
    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING

    @code{.c}
    @endcode
*/
int32_t NIF_EnableAVBShaping(NIF_CntrlIDType aCtrlIdx,
                             NIF_AVBClassType aClass,
                             uint32_t aReservation)
{
    int32_t             retVal = BCM_ERR_INVAL_PARAMS;
#if defined(ENABLE_FQTSS)
    uint32_t            streamIdx = FQTSS_MAX_STREAMS;
    uint32_t            period = aClass * 1000UL;
    uint32_t            clientID;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if (NULL == ethFn) {
        goto done;
    }

    if ((aClass != NIF_AVBCLASS_A) && (aClass != NIF_AVBCLASS_B)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    NIF_GetClientID(aCtrlIdx, &clientID);
    /* Extract stream information from aCtrlIdx */
    retVal = NIF_FindStreamIdx(clientID, &streamIdx);
    if (BCM_ERR_OK == retVal) {
        retVal = FQTSS_EnableStream(streamIdx, period, aReservation);
        if (BCM_ERR_OK == retVal) {
            NIF_Comp.totalAVBReserved += aReservation;
            if (NIF_Comp.totalAVBReserved > FQTSS_MAX_AVB_BANDWIDTH) {
                NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC,
                        BCM_ERR_NOSUPPORT, NIF_Comp.totalAVBReserved, __LINE__);
            }
            if (NIF_Comp.currentTimerPeriod > period) {
                retVal = ethFn->setGPTimer(ethCtrlIdx, period);
                NIF_Comp.currentTimerPeriod = period;
            }
        }
    }
done:
#else
    uint32_t streamIdx = 0UL;
    retVal = BCM_ERR_NOSUPPORT;
#endif
    if (BCM_ERR_OK != retVal) {
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC, retVal,
                        streamIdx, __LINE__);
    }
    return retVal;
}

/**
    Disables AVB shaping for a stream

    @trace #BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC
    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING

    @code{.c}
    @endcode
*/
int32_t NIF_DisableAVBShaping(NIF_CntrlIDType aCtrlIdx)
{
    int32_t             retVal  = BCM_ERR_INVAL_PARAMS;
#if defined(ENABLE_FQTSS)
    uint32_t            streamIdx = FQTSS_MAX_STREAMS;
    uint32_t            fastestTimerPeriod;
    uint32_t            reservation;
    uint32_t            clientID;
    const NIF_EthIntfType *ethFn = NULL;
    NIF_EthCntrlrIdxType ethCtrlIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    ethFn = NIF_GetEthControllerID(aCtrlIdx);
    if (NULL == ethFn) {
        goto done;
    }

    NIF_GetClientID(aCtrlIdx, &clientID);

    /* Extract stream information from aCtrlIdx */
    retVal = NIF_FindStreamIdx(clientID, &streamIdx);
    if (BCM_ERR_OK == retVal) {
        retVal = FQTSS_DisableStream(streamIdx,
                                     &reservation,
                                     &fastestTimerPeriod);
        if (BCM_ERR_OK == retVal) {
            NIF_Comp.totalAVBReserved -= reservation;
            if (NIF_Comp.currentTimerPeriod < fastestTimerPeriod) {
                retVal = ethFn->setGPTimer(ethCtrlIdx, fastestTimerPeriod);
                NIF_Comp.currentTimerPeriod = fastestTimerPeriod;
            }
        }
    }
done:
#else
    uint32_t streamIdx = 0UL;
    retVal = BCM_ERR_NOSUPPORT;
#endif
    if (BCM_ERR_OK != retVal) {
        NIF_ReportError(aCtrlIdx, BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC, retVal,
                       streamIdx, __LINE__);
    }
    return retVal;
}

/**
    Check if clients are waiting for buffer

    @trace #BRCM_SWARCH_NIF_ARECLIENTSWAITINGFORBUFFERS_PROC
    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING

    @code{.c}
    @endcode
*/
uint32_t NIF_AreClientsWaitingForBuffers(void)
{
    uint32_t isWaiting = (0UL == NIF_Comp.isWaitingForBuffer) ? 0UL:1UL;
    return isWaiting;
}

/**
    Process ethernet timer

    @trace #BRCM_SWARCH_NIF_PROCESSETHERNETTIMER_PROC
    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING

    @code{.c}
    @endcode
*/
void NIF_ProcessEthernetTimer(ETHER_HwIDType aID)
{
    uint32_t i;
    NIF_CntrlIDType ctrlIdx;

    /* Go through all clients waiting for buffer and send notification */
    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        if ((NIF_CtrlCfg->cbTbl[i].bufAvailCb != NULL) &&
            (0UL != (NIF_Comp.isWaitingForBuffer & (1UL << i)))) {
            /* Generate nif controller index */
            ctrlIdx = NIF_CTRL(0U, NIF_CtrlCfg->cbTbl[i].clientID,
                               NIF_CtrlCfg->cbTbl[i].cntrlIndex);
            NIF_CtrlCfg->cbTbl[i].bufAvailCb(ctrlIdx);
        }
    }
}

/**
    Ethernet RX packet indication callback

    @trace #BRCM_SWARCH_ETHER_RXIND_PROC
    @trace #BRCM_SWREQ_ETHER_INDICATIONS_NIF

    @code{.c}
    @endcode
*/
void ETHER_RxInd(ETHER_HwIDType aID, const uint8_t *const aBuf,
                 uint32_t aLen, const uint8_t *const aSrcMacAddr,
                 const uint8_t *const aDestMacAddr)
{
    uint32_t i;
    ETHER_EtherType etherType;
    NIF_CntrlIDType ctrlIdx;
    uint32_t len = aLen;
    const uint8_t *buf = aBuf;
    uint32_t priority = 0UL;
    ETHER_VLANIDType vlanID = 0U;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_RXIND_PROC, BCM_ERR_UNINIT,
                        NIF_Comp.state, __LINE__);
        goto done;
    }

    if (aID != NIF_CtrlCfg->hwIdx) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_RXIND_PROC, BCM_ERR_INVAL_PARAMS,
                        aID, __LINE__);
        goto done;
    }

    if ((aBuf != NULL) && (len >= ETHER_HDR_SIZE)) {
        /* Save Rx driver buffer pointer */
        NIF_Comp.rxBufInfo.drvBuf = buf;
        etherType = *((ETHER_EtherType *)(buf));
        etherType = BE2Host16(etherType);
        len -= 2UL;
        buf = &buf[2UL];
        /* Check if the packet is VLAN tagged */
        if(ETHER_ETHERTYPE_VLANTAG == etherType) {
            vlanID = *((ETHER_VLANIDType *)(buf));
            vlanID = BE2Host16(vlanID);
            priority = (uint32_t)(vlanID >> 13UL);
            vlanID &= 0xFFFU;
            /* Update the packet etherType skipping the VLAN tag */
            etherType = *((ETHER_EtherType *)(&buf[2UL]));
            etherType = BE2Host16(etherType);
            len -= 4UL;
            buf = &buf[4UL];
        }
        NIF_Comp.rxBufInfo.vlanID = vlanID;
        NIF_Comp.rxBufInfo.etherType = etherType;
        if (0U != vlanID) {
            for (i = 0UL; i < NIF_CtrlCfg->vlanArraySize; i++) {
                if (vlanID == NIF_CtrlCfg->vlanArray[i]) {
                    break;
                }
            }
            if (i == NIF_CtrlCfg->vlanArraySize) {
                /* Controller is not participating in this VLAN. Drop packet */
                goto done;
            }
        }

        for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
            if (etherType == NIF_CtrlCfg->cbTbl[i].etherType) {
                ctrlIdx = NIF_CTRL(vlanID,
                                   NIF_CtrlCfg->cbTbl[i].clientID,
                                   NIF_CtrlCfg->cbTbl[i].cntrlIndex);
                if (NIF_CtrlCfg->cbTbl[i].rxCb != NULL) {
                    /* Save Rx Client buffer pointer */
                    NIF_Comp.rxBufInfo.clientBuf = buf;
                    NIF_CtrlCfg->cbTbl[i].rxCb(ctrlIdx, buf, len,
                            aSrcMacAddr, aDestMacAddr, priority);
                }
#if !defined(ENABLE_ETH_SWITCH) && !defined(ENABLE_WLAN)
                {
                    int32_t retVal;
                    ETHER_TimestampType ts;
                    ETHER_TimestampQualType tsQual;
                    if ((ETHER_ETHERTYPE_GPTP == etherType)
                            && (NIF_CtrlCfg->cbTbl[i].rxTSCb != NULL)) {
                        retVal = NIF_CtrlCfg->ethIntfTbl[aID].getRxTS(aID, aBuf, &ts, &tsQual);
                        if (BCM_ERR_OK == retVal) {
                            NIF_CtrlCfg->cbTbl[i].rxTSCb(ctrlIdx, buf, NULL,
                                    &ts, &tsQual);
                        }
                    }
                }
#endif
                break;
            }
        }
    }

done:
    return ;
}

/**
    Ethernet TX packet send confirmation callback

    @trace #BRCM_SWARCH_ETHER_TXDONEIND_PROC
    @trace #BRCM_SWREQ_ETHER_INDICATIONS_NIF

    @code{.c}
    @endcode
*/
void ETHER_TxDoneInd(ETHER_HwIDType aID, uint32_t aBufIdx,
                     ETHER_ResultType aResult)
{
    uint32_t              i;
    NIF_TxBufInfoType     *txBufInfo = &NIF_Comp.txBufInfo[aBufIdx];
    ETHER_EtherType       etherType;
    NIF_ClientIDType      clientID;
    NIF_CntrlIDType       ctrlIdx;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_TXDONEIND_PROC, BCM_ERR_UNINIT,
                        NIF_Comp.state, __LINE__);
        goto done;
    }

    if (aID != NIF_CtrlCfg->hwIdx) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_TXDONEIND_PROC,
                        BCM_ERR_INVAL_PARAMS, aID, __LINE__);
        goto done;
    }

    if (NIF_BUFSTATE_TX != txBufInfo->bufState) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_TXDONEIND_PROC,
                        BCM_ERR_INVAL_BUF_STATE, txBufInfo->bufState,
                        __LINE__);
        goto done;
    }

    if (ETHER_RESULT_SUCCESS != aResult) {
        NIF_ReportError(0ULL, BRCM_SWARCH_ETHER_TXDONEIND_PROC, BCM_ERR_UNKNOWN,
                        txBufInfo->bufState, __LINE__);
        goto done;
    }

    NIF_GetClientID(txBufInfo->ctrlIdx, &clientID);
    etherType = txBufInfo->etherType;

    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        if ((etherType == NIF_CtrlCfg->cbTbl[i].etherType) &&
            (clientID == NIF_CtrlCfg->cbTbl[i].clientID)) {
            if (NIF_CtrlCfg->cbTbl[i].txCb != NULL) {
                NIF_CtrlCfg->cbTbl[i].txCb(txBufInfo->ctrlIdx, aBufIdx);
            }
#if !defined(ENABLE_ETH_SWITCH) && !defined(ENABLE_WLAN)
            {
                ETHER_TimestampType ts;
                ETHER_TimestampQualType tsQual;
                int32_t retVal;
                if (TRUE == txBufInfo->isTSed) {
                    txBufInfo->isTSed = FALSE;
                    if (NIF_CtrlCfg->cbTbl[i].txTSCb != NULL) {
                        /* Get Timestamp */
                        retVal = NIF_CtrlCfg->ethIntfTbl[aID].getTxTs(aID, aBufIdx, &ts, &tsQual);
                        if (BCM_ERR_OK == retVal) {
                            NIF_CtrlCfg->cbTbl[i].txTSCb(txBufInfo->ctrlIdx,
                                                      aBufIdx, NULL, &ts,
                                                      &tsQual);
                        }
                    }
                }
            }
#endif
        } else {
            if ((NIF_CtrlCfg->cbTbl[i].bufAvailCb != NULL) &&
                    (0UL != (NIF_Comp.isWaitingForBuffer  & (1UL << i)))) {
                /* Generate nif controller index */
                ctrlIdx = NIF_CTRL(0U, NIF_CtrlCfg->cbTbl[i].clientID,
                                   NIF_CtrlCfg->cbTbl[i].cntrlIndex);
                NIF_CtrlCfg->cbTbl[i].bufAvailCb(ctrlIdx);
            }
        }
    }
done:
    txBufInfo->bufState = NIF_BUFSTATE_FREE;
    return ;
}

/**
    Ethernet transceiver link state indication

    @trace #BRCM_SWARCH_ETHXCVR_LINKSTATECHANGEIND_PROC
    @trace #BRCM_SWREQ_ETHXCVR_STATUS_NIF

    @code{.c}
    @endcode
*/
void ETHXCVR_LinkStateChangeInd(ETHXCVR_IDType aID,
                                ETHXCVR_LinkStateType aLinkState)
{
    uint32_t i, j;
    NIF_CntrlIDType ctrlIdx;
    ETHER_VLANIDType vlanID;

    if (NIF_STATE_INIT != NIF_Comp.state) {
        goto done;
    }

    NIF_ReportError(aID, BRCM_SWARCH_ETHXCVR_LINKSTATECHANGEIND_PROC,
                    BCM_ERR_OK, __LINE__, aLinkState);

    for (i = 0UL; i < NIF_CtrlCfg->cbTblSize; i++) {
        vlanID = 0U;
        ctrlIdx = NIF_CTRL(vlanID, NIF_CtrlCfg->cbTbl[i].clientID,
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
