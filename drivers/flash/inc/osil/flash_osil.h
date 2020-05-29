/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
/** @defgroup grp_flashdrv_il Integration Guide
    @ingroup grp_flashdrv

    @addtogroup grp_flashdrv_il
    @{
    @section sec_flash_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes Flash Driver IL in more detail.

    This driver requires a task for deferred interrupt processing. This task
    shall be referred as TaskFlashDriver in this document. This task will be
    signaled from Flash interrupt handler after calling #FLASH_IRQHandler.

    Flash driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for Flash Driver
    onto #SysCmdHandler(system command handler).

    @section sec_flash_il_seq Sequence Diagram
    @subsection subsec_flash_il_sync_seq Flash Synchronous Operation IL sequence
    The following sequence diagram depicts the sequence #FLASH_Init as an
    example of Flash synchronous operation. The other synchronous flash
    operations like #FLASH_DeInit, #FLASH_GetOpsResult and #FLASH_GetStatus
    shall follow the same sequence.

    @note #FLASH_Cancel operation is an exception and it shall be represented as
    a separate sequence diagram.

    @image html flash_seq_il_diagram_sync.jpg "Flash synchronous operation IL sequence"

    Brief description of life lines shown in above sequence diagram:
    - <b>Client/User</b>: Client or user layer
    - <b>Flash Driver Inf</b>: Flash driver interface layer
    - <b>SVC Layer</b>: Supervisor call layer.
    - <b>Flash Driver IL</b>: Flash driver integration layer
    - <b>Flash Driver</b>: Flash driver core layer

    @subsection subsec_flash_il_async_seq Flash Asynchronous Operation IL sequence
    The following sequence diagram depicts #FLASH_Read as an example of
    asynchronous flash operation. Asynchronous flash operation call on
    successful return initiates the operation. On successful completion or
    error, flash driver shall call #FLASH_OpsDoneInd or #FLASH_OpsErrInd
    respectively.

    The other asynchronous flash operations like #FLASH_Write, #FLASH_Erase,
    #FLASH_Compare, #FLASH_BlankCheck, #FLASH_CfgReadLane, #FLASH_ReadID and
    #FLASH_RegReadWrite shall follow the same sequence.

    @image html flash_seq_il_diagram_async.jpg "Flash asynchronous operation IL sequence"

    Brief description of life lines shown in above sequence diagram:
    - <b>Client/User</b>: Client or user layer
    - <b>Flash Driver Inf</b>: Flash driver interface layer
    - <b>SVC Layer</b>: Supervisor call layer.
    - <b>Flash Driver IL</b>: Flash driver integration layer
    - <b>QSPI HW</b>: QSPI hardware
    - <b>Flash Driver</b>: Flash driver core layer

    @subsection subsec_flash_il_cancel_seq Flash Cancel Operation IL sequence
    The following sequence diagram depicts #FLASH_Cancel operation.
    #FLASH_Cancel is a synchronous operation but before return #FLASH_OpsErrInd
    is called to indicate cancellation of previous flash operation.

    @image html flash_seq_il_diagram_cancel.jpg "Flash cancel operation IL sequence"

    Brief description of life lines shown in above sequence diagram:
    - <b>Client/User</b>: Client or user layer
    - <b>Flash Driver Inf</b>: Flash driver interface layer
    - <b>SVC Layer</b>: Supervisor call layer. To be implemented as per SADS [1]
    - <b>Flash Driver IL</b>: Flash driver integration layer
    - <b>Flash Driver</b>: Flash driver core layer

    @file flash_osil.h
    @brief Flash Integration Interfaces
    This header file contains the integration interface for Flash driver.
    @version 1.31 Imported from docx
*/
#ifndef FLASH_OSIL_H
#define FLASH_OSIL_H

#include <flash.h>
#include <system.h>

/**
    @name Flash interface IDs
    @{
    @brief FLASH integration interface IDs.
*/
#define BRCM_SWARCH_FLASH_BUS_CMD_TYPE             (0x40U) /**< @brief #FLASH_BusCmdType    */
#define BRCM_SWARCH_FLASH_IO_TYPE                  (0x41U) /**< @brief #FLASH_IOType        */
#define BRCM_SWARCH_FLASH_CFG_TBL_TYPE             (0x42U) /**< @brief #FLASH_CfgTblType    */
#define BRCM_SWARCH_FLASH_OPS_DONE_IND_PROC        (0x43U) /**< @brief #FLASH_OpsDoneInd    */
#define BRCM_SWARCH_FLASH_OPS_ERR_IND_PROC         (0x44U) /**< @brief #FLASH_OpsErrInd     */
#define BRCM_SWARCH_FLASH_STATEHANDLER_PROC        (0x45U) /**< @brief #FLASH_StateHandler  */
#define BRCM_SWARCH_FLASH_SYSCMDHANDLER_PROC       (0x47U) /**< @brief #FLASH_SysCmdHandler */
#define BRCM_SWARCH_FLASH_SYSCMDREQ_PROC           (0x48U) /**< @brief #FLASH_SysCmdReq     */
#define BRCM_SWARCH_FLASH_IRQHANDLER_PROC          (0x49U) /**< @brief #FLASH_BusIRQHandler */
#define BRCM_SWARCH_FLASH_IRQHANDLER0_PROC         (0x4AU) /**< @brief #FLASH_IRQHandler0   */
#define BRCM_SWARCH_FLASH_IRQHANDLER1_PROC         (0x4BU) /**< @brief #FLASH_IRQHandler1   */
#define BRCM_SWARCH_FLASH_CS_ASSERT_TYPE           (0x4CU) /**< @brief #FLASH_ChipSelectAssertType */
/** @} */

/**
   @{
   @brief Flash IO command IDs

   @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
*/
typedef uint32_t FLASH_BusCmdType;
#define FLASH_BUS_CMD_INVALID           (0UL)   /**< Invalid command */
#define FLASH_BUS_CMD_INIT              (1UL)   /**< Bus Init command */
#define FLASH_BUS_CMD_CFG_READ_LANE     (2UL)   /**< Bus configure lane command */
#define FLASH_BUS_CMD_READ              (3UL)   /**< Bus read command */
#define FLASH_BUS_CMD_XFER              (4UL)   /**< Bus transfer command */
#define FLASH_BUS_CMD_STATEHANDLER      (5UL)   /**< Bus state handler command */
#define FLASH_BUS_CMD_GETSTATUS         (6UL)   /**< Bus Get Status command */
/** @} */

/**
    @brief Flash IO structure

    Flash IO structure to be used for #FLASH_CmdHandler

    @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
 */
typedef struct _FLASH_IOType {
    int32_t                 retVal;     /**< @brief Return value from the command operation function */
    FLASH_HwIDType          hwID;       /**< @brief Flash controller ID */
    FLASH_ReadLaneType      readLane;   /**< @brief read lane configuration */
    FLASH_AddrType          addr;       /**< @brief Flash address for operation */
    const uint8_t *         txBuf;      /**< @brief Pointer to transmit buffer for operation */
    uint8_t *               rxBuf;      /**< @brief Pointer to receive buffer for operation */
    FLASH_LenType           len;        /**< @brief Length for operation */
    uint32_t                flag;       /**< @brief flags for QSPI xfer */
    const FLASH_CfgType *   config;     /**< @brief Pointer to flash configuration */
    uint32_t                isErr;      /**< @brief Flag to indicate error during operation */
} FLASH_IOType;

/**
 * @{
 * @brief Flag for transfer continuation
 * @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE
 */
typedef uint32_t FLASH_ChipSelectAssertType;
#define FLASH_CS_ASSERT_STOP            (0UL) /**< @brief Deassert Chip select */
#define FLASH_CS_ASSERT_CONTINUE        (1UL) /**< @brief Continue assertion of Chip select */
/** @} */

/**
    @brief FLASH configuration table

    Flash configuration table type. This configuration structure shall be used
    by system integrator to define flash configuration static table to be used
    for flash driver initialization.

    @trace #BRCM_SWREQ_FLASH_INIT
 */
typedef struct _FLASH_CfgTblType {
    FLASH_HwIDType  hwID;       /**< @brief Flash controller ID */
    FLASH_CfgType   config;     /**< @brief Flash driver configuration for hwID
                                     flash controller */
} FLASH_CfgTblType;

/** @brief Flash operation done indication

    Callback indication from driver to indicate successful completion of last
    flash operation. It is mandatory to implement this function for using the
    flash driver in the system.

    @note Callback function shall perform minimum functionality and shall not
    block for large duration.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Controller ID

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_FLASH_RESULT_INDICATION

    @todo Correct trace of FLASH_OpsDoneInd

    @limitations None
*/
extern void FLASH_OpsDoneInd(FLASH_HwIDType aID);

/** @brief Flash operation error indication

    Callback indication from driver to indicate error encountered (failure)
    during last flash operation. It is mandatory to implement this function for
    using the flash driver in the system.

    @note Callback function shall perform minimum functionality and shall not
    block for large duration.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Controller ID

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_FLASH_RESULT_INDICATION

    @todo Correct trace of FLASH_OpsErrInd

    @limitations None
*/
extern void FLASH_OpsErrInd(FLASH_HwIDType aID);

/** @brief Flash State handler

    Function to handle flash driver states. This function shall be called from
    TaskFlashDriver.

    @behavior Sync, Re-entrant (if aID is different)

    @pre TBD

    @param[in]      aID         Controller ID

    @return     void

    @post TBD

    @note This interface shall be called from TaskFlashDriver.

    @trace #BRCM_SWREQ_FLASH_STATE_HANDLER

    @limitations None

    @snippet flsmgr_osil.c Usage of FLASH_StateHandler
*/
extern void FLASH_StateHandler(FLASH_HwIDType aID);

/** @brief Process system command request

    Interface to handle flash system command request processing.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMagicID    Flash system request magic ID
    @param[in]      aCmd        Flash system request command
    @param[in]      aSysIO      Pointer to system request IO for flashIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and install it
    as part of #SysCmdHandler.

    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER

    @limitations None
*/
extern void FLASH_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd,
                                uint8_t *aSysIO);

/** @brief Flash system command request

    Interface to raise flash system command request.

    @behavior Sync, Re-entrants

    @pre TBD

    @param[in]      aCmd        Flash system request command
    @param[inout]   aIO         Pointer to FlashIO

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request success
    @retval     #BCM_ERR_INVAL_PARAMS   (aCmd is invalid) or
                                        (aIO is NULL)

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_FLASH_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t FLASH_SysCmdReq(FLASH_BusCmdType aCmd, FLASH_IOType * const aIO);

/** @brief FLASH Interrupt request handler

    Interface to handle interrupt request.

    @behavior Sync, Re-entrant if aID is different

    @pre TBD

    @param[in]      aID         Controller ID

    @return     void

    @post TBD

    @note This function shall be called from flash interrupt handler registered
    in interrupt vector table.

    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER

    @limitations None

    @snippet flash_osil.c Usage of FLASH_IRQHandler
*/
extern void FLASH_BusIRQHandler(FLASH_HwIDType aID);

/** @brief FLASH IRQ 0 handler

    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
*/
extern void FLASH_IRQHandler0(void);

/** @brief FLASH IRQ 1 handler

    @trace #BRCM_SWREQ_FLASH_KERNEL_HANDLER
*/
extern void FLASH_IRQHandler1(void);
#endif /* FLASH_OSIL_H */
/** @} */
