/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_wdtdrv_ifc Interface
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_ifc
    @{
    @section sec_wdt_overview Overview
    @image html wdt_overview.jpg "Watchdog driver overview"
    Watchdog timers are implemented as 32-bit down counters. When counter is
    loaded with the required timeout value and enabled, the watchdog timer
    starts counting down. If the watchdog is not serviced within the timeout
    period (using #WDT_Service interface), chip reset is triggered (for more
    details on the reset sequence, refer to @ref sec_ref_trm).

    @section sec_wdt_fsm State Machine
    Below figure depicts state machine for watchdog driver.
    @image html wdt_state_machine.jpg "Watchdog driver state machine"
    Whenever user/client calls #WDT_SetMode, #WDT_SetTriggerCondition or
    #WDT_Service, the driver moves to #WDT_STATE_BUSY. Once the API processing
    is complete, the driver returns back to #WDT_STATE_IDLE.

    @section sec_wdt_seq Sequence Diagrams
    @image html wdt_seq_diagram_generic.jpg "Sequence diagram"

    @section sec_wdt_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_wdt.dep

    @limitations None

    @file wdt.h
    @brief Watchdog Driver APIs
    This file provides APIs for the Watchdog Driver.
    @version 1.01 Imported from docx
*/

#ifndef WDT_H
#define WDT_H

#include <stdint.h>

/**
    @name Watchdog driver API IDs
    @{
    @brief API IDs for Watchdog driver
*/
#define BRCM_SWARCH_WDT_HWID_TYPE                     (0x00U) /**< @brief #WDT_HwIDType */
#define BRCM_SWARCH_WDT_MODE_TYPE                     (0x01U) /**< @brief #WDT_ModeType */
#define BRCM_SWARCH_WDT_STATE_TYPE                    (0x02U) /**< @brief #WDT_StateType */
#define BRCM_SWARCH_WDT_DISABLE_TYPE                  (0x03U) /**< @brief #WDT_Disable_Type */
#define BRCM_SWARCH_WDT_CFG_TYPE                      (0x04U) /**< @brief #WDT_CfgType */
#define BRCM_SWARCH_WDT_INIT_PROC                     (0x05U) /**< @brief #WDT_Init */
#define BRCM_SWARCH_WDT_SET_MODE_PROC                 (0x06U) /**< @brief #WDT_SetMode */
#define BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC    (0x07U) /**< @brief #WDT_SetTriggerCondition */
#define BRCM_SWARCH_WDT_SERVICE_PROC                  (0x08U) /**< @brief #WDT_Service */
/** @} */

/**
    @brief Watchdog hardware ID type

    Watchdog hardware ID (Shall be less than #WDT_MAX_HW_ID)

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef uint32_t WDT_HwIDType;

/**
    @name WDT_ModeType
    @{
    @brief Mode of the watchdog hardware

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef uint32_t WDT_ModeType;          /**< @brief Watchdog hardware mode type */
#define WDT_MODE_OFF        (0UL)       /**< @brief Watchdog hardware is off */
#define WDT_MODE_ACTIVE     (1UL)       /**< @brief Watchdog hardware is active */
/** @} */

/**
    @name WDT_StateType
    @{
    @brief State of watchdog driver


    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef uint32_t WDT_StateType;         /**< @brief Watchdog state type */
#define WDT_STATE_UNINIT    (0UL)       /**< @brief Watchdog driver is uninitialized */
#define WDT_STATE_IDLE      (1UL)       /**< @brief Watchdog driver is idle */
#define WDT_STATE_BUSY      (2UL)       /**< @brief Watchdog driver is busy */
/** @} */

/**
    @name Watchdog disabling
    @{
    @brief Runtime disabling of watchdog driver

    Indicates whether runtime disabling of watchdog driver is allowed or not.

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef uint32_t WDT_Disable_Type;
#define WDT_DISABLE_ALLOWED     (0UL)   /**< @brief Runtime disabling of
                                             watchdog driver is allowed */
#define WDT_DISABLE_NOT_ALLOWED (1UL)   /**< @brief Runtime disabling of
                                             watchdog driver is not allowed */
/** @} */

/**
    @brief Watchdog driver configuration

    @trace #BRCM_SWREQ_WDT_KERNEL_INTERFACE
*/
typedef struct _WDT_CfgType {
    uint32_t runtimeDisable;            /**< @brief Indicates whether runtime
                                             disabling of watchdog driver is allowed:
                                             - #WDT_DISABLE_ALLOWED or
                                             - #WDT_DISABLE_NOT_ALLOWED */
    WDT_ModeType defaultMode;           /**< @brief Default mode of the watchdog
                                             driver. If default mode is set to
                                             #WDT_ACTIVE_MODE, watchdog timer is
                                             loaded with */
    uint32_t defaultTimeout_ms;         /**< @brief Default timeout value of the
                                             watchdog timer (in milliseconds) */
} WDT_CfgType;

/** @brief Initialize the given watchdog driver.

    This API initializes the given watchdog with the given configurations. If
    the passed hardware ID or configuration is/are invalid, this API returns
    without initializing the watchdog driver.

    @behavior Sync, Re-entrant for different aID

    @pre This API shall be called in #WDT_STATE_UNINIT state.

    @param[in]      aID         Hardware ID
    @param[in]      aConfig     Pointer to watchdog configuration

    @return     void

    @post Upon successfully initializing, watchdog shall transition to
    #WDT_STATE_IDLE.

    @trace  #BRCM_SWREQ_WDT_INIT

    @limitations None
*/
extern void WDT_Init(WDT_HwIDType aID, const WDT_CfgType *const aConfig);

/** @brief Set mode of watchdog

    This API sets the mode of the given watchdog.

    @behavior Sync, Re-entrant for different aID

    @pre This API shall be called in #WDT_STATE_IDLE state.

    @param[in]      aID         Hardware ID
    @param[in]      aMode       Mode to be set<BR>
                                If the watchdog configuration (passed during
                                driver initialization), do not permit runtime
                                disabling of watchdog, and the mode passed is
                                #WDT_MODE_OFF, this API returns
                                #BCM_ERR_INVAL_PARAMS.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Mode set successfully
    @retval     #BCM_ERR_INVAL_STATE    Watchdog driver is not in idle state
                                        (#WDT_STATE_IDLE)
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Runtime disabling is not allowed by
                                        watchdog configuration
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid

    @post Whenever user/client calls #WDT_SetMode, the driver moves to
    #WDT_STATE_BUSY. Once the API processing is complete, the driver returns
    back to #WDT_STATE_IDLE.

    @trace  #BRCM_SWREQ_WDT_DYNAMIC_CONFIG

    @limitations None
*/
extern int32_t WDT_SetMode(WDT_HwIDType aID, WDT_ModeType aMode);

/** @brief Set the watchdog trigger condition

    This API sets the trigger condition (i.e. the timeout value of the watchdog).

    @behavior Sync, Re-entrant for different aID

    @pre This API shall be called in #WDT_STATE_IDLE state.

    @param[in]      aID         Hardware ID
    @param[in]      aTimeout_ms New timeout value (in milliseconds) to be set.
                                If timeout value is zero, and the watchdog is
                                already active (#WDT_ACTIVE_MODE), the watchdog
                                timer expires immediately.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Timeout value set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid timeout (more than what the watchdog hardware can support)
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid
    @retval     #BCM_ERR_INVAL_STATE    Watchdog driver is not in idle state
                                        (#WDT_STATE_IDLE)
    @retval     #BCM_ERR_UNINIT         Driver not initialized

    @post Whenever user/client calls #WDT_SetTriggerCondition, the driver moves
    to #WDT_STATE_BUSY. Once the API processing is complete, the driver returns
    back to #WDT_STATE_IDLE.

    @trace  #BRCM_SWREQ_WDT_DYNAMIC_CONFIG

    @limitations None
*/
extern int32_t WDT_SetTriggerCondition(WDT_HwIDType aID, uint32_t aTimeout_ms);

/** @brief Service Watchdog

    This interface services the watchdog and restarts the watchdog timer.

    @behavior Sync, Re-entrant for different aID

    @pre TBD

    @param[in]      aID         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watchdog is serviced
    @retval     #BCM_ERR_UNINIT         Driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Watchdog ID is invalid

    @post TBD

    @trace  #BRCM_SWREQ_WDT_SERVICE

    @limitations None
*/
extern int32_t WDT_Service(WDT_HwIDType aID);

#endif  /* WDT_H */
/** @} */
