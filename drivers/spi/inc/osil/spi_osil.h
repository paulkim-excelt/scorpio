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

/** @defgroup grp_spi_il Integration Guide
    @ingroup grp_spidrv

    @addtogroup grp_spi_il
    @{
    @section sec_spi_integration Integration Guide
    This driver requires a task for deferred interrupt processing. This task
    shall be referred as TaskSPIDriver in this document. This task will be
    signaled from SPI interrupt handler after calling #SPI_IRQHandler.

    @section sec_spi_il_seq Sequence Diagram
    @image html spi_seq_il_diagram.jpg "SPI Driver SVC layer interaction"

    @file spi_osil.h
    @brief SPI Integration Interfaces
    This header file contains the integration interface for SPI

    @version 0.11 Import from Docx
*/

#ifndef SPI_OSIL_H
#define SPI_OSIL_H

#include <system.h>
#include <spi.h>

/**
    @name SPI Integration Interface IDs
    @{
    @brief Integration Interface IDs for SPI
*/
#define BRCM_ARCH_SPI_IL_CMD_TYPE                   (0x41U) /**< @brief #SPI_CmdType */
#define BRCM_ARCH_SPI_IL_SEQ_MAX_MACRO              (0x42U) /**< @brief #SPI_SEQ_MAX */
#define BRCM_ARCH_SPI_IL_XFER_PKTS_MAX_MACRO        (0x43U) /**< @brief #SPI_XFER_PKTS_MAX */
#define BRCM_ARCH_SPI_IL_DATA_PKTS_MAX_MACRO        (0x44U) /**< @brief #SPI_DATA_PKTS_MAX */
#define BRCM_ARCH_SPI_IL_GLOBAL_CONFIG_VAR          (0x45U) /**< @brief #SPI_Config */
#define BRCM_ARCH_SPI_IL_GLOBAL_DATA_PKT_LIST_VAR   (0x46U) /**< @brief #SPI_DataPktList */
#define BRCM_ARCH_SPI_IL_IO_TYPE                    (0x47U) /**< @brief #SPIIO */
#define BRCM_ARCH_SPI_IL_CMD_HANDLER_PROC           (0x48U) /**< @brief #SPI_CmdHandler */
#define BRCM_ARCH_SPI_IL_SYS_CMD_HANDLER_PROC       (0x49U) /**< @brief #SPI_SysCmdHandler */
#define BRCM_ARCH_SPI_IL_SYS_CMD_REQ_PROC           (0x4AU) /**< @brief #SPI_SysCmdReq */
#define BRCM_ARCH_SPI_IL_IRQ_HANDLER_PROC           (0x4BU) /**< @brief #SPI_IRQHandler */
#define BRCM_ARCH_SPI_IL_IRQ_EVENT_HANDLER_PROC     (0x4CU) /**< @brief #SPI_IRQEventHandler */
/** @} */

/**
    @name SPI_CmdType
    @{
    @brief SPI command type

    @trace #BRCM_ARCH_SPI_IL_CMD_TYPE #BRCM_REQ_SPI_IL_CMD
*/
typedef uint32_t SPI_CmdType;                   /**< @brief typedef for SPI commands */
#define SPI_CMD_INIT                    (0x0UL) /**< @brief SPI cmd for initialization */
#define SPI_CMD_DEINIT                  (0x1UL) /**< @brief SPI cmd for de-initialization */
#define SPI_CMD_ASYNC_XFER              (0x2UL) /**< @brief SPI cmd to tranmit the sequence asynchronously */
#define SPI_CMD_SETUP_BUF               (0x3UL) /**< @brief SPI cmd to setup the external buffer */
#define SPI_CMD_SYNC_XFER               (0x4UL) /**< @brief SPI cmd to transmit sequence synchronously */
#define SPI_CMD_CANCEL                  (0x5UL) /**< @brief SPI cmd to cancel the sequence */
#define SPI_CMD_IRQ_EVENT_HANDLE        (0x6UL) /**< @brief SPI cmd to handle IRQ Events */
/** @} */

/**
    @brief Maximum number of sequences

    @trace #BRCM_ARCH_SPI_IL_SEQ_MAX_MACRO #BRCM_REQ_SPI_IL_SEQ_MAX
*/
#define SPI_SEQ_MAX                     (10UL)

/**
    @brief Maximum number of transfer packets

    @trace #BRCM_ARCH_SPI_IL_XFER_PKTS_MAX_MACRO #BRCM_REQ_SPI_IL_XFER_PKTS
*/
#define SPI_XFER_PKTS_MAX               (25UL)

/**
    @brief Maximum number of data packets

    @trace #BRCM_ARCH_SPI_IL_DATA_PKTS_MAX_MACRO #BRCM_REQ_SPI_IL_DATA_PKTS
*/
#define SPI_DATA_PKTS_MAX               (10UL)

/**
    @brief Pointer to SPI Driver configuration

    @trace  #BRCM_ARCH_SPI_IL_GLOBAL_CONFIG_VAR  #BRCM_ARCH_SPI_IL_GLOBAL_CONFIG
*/
extern const SPI_ConfigType SPI_Config;

/**
    @brief Array of SPI data packet list

    @trace  #BRCM_ARCH_SPI_IL_GLOBAL_DATA_PKT_LIST_VAR  #BRCM_ARCH_SPI_IL_GLOBAL_DATA_PKT_LIST
*/
extern SPI_DataPktType SPI_DataPktList[SPI_DATA_PKTS_MAX];

/**
    @brief Structure for SPI SVC interface

    @trace #BRCM_ARCH_SPI_IL_IO_TYPE #BRCM_REQ_SPI_IL_IO
 */
typedef struct _SPIIO {
    int32_t retVal;                 /**< @brief SVC response */
    SPI_SeqIDType seqID;            /**< @brief Sequence ID */
    SPI_DataPktIDType dataPktID;    /**< @brief Data Pkt ID */
    SPI_HwIDType hwID;              /**< @brief Hardware ID */
    const SPI_DataBufType *src;     /**< @brief Source buffer */
    SPI_DataBufType *dest;          /**< @brief Destination buffer */
    SPI_DataSizeType xferSize;      /**< @brief Transfer size */
} SPIIO;

/** @brief SPI Internal command handler

    This interface handles commands requested from system command handler.
    This interface shall be called from #SPI_SysCmdHandler

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request completed successfully
    @retval     Others                  API dependent error values
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     #BCM_ERR_UNKNOWN        aIO is NULL

    @post None

    @note To be used.

    @trace  #BRCM_ARCH_SPI_IL_CMD_HANDLER_PROC  #BRCM_REQ_SPI_CMD_HANDLER

    @limitations None

    @snippet spi_osil.c Usage of SPI_CmdHandler
*/
extern int32_t SPI_CmdHandler(SPI_CmdType aCmd,
                              SPIIO *const aIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aMagicID    SPI SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      uint8_t * to be interpreted as SPIIO

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_ARCH_SPI_IL_SYS_CMD_HANDLER_PROC  #BRCM_REQ_SPI_SYS_CMD_HANDLER

    @limitations None
*/
extern void SPI_SysCmdHandler(uint32_t aMagicID,
                              uint32_t aCmd,
                              uint8_t *aSysIO);

/** @brief Interface to raise a request to SPI driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned by #SPI_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @trace  #BRCM_ARCH_SPI_IL_SYS_CMD_REQ_PROC  #BRCM_REQ_SPI_SYS_CMD_REQ

    @limitations None
*/
extern int32_t SPI_SysCmdReq(SPI_CmdType aCmd,
                             SPIIO *const aIO);

/** @brief SPI driver interrupt handlers

    This interface handles all the SPI interrupts. This interface shall be
    called from the interrupt service routine for SPI driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aHwID       SPI hardware ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_ARCH_SPI_IL_IRQ_HANDLER_PROC  #BRCM_REQ_SPI_IRQ_HANDLER

    @limitations None

    @snippet spi_osil.c Usage of SPI_IRQHandler
*/
extern void SPI_IRQHandler(SPI_HwIDType aHwID);

/** @brief SPI bottom half handler

    This interface handles the bottom half processing

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aHwID       SPI hardware ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_ARCH_SPI_IL_IRQ_EVENT_HANDLER_PROC  #BRCM_REQ_SPI_IRQ_EVENT_HANDLER

    @limitations None

    @snippet serial_io_osil.c Usage of SPI_IRQEventHandler
*/
extern void SPI_IRQEventHandler(SPI_HwIDType aHwID);

#endif /* SPI_OSIL_H */

/** @} */
