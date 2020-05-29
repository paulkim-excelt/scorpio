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
    @defgroup grp_bl_eth_xcvr Ethernet Transceiver Driver
    @ingroup grp_bl_drivers

    @addtogroup grp_bl_eth_xcvr
    @{

    @file bl_eth_xcvr.h
    @brief Ethernet Transceiver interface
    This header file contains the interface functions for Ethernet Transceiver

    @version 0.30 Imported from docx
*/

#ifndef BL_ETH_XCVR_H
#define BL_ETH_XCVR_H

#include <stddef.h>
#include <inttypes.h>
#include <bl_bcm_err.h>

/**
    @name Ethernet Transceiver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_ARCH_BL_ETHER_XCVR_ID_TYPE            (0x10U) /**< @brief #BL_ETHER_XcvrIDType */
#define BRCM_ARCH_BL_ETHER_XCVR_SPEED_TYPE         (0x11U) /**< @brief #BL_ETHER_XcvrSpeedType */
#define BRCM_ARCH_BL_ETHER_XCVR_DUPLEX_MODE_TYPE   (0x12U) /**< @brief #BL_ETHER_XcvrDuplexMode */
#define BRCM_ARCH_BL_ETHER_PHY_CONN_MODE_TYPE      (0x13U) /**< @brief #BL_ETHER_PHYConnModeType */
#define BRCM_ARCH_BL_ETHER_XCVR_CFG_TYPE           (0x14U) /**< @brief #BL_ETHER_XcvrCfgType */
#define BRCM_ARCH_BL_ETHER_XCVR_INIT_PROC          (0x15U) /**< @brief #BL_ETHER_XcvrInit */
#define BRCM_ARCH_BL_ETHER_XCVR_SET_MAC_ADDR_PROC  (0x16U) /**< @brief #BL_ETHER_XcvrSetMacAddr */
#define BRCM_ARCH_BL_ETHER_XCVR_GET_MAC_ADDR_PROC  (0x17U) /**< @brief #BL_ETHER_XcvrGetMacAddr */
/** @} */

/**
    @name BL_ETHER_XcvrIDType
    @{
    @brief Ethernet transceiver ID

    @trace #BRCM_ARCH_BL_ETHER_XCVR_ID_TYPE #BRCM_REQ_ETHER_XCVR_ID
*/
typedef uint32_t BL_ETHER_XcvrIDType;                       /**< @brief typedef for Ethernet transceiver ID */
#define BL_ETHER_XCVR_HW_ID_0      (0UL)                    /**< @brief Transceiver ID0 */
#define BL_ETHER_XCVR_HW_ID_MAX    (BL_ETHER_XCVR_HW_ID_0)  /**< @brief Transceiver max ID */
/** @} */

/**
    @name BL_ETHER_XcvrSpeedType
    @{
    @brief Ethernet transceiver speed

    @trace #BRCM_ARCH_BL_ETHER_XCVR_SPEED_TYPE #BRCM_REQ_ETHER_XCVR_SPEED
*/
typedef uint32_t BL_ETHER_XcvrSpeedType;     /**< @brief ETHER_XcvrSpeed typedef */
#define BL_ETHER_XCVR_SPEED_10MBPS     (1UL) /**< @brief 10 Megabits per second */
#define BL_ETHER_XCVR_SPEED_100MBPS    (2UL) /**< @brief 100 Megabits per second */
#define BL_ETHER_XCVR_SPEED_1000MBPS   (4UL) /**< @brief 1000 Megabits per seconds */
/** @} */

/**
    @name BL_ETHER_XcvrDuplexModeType
    @{
    @brief Transceiver duplex mode

    @trace #BRCM_ARCH_BL_ETHER_XCVR_DUPLEX_MODE_TYPE #BRCM_REQ_ETHER_XCVR_DUPLEX_MODE
*/
typedef uint32_t BL_ETHER_XcvrDuplexMode;      /**< @brief ETHER_XcvrDuplexMode typedef */
#define BL_ETHER_XCVR_DUPLEX_MODE_FULL (0x0UL) /**< @brief Full duplex */
#define BL_ETHER_XCVR_DUPLEX_MODE_HALF (0x1UL) /**< @brief Half duplex */
/** @} */

/**
    @name BL_ETHER_PhyConnModeType
    @{
    @brief PHY connection mode

    @trace #BRCM_ARCH_BL_ETHER_PHY_CONN_MODE_TYPE #BRCM_REQ_ETHER_PHY_CONN_MODE
*/
typedef uint32_t BL_ETHER_PHYConnModeType;     /**< @brief ETHER_PhyConnMode typedef */
#define BL_ETHER_PHY_CONN_MODE_MASTER  (0x0UL) /**< @brief PHY Master mode */
#define BL_ETHER_PHY_CONN_MODE_SLAVE   (0x1UL) /**< @brief PHY Slave mode */
/** @} */

/**
    @brief Transceiver configuration structure

    @trace #BRCM_ARCH_BL_ETHER_XCVR_CFG_TYPE #BRCM_REQ_ETHER_XCVR_CFG_STRUCTURE
 */
typedef struct {
    BL_ETHER_XcvrSpeedType speed; /**< @brief transceiver speed */
    BL_ETHER_XcvrDuplexMode mode; /**< @brief transceiver duplex mode */
    uint32_t maxRxFrmLen;         /**< @brief Maximum size of RX frame length.
    @note maxRxFrmLen shall include FCS size. e.g.
    Ethernet II frame size = [12 (mac src + dest addr) + 2 (ether type) + 1500 (payload) + 4 (VLAN tag) + 4 (CRC)]
                           = 1522 Bytes */
    const uint8_t *macAddr;       /** @brief MAC address in network byte order */
} BL_ETHER_XcvrCfgType;

/** @brief Initialize transceiver

    This API initializes the transceiver with the given configuration.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[in]      aConfig     Pointer to transceiver configuration

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_XCVR_INIT_PROC  #BRCM_REQ_ETHER_XCVR_INIT

    @limitations None
*/
void BL_ETHER_XcvrInit(BL_ETHER_XcvrIDType aID,
                       const BL_ETHER_XcvrCfgType *const aConfig);

/** @brief Set MAC address

    This API sets MAC address (Ethernet address) of the given Ethernet controller

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aMacAddr    Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MAC address is set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_XCVR_SET_MAC_ADDR_PROC  #BRCM_REQ_ETHER_SET_MAC_ADDR

    @limitations None
*/
int32_t BL_ETHER_XcvrSetMacAddr(BL_ETHER_HwIDType aID,
                                const uint8_t *const aMacAddr);

/** @brief Get MAC address

    This API retrieves MAC address (Ethernet address) of the given Ethernet controller.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[out]     aMacAddr    Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MAC address is retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_XCVR_GET_MAC_ADDR_PROC  #BRCM_REQ_ETHER_GET_MAC_ADDR

    @limitations None
*/
int32_t BL_ETHER_XcvrGetMacAddr(BL_ETHER_HwIDType aID,
                                uint8_t *const aMacAddr);

#endif /* BL_ETH_XCVR_H */

/** @} */
