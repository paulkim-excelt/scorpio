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

#ifndef HOST_IMGL_H
#define HOST_IMGL_H

#include <stdint.h>
#include <host_system.h>

/**
    @defgroup grp_host_imgl Host Image Loader Handlers
    @ingroup grp_host

    @addtogroup grp_host_imgl
    @{

    @file host_imgl.h
    @brief Host Image Loader Handler API
    This header file contains the host interface functions for Host Image Loader

    @version 1.0 Initial Version

*/

/** @brief Image Loader Group Init

    This API is meant to initialize IMGL group handler

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      aTar             TAR file address
    @param[in]      aSize            TAR File Size

    @return     #BCM_ERR_OK             Notification handled successfully
    @return     #BCM_ERR_INVAL_PARAMS   aTar is NULL or aSize is zero
                                        or aTar does not contain a valid TAR file

    @post Host Image Loader is initialized

    @limitations None
*/
extern int32_t HOST_ImglInit(const uint8_t *tar, uint32_t size);

/** @brief Image Loader Group Bootloader info

    @behavior Sync, Non Re-entrant

    @pre Host Image loader should be in initialized state

    @param[out]     aOffset         Offset within TAR file where Bootloader is present
    @param[out]     aSize           Size of bootloader

    @return     #BCM_ERR_OK             Bootloader image is found
    @return     #BCM_ERR_INVAL_STATE    Host Image Loader is not in initialized state
    @return     #BCM_ERR_INVAL_PARAMS   aOffset is NULL of aSize is NULL

    @post None

    @limitations None
*/
extern int32_t HOST_ImglGetBootloader(uint32_t* aOffset, uint32_t* aSize);

/** @brief Host Image Loader Group Get Parsed Image Headers

    @behavior Sync, Non Re-entrant

    @pre Host Image loader should be in initialized state

    @param[out]     aInfo           Image Headers information structure

    @return     #BCM_ERR_OK             Bootloader image is found
    @return     #BCM_ERR_INVAL_STATE    Host Image Loader is not in initialized state
    @return     #BCM_ERR_INVAL_PARAMS   aInfo is NULL

    @post None

    @limitations None
*/
extern int32_t HOST_ImglGetParsedImgHdrs(SYS_UpdateExecCmdType* aInfo);

/** @brief Image Loader Group Notification Handler

    This API is meant to handle asynchronous notifications sent with IMGL group
    from target

    @behavior Sync, Non Re-entrant

    @pre Host Image loader should be in initialized state

    @param[in]      currentSlave    Slave ID
    @param[in]      comp            Component ID
    @param[in]      notificationId  Notification ID
    @param[in]      msg             Message buffer pointer
    @param[in]      size            Size of the message buffer "msg" in bytes

    @return     #BCM_ERR_OK             Notification handled successfully
    @return     #BCM_ERR_INVAL_STATE    Host Image Loader is not in initialized state
    @return     #BCM_ERR_INVAL_PARAMS   Invalid comp
    @return     #BCM_ERR_INVAL_PARAMS   Invalid notificationId
    @return     #BCM_ERR_INVAL_PARAMS   Invalid msg pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid size

    @post None

    @limitations None
*/
extern int32_t HOST_ImglNotificationHandler(uint32_t currentSlave,
                BCM_CompIDType comp, uint32_t notificationId,
                uint8_t *msg, uint32_t size);


/** @brief HOST Image Loader Deinit

    This API is meant to deinitialize Host IMGL group Handler

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      void

    @return     #BCM_ERR_OK    Deinit successful

    @post Host Image Loader is deinitialized

    @limitations None
*/
extern int32_t HOST_ImglDeInit();

#endif /* HOST_IMGL_H */

/** @} */

