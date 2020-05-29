/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_eth_cntlrdrv_il Integration Guide
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_il
    @{
    @section sec_eth_integration Integration Guide
    Ethernet driver requires a task for deferred Ethernet processing, from which the TX and RX client callback
    indications are called. This task shall be called as CommsTask further in this document. CommsTask shall be signaled
    from the Ethernet IRQ handler and shall have a higher priority than the client tasks. This section gives the
    different integration interfaces that shall be used to implement the Ethernet IRQ handler and the CommsTask.

    Ethernet driver is implemented as a SysCmdReq (system command request) and SysCmdHandler
    (system command handler) model. This section also gives the necessary information on how to
    install command handler for Ethernet Driver onto SysCmdHandler

    @section sec_eth_il_seq Sequence Diagram

    @subsection subsec1_eth_il_seq SVC Layer Interaction
    @image html eth_seq_diagram_svc_layer.jpg "SVC layer interaction"
    Above sequence diagram depicts execution sequence for Ethernet channel initialization (#ETHER_DeInit,
    #ETHER_SetMacAddr, #ETHER_GetTxBuffer, #ETHER_Send, #ETHER_GetRxStats, #ETHER_EnableTxTimestamp,
    #ETHER_GetTxTimestamp, #ETHER_GetRxTimestamp, #ETHER_GetTime, #ETHER_SetTime, #ETHER_SetCorrectionTime,
    #ETHER_TimeInit, #ETHER_TimeInitStateHandler, #ETHER_SetMode, #ETHER_GetMode, #ETHXCVR_Init, #ETHXCVR_SetMode,
    #ETHXCVR_GetMode, #ETHXCVR_GetLinkState and #ETHXCVR_GetSQIValue APIs follow a similar sequence).

    @subsection subsec2_eth_il_seq Ethernet Packet Transmit
    @image html eth_seq_diagram_pkt_tx.jpg "Ethernet packet transmit"

    @subsection subsec3_eth_il_seq Ethernet Packet Receive
    @image html eth_seq_diagram_pkt_rx.jpg "Ethernet packet receive"

    @subsection subsec4_eth_il_seq Ethernet TX confirmation
    @image html eth_seq_diagram_tx_confirm.jpg "Ethernet TX confirmation"
    This figure depicts TX confirmation callback sequence in case of Time Handler

    @subsection subsec5_eth_il_seq Ethernet Driver – Switch Driver TX sequence
    @image html eth_seq_diagram_swt_tx.jpg "Ethernet driver – Switch driver TX sequence"
    This figure depicts interaction between Ethernet driver and switch driver for transmit packet in case of Switch
    Management mode.

    @subsection subsec6_eth_il_seq Ethernet Driver – Switch Driver TX TS sequence
    @image html eth_seq_diagram_swt_tx_ts.jpg "Ethernet driver – Switch driver TX timestamp sequence"
    This figure depicts interaction between Ethernet driver and switch driver for time stamped transmit packet in case
    of Switch Management mode.

    @subsection subsec7_eth_il_seq Ethernet Driver – Switch Driver RX TS sequence
    @image html eth_seq_diagram_swt_rx_ts.jpg "Ethernet driver – Switch driver RX timestamp sequence"
    This figure depicts interaction between Ethernet driver and switch driver for time stamped transmit packet in case
    of Switch Management mode.

    @subsection subsec8_eth_il_seq Transceiver Link Status Change Indication sequence
    @image html eth_seq_diagram_link_status_ind.jpg "Transceiver link status change indication"
    This figure depicts interaction sequence for transceiver link status change indication.

    @file eth_osil.h
    @brief Ethernet Driver Integration Interfaces
    This header file contains the integration interface for Ethernet Driver

    @version 1.26 Imported from docx
*/

#ifndef ETH_OSIL_H
#define ETH_OSIL_H

#include <system.h>
#include <ethernet.h>
#include <ether_ipc.h>

/**
    @name Ethernet Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for Ethernet Driver
*/
#define BRCM_SWARCH_ETHER_CNTLRIOCMD_TYPE                             (0x40U) /**< @brief #ETHER_CntlrIOCmdType */
#define BRCM_SWARCH_ETHER_TIMEIOCMD_TYPE                              (0x41U) /**< @brief #ETHER_TimeIOCmdType */
#define BRCM_SWARCH_ETHER_IRQEVENT_TYPE                               (0x42U) /**< @brief #ETHER_IRQEventType */
#define BRCM_SWARCH_ETHER_IO_TYPE                                     (0x43U) /**< @brief #ETHER_IOType */
#define BRCM_SWARCH_ETHER_TIMEIO_TYPE                                 (0x44U) /**< @brief #ETHER_TimeIOType */
#define BRCM_SWARCH_ETHER_TIMEINITSTATEHANDLER_PROC                   (0x45U) /**< @brief #ETHER_TimeInitStateHandler */
#define BRCM_SWARCH_ETHER_RXIND_PROC                                  (0x46U) /**< @brief #ETHER_RxInd */
#define BRCM_SWARCH_ETHER_TXDONEIND_PROC                              (0x47U) /**< @brief #ETHER_TxDoneInd */
#define BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC                          (0x48U) /**< @brief #ETHER_IntIRQHandler */
#define BRCM_SWARCH_ETHER_PROCESSPKTS_PROC                            (0x49U) /**< @brief #ETHER_ProcessPkts */
#define BRCM_SWARCH_ETHER_TIMESTAMPIRQHANDLER_PROC                    (0x4AU) /**< @brief #ETHER_TimestampIRQHandler */
#define BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC                      (0x4BU) /**< @brief #ETHER_ProcessTimestamps */
#define BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC                         (0x4CU) /**< @brief #ETHER_EnableLoopBack */
#define BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC                     (0x4DU) /**< @brief #ETHER_DisableAllChannels */
#define BRCM_SWARCH_ETHER_SYSCMDREQ_PROC                              (0x4EU) /**< @brief #ETHER_SysCmdReq */
#define BRCM_SWARCH_ETHER_SYSCMDHANDLER_PROC                          (0x4FU) /**< @brief #ETHER_SysCmdHandler */
#define BRCM_SWARCH_ETHER_TIMESYSCMDREQ_PROC                          (0x50U) /**< @brief #ETHER_TimeSysCmdReq */
#define BRCM_SWARCH_ETHER_TIMESYSCMDHANDLER_PROC                      (0x51U) /**< @brief #ETHER_TimeSysCmdHandler */
#define BRCM_SWARCH_ETHER_GETMDIOCONFIG_PROC                          (0x52U) /**< @brief #ETHER_GetMdioConfig */
/** @} */

/**
    @{
    @brief Ethernet driver system command request type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_CntlrIOCmdType;
#define ETHER_CNTLRIOCMD_INIT                      (0UL)    /**< @brief Ethernet controller IO command - Init */
#define ETHER_CNTLRIOCMD_DEINIT                    (1UL)    /**< @brief Ethernet controller IO command - DeInit */
#define ETHER_CNTLRIOCMD_GET_BUF                   (2UL)    /**< @brief Ethernet controller IO command - Get buffer */
#define ETHER_CNTLRIOCMD_SEND                      (3UL)    /**< @brief Ethernet controller IO command - Send */
#define ETHER_CNTLRIOCMD_GET_STATS                 (4UL)    /**< @brief Ethernet controller IO command - Get stats */
#define ETHER_CNTLRIOCMD_SET_MACADDR               (5UL)    /**< @brief Ethernet controller IO command - Set MAC address */
#define ETHER_CNTLRIOCMD_GET_MACADDR               (6UL)    /**< @brief Ethernet controller IO command - Get MAC address */
#define ETHER_CNTLRIOCMD_ENABLE_LOOP_BACK          (7UL)    /**< @brief Ethernet controller IO command - Enable loopback */
#define ETHER_CNTLRIOCMD_GET_RX_PKT                (8UL)    /**< @brief Ethernet controller IO command - Get Rx packets */
#define ETHER_CNTLRIOCMD_PUT_RX_PKT                (9UL)    /**< @brief Ethernet controller IO command - Put Rx packets */
#define ETHER_CNTLRIOCMD_GET_TX_PKT                (10UL)   /**< @brief Ethernet controller IO command - Get Tx packets */
#define ETHER_CNTLRIOCMD_PUT_TX_PKT                (11UL)   /**< @brief Ethernet controller IO command - Put Tx packets */
#define ETHER_CNTLRIOCMD_SET_MODE                  (12UL)   /**< @brief Ethernet controller IO command - Set mode */
#define ETHER_CNTLRIOCMD_GET_MODE                  (13UL)   /**< @brief Ethernet controller IO command - Get mode */
#define ETHER_CNTLRIOCMD_SET_GPTIMER               (14UL)   /**< @brief Ethernet controller IO command - Set GP timer */
#define ETHER_CNTLRIOCMD_TX_MARK_TS_PKT            (15UL)   /**< @brief Ethernet controller IO command - Tx mark TS packet */
#define ETHER_CNTLRIOCMD_IS_TX_PKT_TS_MARKED       (16UL)   /**< @brief Ethernet controller IO command - Is Tx packet marked for TS */
#define ETHER_CNTLRIOCMD_TX_TS_DONE_IND            (17UL)   /**< @brief Ethernet controller IO command - Tx TS done indication */
#define ETHER_CNTLRIOCMD_MDIO_READ                 (18UL)   /**< @brief Ethernet controller IO command - MDIO Read */
#define ETHER_CNTLRIOCMD_MDIO_WRITE                (19UL)   /**< @brief Ethernet controller IO command - MDIO Write */
#define ETHER_CNTLRIOCMD_MDIO_CHECKSTATUS          (20UL)   /**< @brief Ethernet controller IO command - MDIO check status */
/** @} */

/**
    @{
    @brief Ethernet driver system command request type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_TimeIOCmdType;                       /**< @brief Ethernet Time IO command type */
#define ETHER_TIMEIOCMD_ENABLE_EGRESS_TIMESTAMP   (0UL)     /**< @brief Ethernet time IO command - Enable egress timestamp */
#define ETHER_TIMEIOCMD_DISABLE_EGRESS_TIMESTAMP  (1UL)     /**< @brief Ethernet time IO command - Disable egress timestamp */
#define ETHER_TIMEIOCMD_UPDATE_EGRESS_PKT_INFO    (2UL)     /**< @brief Ethernet time IO command - Update egress packet info */
#define ETHER_TIMEIOCMD_GET_EGRESS_TIMESTAMP      (3UL)     /**< @brief Ethernet time IO command - Get egress timestamp */
#define ETHER_TIMEIOCMD_GET_INGRESS_TIMESTAMP     (4UL)     /**< @brief Ethernet time IO command - Get ingress timestamp */
#define ETHER_TIMEIOCMD_TIME_INIT                 (5UL)     /**< @brief Ethernet time IO command - Time init */
#define ETHER_TIMEIOCMD_TIME_INIT_STATE_HANDLER   (6UL)     /**< @brief Ethernet time IO command - Time init state handler */
#define ETHER_TIMEIOCMD_GET_TIME                  (7UL)     /**< @brief Ethernet time IO command - Get time */
#define ETHER_TIMEIOCMD_SET_TIME                  (8UL)     /**< @brief Ethernet time IO command - Set time */
#define ETHER_TIMEIOCMD_SET_CORRECTION_TIME       (9UL)     /**< @brief Ethernet time IO command - Set correction time */
#define ETHER_TIMEIOCMD_PROCESS_TS                (10UL)    /**< @brief Ethernet time IO command - Process timestamp */
/** @} */



/**
    @name ETHER_IRQEventType
    @{
    @brief Ethernet driver interrupt event type

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
typedef uint32_t ETHER_IRQEventType;                             /**< @brief ETHER_IRQEventType typedef */
#define ETHER_IRQEVENT_PKT                         (1UL << 0UL) /**< @brief Packet DMA event */
#define ETHER_IRQEVENT_TIMER                       (1UL << 1UL) /**< @brief GPTimer event */
/** @} */

/**
    @brief IO structure for Ethernet driver

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_IOType {
    int32_t retVal;                           /**< @brief Error value as set by #ETHER_SysCmdHandler */
    ETHER_HwIDType hwID;                      /**< @brief Ethernet controller Index */
    const ETHER_CfgType *ctrlCfg;             /**< @brief Ethernet controller config */
    uint32_t *buffIdxInOut;                   /**< @brief Pointer to buffer index */
    uint8_t *buf;                             /**< @brief Buffer */
    uint32_t *lenInOut;                       /**< @brief Pointer to length of buffer */
    uint32_t txConfirmation;                  /**< @brief TX confirmation */
    uint32_t mode;                            /**< @brief Transceiver/controller mode */
    ETHER_RxStatsType *stats;                     /**< @brief RX stats */
    const uint8_t *macAddrIn;                 /**< @brief MAC address in */
    uint8_t *macAddrOut;                      /**< @brief MAC address out */
    uint32_t *raiseCb;                        /**< @brief Pointer to callback flag */
    uint32_t timerPeriod;                     /**< @brief GP Timer period in nanoseconds */
    ETHER_PrioType priority;                  /**< @brief Buffer priority (PCP) */
    MDIO_HwIDType mdioHwID;                   /**< @brief MDIO hardware ID */
    MDIO_PktType *mdioPkt;                     /**< @brief MDIO packet */
    MDIO_JobIDType mdioJobId;                 /**< @brief MDIO Job ID */
} ETHER_IOType;

/**
    @brief IO structure for Ethernet driver

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TimeIOType {
    int32_t retVal;                           /**< @brief Error value as set by #ETHER_SysCmdHandler */
    ETHER_HwIDType hwID;                      /**< @brief Ethernet controller Index */
    const uint8_t *pktBuf;                    /**< @brief Packet buffer */
    uint32_t *buffIdxInOut;                   /**< @brief Pointer to buffer index */
    ETHER_TimestampType *timestamp;           /**< @brief Time stamp */
    const ETHER_TimestampType *roTimestamp;   /**< @brief Read-only time stamp */
    uint32_t portIdx;                         /**< @brief GP Timer period in nanoseconds */
    ETHER_TimestampQualType *tsQuality;       /**< @brief Time stamp quality */
    const ETHER_TimestampDiffType *tsDiff;    /**< @brief Read-only Time stamp difference */
    ETHER_RateRatioType *rateRatio;           /**< @brief Rate ratio */
    const ETHER_RateRatioType *roRateRatio;   /**< @brief Read-only Rate ratio */
    ETHER_TimeCfgType *timeCfg;               /**< @brief Pointer to Ethernet time config */
    ETHER_TimeInitCmdType initCmd;            /**< @brief Ethernet time init command */
    ETHER_TimeInitOutType *timeVal;           /**< @brief Ethernet time value */
    uint64_t    sysTimeDiffNs;                /**< @brief systime diff nsec */
} ETHER_TimeIOType;

/** @brief Ethernet time initialization state handler

    This API performs the next steps of initialization of Ethernet time module.
    It handles all the initialization states of Ethernet time module.
    Refer to [@ref ref_sads_spec] for information on when to invoke this API.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet Time hardware ID
    @param[in]      aCmd                Ethernet Time initialization command
    @param[in]      aOutput             Ethernet Time command output

    @return     void

    @post None

    @trace  #BRCM_SWREQ_ETHER_TIME

    @limitations None
*/
extern void ETHER_TimeInitStateHandler(ETHER_HwIDType aID,
                                       ETHER_TimeInitCmdType aCmd,
                                       uint64_t aSysTimeDiffNs,
                                       ETHER_TimeInitOutType *const aOutput);

/** @brief Ethernet RX packet indication callback

    This interface is called by the Ethernet driver for every RX packet received. Depending on the ethtype of the packet
    received, the corresponding RX call back to the respective client shall be raised from this interface.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[in]      aBuf            Pointer to packet buffer
    @param[in]      aLen            Length of the packet
    @param[in]      aSrcMacAddr     Pointer to source MAC address
    @param[in]      aDestMacAddr    Pointer to destination MAC address

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_INDICATIONS

    @limitations None
*/
extern void ETHER_RxInd(ETHER_HwIDType aID,
                        const uint8_t *const aBuf,
                        uint32_t aLen,
                        const uint8_t *const aSrcMacAddr,
                        const uint8_t *const aDestMacAddr);

/** @brief Ethernet TX packet send confirmation callback

    This interface is called by the Ethernet Driver to indicate transmission completion of a particular packet.
    Depending on the ethype of the transmitted packet, the corresponding TX callback to the respective client shall be
    raised from this interface. This interface is not invoked by the Ethernet driver if the packet was sent with
    aTxConfirmation set as #FALSE (refer to #ETHER_Send).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[in]      aBufIdx         Packet buffer index
    @param[in]      aResult         Status of packet transmission

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_INDICATIONS

    @limitations None
*/
extern void ETHER_TxDoneInd(ETHER_HwIDType aID,
                            uint32_t aBufIdx,
                            ETHER_ResultType aResult);

/** @brief Handle Ethernet interrupts

    This API handles the interrupt for the given Ethernet controller. Depending on the Ethernet controller for which the
    Interrupt is triggered, the interrupt service routine shall call this interface with the corresponding Ethernet
    controller ID.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[out]     aEvnt           Ethernet IRQ event type

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER

    @limitations None

    @snippet eth_osil.c Usage of ETHER_IntIRQHandler
*/
extern void ETHER_IntIRQHandler(ETHER_HwIDType aID,
                                ETHER_IRQEventType* const aEvnt);

/** @brief Handle deferred Ethernet packet processing

    This interface handles deferred Ethernet processing. This interface shall be called to process the Ethernet Packets
    (TX/RX) from thread/IRQ context (preferably thread context since it goes through DMA descriptor table and processes
    all TX/RX packets). This interface shall be called to process the Ethernet Packets (TX/RX) from CommsTask.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID

    @return     void

    post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_COMMUNICATION

    @limitations None

    @snippet comms_osil.c Usage of ETHER_ProcessPkts
*/
extern void ETHER_ProcessPkts(ETHER_HwIDType aID);

/** @brief Ethernet timestamp hardware IRQ handler

    This API handles ethernet timestamp hardware interrupts.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_KERNEL_HANDLER

    @limitations None

    @snippet eth_osil.c Usage of ETHER_TimestampIRQHandler
*/
extern void ETHER_TimestampIRQHandler(uint32_t aID);

/** @brief Handles deferred Ethernet timestamp processing

    This interface handles bottom half processing of the hardware timestamp interrupt. It shall be invoked from the
    CommsTask when it is signaled from the interrupt handler.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_TIME

    @limitations None

    @snippet comms_osil.c Usage of ETHER_ProcessTimestamps
*/
extern void ETHER_ProcessTimestamps(uint32_t aID);

/** @brief Disable all Ethernet channels

    This interface disables all Ethernet TX & RX channels, disables and clears all interrupts. This interface can be
    called during system initialization to disable all TX and RX channels, and disable and clear all interrupts before
    Ethernet channel initialization.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Ethernet controller ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION

    @limitations None
*/
extern void ETHER_DisableAllChannels(ETHER_HwIDType aID);

/** @brief Enable Unimac loop back

    This interface enables MAC loop back. This interface shall be used only for testing purposes.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Ethernet controller ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ETHER_COMMUNICATION

    @limitations None
*/
extern void ETHER_EnableLoopBack(ETHER_HwIDType aID);

/** @brief Interface to raise a request to Ethernet driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned by #ETHER_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t ETHER_SysCmdReq(ETHER_CntlrIOCmdType aCmd,
                               ETHER_IOType *const aIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    Ethernet SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      uint8_t * to be interpreted as ETHER_IOType

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_KERNEL_HANDLER

    @limitations None
*/
extern void ETHER_SysCmdHandler(uint32_t aMagicID,
                                uint32_t aCmd,
                                uint8_t * aSysIO);


/** @brief Interface to raise a request to Ethernet driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned by #ETHER_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t ETHER_TimeSysCmdReq(ETHER_TimeIOCmdType aCmd,
                               ETHER_TimeIOType *const aIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    Ethernet SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      uint8_t * to be interpreted as ETHER_IOType

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_KERNEL_HANDLER

    @limitations None
*/
extern void ETHER_TimeSysCmdHandler(uint32_t aMagicID,
                                uint32_t aCmd,
                                uint8_t * aSysIO);


/** @brief Get MDIO configuration

    This API would be called in priviledged context to retrieve MDIO
    controller driver configuration parameters

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]  aCfg    Pointer to an array of MDIO_CfgType structures
    @param[out]  aCount  Pointer to obtain the number of entries in aCfg

    @return     BCM_ERR_INVAL_PARAMS    aCfg or aCount is NULL
    @return     BCM_ERR_OK              Configuration obtained successfully

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER

    @limitations None
*/
extern int32_t ETHER_GetMdioConfig(const MDIO_CfgType **aCfg, uint32_t *aCount);

#endif /* ETH_OSIL_H */

/** @} */
