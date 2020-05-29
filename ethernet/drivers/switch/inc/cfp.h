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
/**
    @defgroup grp_eth_switch_cfp Compact Field Processor (CFP)
    @ingroup grp_eth_switchdrv

    @addtogroup grp_eth_switch_cfp
    @{

    @limitations None

    @file ethernet/drivers/switch/inc/cfp.h
    @brief Interface for CFP
    This file provides the interface/APIs which can be used to initialize,
    configure and add/remove rules to/from the CFP block.
    @version 0.1
*/

#ifndef CFP_H
#define CFP_H

#include <stdint.h>
#include "eth_switch.h"

/**
    @name CFP API IDs
    @{
    @brief API IDs for CFP
 */
#define BRCM_SWARCH_CFP_MAX_UDFS_MACRO                (0x01U) /**< @brief #CFP_MAX_UDFS*/
#define BRCM_SWARCH_CFP_MAX_RULES_MACRO               (0x02U) /**< @brief #CFP_MAX_RULES*/
#define BRCM_SWARCH_CFP_MAX_SLICES_MACRO              (0x03U) /**< @brief #CFP_MAX_SLICES*/
#define BRCM_SWARCH_CFP_NUM_FORMATS_MACRO             (0x04U) /**< @brief #CFP_NUM_FORMATS */

#define BRCM_SWARCH_CFP_UDFBASE_TYPE                  (0x0AU) /**< @brief #CFP_UDFBaseType */
#define BRCM_SWARCH_CFP_UDFDEFS_TYPE                  (0x0BU) /**< @brief #CFP_UDFDefsType */
#define BRCM_SWARCH_CFP_KEYDEFS_TYPE                  (0x0CU) /**< @brief #CFP_KeyDefsType */

#define BRCM_SWARCH_CFP_ACTIONDEFS_TYPE               (0x0EU) /**< @brief #CFP_ActionDefsType */
#define BRCM_SWARCH_CFP_METERDEFS_TYPE                (0x0FU) /**< @brief #CFP_MeterDefsType */
#define BRCM_SWARCH_CFP_PKTLENCORR_TYPE               (0x10U) /**< @brief #CFP_PktLenCorrType */
#define BRCM_SWARCH_CFP_L2FRAMING_TYPE                (0x11U) /**< @brief #CFP_L2FramingType */
#define BRCM_SWARCH_CFP_L3FRAMING_TYPE                (0x12U) /**< @brief #CFP_L3FramingType */
#define BRCM_SWARCH_CFP_TTLVAL_TYPE                   (0x13U) /**< @brief #CFP_TTLValType */
#define BRCM_SWARCH_CFP_CHANGEFWDMAP_TYPE             (0x14U) /**< @brief #CFP_ChangeFwdMapType */
#define BRCM_SWARCH_CFP_COLOR_TYPE                    (0x15U) /**< @brief #CFP_ColorType */
#define BRCM_SWARCH_CFP_POLICERMODE_TYPE              (0x16U) /**< @brief #CFP_PolicerModeType */
#define BRCM_SWARCH_CFP_UDF_TYPE                      (0x17U) /**< @brief #CFP_UDFType */
#define BRCM_SWARCH_CFP_KEY_TYPE                      (0x18U) /**< @brief #CFP_KeyType */
#define BRCM_SWARCH_CFP_ACTION_TYPE                   (0x19U) /**< @brief #CFP_ActionType */
#define BRCM_SWARCH_CFP_METER_TYPE                    (0x1AU) /**< @brief #CFP_MeterType */
#define BRCM_SWARCH_CFP_RULE_TYPE                     (0x1BU) /**< @brief #CFP_RuleType */
#define BRCM_SWARCH_CFP_CONFIG_TYPE                   (0x1CU) /**< @brief #CFP_ConfigType */
#define BRCM_SWARCH_CFP_STATS_TYPE                    (0x1DU) /**< @brief #CFP_StatsType */
#define BRCM_SWARCH_CFP_ENTRYSNAPSHOT_TYPE            (0x1EU) /**< @brief #CFP_EntrySnapshotType */
#define BRCM_SWARCH_CFP_TABLESNAPSHOT_TYPE            (0x1FU) /**< @brief #CFP_TableSnapshotType */
#define BRCM_SWARCH_CFP_ROWANDSLICE_TYPE              (0x20U) /**< @brief #CFP_RowAndSliceType */

#define BRCM_SWARCH_CFP_INIT_PROC                     (0x21U) /**< @brief #CFP_Init */
#define BRCM_SWARCH_CFP_ADDRULE_PROC                  (0x22U) /**< @brief #CFP_AddRule */
#define BRCM_SWARCH_CFP_REMOVERULE_PROC               (0x23U) /**< @brief #CFP_RemoveRule */
#define BRCM_SWARCH_CFP_UPDATERULE_PROC               (0x24U) /**< @brief #CFP_UpdateRule */
#define BRCM_SWARCH_CFP_GETSTATS_PROC                 (0x25U) /**< @brief #CFP_GetStats */
#define BRCM_SWARCH_CFP_GETROWCONFIG_PROC             (0x26U) /**< @brief #CFP_GetRowConfig */
#define BRCM_SWARCH_CFP_GETSNAPSHOT_PROC              (0x27U) /**< @brief #CFP_GetSnapshot */
#define BRCM_SWARCH_CFP_ENABLEPORT_PROC               (0x28U) /**< @brief #CFP_EnablePort */
#define BRCM_SWARCH_CFP_DISABLEPORT_PROC              (0x29U) /**< @brief #CFP_DisablePort */
#define BRCM_SWARCH_CFP_DEINIT_PROC                   (0x2AU) /**< @brief #CFP_DeInit */
/** @} */

/**
    @brief Total number of UDFs per rule
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_MAX_UDFS              (9U)

/**
    @brief Total number of slices
    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_MAX_SLICES            (3U)

/**
    @brief Total number of rules
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_MAX_RULES             (256U)

/**
    @brief Number of supported formats
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_NUM_FORMATS (3UL)

/**
    @brief Magic number for the configuration structure, containing 'CFPC'
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_CONFIG_MAGIC_ID       (0x43504643UL)

/**
    @name CFP_UDFBaseType
    @{
    @brief UDF Base definitions

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_UDFBaseType;            /**< @brief typedef for UDF Base */
#define CFP_UDFBASE_SOP                (0U) /**< @brief Start of Packet */
#define CFP_UDFBASE_ENDL2HDR           (2U) /**< @brief End of L2 header */
#define CFP_UDFBASE_ENDL3HDR           (3U) /**< @brief End of L3 header  */
/** @} */

/**
    @name CFP_PktLenCorrType
    @{
    @brief Definitions for packet length correction for flow meter computations

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_PktLenCorrType;         /**< @brief typedef for packet
                                                 length correction */
#define CFP_PKTLENCORR_NONE            (0U) /**< @brief No packet length
                                                 correction */
#define CFP_PKTLENCORR_ADD_PRE_SFD     (1U) /**< @brief Add Preamble and SFD to
                                                 packet length */
#define CFP_PKTLENCORR_ADD_PRE_SFD_IFG (2U) /**< @brief Add Preamble, SFD and
                                                 IFG to packet length */
/** @} */

/**
    @name CFP_TagStatusType
    @{
    @brief Definitions for VLAN tag status

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_TagStatusType;     /**< @brief typedef for tag status */
#define CFP_TAGSTATUS_VID_NONE    (0U) /**< @brief Tag not present */
#define CFP_TAGSTATUS_VID_ZERO    (1U) /**< @brief Tag present with VID=0 */
#define CFP_TAGSTATUS_VID_NONZERO (3U) /**< @brief Tag present with VID!=0 */
/** @} */

/**
    @name CFP_L2FramingType
    @{
    @brief Definitions for L2 framing

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_L2FramingType;     /**< @brief typedef for L2 framing type */
#define CFP_L2FRAMING_DIXV2       (0U) /**< @brief DIXv2 */
#define CFP_L2FRAMING_SNAP_PUB    (1U) /**< @brief SNAP Public */
#define CFP_L2FRAMING_LLC         (2U) /**< @brief LLC */
#define CFP_L2FRAMING_SNAP_PVT    (3U) /**< @brief SNAP Private */
/** @} */

/**
    @name CFP_L3FramingType
    @{
    @brief Definitions for L3 framing

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_L3FramingType;     /**< @brief typedef for L2 framing type */
#define CFP_L3FRAMING_IPV4        (0U) /**< @brief IPv4 */
#define CFP_L3FRAMING_IPV6        (1U) /**< @brief IPv6 */
#define CFP_L3FRAMING_NONIP       (3U) /**< @brief non-IP */
/** @} */

/**
    @name CFP_TTLValType
    @{
    @brief Definitions for TTL value

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_TTLValType;        /**< @brief typedef for TTL value */
#define CFP_TTL_VAL_0             (0U) /**< @brief TTL=0 */
#define CFP_TTL_VAL_1             (1U) /**< @brief TTL=1 */
#define CFP_TTL_VAL_OTHERS        (2U) /**< @brief TTL=Others */
#define CFP_TTL_VAL_255           (3U) /**< @brief TTL=255 */
/** @} */

/**
    @name CFP_ChangeFwdMapType
    @{
    @brief Definitions for changing the forwarding map

    @trace #BRCM_SWREQ_CFP
*/
typedef uint32_t CFP_ChangeFwdMapType;   /**< @brief typedef for changing
                                              forwarding map */
#define CFP_CHANGEFWDMAP_NON       (0UL) /**< @brief No change */
#define CFP_CHANGEFWDMAP_REM       (1UL) /**< @brief Remove ARL destinations */
#define CFP_CHANGEFWDMAP_REP       (2UL) /**< @brief Replace ARL destinations */
#define CFP_CHANGEFWDMAP_ADD       (3UL) /**< @brief Add ARL destinations */
/** @} */

/**
    @name CFP_ColorType
    @{
    @brief Definitions for color

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_ColorType;          /**< @brief typedef for color */
#define CFP_COLOR_GREEN            (0U) /**< @brief Green color */
#define CFP_COLOR_YELLOW           (1U) /**< @brief Yellow color */
#define CFP_COLOR_RED              (2U) /**< @brief Red color */
/** @} */

/**
    @name CFP_PolicerModeType
    @{
    @brief Definitions for policer mode

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_PolicerModeType;  /**< @brief typedef for policer mode */
#define CFP_POLICERMODE_RFC2698  (0U) /**< @brief Policer compliant with RFC2698 */
#define CFP_POLICERMODE_RFC4115  (1U) /**< @brief Policer compliant with RFC4115 */
#define CFP_POLICERMODE_MEF      (2U) /**< @brief Policer compliant with MEF */
#define CFP_POLICERMODE_DISABLED (3U) /**< @brief Policer disabled */
/** @} */

/**
    @name CFP_UDFDefsType
    @{
    @brief Definitions for UDF

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_UDFDefsType;         /**< @brief typedef for UDF definitions */
#define CFP_UDF_OFFSET_SHIFT     (0U)    /**< @brief Bit shift for the UDF offset field
                                              in the baseAndOffset parameter */
#define CFP_UDF_OFFSET_MASK      (0x1FU) /**< @brief Mask for the UDF offset field in
                                              the baseAndOffset parameter */
#define CFP_UDF_BASE_SHIFT       (5U)    /**< @brief Bit shift for the UDF base field in
                                              the baseAndOffset parameter */
#define CFP_UDF_BASE_MASK        (0xE0U) /**< @brief Mask for the UDF base field in the
                                              baseAndOffset parameter */
/** @} */

/**
    @brief Structure for CFP UDF

    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_UDFType {
    uint16_t  value;         /**< @brief Value of the UDF */
    uint16_t  mask;          /**< @brief Value of the UDF mask */
    uint8_t   baseAndOffset; /**< @brief Base and offset of
                                   the UDF encoded in a
                                   byte as follows:
                                   base(3) | offset (5)   */
} CFP_UDFType;

/**
    @brief Structure for CFP statistics
    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_StatsType {
    uint32_t green;  /**< @brief Green counter */
    uint32_t yellow; /**< @brief Yellow counter */
    uint32_t red;    /**< @brief Red counter */
} CFP_StatsType;

/**
    @name CFP_EntrySnapshotType
    @{
    @brief Definitions for CFP entry snapshot

    @trace #BRCM_SWREQ_CFP
*/
typedef uint8_t CFP_EntrySnapshotType;          /**< @brief typedef for entry snapshot field definitions */
#define CFP_ENTRYSNAPSHOT_ENABLE_MASK   (0x80U) /**< @brief Mask for the enable field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_ENABLE_SHIFT  (7U)    /**< @brief Bit shift for the enable field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_STATIC_MASK   (0x40U) /**< @brief Mask for the static field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_STATIC_SHIFT  (6U)    /**< @brief Bit shift for the static field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_SLICE_MASK    (0x30U) /**< @brief Mask for the slice field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_SLICE_SHIFT   (4U)    /**< @brief Bit shift for the slice field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_FORMAT_MASK   (0xCU)  /**< @brief Mask for the format field in the entry snapshot */
#define CFP_ENTRYSNAPSHOT_FORMAT_SHIFT  (2U)    /**< @brief Bit shift for the format field in the entry snapshot */
/** @} */

/**
    @brief Structure for global UDF allocation
    This stores the global UDF information, including the address it is mapped
    to

    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_UDFEntryType {
    uint8_t  enable;                                 /**< @brief Flag indicating
                                                          whether the UDF is
                                                          enabled or not */
    uint8_t  address;                                /**< @brief Address (base
                                                           and offset) of the
                                                           UDF */
} CFP_UDFEntryType;

/**
    @brief List of all allocated global UDFs per format

    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_UDFAllocListType {
    CFP_UDFEntryType udfs[CFP_MAX_SLICES][CFP_MAX_UDFS]; /**< @brief Array of all
                                                              UDFs across all
                                                              slices for a
                                                              particular format */
} CFP_UDFAllocListType;

/**
    @brief Structure for CFP table snapshot
    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_TableSnapshotType {
    uint32_t              numValidEntries;      /**< @brief Number of valid entries */
    CFP_EntrySnapshotType entry[CFP_MAX_RULES]; /**< @brief Table of per row snapshot
                                                     encoded in a bitfield as follows:
                                                     Enable (1) | Static (1) | Slice(2) | Format(2) | Resv(2) */
    CFP_UDFAllocListType  udfList[CFP_NUM_FORMATS]; /**< @brief List of allocated UDFs per format */
    uint16_t              portEnableMask;       /**< @brief Bitmask of ports where CFP
                                                      lookup is enabled */
} CFP_TableSnapshotType;

/**
    @name CFP_KeyDefsType
    @{
    @brief Definitions for TCAM Key

    @trace #BRCM_SWREQ_CFP
*/
typedef uint32_t CFP_KeyDefsType;               /**< @brief typedef for TCAM key definitions */
#define CFP_KEY_TTL_MASK               (0x3UL)      /**< @brief Mask for the TTL field in the flags parameter */
#define CFP_KEY_TTL_SHIFT              (0UL)        /**< @brief Bit shift for the TTL field in the flags parameter */
#define CFP_KEY_AUTH_MASK              (0x4UL)      /**< @brief Mask for the IP authentication field in the flags parameter */
#define CFP_KEY_AUTH_SHIFT             (2UL)        /**< @brief Bit shift for the IP authentication field in the flags parameter */
#define CFP_KEY_NONFIRSTFRAG_MASK      (0x8UL)      /**< @brief Mask for the non first fragment field in the flags parameter */
#define CFP_KEY_NONFIRSTFRAG_SHIFT     (3UL)        /**< @brief Bit shift for the non first fragment field in the flags parameter */
#define CFP_KEY_FRAG_MASK              (0x10UL)     /**< @brief Mask for the fragmentation field in the flags parameter */
#define CFP_KEY_FRAG_SHIFT             (4UL)        /**< @brief Bit shift for the fragmentation field in the flags parameter */
#define CFP_KEY_PROTO_MASK             (0x1FE0UL)   /**< @brief Mask for the protocol field in the flags parameter */
#define CFP_KEY_PROTO_SHIFT            (5UL)        /**< @brief Bit shift for the protocol field in the flags parameter */
#define CFP_KEY_TOS_MASK               (0x1FE000UL) /**< @brief Mask for the TOS (type of service) field in the flags parameter */
#define CFP_KEY_TOS_SHIFT              (13UL)       /**< @brief Bit shift for the TOS (type of service) field in the flags parameter */
#define CFP_KEY_IP_MASK                (0x1FFFFFUL) /**< @brief Mask for the IP fields in the flags parameter */
#define CFP_KEY_ETHTYPE_MASK           (0xFFFFUL)   /**< @brief Mask for the Ethertype field in the flags parameter */
#define CFP_KEY_ETHTYPE_SHIFT          (0UL)        /**< @brief Bit shift for the Ethertype field in the flags parameter */
#define CFP_KEY_NONIP_MASK             (0xFFFFUL)   /**< @brief Mask for the non-IP fields in the flags parameter */
#define CFP_KEY_INGPORTBMP_MASK        (0x1FFU)     /**< @brief Mask for the ingress port bitmap parameter */
#define CFP_KEY_TAG_UN_TAGGED_MASK     (0x200000UL) /**< @brief Mask for the Un-Tagged field in the tag parameter */
#define CFP_KEY_TAG_UN_TAGGED_SHIFT    (21UL)       /**< @brief Bit shift for Un-Tagged field in the tag parameter */
#define CFP_KEY_TAG_PRIO_TAGGED_MASK   (0x400000UL) /**< @brief Mask for the Priority-Tagged field in the tag parameter */
#define CFP_KEY_TAG_PRIO_TAGGED_SHIFT  (22UL)       /**< @brief Bit shift for Priority-Tagged field in the tag parameter */
#define CFP_KEY_TAG_VLAN_TAGGED_MASK   (0x800000UL) /**< @brief Mask for the VLAN-Tagged field in the tag parameter */
#define CFP_KEY_TAG_VLAN_TAGGED_SHIFT  (23UL)       /**< @brief Bit shift for VLAN-Tagged field in the tag parameter */
#define CFP_KEY_TAG_ID_MASK            (0x3FFCUL)   /**< @brief Mask for the id field in the tag parameter */
#define CFP_KEY_TAG_ID_SHIFT           (2UL)        /**< @brief Bit shift for the id field in the tag parameter */
#define CFP_KEY_TAG_DEI_MASK           (0x4000UL)   /**< @brief Mask for the DEI field in the tag parameter */
#define CFP_KEY_TAG_DEI_SHIFT          (14UL)       /**< @brief Bit shift for the DEI field in the tag parameter */
#define CFP_KEY_TAG_PCP_MASK           (0x38000UL)  /**< @brief Mask for the PCP field in the tag parameter */
#define CFP_KEY_TAG_PCP_SHIFT          (15UL)       /**< @brief Bit shift for the PCP field in the tag parameter */
#define CFP_KEY_TAG_ID_VALID_MASK      (0x40000UL)  /**< @brief Mask for the VLAN id valid field in the tag parameter */
#define CFP_KEY_TAG_ID_VALID_SHIFT     (18UL)       /**< @brief Bit shift for the VLAN id valid field in the tag parameter */
#define CFP_KEY_TAG_DEI_VALID_MASK     (0x80000UL)  /**< @brief Mask for the DEI valid field in the tag parameter */
#define CFP_KEY_TAG_DEI_VALID_SHIFT    (19UL)       /**< @brief Bit shift for the DEI valid field in the tag parameter */
#define CFP_KEY_TAG_PCP_VALID_MASK     (0x100000UL) /**< @brief Mask for the PCP valid field in the tag parameter */
#define CFP_KEY_TAG_PCP_VALID_SHIFT    (20UL)       /**< @brief Bit shift for the PCP valid field in the tag parameter */
#define CFP_KEY_TAG_PCPMASK_MASK       (0xE000U)    /**< @brief Mask for the PCP Mask field in the cTagMask parameter */
#define CFP_KEY_TAG_PCPMASK_SHIFT      (13U)        /**< @brief Bit shift for the PCP Mask field in the cTagMask parameter */
#define CFP_KEY_TAG_IDMASK_MASK        (0xFFFU)     /**< @brief Mask for the VLAN-ID Mask field in the cTagMask parameter */
#define CFP_KEY_TAG_IDMASK_SHIFT       (0U)         /**< @brief Bit shift for the VLAN-ID Mask field in the cTagMask parameter */
/** @} */

/**
    @brief Structure for CFP TCAM key
    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_KeyType {

    uint32_t           flags;             /**< @brief Overloaded flags field
                                               Encoded as follows for non-IP:
                                               resv(16) | ethType(16)
                                               Encoded as follows for IPv4/IPv6:
                                               resv(11) | tos(8) | proto(8) | frag(1) | non-first-frag(1) | auth(1) | TTL(2)
                                               */
    uint32_t           flagsMask;         /**< @brief Mask for flags field */
    uint32_t           cTagFlags;         /**< @brief C-Tag Vlan and Status
                                               encoded in a bitfield as follows:
                                               Resv(8)| VLAN-Tagged(1) | Priority-Tagged(1) | Un-Tagged(1)| PCP Valid (1) | DEI Valid (1) | C-Vid Valid (1) | PCP (3) | DEI (1) | C-Vid(12) | Resv(2)*/
    uint16_t           cTagMask;          /**< @brief Mask for Vlan and PCP field in C-Tag
                                               PCP Mask(3) | Resv(1) | VLAN-ID Mask(12) */
    uint32_t           sTagFlags;         /**< @brief S-Tag Vlan and Status
                                               encoded in a bitfield as follows:
                                               Resv(8)| VLAN-Tagged(1) | Priority-Tagged(1) | Un-Tagged(1) | PCP Valid (1) | DEI Valid (1) | S-Vid Valid (1) | PCP (3) | DEI (1) | S-Vid(12) | Resv(2)*/
    uint16_t           sTagMask;          /**< @brief Mask for Vlan and PCP field in S-Tag
                                               PCP Mask(3) | Resv(1) | VLAN-ID Mask(12) */
    uint16_t           ingressPortBitmap; /**< @brief Bitmap representing the
                                               ingress ports to which the rule
                                               applies */
    CFP_UDFType        udfList[CFP_MAX_UDFS]; /**< @brief List of UDFs for the
                                                   rule */
    uint8_t            numEnabledUDFs;        /**< @brief Number of enabled UDFs
                                                   for the rule  */
    CFP_L2FramingType  l2Framing;             /**< @brief L2 framing */
    CFP_L3FramingType  l3Framing;             /**< @brief L3 framing, used to
                                                   decide the slice type */
} CFP_KeyType;


/**
    @name CFP_MeterDefsType
    @{
    @brief Definitions for CFP meter

    @trace #BRCM_SWREQ_CFP
*/
typedef uint32_t CFP_MeterDefsType;         /**< @brief typedef for meter definitions */
#define CFP_METER_TKBKT_SHIFT  (0UL)        /**< @brief Bit shift for token bucket size */
#define CFP_METER_TKBKT_MASK   (0x7FFFFFUL) /**< @brief Mask for token bucket size */
#define CFP_METER_BKTSZ_SHIFT  (0UL)        /**< @brief Bit shift for burst size */
#define CFP_METER_BKTSZ_MASK   (0xFFFFFUL)  /**< @brief Mask for burst size */
#define CFP_METER_REFCNT_SHIFT (0UL)        /**< @brief Bit shift for reference count */
#define CFP_METER_REFCNT_MASK  (0x7FFFFUL)  /**< @brief Mask for reference count */
#define CFP_METER_CM_MASK      (0x1UL)      /**< @brief Mask for color mode */
#define CFP_METER_CM_SHIFT     (0UL)        /**< @brief Bit shift for color mode */
#define CFP_METER_ACT_MASK     (0x2UL)      /**< @brief Mask for meter action mode */
#define CFP_METER_ACT_SHIFT    (1UL)        /** @brief Bit shift for meter action mode */
#define CFP_METER_CF_MASK      (0x4UL)      /** @brief Mask for coupling flag */
#define CFP_METER_CF_SHIFT     (2UL)        /** @brief Bit shift for coupling flag */
#define CFP_METER_MODE_MASK    (0x18UL)     /**< @brief Mask for policer mode */
#define CFP_METER_MODE_SHIFT   (3UL)        /**< @brief Bit shift for policer mode */
/** @} */

/**
    @brief Structure for CFP Meter

    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_MeterType {
    uint32_t   cirTkBkt;     /**< @brief Committed token bucket size */
    uint32_t   cirBktSize;   /**< @brief Committed burst size */
    uint32_t   cirRefCnt;    /**< @brief Committed information rate */
    uint32_t   eirTkBkt;     /**< @brief Excess token bucket size */
    uint32_t   eirBktSize;   /**< @brief Excess burst size */
    uint32_t   eirRefCnt;    /**< @brief Excess information rate */
    uint8_t    policerFlags; /**< @brief Policer flags encoded as follows:
                                  resv(3) | mode(2) | coupling flag(1) | action (1) | color mode(1) */
} CFP_MeterType;

/**
    @name CFP_ActionDefsType
    @{
    @brief Definitions for CFP action

    @trace #BRCM_SWREQ_CFP
*/
typedef uint16_t CFP_ActionDefsType;                  /**< @brief typedef for CFP meter definitions */
#define CFP_ACTION_DSTMAP_SHIFT           (0UL)       /**< @brief Bit shift for destination map */
#define CFP_ACTION_DSTMAP_MASK            (0xFFFFUL)  /**< @brief Mask for destination map */
#define CFP_ACTION_CHANGE_FWDMAP_SHIFT    (16UL)      /**< @brief Bit shift for change of forward map */
#define CFP_ACTION_CHANGE_FWDMAP_MASK     (0x30000UL) /**< @brief Mask for change of forward map */
#define CFP_ACTION_TOS_SHIFT              (0U)        /**< @brief Bit shift for TOS */
#define CFP_ACTION_TOS_MASK               (0x3FU)     /**< @brief Mask for TOS */
#define CFP_ACTION_CHANGE_TOS_SHIFT       (6U)        /**< @brief Bit shift for changing TOS */
#define CFP_ACTION_CHANGE_TOS_MASK        (0x40U)     /**< @brief Mask for changing TOS */
#define CFP_ACTION_TC_SHIFT               (0U)        /**< @brief Bit shift for TC */
#define CFP_ACTION_TC_MASK                (0x7U)      /**< @brief Mask for TC */
#define CFP_ACTION_CHANGE_TC_SHIFT        (3U)        /**< @brief Bit shift for changing TC */
#define CFP_ACTION_CHANGE_TC_MASK         (0x8U)      /**< @brief Mask for changing TC */
#define CFP_ACTION_USE_DFLT_RED_SHIFT     (4U)        /**< @brief Bit shift for default RED profile */
#define CFP_ACTION_USE_DFLT_RED_MASK      (0x10U)     /**< @brief Mask for default RED profile */
#define CFP_ACTION_LPBK_EN_SHIFT          (3U)        /**< @brief Bit shift for enabling loopback */
#define CFP_ACTION_LPBK_EN_MASK           (0x8U)      /**< @brief Mask for enabling loopback */
#define CFP_ACTION_BYPASS_STP_SHIFT       (2U)        /**< @brief Bit shift for bypass STP */
#define CFP_ACTION_BYPASS_STP_MASK        (0x4U)      /**< @brief Mask for bypass STP */
#define CFP_ACTION_BYPASS_EAP_SHIFT       (1U)        /**< @brief Bit shift for bypass EAP */
#define CFP_ACTION_BYPASS_EAP_MASK        (0x2U)      /**< @brief Mask for bypass EAP */
#define CFP_ACTION_BYPASS_VLAN_SHIFT      (0U)        /**< @brief Bit shift for bypass VLAN */
#define CFP_ACTION_BYPASS_VLAN_MASK       (0x1U)      /**< @brief Mask for bypass VLAN */
#define CFP_ACTION_COLOR_SHIFT            (1U)        /**< @brief Bit shift for color */
#define CFP_ACTION_COLOR_MASK             (0x6U)      /**< @brief Mask for color */
#define CFP_ACTION_CHANGE_COLOR_SHIFT     (0U)        /**< @brief Bit shift for changing color */
#define CFP_ACTION_CHANGE_COLOR_MASK      (0x1U)      /**< @brief Mask for changing color */
/** @} */

/**
    @brief Structure for CFP Action

    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_ActionType {
    CFP_MeterType   meter;         /**< @brief CFP meter */
    uint32_t        dstMapIBFlags; /**< @brief Destination map flags for In-bandwidth
                                        profile traffic encoded as follows:
                                        resv(14) | changeFwdMap(2) | dstMap(16) */
    uint32_t        dstMapOBFlags; /**< @brief Destination map flags for Out-bandwidth
                                        profile traffic encoded as follows:
                                        resv(14) | changeFwdMap(2) | dstMap(16) */
    uint8_t         tosIBFlags;    /**< @brief TOS flags for In-bandwidth profile
                                        traffic encoded as follows:
                                        resv(1) | changeTOS(1) | newTOS (6) */
    uint8_t         tosOBFlags;    /**< @brief TOS flags for Out-bandwidth profile
                                        traffic encoded as follows:
                                        resv(1) | changeTOS(1) | newTOS (6) */
    uint8_t         tcFlags;       /**< @brief TC flags encoded as follows:
                                        resv(4) | changeTC(1) | newTC (3) */
    uint8_t         reasonCode;    /**< @brief Reason code */
    uint8_t         otherFlags;    /**< @brief Fields encoded as follows:
                                        resv(3) | RED dflt(1) | lpbkEn(1) | bypassSTP(1) | bypassEAP(1) | bypassVLAN(1) */
    uint8_t         colorFlags;    /**< @brief Color flags encoded as follows:
                                        resv(5) | color(2) | changeColor(1) */
    uint8_t         chainID;       /**< @brief chain ID */
} CFP_ActionType;

/**
    @name CFP_RowAndSliceType
    @{
    @brief Definitions for encoded CFP row and slice

    @trace #BRCM_SWREQ_CFP
*/
typedef uint16_t CFP_RowAndSliceType;            /**< @brief typedef for CFP encoded row and slice */
#define CFP_ROWANDSLICE_ROW_SHIFT      (0U)      /**< @brief Bit shift for row number */
#define CFP_ROWANDSLICE_ROW_MASK       (0xFFFU)  /**< @brief Mask for row number */
#define CFP_ROWANDSLICE_SLICE_SHIFT    (12U)     /**< @brief Bit shift for slice number */
#define CFP_ROWANDSLICE_SLICE_MASK     (0x3000U) /**< @brief Mask for slice number */
/** @} */

/**
    @brief Structure for CFP Rule

    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_RuleType {
    CFP_KeyType         key;         /**< @brief key */
    CFP_ActionType      action;      /**< @brief action */
    CFP_RowAndSliceType rowAndSlice; /**< @brief row and slice number encoded as follows:
                                          resv(2) | slice (2) | row (12) */
} CFP_RuleType;

/**
    @brief Structure for CFP config

    @trace #BRCM_SWREQ_CFP
*/
typedef struct COMP_PACKED _CFP_ConfigType {
    uint32_t            magicId;                 /**< @brief Magic number 'CFPC'
                                                      (checked for validity) */
    CFP_RuleType        ruleList[CFP_MAX_RULES]; /**< @brief List of rules  */
    uint16_t            ruleListSz;              /**< @brief Number of valid
                                                      rules */
    CFP_PktLenCorrType  pktLenCorr;              /**< @brief Packet length
                                                      correction */
    uint16_t            portEnableMask;          /**< @brief Bitmask of ports
                                                      where CFP lookup is
                                                      enabled */
} CFP_ConfigType;

/** @brief Initialize CFP

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver is initialized

    This API initializes the CFP block. It performs the following steps:
        - clear the CFP TCAM
        - enables the feature on all ports
        - enable the global rate meter
        - programs the rules from the configuration. All these rules are
          marked as static (read only). They cannot be modified later using
          any of the APIs like #CFP_RemoveRule, #CFP_UpdateRule

    @param[in]   aID                   Switch index
    @param[in]   aConfig               Pointer to CFP configuration

    @retval  #BCM_ERR_OK               CFP block initialized successfully
    @retval  #BCM_ERR_INVAL_PARAMS     Register/Configuration is invalid
    @retval  #BCM_ERR_INVAL_PARAMS     ruleListSz is greater than #CFP_MAX_RULES
    @retval  #BCM_ERR_INVAL_PARAMS     aConfig is NULL
    @retval  #BCM_ERR_INVAL_PARAMS     Switch index is invalid
    @retval  #BCM_ERR_TIME_OUT         CFP/Switch Register access timed out
    @retval  #BCM_ERR_NOMEM            Ran out of UDFs to accomodate a rule
    @retval  #BCM_ERR_INVAL_MAGIC      Config magic is invalid
    @retval  #BCM_ERR_INVAL_STATE      CFP is not in reset state

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_Init(ETHERSWT_HwIDType aID,
                 const CFP_ConfigType* const aConfig);

/** @brief Add a CFP rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API validates and adds a rule to CFP TCAM. If a valid slice is provided in input,
    the driver tries to accomodate all UDFs of the rule in the slice. If no valid slice is
    provided, the driver tries to allocate the highest slice where all UDFs can be accomodated.
    If required, new UDFs can be allocated in the slice. Similarly, if a valid row number is
    provided as input, the driver checks if it is free and proceeds with programming the rule.
    In the case a static entry is already present at the input row error is returned. In case a
    non-static entry is present then the input rule is inserted at the row by shifting the
    existing entries. If no valid row is provided in input, the driver allocates the lowest free
    row number and proceeds with programming the rule.

    @param[in]     aID                 Switch index
    @param[in]     aKey                Key configuration
    @param[in]     aAction             Action configuration
    @param[inout]  aRow                Pointer to the requested row number (set to CFP_MAX_RULES
                                       in case the driver needs to allocate) on input. On output
                                       it provides the configured row number
    @param[inout]  aSlice              Pointer to the requested slice number(set to CFP_MAX_SLICES
                                       in case the driver needs to allocate) on input. On output
                                       it provides the configured slice number

    @retval  #BCM_ERR_OK               Rule programmed successfully in CFP block
    @retval  #BCM_ERR_INVAL_PARAMS     Register/Configuration is invalid
    @retval  #BCM_ERR_TIME_OUT         CFP/Switch Register access timed out
    @retval  #BCM_ERR_NOMEM            All rows in TCAM are occupied or unable
                                       to find a slice where the UDFs for this
                                       rule can be accomodated

    @retval  #BCM_ERR_INVAL_STATE      CFP is not in initialized state
    @retval  #BCM_ERR_NOPERM           Static rule configured at row which is
                                       being requested in aRule

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_AddRule(ETHERSWT_HwIDType aID,
                    const CFP_KeyType* const aKey,
                    const CFP_ActionType* const aAction,
                    uint32_t* const aRow,
                    uint32_t* const aSlice);

/** @brief Remove a CFP rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API removes a rule (if it exists) from the CFP block. It also frees up
    any UDFs which are no longer used after deletion of this rule. This API returns
    an error in case an attempt to delete a static rule is made

    @param[in]     aID                 Switch index
    @param[in]     aRow                Row number

    @retval  #BCM_ERR_OK               Rule removed successfully from CFP block
    @retval  #BCM_ERR_INVAL_PARAMS     Register is invalid
    @retval  #BCM_ERR_TIME_OUT         CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE      CFP is not in initialized state
    @retval  #BCM_ERR_NOT_FOUND        No rule configured at aRow
    @retval  #BCM_ERR_NOPERM           Static rule configured at aRow

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_RemoveRule(ETHERSWT_HwIDType aID,
                       uint32_t aRow);

/** @brief Update a CFP rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API updates an existing rule in the CFP block. Only the action policy
    RAM is updated. This API returns an error in case an attempt to update a
    static rule is made

    @param[in]     aID                 Switch index
    @param[in]     aRow                Row number
    @param[in]     aAction             Pointer to action policy configuration

    @retval  #BCM_ERR_OK               Action policy RAM successfully updated
    @retval  #BCM_ERR_INVAL_PARAMS     Register is invalid
    @retval  #BCM_ERR_TIME_OUT         CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE      CFP is not in initialized state
    @retval  #BCM_ERR_NOT_FOUND        No rule configured at aRow
    @retval  #BCM_ERR_NOPERM           Static rule configured at aRow

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_UpdateRule(ETHERSWT_HwIDType aID,
                       uint32_t aRow,
                       const CFP_ActionType* const aAction);

/** @brief Get statistic counters for a CFP rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API fetches the red, yellow and green statistic counters for an
    existing CFP rule

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number
    @param[out]    aStats            Pointer to location to store statistics

    @retval  #BCM_ERR_OK             Statistics fetched successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE    CFP is not in initialized state
    @retval  #BCM_ERR_NOT_FOUND      No rule configured at aRow

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_GetStats(ETHERSWT_HwIDType aID,
                     uint32_t aRow,
                     CFP_StatsType *const aStats);

/** @brief Get configuration for a CFP rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API fetches the configuration for an existing CFP rule

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number
    @param[out]    aConfig           Pointer to location to store configuration

    @retval  #BCM_ERR_OK             Configuration fetched successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval  #BCM_ERR_INVAL_PARAMS   aRow is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE    CFP is not in initialized state
    @retval  #BCM_ERR_NOT_FOUND      No rule configured at aRow

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_GetRowConfig(ETHERSWT_HwIDType aID,
                         uint32_t aRow,
                         CFP_RuleType *const aConfig);

/** @brief Get summarised snapshot of the CFP table

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API fetches the summarised snapshot of the CFP table

    @param[in]     aID               Switch index
    @param[out]    aTable            Pointer to location to store table snapshot

    @retval  #BCM_ERR_OK             Snapshot fetched successfully
    @retval  #BCM_ERR_INVAL_PARAMS   aSnapshot is NULL
    @retval  #BCM_ERR_INVAL_STATE    CFP is not in initialized state

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_GetSnapshot(ETHERSWT_HwIDType aID,
                        CFP_TableSnapshotType *const aSnapshot);

/** @brief Enable CFP lookup on a port

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API enables CFP lookup on a port

    @param[in]     aID               Switch index
    @param[in]     aPortNum          Port number

    @retval  #BCM_ERR_OK             Port enabled successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid or port number is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE    CFP is not in initialized state

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_EnablePort(ETHERSWT_HwIDType aID,
                       uint32_t aPortNum);

/** @brief Disable CFP lookup on a port

    @behavior Sync, Re-entrant (for different switch index)

    @pre Switch driver and CFP is initialized

    This API disables CFP lookup on a port

    @param[in]     aID               Switch index
    @param[in]     aPortNum          Port number

    @retval  #BCM_ERR_OK             Port disabled successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid or port number is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE    CFP is not in initialized state

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_DisablePort(ETHERSWT_HwIDType aID,
                       uint32_t aPortNum);

/** @brief De-Initialize CFP

    @behavior Sync, Re-entrant (for different switch index)

    @pre CFP, Switch driver is initialized

    This API de-initializes the CFP block. It disables the feature on all ports

    @param[in]   aID                   Switch index

    @retval  #BCM_ERR_OK               CFP block de-initialized successfully
    @retval  #BCM_ERR_INVAL_PARAMS     Register is invalid
    @retval  #BCM_ERR_TIME_OUT         CFP/Switch Register access timed out
    @retval  #BCM_ERR_INVAL_STATE      CFP is already in reset state

    @post None

    @trace #BRCM_SWREQ_CFP

    @limitations None
*/
int32_t CFP_DeInit(ETHERSWT_HwIDType aID);

#endif /* CFP_H */
/** @} */
