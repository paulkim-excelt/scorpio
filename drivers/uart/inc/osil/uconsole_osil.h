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
    @defgroup grp_uconsoledrv_osil UCONSOLE Integration
    @ingroup grp_uartdrv

    @addtogroup grp_uconsoledrv_osil
    @{

    @file uconsole_osil.h

    @version 0.1 Initial version
*/

#ifndef UCONSOLE_OSIL_H
#define UCONSOLE_OSIL_H

#include <stdint.h>
#include <uart.h>
#include <svc.h>
#include <bcm_err.h>

/**
    @name UCONSOLE Integration IDs
    @{
    @brief Integration IDs for UCONSOLE
 */
#define BRCM_SWARCH_UCONSOLE_CMD_TYPE               (0x50U) /**< @brief #UCONSOLE_CmdType               */
#define BRCM_SWARCH_UCONSOLE_IO_TYPE                (0x51U) /**< @brief #UCONSOLE_IOType                */
#define BRCM_SWARCH_UCONSOLE_SYS_CMD_REQ_PROC       (0x52U) /**< @brief #UCONSOLE_SysCmdReq             */
#define BRCM_SWARCH_UCONSOLE_SYS_CMD_HANDLER_PROC   (0x53U) /**< @brief #UCONSOLE_SysCmdHandler         */
#define BRCM_SWARCH_UCONSOLE_RCV_INDICATION_PROC    (0x54U) /**< @brief #UCONSOLE_RcvIndication         */
#define BRCM_SWARCH_UCONSOLE_ERR_INDICATION_PROC    (0x55U) /**< @brief #UCONSOLE_ErrIndication         */
#define BRCM_SWARCH_UCONSOLE_NOTIFY_RCVIND_PROC     (0x56U) /**< @brief #UCONSOLE_NotifyRcvIndication   */
#define BRCM_SWARCH_UCONSOLE_TX_CONFIRMATION_PROC   (0x57U) /**< @brief #UCONSOLE_TxConfirmation        */
/** @} */

/**
    @name UCONSOLE_CmdType
    @{
    @brief UART driver command type

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE
*/
typedef uint32_t UCONSOLE_CmdType;                     /**< @brief UCONSOLE driver command type */
#define UCONSOLE_CMD_READ                   (0x1UL)    /**< @brief UCONSOLE read command */
/** @} */

/**
    @brief UART IO structure

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE
*/
typedef struct _UCONSOLE_IOType {
    int32_t retVal;     /**< @brief Return value as set by #UCONSOLE_SysCmdHandler */
    char* data;/**< @brief UCONSOLE data pointer to read */
    uint32_t size;      /**< @brief UCONSOLE size to be read  */
} UCONSOLE_IOType;

/** @brief UCONSOLE driver system command request

    Interface to raise System Command Request for UCONSOLE driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to In-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by #UCONSOLE_SysCmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic ID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t UCONSOLE_SysCmdReq(UCONSOLE_CmdType aCmd, UCONSOLE_IOType *const aIO);

/** @brief Handle UCONSOLE driver commands

    This interface handles all the UCONSOLE driver commands requested from system
    command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    UCONSOLE driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #UCONSOLEIO

    @return     void

    @post TBD

    @note This interface shall be installed it to SVC layer by integrator.

    @trace  #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

    @limitations None
*/
extern void UCONSOLE_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief UCONSOLE Rx callback

    Interface to handle UCONSOLE Rx callback from UART

    @behavior Sync, Non-Reentrant

    @pre TBD

    @param[in]      aData       Data received on the UCONSOLE
    @param[in]      aSize       Size of the data received (in bytes)

    Return values are documented in reverse-chronological order
    @return     void

    @post TBD

    @note This interface shall be installed for appropriate UART instance
    by the integrator.

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

    @limitations None
*/
extern void UCONSOLE_RcvIndication(uint8_t * aData, uint32_t aSize);

/** @brief UCONSOLE Error callback

    Interface to handle UCONSOLE Error callback from UART

    @behavior Sync, Non-Reentrant

    @pre TBD

    @param[in]      aError      Error Type from UART
    @param[in]      aData       Data Byte which contained the error

    Return values are documented in reverse-chronological order
    @return     void

    @post TBD

    @note This interface shall be installed for appropriate UART instance
    by the integrator.

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

    @limitations None
*/
extern void UCONSOLE_ErrIndication(UART_ErrorType aError, uint8_t aData);

/** @brief Indicate Rx data to UCONSOLE Clients

    @behavior Sync, Non-Reentrant

    @pre TBD

    @param[in]      void

    Return values are documented in reverse-chronological order
    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

    @limitations None
*/
void UCONSOLE_NotifyRcvIndication(void);

/** @brief Indicate Tx confirmation to UCONSOLE Clients

    @behavior Sync, Non-Reentrant

    @pre TBD

    @param[in]      void

    Return values are documented in reverse-chronological order
    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_UCONSOLE_KERNEL_HANDLER

    @limitations None
*/
void UCONSOLE_TxConfirmation(void);

#endif /* UCONSOLE_OSIL_H */
/** @} */
