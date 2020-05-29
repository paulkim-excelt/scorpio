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
  @addtogroup grp_ets_ifc
  @{
  @file ets_config.h
  @brief Configuration data structure for the Ethernet TimeSync subsystem

  This file provides the definition of the configuration for Ethernet TimeSync
  subsystem which is used to provide the required initialization parameters
  via the ETS_Init() API

  @version 1.25 Imported from MS Word document
*/

#ifndef ETS_CONFIG_H
#define ETS_CONFIG_H

#include <stdint.h>
#include <compiler.h>

/**
    @name ETS Library API IDs
    @{
    @brief API IDs for ETS Library
 */
#define BRCM_SWARCH_ETS_CONFIGMAGIC_MACRO   (0x12U)  /**< @brief #ETS_CONFIG_MAGIC_ID */
#define BRCM_SWARCH_ETS_ADMINMODE_TYPE      (0x13U) /**< @brief #ETS_AdminModeType */
#define BRCM_SWARCH_ETS_CLKMODE_TYPE        (0x14U) /**< @brief #ETS_ClockModeType */
#define BRCM_SWARCH_ETS_ROLE_TYPE           (0x15U) /**< @brief #ETS_RoleType */
#define BRCM_SWARCH_ETS_INITPDELAY_TYPE     (0x16U) /**< @brief #ETS_InitLogPdelayType */
#define BRCM_SWARCH_ETS_OPERPDELAY_TYPE     (0x17U) /**< @brief #ETS_OperLogPdelayType */
#define BRCM_SWARCH_ETS_INITSYNC_TYPE       (0x18U) /**< @brief #ETS_InitLogSyncType */
#define BRCM_SWARCH_ETS_OPERSYNC_TYPE       (0x19U) /**< @brief #ETS_OperLogSyncType */
#define BRCM_SWARCH_ETS_INTFCONFIG_TYPE     (0x1AU) /**< @brief #ETS_IntfConfigType */
#define BRCM_SWARCH_ETS_CONFIG_TYPE         (0x1BU) /**< @brief #ETS_ConfigType */
/** @} */

/**
    @name ETS_AdminModeType
    @{
    @brief ETS Admin Mode

*/
typedef uint8_t ETS_AdminModeType;    /**< @brief typedef for ETS Admin Mode */
#define ETS_ADMINMODE_DISABLE    (0U) /**< @brief Operation/feature disabled */
#define ETS_ADMINMODE_ENABLE     (1U) /**< @brief Operation/feature enabled */
/** @} */

/**
    @name ETS_ClockModeType
    @{
    @brief ETS Clock Mode

*/
typedef uint8_t ETS_ClockModeType;      /**< @brief typedef for ETS Admin Mode*/
#define ETS_CLOCKMODE_GM           (0U) /**< @brief Grandmaster mode */
#define ETS_CLOCKMODE_SLAVE        (1U) /**< @brief Slave mode */
/** @} */

/**
    @name ETS_RoleType
    @{
    @brief ETS Port Role

*/
typedef uint8_t ETS_RoleType;         /**< @brief typedef for ETS port role */
#define ETS_ROLE_MASTER          (0U) /**< @brief Master role */
#define ETS_ROLE_SLAVE           (1U) /**< @brief Slave role */
#define ETS_ROLE_STACKING        (2U) /**< @brief Stacking role */
/** @} */

/**
    @name ETS_InitLogPdelayType
    @{
    @brief Allowed values of initial logarithmic pdelay intervals

*/
typedef int32_t ETS_InitLogPdelayType; /**< @brief typedef for init pDelay intervals */
#define ETS_INITLOGPDELAY_1S   (0L)    /**< @brief 1s initial pDelay period */
#define ETS_INITLOGPDELAY_INF  (127L)  /**< @brief Initial pDelay disabled */
/** @} */

/**
    @name ETS_InitLogPdelayType
    @{
    @brief Allowed values of operational logarithmic pdelay intervals

*/
typedef int32_t ETS_OperLogPdelayType;  /**< @brief typedef for oper pDelay intervals */
#define ETS_OPERLOGPDELAY_1S   (0L)     /**< @brief 1s operational pDelay period*/
#define ETS_OPERLOGPDELAY_2S   (1L)     /**< @brief 2s operational pDelay period*/
#define ETS_OPERLOGPDELAY_4S   (2L)     /**< @brief 4s operational pDelay period*/
#define ETS_OPERLOGPDELAY_8S   (3L)     /**< @brief 8s operational pDelay period*/
#define ETS_OPERLOGPDELAY_INF  (127L)   /**< @brief Operational pDelay disabled */
#define ETS_OPERLOGPDELAY_MIN  ETS_OPERLOGPDELAY_1S /**< @brief Operational
                                                         logarithmic pdelay
                                                         interval with
                                                         minimum period */
#define ETS_OPERLOGPDELAY_MAX  ETS_OPERLOGPDELAY_8S /**< @brief Operational
                                                         logarithmic pdelay
                                                         interval with
                                                         maximum period */
/** @} */

/**
    @name ETS_InitLogSyncType
    @{
    @brief Allowed values of initial logarithmic sync intervals

*/
typedef int32_t ETS_InitLogSyncType;      /**< @brief typedef for init sync intervals */
#define ETS_INITLOGSYNC_31_25MS   (-5L)   /**< @brief 31.25ms init sync period*/
#define ETS_INITLOGSYNC_62_5MS    (-4L)   /**< @brief 62.5ms init sync period */
#define ETS_INITLOGSYNC_125MS     (-3L)   /**< @brief 125ms init sync period */
#define ETS_INITLOGSYNC_MAX       ETS_INITLOGSYNC_125MS /**< @brief Initial
                                                             logarithmic sync
                                                             interval with
                                                             maximum period */
#define ETS_INITLOGSYNC_MIN       ETS_INITLOGSYNC_31_25MS /**< @brief Initial
                                                               logarithmic sync
                                                               interval with
                                                               minimum period */
/** @} */

/**
    @name ETS_OperLogSyncType
    @{
    @brief Allowed values of operational logarithmic sync intervals

*/
typedef int32_t ETS_OperLogSyncType;    /**< @brief typedef for oper sync intervals */
#define ETS_OPERLOGSYNC_125MS     (-3L) /**< @brief 125ms oper sync period */
#define ETS_OPERLOGSYNC_250MS     (-2L) /**< @brief 250ms oper sync period */
#define ETS_OPERLOGSYNC_500MS     (-1L) /**< @brief 500ms oper sync period */
#define ETS_OPERLOGSYNC_1S        (0L)  /**< @brief 1s oper sync period    */
#define ETS_OPERLOGSYNC_MAX       ETS_OPERLOGSYNC_1S /**< @brief Operational
                                                          logarithmic sync
                                                          interval with
                                                          maximum period */
#define ETS_OPERLOGSYNC_MIN       ETS_OPERLOGSYNC_125MS /**< @brief Operational
                                                             logarithmic sync
                                                             interval with
                                                             minimum period */
/** @} */

/**
    @brief Configuration structure magic number

    Magic number for the configuration structure, containing 'GPTP'
*/
#define ETS_CONFIG_MAGIC_ID         (0x50545047UL)

/**
    @brief Per interface configuration

*/
typedef struct COMP_PACKED _ETS_IntfConfigType {
    uint8_t            hwPortNum;             /**< @brief Hardware port
                                                   corresponding to the
                                                   interface */

    ETS_AdminModeType  asCapable;             /**< @brief 802.1as capable mode*/

    ETS_RoleType       role;                  /**< @brief Port gPTP role */

    ETS_InitLogPdelayType  initLogPdelayInterval; /**< @brief Initial PDelay
                                                   interval in logarithmic base
                                                   2 format*/

    ETS_OperLogPdelayType  operLogPdelayInterval; /**< @brief Operational PDelay
                                                   interval in logarithmic base
                                                   2 format */

    ETS_InitLogSyncType    initLogSyncInterval;   /**< @brief Initial Sync interval
                                                   in logarithmic base 2
                                                   format*/

    ETS_OperLogSyncType    operLogSyncInterval;   /**< @brief Operational Sync
                                                   interval in logarithmic base
                                                   2 format. Only valid for end
                                                   points */

    uint32_t           nbrPropDelay;          /**< @brief Neighbour propagation
                                                   delay in nanoseconds */

    int32_t            nbrRateRatio;          /**< @brief Neighbour rate ratio
                                                   (stored as 2^30 scale factor
                                                   fixed point) */

    uint8_t           syncReceiptTimeout;    /**< @brief Intervals without
                                                   receiving sync before
                                                   declaring loss of sync. Only
                                                   valid in slave mode */

    uint16_t           allowedLostResponses;  /**< @brief Maximum number of
                                                   PDelay Request messages for
                                                   which a valid response is not
                                                   received, above which a port
                                                   is considered to not be
                                                   exchanging peer delay
                                                   messages with its neighbor */
} ETS_IntfConfigType;

/**
    @brief Configuration information received from framework/application

*/
typedef struct COMP_PACKED _ETS_ConfigType {
    uint32_t           magicId;                     /**< @brief Magic number 'GPTP'
                                                         (checked for validity) */

    ETS_AdminModeType  adminMode;                   /**< @brief Global admin mode */

    ETS_ClockModeType  clockMode;                   /**< @brief Clock mode */

    ETS_AdminModeType  boundaryModeEnable;          /**< @brief Indicates whether operating
                                                         as a boundary clock or not in case
                                                         if switch slave mode */
    uint8_t            avnuSyncAbsenceTimeout;      /**< @brief Duration to wait for
                                                         GM to appear on startup
                                                         before operating as proxy.
                                                         Only applicable for bridge
                                                         in slave mode */

    int32_t            gmRateRatio;                 /**< @brief Grandmaster rate
                                                         ratio (stored as 2^30
                                                         scale factor fixed point) */
    uint32_t           numInterfaces;               /**< @brief Number of valid interfaces */
    ETS_IntfConfigType intfCfg[ETS_MAX_INTERFACES]; /**< @brief Per port
                                                         configuration */
} ETS_ConfigType;

#endif /* ETS_CONFIG_H */
/** @} */
