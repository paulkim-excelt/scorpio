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
    @defgroup grp_secureheap_ifc Interface
    @ingroup grp_secureheap

    @addtogroup grp_secureheap_ifc
    @{
    We provide a rather simple interface to work with the secure heap.
    The clients get to allocate memory from the secure heap and free it when
    they are done. They can pin their allocations to obtain the CPU and DMA
    addresses of the storage backing the said allocations. Unpinning invalidates
    those CPU and DMA addresses.

    The library is statically initialized and therefore, there is no runtime API to
    perform any initialization operations.

    @file secure_heap.h
    @brief Interface File
    This header file contains the interface functions for the secure heap
    @version 0.1 Initial Version
*/

#ifndef SECURE_HEAP_H
#define SECURE_HEAP_H

#include <stdint.h>

/**
    @name Secure heap Architecture IDs
    @{
    @brief Architecture IDs for the secure heap
*/
#define BRCM_SWARCH_SECHEAP_ALLOC_PROC          (0x80U) /**< @brief #SECHEAP_Alloc          */
#define BRCM_SWARCH_SECHEAP_FREE_PROC           (0x81U) /**< @brief #SECHEAP_Free           */
#define BRCM_SWARCH_SECHEAP_PIN_PROC            (0x82U) /**< @brief #SECHEAP_Pin            */
#define BRCM_SWARCH_SECHEAP_UNPIN_PROC          (0x83U) /**< @brief #SECHEAP_Unpin          */
/*  * @} */

/** @brief Allocate memory from the secure heap

    Allocates memory from the secure heap.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aAllocSize               Memory chunk size to allocate.
    @param[in]       aAllocFlags              Allocation flags, ignored currently.
    @param[out]      aAllocHdl                Handle to the allocated memory.

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              On success. Valid handle is written to aAllocHdl
                                              pointer
    @retval          #BCM_ERR_NOMEM           No free memory left in the pool that supports
                                              an allocation of size aAllocSize
    @retval          #BCM_ERR_INVAL_PARAMS    aAllocSize too large or a NULL aAllocHdl
                                              passed in

    @post aAllocHdl represents a valid allocation inside the secure heap. The underlying memory
            shall be marked allocated.

    @trace #BRCM_SWREQ_SECHEAP
*/
int32_t SECHEAP_Alloc(const uint32_t aAllocSize, const uint32_t aAllocFlags, uint32_t *const aAllocHdl);

/** @brief Free the previously allocated memory from the secure heap

    Frees the previously allocated memory secure heap

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aAllocHdl                Handle corresponding to the
                                              allocation to be freed

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              On success
    @retval          #BCM_ERR_INVAL_PARAMS    aAllocHdl is not a valid handle

    @post aAllocHdl is no longer valid. The underlying memory is marked free for a
            future allocation.

    @trace #BRCM_SWREQ_SECHEAP
*/
int32_t SECHEAP_Free(const uint32_t aAllocHdl);

/** @brief Pin the allocation

    Pins the allocation, returns the CPU and DMA addresses to access the contents
    of the allocation. Might perform any cache maintainance operations if the
    platform calls for it.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aAllocHdl                Handle corresponding to the allocation
                                              to be pinned
    @param[out]      aAllocCpuAddr            CPU address to access the contents of the
                                              allocation. If NULL, pin operation is completed
                                              anyway, but the CPU address is not returned to the
                                              caller.
    @param[out]      aAllocDmaAddr            DMA address to access the contents of the
                                              allocation. If NULL, pin operation is completed
                                              anyway, but the CPU address is not returned to the
                                              caller.

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              On success
    @retval          #BCM_ERR_INVAL_PARAMS    aAllocHdl is not a valid handle

    @post Valid CPU and DMA addresses are filled into the pointers provided.

    @trace #BRCM_SWREQ_SECHEAP
*/
int32_t SECHEAP_Pin(const uint32_t aAllocHdl, uint8_t **const aAllocCpuAddr, uint32_t *const aAllocDmaAddr);

/** @brief Unpin the allocation

    Unpins a previously pinned allocation.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]       aAllocHdl                Handle corresponding to the allocation
                                              to be unpinned.

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              On success
    @retval          #BCM_ERR_INVAL_PARAMS    aAllocHdl is not a valid handle

    @post The CPU and DMA addresses returned by the pin operation are no longer valid

    @trace #BRCM_SWREQ_SECHEAP
*/
int32_t SECHEAP_Unpin(const uint32_t aAllocHdl);

#endif
/*  * @} */

