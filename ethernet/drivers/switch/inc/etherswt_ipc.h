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

#ifndef ETHERSWT_IPC_H
#define ETHERSWT_IPC_H

#include <stdint.h>
#include <stdlib.h>
#include <mcu.h>
#include <eth_switch.h>
#include <cfp.h>
#include <rpc_cmds.h>
#include <comms_stream.h>

/* Firmware - Switch command group */
typedef uint8_t ETHERSWT_CmdIdType;
#define ETHERSWT_CMDID_PORT_INFO_GET              (0x00U)
#define ETHERSWT_CMDID_PORT_INFO_SET              (0x01U)
#define ETHERSWT_CMDID_SWITCH_INFO_GET            (0x02U)
#define ETHERSWT_CMDID_SWITCH_INFO_SET            (0x03U)
#define ETHERSWT_CMDID_VLAN_GET                   (0x04U)
#define ETHERSWT_CMDID_VLAN_PORT_ADD              (0x05U)
#define ETHERSWT_CMDID_VLAN_PORT_REMOVE           (0x06U)
#define ETHERSWT_CMDID_MIRROR_ENABLE              (0x07U)
#define ETHERSWT_CMDID_MIRROR_DISABLE             (0x08U)
#define ETHERSWT_CMDID_MIRROR_STATUS              (0x09U)
#define ETHERSWT_CMDID_WRITE                      (0x0AU)
#define ETHERSWT_CMDID_READ                       (0x0BU)
#define ETHERSWT_CMDID_MIBS                       (0x0CU)
#define ETHERSWT_CMDID_MIBS_CLEAR                 (0x0DU)
#define ETHERSWT_CMDID_ARL_ADD                    (0x0EU)
#define ETHERSWT_CMDID_ARL_DELETE                 (0x0FU)
#define ETHERSWT_CMDID_ARL_SNAPSHOT               (0x10U)
#define ETHERSWT_CMDID_ARL_GET                    (0x11U)
#define ETHERSWT_CMDID_CFP_ADDRULE                (0x21U)
#define ETHERSWT_CMDID_CFP_REMOVERULE             (0x22U)
#define ETHERSWT_CMDID_CFP_UPDATERULE             (0x23U)
#define ETHERSWT_CMDID_CFP_GETSTATS               (0x24U)
#define ETHERSWT_CMDID_CFP_SETPORTMODE            (0x25U)
#define ETHERSWT_CMDID_CFP_GETROWCONFIG           (0x26U)
#define ETHERSWT_CMDID_CFP_GETSNAPSHOT            (0x27U)
#define ETHERSWT_CMDID_STREAMPOLICER_ADD          (0x31U)
#define ETHERSWT_CMDID_STREAMPOLICER_DEL          (0x32U)
#define ETHERSWT_CMDID_BLOCKSTREAM                (0x33U)
#define ETHERSWT_CMDID_RESUMESTREAM               (0x34U)
#define ETHERSWT_CMDID_STREAMPOLICER_FIND         (0x35U)
#define ETHERSWT_CMDID_STREAMPOLICER_GET          (0x36U)
#define ETHERSWT_CMDID_STREAMPOLICER_SNAPSHOT     (0x37U)

/* Switch notification event group */
typedef uint8_t EtherSwt_EventType;
#define ETHERSWT_EVENT_PORT_LINK_INFO  (0x00U)
#define ETHERSWT_EVENT_STREAM_EXCEEDED (0x01U)

/**
  @brief Maximum number of ARL entries that can be sent in one
  IPC response.
*/
#define ETHERSWT_ARL_ENTRIES_MAX (RPC_RESPPAYLOADSZ/sizeof(ETHERSWT_ARLEntryType))

/**
  @brief Structure to manage the port Mirroring.
*/
typedef struct _ETHERSWT_MirrorEnableType {
    ETHERSWT_PortIDType probePort;           /* probe port */
    ETHERSWT_TrafficDirType direction;       /* traffic direction */
    ETHERSWT_PortMirrorCfgType mirrorCfg;
} ETHERSWT_MirrorEnableType;

/**
  @brief Structure to manage the port Mirroring status.
*/
typedef struct _ETHERSWT_MirrorStatusType {
    ETHERSWT_PortMirrorStateType state;      /* enabled or disabled */
    ETHERSWT_PortIDType probePort;           /* probe port */
    ETHERSWT_PortMirrorCfgType ingressMirrorCfg; /* mirror configuration for ingress traffic */
    ETHERSWT_PortMirrorCfgType egressMirrorCfg; /* mirror configuration for egress traffic */
} ETHERSWT_MirrorStatusType;

/**
  @brief Structure to manage the switch register R/W access.
*/
typedef struct _ETHERSWT_RegAccessType{
    uint8_t        data[128UL];
    uint32_t       addr;
    uint32_t       len;
    MCU_DeviceType deviceID;
} ETHERSWT_RegAccessType;

/**
  @brief Structure to manage the port VLANs.
*/
typedef struct _ETHERSWT_VLANPortType {
    ETHERSWT_VLANIDType vlan;           /* VLAN ID */
    uint32_t            portMask;       /* Port mask */
    uint32_t            tagMask;        /* Tagged port mask */
    uint32_t            staticPortMask; /* Static port mask */
} ETHERSWT_VLANPortType;

/**
  @brief Structure to manage the ARL commands.
*/
typedef struct _ETHERSWT_ARLListType {
    uint32_t num;
    ETHERSWT_ARLEntryType entries[ETHERSWT_ARL_ENTRIES_MAX];
} ETHERSWT_ARLListType;

/**
  @brief Structure to manage the Rx/Tx statistics.
*/
typedef struct _ETHERSWT_MibType {
    ETHERSWT_PortIDType  port;
    ETHER_RxStatsType    rxStats;   /**< Receive stats */
    ETHER_TxStatsType    txStats;   /**< Transmit stats */
} ETHERSWT_MibType;

/**
  @brief Structure to manage the port parameters.
*/
typedef struct _ETHERSWT_PortInfoType {
    ETHERSWT_PortIDType        port;
    ETHXCVR_ModeType           adminMode;           /* Admin mode of the port - enabled/disabled */
    ETHXCVR_LinkStateType      linkStatus;          /* Link status of the port - up/down */
    ETHXCVR_SpeedType          speed;               /* Current speed of the port - 10/100/Autoneg */
    ETHXCVR_BooleanType        masterEnable;        /* BroadR-Reach mode - master/slave */
    ETHXCVR_BooleanType        jumboEnable;         /* Jumbo enable */
    ETHXCVR_BooleanType        loopbackEnable;      /* Loop back mode - enabled/disabled */
    ETHXCVR_BooleanType        autonegEnable;       /* Auto Neg - enabled/disabled */
    ETHXCVR_AutoNegStatusType  autonegComplete;     /* Auto Neg - complete/pending */
    ETHXCVR_DuplexModeType     duplex;              /* duplex - full/half */
    uint32_t                   led;                 /* led - on/off */
    uint32_t                   linkStateChangeCnt;  /* no. of state changes */
    ETHXCVR_BusModeType        busMode;             /* XCVR bus mode */
    ETHXCVR_PhyMediaType       phyMedia;            /* XCVR phy media */
    uint32_t                   linkSQI;
    uint32_t                   pvid;
    uint32_t                   prio;
} ETHERSWT_PortInfoType;

/**
  @brief Structure to manage the switch port link information type.
*/
typedef struct _ETHERSWT_PortLinkInfoType {
    ETHERSWT_PortIDType        port;                /* Port identifier */
    ETHXCVR_LinkStateType      linkState;           /* Link status of the port - up/down */
    ETHERSWT_PortStatsType     portStats;            /* Port stats */
} ETHERSWT_PortLinkInfoType;

/**
  @brief Structure to manage the switch get/set parameters.
*/
typedef struct _ETHERSWT_SwitchInfoType {
    uint32_t ageTime;
    ETHERSWT_DumbFwdModeType dumbFwdMode;
    ETHERSWT_VLANIngressFilterModeType iFilter;
} ETHERSWT_SwitchInfoType;

/**
  @brief Structure to manage the CFP statistics.
*/
typedef struct _ETHERSWT_CFPStatsType {
    uint32_t      row;
    CFP_StatsType stats;
} ETHERSWT_CFPStatsType;

/**
  @brief Structure to manage the CFP port mode.
*/
typedef struct _ETHERSWT_CFPPortModeType {
    uint32_t      port;
    uint32_t      mode;
} ETHERSWT_CFPPortModeType;

/**
  @brief Structure to manage stream policer
*/
typedef struct _ETHERSWT_StreamPolicerType {
    uint8_t                       macAddr[6];    /**< @brief MAC address */
    ETHERSWT_VLANIDType           vlanID;        /**< @brief VLAN ID */
    COMMS_StreamPolicerConfigType policerParams; /**< @brief Policer parameters */
    uint32_t                      streamIdx;     /**< @brief Stream index */
} ETHERSWT_StreamPolicerType;


/**
  @brief Union of all possible structures used for IPC communication.
*/
typedef union _ETHERSWT_PayloadType {
    uint8_t                         *u8Ptr;
    const uint8_t                   *u8ConstPtr;
    ETHERSWT_PortInfoType           *portInfo;
    ETHERSWT_SwitchInfoType         *switchInfo;
    ETHERSWT_VLANPortType           *vlanPort;
    ETHERSWT_MirrorEnableType       *mirrorEnable;
    ETHERSWT_MirrorStatusType       *mirrorStatus;
    ETHERSWT_RegAccessType          *regAccess;
    ETHERSWT_MibType                *mib;
    ETHERSWT_ARLEntryType           *arlEntry;
    ETHERSWT_ARLListType            *arlList;
    CFP_RuleType                    *cfpRule;
    CFP_TableSnapshotType           *cfpSnapshot;
    ETHERSWT_CFPStatsType           *cfpStats;
    ETHERSWT_CFPPortModeType        *cfpPortMode;
    ETHERSWT_PortLinkInfoType       *portLinkInfo;
    ETHERSWT_StreamPolicerType      *streamPolicerEntry;
    COMMS_StreamPolicerSnapshotType *streamPolicerSnapshot;
    COMMS_StreamPolicerStatusType   *streamPolicerStatus;
} ETHERSWT_PayloadType;

/**
    @brief Compile type checks for the payload size
*/
_Static_assert(sizeof(ETHERSWT_PortInfoType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_PortInfoType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_PortLinkInfoType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_PortLinkInfoType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_SwitchInfoType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_SwitchInfoType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_VLANPortType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_VLANPortType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_MirrorEnableType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_MirrorEnableType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_MirrorStatusType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_MirrorStatusType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_RegAccessType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_RegAccessType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_MibType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_MibType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_ARLEntryType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_ARLEntryType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_ARLListType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_ARLListType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_CFPStatsType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_CFPStatsType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_CFPPortModeType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_CFPPortModeType does not fit in IPC payload");
_Static_assert(sizeof(CFP_RuleType) <= RPC_RESPPAYLOADSZ, "CFP_RuleType does not fit in IPC payload");
_Static_assert(sizeof(CFP_TableSnapshotType) <= RPC_RESPPAYLOADSZ, "CFP_TableSnapshotType does not fit in IPC payload");
_Static_assert(sizeof(ETHERSWT_StreamPolicerType) <= RPC_RESPPAYLOADSZ, "ETHERSWT_StreamPolicerType does not fit in IPC payload");
_Static_assert(sizeof(COMMS_StreamPolicerSnapshotType) <= RPC_RESPPAYLOADSZ, "COMMS_StreamPolicerSnapshotType does not fit in IPC payload");
_Static_assert(sizeof(COMMS_StreamPolicerStatusType) <= RPC_RESPPAYLOADSZ, "COMMS_StreamPolicerStatusType does not fit in IPC payload");

#endif /* ETHERSWT_IPC_H */
