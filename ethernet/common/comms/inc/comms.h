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
    @defgroup grp_comms_notify Comms Notifications Interface
    @ingroup grp_comms_sw

    @addtogroup grp_comp_list
    @{

    @file comms.h
    @brief Subsystem Notifications Interface

    @version 0.1 Initial version
*/

#ifndef COMMS_H
#define COMMS_H

#include <stdint.h>
#include <bcm_err.h>
#ifdef ENABLE_ETH_SWITCH
#ifdef ENABLE_ETS
#include <ets.h>
#endif
#include <mcu_ext.h>
#endif

/**
    @brief Invalid port number
*/
#define COMMS_INVALID_PORT     (0xFFFFFFFFUL)

/**
    @name Subsystem IDs
    @{
    @brief Different subsystems in the System

    @trace #BRCM_ARCH_COMP_STATE_TYPE #BRCM_REQ_COMP_GROUPMACRO
*/
typedef uint32_t COMMS_SubsystemIDType;  /**< @brief Subsystem types              */
#define COMMS_SUBSYSTEM_IPC    (0UL)     /**< @brief IPC Subsystem                */
#define COMMS_SUBSYSTEM_ETS    (1UL)     /**< @brief ETS Subsystem                */
#define COMMS_SUBSYSTEM_MAX    (2UL)     /**< @brief MAX Subsystem  ID            */
/** @} */

/**
    @name Subsystem IDs
    @{
    @brief Different subsystems in the System

    @trace #BRCM_ARCH_COMP_STATE_TYPE #BRCM_REQ_COMP_GROUPMACRO
*/
typedef uint32_t COMMS_SubsystemStateType;      /**< @brief Subsystem types         */
#define COMMS_SUBSYSTEM_STATE_UNINIT  (0UL)   /**< @brief Uninitialized state     */
#define COMMS_SUBSYSTEM_STATE_INIT    (1UL)   /**< @brief Initialized state       */
#define COMMS_SUBSYSTEM_STATE_ERROR   (2UL)   /**< @brief Error state             */
/** @} */


/** @brief Notify Major state change in Subsystem

    Function to be invoked by Subsystems when there is a major change in state.
    For example, when subsystem moves from UNINIT to INIT, Any state to Error, any state
    to UNINIT.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID        Subsystem ID
    @param[in]      aState     State that Subsystem is entering into

    @return void

    @post System Task may take appropriate action based on state entered

    @trace  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
extern void COMMS_NotifyState(COMMS_SubsystemIDType aID, COMMS_SubsystemStateType aState);

/** @brief Get Notification from Subsytem

    This function should be invoked by system task only. Other tasks must not invoke this API.
    Based on notification, System task will take appropriate action, including a reboot.

    @behavior Sync, Non-Reentrant

    @pre None

    @param[out]      aID        Subsystem ID
    @param[out]      aState     Sybsystem state

    Return values are documented in reverse-chronological order
    @return     #BCM_ERR_OK             When a valid notification is returned
    @return     #BCM_ERR_NOMEM          When No new notifications are present
    @return     #BCM_ERR_INVAL_PARAMS   When aID is NULL or aState is NULL

    @post System Task may take appropriate action based on state entered by subsystem

    @trace  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
extern int32_t COMMS_GetNotification(COMMS_SubsystemIDType *aID, COMMS_SubsystemStateType *aState);

#ifdef ENABLE_ETH_SWITCH
#ifdef ENABLE_ETS
extern void COMMS_UpdateETSConfig(const MCU_ExtendedInfoType *const aStackingInfo,
                                  ETS_ConfigType *const aCfg,
                                  uint32_t aUnifiedToLocal);
#endif
/** @brief Convert input port number to local port number

    This converts the input port number to local port number

    @behavior Sync, Reentrant

    @param[in]       aStackingInfo          Stacking information
    @param[in]       aPort                  Input unified port number
    @param[out]      aIsStackingPort        Local port is the stacking port

    @retval          #COMMS_INVALID_PORT    aPort is invalid
    @retval          Local port number
*/
uint32_t COMMS_ConvertToLocalPort(const MCU_ExtendedInfoType *const aStackingInfo,
                                  uint32_t aPort,
                                  uint32_t *const aIsStackingPort);

/** @brief Convert input port number to unified port number

    This converts the input port number to unified port number

    @behavior Sync, Reentrant

    @param[in]       aStackingInfo       Stacking information
    @param[in]       aPort               Input local port number
    @param[out]      aIsStackingPort     Input port is a stacking port

    @retval          #COMMS_INVALID_PORT aPort is invalid
    @retval          Unified port number
*/
uint32_t COMMS_ConvertToUnifiedPort(const MCU_ExtendedInfoType *const aStackingInfo,
                                    uint32_t aPort,
                                    uint32_t *const aIsStackingPort);

/** @brief Convert input port mask to unified port mask

    This converts the input port mask to unified port mask

    @behavior Sync, Reentrant

    @param[in]       aStackingInfo       Stacking information
    @param[in]       aPortMask           Local port mask

    @retval          #COMMS_INVALID_PORT aPortMask is invalid
    @retval          Unified port bit mask
*/
uint16_t COMMS_ConvertToUnifiedPortMask(const MCU_ExtendedInfoType *const aStackingInfo,
                                        uint16_t const aPortMask);
#endif

#endif /* COMMS_H */

/** @} */
