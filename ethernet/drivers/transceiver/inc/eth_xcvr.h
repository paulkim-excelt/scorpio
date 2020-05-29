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

/**
    @defgroup grp_eth_xcvrdrv_ifc Ethernet Transceiver Interface
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_ifc
    @{

    @file eth_xcvr.h
    @brief Ethernet Transceiver interface
    This header file contains the interface functions for Ethernet Transceiver

    @version 1.26 Imported from docx
*/

#ifndef ETH_XCVR_H
#define ETH_XCVR_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>

/**
    @name Ethernet Transceiver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_SWARCH_ETHXCVR_MAX_PORTS_MACRO           (0x01U) /**< @brief #ETHXCVR_MAX_PORTS */
#define BRCM_SWARCH_ETHXCVR_MAX_DRIVER_PARAMS_MACRO   (0x02U) /**< @brief #ETHXCVR_MAX_DRIVER_PARAMS */
#define BRCM_SWARCH_ETHXCVR_CONFIG_MAGIC_MACRO        (0x03U) /**< @brief #ETHXCVR_CONFIG_MAGIC */
#define BRCM_SWARCH_ETHXCVR_ID_TYPE                   (0x04U) /**< @brief #ETHXCVR_IDType */
#define BRCM_SWARCH_ETHXCVR_MODE_TYPE                 (0x05U) /**< @brief #ETHXCVR_ModeType */
#define BRCM_SWARCH_ETHXCVR_LINKSTATE_TYPE            (0x06U) /**< @brief #ETHXCVR_LinkStateType */
#define BRCM_SWARCH_ETHXCVR_SPEED_TYPE                (0x07U) /**< @brief #ETHXCVR_SpeedType */
#define BRCM_SWARCH_ETHXCVR_DUPLEXMODE_TYPE           (0x08U) /**< @brief #ETHXCVR_DuplexModeType */
#define BRCM_SWARCH_ETHXCVR_FLOWCONTROL_TYPE          (0x09U) /**< @brief #ETHXCVR_FlowControlType */
#define BRCM_SWARCH_ETHXCVR_ACCESSMODE_TYPE           (0x0BU) /**< @brief #ETHXCVR_AccessModeType */
#define BRCM_SWARCH_ETHXCVR_BOOLEAN_TYPE              (0x0DU) /**< @brief #ETHXCVR_BooleanType */
#define BRCM_SWARCH_ETHXCVR_AUTONEGSTATUS_TYPE        (0x0EU) /**< @brief #ETHXCVR_AutoNegStatusType */
#define BRCM_SWARCH_ETHXCVR_PHYMEDIA_TYPE             (0x0FU) /**< @brief #ETHXCVR_PhyMediaType */
#define BRCM_SWARCH_ETHXCVR_BUSMODE_TYPE              (0x10U) /**< @brief #ETHXCVR_BusModeType */
#define BRCM_SWARCH_ETHXCVR_STATS_TYPE                (0x12U) /**< @brief #ETHXCVR_StatsType */
#define BRCM_SWARCH_ETHXCVR_KEYVALUEPAIR_TYPE         (0x13U) /**< @brief #ETHXCVR_KeyValuePairType */
#define BRCM_SWARCH_ETHXCVR_FUNCTBL_TYPE              (0x14U) /**< @brief #ETHXCVR_FuncTblType */
#define BRCM_SWARCH_ETHXCVR_BUSCONFIG_TYPE            (0x15U) /**< @brief #ETHXCVR_PhyConfigType */
#define BRCM_SWARCH_ETHXCVR_PHYCONFIG_TYPE            (0x16U) /**< @brief #ETHXCVR_BusConfigType */
#define BRCM_SWARCH_ETHXCVR_PORTCONFIG_TYPE           (0x17U) /**< @brief #ETHXCVR_PortConfigType */
#define BRCM_SWARCH_ETHXCVR_CONFIG_TYPE               (0x18U) /**< @brief #ETHXCVR_ConfigType */
#define BRCM_SWARCH_ETHXCVR_INIT_TYPE                 (0x19U) /**< @brief #ETHXCVR_InitType */
#define BRCM_SWARCH_ETHXCVR_RESET_TYPE                (0x1AU) /**< @brief #ETHXCVR_ResetType */
#define BRCM_SWARCH_ETHXCVR_SETMODE_TYPE              (0x1BU) /**< @brief #ETHXCVR_SetModeType */
#define BRCM_SWARCH_ETHXCVR_GETMODE_TYPE              (0x1CU) /**< @brief #ETHXCVR_GetModeType */
#define BRCM_SWARCH_ETHXCVR_SETMASTERMODE_TYPE        (0x1DU) /**< @brief #ETHXCVR_SetMasterModeType */
#define BRCM_SWARCH_ETHXCVR_GETMASTERMODE_TYPE        (0x1EU) /**< @brief #ETHXCVR_GetMasterModeType */
#define BRCM_SWARCH_ETHXCVR_GETSPEED_TYPE             (0x1FU) /**< @brief #ETHXCVR_GetSpeedType */
#define BRCM_SWARCH_ETHXCVR_SETSPEED_TYPE             (0x20U) /**< @brief #ETHXCVR_SetSpeedType */
#define BRCM_SWARCH_ETHXCVR_GETDUPLEXMODE_TYPE        (0x21U) /**< @brief #ETHXCVR_GetDuplexModeType */
#define BRCM_SWARCH_ETHXCVR_SETDUPLEXMODE_TYPE        (0x22U) /**< @brief #ETHXCVR_SetDuplexModeType */
#define BRCM_SWARCH_ETHXCVR_GETFLOWCONTROL_TYPE       (0x23U) /**< @brief #ETHXCVR_GetFlowControlType */
#define BRCM_SWARCH_ETHXCVR_SETFLOWCONTROL_TYPE       (0x24U) /**< @brief #ETHXCVR_SetFlowControlType */
#define BRCM_SWARCH_ETHXCVR_GETLINKSTATE_TYPE         (0x25U) /**< @brief #ETHXCVR_GetLinkStateType */
#define BRCM_SWARCH_ETHXCVR_GETSQIVALUE_TYPE          (0x26U) /**< @brief #ETHXCVR_GetSQIValueType */
#define BRCM_SWARCH_ETHXCVR_LINKIRQHANDLER_TYPE       (0x27U) /**< @brief #ETHXCVR_LinkIRQHandlerType */
#define BRCM_SWARCH_ETHXCVR_LINKCHANGEINDHANDLER_TYPE (0x28U) /**< @brief #ETHXCVR_LinkChangeIndHandlerType */
#define BRCM_SWARCH_ETHXCVR_SETLOOPBACKMODE_TYPE      (0x29U) /**< @brief #ETHXCVR_SetLoopbackModeType */
#define BRCM_SWARCH_ETHXCVR_GETLOOPBACKMODE_TYPE      (0x2AU) /**< @brief #ETHXCVR_GetLoopbackModeType */
#define BRCM_SWARCH_ETHXCVR_SETJUMBOMODE_TYPE         (0x2BU) /**< @brief #ETHXCVR_SetJumboModeType */
#define BRCM_SWARCH_ETHXCVR_GETJUMBOMODE_TYPE         (0x2CU) /**< @brief #ETHXCVR_GetJumboModeType */
#define BRCM_SWARCH_ETHXCVR_SETAUTONEGMODE_TYPE       (0x2DU) /**< @brief #ETHXCVR_SetAutoNegModeType */
#define BRCM_SWARCH_ETHXCVR_GETAUTONEGSTATUS_TYPE     (0x2EU) /**< @brief #ETHXCVR_GetAutoNegStatusType */
#define BRCM_SWARCH_ETHXCVR_RESTARTAUTONEG_TYPE       (0x2FU) /**< @brief #ETHXCVR_RestartAutoNegType */
#define BRCM_SWARCH_ETHXCVR_GETSTATS_TYPE             (0x30U) /**< @brief #ETHXCVR_GetStatsType */
#define BRCM_SWARCH_ETHXCVR_STATEHANDLER_TYPE         (0x31U) /**< @brief #ETHXCVR_StateHandlerType */
#define BRCM_SWARCH_ETHXCVR_UPDATEHWSTATUS_TYPE       (0x32U) /**< @brief #ETHXCVR_UpdateHWStatusType */
#define BRCM_SWARCH_ETHXCVR_DEINIT_TYPE               (0x33U) /**< @brief #ETHXCVR_DeInitType */
#define BRCM_SWARCH_ETHXCVR_INIT_PROC                 (0x34U) /**< @brief #ETHXCVR_Init */
#define BRCM_SWARCH_ETHXCVR_DEINIT_PROC               (0x35U) /**< @brief #ETHXCVR_DeInit */
#define BRCM_SWARCH_ETHXCVR_SETMODE_PROC              (0x36U) /**< @brief #ETHXCVR_SetMode */
#define BRCM_SWARCH_ETHXCVR_GETMODE_PROC              (0x37U) /**< @brief #ETHXCVR_GetMode */
#define BRCM_SWARCH_ETHXCVR_SETMASTERMODE_PROC        (0x38U) /**< @brief #ETHXCVR_SetMasterMode */
#define BRCM_SWARCH_ETHXCVR_GETMASTERMODE_PROC        (0x39U) /**< @brief #ETHXCVR_GetMasterMode */
#define BRCM_SWARCH_ETHXCVR_GETLINKSTATE_PROC         (0x3AU) /**< @brief #ETHXCVR_GetLinkState */
#define BRCM_SWARCH_ETHXCVR_GETSPEED_PROC             (0x3BU) /**< @brief #ETHXCVR_GetSpeed */
#define BRCM_SWARCH_ETHXCVR_SETSPEED_PROC             (0x3CU) /**< @brief #ETHXCVR_SetSpeed */
#define BRCM_SWARCH_ETHXCVR_GETDUPLEXMODE_PROC        (0x3DU) /**< @brief #ETHXCVR_GetDuplexMode */
#define BRCM_SWARCH_ETHXCVR_SETDUPLEXMODE_PROC        (0x3EU) /**< @brief #ETHXCVR_SetDuplexMode */
#define BRCM_SWARCH_ETHXCVR_GETFLOWCONTROL_PROC       (0x3FU) /**< @brief #ETHXCVR_GetFlowControl */
#define BRCM_SWARCH_ETHXCVR_SETFLOWCONTROL_PROC       (0x40U) /**< @brief #ETHXCVR_SetFlowControl */
#define BRCM_SWARCH_ETHXCVR_GETSQIVALUE_PROC          (0x41U) /**< @brief #ETHXCVR_GetSQIValue */
#define BRCM_SWARCH_ETHXCVR_SETLOOPBACKMODE_PROC      (0x42U) /**< @brief #ETHXCVR_SetLoopbackMode */
#define BRCM_SWARCH_ETHXCVR_GETLOOPBACKMODE_PROC      (0x43U) /**< @brief #ETHXCVR_GetLoopbackMode */
#define BRCM_SWARCH_ETHXCVR_SETJUMBOMODE_PROC         (0x44U) /**< @brief #ETHXCVR_SetJumboMode */
#define BRCM_SWARCH_ETHXCVR_GETJUMBOMODE_PROC         (0x45U) /**< @brief #ETHXCVR_GetJumboMode */
#define BRCM_SWARCH_ETHXCVR_SETAUTONEGMODE_PROC       (0x46U) /**< @brief #ETHXCVR_SetAutoNegMode */
#define BRCM_SWARCH_ETHXCVR_GETAUTONEGSTATUS_PROC     (0x47U) /**< @brief #ETHXCVR_GetAutoNegStatus */
#define BRCM_SWARCH_ETHXCVR_RESTARTAUTONEG_PROC       (0x48U) /**< @brief #ETHXCVR_RestartAutoNeg */
#define BRCM_SWARCH_ETHXCVR_GETSTATS_PROC             (0x49U) /**< @brief #ETHXCVR_GetStats */
#define BRCM_SWARCH_ETHXCVR_STATEHANDLER_PROC         (0x4AU) /**< @brief #ETHXCVR_StateHandler */
#define BRCM_SWARCH_ETHXCVR_UPDATEHWSTATUS_PROC       (0x4BU) /**< @brief #ETHXCVR_UpdateHWStatus */
#define BRCM_SWARCH_ETHXCVR_STATE_TYPE                (0x4CU) /**< @brief #ETHXCVR_StateType */
#define BRCM_SWARCH_ETHXCVR_GETPORTCONFIG_PROC        (0x4DU) /**< @brief #ETHXCVR_GetPortConfig */
/** @} */

/**
    @brief Number of ports supported

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
#define ETHXCVR_MAX_PORTS          (16U)

/**
    @brief Number of driver parameters

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
#define ETHXCVR_MAX_DRIVER_PARAMS  (4U)

/**
    @brief Magic number for XCVR configuration

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
#define ETHXCVR_CONFIG_MAGIC   (0x52564358UL)

/**
    @brief Ethernet transceiver ID

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_IDType;

/**
    @name ETHXCVR_BusModeType
    @brief Mode of the data bus connecting the MAC and the PHY. It could be
           an MII like bus or an integrated PHY where the bus is not exposed.
    @{
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_BusModeType;       /**< @brief ETHXCVR_BusModeType typedef */
#define ETHXCVR_BUSMODE_INTG          (1U) /**< @brief Integrated PHY */
#define ETHXCVR_BUSMODE_RGMII         (2U) /**< @brief RGMII bus */
#define ETHXCVR_BUSMODE_RVMII         (3U) /**< @brief RVMII bus */
#define ETHXCVR_BUSMODE_SGMII         (4U) /**< @brief SGMII bus */
#define ETHXCVR_BUSMODE_RMII          (5U) /**< @brief RMII bus */
#define ETHXCVR_BUSMODE_MII           (6U) /**< @brief MII bus */
#define ETHXCVR_BUSMODE_PCIE          (7U) /**< @brief PCIE bus */
/** @} */

/**
    @name ETHXCVR_StateType
    @{
    @brief Ethernet transceiver driver state

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_StateType;  /**< @brief ETHXCVR_State typedef */
#define ETHXCVR_STATE_UNINIT (1U)   /**< @brief Transceiver driver uninitialised */
#define ETHXCVR_STATE_INIT   (2U)   /**< @brief Transceiver driver initialialised */
/** @} */

/**
    @name ETHXCVR_ModeType
    @{
    @brief Ethernet transceiver mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_ModeType; /**< @brief ETHXCVR_Mode typedef */
#define ETHXCVR_MODE_DOWN    (1U) /**< @brief Transceiver down */
#define ETHXCVR_MODE_ACTIVE  (2U) /**< @brief Transceiver active */
/** @} */

/**
    @name ETHXCVR_LinkStateType
    @{
    @brief Transceiver link status

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_LinkStateType;    /**< @brief ETHXCVR_LinkState typedef */
#define ETHXCVR_LINKSTATE_DOWN      (1U)  /**< @brief Link down */
#define ETHXCVR_LINKSTATE_ACTIVE    (2U)  /**< @brief Link up */
/** @} */

/**
    @name ETHXCVR_SpeedType
    @{
    @brief Ethernet transceiver speed

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_SpeedType;       /**< @brief ETHXCVR_Speed typedef */
#define ETHXCVR_SPEED_10MBPS     (0x01U) /**< @brief 10Mbps */
#define ETHXCVR_SPEED_100MBPS    (0x02U) /**< @brief 100Mbps */
#define ETHXCVR_SPEED_1000MBPS   (0x04U) /**< @brief 1000Mbps */
#define ETHXCVR_SPEED_2500MBPS   (0x08U) /**< @brief 2500Mbps */
#define ETHXCVR_SPEED_5000MBPS   (0x10U) /**< @brief 5000Mbps */
#define ETHXCVR_SPEED_VALID_MASK (0x1FU) /**< @brief Mask indicating valid bits */
/** @} */

/**
    @name ETHXCVR_DuplexModeType
    @{
    @brief Transceiver duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_DuplexModeType; /**< @brief ETHXCVR_DuplexMode typedef */
#define ETHXCVR_DUPLEXMODE_HALF (0x1U)  /**< @brief Half duplex */
#define ETHXCVR_DUPLEXMODE_FULL (0x2U)  /**< @brief Full duplex */
/** @} */

/**
    @name ETHXCVR_FlowControlType
    @brief Transceiver Flow control
    This will be the preferred flow control when auto-negotiation is enabled
    @{
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_FlowControlType;
#define ETHXCVR_FLOWCONTROL_NONE   (0U) /**< @brief No pause */
#define ETHXCVR_FLOWCONTROL_TXONLY (1U) /**< @brief Transmit pause only */
#define ETHXCVR_FLOWCONTROL_TXRX   (2U) /**< @brief Both transmit and receive pause */
#define ETHXCVR_FLOWCONTROL_RXONLY (3U) /**< @brief Receive pause only */
/** @} */

/**
    @name Transceiver access mode
    @brief Access mode for the PHY. Bus is assumed to be integrated and hence
    memory mapped
    @{
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_AccessModeType; /**< @brief ETHXCVR_AccessModeType typedef */
#define ETHXCVR_ACCESSMODE_MMAP  (1U) /**< @brief Memory mapped access */
#define ETHXCVR_ACCESSMODE_MDIO  (2U) /**< @brief MDIO access */
/** @} */

/**
    @name ETHXCVR_PhyMediaType
    @brief Physical medium for connecting to external world
    @{
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_PhyMediaType;     /**< @brief ETHXCVR_PhyMediaType typedef */
#define ETHXCVR_PHYMEDIA_NONE        (0U) /**< @brief Invalid */
#define ETHXCVR_PHYMEDIA_10BASET1    (1U) /**< @brief 10Base-T1 */
#define ETHXCVR_PHYMEDIA_10BASET     (2U) /**< @brief 10Base-T */
#define ETHXCVR_PHYMEDIA_100BASET1   (3U) /**< @brief 100Base-T1 */
#define ETHXCVR_PHYMEDIA_100BASETX   (4U) /**< @brief 100Base-TX */
#define ETHXCVR_PHYMEDIA_1000BASET1  (5U) /**< @brief 1000Base-T1 */
#define ETHXCVR_PHYMEDIA_1000BASET   (6U) /**< @brief 1000Base-T */
#define ETHXCVR_PHYMEDIA_1000BASEX   (7U) /**< @brief 1000Base-X */
/** @} */

/**
    @name ETHXCVR_BooleanType
    @brief Enable/disable for a feature
    @{
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_BooleanType;
#define ETHXCVR_BOOLEAN_FALSE  (0U) /**< @brief False */
#define ETHXCVR_BOOLEAN_TRUE   (1U) /**< @brief True  */
/** @} */

/**
    @name ETHXCVR_AutoNegStatusType
    @{
    @brief Auto-negotiation status

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef uint8_t ETHXCVR_AutoNegStatusType;          /**< @brief ETHXCVR_AutoNegStatusType typedef */
#define ETHXCVR_AUTONEGSTATUS_COMPLETE      (0x1U) /**< @brief Auto-neg complete */
#define ETHXCVR_AUTONEGSTATUS_INCOMPLETE    (0x2U) /**< @brief Auto-neg incomplete */
#define ETHXCVR_AUTONEGSTATUS_NO_ABILITY    (0x3U) /**< @brief Auto-neg ability not present  */
/** @} */

/**
    @name ETHXCVR_StatsType
    @{
    @brief Transceiver statistics type

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
*/
typedef struct _ETHXCVR_StatsType {
    uint32_t linkStateChangeCount;  /**< @brief Link state change count */
} ETHXCVR_StatsType;
/** @} */

/**
    @name ETHXCVR_KeyValuePairType
    @{
    @brief Key value pair structure

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_KeyValuePairType {
    uint16_t    key;    /**< @brief Key */
    uint16_t    value;  /**< @brief Value */
} ETHXCVR_KeyValuePairType;
/** @} */

/**
    @name ETHXCVR_BusConfigType
    @{
    @brief Bus configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_BusConfigType {
    uint8_t                  cntrlID;           /**< @brief Controller index */
    uint8_t                  instID;            /**< @brief Instance index */
    uint8_t                  reserved[2U];      /**< @brief Reserved field */
    ETHXCVR_KeyValuePairType driverParams[ETHXCVR_MAX_DRIVER_PARAMS]; /**< @brief Driver parameters */
} ETHXCVR_BusConfigType;
/** @} */

/**
    @name ETHXCVR_PhyConfigType
    @{
    @brief Phy configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_PhyConfigType {
    uint8_t                  hwID;              /**< @brief Hardware index of the controller */
    uint8_t                  slaveID;           /**< @brief Slave address for PHY controled via MDIO */
    ETHXCVR_AccessModeType   accessMode;        /**< @brief Access Mode */
    ETHXCVR_PhyMediaType     phyMedia;          /**< @brief Physical media */
    ETHXCVR_BooleanType   masterMode;        /**< @brief Master/Slave Mode */
    uint8_t                  reserved[3U];      /**< @brief Reserved field */
    ETHXCVR_KeyValuePairType driverParams[ETHXCVR_MAX_DRIVER_PARAMS]; /**< @brief Driver parameters */
} ETHXCVR_PhyConfigType;
/** @} */

/**
    @brief Port configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_INTERFACE
 */
typedef struct _ETHXCVR_PortConfigType {
    ETHXCVR_IDType          id;          /**< @brief Port index. This is the chip port number */
    ETHXCVR_BooleanType     portEnable;  /**< @brief Flag indicating whether the port is enabled or not */
    ETHXCVR_SpeedType       speed;       /**< @brief Speed */
    ETHXCVR_BooleanType     autoNeg;     /**< @brief Autoneg enable/disable */
    ETHXCVR_DuplexModeType  duplex;      /**< @brief Duplex */
    ETHXCVR_FlowControlType flowControl; /**< @brief Flow Control */
    ETHXCVR_BooleanType     jumbo;       /**< @brief Jumbo enable/disable */
    ETHXCVR_BusModeType     busMode;     /**< @brief Bus mode */
    ETHXCVR_BusConfigType   bus;         /**< @brief Bus device */
    ETHXCVR_PhyConfigType   phy;         /**< @brief PHY device */
} ETHXCVR_PortConfigType;

/**
    @brief XCVR configuration structure

    @trace #BRCM_SWREQ_ETHXCVR_INIT
 */
typedef struct _ETHXCVR_ConfigType {
    uint32_t magic;               /**< @brief Magic number */
    uint32_t version;             /**< @brief BCM transceiver configuration structuire version */
    uint32_t numPorts;            /**< @brief Number of valid ports in the configuration. Should not be more
                                              than #ETHXCVR_MAX_PORTS */
    ETHXCVR_PortConfigType port[ETHXCVR_MAX_PORTS]; /**< @brief Port configuration */
    uint8_t name[16UL];           /**< @brief Configuration name */
    uint8_t file[128UL];          /**< @brief File name from which this configuration is generated */
} ETHXCVR_ConfigType;
/**
    @brief Function type to initialize

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_InitType)(uint8_t aId,
                                    const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief function type to de-initialize

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_DeInitType)(uint8_t aId,
                                      const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to reset

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_ResetType)(uint8_t aId,
                                     const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to set mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetModeType)(uint8_t aId,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_ModeType aMode);

/**
    @brief Function type to get mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetModeType)(uint8_t aId,
                                       const ETHXCVR_PortConfigType *const aConfig,
                                       ETHXCVR_ModeType *const aMode);

/**
    @brief Function type to set master/slave mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetMasterModeType)(uint8_t aId,
                                             const ETHXCVR_PortConfigType *const aConfig,
                                             ETHXCVR_BooleanType aMode);

/**
    @brief Function type to get master/slave mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetMasterModeType)(uint8_t aId,
                                             const ETHXCVR_PortConfigType *const aConfig,
                                             ETHXCVR_BooleanType *const aMode);

/**
    @brief Function type to get speed

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetSpeedType)(uint8_t aId,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_SpeedType *const aSpeed);

/**
    @brief Function type to set speed

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetSpeedType)(uint8_t aId,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_SpeedType aSpeed);

/**
    @brief Function type to get duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetDuplexModeType)(uint8_t aId,
                                             const ETHXCVR_PortConfigType *const aConfig,
                                             ETHXCVR_DuplexModeType *const aDuplexMode);

/**
    @brief Function type to set duplex mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetDuplexModeType)(uint8_t aId,
                                             const ETHXCVR_PortConfigType *const aConfig,
                                             ETHXCVR_DuplexModeType aDuplexMode);

/**
    @brief Function type to get flow control

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetFlowControlType)(uint8_t aId,
                                              const ETHXCVR_PortConfigType *const aConfig,
                                              ETHXCVR_FlowControlType *const aFlowControl);

/**
    @brief Function type to set flow control

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetFlowControlType)(uint8_t aId,
                                              const ETHXCVR_PortConfigType *const aConfig,
                                              ETHXCVR_FlowControlType aFlowControl);

/**
    @brief Function type to get link state

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetLinkStateType)(uint8_t aId,
                                            const ETHXCVR_PortConfigType *const aConfig,
                                            ETHXCVR_LinkStateType *const aLinkState);

/**
    @brief Function type to get SQI value

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetSQIValueType)(uint8_t aId,
                                           const ETHXCVR_PortConfigType *const aConfig,
                                           uint32_t *const aSQIValue);

/**
    @brief Function type to handle link IRQ

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_LinkIRQHandlerType)(uint8_t aId,
                                              const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to handle link change indication

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_LinkChangeIndHandlerType)(uint8_t aId,
                                                    const ETHXCVR_PortConfigType *const aConfig,
                                                    ETHXCVR_LinkStateType *const aLinkState,
                                                    uint32_t *const aIsLinkStateChanged);

/**
    @brief Function type to set loopback mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetLoopbackModeType)(ETHXCVR_IDType aID,
                                               const ETHXCVR_PortConfigType *const aConfig,
                                               ETHXCVR_BooleanType aMode);

/**
    @brief Function type to get loopback mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetLoopbackModeType)(ETHXCVR_IDType aID,
                                               const ETHXCVR_PortConfigType *const aConfig,
                                               ETHXCVR_BooleanType *const aMode);

/**
    @brief Function type to set jumbo mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetJumboModeType)(ETHXCVR_IDType aID,
                                            const ETHXCVR_PortConfigType *const aConfig,
                                            ETHXCVR_BooleanType aJumboMode);
/**
    @brief Function type to get jumbo mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetJumboModeType)(ETHXCVR_IDType aID,
                                            const ETHXCVR_PortConfigType *const aConfig,
                                            ETHXCVR_BooleanType *const aJumboMode);

/**
    @brief Function type to set auto-negotiation mode

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_SetAutoNegModeType)(ETHXCVR_IDType aID,
                                              const ETHXCVR_PortConfigType *const aConfig,
                                              ETHXCVR_BooleanType aAutoNegMode);

/**
    @brief Function type to get auto-negotiation status

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetAutoNegStatusType)(ETHXCVR_IDType aID,
                                                const ETHXCVR_PortConfigType *const aConfig,
                                                ETHXCVR_AutoNegStatusType *const aAutoNegStatus);

/**
    @brief Function type to restart auto-negotiation

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_RestartAutoNegType)(ETHXCVR_IDType aID,
                                              const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Function type to retrieve statistics

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_GetStatsType)(ETHXCVR_IDType aID,
                                        const ETHXCVR_PortConfigType *const aConfig,
                                        ETHXCVR_StatsType *const aStats);

/**
    @brief Function type for state handler

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_StateHandlerType)(ETHXCVR_IDType aID,
                                            const ETHXCVR_PortConfigType *const aConfig,
                                            uint32_t *const aIsModeChanged,
                                            ETHXCVR_ModeType *const aMode);

/**
    @brief Function type to update HW status parameters

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
typedef int32_t (*ETHXCVR_UpdateHWStatusType)(ETHXCVR_IDType aID,
                                              const ETHXCVR_PortConfigType *const aConfig);

/**
    @brief Ethernet Bus/PHY function table type

    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
 */
typedef struct _ETHXCVR_FuncTblType {
    ETHXCVR_InitType                 init;                 /**< @brief Function type to initialize */
    ETHXCVR_DeInitType               deinit;               /**< @brief Function type to de-initialize */
    ETHXCVR_ResetType                reset;                /**< @brief Function type to reset */
    ETHXCVR_SetModeType              setMode;              /**< @brief Function type to set mode */
    ETHXCVR_GetModeType              getMode;              /**< @brief Function type to get mode */
    ETHXCVR_SetMasterModeType        setMasterMode;        /**< @brief Function type to set master/slave mode */
    ETHXCVR_GetMasterModeType        getMasterMode;        /**< @brief Function type to get master/slave mode */
    ETHXCVR_GetSpeedType             getSpeed;             /**< @brief Function type to get speed */
    ETHXCVR_SetSpeedType             setSpeed;             /**< @brief Function type to set speed */
    ETHXCVR_SetDuplexModeType        setDuplexMode;        /**< @brief Function type to set duplex mode */
    ETHXCVR_GetDuplexModeType        getDuplexMode;        /**< @brief Function type to get duplex mode */
    ETHXCVR_SetFlowControlType       setFlowControl;       /**< @brief Function type to set flow control */
    ETHXCVR_GetFlowControlType       getFlowControl;       /**< @brief Function type to get flow control */
    ETHXCVR_GetLinkStateType         getLinkState;         /**< @brief Function type to get link state */
    ETHXCVR_GetSQIValueType          getSQIValue;          /**< @brief Function type to get SQI value */
    ETHXCVR_SetLoopbackModeType      setLoopbackMode;      /**< @brief Function type to set loopback mode */
    ETHXCVR_GetLoopbackModeType      getLoopbackMode;      /**< @brief Function type to get loopback mode */
    ETHXCVR_SetJumboModeType         setJumboMode;         /**< @brief Function type to set jumbo mode */
    ETHXCVR_GetJumboModeType         getJumboMode;         /**< @brief Function type to get jumbo mode */
    ETHXCVR_SetAutoNegModeType       setAutoNegMode;       /**< @brief Function type to set Auto-negotiation mode */
    ETHXCVR_GetAutoNegStatusType     getAutoNegStatus;     /**< @brief Function type to get Auto-negotiation status */
    ETHXCVR_RestartAutoNegType       restartAutoNeg;       /**< @brief Function type to restart Auto-negotiation */
    ETHXCVR_GetStatsType             getStats;             /**< @brief Function type to get statistics */
    ETHXCVR_StateHandlerType         stateHandler;         /**< @brief Function type for state handler */
    ETHXCVR_UpdateHWStatusType       updateHWStatus;       /**< @brief Function type to update HW status parameters */
    ETHXCVR_LinkIRQHandlerType       linkIRQHandler;       /**< @brief Function type to handle link IRQ */
    ETHXCVR_LinkChangeIndHandlerType linkChangeIndHandler; /**< @brief Function type to handle link change indication */
} ETHXCVR_FuncTblType;

/** @brief Initialize transceiver

    This API initializes the transceiver with the given configuration.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[in]      aConfig     Pointer to transceiver configuration

    @return     void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
extern void ETHXCVR_Init(ETHXCVR_IDType aID,
                         const ETHXCVR_PortConfigType *const aConfig);

/** @brief De-initialize transceiver

    This API de-initializes the transceiver driver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the Transceiver

    @return     void

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_INIT
*/
extern void ETHXCVR_DeInit(ETHXCVR_IDType aID);

/** @brief Set transceiver mode

    This API sets mode of the transceiver. If aMode is #ETHXCVR_MODE_DOWN, transceiver is disabled by driver
    (both RX/TX disabled). If aMode is #ETHXCVR_MODE_ACTIVE, transceiver is enabled by the driver (both RX/TX active)

    @behavior ASync, Non Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_EAGAIN         Transceiver mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType aMode);

/** @brief Get transceiver mode

    This API retrieves the current mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMode       Pointer to Mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetMode(ETHXCVR_IDType aID,
                               ETHXCVR_ModeType *const aMode);

/** @brief Set transceiver master/slave mode

    This API sets master/slave mode of the transceiver. If aMode is
    #ETHXCVR_MASTER_MODE_EN, transceiver is configured in master
    mode. If aMode is #ETHXCVR_MASTER_MODE_DIS, transceiver is
    configured by driver in slave mode.

    @note Transceiver master/slave mode might not be applicable to all
    type of transceiver. If the mode is not applicable, this
    API shall return #BCM_ERR_NOSUPPORT


    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMasterMode Master/slave mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Master/slave mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting master/slave mode is not supported
    @retval     #BCM_ERR_EAGAIN         Master mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType aMasterMode);

/** @brief Get transceiver master/slave mode

    This API retrieves the current master/slave mode of the Ethernet transceiver.

    @note Transceiver master/slave mode might not be applicable to all
    types of transceivers. If the mode is not applicable, this
    API shall return #BCM_ERR_NOSUPPORT

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMasterMode Pointer to retrieve master/slave mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Master/slave mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Master/slave mode is not supported

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetMasterMode(ETHXCVR_IDType aID,
                                     ETHXCVR_BooleanType *const aMasterMode);

/** @brief Get transceiver link state

    This API retrieves the state of the Ethernet transceiver link.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aLinkState  Pointer to link state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved transceiver link state
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aLinkState is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetLinkState(ETHXCVR_IDType aID,
                                    ETHXCVR_LinkStateType *const aLinkState);

/** @brief Get transceiver speed

    This API retrieves the current speed of the Ethernet transceiver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aSpeed      Speed of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved transceiver speed
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType *const aSpeed);

/** @brief Set transceiver speed

    This API set speed of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aSpeed      Speed of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver speed
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting speed is not supported
    @retval     #BCM_ERR_EAGAIN         Speed can not be changed at this
                                        moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetSpeed(ETHXCVR_IDType aID,
                                ETHXCVR_SpeedType aSpeed);

/** @brief Get duplex mode

    This API retrieves the current duplex mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aDuplexMode Duplex Mode of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved current duplex mode of the transceiver
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aDuplexMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType *const aDuplexMode);

/** @brief Set transceiver duplex mode

    This API set duplex mode of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aDuplexMode Duplex mode of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver duplex mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aDuplexMode is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting duplex mode is not supported
    @retval     #BCM_ERR_EAGAIN         Duplex mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetDuplexMode(ETHXCVR_IDType aID,
                                     ETHXCVR_DuplexModeType aDuplexMode);

/** @brief Get flow control

    This API retrieves the current flow control of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID          Index of the transceiver
    @param[out]     aFlowControl Flow control of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved current flow control of the transceiver
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aFlowControl is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetFlowControl(ETHXCVR_IDType aID,
                                     ETHXCVR_FlowControlType *const aFlowControl);

/** @brief Set transceiver flow control

    This API sets flow control of the Ethernet transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID          Index of the transceiver
    @param[in]      aFlowControl Flow control of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set transceiver flow control
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aFlowControl  is invalid
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting flow control is not supported
    @retval     #BCM_ERR_EAGAIN         Flow control mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetFlowControl(ETHXCVR_IDType aID,
                                      ETHXCVR_FlowControlType aFlowControl);

/** @brief Get SQI mode

    This API retrieves the SQI value of the established link

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aSQIValue   SQI value of the link

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully returned the SQI value of the link.
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aSQIValue is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetSQIValue(ETHXCVR_IDType aID,
                                   uint32_t *const aSQIValue);

/** @brief Set transceiver loopback mode

    This API sets loopback mode of the transceiver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Loopback mode to be configured.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Loopback mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid loopback mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting loopback mode is not supported
    @retval     #BCM_ERR_EAGAIN         Loopback mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_BooleanType aMode);

/** @brief Get transceiver loopback mode

    This API retrieves the current loopback mode of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aMode       Pointer to retrieve loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Loopback mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Loopback mode is not supported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetLoopbackMode(ETHXCVR_IDType aID,
                                       ETHXCVR_BooleanType *const aMode);

/** @brief Set transceiver jumbo mode

    This API sets jumbo mode of the transceiver.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Jumbo mode to be configured.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Jumbo mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid jumbo mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting jumbo mode is not supported
    @retval     #BCM_ERR_EAGAIN         Jumbo mode can not be set at this
                                        moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType aMode);

/** @brief Get transceiver jumbo mode

    This API retrieves the current jumbo packet mode enabled/disabled for
    the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aJumboMode  Pointer to retrieve jumbo mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Jumbo mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aJumboMode is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Jumbo packet mode is not supported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetJumboMode(ETHXCVR_IDType aID,
                                    ETHXCVR_BooleanType *const aJumboMode);

/** @brief Set auto-negotiation mode

    This API sets the auto-negotiation mode for the transceiver. In case
    aMode is #ETHXCVR_AUTONEGMODE_EN then it also performs auto-negotiation.

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[in]      aMode       Auto-negotiation enable/disable mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Auto-negotiation mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid auto-negotiation mode
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Setting auto-negotiation mode is not supported
    @retval     #BCM_ERR_EAGAIN         Auto-negotiation mode can not be set
                                        at this moment. Try again.

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_SetAutoNegMode(ETHXCVR_IDType aID,
                                      ETHXCVR_BooleanType aMode);

/** @brief Get transceiver auto-negotiation status

    This API retrieves auto-negotiation status of the Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Index of the transceiver
    @param[out]     aAutoNegStatus  Pointer to retrieve status of auto-negotiation

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver auto-negotiation status
                                        retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aAutoNegStatus is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Auto-negotiation status is not supported

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetAutoNegStatus(ETHXCVR_IDType aID,
                                        ETHXCVR_AutoNegStatusType *const aAutoNegStatus);

/** @brief Restart auto-negotiation

    This API restarts auto-negotiation for the transceiver

    @behavior Sync, Re-entrant across transceivers

    @pre None

    @param[in]      aID         Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Auto-negotiation restarted successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_STATE    Auto-negotiation is not enabled
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_NOSUPPORT      Restarting auto-negotiation is not supported

    @post None

    @limitations Set operation could result in hit in traffic on the device

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_RestartAutoNeg(ETHXCVR_IDType aID);

/** @brief Get transceiver statistics

    This API retrieves the statistics of Ethernet transceiver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aStats      Pointer to retrieve statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver statistics retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aStats is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetStats(ETHXCVR_IDType aID,
                                ETHXCVR_StatsType *const aStats);

/** @brief Transceiver state handler

    This API triggers the transceiver state machine

    @behavior Sync, Non re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver state machine triggered successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_BUSY           Transceiver driver state handler is
                                        busy processing previous state. Call
                                        state handler again.
    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_RUNTIME_CONFIG
*/
extern int32_t ETHXCVR_StateHandler(ETHXCVR_IDType aID);

/** @brief Update HW status parameters

    This API triggers the driver to read and cache the HW status parameters

    @behavior Async, Non re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transceiver driver triggered successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized
    @retval     #BCM_ERR_EAGAIN         Transceiver driver can't be triggered at
                                        this moment. Try again.

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_UpdateHWStatus(ETHXCVR_IDType aID);

/** @brief Get port config

    This API getches the port config

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aConfig     Pointer to copy the port config

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Config retrived successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid transceiver ID
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval     #BCM_ERR_UNINIT         Transceiver driver is not initialized

    @post None

    @limitations None

    @trace #BRCM_SWREQ_ETHXCVR_STATUS
*/
extern int32_t ETHXCVR_GetPortConfig(ETHXCVR_IDType aID, ETHXCVR_PortConfigType *const aConfig);

#endif /* ETH_XCVR_H */

/** @} */
