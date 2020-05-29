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
 * @file eth_switch_osil.h
 *
 * @brief Ethernet switch OSIL interface.
 */
/** @defgroup grp_eth_switch_il Integration Guide
    @ingroup grp_eth_switchdrv

    @addtogroup grp_eth_switch_il
    @{
    @section sec_eth_switch_integration Integration Guide
    Ethernet switch driver is implemented as a #SysCmdReq (system command
    request) and #SysCmdHandler (system command handler) model. This section
    also gives the necessary information on how to install command handler for
    Ethernet Switch Driver onto #SysCmdHandler.

    @section sec_eth_switch_il_seq Sequence Diagram
    Sequence diagrams in this section have SVC layer and SVC command handler
    lifelines.
    @subsection subsec_eth_switch_il_seq_svc SVC Layer interaction
    Below sequence diagram depicts execution sequence for Ethernet switch driver
    interfaces (APIs follow a similar sequence, #ETHERSWT_Init sequence is shown
    as an example).
    @image html eth_switch_il_diagram_svc.jpg "SVC Layer interaction"

    @subsection subsec_eth_switch_il_seq_tx_ts Switch Driver – Ethernet Driver TX TS sequence
    Below figure depicts interaction between Ethernet driver and Switch driver
    for time stamped transmit packet in case of Switch Management mode.
    @image html eth_switch_il_diagram_tx_ts.jpg

    @subsection subsec_eth_switch_il_seq_rx_ts Switch Driver – Ethernet Driver RX TS sequence
    Below figure depicts interaction between Ethernet driver and switch driver
    for time stamped receive packet in case of Switch Management mode.
    @image html eth_switch_il_diagram_rx_ts.jpg

    @subsection  subsec_eth_switch_il_seq_link_ind Switch Port Link State Change indication
    Below figure depicts sequence diagram for Switch port link change
    indication.
    @image html eth_switch_il_diagram_link_ind.jpg

    @file eth_switch_osil.h
    @brief Ethernet switch driver Integration Interfaces
    This header file contains the integration interfaces for Ethernet Switch
    driver
    @version 0.51 Imported from docx
*/
#ifndef ETH_SWITCH_OSIL_H
#define ETH_SWITCH_OSIL_H

#include <system.h>
#include <msg_queue.h>
#include <eth_switch.h>
#include <etherswt_ipc.h>
#include <cfp.h>

/**
    @name Ethernet Switch Driver interface IDs
    @{
    @brief API IDs for thernet Switch Driver
*/
#define BRCM_SWARCH_ETHERSWT_IL_CMD_TYPE                          (0x40U) /**< @brief #ETHERSWT_CmdType */
#define BRCM_SWARCH_ETHERSWT_IL_IO_TYPE                           (0x41U) /**< @brief #EthSwtIO */
#define BRCM_SWARCH_ETHERSWT_IL_CFG_GLOBAL                        (0x42U) /**< @brief #ETHERSWT_Cfg */
#define BRCM_SWARCH_ETHERSWT_IL_SYS_CMD_REQ_PROC                  (0x43U) /**< @brief #ETHERSWT_SysCmdReq */
#define BRCM_SWARCH_ETHERSWT_IL_SYS_CMD_HANDLER_PROC              (0x44U) /**< @brief #ETHERSWT_SysCmdHandler */
#define BRCM_SWARCH_ETHERSWT_IL_CMD_HANDLER_PROC                  (0x45U) /**< @brief #ETHERSWT_CmdHandler */
#define BRCM_SWARCH_ETHERSWT_IL_MGMT_INFO_IND_PROC                (0x46U) /**< @brief #ETHERSWT_MgmtInfoInd */
#define BRCM_SWARCH_ETHERSWT_IL_TX_TS_IND_PROC                    (0x47U) /**< @brief #ETHERSWT_TxTSInd */
#define BRCM_SWARCH_ETHERSWT_IL_RX_TS_IND_PROC                    (0x48U) /**< @brief #ETHERSWT_RxTSInd */
#define BRCM_SWARCH_ETHERSWT_IL_TX_ADAPT_BUF_PROC                 (0x49U) /**< @brief #ETHERSWT_TxAdaptBuffer */
#define BRCM_SWARCH_ETHERSWT_IL_TX_PROCESS_FRAME_PROC             (0x4AU) /**< @brief #ETHERSWT_TxProcessFrame */
#define BRCM_SWARCH_ETHERSWT_IL_TX_DONE_IND_PROC                  (0x4BU) /**< @brief #ETHERSWT_TxDoneInd */
#define BRCM_SWARCH_ETHERSWT_IL_RX_PROCESS_FRAME_PROC             (0x4CU) /**< @brief #ETHERSWT_RxProcessFrame */
#define BRCM_SWARCH_ETHERSWT_IL_RX_DONE_IND_PROC                  (0x4DU) /**< @brief #ETHERSWT_RxDoneInd */
#define BRCM_SWARCH_ETHERSWT_IL_LINK_IRQ_HANDLER_PROC             (0x4EU) /**< @brief #ETHERSWT_LinkIRQHandler */
#define BRCM_SWARCH_ETHERSWT_IL_LINK_STAT_CHG_IND_HANDLER_PROC    (0x4FU) /**< @brief #ETHERSWT_LinkStatChgIndHandler */
#define BRCM_SWARCH_ETHERSWT_IL_LINK_STATE_CHANGE_IND_PROC        (0x50U) /**< @brief #ETHERSWT_LinkStateChangeInd */
#define BRCM_SWARCH_ETHERSWT_INTG_LINK_STATE_CHANGE_IND_PROC      (0x51U) /**< @brief #ETHERSWT_IntgLinkStateChangeInd */
/** @} */

/**
    @name ETHERSWT_CmdType
    @{
    @brief Ethernet switch driver system command type

    @trace #BRCM_SWARCH_ETHERSWT_IL_CMD_TYPE #BRCM_REQ_ETHERSWT_IL_CMD
*/
typedef uint32_t ETHERSWT_CmdType;  /**< @brief Ethernet switch command type */
#define SWT_IO_CMD_INIT                      (0UL)
#define SWT_IO_CMD_GET_ARL_TABLE             (1UL)
#define SWT_IO_CMD_READ_REG                  (2UL)
#define SWT_IO_CMD_WRITE_REG                 (3UL)
#define SWT_IO_CMD_GET_RX_STAT               (4UL)
#define SWT_IO_CMD_GET_TX_STAT               (5UL)
#define SWT_IO_CMD_GET_PORT_MAC_ADDR         (6UL)
#define SWT_IO_CMD_SET_MACLEARNING_MODE      (7UL)
#define SWT_IO_CMD_GET_MACLEARNING_MODE      (8UL)
#define SWT_IO_CMD_ENABLE_VLAN               (9UL)
#define SWT_IO_CMD_SET_PORT_MIRROR_CFG       (10UL)
#define SWT_IO_CMD_GET_PORT_MIRROR_CFG       (11UL)
#define SWT_IO_CMD_SET_PORT_MIRROR_MODE      (12UL)
#define SWT_IO_CMD_GET_PORT_MIRROR_MODE      (13UL)
#define SWT_IO_CMD_MGMT_INIT                 (14UL)
#define SWT_IO_CMD_SET_MGMT_INFO             (15UL)
#define SWT_IO_CMD_ENABLE_TX_TIMESTAMP       (16UL)
#define SWT_IO_CMD_SET_LED_STATE             (17UL)
#define SWT_IO_CMD_GET_LED_STATE             (18UL)
#define SWT_IO_CMD_SET_JUMBO_MODE            (19UL)
#define SWT_IO_CMD_GET_JUMBO_MODE            (20UL)
#define SWT_IO_CMD_GET_DUMBFWD_MODE          (21UL)
#define SWT_IO_CMD_SET_DUMBFWD_MODE          (22UL)
#define SWT_IO_CMD_GET_XCVR_STATS            (23UL)
#define SWT_IO_CMD_GET_LINKSTATE             (24UL)
#define SWT_IO_CMD_ADD_VLAN_PORT             (25UL)
#define SWT_IO_CMD_REMOVE_VLAN_PORT          (26UL)
#define SWT_IO_CMD_SET_PORT_DEFAULT_VLAN     (27UL)
#define SWT_IO_CMD_SET_VLAN_IFILTER_MODE     (28UL)
#define SWT_IO_CMD_SET_AGE                   (29UL)
#define SWT_IO_CMD_GET_AGE                   (30UL)
#define SWT_IO_CMD_ADD_ARL_ENTRY             (31UL)
#define SWT_IO_CMD_DELETE_ARL_ENTRY          (32UL)
#define SWT_IO_CMD_GET_VLAN_PORTS            (33UL)
#define SWT_IO_CMD_GET_PORT_DEFAULT_VLAN     (34UL)
#define SWT_IO_CMD_GET_VLAN_IFILTER_MODE     (35UL)
#define SWT_IO_CMD_CLEAR_RX_STAT             (36UL)
#define SWT_IO_CMD_CLEAR_TX_STAT             (37UL)
#define SWT_IO_CMD_GET_MIRROR_CAPTURE_PORT   (38UL)
#define SWT_IO_CMD_MGMT_DEINIT               (39UL)
#define SWT_IO_CMD_TX_ADAPT_BUFFER           (40UL)
#define SWT_IO_CMD_TX_PROCESS_FRAME          (41UL)
#define SWT_IO_CMD_TX_DONE_IND               (42UL)
#define SWT_IO_CMD_TX_DONE_IND_COMPLETE      (43UL)
#define SWT_IO_CMD_RX_PROCESS_FRAME          (44UL)
#define SWT_IO_CMD_RX_DONE_IND               (45UL)
#define SWT_IO_CMD_RX_DONE_IND_COMPLETE      (46UL)
#define SWT_IO_CMD_LINK_STATE_CHANGE_HDLR    (47UL)

#define SWT_IO_CMD_CFP_INIT                  (100UL)
#define SWT_IO_CMD_CFP_ADDRULE               (101UL)
#define SWT_IO_CMD_CFP_REMOVERULE            (102UL)
#define SWT_IO_CMD_CFP_UPDATERULE            (103UL)
#define SWT_IO_CMD_CFP_GETSTATS              (104UL)
#define SWT_IO_CMD_CFP_GETSNAPSHOT           (105UL)
#define SWT_IO_CMD_CFP_GETROWCONFIG          (106UL)
#define SWT_IO_CMD_CFP_ENABLEPORT            (107UL)
#define SWT_IO_CMD_CFP_DISABLEPORT           (108UL)
#define SWT_IO_CMD_CFP_DEINIT                (109UL)

/** @} */

/**
    @brief Ethernet switch IO structure

    @trace #BRCM_SWARCH_ETHERSWT_IL_IO_TYPE #BRCM_REQ_ETHERSWT_IL_IO
 */
typedef struct _EthSwtIO {
    int32_t                     retVal;             /**< @brief Return value */
    ETHERSWT_HwIDType           swtHwID;            /**< @brief Switch hardware
                                                         index */
    ETHERSWT_PortIDType         portHwID;           /**< @brief Port hardware
                                                         index */
    uint32_t                    mode;               /**< @brief Switch mode */
    ETHERSWT_VLANIDType         vlanID;             /**< @brief VLAN ID */
    const ETHERSWT_CfgType      *cfg;               /**< @brief Pointer to
                                                         switch configurations
                                                         */
    ETHERSWT_ARLEntryType       *ARLTbl;            /**< @brief Pointer to ARL
                                                         table */
    uint16_t                    *ARLTblSz;          /**< @brief Pointer to ARL
                                                         table size */
    ETHER_RxStatsType           *rxStat;            /**< @brief Pointer to RX
                                                     statistics */
    ETHER_TxStatsType           *txStat;            /**< @brief Pointer to TX
                                                         statistics */
    uint64_t                    *regVal;            /**< @brief Pointer to
                                                         register value */
    uint8_t const               *macAddr;           /**< @brief Pointer to MAC
                                                         address */
    ETHERSWT_MacLearningMode    *learningMode;      /**< @brief Pointer to MAC
                                                         learning mode */
    ETHERSWT_PortMirrorCfgType  *portMirrorCfg;     /**< @brief Pointer to port
                                                         mirror configurations
                                                         */
    ETHERSWT_PortMirrorStateType *portMirrorState;  /**< @brief Pointer to port
                                                         mirror state */
    uint32_t                    regAddr;            /**< @brief Register
                                                         Address */
    uint32_t                    enable;             /**< @brief Enable flag */
    ETHER_HwIDType              ctrlHwID;           /**< @brief Controller
                                                         hardware index */
    uint32_t                    bufIdx;             /**< @brief Buffer index */
    ETHERSWT_MgmtInfoType       *mgmtInfo;          /**< @brief Pointer to
                                                         management information
                                                         */
    const ETHERSWT_MgmtInfoType *mgmtInfoIn;        /**< @brief Pointer to
                                                         management information
                                                         */
    ETHERSWT_LedType            ledType;            /**< @brief LED type */

    const CFP_ConfigType        *cfpCfg;            /**< @brief Pointer to CFP
                                                         configuration */
    const CFP_KeyType           *keyCfg;            /**< @brief Pointer to CFP Key
                                                         configuration */
    const CFP_ActionType        *actionCfg;         /**< @brief Pointer to CFP
                                                         Action configuration */
    CFP_TableSnapshotType       *snapShot;          /**< @brief Pointer to CFP
                                                         table snapshot */
    CFP_RuleType                *ruleCfg;           /**< @brief Pointer to CFP
                                                         Rule configuration */
    uint32_t                    *row;               /**< @brief Pointer to CFP
                                                         row number */
    uint32_t                    *slice;             /**< @brief Pointer to CFP
                                                         slice number */
    CFP_StatsType               *cfpStats;          /**< @brief Pointer to CFP
                                                         statistics */
    ETHERSWT_PortStatsType      *portStats;         /**< @brief Port Statistics */
    ETHERSWT_ARLEntryType       *arlEntry;          /**< @brief Pointer to
                                                         ARL entry */
    uint8_t                     linkState;          /**< @brief Link state */
    uint8_t                     jumbo;              /**< @brief Jumbo enable */
    uint32_t                    age;                /**< @brief FDB age */
    uint32_t                    portMask;           /**< @brief Port mask */
    uint32_t                    tagMask;            /**< @brief Tag mask */
    uint32_t                    staticPortMask;     /**< @brief Static port mask */
    ETHERSWT_TrafficDirType     direction;          /**< @brief Traffic direction */
    ETHER_PrioType              prio;               /**< @brief Packet priority */
    uint32_t                    *port2TimeFifoMap;  /**< @brief Pointer to Switch port - time FIFO map */
    uint8_t                     **data;             /**< @brief Pointer to a pointer to data*/
    uint32_t                    *len;               /**< @brief Pointer to length */
    uint32_t                    *isMgmtFrameOnly;   /**< @brief Pointer to MgmtFrameOnly value */
    ETHER_TimestampType         *ts;                /**< @brief Pointer to timestamp */
    ETHER_TimestampQualType     *tsQual;            /**< @brief Pointer to timestamp Qual*/
    uint32_t                    tsAvailable;        /**< @brief Timestamp available */
    uint8_t                     *buf;               /**< @brief Pointer to buffer */
    ETHXCVR_LinkStateType       *linkStatePtr;      /**< @brief Pointer to Link State*/
    uint32_t                    *isLinkStateChangedPtr;  /**< @brief Pointer to Link state changed */
} EthSwtIO;

/**
    @brief Ethernet Switch driver configuration

    Extern constant pointer to Ethernet switch configurations

    @trace #BRCM_SWARCH_ETHERSWT_IL_CFG_GLOBAL #BRCM_REQ_ETHERSWT_IL_CFG
 */
extern const ETHERSWT_CfgType *const ETHERSWT_Cfg;

/** @brief Raise a command request

    This interface raises a command request to the Ethernet switch driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]]     aCmd        Command requested by user
    @param[inout]   aIO         Pointer to in-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     Others                  As returned by #ETHERSWT_CmdHandler
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_SYS_CMD_REQ_PROC  #BRCM_REQ_ETHERSWT_IL_SYS_CMD_REQ

    @limitations None
*/
extern int32_t ETHERSWT_SysCmdReq(ETHERSWT_CmdType aCmd, EthSwtIO *const aIO);

/** @brief Handle all the Ethernet switch driver commands requested

    This interface handles all the Ethernet switch driver commands requested
    from System command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    Ethernet switch driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as EthSwtIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_SYS_CMD_HANDLER_PROC  #BRCM_REQ_ETHERSWT_IL_SYS_CMD_HANDLER

    @limitations None
*/
extern void ETHERSWT_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd,
        uint8_t * aSysIO);

/** @brief Internal command handler

    This interface handles all the commands requested from system command
    handler.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to in-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command processed successfully
    @retval     #BCM_ERR_UNKNOWN        aIO is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     Others                  Error value specific to the requested
                                        command

    @post TBD

    @note This interface shall be called from #ETHERSWT_SysCmdHandler.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_CMD_HANDLER_PROC  #BRCM_REQ_ETHERSWT_IL_CMD_HANDLER

    @limitations any limitations for this api

    @snippet switch_osil.c Usage of ETHERSWT_CmdHandler
*/
extern int32_t ETHERSWT_CmdHandler(ETHERSWT_CmdType aCmd, EthSwtIO * const aIO);

/** @brief  Switch Management information call back

    Switch Management information call back from switch driver for Rx buffer
    received.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of Ethernet controller
    @param[in]      aData       Pointer to data buffer
    @param[inout]   aMgmtInfo   Pointer to management information

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_MGMT_INFO_IND_PROC  #BRCM_REQ_ETHERSWT_IL_MGMT_INFO_IND

    @limitations any limitations for this api
*/
extern void ETHERSWT_MgmtInfoInd(ETHER_HwIDType aCntrlID,
        const uint8_t *const aData,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo);

/** @brief Callback from switch driver to provide timestamp of TX packet

    This callback is raised by the switch driver to provide timestamp of
    transmitted packet.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Pointer to data buffer
    @param[in]      aMgmtInfo   Pointer to management information
    @param[in]      aTS         Pointer to packet timestamp
    @param[in]      aTSQual     Pointer to packet timestamp quality

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_TX_TS_IND_PROC  #BRCM_REQ_ETHERSWT_IL_TX_TS_IND

    @limitations None
*/
extern void ETHERSWT_TxTSInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo,
        const ETHER_TimestampType *const aTS,
        const ETHER_TimestampQualType* const aTSQual);

/** @brief allback from switch driver to provide timestamp of RX packet

    This callback is raised by the switch driver to provide timestamp of
    received packet.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aData       Pointer to data buffer
    @param[in]      aMgmtInfo   Pointer to management information
    @param[in]      aTS         Pointer to packet timestamp
    @param[in]      aTSQual     Pointer to packet timestamp quality

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_FUNCTION_PROC  #BRCM_REQ_ETHERSWT_FUNCTION

    @limitations None
*/
extern void ETHERSWT_RxTSInd(ETHER_HwIDType aCntrlID,
        const uint8_t *const aData,
        const ETHERSWT_MgmtInfoType *const aMgmtInfo,
        const ETHER_TimestampType *const aTS,
        const ETHER_TimestampQualType* const aTSQual);

/** @brief Adapt the transmit buffer

    This API adapts the transmit buffer and prepare for addition of management
    information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame
    @param[inout]   aLenInOut   - In: Size of the Ethernet frame without
                                  management information
                                - Out: Size of the Ethernet frame with
                                  management information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully adapted transmit buffer
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut points to a NULL pointer
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_TX_ADAPT_BUF_PROC  #BRCM_REQ_ETHERSWT_IL_TX_ADAPT_BUF

    @limitations None
*/
extern int32_t ETHERSWT_TxAdaptBuffer(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t **const aDataInOut,
        uint32_t * const aLenInOut);

/** @brief Process transmit frame

    This API processes transmit frame to add management information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame
    @param[inout]   aLenInOut   - In: Size of the Ethernet frame without
                                  management information
                                - Out: Size of the Ethernet frame with
                                  management information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully processed transmit
                                            frame
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid aBufIdx
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Data buffer pointer pointed by
                                            aDataInOut is  NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Value pointed by aLenInOut is zero
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_TX_PROCESS_FRAME_PROC  #BRCM_REQ_ETHERSWT_IL_TX_PROCESS_FRAME

    @limitations None
*/
extern int32_t ETHERSWT_TxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut);

/** @brief Callback indication from the Ethernet driver when a management
    enabled packet transmission is completed

    This callback indication is called from the Ethernet driver when a
    management enabled packet transmission is completed and there is nothing
    more to be done on that packet. (This indication comes from the Ethernet
    driver when all the required processing for the frame is completed including
    the TX confirmation callback to the client.)

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      bufIdx      Adapted Ethernet buffer index


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_TX_DONE_IND_PROC  #BRCM_REQ_ETHERSWT_IL_TX_DONE_IND

    @limitations None
*/
extern int32_t ETHERSWT_TxDoneInd (ETHER_HwIDType aCntrlID,
        uint32_t bufIdx);

/** @brief RX process frame callback

    This RX process frame callback is called from the Ethernet driver when a
    packet arrives to extract management information.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aDataInOut  - In: Pointer to the EtherType in common
                                  Ethernet frame
                                - Out: Pointer to EtherType in management frame
    @param[inout]   aLenInOut   - In: Length of the frame received
                                - Out: Length of the frame deceased by the
                                  management information
    @param[out]     isMgmtFrame Set to #TRUE if its management frame only
                                (consumed by Ethernet switch driver)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully processed
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aDataInOut points to a NULL pointer
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut in NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aLenInOut points to zero
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized
    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_RX_PROCESS_FRAME_PROC  #BRCM_REQ_ETHERSWT_IL_RX_PROCESS_FRAME

    @limitations None
*/
extern int32_t ETHERSWT_RxProcessFrame(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx,
        uint8_t ** const aDataInOut,
        uint32_t * const aLenInOut,
        uint32_t * const isMgmtFrame);

/** @brief RX finished callback indication

    Rx finished callback indication from the Ethernet driver. This interface
    shall raise the Rx timestamp callback to the client.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of the Ethernet controller
    @param[in]      aBufIdx     Adapted Ethernet buffer index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_BUF_STATE    Ethernet buffer is in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_STATE        Ethernet switch driver management
                                            support is not initialized
    @retval     #BCM_ERR_UNINIT             Ethernet switch driver is not
                                            initialized

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_RX_DONE_IND_PROC  #BRCM_REQ_ETHERSWT_IL_RX_DONE_IND

    @limitations any None
*/
extern int32_t ETHERSWT_RxDoneInd(ETHER_HwIDType aCntrlID,
        uint32_t aBufIdx);

/** @brief Port link IRQ handler

    Port link IRQ handler to handle link state change.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index

    @return     void

    @post TBD

    @note This interface shall be called from the corressponding port's
    interrupt handler. Integrator shall install the port link interrupt handler
    for each switch port in the vector table. Refer to @ref sec_ref_trm for
    interrupt number mapping.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_LINK_IRQ_HANDLER_PROC  #BRCM_REQ_ETHERSWT_IL_LINK_IRQ_HANDLER

    @limitations None

    @snippet switch_osil.c Usage of ETHERSWT_LinkIRQHandler
*/
extern void ETHERSWT_LinkIRQHandler(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID);

/** @brief Port link state change indication handler

    Port link state change indication handler.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index

    @return     void

    @post TBD

    @note This interface shall be called from the comms task whenever there it
    is notified of a link status change.

    @trace  #BRCM_SWARCH_ETHERSWT_IL_LINK_STAT_CHG_IND_HANDLER_PROC  #BRCM_REQ_ETHERSWT_IL_LINK_STAT_CHG_IND_HANDLER

    @limitations None

    @snippet comms_osil.c Usage of ETHERSWT_LinkStatChgIndHandler
*/
extern void ETHERSWT_LinkStatChgIndHandler(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID);

/** @brief Port link state change indication callback for integration layer .

    Integration layer Port link state change indication callback.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aLinkState  Link current state

    @return     void

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_INTG_LINK_STATE_CHANGE_IND_PROC  #BRCM_REQ_ETHERSWT_INTG_LINK_STATE_CHANGE_IND

    @limitations None
*/
extern void ETHERSWT_IntgLinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType aLinkState);

/** @brief Port link state change indication callback.

    Port link state change indication callback.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aLinkState  Link current state

    @return     void

    @post TBD

    @note TBD
    @todo Explain how to use this interface (or where to be called from)

    @trace  #BRCM_SWARCH_ETHERSWT_IL_LINK_STATE_CHANGE_IND_PROC  #BRCM_REQ_ETHERSWT_IL_LINK_STATE_CHANGE_IND

    @limitations None
*/
extern void ETHERSWT_LinkStateChangeInd(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType aLinkState);

#endif /* ETH_SWITCH_OSIL_H */
/** @} */
