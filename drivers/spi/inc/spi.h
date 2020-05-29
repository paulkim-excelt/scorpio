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
    @defgroup grp_spidrv_ifc Interface
    @ingroup grp_spidrv

    @addtogroup grp_spidrv_ifc
    @{
    @section sec_spi_overview Overview
    @image html spi_overview.jpg
    The SPI is a master interface that enables synchronous serial communication
    with slave peripherals having one of the following:
    - a Motorola SPI-compatible interface
    - a Texas Instruments synchronous serial interface
    - a National Semiconductor Microwire interface.

    In the master configuration, the SPI performs:
    - parallel-to-serial conversion on data written to an internal 16-bit wide,
    8-location deep transmit FIFO
    - serial-to-parallel conversion on received data, buffering it in a similar
    16-bit wide,
    - 8-location deep receive FIFO.

    @section sec_spi_fsm State Machine
    @image html spi_state_machine.jpg "SPI Driver state machine "

    @section sec_spi_seq Sequence Diagrams

    @subsection subsec1_spi_seq Synchronous transfer
    @image html spi_seq_diagram_sync_xfer.jpg "SPI Synchronous transfer Sequence Diagram"

    @subsection subsec2_spi_seq Asynchronous transfer
    @image html spi_seq_diagram_async_xfer.jpg "SPI Asynchronous transfer Sequence Diagram"

    @section sec_spi_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_spi.dep

    @limitations None

    @file spi.h
    @brief SPI Driver Interface
    This file defines interface for SPI driver

    @version 0.11 Import from Docx
*/

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

/**
    @name SPI Driver API IDs
    @{
    @brief API IDs for SPI Driver
*/
#define BRCM_ARCH_SPI_HW_ID_TYPE                (0x00U) /**< @brief #SPI_HwIDType */
#define BRCM_ARCH_SPI_DATA_PKT_ID_TYPE          (0x01U) /**< @brief #SPI_DataPktIDType */
#define BRCM_ARCH_SPI_XFER_PKT_ID_TYPE          (0x02U) /**< @brief #SPI_XferPktIDType */
#define BRCM_ARCH_SPI_SEQ_ID_TYPE               (0x03U) /**< @brief #SPI_SeqIDType */
#define BRCM_ARCH_SPI_DATA_BUF_TYPE             (0x04U) /**< @brief #SPI_DataBufType */
#define BRCM_ARCH_SPI_DATA_SIZE_TYPE            (0x05U) /**< @brief #SPI_DataSizeType */
#define BRCM_ARCH_SPI_STATE_TYPE                (0x06U) /**< @brief #SPI_StateType */
#define BRCM_ARCH_SPI_XFER_PKT_STATUS_TYPE      (0x07U) /**< @brief #SPI_XferPktStatusType */
#define BRCM_ARCH_SPI_SEQ_STATUS_TYPE           (0x08U) /**< @brief #SPI_SeqStatusType */
#define BRCM_ARCH_SPI_XFER_PKT_PRIO_TYPE        (0x09U) /**< @brief #SPI_XferPktPrioType */
#define BRCM_ARCH_SPI_DATA_WIDTH_TYPE           (0x0AU) /**< @brief #SPI_DataWidthType */
#define BRCM_ARCH_SPI_FRM_FORMAT_TYPE           (0x0BU) /**< @brief #SPI_FRMFormatType */
#define BRCM_ARCH_SPI_XFER_PKT_END_CB_TYPE_PROC (0x0CU) /**< @brief #SPI_XferPktEndCbType */
#define BRCM_ARCH_SPI_SEQ_END_CB_TYPE_PROC      (0x0DU) /**< @brief #SPI_SeqEndCbType */
#define BRCM_ARCH_SPI_DATA_PKT_TYPE             (0x0EU) /**< @brief #SPI_DataPktType */
#define BRCM_ARCH_SPI_XFER_PKT_TYPE             (0x0FU) /**< @brief #SPI_XferPktType */
#define BRCM_ARCH_SPI_SEQ_TYPE                  (0x10U) /**< @brief #SPI_SeqType */
#define BRCM_ARCH_SPI_CONFIG_TYPE               (0x11U) /**< @brief #SPI_ConfigType */
#define BRCM_ARCH_SPI_INIT_PROC                 (0x12U) /**< @brief #SPI_Init */
#define BRCM_ARCH_SPI_DEINIT_PROC               (0x13U) /**< @brief #SPI_DeInit */
#define BRCM_ARCH_SPI_ASYNC_XFER_PROC           (0x14U) /**< @brief #SPI_AsyncXfer */
#define BRCM_ARCH_SPI_SETUP_BUF_PROC            (0x15U) /**< @brief #SPI_SetupBuf */
#define BRCM_ARCH_SPI_GET_DRV_STATE_PROC        (0x16U) /**< @brief #SPI_GetDrvState */
#define BRCM_ARCH_SPI_GET_XFER_PKT_STATUS_PROC  (0x17U) /**< @brief #SPI_GetXferPktStatus */
#define BRCM_ARCH_SPI_GET_SEQ_STATUS_PROC       (0x18U) /**< @brief #SPI_GetSeqStatus */
#define BRCM_ARCH_SPI_SYNC_XFER_PROC            (0x19U) /**< @brief #SPI_SyncXfer */
#define BRCM_ARCH_SPI_GET_HW_STATE_PROC         (0x1AU) /**< @brief #SPI_GetHWState */
#define BRCM_ARCH_SPI_CANCEL_PROC               (0x1BU) /**< @brief #SPI_Cancel */
/** @} */

/**
    @brief Index of the SPI bus

    @trace #BRCM_ARCH_SPI_HW_ID_TYPE #BRCM_REQ_SPI_HW_ID
*/
typedef uint8_t SPI_HwIDType;

/**
    @brief ID of the SPI data packet

    @trace #BRCM_ARCH_SPI_DATA_PKT_ID_TYPE #BRCM_REQ_SPI_DATA_PKT_ID
*/
typedef uint8_t SPI_DataPktIDType;

/**
    @brief ID of the SPI transfer packet

    @trace #BRCM_ARCH_SPI_XFER_PKT_ID_TYPE #BRCM_REQ_SPI_XFER_PKT_ID
*/
typedef uint16_t SPI_XferPktIDType;

/**
    @brief ID of the SPI sequence

    @trace #BRCM_ARCH_SPI_SEQ_ID_TYPE #BRCM_REQ_SPI_SEQ_ID
*/
typedef uint8_t SPI_SeqIDType;

/**
    @brief Type for the SPI data buffer

    @trace #BRCM_ARCH_SPI_DATA_BUF_TYPE #BRCM_REQ_SPI_DATA_BUF
*/
typedef uint8_t SPI_DataBufType;

/**
    @brief Size of the SPI data buffer

    @trace #BRCM_ARCH_SPI_DATA_BUF_TYPE #BRCM_REQ_SPI_DATA_BUF
*/
typedef uint16_t SPI_DataSizeType;

/**
    @name SPI_StateType
    @{
    @brief State of the SPI Driver

    @trace #BRCM_ARCH_SPI_STATE_TYPE #BRCM_REQ_SPI_STATE
*/
typedef uint32_t SPI_StateType;                 /**< @brief typedef for SPI states */
#define SPI_STATE_UNINIT                (0UL)   /**< @brief Uninitialized state */
#define SPI_STATE_IDLE                  (1UL)   /**< @brief Idle state */
#define SPI_STATE_BUSY                  (2UL)   /**< @brief Busy state */
/** @} */

/**
    @name SPI_XferPktStatusType
    @{
    @brief Status of the SPI packet transfer

    @trace #BRCM_ARCH_SPI_XFER_PKT_STATUS_TYPE #BRCM_REQ_SPI_XFER_PKT_STATUS
*/
typedef uint32_t SPI_XferPktStatusType;   /**< @brief typedef for SPI packet transfer status */
#define SPI_XFER_PKT_STATUS_OK      (0UL) /**< @brief The last transfer is successful */
#define SPI_XFER_PKT_STATUS_PENDING (1UL) /**< @brief The SPI driver is busy with transfer */
#define SPI_XFER_PKT_STATUS_FAILED  (2UL) /**< @brief The last transfer has failed */
#define SPI_XFER_PKT_STATUS_QUEUED  (3UL) /**< @brief The packet is queued and the actual transfer is not started */
/** @} */

/**
    @name SPI_SeqStatusType
    @{
    @brief Status of the SPI sequence

    @trace #BRCM_ARCH_SPI_SEQ_STATUS_TYPE #BRCM_REQ_SPI_SEQ_STATUS
*/
typedef uint32_t SPI_SeqStatusType;       /**< @brief typedef for SPI sequence status */
#define SPI_SEQ_STATUS_OK           (0UL) /**< @brief The last transfer of the sequence is successful */
#define SPI_SEQ_STATUS_PENDING      (1UL) /**< @brief The SPI driver is busy with sequence transfer */
#define SPI_SEQ_STATUS_FAILED       (2UL) /**< @brief The SPI driver is busy with sequence transfer */
#define SPI_SEQ_STATUS_CANCELLED    (3UL) /**< @brief The last transfer of the Sequence has been canceled by user */
/** @} */

/**
    @name SPI_XferPktPrioType
    @{
    @brief Priority of the SPI transfer packet

    @trace #BRCM_ARCH_SPI_XFER_PKT_PRIO_TYPE #BRCM_REQ_SPI_XFER_PKT_PRIO
*/
typedef uint32_t SPI_XferPktPrioType;           /**< @brief typedef for SPI transfer packet priority */
#define SPI_XFER_PKT_PRIO_MIN           (0UL)   /**< @brief Minimum priority */
#define SPI_XFER_PKT_PRIO_MAX           (3UL)   /**< @brief Maximum priority */
/** @} */

/**
    @name SPI_DataWidthType
    @{
    @brief Width of the SPI data to be transferred

    @trace #BRCM_ARCH_SPI_DATA_WIDTH_TYPE #BRCM_REQ_SPI_DATA_WIDTH
*/
typedef uint32_t SPI_DataWidthType;             /**< @brief typedef for SPI data width */
#define SPI_DATA_WIDTH_MIN              (4UL)   /**< @brief 4 bits */
#define SPI_DATA_WIDTH_MAX              (16UL)  /**< @brief 16 bits */
/** @} */

/**
    @name SPI_FRMFormatType
    @{
    @brief SPI Frame format

    @trace #BRCM_ARCH_SPI_FRM_FORMAT_TYPE #BRCM_REQ_SPI_FRM_FORMAT
*/
typedef uint16_t SPI_FRMFormatType;             /**< @brief typedef for SPI frame format */
#define SPI_FRM_FORMAT_MOTOROLA         (0U)    /**< @brief MOTOROLA slave */
#define SPI_FRM_FORMAT_TI               (1U)    /**< @brief TI slave */
#define SPI_FRM_FORMAT_NATIONAL         (2U)    /**< @brief NATIONAL microwire slave */
/** @} */

/** @brief Callback type for the end of the SPI packet transfer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_SPI_XFER_PKT_END_CB_TYPE_PROC  #BCM_REQ_SPI_XFER_PKT_END_CB_TYPE

    @limitations None
*/
typedef void (*SPI_XferPktEndCbType)(void);

/** @brief Callback type for the end of the SPI sequence

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_SPI_SEQ_END_CB_TYPE_PROC  #BCM_REQ_SPI_SEQ_END_CB_TYPE

    @limitations None
*/
typedef void (*SPI_SeqEndCbType)(void);

/**
    @brief SPI data packet structure
    This is the basic unit of the SPI transfer which contains the TX/RX buffer
    pointers and width of the transfer.

    @trace #BRCM_ARCH_SPI_DATA_PKT_TYPE #BRCM_REQ_SPI_DATA_PKT
 */
typedef struct {
    SPI_DataPktIDType id;           /**< @brief Data index */
    SPI_DataWidthType dataWidth;    /**< @brief Width of the transfer data */
    uint32_t defaultTxValue;        /**< @brief default value to be used for the
                                         transfer if the buffer is pointing
                                         to NULL */
    const SPI_DataBufType *srcBuf;  /**< @brief Pointer to source buffer */
    SPI_DataSizeType srcBufMaxSize; /**< @brief Maximum size of the source buffer */
    SPI_DataBufType *destBuf;       /**< @brief Pointer to destination buffer */
    SPI_DataSizeType destBufMaxSize;/**< @brief Maximum size of the destination buffer */
    SPI_DataSizeType xferSize;      /**< @brief Transfer size */
} SPI_DataPktType;

/**
    @brief SPI transfer packet structure
    Transfer packet contains the list of data packets which are transferred over
    SPI with de-asserting the chip select.
    dataPktIdxList is a pointer to a list which contains the data packet indexes
    (All the data packets shall be defined in advance during compilation with
    proper indices)

    @trace #BRCM_ARCH_SPI_XFER_PKT_TYPE #BRCM_REQ_SPI_XFER_PKT
 */
typedef struct {
    SPI_XferPktIDType id;               /**< @brief Transmit packet index */
    SPI_HwIDType hwID;                  /**< @brief Hardware index */
    SPI_FRMFormatType frameFormat;      /**< @brief Frame Format */
    uint32_t bitRate;                   /**< @brief Bit Rate */
#define BITRATE_MIN                 (769UL)
#define BITRATE_MAX                 (25000000UL)
    SPI_XferPktPrioType priority;       /**< @brief Priority of the transfer packet */
    SPI_DataPktIDType *dataPktIdxList;  /**< @brief Pointer to data packet indices list */
    uint32_t dataPktIdxListSz;          /**< @brief Number of entries in the data
                                             packet indices list */
    SPI_XferPktEndCbType cb;            /**< @brief Transmission completion callback */
} SPI_XferPktType;

/**
    @brief SPI transfer sequence structure
    Transfer sequence contains the list of the transfer packets (SPI_XferPktType).
    A sequence might contain transfers on different SPI buses.

    @trace #BRCM_ARCH_SPI_SEQ_TYPE #BRCM_REQ_SPI_SEQ
 */
typedef struct {
    SPI_SeqIDType id;                   /**< @brief Sequence index */
    const SPI_XferPktType *xferPktList; /**< @brief Pointer to transfer packet list */
    uint32_t xferPktListSz;             /**< @brief No of entries in transfer pkt list */
    uint32_t isInterruptible;           /**< @brief Flag that tells whether the sequence
                                             can be suspended during transfer */
    SPI_SeqEndCbType cb;                /**< @brief Sequence completion callback */
} SPI_SeqType;

/**
    @brief SPI configuration structure

    @trace #BRCM_ARCH_SPI_CONFIG_TYPE #BRCM_REQ_SPI_CONFIG
 */
typedef struct {
    const SPI_SeqType *seqList;     /**< @brief Pointer to sequence list */
    uint32_t seqListSz;             /**< @brief No of entries in the sequence list */
    uint32_t isSeqInterruptible;    /**< @brief Flag that tells whether sequence
                                         interruption is allowed or not */
} SPI_ConfigType;

/** @brief Interface to initialize the SPI Driver

    This API initializes all the buses which are present as part of XferPktType
    structure

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_SPI_INIT_PROC  #BCM_REQ_SPI_INIT

    @limitations None
*/
extern void SPI_Init(void);

/** @brief Interface to de-initialize the SPI Driver

    This API de-initializes all the buses which are initialized

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         DeInit successful
    @retval     #BCM_ERR_UNINIT     Driver is in uninitialized state
    @retval     #BCM_ERR_BUSY       Driver is busy

    @post None

    @trace  #BRCM_ARCH_SPI_DEINIT_PROC  #BCM_REQ_SPI_DEINIT

    @limitations None
*/
extern int32_t SPI_DeInit(void);

/** @brief Interface to transfer the sequence.

    At the end of the sequence and at the end of each xfer pkt, client will be
    notified (#SPI_SeqEndCbType and #SPI_XferPktEndCbType). The client needs to call
    #SPI_GetSeqStatus to know the status of the sequence and #SPI_GetXferPktStatus
    to know the status of the xfer pkt.

    @behavior Async, Re-entrant

    @pre #SPI_SetupBuf shall be called before calling this function

    @param[in]      aSeqID          Sequence ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Sequence is successfully queued
    @retval     #BCM_ERR_UNINIT         Driver is in uninitialized state
    @retval     #BCM_ERR_BUSY           Sequence is already queued
    @retval     #BCM_ERR_INVAL_STATE    Sequence shares the xfer pkts with other
                                        sequence which is already in the queue

    @post None

    @trace  #BRCM_ARCH_SPI_ASYNC_XFER_PROC  #BCM_REQ_SPI_ASYNC_XFER

    @limitations None
*/
extern int32_t SPI_AsyncXfer(SPI_SeqIDType aSeqID);

/** @brief Interface to setup the buffers and length of the data for the transfer

    This API updates the source and destination pointers and the xfer size of the
    data pkt aDataPktID
    This API shall be called before calling SPI_AsyncXfer/SPI_SyncXfer
    If aSrc is NULL, defaultTxValue of the corresponding data pkt is transferred
    when SPI_AsyncXfer/SPI_SyncXfer is called
    If aDest is NULL, received data is ignored when SPI_AsyncXfer/SPI_SyncXfer is
    called

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aDataPktID          Data packet ID
    @param[in]      aSrc                Pointer to source buffer
    @param[in]      aDest               Pointer to destination buffer
    @param[in]      aLen                Length of the data to be transferred

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Setup is success
    @retval     #BCM_ERR_UNINIT         Driver is in uninitialized state
    @retval     #BCM_ERR_BUSY           Driver is busy processing

    @post None

    @trace  #BRCM_ARCH_SPI_SETUP_BUF_PROC  #BCM_REQ_SPI_SETUP_BUF

    @limitations None
*/
extern int32_t SPI_SetupBuf(SPI_DataPktIDType aDataPktID,
                            const SPI_DataBufType *const aSrc,
                            SPI_DataBufType *const aDest,
                            SPI_DataSizeType aLen);

/** @brief This API returns the state of the driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     #SPI_StateType      State of the driver

    @post None

    @trace  #BRCM_ARCH_SPI_GET_DRV_STATE_PROC  #BCM_REQ_SPI_GET_DRV_STATE

    @limitations None
*/
extern SPI_StateType SPI_GetDrvState(void);

/** @brief This API returns the status of the last transfer

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aXferPktID             Transmit ID

    Return values are documented in reverse-chronological order
    @retval     #SPI_XferPktStatusType     Status of the last transfer

    @post None

    @trace  #BRCM_ARCH_SPI_GET_XFER_PKT_STATUS_PROC  #BCM_REQ_SPI_GET_XFER_PKT_STATUS

    @limitations None
*/
extern SPI_XferPktStatusType SPI_GetXferPktStatus(SPI_XferPktIDType aXferPktID);

/** @brief This API returns the status of the last transfer of the sequence

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aSeqID             Sequence ID

    Return values are documented in reverse-chronological order
    @retval     #SPI_SeqStatusType     Status of the last transfer of the sequence

    @post None

    @trace  #BRCM_ARCH_SPI_GET_SEQ_STATUS_PROC  #BCM_REQ_SPI_GET_SEQ_STATUS

    @limitations None
*/
extern SPI_SeqStatusType SPI_GetSeqStatus(SPI_SeqIDType aSeqID);

/** @brief Interface to transfer the sequence.

    @behavior Sync, Re-entrant

    @pre #SPI_SetupBuf shall be called before calling this function

    @param[in]      aSeqID             Sequence ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Sequence is successfully transferred
    @retval     #BCM_ERR_UNINIT     Driver is in uninitialized state

    @post None

    @trace  #BRCM_ARCH_SPI_SYNC_XFER_PROC  #BCM_REQ_SPI_SYNC_XFER

    @limitations None
*/
extern int32_t SPI_SyncXfer(SPI_SeqIDType aSeqID);

/** @brief This API returns the state of the hardware

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHwID           Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #SPI_StateType      State of the hardware

    @post None

    @trace  #BRCM_ARCH_SPI_GET_HW_STATE_PROC  #BCM_REQ_SPI_GET_HW_STATE

    @limitations None
*/
extern SPI_StateType SPI_GetHWState(SPI_HwIDType aHwID);

/** @brief This API cancels the specified on-going sequence transfer

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aSeqID              Sequence ID

    @return     void

    @post None

    @trace  #BRCM_ARCH_SPI_CANCEL_PROC  #BCM_REQ_SPI_CANCEL

    @limitations None
*/
extern void SPI_Cancel(SPI_SeqIDType aSeqID);

#endif /* SPI_H */

/** @} */
