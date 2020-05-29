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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/** @defgroup grp_imgl_il Integration Guide
    @ingroup grp_imgl

    @addtogroup grp_imgl_il
    @{
    Components which would like to support loading images over varous interfaces,
    shall expose a structure of @ref IMGL_ModeHandlerType, and shall be integrated
    into Image Loader according to the static priority of preference for the platform.

    @file imgl_osil.h
    @brief ImageLoader Integration Interfaces
    @version 0.1 Initial Version
*/

#ifndef IMGL_OSIL_H
#define IMGL_OSIL_H

#include <imgl.h>

/**
    @name Image Loader Integration Interface IDs
    @{
    @brief Integration Interface IDs for Image Loader

*/
#define BRCM_SWARCH_IMGL_LOAD_IMG_TYPE      (0x41U)   /**< @brief #IMGL_LoadImgType     */
#define BRCM_SWARCH_IMGL_GET_STATUS_TYPE    (0x42U)   /**< @brief #IMGL_GetStatusType   */
#define BRCM_SWARCH_IMGL_MODE_HANDLER_TYPE  (0x43U)   /**< @brief #IMGL_ModeHandlerType */
#define BRCM_SWARCH_IMGL_GET_IMG_TBL_TYPE   (0x44U)   /**< @brief #IMGL_GetImgTblType   */
/** @} */

/** @brief get an image table

    This is a synchronous API which retrieves Image Table.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aImgTblID   8-bit Image Table ID

    @return     NULL if aImgTblID is invalid or not found or a valid Image Table pointer

    @post None

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @limitations This API shall be called from the task context.
*/
typedef const ITBL_Type * (*IMGL_GetImgTblType)(uint8_t aImgTblID);

/** @brief Request to load an image

    This is an asynchronous API which sends a message to appropriate Server.
    Upon the completion, client is signaled.

    Client shall call @ref IMGL_GetStatus API to get the status of the image load
    request.

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aPID      8-bit Partition ID
    @param[in]      aImgID      16-bit Image ID
    @param[out]     aAddr       Address where image shall be loaded. It is
                                client's responsibility to ensure that this
                                address is valid until the load request is
                                completed by the server (upon the completion
                                of the load request, client shall be signaled
                                using @c aClientMask)
    @param[in]      aOffset     Offset, within the image, from where to load
    @param[in]      aBufSize    Size of the image
    @param[in]      aClientMask Client event mask
    @param[out]     aHdr        message handle from server.
                                Shall be used by the client in #IMGL_GetStatusType
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to Load image is accepted and
                                        shall be processed asynchronously. Upon
                                        completion of the request, client shall
                                        be signaled using @c aClientMask
    @return     #BCM_ERR_NODEV          No Method to load image
    @retval     #BCM_ERR_INVAL_PARAMS   aStatus is NULL or
                                        aBufSize is NULL or
                                        aAddr is NULL

    @post None

    @note To be implemented/used.

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @limitations This API shall be called from the task context.
*/
typedef int32_t (*IMGL_LoadImgType)(uint8_t aPID, uint16_t aImgID, uint8_t *const aAddr, uint32_t aOffset,
                           uint32_t aBufSize, const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Get the status of Load request

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aHdr       Message Handle
    @param[out]  aImgSize   Filled size

    @return     #BCM_ERR_OK             Load completed successfully
    @return     #BCM_ERR_BUSY           Load is in progress
    @return     #BCM_ERR_NOT_FOUND      Image ID not found in aStatus->mode
    @return     #BCM_ERR_INVAL_STATE    Loader (of that mode) was not initialized
    @return     #BCM_ERR_DATA_INTEG     Data integrity check failed
    @return     #BCM_ERR_NOMEM          Insufficient memory

    @post TBD

    @note To be implemented/used.

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @limitations This API shall be called from the task context.
*/
typedef int32_t (*IMGL_GetStatusType)(const MSGQ_MsgHdrType *const aHdr,
                                    uint32_t *const aImgSize);

/**
    @brief Image Loader Function table for a specific Mode

    Individual modes (like NVM, TFTP etc) shall implement a function
    table of this type to be integrated into Image Loader.

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
 */
typedef struct _IMGL_ModeHandlerType {
    IMGL_LoadModeType   mode;      /**< @brief Mode for this Image Loader instance                   */
    IMGL_GetImgTblType  getImgTbl; /**< @brief get Image Table function pointer of a specific mode   */
    IMGL_LoadImgType    loadImg;   /**< @brief load Image function pointer of a specific mode        */
    IMGL_GetStatusType  getStatus; /**< @brief get status function pointer of a specific mode        */
} IMGL_ModeHandlerType;

#endif /* IMGL_OSIL_H */

/** @} */
