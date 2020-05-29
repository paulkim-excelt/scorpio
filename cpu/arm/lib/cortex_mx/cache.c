/*****************************************************************************
Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_cortex_mx_cache Implementation
    @ingroup grp_cortex_cache

    @addtogroup grp_cortex_mx_cache
    @{

    @file cpu/arm/cortex_mx/cache.c
    @brief Implementation of Cache maintenance for Cortex-M processors

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <svc.h>
#include <cache.h>
#include <cortex_mx.h>
#include <osil/cache_osil.h>

/**
    @name Cortex-M Cache Design IDs
    @{
    @brief Design IDs for Cortex-M Cache
*/
#define BRCM_SWDSGN_DCACHE_CLEAN_ALL_PROC            (0x80U)   /**< @brief #DCACHE_CleanAll       */
#define BRCM_SWDSGN_DCACHE_CLEAN_RANGE_PROC          (0x81U)   /**< @brief #DCACHE_CleanRange     */
#define BRCM_SWDSGN_DCACHE_INVAL_RANGE_PROC          (0x82U)   /**< @brief #DCACHE_InvalRange     */
#define BRCM_SWDSGN_DCACHE_FLUSH_RANGE_PROC          (0x83U)   /**< @brief #DCACHE_FlushRange     */
#define BRCM_SWDSGN_DCACHE_SYS_CLEAN_ALL_PROC        (0x84U)   /**< @brief #DCACHE_SysCleanAll   */
#define BRCM_SWDSGN_DCACHE_SYS_CLEAN_RANGE_PROC      (0x85U)   /**< @brief #DCACHE_SysCleanRange */
#define BRCM_SWDSGN_DCACHE_SYS_INVAL_RANGE_PROC      (0x86U)   /**< @brief #DCACHE_SysInvalRange */
#define BRCM_SWDSGN_DCACHE_SYS_FLUSH_RANGE_PROC      (0x87U)   /**< @brief #DCACHE_SysFlushRange */
#define BRCM_SWDSGN_DCACHE_REPORTERROR_PROC          (0x88U)   /**< @brief #DCACHE_ReportError    */
#define BRCM_SWDSGN_DCACHE_SYSIO_TYPE                (0x89U)   /**< @brief #DCACHE_SysIOType      */
#define BRCM_SWDSGN_DCACHE_SYSCMDHDLR_PROC           (0x8AU)   /**< @brief #DCACHE_SysCmdHandler  */
/** @} */

/**
    @trace #BRCM_SWARCH_DCACHE_CLEAN_ALL_PROC
    @trace #BRCM_SWARCH_DCACHE_CLEAN_RANGE_PROC
    @trace #BRCM_SWARCH_DCACHE_INVAL_RANGE_PROC
    @trace #BRCM_SWARCH_DCACHE_FLUSH_RANGE_PROC
    @trace #BRCM_SWARCH_DCACHE_SYSCMDHDLR_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
static void DCACHE_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                      uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_DCA_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_DCACHE_SYS_CLEAN_ALL_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_SysCleanAll(void)
{
    CORTEX_MX_DCacheCleanAll();
}

/**
    @trace #BRCM_SWARCH_DCACHE_SYS_CLEAN_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_SysCleanRange(uint32_t aAddr, uint32_t aSize)
{
    CORTEX_MX_DCacheCleanRange(aAddr, aSize);
}

/**
    @trace #BRCM_SWARCH_DCACHE_SYS_INVAL_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_SysInvalRange(uint32_t aAddr, uint32_t aSize)
{
    CORTEX_MX_DCacheInvalRange(aAddr, aSize);
}

/**
    @trace #BRCM_SWARCH_DCACHE_SYS_FLUSH_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_SysFlushRange(uint32_t aAddr, uint32_t aSize)
{
    CORTEX_MX_DCacheFlushRange(aAddr, aSize);
}

/**
    @trace #BRCM_SWARCH_DCACHE_CLEAN_ALL_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_CleanAll(void)
{
    int ret;
    DCACHE_IOType dCacheIO;

    ret = DCACHE_SysCmdReq(DCACHE_CMD_CLEANALL, &dCacheIO);
    if (BCM_ERR_OK != ret) {
        DCACHE_ReportError(BRCM_SWARCH_DCACHE_CLEAN_ALL_PROC, ret, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_DCACHE_CLEAN_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_CleanRange(uint32_t aAddr, uint32_t aSize)
{
    int ret;
    DCACHE_IOType dCacheIO = {
        .addr = aAddr,
        .size = aSize,
    };

    ret = DCACHE_SysCmdReq(DCACHE_CMD_CLEANRANGE, &dCacheIO);
    if (BCM_ERR_OK != ret) {
        DCACHE_ReportError(BRCM_SWARCH_DCACHE_CLEAN_RANGE_PROC, ret, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_DCACHE_INVAL_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_InvalRange(uint32_t aAddr, uint32_t aSize)
{
    int ret;
    DCACHE_IOType dCacheIO = {
        .addr = aAddr,
        .size = aSize,
    };

    ret = DCACHE_SysCmdReq(DCACHE_CMD_INVALRANGE, &dCacheIO);
    if (BCM_ERR_OK != ret) {
        DCACHE_ReportError(BRCM_SWARCH_DCACHE_INVAL_RANGE_PROC, ret, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_DCACHE_FLUSH_RANGE_PROC
    @trace #BRCM_SWREQ_DCACHE_CORTEX_MX
 */
void DCACHE_FlushRange(uint32_t aAddr, uint32_t aSize)
{
    int ret;
    DCACHE_IOType dCacheIO = {
        .addr = aAddr,
        .size = aSize,
    };

    ret = DCACHE_SysCmdReq(DCACHE_CMD_FLUSHRANGE, &dCacheIO);
    if (BCM_ERR_OK != ret) {
        DCACHE_ReportError(BRCM_SWARCH_DCACHE_FLUSH_RANGE_PROC, ret, (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }
    return;
}


/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace  #BRCM_SWARCH_DCACHE_IO_TYPE
    @trace  #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_INTERFACE
*/
typedef union _DCACHE_SysIOType {
    uint8_t *data;
    DCACHE_IOType *io;
} DCACHE_SysIOType;

/**
    @trace  #BRCM_SWARCH_DCACHE_SYSCMDHDLR_PROC
    @trace  #BRCM_SWREQ_DCACHE_CORTEX_MX_KERNEL_HANDLER
*/
void DCACHE_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    int32_t ret = BCM_ERR_OK;
    DCACHE_SysIOType dCacheSVCIO;
    dCacheSVCIO.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_DCA_ID == aMagicID) {
            switch(aCmd){
            case DCACHE_CMD_CLEANALL:
                CORTEX_MX_DCacheCleanAll();
                break;
            case DCACHE_CMD_CLEANRANGE:
                CORTEX_MX_DCacheCleanRange(dCacheSVCIO.io->addr, dCacheSVCIO.io->size);
                break;
            case DCACHE_CMD_INVALRANGE:
                CORTEX_MX_DCacheInvalRange(dCacheSVCIO.io->addr, dCacheSVCIO.io->size);
                break;
            case DCACHE_CMD_FLUSHRANGE:
                CORTEX_MX_DCacheFlushRange(dCacheSVCIO.io->addr, dCacheSVCIO.io->size);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        DCACHE_ReportError(BRCM_SWARCH_DCACHE_SYSCMDHDLR_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, aCmd, aMagicID, (uint32_t)aSysIO);
    }
    return;
}

/** @} */
