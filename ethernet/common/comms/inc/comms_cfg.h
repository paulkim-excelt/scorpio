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
    @defgroup grp_comms_cfg_ifc Communication Subsystem Configuration
    @ingroup grp_comms_sw

    @addtogroup grp_comms_cfg_ifc
    @{

    @limitations None

    @file comms_cfg.h
    @brief Interface for Communication Subsystem
    This header file contains the interfaces for Communication Subsystem
    @version 0.1 Initial Version
 */

#ifndef COMMS_CFG_H
#define COMMS_CFG_H

#include <compiler.h>
#include <eth_switch.h>
#include "comms_stream.h"

/**
    @name Communication Subsystem API IDs
    @{
    @brief API IDs for Communication Subsystem
*/
#define BRCM_SWARCH_COMMS_CONFIG_MAGIC_ID_MACRO            (0x01U) /**< @brief #COMMS_CONFIG_MAGIC_ID */
#define BRCM_SWARCH_COMMS_MAX_PORTS_MACRO                  (0x02U) /**< @brief #COMMS_MAX_PORTS */
#define BRCM_SWARCH_COMMS_MAX_ARL_ENTRIES_MACRO            (0x03U) /**< @brief #COMMS_MAX_ARL_ENTRIES */
#define BRCM_SWARCH_COMMS_MAX_VLAN_ENTRIES_MACRO           (0x04U) /**< @brief #COMMS_MAX_VLAN_ENTRIES */
#define BRCM_SWARCH_COMMS_SWITCHPORTCONFIG_TYPE            (0x05U) /**< @brief #COMMS_SwitchPortConfigType */
#define BRCM_SWARCH_COMMS_PORTCONFIG_TYPE                  (0x06U) /**< @brief #COMMS_PortConfigType */
#define BRCM_SWARCH_COMMS_CONFIG_TYPE                      (0x07U) /**< @brief #COMMS_ConfigType */
#define BRCM_SWARCH_COMMS_ARLENTRY_TYPE                    (0x08U) /**< @brief #COMMS_ARLEntryType */
#define BRCM_SWARCH_COMMS_VLANENTRY_TYPE                   (0x09U) /**< @brief #COMMS_VLANEntryType */
/** @} */

/**
    @brief Magic number for the configuration structure, containing 'COMM'
*/
#define COMMS_CONFIG_MAGIC_ID                        (0x4D4D4F43UL)

/**
    @brief Maximum number of ports supported by Communication Subsystem
*/
#define COMMS_MAX_PORTS                               (16UL)

/**
    @brief Maximum number of ARL entries supported by Communication Subsystem
*/
#define COMMS_MAX_ARL_ENTRIES                         (512UL)

/**
    @brief Maximum number of VLANS supported by Communication Subsystem
*/
#define COMMS_MAX_VLAN_ENTRIES                        (512UL)

/**
    @brief Switch Port configuration
*/
typedef struct COMP_PACKED _COMMS_SwitchPortConfigType {
    ETHERSWT_PortType role;                 /**< @brief Port role */
    uint32_t          enableTimeStamp;      /**< @brief Enable or disable timestamp support*/
    uint32_t          enableJumboFrm;       /**< @brief Enable jumbo frames support on this
                                                        port */
    ETHERSWT_PortIngressCfgType ingressCfg; /**< @brief Port ingress configuration */
    ETHERSWT_PortEgressCfgType egressCfg;   /**< @brief Port egress configuration */
} COMMS_SwitchPortConfigType;

/**
    @brief Port configuration
*/
typedef struct COMP_PACKED _COMMS_PortConfigType {
    uint32_t                   portID;        /**< @brief Port Index */
    COMMS_SwitchPortConfigType switchPortCfg; /**< @brief Switch port configuration */
} COMMS_PortConfigType;

/* @brief ARL entry */
typedef struct COMP_PACKED _COMMS_ARLEntryType {
    uint8_t  macAddress[6U]; /**< @brief MAC address */
    uint16_t vlan;           /**< @brief VLAN Identifier */
    uint16_t portMask;       /**< @brief Bitmap of destination ports */
} COMMS_ARLEntryType;

/* @brief VLAN entry */
typedef struct COMP_PACKED _COMMS_VLANEntryType {
    uint16_t vlan;      /**< @brief VLAN Identifier */
    uint16_t portMask;  /**< @brief Bitmap of participating ports */
    uint16_t tagMask;   /**< @brief Bitmap of participating ports
                                    where egress packets are tagged */
} COMMS_VLANEntryType;

/**
    @brief Switch configuration
*/
typedef struct COMP_PACKED _COMMS_ConfigType {
    uint32_t             magicId;                         /**< @brief Magic number 'COMM'
                                                                (checked for validity) */
    COMMS_PortConfigType portCfgList[COMMS_MAX_PORTS];    /**< @brief Port configuration list */
    uint32_t             portCfgListSz;                   /**< @brief Number of ports */
    ETHERSWT_SwitchType  switchType;                      /**< @brief Type of switch w.r.t to operation */
    COMMS_VLANEntryType  vidTable[COMMS_MAX_VLAN_ENTRIES];/**< @brief VLAN table */
    uint32_t             vidTblSz;                        /**< @brief Number of entries in VLAN table */
    COMMS_ARLEntryType   arlTable[COMMS_MAX_ARL_ENTRIES]; /**< @brief ARL table */
    uint32_t             arlTblSz;                        /**< @brief Number of entries in ARL table */
    COMMS_StreamPolicerConfigType streamPolicers[COMMS_MAX_STREAM_POLICER_ENTRIES]; /**< @brief Stream policers */
    uint32_t                numStreamPolicers;               /**< @brief Number of stream policers */
} COMMS_ConfigType;

#endif /* COMMS_CFG_H */

/** @} */
