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
/** @defgroup grp_utils_log_il Integration Guide
    @ingroup grp_utils_log

    @addtogroup grp_utils_log_il
    @{
    @section sec_comp_integration Integration Guide
    This section contains details of integrating Logging.

    @section sec_comp_il_seq Sequence Diagram
    @image html log_seq_ulog_startdump.jpg "ULOG_StartDump"

    @file log_osil.h
    @brief Log integration interface

    This file specifies Integration interfaces for logging
    @version 1.3 Import from Document
*/

#ifndef LOG_OSIL_H
#define LOG_OSIL_H

#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>

/**
    @brief Kernel log buffer size

    @trace #BRCM_ARCH_COMP_CONFIG_MACRO #BRCM_REQ_COMP_CONFIG
*/
#define KERNEL_LOG_BUF_SIZE     (1024UL)

/**
    @brief User log buffer size

    @trace #BRCM_ARCH_COMP_CONFIG_MACRO #BRCM_REQ_COMP_CONFIG
*/
#define USER_LOG_BUF_SIZE       (2048UL)

/**
    @name Log commands
    @{
    @brief Type for Log commands

    @trace #BRCM_ARCH_COMP_STATE_TYPE #BRCM_REQ_COMP_GROUPMACRO
*/
typedef uint32_t Log_CmdType;               /**< @brief Type for Log commands */
#define LOG_ULOG_START_DUMP_CMD     (0UL)   /**< @brief Start ULog dump command */
#define LOG_ULOG_STOP_DUMP_CMD      (1UL)   /**< @brief Stop ULog dump command */
#define LOG_ULOG_TRIGGER_DUMP_CMD   (2UL)   /**< @brief Trigger Ulog dump command */
/** @} */

/** @brief Raise system command requests

    This interface raises System command request for logging.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request raised successfully
    @retval     Others                  As returned by Log_CmdHandler

    @post None

    @note To be implemented.

    @trace  #BRCM_ARCH_COMP_IL_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION

    @limitations None
*/
extern int32_t Log_SysCmdReq(const Log_CmdType aCmd);

/** @brief Internal command handler for logging

    This internal loging interface handles all the commands requested by
    Log_SysCmdReq() API. This interface shall be called from Log_SysCmdHandler().

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmd        Command to be processed

    @return void

    @post None

    @note To be used.

    @trace  #BRCM_ARCH_COMP_IL_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION

    @limitations None

    @snippet log_osil.c Usage of Log_CmdHandler
*/
extern void Log_CmdHandler(Log_CmdType aCmd);

/** @brief Handle system command request for logging

    This interface handles all the Log related commands requested from SysCmdReq.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aMagicID        Log SVC Magic Number
    @param[in]      aCmd            Command requested from SysCmdReq
    @param[inout]   aSysIO          uint8_t *

    @return void

    @post describe post condition (status after execution of the function)

    @note To be implemented.

    @trace  #BRCM_ARCH_COMP_IL_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION

    @limitations None

*/
extern void Log_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief UART Tx callback for Logging

    This interface shall be called from the UART driver when the driver is ready
    to send more data (i.e. FIFO is available). This callback shall fill the
    available log buffer data to aData memory, size given by aSize.

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]     aData       Pointer to input buffer that shall be pushed to UART
    @param[in]      aSize       Size of the buffer

    @return Actual size filled by the client

    @post None

    @note To be used as UART_TxCbType for UART log port

    @trace  #BRCM_ARCH_COMP_IL_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION

    @limitations None
*/
extern uint32_t Log_UARTTxCb(char * const aData, uint32_t aSize);

/** @brief Brief Process UART Tx data

    This interface triggers the UART driver to process its internal transmit queue (Tx FIFO).

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @note To be implemented.

    @trace  #BRCM_ARCH_COMP_IL_FUNCTION_PROC  #BCM_REQ_COMP_FUNCTION

    @limitations None
*/
extern void Log_ProcessUARTTx(void);
#endif /* LOG_OSIL_H */
/** @} */
