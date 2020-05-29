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
    @defgroup grp_wds_il Integration Interfaces
    @ingroup grp_wds

    @addtogroup grp_wds_il
    @{

    @file wds_osil.h
    @brief Integration Interface File
    This header file contains integration interface functions for watchdog service
    @version 0.1 Initial Version
*/

#ifndef WDS_OSIL_H
#define WDS_OSIL_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <wds.h>

/**
    @name Watchdog Service Architecture IDs
    @{
    @brief Architecture IDs for Watchdog Service
*/
#define BRCM_SWARCH_WDS_INIT_PROC        (0x10U)   /**< @brief #WDS_Init        */
#define BRCM_SWARCH_WDS_MONITOR_PROC     (0x11U)   /**< @brief #WDS_Monitor     */
#define BRCM_SWARCH_WDS_DEINIT_PROC      (0x12U)   /**< @brief #WDS_DeInit      */
/** @} */

/** @brief WDS Initialization

    This API initializes the watchdog service contexts for all supervision
    entities. Watchdog Timer is initialized with appropriate configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]    aSE             Pointer to an array of Supervision entities
    @param[in]       aCnt            Number of supervision entities in the array

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    if aSE is NULL
    @retval          #BCM_ERR_OK              On success

    @post Watchdog Timer is initialized

    @trace #BRCM_SWREQ_WDS_INIT
*/
int32_t WDS_Init(const WDS_SEType* const aSE, uint32_t aCnt);

/** @brief Monitor all WDS Supervision entities

    Monitors for errors in all enabled Supervision entities.
    Checks Frequency of graph execution. If total number of errors for each
    supervision entity is more than maximum allowed errors, watchdog timer
    will not be serviced. Callbacks will be raised for all SEs where errors
    are detected.

    @behavior Sync, Non-reentrant

    @pre WDS shall be initialized

    @param[inout]    aSE             Pointer to an array of Supervision entities
    @param[in]       aCnt            Number of supervision entities in the array

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    if aSE is NULL
    @retval          #BCM_ERR_INVAL_STATE     WDS was not initialized
    @retval          #BCM_ERR_MAX_ATTEMPS     WDS encountered maximum errors
    @retval          #BCM_ERR_CUSTOM          WDS detected errors in enabled SEs
    @retval          #BCM_ERR_OK              On success

    @post If MAX_ATTEMPS is returned, Watchdog may reset the chip before next cycle.

    @trace #BRCM_SWREQ_WDS_SYSTEM
*/
int32_t WDS_Monitor(const WDS_SEType* const aSE, uint32_t aCnt);

/** @brief WDS De-Initialization

    This API de-initializes the watchdog service and stops monitoring
    of all supervised entities. This de-initializes Watchdog Timer.

    @behavior Sync, Non-reentrant

    @pre WDS shall be in initialized state

    @param[inout]    aSE             Pointer to an array of Supervision entities
    @param[in]       aCnt            Number of supervision entities in the array

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_INVAL_PARAMS    if aSE is NULL
    @retval          #BCM_ERR_OK              On success

    @post WDS will be moved to deinitialized state

    @trace #BRCM_SWREQ_WDS_INIT
*/
int32_t WDS_DeInit(const WDS_SEType* const aSE, uint32_t aCnt);


#endif /* WDS_OSIL_H */
/** @} */
