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
    @defgroup grp_memcdrv_ifc Interface
    @ingroup grp_memcdrv

    @addtogroup grp_memcdrv_ifc
    @{
    @section sec_memc_overview Overview
    @file memc.h
    @brief DDR Memory controller driver application interface
    This header file contains the interfaces for DDR MEMC driver
    @version 0.30 Imported from docx
*/

#ifndef MEMC_H
#define MEMC_H

#include <system.h>
/**
    @name DDR Memory Controller driver API IDs
    @{
    @brief API IDs for DDR Memory Controller driver
*/
#define BRCM_SWARCH_MEMC_HWID_TYPE             (0x00U) /**< @brief #MEMC_HwIDType     */
#define BRCM_SWARCH_MEMC_STATE_TYPE            (0x01U) /**< @brief #MEMC_StateType    */
#define BRCM_SWARCH_MEMC_INIT_PROC             (0x02U) /**< @brief #MEMC_Init         */
#define BRCM_SWARCH_MEMC_GET_STATE_PROC        (0x03U) /**< @brief #MEMC_GetState     */
/** @} */

/**
    @trace #BRCM_SWREQ_MEMC_KERNEL_INTERFACE
*/
typedef uint32_t MEMC_HwIDType;

/** @name DDR Memc state IDs
    @{
    @brief DDR Memc state type

    @trace #BRCM_SWREQ_MEMC_KERNEL_INTERFACE
*/
typedef uint32_t MEMC_StateType;
#define MEMC_STATE_UNINIT        (0UL)
#define MEMC_STATE_INIT          (1UL)
/** @} */

/** @brief Initialize DDR Memory Controller

    This API initializes the DDR Memory Controller 

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aId         Index of the DDR controller

    @return         void

    @post TBD

    @trace #BRCM_SWREQ_MEMC_INIT

    @limitations None
*/
extern void MEMC_Init(MEMC_HwIDType aID);

/** @brief Get the state of the DDR Memory controller
    This API retrieves the current state of the DDR controller in aState
    argument

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aId         Index of the DDR controller
    @param[out]     aState      Current state of DDR MEMC

    @return         void

    @post None

    @trace  #BRCM_SWREQ_MEMC_INIT

    @limitations None
*/
int32_t MEMC_GetState(MEMC_HwIDType aId,
                              MEMC_StateType *const aState);

#endif /* MEMC_H */
/** @} */
