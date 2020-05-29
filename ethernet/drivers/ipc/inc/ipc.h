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

/**
    @defgroup grp_ipcdrv_ifc Interface
    @ingroup grp_ipcdrv

    @addtogroup grp_ipcdrv_ifc
    @{
    @section sec_ipc_overview Overview
    The IPC component consists of an API and an underlying driver as depicted in
    figure below:
    @image html ipc_overview.jpg "Block Diagram"

    @section sec_ipc_fsm State Machine
    Below figure shows valid states and transitions of IPC driver. The same may
    be used as a reference while designing/developing Client/Application.

    States and descriptions are as below:
    - UNINIT:<BR>
      After reset, the default state of IPC would be uninitialized state. IPC
      also  would enter this state after De-initialization sequence is executed.
    - INIT:<BR>
      IPC would be initialized and messages can be sent and received when in
      INIT state.

    @image html ipc_state_machine.jpg "IPC State Machine"

    @section sec_ipc_seq Sequence Diagrams
    NA

    @section sec_ipc_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   TBD                                             |

    @includedoc drivers_ipc.dep

    @limitations None

    @file ipc.h
    @brief Application Programmer Interface for IPC driver
    This header file contains the APIs for IPC driver.
    @version 0.20 Imported from docx
*/

#ifndef IPC_H
#define IPC_H

#include <bcm_err.h>
#include "system.h"

/**
    @name IPC  API IDs
    @{
    @brief API IDs for IPC driver
*/
#define BRCM_ARCH_IPC_MSG_LOG2_SIZE_MACRO   (0x00U) /**< @brief #IPC_MSG_LOG2_SIZE */
#define BRCM_ARCH_IPC_MSG_HDR_SIZE_MACRO    (0x01U) /**< @brief #IPC_MSG_HDR_SIZE */
#define BRCM_ARCH_IPC_MSG_MAXSZ_MACRO       (0x02U) /**< @brief #IPC_MSG_MAXSZ */
#define BRCM_ARCH_IPC_INIT_PROC             (0x04U) /**< @brief #IPC_Init */
#define BRCM_ARCH_IPC_DEINIT_PROC           (0x05U) /**< @brief #IPC_Deinit */
#define BRCM_ARCH_IPC_SEND_PROC             (0x06U) /**< @brief #IPC_Send */
#define BRCM_ARCH_IPC_RECV_PROC             (0x07U) /**< @brief #IPC_Receive */
#define BRCM_ARCH_IPC_PAUSE_PROC            (0x08U) /**< @brief #IPC_Pause */
#define BRCM_ARCH_IPC_RESUME_PROC           (0x09U) /**< @brief #IPC_Resume */
/** @} */

/**
    @brief Size of internal messages header

    16 bytes of header

    @trace #BRCM_ARCH_IPC_MSG_HDR_SIZE_MACRO #BRCM_REQ_IPC_MSG_HDR_SIZE
*/
#define IPC_MSG_HDR_SIZE            (16UL)

/**
 * @brief Maximum size of the IPC buffer (including the header)
 *
 * Maximum size of the IPC buffer in power of 2 format (=2^16 = 64KB)
 */
#define IPC_BUF_LOG2_MAX_SIZE       (0xFU)

/**
 * @brief Maximum number of IPC messages (slots)
 *
 *  Maximum number of IPC messages in power of 2 format
 *  (=2^3 = 8)
 */
#define IPC_BUF_LOG2_MAX_CNT        (0x3U)

typedef uint32_t IPC_CmdRespMagicType;
#define IPC_CMDRESPMAGIC_CMD        (0xa5a5a5a5UL)
#define IPC_CMDRESPMAGIC_RESP       (0x5a5a5a5aUL)

typedef struct _IPC_MsgHdrType {
    IPC_CmdRespMagicType    magic;
    uint32_t                checksum;
    uint32_t                cmd;
    uint32_t                len;
} IPC_MsgHdrType;

/**
 * @brief IPC channel ID type
 */
typedef uint8_t IPC_ChannIDType;
#define IPC_CHANNID_INVALID   (0xFFUL)

/**
 * @brief IPC channel mode type
 *
 * Each IPC channel either works in master or slave mode
 * if Channel is configured in master mode, it will be
 * using the bus interface to read/write to IPC registers
 *
 */
typedef uint8_t IPC_ChannModeType;
#define IPC_CHANN_MODE_SLAVE    (1UL)   /**< @brief IPC channel is in slave mode */
#define IPC_CHANN_MODE_MASTER   (2UL)   /**< @brief IPC channel is in mastre mode */

/**
 * @brief IPC bus hardware ID type
 */
typedef uint32_t IPC_BusHwIDType;

/**
 * @brief IPC bus type
 */
typedef uint8_t IPC_BusType;
#define IPC_BUS_MEMMAP  (0UL)   /**< @brief Memory mapped bus */
#define IPC_BUS_SPI     (1UL)   /**< @brief SPI bus */
#define IPC_BUS_PCIE    (2UL)   /**< @brief PCIE bus */

typedef uint32_t IPC_AccWidthType;
#define IPC_ACCESS_WIDTH_8  (1UL)
#define IPC_ACCESS_WIDTH_16 (2UL)
#define IPC_ACCESS_WIDTH_32 (3UL)
#define IPC_ACCESS_WIDTH_64 (4UL)

/**
 * @brief IPC bus configuration type
 */
typedef union _IPC_BusConfigType {
    uint32_t    speed;
} IPC_BusConfigType;

/**
 * @brief IPC Bus Init function type
 */
typedef void (*IPC_BusInitFnType) (IPC_BusHwIDType aID, const IPC_BusConfigType *const aConfig);

/**
 * @brief IPC Bus De-Init function type
 */
typedef void (*IPC_BusDeInitFnType) (IPC_BusHwIDType aID);

/**
 * @brief IPC bus function read type
 */
typedef int32_t (*IPC_BusXferReadFnType) (IPC_BusHwIDType aID, uint16_t slaveID, uint32_t aAddr, uint8_t *const aData, uint32_t aLen, IPC_AccWidthType aWidth);

/**
 * @brief IPC bus function write type
 */
typedef int32_t (*IPC_BusXferWriteFnType) (IPC_BusHwIDType aID, uint16_t slaveID, uint32_t aAddr, uint8_t *const aData, uint32_t aLen, IPC_AccWidthType aWidth);

/**
 * @brief IPC bus function table type
 */
typedef struct _IPC_BusFnTblType {
    IPC_BusInitFnType       init;
    IPC_BusDeInitFnType     deInit;
    IPC_BusXferReadFnType   read;
    IPC_BusXferWriteFnType  write;
} IPC_BusFnTblType;

/**
 * @brief IPC bus info type
 */
typedef struct _IPC_BusInfoType {
    uint8_t             hwID;
    IPC_BusType         busType;
    uint16_t            slaveID;
    const IPC_BusFnTblType    *const fnTbl;
    IPC_BusConfigType   config;
} IPC_BusInfoType;

/**
 * @brief IPC channel configuration type
 */
typedef struct _IPC_ChannCfgType {
    IPC_ChannIDType     ID;           /**< @brief IPC Channel ID */
    IPC_ChannModeType   mode;         /**< @brief Mode of the IPC */
    uint8_t             sizeLog2;     /**< @brief Size of the message in log2 (size = 2^sizeLog2) */
    uint8_t             cntLog2;      /**< @brief count of the message (slots) in log2 */
    IPC_BusInfoType     busInfo;      /**< @brief Bus information */
} IPC_ChannCfgType;

/** @brief Initialize IPC driver

    This API intializes and configures IPC driver.

    @behavior Sync, Non-reentrant

    @param[in]  aID    IPC Channel ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_STATE    IPC driver is not yet initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameter

    @post If successfully initialized, the IPC driver shall move to INIT state

    @trace  #BRCM_ARCH_IPC_INIT_PROC  #BRCM_REQ_IPC_INIT

    @limitations None
*/
extern void IPC_Init(IPC_ChannIDType aID);

/** @brief De-initialize IPC driver

    This API deinitializes IPC driver.

    @behavior Sync, Non-reentrant

    @param[in]  aID     IPC Channel ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_STATE    IPC driver is not yet initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameter

    @post If successfully de-initialized, the IPC driver shall move to UNINIT state

    @trace  #BRCM_ARCH_IPC_DEINIT_PROC  #BRCM_REQ_IPC_DEINIT

    @limitations None
*/
extern int32_t IPC_DeInit(IPC_ChannIDType aID);

/** @brief Send IPC message

    This API sends a message through IPC.

    @behavior Sync, Non-reentrant

    @param[in]      aID         IPC Channel ID
    @param[out]     aCmd        Command to process
    @param[out]     aMsg        Pointer to message
    @param[in]      aLen        Pointer to length of the message

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_STATE    IPC is not yet initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameter

    @trace  #BRCM_ARCH_IPC_SEND_PROC  #BRCM_REQ_IPC_SEND

    @limitations None
*/
extern int32_t IPC_Send(IPC_ChannIDType aID, uint32_t aCmd, uint8_t *aMsg, uint32_t aLen);

/** @brief Receive message

    This API retrieves a message obtained through IPC

    @behavior Sync, Non-reentrant

    @param[in]      aID         IPC Channel ID
    @param[out]     aCmd        Pointer to Command to process
    @param[out]     aMsg        Pointer to message
    @param[in]      aInLen      Pointer to length of the message
    @param[in]      aOutLen     Pointer to length of the received message

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NOMEM          Ran out of buffers
    @retval     #BCM_ERR_DATA_INTEG     Data integrity failed
    @retval     #BCM_ERR_INVAL_STATE    IPC is not yet initialized
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid Magic Header
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameter

    @trace  #BRCM_ARCH_IPC_RECV_PROC  #BRCM_REQ_IPC_RECV

    @limitations None
*/
extern int32_t IPC_Receive(IPC_ChannIDType aID, uint32_t* aCmd, uint8_t *aMsg, uint32_t aInLen, uint32_t *aOutLen);

/** @brief Pause IPC Communication.

    @behavior Sync, Non-reentrant

    @param[in]  aID         Channel ID

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameters or state.
    @retval     #BCM_ERR_NOSUPPORT      Feature not supported.

    @trace  #BRCM_ARCH_IPC_PAUSE_PROC  #BRCM_REQ_IPC_INIT

    @limitations None
*/
extern int32_t IPC_Pause(IPC_ChannIDType aID);

/** @brief Resume IPC Communication

    @behavior Sync, Non-reentrant

    @param[in]  aID         Channel ID

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameters or state.
    @retval     #BCM_ERR_NOSUPPORT      Feature not supported.

    @trace  #BRCM_ARCH_IPC_RESUME_PROC  #BRCM_REQ_IPC_INIT

    @limitations None
*/
extern int32_t IPC_Resume(IPC_ChannIDType aID);

/** @brief Read memory directly on slave device

    This API tries to read the memory of on the slave
    device using direct bus protocol method (this API
    is valid only when channel is configured in master
    mode and underlying bus has support for direct
    read protocol)

    @behavior Sync, Non-reentrant

    @param[in]  aID         Channel ID
    @param[in]  aAddr       Memory address of slave.
    @param[out] aData       Pointer to data to be updated during read.
    @param[in]  aWidth      Width of data based on slave memory access.

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameters
    @retval     #BCM_ERR_NOSUPPORT      Feature not supported.
    @retval     #BCM_ERR_UNINIT         Uninitialized state.

    @limitations None
*/
extern int32_t IPC_ReadDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth);

/** @brief Write memory directly on slave device.

    This API tries to write the memory of on the slave
    device using direct bus protocol method (this API
    is valid only when channel is configured in master
    mode and underlying bus has support for direct
    write protocol)

    @behavior Sync, Non-reentrant

    @param[in]  aID         Channel ID
    @param[in]  aAddr       Memory address of slave.
    @param[in]  aData       Pointer to data to be written.
    @param[in]  aWidth      Width of data based on slave memory access.

    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Parameters
    @retval     #BCM_ERR_NOSUPPORT      Feature not supported.
    @retval     #BCM_ERR_UNINIT         Uninitialized state.

    @limitations None
*/
extern int32_t IPC_WriteDirect(IPC_ChannIDType aID, uint32_t aAddr, uint8_t *aData, IPC_AccWidthType aWidth);

#endif /* IPC_H */
/** @} */
