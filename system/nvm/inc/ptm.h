/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_ptm Partition Table Manager
    @ingroup grp_system_sw

    @addtogroup grp_ptm
    @{
    @section sec_ptm_overview Overview
    @image html ptm_overview.jpg "PTM Layers"

    Partition table acts as TOC (table of contents) for flash based system which
    holds the information about the flash partitions, image location etc.
    Partition table is placed at known location in the flash. This is read by
    Partition Table Manager to locate an image in the flash and load them on
    request by client.

    Partition table manager support following features:
    -# Search for a valid partition table in flash or use the Partition table
       already loaded by BootROM
    -# Validate the partition table integrity before using it
    -# Loading of the image from flash to the specified RAM location
    -# Data integrity check of the image using CRC32/Checksum
    -# Find a valid redundant copy (which passes integrity check if enabled) and
       load it to specified RAM location

    @section sec_ptm_fsm State Machine
    Figure below shows the state machine of Partition table manager:
    @image html ptm_state_machine.jpg "PTM state machine"

    PTM states:
    - @b PTM_STATE_UNINIT: Uninitialized state. This is the state of PTM when
         not initialized. In this state, #PTM_LoadImg request is still taken and
         queued. It is processed only after PTM moves to @b PTM_STATE_IDLE.
    - @b PTM_STATE_IDLE: Valid PT is found and PTM is initialized. It’s ready to
         process the #PTM_LoadImg requests.
    - @b PTM_STATE_BUSY: PTM is processing #PTM_LoadImg request.
    - @b PTM_STATE_ERROR: Error state. PTM enters into this state if no valid PT
         is found in the flash. In this state, PTM does not process any
         requests.

    @section sec_ptm_seq Sequence Diagrams
    Below figure shows the sequence diagram for #PTM_LoadImg API. #PTM_LoadImg
    API is an asynchronous API, that sends a message to Server (refer to
    @ref sec_comp_integration for details about Server) using Message Queue.
    @todo Add reference to MsgQ.

    Client shall wait on event mask which was passed through #PTM_LoadImg API
    for the completion of the message.

    The Server processes the message and loads the image to the requested
    address (see #PTM_LoadImg).  Upon completion of the request, client is
    signaled.

    Upon receiving the signal, client shall check the status of request by
    calling #PTM_GetStatus API.

    @image html ptm_seq_diagram_use_case.jpg "Sequence diagram"


    @section sec_ptm_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |  NA                                               |
    | Memory Bandwidth  |  NA                                               |
    | HW Utilization    |  NA                                               |


    @limitations None

    @file ptm.h
    @brief Partition Table Manager Application Interface
    This header file contains the application programmer interface for Partition
    Table Manager
    @version 1.21 Imported from docx
*/

#ifndef PTM_H
#define PTM_H

#include <msg_queue.h>
#include <osil/imgl_osil.h>
/**
    @name Component API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_PTM_ADDR_TYPE               (0x00U) /**@brief #PTM_AddrType */
#define BRCM_SWARCH_PTM_SIZE_TYPE               (0x01U) /**@brief #PTM_SizeType */
#define BRCM_SWARCH_PTM_IMGID_TYPE              (0x02U) /**@brief #PTM_ImgIDType */
#define BRCM_SWARCH_PTM_LOAD_IMG_PROC           (0x04U) /**@brief #PTM_LoadImg */
#define BRCM_SWARCH_PTM_LOAD_USER_DATA_PROC     (0x05U) /**@brief #PTM_LoadUserData */
#define BRCM_SWARCH_PTM_WRITE_USER_DATA_PROC    (0x06U) /**@brief #PTM_WriteUserData */
#define BRCM_SWARCH_PTM_ERASE_USER_DATA_PROC    (0x07U) /**@brief #PTM_EraseUserData */
#define BRCM_SWARCH_PTM_GET_STATUS_PROC         (0x08U) /**@brief #PTM_GetStatus */
/** @} */

/**
    @name PTM_AddrType
    @{
    @brief PTM Address type

    @trace #BRCM_SWREQ_PTM_ADDR
*/
typedef uint8_t *PTM_AddrType;      /**< @brief PTM Address type */
/** @} */

/**
    @name PTM_SizeType
    @{
    @brief PTM Size type (0… UINT32_MAX)

    @trace #BRCM_SWREQ_PTM_SIZE
*/
typedef uint32_t PTM_SizeType;      /**< @brief PTM Size type */
/** @} */

/**
    @name PTM_ImgIDType
    @{
    @brief Partition table image ID types

    @trace #BRCM_SWREQ_PTM_IMGID
*/
typedef uint16_t PTM_ImgIDType;     /**< @brief Partition table image ID type */
/** @} */

/** @brief Request to load an image from flash

    This is an asynchronous API which sends a message to the Server (refer to
    @ref sec_ptm_integration for details about Server). Upon the completion,
    client is signaled.

    Client shall call #PTM_GetStatus API to get the status of the image load
    request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aImgID      Image ID
    @param[out]     aAddr       Address where image shall be loaded. It is
                                client's responsibility to ensure that this
                                address is valid until the load request is
                                completed by the PTM (upon the completion of the
                                load request, client shall be signaled using
                                @c aClientMask)
    @param[in]      aOffset     Offset, within the image, from where to load from
    @param[in]      aBufSize    Size of the image
    @param[in]      aClientMask Client event mask
    @param[out]     aHdr        Message handle (Pointer to message header).
                                Shall be used by the client in #PTM_GetStatus
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to Load image is accepted and
                                        shall be processed asynchronously. Upon
                                        completion of the request, client shall
                                        be signaled using @c aClientMask
    @retval     Others                  As retuned from #PTM_SendMsg
    @retval     #BCM_ERR_INVAL_STATE    PTM is not initialized correctly (could be empty flash)
    @retval     #BCM_ERR_INVAL_PARAMS   @c aHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aBufSize is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aAddr is NULL

    @post TBD

    @trace #BRCM_SWREQ_PTM_LOAD_IMG

    @limitations This API shall be called from the task context.
*/
extern int32_t PTM_LoadImg(uint8_t aPID,
                           PTM_ImgIDType aImgID,
                           PTM_AddrType const aAddr,
                           uint32_t aOffset,
                           PTM_SizeType aBufSize,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr);

/** @brief Request to load user data image from flash

    This is an asynchronous API which sends a message to the Server (refer
    @ref sec_ptm_integration for details about Server). Upon the completion,
    client is signaled.

    Client shall call #PTM_GetStatus API to get the status of the user data load
    request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aImgID      Image ID
    @param[out]     aAddr       Address where user data image shall be loaded.
                                It is client's responsibility to ensure that
                                this address is valid until the load request is
                                completed by the PTM. Uupon the completion of
                                the load request, caller shallll be signaled
                                using @c aClientMask
    @param[in]      aBufSize    Length of the memory where the user data image
                                is to be loaded
    @param[in]      aClientMask Client event mask, with which caller is signaled
                                upon completion of this load request
    @param[in]      aHdr        Message handle (Pointer to message header).
                                Shall be used by the client in #PTM_GetStatus
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to load user data image is
                                        accepted and shall be processed
                                        asynchronously. Upon completion of
                                        request, client is signaled
    @retval     Others                  As retuned from #PTM_SendMsg
    @retval     #BCM_ERR_INVAL_STATE    PTM is not initialized correctly (could be empty flash)
    @retval     #BCM_ERR_INVAL_PARAMS   @c aHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aBufSize is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aAddr is NULL

    @post TBD

    @trace #BRCM_SWREQ_PTM_LOAD_USER_DATA

    @limitations This API shall be called only from a task context.
*/
extern int32_t PTM_LoadUserData(PTM_ImgIDType aImgID,
                           PTM_AddrType const aAddr,
                           PTM_SizeType aBufSize,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr);

/** @brief Request to write to user data image in flash

    This is an asynchronous API which sends a message to the Server (refer
    @sec_ptm_integration for details about Server). Upon completion, client is
    signaled.

    Client shall call #PTM_GetStatus API to get the status of the write user
    data request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aImgID      Image ID
    @param[in]      aAddr       Address of image to be written
    @param[in]      aBufSize    Size of the image
    @param[in]      aClientMask Client event mask, with which caller is signaled
                                upon completion of this write request
    @param[in]      aHdr        Message handle (Pointer to message header).
                                Shall be used by the client in #PTM_GetStatus
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to write user data image is
                                        accepted and is processed
                                        asynchronously. Upon completion of
                                        request, client is signaled
    @retval     Others                  As retuned from #PTM_SendMsg
    @retval     #BCM_ERR_INVAL_STATE    PTM is not initialized correctly (could be empty flash)
    @retval     #BCM_ERR_INVAL_PARAMS   @c aHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aBufSize is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aAddr is NULL

    @post TBD

    @trace #BRCM_SWREQ_PTM_WRITE_USER_DATA

    @limitations This API shall be called only from a task context.
*/
extern int32_t PTM_WriteUserData(PTM_ImgIDType aImgID,
                           PTM_AddrType const aAddr,
                           PTM_SizeType aBufSize,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr);

/** @brief Request to erase the given user data image in flash

    This is an asynchronous API which sends a message to the Server (refer
    @ref sec_ptm_integration details about Server). Upon completion, client is
    signaled with @c aClientMask.

    Client shall call #PTM_GetStatus API to get the status of the erase user
    data request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aImgID      Image ID
    @param[in]      aClientMask Client event mask, with which caller is signaled
                                upon completion of this erase request
    @param[out]     aHdr        Message handle (Pointer to message header).
                                Shall be used by the client in #PTM_GetStatus
                                API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to erase user data image is
                                        accepted and is processed
                                        asynchronously. Upon completion of
                                        request, client is signaled
    @retval     Others                  As retuned from #PTM_SendMsg
    @retval     #BCM_ERR_INVAL_STATE    PTM is not initialized correctly (could be empty flash)
    @retval     #BCM_ERR_INVAL_PARAMS   @c aHdr is NULL

    @post TBD

    @trace #BRCM_SWREQ_PTM_ERASE_USER_DATA

    @limitations This API shall be called only from a task context.
*/
extern int32_t PTM_EraseUserData(PTM_ImgIDType aImgID,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr);

/** @brief Get the status of the PTM request

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aHdr        Handle of the Message
    @param[out]     aImgSize    Size of the image loaded. This field is valid
                                only if this API returns #BCM_ERR_OK.

    @return     #BCM_ERR_OK             Load completed successfully
    @return     #BCM_ERR_BUSY           Load is in progress
    @return     #BCM_ERR_NOT_FOUND      Image ID not found in aStatus->mode
    @return     #BCM_ERR_INVAL_STATE    Loader (of that mode) was not initialized
    @return     #BCM_ERR_DATA_INTEG     Data integrity check failed
    @return     #BCM_ERR_NOMEM          Insufficient memory

    @post TBD

    @trace #BRCM_SWREQ_PTM_GET_STATUS

    @limitations This API shall be called from the task context.
*/
extern int32_t PTM_GetStatus(const MSGQ_MsgHdrType *const aHdr,
                                    PTM_SizeType *const aImgSize);

/**
*/
extern const IMGL_ModeHandlerType PTM_ImageHandler;

#endif /* PTM_H */
/** @} */
