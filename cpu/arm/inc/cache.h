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
    @defgroup grp_cortex_cache Cache Maintenance
    @ingroup grp_cortex

    @addtogroup grp_cortex_cache
    @{
    @section sec_cortex_cache_overview Overview
    This section describes cache maintenance APIs

    @file cache.h
    @brief Cache Maintenance APIs

    Utility APIs for D-cache maintenance are provided for use in drivers,
    middleware, and application.

    @version 1.0 Initial Version
*/

#ifndef CACHE_H
#define CACHE_H

/**
    @name Data Cache Architecture IDs
    @{
    @brief Architecture IDs for Data Cache

*/
#define BRCM_SWARCH_DCACHE_CLEAN_ALL_PROC       (0x20U)   /**< @brief #DCACHE_CleanAll      */
#define BRCM_SWARCH_DCACHE_CLEAN_RANGE_PROC     (0x21U)   /**< @brief #DCACHE_CleanRange    */
#define BRCM_SWARCH_DCACHE_INVAL_RANGE_PROC     (0x22U)   /**< @brief #DCACHE_InvalRange    */
#define BRCM_SWARCH_DCACHE_FLUSH_RANGE_PROC     (0x23U)   /**< @brief #DCACHE_FlushRange    */
#define BRCM_SWARCH_DCACHE_SYS_CLEAN_ALL_PROC   (0x28U)   /**< @brief #DCACHE_SysCleanAll   */
#define BRCM_SWARCH_DCACHE_SYS_CLEAN_RANGE_PROC (0x29U)   /**< @brief #DCACHE_SysCleanRange */
#define BRCM_SWARCH_DCACHE_SYS_INVAL_RANGE_PROC (0x2AU)   /**< @brief #DCACHE_SysInvalRange */
#define BRCM_SWARCH_DCACHE_SYS_FLUSH_RANGE_PROC (0x2BU)   /**< @brief #DCACHE_SysFlushRange */
/** @} */

/** @brief Clean D-Cache

    This API cleans complete D-Cache. This is an Atomic API; it disables
    the system interrupt cleans the complete DCache and enables the
    interrupt before returning. If D-Cache is not enabled, this API has
    no effect

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return void

    @post Cache and external memory are in sync

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations This API can only be called from user context (in
    unprivileged/thread context)
*/
extern void DCACHE_CleanAll(void);

/** @brief Clean D-Cache by address range

    This API cleans D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from user context (in unprivileged/thread context)
*/
extern void DCACHE_CleanRange(uint32_t aAddr, uint32_t aSize);

/** @brief Invalidate D-Cache by address range

    This API invalidates D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from user context (in unprivileged/thread context)
*/
extern void DCACHE_InvalRange(uint32_t aAddr, uint32_t aSize);

/** @brief Clean and invalidate D-Cache by address range

    This API cleans and invalidates D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void (for void functions)

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from user context (in unprivileged/thread context)
*/
extern void DCACHE_FlushRange(uint32_t aAddr, uint32_t aSize);

/** @brief Clean D-Cache

    This API cleans complete D-Cache. This is an Atomic API; it disables
    the system interrupt cleans the complete DCache and enables the
    interrupt before returning. If D-Cache is not enabled, this API has
    no effect

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return void

    @post Cache and external memory are in sync

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations This API can only be called from system context (in
    privileged/exception context)
*/
extern void DCACHE_SysCleanAll(void);

/** @brief Clean D-Cache by address range

    This API cleans D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from system context (in privileged/exception context)
*/
extern void DCACHE_SysCleanRange(uint32_t aAddr, uint32_t aSize);

/** @brief Invalidate D-Cache by address range

    This API invalidates D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from system context (in privileged/exception context)
*/
extern void DCACHE_SysInvalRange(uint32_t aAddr, uint32_t aSize);

/** @brief Clean and invalidate D-Cache by address range

    This API cleans and invalidates D-Cache for given address range.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aAddr       Address (must be aligned to cache-line boundary)
    @param[inout]   aSize       size of the memory block in bytes (must be multiple of
                                cache line size)

    @return void (for void functions)

    @post Cache and memory is in sync from aAddr for aSize

    @trace #BRCM_SWREQ_DCACHE_CORTEX

    @limitations aAddr must be aligned to cache-line boundary and this
    API can only be called from system context (in privileged/exception context)
*/
extern void DCACHE_SysFlushRange(uint32_t aAddr, uint32_t aSize);

#endif /* CACHE_H */
/** @} */
