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
    @defgroup grp_comms_stream_cfg_ifc Communication Subsystem Stream Configuration
    @ingroup grp_comms_sw

    @addtogroup grp_comms_stream_cfg_ifc
    @{

    @limitations None

    @file comms_stream.h
    @brief Interface for Communication Subsystem Streams
    This header file contains the interfaces for Communication Subsystem Streams
    @version 0.1 Initial Version
 */

#ifndef COMMS_STREAM_H
#define COMMS_STREAM_H

#include <stdint.h>



/**
    @name Communication Subsystem API IDs
    @{
    @brief API IDs for Communication Subsystem
*/
#define BRCM_SWARCH_COMMS_STREAMPOLICERCONFIG_TYPE         (0x01U) /**< @brief #COMMS_StreamPolicerConfigType */
#define BRCM_SWARCH_COMMS_STREAMPOLICERACTION_TYPE         (0x02U) /**< @brief #COMMS_StreamPolicerActionType */
#define BRCM_SWARCH_COMMS_STREAMPOLICERSNAPSHOT_TYPE       (0x03U) /**< @brief #COMMS_StreamPolicerSnapshotType */
#define BRCM_SWARCH_COMMS_STREAMPOLICERSTATUS_TYPE         (0x04U) /**< @brief #COMMS_StreamPolicerStatusType */
#define BRCM_SWARCH_COMMS_STREAMPOLICERSTATE_TYPE          (0x05U) /**< @brief #COMMS_StreamPolicerStateType */
/** @} */

/**
    @name COMMS_StreamPolicerActionType
    @{
    @brief Policer action definitions
    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
typedef uint8_t COMMS_StreamPolicerActionType;        /**< @brief typedef for Stream policer action definitions */
#define COMMS_STREAMPOLICERACTION_BLOCK_SHIFT  (0U)   /**< @brief Bit shift for action block */
#define COMMS_STREAMPOLICERACTION_BLOCK_MASK   (0x1U) /**< @brief Mask for action block */
#define COMMS_STREAMPOLICERACTION_REPORT_SHIFT (1U)   /**< @brief Bit shift for action report */
#define COMMS_STREAMPOLICERACTION_REPORT_MASK  (0x2U) /**< @brief Mask for action report */
/** @} */

/**
    @name COMMS_StreamPolicerStatusType
    @{
    @brief Policer status definitions
    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
typedef uint8_t COMMS_StreamPolicerStateType;         /**< @brief typedef for Stream policer status definitions */
#define COMMS_STREAMPOLICERSTATE_VALID_SHIFT   (0U)   /**< @brief Bit shift for valid */
#define COMMS_STREAMPOLICERSTATE_VALID_MASK    (0x1U) /**< @brief Mask for valid */
#define COMMS_STREAMPOLICERSTATE_BLOCKED_SHIFT (1U)   /**< @brief Bit shift for blocked */
#define COMMS_STREAMPOLICERSTATE_BLOCKED_MASK  (0x2U) /**< @brief Mask for blocked */
#define COMMS_STREAMPOLICERSTATE_STATIC_SHIFT  (2U)   /**< @brief Bit shift for static */
#define COMMS_STREAMPOLICERSTATE_STATIC_MASK   (0x4U) /**< @brief Mask for static */


/** @} */
/**
    @brief Switch Port configuration
    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
typedef struct _COMMS_StreamPolicerConfigType {
    uint32_t                      rate;               /**< @brief Policer committed information rate in Kbps */
    uint32_t                      burstSize;          /**< @brief Policer committed burst size in bytes */
    uint32_t                      dropThreshold;      /**< @brief Number of packets which are allowed to exceed the
                                                           bandwidth before the stream is blocked/reported */
    uint32_t                      monitoringInterval; /**< @brief Number of system ticks to be used as the polling
                                                           interval for checking if dropThreshold is exceeded */
    uint32_t                      portMask;           /**< @brief Ingress port mask */
    uint16_t                      arlEntryIndex;      /**< @brief Index (in ARL table) of associated ARL entry */
    COMMS_StreamPolicerActionType action;             /**< @brief Policer action to be taken on exceeding dropThreshold */
    uint8_t                       reserved;           /**< @brief Reserved */
} COMMS_StreamPolicerConfigType;

/**
    @brief Stream policer status
    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
typedef struct _COMMS_StreamPolicerStatusType {
    uint32_t idx;            /**< @brief Stream index */
    uint8_t  macAddress[6U]; /**< @brief Destination MAC address */
    uint16_t vlan;           /**< @brief VLAN identifier */
    uint32_t portMask;       /**< @brief Ingress port mask */
    uint32_t blocked;        /**< @brief Flag indicating whether the stream is blocked or functional */
    uint32_t isStatic;       /**< @brief Flag indicating whether the stream is static or not */
    uint32_t greenCntr;      /**< @brief Number of in-bound frames */
    uint32_t redCntr;        /**< @brief Number of out-of-bound frames */
} COMMS_StreamPolicerStatusType;

/**
    @brief Stream policer snapshot
    @trace #BRCM_SWREQ_COMMS_STREAMS
*/
typedef struct _COMMS_StreamPolicerSnapshotType {
    COMMS_StreamPolicerStateType policer[COMMS_MAX_STREAM_POLICER_ENTRIES]; /**< @brief Stream policer state */
} COMMS_StreamPolicerSnapshotType;

#endif /* COMMS_STREAM_H */

/** @} */
