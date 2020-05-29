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

 2. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_imgl_ifc Interface
    @ingroup grp_imgl

    @addtogroup grp_imgl_ifc
    @{

    @section sec_comp_seq Sequence Diagrams
    @tagseq comp_seq_diagram.svg "Sequence diagram"
    @startseq
    group {label = "client"; color = "#EFFFFF";  Client;};
    group {label = "imgl"; color = "#FFEFFF"; ImageLoader;ServerIf};
    group {label = "server"; color = "#FFFFEF"; ModeServer;};
    loop {
        Client -> ImageLoader [label = "IMGL_LoadImg"];
        ImageLoader -> ServerIf [label = "<MODE>_LoadImg"];
        ServerIf -> ModeServer [label = "signal", rightnote = "Image Loading Start"] {
        Client <-- ServerIf;
            Client <-- ModeServer [label = "notify", rightnote = "Image Loading Complete"];
        };
        Client =>  ImageLoader [label = "IMGL_GetStatus", return="return"]{
            ImageLoader =>  ServerIf [label = "<MODE>_GetStatus"];
        };
    };
    @endseq


    @file imgl.h
    @brief Image Loader Interface

    This header file contains the interface functions for image loader.
    @version 0.1 Initial Version
*/

#ifndef IMGL_H
#define IMGL_H

#include <stdint.h>
#include <bcm_err.h>
#include <msg_queue.h>
#include <nvm_pt.h>

/**
    @name Image Loader Architecture IDs
    @{
    @brief Architecture IDs for Image Loader
*/
#define BRCM_SWARCH_IMGL_LOAD_MODE_TYPE    (0x00U)   /**< @brief #IMGL_LoadModeType  */
#define BRCM_SWARCH_IMGL_LOAD_STATUS_TYPE  (0x01U)   /**< @brief #IMGL_LoadStatusType*/
#define BRCM_SWARCH_IMGL_LOAD_IMG_PROC     (0x02U)   /**< @brief #IMGL_LoadImg       */
#define BRCM_SWARCH_IMGL_GET_STATUS_PROC   (0x03U)   /**< @brief #IMGL_GetStatus     */
#define BRCM_SWARCH_IMGL_LOAD_IMG_TBL_PROC (0x04U)   /**< @brief #IMGL_GetImgTbl     */
/** @} */

/**
    @name Image Loader Modes
    @{
    @brief Image Loader Modes

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
*/
typedef uint32_t IMGL_LoadModeType; /**< @brief Load Mode Type    */
#define IMGL_LOAD_MODE_NVM  (1UL)   /**< @brief Load Mode NVM     */
#define IMGL_LOAD_MODE_RFS  (2UL)   /**< @brief Load Mode TFTP    */
#define IMGL_LOAD_MODE_IPC  (3UL)   /**< @brief Load Mode IPC/SPI */
/** @} */

/**
    @brief Image Load status type

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
 */
typedef struct _IMGL_LoadStatusType {
    IMGL_LoadModeType   mode;   /**< @brief load mode      */
    uint32_t            size;   /**< @brief filled size    */
    const MSGQ_MsgHdrType *hdr; /**< @brief message handle  */
} IMGL_LoadStatusType;

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
extern const ITBL_Type *IMGL_GetImgTbl(uint8_t aImgTblID);

/** @brief Request to load an image

    This is an asynchronous API which sends a message to appropriate Server.
    Upon the completion, client is signaled.

    Client shall call @ref IMGL_GetStatus API to get the status of the image load
    request.

    @behavior Async, Re-entrant

    @pre None

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
    @param[out]     aStatus     Status type structure to contain mode of operation
                                and message handle from server.
                                Shall be used by the client in #IMGL_GetStatus
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to Load image is accepted and
                                        shall be processed asynchronously. Upon
                                        completion of the request, client shall
                                        be signaled using @c aClientMask
    @return     #BCM_ERR_NODEV          No Method to load image
    @retval     #BCM_ERR_INVAL_PARAMS   aStatus is NULL or
                                        aBufSize is zero or
                                        aAddr is NULL

    @post None

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @limitations This API shall be called from the task context.
*/
extern int32_t IMGL_LoadImg(uint16_t aImgID, uint8_t *const aAddr, uint32_t aOffset,
                           uint32_t aBufSize, const uint32_t aClientMask,
                           IMGL_LoadStatusType* const aStatus);

/** @brief Get the status of Load request

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aStatus     Status type structure

    @return     #BCM_ERR_OK             Load completed successfully
    @return     #BCM_ERR_BUSY           Load is in progress
    @return     #BCM_ERR_NOT_FOUND      Image ID not found in aStatus->mode
    @return     #BCM_ERR_INVAL_STATE    Loader (of that mode) was not initialized
    @return     #BCM_ERR_DATA_INTEG     Data integrity check failed
    @return     #BCM_ERR_NOMEM          Insufficient memory
    @return     #BCM_ERR_INVAL_PARAMS   aStatus is NULL
                                        or aStatus->hdr is NULL
                                        or aStatus->mode is invalid

    @post TBD

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @limitations This API shall be called from the task context.
*/
extern int32_t IMGL_GetStatus(IMGL_LoadStatusType *const aStatus);

#endif /* IMGL_H */

/** @} */
