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
    @defgroup grp_flsmgr Flash Manager
    @ingroup grp_system_sw

    @addtogroup grp_flsmgr
    @{
    @section sec_flsmgr_overview Overview
    @image html flsmgr_overview.jpg "Flash Manager Layers"
    Flash manager is thin interface layer which supports queuing of flash
    operation requests from the multiple clients for same flash controller.
    Flash operation requests are queued to flash manager message queue  and upon
    completion, client is signaled. Upon signaling, client shall check the
    status of the request by calling #FLSMGR_GetOpResult.

    Flash manager support following flash operations:
    - Flash initialization (with flash read lane configuration)
    - Flash read
    - Flash write
    - Flash erase
    - Flash compare
    - Flash blank check
    - Flash de-initialization

    @section sec_flsmgr_fsm State Machine
    @image html flsmgr_state_machine.jpg "Flash manager state machine"

    Flash manager states:
    - #FLSMGR_STATE_UNINIT: Flash manager uninitialized or de-initialized after
    initialization.
    - #FLSMGR_STATE_IDLE: Flash manager is initialized and currently no
    operation request in progress.
    - #FLSMGR_STATE_BUSY: Flash manager is busy in processing operation request

    @note Any operation request queued after flash manager de-initialization
    request and before flash manager is de-initialized (Flash manager moves to
    #FLSMGR_STATE_UNINIT) shall be marked failed with #FLSMGR_OP_RESULT_FAILED.

    @section sec_flsmgr_seq Sequence Diagrams
    This section describes use case diagram for flash manager.

    Note that APIs like #FLSMGR_Erase, #FLSMGR_Write, #FLSMGR_Compare,
    #FLSMGR_BlankCheck shall follow same sequence as #FLSMGR_Read.

    @image html flsmgr_seq_diagram_gen.jpg "Flash manager use-case "

    @section sec_flsmgr_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |  NA                                               |
    | Memory Bandwidth  |  NA                                               |
    | HW Utilization    |  NA                                               |

    @limitations None

    @file flsmgr.h
    @brief Application interface for Flash Manager
    This header file contains the interfaces for Flash Manager
    @version 0.52 Imported from docx
*/

#ifndef FLSMGR_H
#define FLSMGR_H

#include <msg_queue.h>
#include <flash.h>
#include <osil/flash_osil.h>

/**
    @name Flash Manager API IDs
    @{
    @brief API IDs for Flash manager
*/
#define BRCM_SWARCH_FLSMGR_DEV_ID_TYPE          (0x00U) /**< @brief #FLSMGR_DevIDType */
#define BRCM_SWARCH_FLSMGR_ADDR_TYPE            (0x01U) /**< @brief #FLSMGR_AddrType */
#define BRCM_SWARCH_FLSMGR_LEN_TYPE             (0x02U) /**< @brief #FLSMGR_LenType */
#define BRCM_SWARCH_FLSMGR_MSG_HDR_TYPE         (0x03U) /**< @brief #FLSMGR_MsgHdrType */
#define BRCM_SWARCH_FLSMGR_STATE_TYPE           (0x04U) /**< @brief #FLSMGR_StateType */
#define BRCM_SWARCH_FLSMGR_OP_RESULT_TYPE       (0x05U) /**< @brief #FLSMGR_OpResultType */
#define BRCM_SWARCH_FLSMGR_CFG_TYPE             (0x06U) /**< @brief #FLSMGR_CfgType */
#define BRCM_SWARCH_FLSMGR_INIT_PROC            (0x07U) /**< @brief #FLSMGR_Init */
#define BRCM_SWARCH_FLSMGR_DEINIT_PROC          (0x08U) /**< @brief #FLSMGR_DeInit */
#define BRCM_SWARCH_FLSMGR_GET_STATE_PROC       (0x09U) /**< @brief #FLSMGR_GetState */
#define BRCM_SWARCH_FLSMGR_CANCEL_PROC          (0x0AU) /**< @brief #FLSMGR_Cancel */
#define BRCM_SWARCH_FLSMGR_READ_PROC            (0x0BU) /**< @brief #FLSMGR_Read */
#define BRCM_SWARCH_FLSMGR_WRITE_PROC           (0x0CU) /**< @brief #FLSMGR_Write */
#define BRCM_SWARCH_FLSMGR_ERASE_PROC           (0x0DU) /**< @brief #FLSMGR_Erase */
#define BRCM_SWARCH_FLSMGR_COMPARE_PROC         (0x0EU) /**< @brief #FLSMGR_Compare */
#define BRCM_SWARCH_FLSMGR_BLANK_CHECK_PROC     (0x0FU) /**< @brief #FLSMGR_BlankCheck */
#define BRCM_SWARCH_FLSMGR_GET_OP_RESULT_PROC   (0x10U) /**< @brief #FLSMGR_GetOpResult */
/** @} */

/**
    @name FLSMGR_DevIDType
    @{
    @brief Flash manager device type

    @trace #BRCM_SWREQ_FLSMGR_DEV_ID
*/
typedef FLASH_HwIDType  FLSMGR_DevIDType;   /**< @brief Flash manager device
                                                 type */
/** @} */

/**
    @name FLSMGR_AddrType
    @{
    @brief Flash manager address type

    This type is used for describing flash address for flash operation.

    @trace #BRCM_SWREQ_FLSMGR_ADDR
*/
typedef FLASH_AddrType FLSMGR_AddrType; /**< @brief Flash manager address type */
/** @} */

/**
    @name FLSMGR_LenType
    @{
    @brief Flash manager length type

    This type is used for describing the length (in bytes) parameter for flash
    operation.

    @trace #BRCM_SWREQ_FLSMGR_LEN
*/
typedef FLASH_LenType FLSMGR_LenType;   /**< @brief Flash manager length type */
/** @} */

/**
    @name FLSMGR_MsgHdrType
    @{
    @brief Flash manager message header type

    This is used to retrieve the unique message header for the flash operation
    request (message) queued with flash manager.

    @trace #BRCM_SWREQ_FLSMGR_MSG_HDR
*/
typedef MSGQ_MsgHdrType FLSMGR_MsgHdrType;   /**< @brief Flash manager message header
                                         type */
/** @} */

/**
    @name FLSMGR_StateType
    @{
    @brief Flash manager state types

    @trace #BRCM_SWREQ_FLSMGR_STATE
*/
typedef uint32_t FLSMGR_StateType;      /**< @brief Flash manager state type */
#define FLSMGR_STATE_UNINIT     (0UL)   /**< @brief Flash manager state uninit */
#define FLSMGR_STATE_IDLE       (1UL)   /**< @brief Flash manager state idle */
#define FLSMGR_STATE_BUSY       (2UL)   /**< @brief Flash manager state busy */
/** @} */

/**
    @name FLSMGR_OpResultType
    @{
    @brief Flash manager operation result type

    @trace #BRCM_SWREQ_FLSMGR_OP_RESULT
*/
typedef uint32_t FLSMGR_OpResultType;       /**< @brief Operation result type */
#define FLSMGR_OP_RESULT_SUCCESS    (1UL)   /**< @brief Operation result success */
#define FLSMGR_OP_RESULT_FAILED     (2UL)   /**< @brief Operation result failed */
#define FLSMGR_OP_RESULT_PENDING    (3UL)   /**< @brief Operation result pending */
/** @} */

/**
    @brief Flash manager configuration type

    @trace #BRCM_SWREQ_FLSMGR_CFG
*/
typedef struct {
    const FLASH_CfgTblType *const flashCfg; /**< @brief Pointer to Flash driver
                                                 configuration */
} FLSMGR_CfgType;

/** @brief Initialize flash manager

    Interface to initialize flash manager. This interface also initializes flash
    driver with given configurations and configures the flash read lanes.

    @behavior Async, Re-entrant (for differnt aID)

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aID         Flash device ID
    @param[in]      aConfig     Flash manager configuration
    @param[in]      aClientMask Client signal mask, this signal is used by flash
                                manager to notify completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash Initialization request
                                        successfully queued.
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     Others                  As returned from #FLASH_Init
    @retval     #BCM_ERR_UNKNOWN        Flash is not in #FLASH_STATUS_IDLE state
                                        after initialization
    @retval     #BCM_ERR_INVAL_STATE    Flash manager is not in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig->flashCfg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_INIT

    @limitations None
*/
extern int32_t FLSMGR_Init(FLSMGR_DevIDType aID,
                           const FLSMGR_CfgType *const aConfig,
                           const uint32_t aClientMask,
                           const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief De-initialize flash manager

    Interface to de-initialize Flash manager. Flash manager processes all the
    pending requests in the queue before processing de-initialization request.
    On completion of this request, client shall be notified by signaling using
    @c aClientMask.

    All requests queued after queuing of de-initialization request and before
    flash manager is de-initialized are marked failed with operation result as
    #FLSMGR_OP_RESULT_FAILED.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aID         Flash manager device ID
    @param[in]      aClientMask Client signal mask, this signal is used by flash
                                manager to notify completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-initialization request successfully
                                        queued.
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_DEINIT

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_DeInit(FLSMGR_DevIDType aID,
                     const uint32_t aClientMask,
                     const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Retrieve flash manager state

    Interface to retrieve flash manager state.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[out]     aState      Pointer to retrieve flash manager state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash manager state retrieved
                                        successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aState is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_GET_STATE

    @limitations None
*/
extern int32_t FLSMGR_GetState(FLSMGR_DevIDType aID,
                               FLSMGR_StateType * const aState);

/** @brief Cancel flash operation request

    Interface to cancel flash operation requests already queued with flash
    manager. If the request to be cancelled is already in progress, then the
    request shall complete as usual without cancellation.

    If the request to be cancelled is already completed, then operation result
    shall be retrieved in the interface call.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aOpResult   Pointer to retrieve operation result, in case
                                the operation request is already processed
    @param[out]     aMsgHdr     Unique message header retrieved during operation
                                request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request already processed cancellation
                                        not possible. Result is retrieved in
                                        @c aOpRessult.
    @retval     Others                  As returned from #FLSMGR_TryCancelMsg
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_CANCEL

    @limitations Request shall only be cancelled by the original requestor
    lient.
    @limitations This interface shall be called from task context.

*/
extern int32_t FLSMGR_Cancel(FLSMGR_DevIDType aID,
                             FLSMGR_OpResultType * const aOpResult,
                             const FLSMGR_MsgHdrType * const aMsgHdr);

/** @brief Flash read request

    Interface to queue flash read request to flash manager. For successfully
    queued request, flash read shall be performed from aAddr for @c aLen bytes.
    On completion of this request, client shall be signaled using @c
    aClientMask. Client shall only wait for signal from flash manager for
    successfully queued request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aAddr       Flash read address
    @param[out]     aBuf        Pointer to output data buffer
    @param[in]      aLen        Length in bytes
    @param[in]      aClientMask Client signal mask. This signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash read request successfully queued
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in #FLSMGR_STATE_UNINIT
                                        state
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aLen is zero
    @retval     #BCM_ERR_INVAL_PARAMS   aBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aIInvalid aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_READ

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_Read(FLSMGR_DevIDType aID,
                           FLSMGR_AddrType aAddr,
                           uint8_t * const aBuf,
                           FLSMGR_LenType aLen,
                           const uint32_t aClientMask,
                           const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Flash write request

    Interface to queue flash write request to flash manager. For successfully
    queued request, flash write shall be performed from @c aAddr for @c aLen
    bytes. On completion of this request, client shall be signaled using
    @c aClientMask.

    Client shall only wait for signal from flash manager for successfully queued
    request.


    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aAddr       Flash write address
    @param[in]      aBuf        Pointer to data buffer to be written
    @param[in]      aLen        Length in bytes
    @param[in]      aClientMask Client signal mask. This signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash write request successfully queued
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aLen is zero
    @retval     #BCM_ERR_INVAL_PARAMS   @c aBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID


    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_WRITE

    @limitations This interface shall be called from task context
*/
extern int32_t FLSMGR_Write(FLSMGR_DevIDType aID,
                           FLSMGR_AddrType aAddr,
                           const uint8_t * const aBuf,
                           FLSMGR_LenType aLen,
                           const uint32_t aClientMask,
                           const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Flash erase request

    Interface to queue flash erase request to flash manager. For successfully
    queued request, flash erase shall be performed from @c aAddr for @c aLen
    bytes. On completion of this request, client shall be signaled using
    @c aClientMask.

    Client shall only wait for signal from flash manager for successfully queued
    request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aAddr       Flash erase address
    @param[in]      aLen        Length in bytes to be erased
    @param[in]      aClientMask Client signal mask. This signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash erase request successfully queued
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aLen is zero
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_ERASE

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_Erase(FLSMGR_DevIDType aID,
                           FLSMGR_AddrType aAddr,
                           FLSMGR_LenType aLen,
                           const uint32_t aClientMask,
                           const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Flash compare request

    Interface to queue flash compare request to flash manager. For successfully
    queued request, flash compare shall be performed from @c aAddr for @c aLen
    bytes. On completion of this request, client shall be signaled using
    @c aClientMask.

    Client shall only wait for signal from flash manager for successfully queued
    request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aAddr       Flash compare address
    @param[in]      aBuf        Pointer to data buffer to be compared
    @param[in]      aLen        Length in bytes
    @param[in]      aClientMask Client signal mask. This signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash compare request successfully
                                        queued
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aLen is zero
    @retval     #BCM_ERR_INVAL_PARAMS   @c aBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_COMPARE

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_Compare(FLSMGR_DevIDType aID,
                               FLSMGR_AddrType aAddr,
                               const uint8_t * const aBuf,
                               FLSMGR_LenType aLen,
                               const uint32_t aClientMask,
                               const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Flash blank check request

    Interface to queue flash blank check request to flash manager. For
    successfully queued request, flash blank check shall be performed from
    @c aAddr for @c aLen bytes. On completion of this request, client shall be
    signaled using @c aClientMask.

    Client shall only wait for signal from flash manager for successfully queued
    request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aAddr       Flash blank check address
    @param[in]      aLen        Length in bytes
    @param[in]      aClientMask Client signal mask. This signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Flash blank check request successfully
                                        queued
    @retval     Others                  As returned from #FLSMGR_SendMsg
    @retval     #BCM_ERR_UNINIT         Flash manager is in
                                        #FLSMGR_STATE_UNINIT state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aLen is zero
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_BLANK_CHECK

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_BlankCheck(FLSMGR_DevIDType aID,
                                FLSMGR_AddrType aAddr,
                                FLSMGR_LenType aLen,
                                const uint32_t aClientMask,
                                const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Get flash operation result

    Interface to retrieve result for the flash operation request queued
    previously and associated with unique @c aMsgHdr.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[out]     aOpResult   Flash operation result
    @param[in]      aMsgHdr     Pointer to unique message header for the
                                operation request, this message header shall be
                                used to:
                                - Query the result of the operation using
                                  #FLSMGR_GetOpResult or
                                - Cancel the operation using #FLSMGR_Cancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Result successfully retrieved
    @retval     Others                  As returned from #FLSMGR_RecvMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aOpResult is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @trace #BRCM_SWREQ_FLSMGR_GET_OP_RESULT

    @limitations Result shall only be retrieved by the original requestor
    client.
    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_GetOpResult(FLSMGR_DevIDType aID,
                                  FLSMGR_OpResultType * const aOpResult,
                                  const FLSMGR_MsgHdrType * const aMsgHdr);

#endif /* FLSMGR_H */
/** @} */
