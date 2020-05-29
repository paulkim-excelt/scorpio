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
    @defgroup grp_mpu_ifc MPU Table
    @ingroup grp_cortex

    @addtogroup grp_mpu_ifc
    @{
    @section sec_mpu_overview Overview
    @todo update details

    @file mpu.h
    @brief MPU Table structures and macros
    This header file contains the interface macros and structures for MPU
    configuration

    @version 0.1 Initial Version
*/

#ifndef MPU_H
#define MPU_H

#include <stdint.h>

/**
    @name MPU Table Architecture IDs
    @{
    @brief Architecture IDs for MPU Table

    @note If any new interface (macro/typedef/function or any other) is being
    added, the corresponding API ID shall be added at the end of the  Component
    Architecture ID list, immaterial of where the interface is placed in the header.
    This is to ensure that the existing SWARCH IDs remains unchanged.
*/
#define BRCM_SWARCH_CORTEX_MPU_REGION_TYPE  (0x00U)   /**< @brief #COMP_CONFIG      */
/** @} */

/**
    @brief MPU Memory Region Attributes

    Structure containing the attributes of a memory region in MPU

    @trace #BRCM_SWREQ_CORTEX_MX
 */
typedef struct {
       uint32_t baseAddr;   /**< @brief Base address of the region */
       uint32_t size;       /**< @brief size of region (including all
                                    sub regions) */
       uint32_t attr;       /**< @brief Attributes of the Region
                                as described in # */
} MPU_RegionType;

#endif  /* MPU_H */

/** @} */
