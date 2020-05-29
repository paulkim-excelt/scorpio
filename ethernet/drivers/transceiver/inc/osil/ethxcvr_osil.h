/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_eth_xcvrdrv_il Integration Guide
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_il
    @{
    @section sec_ethxcvr_integration Integration Guide
    Ethernet transceiver driver requires a task for deferred processing.
    This task shall be called as CommsTask further in this document.
    CommsTask shall be signaled from the Ethernet transceiver IRQ handler and
    shall have a higher priority than the client tasks.  This section gives the
    different integration interfaces that shall be used to implement the
    Ethernet IRQ handler and the CommsTask.

    Ethernet transceiver driver is implemented as a SysCmdReq (system command
    request) and SysCmdHandler (system command handler) model. This section also
    gives the necessary information on how to install command handler for
    Ethernet Transceiver Driver onto SysCmdHandler

    @section sec_ethxcvr_il_seq Sequence Diagram

    @subsection subsec1_ethxcvr_il_seq SVC Layer Interaction
    @image html eth_seq_diagram_svc_layer.jpg "SVC layer interaction"
    Above sequence diagram depicts execution sequence for Ethernet channel
    initialization (#ETHXCVR_Init, #ETHXCVR_SetMode, #ETHXCVR_GetMode,
    #ETHXCVR_GetLinkState and #ETHXCVR_GetSQIValue APIs follow a similar
    sequence).

    @subsection subsec2_ethxcvr_il_seq Transceiver Link Status Change Indication sequence
    @image html eth_seq_diagram_link_status_ind.jpg "Transceiver link status change indication"
    This figure depicts interaction sequence for transceiver link status change indication.

    @file ethxcvr_osil.h
    @brief Ethernet Transceiver Driver Integration Interfaces
    This header file contains the integration interface for Ethernet Transceiver Driver

    @version 1.26 Imported from docx
*/

#ifndef ETHXCVR_OSIL_H
#define ETHXCVR_OSIL_H

#include <system.h>
#include <eth_xcvr.h>

/**
    @name Ethernet Transceiver Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for Ethernet Transceiver Driver
*/
#define BRCM_SWARCH_ETHXCVR_CMD_TYPE                   (0x40U) /**< @brief #ETHXCVR_CmdType */
#define BRCM_SWARCH_ETHXCVR_IO_TYPE                    (0x41U) /**< @brief #ETHXCVR_IOType */
#define BRCM_SWARCH_ETHXCVR_SYSCMDREQ_PROC             (0x42U) /**< @brief #ETHXCVR_SysCmdReq */
#define BRCM_SWARCH_ETHXCVR_CMDHANDLER_PROC            (0x43U) /**< @brief #ETHXCVR_CmdHandler */
#define BRCM_SWARCH_ETHXCVR_SYSCMDHANDLER_PROC         (0x44U) /**< @brief #ETHXCVR_SysCmdHandler */
#define BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_PROC        (0x45U) /**< @brief #ETHXCVR_LinkIRQHandler */
#define BRCM_SWARCH_ETHXCVR_LINKCHGINDHANDLER_PROC     (0x46U) /**< @brief #ETHXCVR_LinkChgIndHandler */
#define BRCM_SWARCH_ETHXCVR_LINKSTATECHANGEIND_PROC    (0x47U) /**< @brief #ETHXCVR_LinkStateChangeInd */
#define BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER0_PROC       (0x48U) /**< @brief #ETHXCVR_LinkIRQHandler0 */
#define BRCM_SWARCH_ETHXCVR_PHYCACHEDINFO_TYPE         (0x49U) /**< @brief #ETHXCVR_PhyCachedInfoType */
#define BRCM_SWARCH_ETHXCVR_PHYCNTXT_TYPE              (0x4AU) /**< @brief #ETHXCVR_PhyCntxtType */
#define BRCM_SWARCH_ETHXCVR_MODEINDICATION_PROC        (0x4BU) /**< @brief #ETHXCVR_ModeIndication */
/** @} */

/** @name ETHXCVR_CmdType
    @{
    @brief Ethernet transceiver driver system command request type

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint32_t ETHXCVR_CmdType;
#define ETHXCVR_CMD_INIT                 (0UL)
#define ETHXCVR_CMD_SET_MODE             (1UL)
#define ETHXCVR_CMD_GET_MODE             (2UL)
#define ETHXCVR_CMD_GET_LINK_STATE       (3UL)
#define ETHXCVR_CMD_GET_SQI_VALUE        (4UL)
#define ETHXCVR_CMD_DEINIT               (5UL)
#define ETHXCVR_CMD_SET_MASTERMODE       (6UL)
#define ETHXCVR_CMD_GET_MASTERMODE       (7UL)
#define ETHXCVR_CMD_SET_SPEED            (8UL)
#define ETHXCVR_CMD_SET_LOOPBACK         (9UL)
#define ETHXCVR_CMD_GET_LOOPBACK         (10UL)
#define ETHXCVR_CMD_SET_JUMBO_MODE       (11UL)
#define ETHXCVR_CMD_GET_JUMBO_MODE       (12UL)
#define ETHXCVR_CMD_SET_AUTONEG_MODE     (13UL)
#define ETHXCVR_CMD_GET_AUTONEG_STATUS   (14UL)
#define ETHXCVR_CMD_GET_STATS            (15UL)
#define ETHXCVR_CMD_GET_SPEED            (16UL)
#define ETHXCVR_CMD_GET_DUPLEXMODE       (17UL)
#define ETHXCVR_CMD_SET_DUPLEXMODE       (18UL)
#define ETHXCVR_CMD_GET_FLOWCONTROL      (19UL)
#define ETHXCVR_CMD_SET_FLOWCONTROL      (20UL)
#define ETHXCVR_CMD_RESTART_AUTONEG      (21UL)
#define ETHXCVR_CMD_STATEHANDLER         (22UL)
#define ETHXCVR_CMD_UPDATE_HW_STATUS     (23UL)
#define ETHXCVR_CMD_LINK_CHGIND_HANDLER  (24UL)
/** @} */


/**
    @brief IO structure for Ethernet driver

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_IOType {
    int32_t                      retVal;        /**< @brief Error value as set by #ETHXCVR_SysCmdHandler */
    ETHXCVR_IDType               hwID;          /**< @brief Ethernet transceiver Index */
    ETHXCVR_ModeType             mode;          /**< @brief Transceiver mode */
    ETHXCVR_BooleanType          paramEnable;   /**< @brief Overloaded field; Jumbo/Autoneg/Master/Loopback enable */
    ETHXCVR_SpeedType            speed;         /**< @brief Transceiver speed */
    ETHXCVR_AutoNegStatusType    autoNegStatus; /**< @brief Transceiver auto-neg status */
    ETHXCVR_LinkStateType        linkState;     /**< @brief Transceiver link state */
    uint32_t                     isLinkStateChanged; /**< @brief link state changed flag */
    const ETHXCVR_PortConfigType *xcvrCfg;      /**< @brief Pointer to transceiver config */
    ETHXCVR_StatsType            *stats;        /**< @brief Pointer to transceiver statistics */
    uint32_t                     sqiValue;      /**< @brief Signal Quality Indicator */
    ETHXCVR_DuplexModeType       duplex;        /**< @brief Duplex */
    ETHXCVR_FlowControlType      flowControl;   /**< @brief Flow control */
    uint32_t                     isModeChanged; /**< @brief Flag indicating if mode was changed */
} ETHXCVR_IOType;

/**
    @brief PHY cached info structure type

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_PhyCachedInfoType {
    ETHXCVR_LinkStateType       linkState;      /**< @brief PHY Link state */
    ETHXCVR_ModeType            mode;           /**< @brief PHY Mode */
    ETHXCVR_BooleanType         connMode;       /**< @brief PHY connection mode */
    ETHXCVR_BooleanType         loopbackMode;   /**< @brief PHY loopback mode */
    ETHXCVR_BooleanType         jumboMode;      /**< @brief PHY jumbo mode */
    ETHXCVR_SpeedType           speed;          /**< @brief PHY speed */
    ETHXCVR_DuplexModeType      duplexMode;     /**< @brief PHY duplex mode */
    ETHXCVR_FlowControlType     flowControl;    /**< @brief PHY flow control */
    uint32_t                    sqiValue;       /**< @brief PHY SQI value */
} ETHXCVR_PhyCachedInfoType;

/**
    @brief PHY context structure type

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_PhyCntxtType {
    ETHXCVR_IDType              id;         /**< @brief PHY identifier */
    uint32_t                    data[16UL]; /**< @brief PHY context data */
    ETHXCVR_PhyCachedInfoType   cacheInfo;  /**< @brief PHY cached information */
} ETHXCVR_PhyCntxtType;

/** @brief Interface to raise a request to Ethernet driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned by #ETHXCVR_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
extern int32_t ETHXCVR_SysCmdReq(ETHXCVR_CmdType aCmd,
                                 ETHXCVR_IOType *const aIO);

/** @brief Internal command handler

    This interface handles commands requested from system command handler.
    This interface shall be called from #ETHXCVR_SysCmdHandler

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request completed successfully
    @retval     Others                  API dependent error values
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     #BCM_ERR_UNKNOWN        aADCIO is NULL

    @post None

    @note To be used.

    @limitations None

    @snippet ethxcvr_osil.c Usage of ETHXCVR_CmdHandler

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern int32_t ETHXCVR_CmdHandler(ETHXCVR_CmdType aCmd, ETHXCVR_IOType * const aIO);

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

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern void ETHXCVR_SysCmdHandler(uint32_t aMagicID,
                                uint32_t aCmd,
                                uint8_t * aSysIO);

/** @brief Ethernet Xcvr Link IRQ handler

    This API handles Ethernet xcvr link IRQ processing.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Xcvr ID

    @return     void

    @post None

    @note To be used.

    @limitations None

    @snippet eth_osil.c Usage of ETHXCVR_LinkIRQHandler

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
extern void ETHXCVR_LinkIRQHandler(ETHXCVR_IDType aID);

/** @brief Ethernet Xcvr change indication handler

    This API handles Ethernet xcvr link change indication processing.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID             Xcvr ID

    @return     void

    @post None

    @note To be used.

    @limitations None

    @snippet comms_osil.c Usage of ETHXCVR_LinkChgIndHandler

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern void ETHXCVR_LinkChgIndHandler(ETHXCVR_IDType aID);


/** @brief Ethernet transceiver link state indication

    Link state change indication callback from Transceiver during link state transition (from down to active or from
    active to down).

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[in]      aLinkState  Transceiver current link state

    @return     void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern void ETHXCVR_LinkStateChangeInd(ETHXCVR_IDType aID,
                                       ETHXCVR_LinkStateType aLinkState);

/** @brief Mode change indication

    Callback from transceiver to indicate change in mode. Triggered by a
    #ETHXCVR_SetMode invocation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Mode of the transceiver

    @return     void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern void ETHXCVR_ModeIndication(ETHXCVR_IDType aID, ETHXCVR_ModeType aMode);

#endif /* ETHXCVR_OSIL_H */

/** @} */
