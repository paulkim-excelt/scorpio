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
    @addtogroup grp_mcudrv_ifc
    @{

    @file mcu_ext.h
    @brief MCU extension interface
    This header file contains the interface functions for MCU driver

    @version 0.86 Imported from docx
*/


#ifndef INCLUDE_BCM8956X_MCU_EXT_H
#define INCLUDE_BCM8956X_MCU_EXT_H

#include <stdint.h>
#include <mcu.h>

/**
    @name MCU API IDs
    @{
    @brief API IDs for MCU Driver
*/
#define BRCM_SWARCH_MCU_GETSWITCHPORT2TIMEFIFOMAP_PROC          (0x36U) /**< @brief #MCU_GetSwitchPort2TimeFifoMap */
#define BRCM_SWARCH_MCU_ENABLESWITCHCPUPORT_PROC                (0x37U) /**< @brief #MCU_EnableSwitchCPUPort */
#define BRCM_SWARCH_MCU_DISABLESWITCHCPUPORT_PROC               (0x38U) /**< @brief #MCU_DisableSwitchCPUPort */
#define BRCM_SWARCH_MCU_EXTENDEDINFO_TYPE                       (0x39U) /**< @brief #MCU_ExtendedInfoType */
#define BRCM_SWARCH_MCU_GETEXTENDEDINFO_PROC                    (0x3AU) /**< @brief #MCU_GetExtendedInfo */
/** @} */

/**
    @name Stacking Port information
    @{
    @brief Stacking Port information

    @trace #BRCM_SWREQ_MCU_QUERY_BCM8956X
*/
typedef struct _MCU_ExtendedInfoType {
    uint8_t            stackingEn; /**< @brief Stacking Enable, 0: Disable, 1: Enable */
    MCU_DeviceType     mstSlvMode; /**< @brief 0: Master, 1: Slave-1, 2: Slave-2      */
    uint8_t            stackPort0; /**< @brief Stacking Port 0 info                   */
    uint8_t            stackPort1; /**< @brief Stacking Port 1 info                   */
} MCU_ExtendedInfoType;
/** @} */


/** @brief Get Ethernet Switch port to Time hardware FIFO mapping

    This API retrieves Ethernet switch port to time hardware FIFO mapping as
    per the hardware configuration of the chip.

    @behavior Sync, reentrant

    @pre None

    @param[in]   aPort2TimeFifoMap   Pointer to array to retrieve switch port
                                     to time fifo map. The size of array is
                                     equal to #ETHERSWT_PORT_ID_MAX

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved switch port to
                                        time fifo map
    @retval     #BCM_ERR_UNINIT         MCU not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   aPort2TimeFifoMap is NULL


    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
extern int32_t MCU_GetSwitchPort2TimeFifoMap(uint32_t *const aPort2TimeFifoMap);

/** @brief Enable Switch CPU port

    This API enables Ethernet switch CPU port.

    @behavior Sync, reentrant

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Enabled Switch CPU port
    @retval     #BCM_ERR_UNINIT         MCU not initialized


    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
extern int32_t MCU_EnableSwitchCPUPort(void);

/** @brief Disable Switch CPU port

    This API disables Ethernet switch CPU port.

    @behavior Sync, reentrant

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Enabled Switch CPU port
    @retval     #BCM_ERR_UNINIT         MCU not initialized


    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
extern int32_t MCU_DisableSwitchCPUPort(void);

/** @brief Get stacking port info

    This API fetches Stacking port info from OTP.

    @behavior Sync, reentrant

    @pre None

    @param[in]   aStackingInfo   Pointer to structure containing stacking
                                 information.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Enabled Switch CPU port
    @retval     #BCM_ERR_UNKNOWN        Unknown issue with Otp read.
    @retval     #BCM_ERR_NOSUPPORT      Unsupported OTP configuarion.

    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8956X
*/
extern int32_t MCU_GetExtendedInfo(MCU_ExtendedInfoType * aStackingInfo);

#endif /* INCLUDE_BCM8956X_MCU_EXT_H */

/** @} */
