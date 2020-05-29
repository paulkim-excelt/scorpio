/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_cortex_mx_cache_il Cache Integration
    @ingroup grp_cortex

    @addtogroup grp_cortex_mx_cache_il
    @{

    @file cpu/arm/inc/osil/cache_osil.h
    @brief Integration for cache operations

    @version 0.1 Initial version
*/

#ifndef CACHE_OSIL_H
#define CACHE_OSIL_H

#include <utils.h>
#include <bcm_err.h>

/**
    @name CACHE Integration IDs
    @{
    @brief API IDs for CACHE Integration
*/
#define BRCM_SWARCH_DCACHE_CMD_TYPE         (0x41U) /**< @brief #DCACHE_CmdType */
#define BRCM_SWARCH_DCACHE_SYSCMDREQ_PROC   (0x42U) /**< @brief #DCACHE_SysCmdReq */
#define BRCM_SWARCH_DCACHE_IO_TYPE          (0x43U) /**< @brief #DCACHE_IOType */
#define BRCM_SWARCH_DCACHE_SYSCMDHDLR_PROC  (0x44U) /**< @brief #DCACHE_SysCmdHandler */
/** @} */

/**
    @name DCACHE_CmdType
    @{
    @brief DCACHE SVC Command
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_INTERFACE
*/
typedef uint32_t DCACHE_CmdType;                     /**< @brief DCACHE SVC Command */
#define DCACHE_CMD_CLEANALL             (0x1UL)      /**< @brief DCACHE_CleanAll */
#define DCACHE_CMD_CLEANRANGE           (0x2UL)      /**< @brief DCACHE_CleanRange */
#define DCACHE_CMD_INVALRANGE           (0x3UL)      /**< @brief DCACHE_InvalRange */
#define DCACHE_CMD_FLUSHRANGE           (0x4UL)      /**< @brief DCACHE_FlushRange */
/** @} */

/**
    @brief  Structure to define input and output parameters

    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_INTERFACE
*/
typedef struct _DCACHE_IOType {
    uint32_t addr;              /**< @brief address */
    uint32_t size;              /**< @brief size */
} DCACHE_IOType;

/** @brief DCache system command request

    This API will trigger a system command request for a given command

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       aCmd            command to be executed in system context
    @param[inout]    aIO             DCACHE_IOType structure to exchange information

    Return values are documented in reverse-chronological order
    @retval          BCM_ERR_OK          success
    @retval          others              failure

    @post none

    @trace  #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_INTERFACE

    @limitations none
*/
extern int32_t DCACHE_SysCmdReq(const uint32_t aCmd, DCACHE_IOType *aIO);

/** @brief DCache system command request handler

    This API will handle the system command request and call internal handler
    after validating the MAGIC ID

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       aMagicID        Magic ID of DCACHE
    @param[in]       acmd            command to be executed in system context
    @param[inout]    aSysIO          System IO structure to exchange information

    Return values are documented in reverse-chronological order
    @return          none

    @post none

    @trace  #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_HANDLER

    @limitations none
*/
extern void DCACHE_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

#endif	/* CACHE_OSIL_H */

/** @} */
