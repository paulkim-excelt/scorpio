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
    @defgroup grp_comp_list Component List
    @ingroup grp_system_sw

    @addtogroup grp_comp_list
    @{

    @file system.h
    @brief APIs to switch to priviledged mode and list of components.

    This header file describes the APIs for switching to Privileged mode using SVC instruction in ARM Cortex.
    This header file also lists the component IDs, magics and SVC numbers assigned to components

    @version 1.0 Changelist1
*/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>
#include <svc.h>

/**
    @name System Config IDs
    @{
    @brief PT Image IDs for system configuration

    @trace #BRCM_ARCH_COMP_SETTING_MACRO #BRCM_REQ_COMP_SETTING
*/
#define BRCM_PP_CODEC_CONFIG    (0xBC10UL)  /**< @brief BRCM PP codec Config    */
#define BRCM_PP_MP_CONFIG       (0xBC11UL)  /**< @brief BRCM MP safety Config   */
#define BRCM_PP_SP_CONFIG       (0xBC12UL)  /**< @brief BRCM SP safety Config   */
#define BRCM_COMMS_CONFIG       (0xBC10U)   /**< @brief BRCM Comms Config       */
#define BRCM_ETS_CONFIG         (0xBC20UL)  /**< @brief BRCM ETS Config         */
#define BRCM_CFP_CONFIG         (0xBC30UL)  /**< @brief BRCM CFP Config         */
/** @} */

/**
    @name Subsystem IDs
    @{
    @brief Different subsystems in the System

    @trace #BRCM_ARCH_COMP_STATE_TYPE #BRCM_REQ_COMP_GROUPMACRO
*/
typedef uint32_t SYS_SubsystemIDType;  /**< @brief Subsystem types              */
#define SYS_SUBSYSTEM_NVM    (0UL)   /**< @brief Non-Volatile Memory Subsystem  */
#define SYS_SUBSYSTEM_COMMS  (1UL)   /**< @brief Communication Subsystem        */
#define SYS_SUBSYSTEM_MAX    (2UL)   /**< @brief MAX Subsystem  ID              */
/** @} */

/**
    @name Subsystem IDs
    @{
    @brief Different subsystems in the System

    @trace #BRCM_ARCH_COMP_STATE_TYPE #BRCM_REQ_COMP_GROUPMACRO
*/
typedef uint32_t SYS_SubsystemStateType;    /**< @brief Subsystem types         */
#define SYS_SUBSYSTEM_STATE_UNINIT  (0UL)   /**< @brief Uninitialized state     */
#define SYS_SUBSYSTEM_STATE_INIT    (1UL)   /**< @brief Initialized state       */
#define SYS_SUBSYSTEM_STATE_ERROR   (2UL)   /**< @brief Error state             */
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
extern void SYS_NotifyState(SYS_SubsystemIDType aID, SYS_SubsystemStateType aState);

/** @brief Get Notification from Subsytem

    This function should be invoked by system task only. Other tasks must not invoke this API.
    This function also updates state of all subsystems into internal state variables for a quicker
    query. Based on notification, System task will take appropriate action, including a reboot.

    @behavior Sync, Non-Reentrant

    @pre None

    @param[in]       aID        Subsystem ID

    Return values are documented in reverse-chronological order
    @return     state of subsystem aID as @ref SYS_SubsystemStateType

    @post System Task may take appropriate action based on state entered by subsystem

    @trace  #BCM_REQ_COMP_FUNCTION0

    @limitations None
*/
extern SYS_SubsystemStateType SYS_GetState(SYS_SubsystemIDType aID);


#endif /* SYSTEM_H */

/** @} */
