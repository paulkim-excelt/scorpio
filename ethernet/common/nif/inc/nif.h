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
    @defgroup grp_nif_ifc Interface
    @ingroup grp_nif

    @addtogroup grp_nif_ifc
    @{

    @file nif.h
    @brief NIF Interface File
    This header file contains the interface functions for NIF
    @version 0.1 Initial Version
*/

#ifndef NIF_H
#define NIF_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <ethernet.h>
#include <eth_xcvr.h>
#include <eth_switch.h>

/**
    @name NIF Architecture IDs
    @{
    @brief Architecture IDs for NIF
*/
#define BRCM_SWARCH_NIF_AVBCLASS_TYPE                    (0x00U) /**< @brief #NIF_AVBClassType */
#define BRCM_SWARCH_NIF_CLIENTIDTYPE_TYPE                (0x01U) /**< @brief #NIF_ClientIDType */
#define BRCM_SWARCH_NIF_CNTRLIDTYPE_TYPE                 (0x02U) /**< @brief #NIF_CntrlIDType */
#define BRCM_SWARCH_NIF_RXCLIENTCB_TYPE                  (0x03U) /**< @brief #NIF_RxClientCbType */
#define BRCM_SWARCH_NIF_TXDONECLIENTCB_TYPE              (0x04U) /**< @brief #NIF_TxDoneClientCbType */
#define BRCM_SWARCH_NIF_RXMGMTINFOCLIENTCB_TYPE          (0x05U) /**< @brief #NIF_RxMgmtInfoClientCbType */
#define BRCM_SWARCH_NIF_TXTSCLIENTCB_TYPE                (0x06U) /**< @brief #NIF_TxTSClientCbType */
#define BRCM_SWARCH_NIF_RXTSCLIENTCB_TYPE                (0x07U) /**< @brief #NIF_RxTSClientCbType */
#define BRCM_SWARCH_NIF_XCVRLINKSTATECHGCLIENTCB_TYPE    (0x08U) /**< @brief #NIF_XcvrLinkStateChgClientCbType */
#define BRCM_SWARCH_NIF_BUFFERAVAILABLECB_TYPE           (0x09U) /**< @brief #NIF_BufferAvailableCbType */
#define BRCM_SWARCH_NIF_CBTBLTYPE_TYPE                   (0x0AU) /**< @brief #NIF_CbTblType */
#define BRCM_SWARCH_NIF_CTRLCFGTYPE_TYPE                 (0x0BU) /**< @brief #NIF_CtrlCfgType */
#define BRCM_SWARCH_NIF_CFGTYPE_TYPE                     (0x0CU) /**< @brief #NIF_CfgType */
#define BRCM_SWARCH_NIF_INIT_PROC                        (0x0DU) /**< @brief #NIF_Init */
#define BRCM_SWARCH_NIF_GETCTRLIDX_PROC                  (0x0EU) /**< @brief #NIF_GetCtrlIdx */
#define BRCM_SWARCH_NIF_SETCTRLMODE_PROC                 (0x0FU) /**< @brief #NIF_SetCtrlMode */
#define BRCM_SWARCH_NIF_GETCTRLMODE_PROC                 (0x10U) /**< @brief #NIF_GetCtrlMode */
#define BRCM_SWARCH_NIF_GETMACADDR_PROC                  (0x11U) /**< @brief #NIF_GetMacAddr */
#define BRCM_SWARCH_NIF_GETVLANID_PROC                   (0x12U) /**< @brief #NIF_GetVlanID */
#define BRCM_SWARCH_NIF_GETTXBUFFER_PROC                 (0x14U) /**< @brief #NIF_GetTxBuffer */
#define BRCM_SWARCH_NIF_SEND_PROC                        (0x15U) /**< @brief #NIF_Send */
#define BRCM_SWARCH_NIF_ENABLETXTIMESTAMP_PROC           (0x16U) /**< @brief #NIF_EnableTxTimestamp */
#define BRCM_SWARCH_NIF_GETTIME_PROC                     (0x17U) /**< @brief #NIF_GetTime */
#define BRCM_SWARCH_NIF_SETTIME_PROC                     (0x18U) /**< @brief #NIF_SetTime */
#define BRCM_SWARCH_NIF_SETCORRECTIONTIME_PROC           (0x19U) /**< @brief #NIF_SetCorrectionTime */
#define BRCM_SWARCH_NIF_GETLINKSTATE_PROC                (0x1AU) /**< @brief #NIF_GetLinkState */
#define BRCM_SWARCH_NIF_XCVRGETSQIVALUE_PROC             (0x1BU) /**< @brief #NIF_XcvrGetSQIValue */
#define BRCM_SWARCH_NIF_ENABLEAVBSHAPING_PROC            (0x1CU) /**< @brief #NIF_EnableAVBShaping */
#define BRCM_SWARCH_NIF_DISABLEAVBSHAPING_PROC           (0x1DU) /**< @brief #NIF_DisableAVBShaping */
#define BRCM_SWARCH_NIF_PROCESSETHERNETTIMER_PROC        (0x1EU) /**< @brief #NIF_ProcessEthernetTimer */
#define BRCM_SWARCH_NIF_ARECLIENTSWAITINGFORBUFFERS_PROC (0x1FU) /**< @brief #NIF_AreClientsWaitingForBuffers */
#define BRCM_SWARCH_NIF_CLIENTIDVAL_TYPE                 (0x20U) /**< @brief #NIF_ClientIDValType */
#define BRCM_SWARCH_NIF_CLIENT_ID_MACRO                  (0x21U) /**< @brief #NIF_CLIENT_ID */
#define BRCM_SWARCH_NIF_CNTRLRBIT_TYPE                   (0x22U) /**< @brief #NIF_CntrlrBitType */
#define BRCM_SWARCH_NIF_CNTRLR_ID_MACRO                  (0x23U) /**< @brief #NIF_CNTRLR_ID */
#define BRCM_SWARCH_NIF_CNTRLRVAL_TYPE                   (0x24U) /**< @brief #NIF_CntrlrValType */
#define BRCM_SWARCH_NIF_ETHCNTRLR_TYPE                   (0x25U) /**< @brief #NIF_EthCntrlrType */
#define BRCM_SWARCH_NIF_ETHCNTRLRIDX_TYPE                (0x26U) /**< @brief #NIF_EthCntrlrIdxType */
#define BRCM_SWARCH_NIF_ETHGETTXBUFFER_TYPE              (0x27U) /**< @brief #NIF_EthGetTxBufferType */
#define BRCM_SWARCH_NIF_ETHSEND_TYPE                     (0x28U) /**< @brief #NIF_EthSendType */
#define BRCM_SWARCH_NIF_ETHGETTIME_TYPE                  (0x29U) /**< @brief #NIF_EthGetTimeType */
#define BRCM_SWARCH_NIF_ETHGETRXTIMESTAMP_TYPE           (0x2AU) /**< @brief #NIF_EthGetRxTimestampType */
#define BRCM_SWARCH_NIF_ETHGETTXTIMESTAMP_TYPE           (0x2BU) /**< @brief #NIF_EthGetTxTimestampType */
#define BRCM_SWARCH_NIF_ETHENABLETXTIMESTAMP_TYPE        (0x2CU) /**< @brief #NIF_EthEnableTxTimestampType */
#define BRCM_SWARCH_NIF_ETHGETMACADDR_TYPE               (0x2DU) /**< @brief #NIF_EthGetMacAddrType */
#define BRCM_SWARCH_NIF_ETHSETMODE_TYPE                  (0x2EU) /**< @brief #NIF_EthSetModeType */
#define BRCM_SWARCH_NIF_ETHGETMODE_TYPE                  (0x2FU) /**< @brief #NIF_EthGetModeType */
#define BRCM_SWARCH_NIF_ETHSETTIME_TYPE                  (0x30U) /**< @brief #NIF_EthSetTimeType */
#define BRCM_SWARCH_NIF_ETHSETCORRECTIONTIME_TYPE        (0x31U) /**< @brief #NIF_EthSetCorrectionTimeType */
#define BRCM_SWARCH_NIF_ETHSETGPTIMER_TYPE               (0x32U) /**< @brief #NIF_EthSetGPTimerType */
#define BRCM_SWARCH_NIF_ETHINTF_TYPE                     (0x33U) /**< @brief #NIF_EthIntfType */
#define BRCM_SWARCH_NIF_ETHXCVR_GETLINKSTATE_TYPE        (0x34U) /**< @brief #NIF_ETHXCVR_GetLinkStateType */
#define BRCM_SWARCH_NIF_ETHXCVR_GETSQIVALUE_TYPE         (0x35U) /**< @brief #NIF_ETHXCVR_GetSQIValueType */
#define BRCM_SWARCH_NIF_XCVRINTF_TYPE                    (0x36U) /**< @brief #NIF_XcvrIntfType */
#define BRCM_SWARCH_NIF_ETHERSWT_ENABLETXTIMESTAMP_TYPE  (0x37U) /**< @brief #NIF_ETHERSWT_EnableTxTimestampType */
#define BRCM_SWARCH_NIF_SWTINTF_TYPE                     (0x38U) /**< @brief #NIF_SwtIntfType */
#define BRCM_SWARCH_NIF_GET_CONTROLLER_INDEX_MACRO       (0x39U) /**< @brief #NIF_GET_CONTROLLER_INDEX */
#define BRCM_SWARCH_NIF_CONTROLLERVAL_TYPE               (0x3AU) /**< @brief #NIF_ControllerValType */

/** @} */

/**
    @name AVB Traffic Class Type
    @{
    @brief AVB Traffic Class type of NIF

    This is the listing of the AVB traffic class types that are
    supported by NIF.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint32_t NIF_AVBClassType;          /**< @brief AVB class */
#define NIF_AVBCLASS_A            (125UL)  /**< @brief AVB class type A */
#define NIF_AVBCLASS_B            (250UL)  /**< @brief AVB class type B */
/** @} */

/**
    @name NIF client ID composition macros
    @{

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint32_t NIF_ClientIDValType;                  /**< @brief Values for ClientID composition  */
#define NIF_CLIENTIDVAL_COMPID_SHIFT      (0UL)        /**< @brief Shift for COMPID */
#define NIF_CLIENTIDVAL_COMPID_MASK       (0xFFFFUL)   /**< @brief Mask for COMPID */
#define NIF_CLIENTIDVAL_INSTID_SHIFT      (16UL)       /**< @brief Shift for INSTID */
#define NIF_CLIENTIDVAL_INSTID_MASK       (0xFF0000UL) /**< @brief Mask for INSTID */
/** @} */

/**
    @brief NIF client ID

    macro used to compose the client ID value.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_CLIENT_ID(aCompID, aInstID) ((((uint32_t)(aCompID) <<                          \
                            NIF_CLIENTIDVAL_COMPID_SHIFT) & NIF_CLIENTIDVAL_COMPID_MASK) | \
                            (((uint32_t)(aInstID) <<                                       \
                            NIF_CLIENTIDVAL_INSTID_SHIFT) & NIF_CLIENTIDVAL_INSTID_MASK))

/**
    @name NIF controller ID composition macros
    @{

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint32_t NIF_ControllerValType;                   /**< @brief Values for Controller index composition  */
#define NIF_CONTROLLERVAL_CNTRLRID_MASK           (0xFUL) /**< @brief Eth controller ID mask */
#define NIF_CONTROLLERVAL_CNTRLRID_SHIFT          (0UL)   /**< @brief Eth controller ID shift */
/** @} */

/**
    @brief NIF get controller ID

    macro used to fetch the controller ID value.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_GET_CONTROLLER_INDEX(aCtrlIdx) ((uint32_t)(((aCtrlIdx) & NIF_CONTROLLERVAL_CNTRLRID_MASK) \
                                                >> NIF_CONTROLLERVAL_CNTRLRID_SHIFT))

/**
    @name NIF controller ID type
    @{

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint32_t NIF_CntrlrBitType;                 /**< @brief Values for controller ID composition  */
#define NIF_CNTRLRBIT_TYPE_SHIFT          (4UL)     /**< @brief Shift for controller type */
#define NIF_CNTRLRBIT_TYPE_MASK           (0xF0UL)  /**< @brief Mask for controller type */
#define NIF_CNTRLRBIT_ID_SHIFT            (0UL)     /**< @brief Shift for controller ID */
#define NIF_CNTRLRBIT_ID_MASK             (0xFUL)   /**< @brief Mask for controller ID */
/** @} */

/**
    @brief NIF controller ID

    macro used to compose the controller ID value.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
#define NIF_CNTRLR_ID(aCtrlType, aCtrlIdx) ((((uint32_t)(aCtrlType) <<                         \
                                        NIF_CNTRLRBIT_TYPE_SHIFT) & NIF_CNTRLRBIT_TYPE_MASK) | \
                                        (((uint32_t)(aCtrlIdx) <<                              \
                                        NIF_CNTRLRBIT_ID_SHIFT) & NIF_CNTRLRBIT_ID_MASK))

/**
    @name NIF client identifier
    @brief NIF client identifier

    NIF client identifier registered with the NIF.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint32_t NIF_ClientIDType; /**< @brief Client ID type value */

/**
    @name NIF controller index value
    @{
    Index of the ethernet controller (type,index) to be used for the NIF client.

    @trace #BRCM_SWREQ_NIF_CONFIG
*/
typedef uint8_t NIF_CntrlrValType;      /**< @brief Ethernet controller index value */
#define NIF_CNTRLRVAL_INVALID (0xFFU)   /**< @brief Invalid value */
/** @} */

/**
    @name NIF controller identifier
    @{
    @brief NIF controller identifier

    NIF client identifier that is used by NIF to identify the ethernet
    controller index using its instance ID, VLAN and hardware
    controller index information.

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef uint64_t NIF_CntrlIDType; /**< @brief Control ID type value */
#define NIF_CNTRLIDTYPE_INVALID (0xFFFFFFFFFFFFFFFFULL) /**< @brief Invalid value */
/** @} */

/** @brief RX client callback type

    Each client shall register callback function for each EtherType.
    This callback shall be invoked when an Ethernet packet is received
    with corressponding EtherType.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx        Controller index in the context of NIF
    @param[in]   aBuf            Pointer to the packet buffer without L2
                                 packet encapsulation
    @param[in]   aLen            Length of the packet received
    @param[in]   aSrcMacAddr     Pointer to source MAC address
    @param[in]   aDestMacAddr    Pointer to destination MAC address
    @param[in]   aPriority       PCP priority of the packet

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_RxClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                   const uint8_t *const aBuf,
                                   uint16_t aLen,
                                   const uint8_t *const aSrcMacAddr,
                                   const uint8_t *const aDestMacAddr,
                                   uint32_t aPriority);

/** @brief TX done indication callback type

    Each client shall register this Transmit done indication callback.
    This callback shall be invoked on transmission completion of
    corresponding aBufIdx.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx    Controller index in the context of NIF
    @param[in]   aBufIdx     Packet buffer index

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_TxDoneClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                       uint32_t aBufIdx);

/** @brief RX Management information callback

    Each client shall register this management information callback.
    This callback function shall be invoked with management
    information of each RX packet with management information.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx    Controller index in the context of NIF
    @param[in]   aBuf        Pointer to the packet buffer
    @param[in]   aMgmtInfo   Pointer to management information

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_RxMgmtInfoClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                           const uint8_t *const aBuf,
                                           const ETHERSWT_MgmtInfoType *const aMgmtInfo);

/** @brief TX timestamp callback

    Each client shall register this timestamp callback. This callback
    shall be invoked for each packet transmitted with timestamp enabled.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx        Controller index
    @param[in]   aBuf            Pointer to the packet buffer
    @param[in]   aMgmtInfo       Pointer to management information
    @param[in]   aTS             Pointer to timestamp
    @param[in]   aTSQual         Pointer to quality of captured timestamp

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_TxTSClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                     uint32_t aBufIdx,
                                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                                     const ETHER_TimestampType *const aTS,
                                     const ETHER_TimestampQualType* const aTSQual);

/** @brief RX timestamp callback

    Each client shall register this timestamp callback. This callback
    shall be invoked each packet received and timestamped by hardware.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx        Controller index
    @param[in]   aBuf            Pointer to the packet buffer
    @param[in]   aMgmtInfo       Pointer to management information
    @param[in]   aTS             Pointer to timestamp
    @param[in]   aTSQual         Pointer to quality of captured timestamp

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_RxTSClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                     const uint8_t *const aBuf,
                                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                                     const ETHER_TimestampType *const aTS,
                                     const ETHER_TimestampQualType* const aTSQual);

/** @brief NIF Xcvr link change client callback

    Each client shall register this Xcvr link change callback. This callback
    shall be invoked for Xcvr link state change.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx        Controller index
    @param[in]   aLinkState      Current link state

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_XcvrLinkStateChgClientCbType)(NIF_CntrlIDType aCtrlIdx,
                                                 ETHXCVR_LinkStateType aLinkState);

/** @brief NIF buffer available client callback

    Each client which registers for this callback will be informed of
    buffer availability in case it was waiting for a buffer.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]   aCtrlIdx        Controller index

    @return void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef void (*NIF_BufferAvailableCbType) (NIF_CntrlIDType aCtrlIdx);

/**
    @name NIF controller type
    @{

    @trace #BRCM_SWREQ_NIF_INIT
*/
typedef uint32_t NIF_EthCntrlrType;          /**< @brief Controller type */
#define NIF_ETHCNTRLR_WIRED           (0UL)  /**< @brief Wired ethernet */
#define NIF_ETHCNTRLR_WIRELESS        (1UL)  /**< @brief Wireless ethernet */
/** @} */

/**
    @brief NIF controller index

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef uint32_t NIF_EthCntrlrIdxType;

/**
    @brief NIF client callback table configuration

    This is the callback configuration table for all the NIF client
    that are to be registered with NIF.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_CbTblType {
    NIF_ClientIDType             clientID;           /**< @brief Client ID */
    ETHER_EtherType              etherType;          /**< @brief Ether type */
    NIF_CntrlrValType            cntrlIndex;         /**< @brief Controller type and index */
    NIF_RxClientCbType           rxCb;               /**< @brief Rx callback function */
    NIF_RxTSClientCbType         rxTSCb;             /**< @brief Rx timestamp callback function */
    NIF_RxMgmtInfoClientCbType   rxMgmtInfoCb;       /**< @brief Rx management information callback function */
    NIF_TxDoneClientCbType       txCb;               /**< @brief Tx callback function */
    NIF_TxTSClientCbType         txTSCb;             /**< @brief Tx timestamp callback function */
    NIF_XcvrLinkStateChgClientCbType linkStateChgCb; /**< @brief Link state change callback function */
    NIF_BufferAvailableCbType    bufAvailCb;         /**< @brief Buffer available callback function */
} NIF_CbTblType;

/** @brief Get transmit buffer

    This function is used by NIF to get a free TX buffer.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aPriority   Priority of TX packet
    @param[out]     aBufIdx     Buffer index
    @param[out]     aBuf        Pointer to TX buffer
    @param[inout]   aLenInout   Length of the requested buffer (in),
                                Length of the allocated buffer (out)

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetTxBufferType)(ETHER_HwIDType aID, ETHER_PrioType aPriority,
                                          uint32_t *const aBufIdx, uint8_t **const aBuf,
                                          uint32_t *const aLenInOut);

/** @brief Send transmit buffer

    This function is used by NIF to send TX buffer over ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTxConfirmation     TX confirmation flag
                                        - 1UL: Ethernet driver gives the callback
                                        notification with the registered callback
                                        when the packet is sent. (The packet
                                        buffer is not released until the
                                        callback returns)
                                        - 0UL: No callback provided by driver
                                        when packet is sent
    @param[in]      aBufIdx             Buffer index (received by calling
                                    #ETHER_GetTxBuffer)
    @param[in]      aLen                Length of the data in bytes. If this API
                                        is called with aLen as zero, the packet
                                        is not transmitted, instead the buffer
                                        (as given by aBufIdx) is returned back
                                        to the Ethernet driver.
    @param[in]      aDestMacAddr        Pointer to destination MAC address

    @retval   As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthSendType)(ETHER_HwIDType aID, uint32_t aTxConfirmation,
                                   uint32_t aBufIdx, uint32_t aLen,
                                   const uint8_t *const aDestMacAddr);

/** @brief Get network time

    This function is used by NIF to get the network time managed by
    ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[out]     aTS                 Current Ethernet time
    @param[out]     aTSQual             Quality of the retrieved time

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetTimeType)(ETHER_HwIDType aID,
                                      ETHER_TimestampType* const aTS,
                                      ETHER_TimestampQualType* const aTSQual);

/** @brief Get Rx timestamp

    This function is used by NIF to get the Rx timestamp for a buffer
    recieved on ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPktBuf             Pointer to the received packet buffer
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was received. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetRxTimestampType)(ETHER_HwIDType aID, const uint8_t* const aPktBuf,
                                             ETHER_TimestampType* const aTS,
                                             ETHER_TimestampQualType* const aTSQual);

/** @brief Get Tx timestamp

    This function is used by NIF to get the Tx timestamp for a buffer
    transmitted over ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index (received in the transmit
                                        confirmation)
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was transmitted. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetTxTimestampType)(ETHER_HwIDType aID, uint32_t aBufIdx,
                                             ETHER_TimestampType* const aTS,
                                             ETHER_TimestampQualType* const aTSQual);

/** @brief Enable Tx timestamp

    This function is used by NIF to enable Tx timestamping.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthEnableTxTimestampType)(ETHER_HwIDType aID, uint32_t aBufIdx);

/** @brief Get MAC address

    This function is used by NIF to get the ethernet MAC address.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[out]     aMacAddr    Pointer to MAC address

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetMacAddrType)(ETHER_HwIDType aID, uint8_t *const aMacAddr);

/** @brief Ethernet set mode

    This function is used by NIF to set the ethernet mode.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aMode       Mode to be set

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthSetModeType)(ETHER_HwIDType aID, ETHER_ModeType aMode);

/** @brief Ethernet get mode

    This function is used by NIF to get the ethernet mode.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[out]     aMode       Pointer to retrieve mode

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthGetModeType)(ETHER_HwIDType aID, ETHER_ModeType *const aMode);

/** @brief Ethernet set time

    This function is used by NIF to set the time to ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTS                 New Ethernet time to be set

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthSetTimeType)(ETHER_HwIDType aID, const ETHER_TimestampType* const aTS);

/** @brief Ethernet set correction time

    This function is used by NIF to set the correction time to ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTD                 Time difference between time-master
                                        time and local Ethernet time
    @param[in]      aRR                 Rate ratio between the time master and
                                        local clock

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthSetCorrectionTimeType)(ETHER_HwIDType aID,
                                                const ETHER_TimestampDiffType* const aTD,
                                                const ETHER_RateRatioType*     const aRR);

/** @brief Ethernet set GPTimer

    This function is used by NIF to set the GPTimer to ethernet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPeriod             Timer period in nanoseconds

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_EthSetGPTimerType)(ETHER_HwIDType aID, uint32_t aPeriod);

/**
    @brief NIF ethernet controller function table configuration

    This is the callback configuration table for the ethernet controllers
    that are to be registered with NIF.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_EthIntfType {
    NIF_EthGetTxBufferType getTxBuf;                /**< @brief Get Tx buffer */
    NIF_EthSendType sendTxBuf;                      /**< @brief Send Tx buffer */
    NIF_EthGetTimeType getTime;                     /**< @brief Get time */
    NIF_EthGetRxTimestampType getRxTS;              /**< @brief Get Rx Timestamp */
    NIF_EthGetTxTimestampType getTxTs;              /**< @brief Get Tx timestamp */
    NIF_EthEnableTxTimestampType enableTs;          /**< @brief Enable timestamp */
    NIF_EthGetMacAddrType getMacAddr;               /**< @brief Get MAC address */
    NIF_EthSetModeType setMode;                     /**< @brief Set mode */
    NIF_EthGetModeType getMode;                     /**< @brief Get mode */
    NIF_EthSetTimeType setTime;                     /**< @brief Set time */
    NIF_EthSetCorrectionTimeType setCorrectionTime; /**< @brief Set correction time */
    NIF_EthSetGPTimerType setGPTimer;               /**< @brief Set GP timer */
} NIF_EthIntfType;

/** @brief XCVR get link state

    This function is used by NIF to get the link state from ethernet XCVR.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aLinkState  Pointer to link state

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_ETHXCVR_GetLinkStateType)(ETHXCVR_IDType aID,
                                                ETHXCVR_LinkStateType *const aLinkState);

/** @brief XCVR get SQI value

    This function is used by NIF to get the SQI value from ethernet XCVR.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aSQIValue   SQI value of the link

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_ETHXCVR_GetSQIValueType)(ETHXCVR_IDType aID,
                                               uint32_t *const aSQIValue);

/**
    @brief NIF ethernet XCVR controller function table configuration

    This is the callback configuration table for the transciever controllers
    that are to be registered with NIF.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_XcvrIntfType {
    NIF_ETHXCVR_GetLinkStateType getLinkState;  /**< @brief Get link state */
    NIF_ETHXCVR_GetSQIValueType getSQIValue;    /**< @brief Get SQI value */
} NIF_XcvrIntfType;

/** @brief Enable Tx Timestamp for switch driver

    This function is used by NIF to enable the Tx timestamp for the
    switch driver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aSwtID      Switch index
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aMgmtInfo   Pointer to management information
                                - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame

    @retval     As returned by the hooked function

    @post None

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations None
*/
typedef int32_t (*NIF_ETHERSWT_EnableTxTimestampType)(ETHER_HwIDType aSwtID,
                                                      uint32_t aBufIdx,
                                                      ETHERSWT_MgmtInfoType * const aMgmtInfo);

/**
    @brief NIF switch driver function table configuration

    This is the callback configuration table for the switch driver
    functions that are to be registered with NIF.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_SwtIntfType {
    NIF_ETHERSWT_EnableTxTimestampType enableTxTs; /**< @brief Enable Tx timestamp */
} NIF_SwtIntfType;

/**
    @brief NIF controller configuration table

    NIF controller configuration table.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_CtrlCfgType {
    ETHER_HwIDType hwIdx;                       /**< @brief Physical controller index */
    const ETHER_VLANIDType *const vlanArray;    /**< VLAN ID array */
    const uint32_t vlanArraySize;               /**< @brief Size of VLAN ID array */
    const NIF_CbTblType *const cbTbl;           /**< @brief Client callback table */
    const uint32_t cbTblSize;                   /**< @brief Client callback table size */
    const NIF_ClientIDType *const streamTbl;    /**< @brief Client streams table */
    const uint32_t streamTblSize;               /**< @brief Client streams table size */
    const NIF_EthIntfType *const ethIntfTbl;    /**< @brief Ethernet interface table */
    const uint32_t ethIntfTblSize;              /**< @brief Ethernet interface table size */
    const NIF_EthIntfType *const wlEthIntfTbl;  /**< @brief WL Ethernet interface table */
    const uint32_t wlEthIntfTblSize;            /**< @brief WL Ethernet interface table size */
    const NIF_XcvrIntfType *const xcvrIntfTbl;  /**< @brief XCVR interface table table */
    const uint32_t xcvrIntfTblSize;             /**< @brief XCVR interface table size */
    const NIF_XcvrIntfType *const wlXcvrIntfTbl;/**< @brief WL XCVR interface table */
    const uint32_t wlXcvrIntfTblSize;           /**< @brief WL XCVR interface table size */
    const NIF_SwtIntfType *const swtIntfTbl;    /**< @brief Switch interface table */
    const uint32_t swtIntfTblSize;              /**< @brief Switch interface table size */
} NIF_CtrlCfgType;

/**
    @brief NIF configuration

    NIF configuration.

    @trace #BRCM_SWREQ_NIF_INIT
 */
typedef struct _NIF_CfgType {
    const NIF_CtrlCfgType *const ctrlCfg;       /**< @brief Controller config */
} NIF_CfgType;

/** @brief NIF Initializiation

    This API initializes Ethernet system layer which is interfaced by NIF.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aConfig                 Pointer to NIF configuration

    @retval          #BCM_ERR_INVAL_PARAMS    aConfig is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid configuration
    @retval          #BCM_ERR_INVAL_STATE     NIF is already initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT

    @limitations Ethernet driver, switch driver and transceiver drivers shall
    be initialised by system outside the scope of Ethernet system
    layer
*/
extern int32_t NIF_Init(const NIF_CfgType *const aConfig);

/** @brief Get NIF controller index

    This API retrieves controller index in the context of NIF.
    Controller index is based on the physical controller and VLANs in
    which that physical controller is participating.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aVlanID                 VLAN ID in which physical controller
                                             is participating
    @param[in]       aCompID                 Component identifier
    @param[in]       aInstID                 Instance identifier
    @param[out]      aCtrlIdx                Pointer to retrieve controller index

    @retval          #BCM_ERR_OK              Controller indexes retrieved
                                              successfully
    @retval          #BCM_ERR_INVAL_PARAMS    aCtrlIdx is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aCtrlIdxNum is NULL
    @retval          #BCM_ERR_UNINIT          EthSys is not initialised
    @retval          #BCM_ERR_NOT_FOUND       No controller index found for the

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TRANSMIT
*/
extern int32_t NIF_GetCtrlIdx(ETHER_VLANIDType aVlanID,
                              uint16_t aCompID,
                              uint8_t aInstID,
                              NIF_CntrlIDType *const aCtrlIdx);

/** @brief Set NIF controller mode

    This API sets the NIF controller mode.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF.
    @param[in]       aMode                   Mode to be set

    @retval          #BCM_ERR_OK              Mode set successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller mode
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_INIT
*/
extern int32_t NIF_SetCtrlMode(NIF_CntrlIDType aCtrlIdx,
                               ETHER_ModeType aMode);

/** @brief Get NIF controller mode

    This API retrieves the NIF controller mode configured.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF
    @param[out]      aMode                   Pointer to retrieve mode

    @retval          #BCM_ERR_OK              Mode retrieved successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aMode is NULL
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_QUERY
*/
extern int32_t NIF_GetCtrlMode(NIF_CntrlIDType aCtrlIdx,
                               ETHER_ModeType *const aMode);

/** @brief Get MAC address

    This API retrieves MAC address of the controller.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF
    @param[out]      aMacAddr                Pointer to retrieve MAC address

    @retval          #BCM_ERR_OK              MAC address is retrieved successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aMacAddr is NULL
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_QUERY
*/
extern int32_t NIF_GetMacAddr(NIF_CntrlIDType aCtrlIdx,
                              uint8_t *const aMacAddr);

/** @brief Get VLAN ID

    This API retrieves VLAN ID of the controller.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF
    @param[out]      aVlanID                 Pointer to retrieve VLAN ID

    @retval          #BCM_ERR_OK              VLAN ID retrieved successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aVlanID is NULL
    @retval          #BCM_ERR_UNINIT          NIF is not initialised
    @retval          #BCM_ERR_NOT_FOUND       No VLAN ID configured for this
                                             controller index
    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_QUERY
*/
extern int32_t NIF_GetVlanID(NIF_CntrlIDType aCtrlIdx,
                             ETHER_VLANIDType *const aVlanID);

/** @brief Get empty transmit buffer

    This API provides an empty transmit buffer.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF
    @param[in]       aEtherType              Ether type
    @param[in]       aPriority               Priority value which shall be used
                                             for the 3-bit PCP field of the VLAN
                                             tag
    @param[out]      aBufIdx                 Pointer to retrieve buffer index
    @param[out]      aBuf                    Pointer to retrieve buffer pointer
    @param[inout]    aLenInout               Length of the requested buffer (in),
                                             Length of the allocated buffer (out)

    @retval          #BCM_ERR_OK              TX buffer successfully obtained
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Ether type
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid priority
    @retval          #BCM_ERR_INVAL_PARAMS    aBufIdx is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aBuf is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aLenInOut is NULL
    @retval          #BCM_ERR_NOMEM           No TX buffers are available
    @retval          #BCM_ERR_INVAL_STATE     Invalid controller state
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TRANSMIT
*/
extern int32_t NIF_GetTxBuffer(NIF_CntrlIDType aCtrlIdx,
                               ETHER_EtherType aEtherType,
                               ETHER_PrioType aPriority,
                               uint32_t *const aBufIdx,
                               uint8_t **const aBuf,
                               uint32_t *const aLenInOut);

/** @brief Send Ethernet packet

    This API enqueues an Ethernet packet to be sent.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Ethernet controller index in the
                                             context of NIF
    @param[in]       aEtherType              Ether type
    @param[in]       aIsTxConfEnable         TX confirmation enable flag
    @param[in]       aBufIdx                 Buffer index (received by calling
                                             ETHER_GetTxBuffer())
    @param[in]       aLen                    Length of the data to be sent in bytes
    @param[in]       aDestMacAddr            Pointer to destination MAC address

    @retval          #BCM_ERR_OK              TX buffer successfully obtained
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Ether type
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid priority
    @retval          #BCM_ERR_INVAL_PARAMS    aBufIdx is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aBuf is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aLenInOut is NULL
    @retval          #BCM_ERR_NOMEM           No TX buffers are available
    @retval          #BCM_ERR_INVAL_STATE     Invalid controller state
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TRANSMIT
*/
extern int32_t NIF_Send(NIF_CntrlIDType aCtrlIdx,
                        ETHER_EtherType aEtherType,
                        uint32_t aIsTxConfEnable,
                        uint32_t aBufIdx,
                        uint32_t aLen,
                        const uint8_t *const aDestMacAddr);

/** @brief Enable Timestamping for transmit packet

    This API enables timestamping for a particular transmit buffer.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index in the context
                                             of NIF
    @param[in]       aBufIdx                 Buffer index

    @retval          #BCM_ERR_OK              Successfully enabled timestamp for
                                             the given buffer index
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid channel
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid buffer index
    @retval          #BCM_ERR_INVAL_STATE     Invalid controller state
    @retval          #BCM_ERR_INVAL_BUF_STATE Invalid packet buffer state
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TIME
*/
extern int32_t NIF_EnableTxTimestamp(NIF_CntrlIDType aCtrlIdx,
                                     uint32_t aBufIdx);

/** @brief Retrieve current Ethernet time

    This API retrieves the Ethernet current time.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index
    @param[out]      aTS                     Current Ethernet time
    @param[out]      aTSQual                 Quality of the retrieved time

    @retval          #BCM_ERR_OK              Successfully retrieved current
                                             Ethernet time
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aTS is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aTSQual is NULL
    @retval          #BCM_ERR_INVALID_STATE   Controller not initialized
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TIME
*/
extern int32_t NIF_GetTime(NIF_CntrlIDType aCtrlIdx,
                           ETHER_TimestampType* const aTS,
                           ETHER_TimestampQualType* const aTSQual);

/** @brief Set Ethernet time

    This API sets the Ethernet time.
    This API shall be invoked only on time masters.
    For time slaves the ETHER_SetCorrectionTime API shall be used.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index
    @param[in]       aTS                     New Ethernet time to be set

    @retval          #BCM_ERR_OK              Ethernet time successfully set
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aTS is NULL
    @retval          #BCM_ERR_INVALID_STATE   Controller not initialized
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TIME
*/
extern int32_t NIF_SetTime(NIF_CntrlIDType aCtrlIdx,
                           const ETHER_TimestampType* const aTS);

/** @brief Adjust Ethernet time

    This API adjusts local Ethernet clock on the basis of timing
    messages received from the time-master.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                Controller index
    @param[in]       aTD                     Time difference between time-master
                                             time and local Ethernet time
    @param[in]       aRR                     Rate ratio between the time master

    @retval          #BCM_ERR_OK              Ethernet time corrected successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid controller index
    @retval          #BCM_ERR_INVAL_PARAMS    aTD is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aRR is NULL
    @retval          #BCM_ERR_INVALID_STATE   Controller not initialized
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_TIME
*/
extern int32_t NIF_SetCorrectionTime(NIF_CntrlIDType aCtrlIdx,
                                     const ETHER_TimestampDiffType* const aTD,
                                     const ETHER_RateRatioType* const aRR);

/** @brief Get Ethernet transceiver/port link state

    This API retrieves the transceiver/port link state.

    @behavior Sync, Reentrant

    @param[in]       aID                     Index of the transceiver/port
    @param[out]      aLinkState              Pointer to retrieve link state

    @return          #BCM_ERR_OK              Successfully retrieved transceiver/
                                              port link state
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver/port index
    @return          #BCM_ERR_INVAL_PARAMS    aLinkState is NULL
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_QUERY
*/
extern int32_t NIF_GetLinkState(uint32_t aIdx,
                            ETHXCVR_LinkStateType *const aLinkState);

/** @brief Get SQI Value

    This API returns the SQI value of the established link.

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aXcvrIdx                Index of the transceiver
    @param[out]      aSQIValue               SQI value of the link

    @retval          #BCM_ERR_OK              Successfully retrieved the
                                             SQI value of the link.
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid transceiver index
    @retval          #BCM_ERR_INVAL_PARAMS    aSQIValue is NULL
    @retval          #BCM_ERR_UNINIT          Transceiver not initialized.
    @retval          #BCM_ERR_INVALID_STATE   Controller not initialized
    @retval          #BCM_ERR_UNINIT          NIF is not initialised

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_QUERY
*/
extern int32_t NIF_XcvrGetSQIValue(uint32_t aXcvrIdx,
                                     uint32_t *const aSQIValue);

/** @brief Enables AVB shaping for a stream

    This API verifies that the client has a stream allocated in the shaper and
    then enables shaping on it. If required, it will update the Ethernet GPTimer
    according to the minimum shaping class interval

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx            NIF controller index
    @param[in]       aClass              AVB shaping class
    @param[in]       aReservation        Percentage of link bandwidth reserved
                                         for the stream

    @retval         #BCM_ERR_OK              Successful
    @retval         #BCM_ERR_INVAL_PARAMS    Invalid aCtrlIdx
    @retval         #BCM_ERR_INVAL_PARAMS    Stream already enabled
    @retval         #BCM_ERR_INVAL_PARAMS    aReservation exceeds bounds
    @retval         #BCM_ERR_UNINIT          NIF is not initialized

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING
*/
int32_t NIF_EnableAVBShaping(NIF_CntrlIDType aCtrlIdx,
                             NIF_AVBClassType aClass,
                             uint32_t aReservation);

/** @brief Disables AVB shaping for a stream

    This API verifies that the client has a stream allocated in the shaper and
    then disables shaping on it. If required, it will update the Ethernet GPTimer
    according to the minimum shaping class interval

    @behavior Sync, Reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx                NIF controller index
                                         for the stream

    @retval         #BCM_ERR_OK              Successful
    @retval         #BCM_ERR_INVAL_PARAMS    Invalid aCtrlIdx
    @retval         #BCM_ERR_INVAL_PARAMS    Stream already disabled
    @retval         #BCM_ERR_UNINIT          NIF is not initialized

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING
*/
int32_t NIF_DisableAVBShaping(NIF_CntrlIDType aCtrlIdx);

/** @brief Process ethernet timer

    This API is invoked periodically at the class interval expiry to wake
    up any stream clients for which credits have been replenished and which
    are waiting for a buffer to become free

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aID                Index of H/W controller

    @retval          void

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING
*/
void NIF_ProcessEthernetTimer(ETHER_HwIDType aID);

/** @brief Check if clients are waiting for buffer

    This API is invoked by the interrupt handler on class interval expiry to
    deduce if any stream clients are waiting for buffers

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param void

    @retval          #TRUE                Clients are waiting for buffers
    @retval          #FALSE               No clients waiting for buffers

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_STREAM_SHAPING
*/
uint32_t NIF_AreClientsWaitingForBuffers(void);

#endif /* NIF_H */
/** @} */
