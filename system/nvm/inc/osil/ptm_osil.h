/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_ptm_il Integration Guide
    @ingroup grp_ptm

    @addtogroup grp_ptm_il
    @{
    @section sec_ptm_integration Integration Guide
    This section provides information about how to integrate Partition table
    manager into the system. This section describes all the integration
    interfaces required by PTM and how to implement them (pseudo code).

    PTM requires a Server task to be implemented as part of the system
    integration. This task shall be referred as @c TaskPTMServer in this
    document. This task will be signaled when there is a new #PTM_LoadImg
    request or when flash manager operation is done. PTM initialization is
    initiated by TaskPTMServer by calling #PTM_Init. TaskPTMServer call
    #PTM_EventHandler upon signaling of completion of flash manager operation or
    when a new #PTM_LoadImg request is received.

    @section sec_ptm_il_seq Sequence Diagram
    Figure below shows the sequence diagram for #PTM_LoadImg which involves
    posting a message to @c TaskPTMServer, processing of the message by PTM,
    reading the flash and then signaling client upon completion of the load
    image:
    @image html ptm_seq_il_diagram_load_img.jpg "PTM Integration"

    @file ptm_osil.h
    @brief PTM Integration Interfaces
    This header file contains the integration interface for PTM.
    @version 1.21 Imported from docx
*/

#ifndef PTM_OSIL_H
#define PTM_OSIL_H

#include <ptm.h>
#include <flsmgr.h>

/**
    @name PTM Integration Interface IDs
    @{
    @brief Integration Interface IDs for PTM
*/
#define BRCM_SWARCH_PTM_IL_STATE_TYPE                   (0x40U) /**< @brief #PTM_StateType */
#define BRCM_SWARCH_PTM_IL_FLASH_ID_TYPE                (0x41U) /**< @brief #PTM_FlashIDType */
#define BRCM_SWARCH_PTM_IL_MSGQ_CMD_TYPE                (0x42U) /**< @brief #PTM_CmdType */
#define BRCM_SWARCH_PTM_IL_LOOK_TBL_ENT                 (0x43U) /**< @brief #PTM_LookupTblEntType */
#define BRCM_SWARCH_PTM_IL_CFG_TYPE                     (0x44U) /**< @brief #PTM_CfgType */
#define BRCM_SWARCH_PTM_IL_MSG_TYPE                     (0x45U) /**< @brief #PTM_MsgType */
#define BRCM_SWARCH_PTM_IL_CFG_GLOBAL                   (0x46U) /**< @brief #PTM_Cfg */
#define BRCM_SWARCH_PTM_IL_INIT_PROC                    (0x47U) /**< @brief #PTM_Init */
#define BRCM_SWARCH_PTM_IL_DEINIT_PROC                  (0x48U) /**< @brief #PTM_DeInit */
#define BRCM_SWARCH_PTM_IL_READ_FLASH_PROC              (0x49U) /**< @brief #PTM_ReadFlash */
#define BRCM_SWARCH_PTM_IL_ERASE_FLASH_PROC             (0x4AU) /**< @brief #PTM_EraseFlash */
#define BRCM_SWARCH_PTM_IL_WRITE_FLASH_PROC             (0x4BU) /**< @brief #PTM_WriteFlash */
#define BRCM_SWARCH_PTM_IL_FLASH_GET_OPS_RESULT_PROC    (0x4CU) /**< @brief #PTM_FlashGetOpsResult */
#define BRCM_SWARCH_PTM_IL_SEND_MSG_PROC                (0x4DU) /**< @brief #PTM_SendMsg */
#define BRCM_SWARCH_PTM_IL_RECV_MSG_PROC                (0x4EU) /**< @brief #PTM_RecvMsg */
#define BRCM_SWARCH_PTM_IL_PROCESS_MSG_PROC             (0x4FU) /**< @brief #PTM_ProcessMsg */
#define BRCM_SWARCH_PTM_IL_GET_MSG_PROC                 (0x50U) /**< @brief #PTM_GetMsg */
#define BRCM_SWARCH_PTM_IL_GET_STATE_PROC               (0x51U) /**< @brief #PTM_GetState */
#define BRCM_SWARCH_PTM_IL_EVENT_HANDLER_PROC           (0x52U) /**< @brief #PTM_EventHandler */
/** @} */

/**
    @name PTM_StateType
    @{
    @brief State of the PTM

    @trace #BRCM_SWREQ_PTM_IL_STATE
*/
typedef uint32_t PTM_StateType;     /**< @brief PTM state type */
#define PTM_STATE_UNINIT    (0UL)   /**< @brief PTM is uninitialized */
#define PTM_STATE_IDLE      (1UL)   /**< @brief PTM is initialized */
#define PTM_STATE_BUSY      (2UL)   /**< @brief PTM is busy */
#define PTM_STATE_ERROR     (3UL)   /**< @brief PTM is in error state */
/** @} */

/**
    @name PTM_FlashIDType
    @{
    @brief PTM Flash device ID type

    @trace #BRCM_SWREQ_PTM_IL_FLASH_ID
*/
typedef FLSMGR_DevIDType PTM_FlashIDType;   /**< @brief PTM Flash device ID
                                                 type */
/** @} */

/**
    @name PTM_CmdType
    @{
    @brief PTM MsgQ command

    @trace #BRCM_SWREQ_PTM_IL_MSGQ_CMD
*/
typedef uint32_t PTM_CmdType;           /**< @brief PTM MsgQ command type */
#define PTM_CMD_LOAD_IMG        (0x1UL) /**< @brief Load image command */
#define PTM_CMD_LOAD_USERDATA   (0x2UL) /**< @brief Load user data command */
#define PTM_CMD_WRITE_USERDATA  (0x3UL) /**< @brief Write user data command */
#define PTM_CMD_ERASE_USERDATA  (0x4UL) /**< @brief Erase user data command */
/** @} */

/**
    @brief Lookup table entry structure

    Lookup table entry structure consisting of flash id and address of the flash
    (flashAddr) where Partition table is searched by PTM.

    @trace #BRCM_SWREQ_PTM_IL_LOOK_TBL_ENT
*/
typedef struct {
    PTM_FlashIDType flashID;    /**< @brief Flash device ID */
    uint32_t flashAddr;         /**< @brief Address of the flash where Partition
                                     table can be found */
} PTM_LookupTblEntType;

/**
    @brief PTM configuration

    @trace #BRCM_SWREQ_PTM_IL_CFG
*/
typedef struct {
    PTM_AddrType ptLoadAddr;    /**< @brief Address where PT table is loaded. If
                                     this is not NULL, PTM assumes that a valid
                                     partition table is already loaded at this
                                     address and can be used by PTM. If
                                     @c ptLoadAddr is NULL, PTM searches the
                                     partition table in flash using the lookup
                                     table provided as part of this structure. */
    const PTM_LookupTblEntType *const lookupTbl;    /**< @brief Lookup table
                                     address (Partition table manager searches
                                     for a valid partition table in the flash
                                     using this table) */
    uint32_t lookupTblSize;     /**< @brief Number of entries in lookup table */
} PTM_CfgType;

/**
    @brief Message Type for PTM Message Queue

    @trace #BRCM_SWREQ_PTM_IL_MSG
*/
typedef struct {
    uint8_t       pid;          /**< @brief Partition ID */
    PTM_ImgIDType imgID;        /**< @brief Image ID */
    PTM_CmdType cmd;            /**< @brief Command ID */
    PTM_AddrType imgLoadAddrBuf;/**< @brief Address buffer where image shall be
                                     loaded by PTM (shall be set to address
                                     passed in #PTM_LoadImg API) */
    uint32_t 	offset;         /**< @brief offset from where the image need to be loaded */
    PTM_SizeType inLen;         /**< @brief Length of the buffer (shall be set
                                     to length passed in #PTM_LoadImg API) */
    PTM_SizeType imgActualLen;  /**< @brief Actual length of the image (actual
                                     length of the image loaded by PTM). */
    int32_t result;      /**< @brief Result of the last request */
} PTM_MsgType;

/**
    @brief Pointer to PTM configuration

    @trace #BRCM_SWREQ_PTM_IL_CFG_GLOBAL
*/
extern const PTM_CfgType *PTM_Cfg;

/** @brief Initialize partition manager

    This is synchronous API and it does not search for the Partition table
    during initialization.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aConfig     Configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Initialization of PTM successfully
                                        initiated
    @retval     Others                  As returned from #PTM_ReadFlash
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid configuration
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid PT magic
    @retval     #BCM_ERR_INVAL_STATE    PTM not in uninitialized state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aConfig is NULL

    @return     void

    @post TBD

    @note This interface shall be used by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_INIT

    @limitations This interface shall be called from task context.

    @snippet /flsmgr_osil.c Usage of PTM_Init
*/
int32_t PTM_Init(const PTM_CfgType *const aConfig);

/** @brief De-initialize partition manager

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note To be used

    @trace  #BRCM_SWREQ_PTM_IL_DEINIT

    @limitations None
*/
void PTM_DeInit(void);

/** @brief Read request

    This API shall queue read request to underlying flash manager for @c aLen
    number of bytes from address @c aAddr.

    Completion of read request shall be notified by signaling the TaskPTMServer.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aFlashID    Flash Device ID
    @param[in]      aAddr       Address of the flash memory to read from
    @param[in]      aLen        Number of bytes to read from flash
    @param[out]     aData       Pointer to the output data buffer
    @param[out]     aMsgHdr     Unique message header retrieved during flash
                                read request

    Return values are documented in reverse-chronological order
    @retval      Others                 As retruned from #FLSMGR_Read

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_READ_FLASH

    @limitations None
*/
int32_t PTM_ReadFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      uint8_t *const aData,
                      const MSGQ_MsgHdrType **const aMsgHdr);

/** @brief Erase request

    This API shall queue an erase request to underlying flash manager for
    @c aLen number of bytes from address @c aAddr.

    Completion of read request shall be notified by signaling the TaskPTMServer.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aFlashID    Flash Device ID
    @param[in]      aAddr       Address of the flash memory to start erasing
                                from
    @param[in]      aLen        Number of bytes to be erased from flash
    @param[out]     aMsgHdr     Unique message header retrieved during flash
                                erase request

    Return values are documented in reverse-chronological order
    @retval      Others                 As returned from #FLSMGR_Erase

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_ERASE_FLASH

    @limitations None
*/
int32_t PTM_EraseFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      const MSGQ_MsgHdrType **const aMsgHdr);

/** @brief Write request

    This API shall queue a write request to the underlying flash manager for
    @c aLen number of bytes to address @c aAddr.

    Completion of read request shall be notified by signaling the TaskPTMServer.


    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aFlashID    Flash device ID
    @param[in]      aAddr       Address of the flash/NVM memory to write to
    @param[in]      aLen        Number of bytes to write to flash
    @param[in]      aData       Pointer to the data to be written
    @param[out]     aMsgHdr     Unique message header retrieved during flash
                                write request

    Return values are documented in reverse-chronological order
    @retval     Others                  As returned from #FLSMGR_Write

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_WRITE_FLASH

    @limitations None
*/
int32_t PTM_WriteFlash(PTM_FlashIDType aFlashID,
                      uint32_t aAddr,
                      uint32_t aLen,
                      uint8_t *const aData,
                      const MSGQ_MsgHdrType **const aMsgHdr);

/** @brief Result of the last flash operation.

    Returns the result of the last flash operation.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aFlashID    Flash Device ID
    @param[in]      aMsgHdr     Unique message header retrieved during
                                #PTM_ReadFlash request

    Return values are documented in reverse-chronological order
    @retval     #FLSMGR_OpResultType    Result of last flash operation

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_FLASH_GET_OPS_RESULT

    @limitations None
*/
FLSMGR_OpResultType PTM_FlashGetOpsResult(PTM_FlashIDType aFlashID,
                                          const MSGQ_MsgHdrType *const aMsgHdr);

/** @brief Send a message to TaskPTMServer

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMsg        Pointer to message
    @param[in]      aClientMask Client signal mask, client shall wait on this
                                mask for request completion notification
    @param[out]     aHdr        Pointer to unique message header associated with
                                request

    Return values are documented in reverse-chronological order
    @retval     Others                  As returned from #MSGQ_SendMsg

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_SEND_MSG

    @limitations None
*/
extern int32_t PTM_SendMsg(PTM_MsgType * const aMsg,
                           const uint32_t aClientMask,
                           const MSGQ_MsgHdrType** const aHdr);

/** @brief Receive response

    Receive response of the request associated with @c aHdr.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHdr        Pointer to unique message header associated with
                                request
    @param[out]     aMsg        Pointer to output message


    Return values are documented in reverse-chronological order
    @retval     Others                  As returned from #MSGQ_RecvMsg

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_RECV_MSG

    @limitations None
*/
extern int32_t PTM_RecvMsg(const MSGQ_MsgHdrType *const aHdr,
                           PTM_MsgType *const aMsg);

/** @brief Process PTM message

    This API shall be called by integration layer to process PTM message queue
    messages.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMsg        Message to be processed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     BCM_ERR_BUSY            PTM is busy processing previous message
    @retval     BCM_ERR_NOT_FOUND       Image not found
    @retval     BCM_ERR_DATA_INTEG      Data integrity failed
    @retval     BCM_ERR_UNKNOWN         Unknown error while processing the
                                        message

    @post TBD

    @note This interface shall be called by the integrator to process any PTM
    message received.

    @trace  #BRCM_SWREQ_PTM_IL_PROCESS_MSG

    @limitations None

    @snippet ptm_osil.c Usage of PTM_ProcessMsg
*/
int32_t PTM_ProcessMsg(PTM_MsgType *const aMsg);

/** @brief Returns the server message

    Returns the server message indexed by @c aIdx. This interface is used by the
    @ref grp_msgq core.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aIdx        Index of the message

    Return values are documented in reverse-chronological order
    @retval     void*                   void pointer to message

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_GET_MSG

    @limitations None
*/
void *PTM_GetMsg(uint32_t aIdx);

/** @brief Returns the state of the PTM

    @behavior Sync, Re-entrant

    @pre TBD

    @param          void

    Return values are documented in reverse-chronological order
    @retval     #PTM_StateType          State of PTM

    @post TBD

    @note This interface shall be used by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_GET_STATE

    @limitations None
*/
extern PTM_StateType PTM_GetState(void);

/** @brief Handle PTM events

    This API handles the msgQ and flash manager operation done event. This API
    shall be called by TaskPTMServer when there is a new message or when flash
    manager operation is done

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note This interface shall be used by the integrator.

    @trace  #BRCM_SWREQ_PTM_IL_EVENT_HANDLER

    @limitations None
*/
extern void PTM_EventHandler(void);

#endif  /* PTM_OSIL_H */
/** @} */
