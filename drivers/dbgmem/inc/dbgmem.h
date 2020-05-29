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
    @defgroup grp_dbgmemdrv_ifc Interface
    @ingroup grp_dbgmemdrv

    @addtogroup grp_dbgmemdrv_ifc
    @{
    @section sec_dbgmem_overview Overview
    DBGMEM driver is a debug memory driver used to read-from/write-to peripheral
    registers or privileged memory.

    @section sec_dbgmem_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_dbgmem.dep

    @limitations Support 8/16/32 bit accesses only

    |   References                                                          |
    |-----------------------------------------------------------------------|
    |   @anchor ref_uml_spec UML 2.5 specification                          |
    |   @anchor ref_sads_spec Software Architecture and Design Specification|
    |   @anchor ref_trm_spec Technical Reference Manual                     |
    |   @anchor ref_utilities_spec Utilities Interface Specification        |

    @file dbgmem.h
    @brief Interface
    This header file contains the interface functions for DBGMEM Driver

    @version 0.1 Initial draft
*/


#ifndef DBGMEM_H
#define DBGMEM_H

#include <stdint.h>
#include <stdlib.h>
#include <bcm_err.h>
#include <system.h>
#include <inttypes.h>

/**
    @name DBGMEM Driver Architecture IDs
    @{
    @brief Architecture IDs for DBGMEM Driver
*/
#define BRCM_SWARCH_DBGMEM_HANDLE_TYPE      (0x00U) /**< @brief #DBGMEM_HandleType */
#define BRCM_SWARCH_DBGMEM_COPY_PROC        (0x01U) /**< @brief #DBGMEM_Copy */
/** @} */

/**
    @brief DBGMEM Handle

    @trace #BRCM_SWREQ_DBGMEM
 */
typedef union _DBGMEM_HandleType {
    uint32_t u32Val;            /**< @brief 32-bit memory address */
    volatile uint8_t *u8Ptr;    /**< @brief 8-bit memory access handle */
    volatile uint16_t *u16Ptr;  /**< @brief 16-bit memory access handle */
    volatile uint32_t *u32Ptr;  /**< @brief 32-bit memory access handle */
} DBGMEM_HandleType;

/** @brief Copy memory

    This API is used to read-from or write-to memory/peripheral register.
    Multiple accesses are performed on consecutive ones based on the access
    width chosen.
    Examples:
    1. Read can be performed by specifying the read start address as the source
    pointer and by providing a buffer, to read-to, as the destination pointer.
    2. Write can be performed by providing a buffer with content to write-to a
    contiguous chun of memory/peripheral registers specified as the destination
    start address.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aSrc        Source start address
    @param[out]     aDst        Destination start address
    @param[in]      aWidth      Width of accesses
    @param[in]      aLength     Number of accesses of aWidth

    @retval     #BCM_ERR_OK             Copy successful
    @retval     #BCM_ERR_INVAL_PARAMS   aWidth is not 8 or 16 or 32
    @retval     #BCM_ERR_INVAL_PARAMS   aLength is 0

    @post None

    @trace #BRCM_SWREQ_DBGMEM

    @limitations None
*/
extern int32_t DBGMEM_Copy(uint32_t aSrc, uint32_t aDst,
                            uint32_t aWidth, uint32_t aLength);

#endif /* DBGMEM_H */

/** @} */
