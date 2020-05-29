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
    @defgroup grp_system_sw_maccfg MAC Configuration
    @ingroup grp_system_sw_cfg

    @addtogroup grp_system_sw_maccfg
    @{

    @file syscfg.h
    @brief System configuration header

    @version 0.9 Imported from document
*/

#ifndef SYSCFG_H
#define SYSCFG_H

#if defined(ENABLE_DMA)
#include <dma.h>
#endif

/** System Configuration structure */
typedef struct _SysCfg_EthConfigType {
    uint8_t ethMacAddr[6];  /**< Ethernet MAC Address */
} SysCfg_EthConfigType;


/**
 * @brief System Configuration Initialization
 *
 * This API is used to load and save the system configuration read from the
 * flash
 *
 * @param[in]   aEvent                  Event used for loading image
 *
 * @return      void
 */
extern void SysCfg_Init(uint32_t aEvent);

/**
 * @brief Get the system configuration for Ethernet
 *
 * This API is used to read the saved system configuration
 *
 * @param[in]   aSysCfg                 Pointer to system configuration
 *
 * @retval      BCM_ERR_OK              Success
 *              BCM_ERR_INVAL_PARAMS    aSysCfg is NULL
 *              BCM_ERR_INVAL_MAGIC     Invalid system configuration
 */
extern int32_t SysCfg_GetEthConfig(SysCfg_EthConfigType *const aSysCfg);

#if defined(ENABLE_DMA)
/**
 * @brief Get DMA Channel ID
 *
 * This API is used to Get DMA Channel ID for a given component
 *
 * @param[in]   aCompID                 Component ID requesting the DMA channel
 * @param[in]   aInstanceID             Instance ID of the component
 * @param[in]   aXferType               Transfer type needed by component
 *
 * @retval      uint32_t                Channel number that component can use
 */
extern uint32_t SysCfg_GetDMAChannel(uint64_t aCompID, uint32_t aInstanceID,
                        DMA_XferType aXferType);
#endif

#endif /* SYSCFG_H */

/** @}*/
