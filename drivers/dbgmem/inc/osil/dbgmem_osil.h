/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    PARTICULAR PURPOSE, LACK OF VIRUSES, ADBGMEMRACY OR COMPLETENESS,
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

/** @defgroup grp_dbgmemdrv_il Integration Guide
    @ingroup grp_dbgmemdrv

    @addtogroup grp_dbgmemdrv_il
    @{
    @section sec_dbgmemdrv_integration Integration Guide
    DBGMEM driver is implemented as a SysCmdReq (system command request
    [@ref ref_sads_spec]) and SysCmdHandler (system command handler [@ref ref_sads_spec])
    model. This section gives the necessary information on how to install
    command handler for DBGMEM driver for privileged access as well as setting up
    the DBGMEM driver’s interrupt handler

    @file dbgmem_osil.h
    @brief DBGMEM Driver Integration Interfaces
    This header file contains the integration interface for DBGMEM Driver

    @version 0.1 Initial draft
*/


#ifndef DBGMEM_OSIL_H
#define DBGMEM_OSIL_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <system.h>
#include <inttypes.h>
#include <dbgmem.h>

/**
    @name DBGMEM Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for DBGMEM Driver
*/
#define BRCM_SWARCH_DBGMEM_CMD_TYPE             (0x40U) /**< @brief #DBGMEM_CmdType       */
#define BRCM_SWARCH_DBGMEM_IOTYPE_TYPE          (0x41U) /**< @brief #DBGMEM_IOType        */
#define BRCM_SWARCH_DBGMEM_SYSCMDREQ_PROC       (0x42U) /**< @brief #DBGMEM_SysCmdReq     */
#define BRCM_SWARCH_DBGMEM_SYSCMDHANDLER_PROC   (0x43U) /**< @brief #DBGMEM_SysCmdHandler */
/** @} */

/**
    @name DBGMEM_CmdType
    @{
    @brief DBGMEM driver system command request type

    @trace #BRCM_SWREQ_DBGMEM_KERNEL_INTERFACE
*/
typedef uint32_t DBGMEM_CmdType;         /**< @brief typedef for command type */
#define DBGMEM_CMD_COPY         (0x01UL) /**< @brief #DBGMEM_Copy */
/** @} */

/**
    @brief IO structure for DBGMEM driver

    @trace #BRCM_SWREQ_DBGMEM_KERNEL_INTERFACE
 */
typedef struct _DBGMEM_IOType {
    int32_t retVal;     /**< @brief Return value set by #DBGMEM_SysCmdHandler */
    const uint32_t width; /**< @brief Access width */
    const uint32_t length; /**< @brief Number of access */
    const uint32_t src; /**< @brief Source address */
    const uint32_t dst; /**< @brief Destination address */
} DBGMEM_IOType;

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    DBGMEM SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      uint8_t * to be interpreted as DBGMEM_IOType

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_DBGMEM_KERNEL_HANDLER

    @limitations None
*/
extern void DBGMEM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd,
                                 uint8_t *aSysIO);

/** @brief Interface to raise a request to DBGMEM driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aDBGMEM_IOType      In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  As returned by #DBGMEM_CmdHandler

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_DBGMEM_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t DBGMEM_SysCmdReq(DBGMEM_CmdType aCmd, DBGMEM_IOType * const aIO);

#endif /* DBGMEM_OSIL_H */

/** @} */
