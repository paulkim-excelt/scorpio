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
    @defgroup grp_bl_eth Ethernet Controller Driver
    @ingroup grp_bl_drivers

    @addtogroup grp_bl_eth
    @{
    @section sec_bl_eth_fsm State Machine
    @image html bl_eth_state_machine.jpg "Ethernet Driver State Machine"

    @section sec_bl_eth_seq Sequence Diagrams
    @image html bl_eth_seq_diagram.jpg "Ethernet Driver Use-case sequence"

    @file bl_ethernet.h
    @brief Ethernet Driver interface
    This header file contains the interface functions for Ethernet Driver

    @version 0.30 Imported from docx
*/

#ifndef BL_ETHERNET_H
#define BL_ETHERNET_H

#include <stddef.h>
#include <inttypes.h>

/**
    @name Ethernet Driver API IDs
    @{
    @brief API IDs for Ethernet Driver
*/
#define BRCM_ARCH_BL_ETHER_MAC_ADDR_SIZE_MACRO  (0x00U) /**< @brief #BL_ETHER_MAC_ADDR_SIZE */
#define BRCM_ARCH_BL_ETHER_TYPE_SIZE_MACRO      (0x01U) /**< @brief #BL_ETHER_TYPE_SIZE */
#define BRCM_ARCH_BL_ETHER_MIN_FRM_SIZE_MACRO   (0x02U) /**< @brief #BL_ETHER_MIN_FRM_SIZE */
#define BRCM_ARCH_BL_ETHER_HDR_SIZE_MACRO       (0x03U) /**< @brief #BL_ETHER_HDR_SIZE */
#define BRCM_ARCH_BL_ETHER_HW_ID_TYPE           (0x04U) /**< @brief #BL_ETHER_HwIDType */
#define BRCM_ARCH_BL_ETHER_CHAN_ID_TYPE         (0x05U) /**< @brief #BL_ETHER_ChanIDType */
#define BRCM_ARCH_BL_ETHER_STATE_TYPE           (0x06U) /**< @brief #BL_ETHER_StateType */
#define BRCM_ARCH_BL_ETHER_ETHER_TYPE           (0x07U) /**< @brief #BL_ETHER_EtherType */
#define BRCM_ARCH_BL_ETHER_VLANID_TYPE          (0x08U) /**< @brief #BL_ETHER_VLANIDType */
#define BRCM_ARCH_BL_ETHER_PRIO_TYPE            (0x09U) /**< @brief #BL_ETHER_PrioType */
#define BRCM_ARCH_BL_ETHER_RX_CLIENT_CB_PROC    (0x0AU) /**< @brief #BL_ETHER_RxClientCb */
#define BRCM_ARCH_BL_ETHER_INIT_PROC            (0x0BU) /**< @brief #BL_ETHER_Init */
#define BRCM_ARCH_BL_ETHER_GET_TX_BUFFER_PROC   (0x0CU) /**< @brief #BL_ETHER_GetTxBuffer */
#define BRCM_ARCH_BL_ETHER_SEND_PROC            (0x0DU) /**< @brief #BL_ETHER_Send */
#define BRCM_ARCH_BL_ETHER_RX_IND_PROC          (0x0EU) /**< @brief #BL_ETHER_RxInd */
#define BRCM_ARCH_BL_ETHER_PROCESS_PKTS_PROC    (0x0FU) /**< @brief #BL_ETHER_ProcessPkts */
/** @} */

/**
    @brief Ethernet MAC address size

    @trace #BRCM_ARCH_BL_ETHER_MAC_ADDR_SIZE_MACRO #BRCM_REQ_ETHER_MAC_ADDR
*/
#define BL_ETHER_MAC_ADDR_SIZE         (6UL)

/**
    @brief Ethernet type size

    @trace #BRCM_ARCH_BL_ETHER_TYPE_SIZE_MACRO #BRCM_REQ_ETHER_TYPE
*/
#define BL_ETHER_TYPE_SIZE             (2UL)

/**
    @brief Minimum Ethernet frame size

    @trace #BRCM_ARCH_BL_ETHER_MIN_FRM_SIZE_MACRO #BRCM_REQ_ETHER_FRM
*/
#define BL_ETHER_MIN_FRM_SIZE          (64UL)

/**
    @brief Ethernet header size

    @trace #BRCM_ARCH_BL_ETHER_HDR_SIZE_MACRO #BRCM_REQ_ETHER_HDR
*/
#define BL_ETHER_HDR_SIZE                \
    (2UL * BL_ETHER_MAC_ADDR_SIZE + BL_ETHER_TYPE_SIZE)

/**
    @brief Index of the Ethernet controller

    @trace #BRCM_ARCH_BL_ETHER_HW_ID_TYPE #BRCM_REQ_ETHER_HW_ID
*/
typedef uint32_t BL_ETHER_HwIDType;

/**
    @brief Index of Ethernet channel within a controller

    @trace #BRCM_ARCH_BL_ETHER_CHAN_ID_TYPE #BRCM_REQ_ETHER_CHAN_ID
*/
typedef uint32_t BL_ETHER_ChanIDType;

/**
    @name BL_ETHER_StateType
    @{
    @brief States of Ethernet driver

    @trace #BRCM_ARCH_BL_ETHER_STATE_TYPE #BRCM_REQ_ETHER_STATE
*/
typedef uint32_t BL_ETHER_StateType; /**< @brief typedef for Ethernet states */
#define BL_ETHER_STATE_UNINIT  (0UL) /**< @brief Uninitialised state */
#define BL_ETHER_STATE_ACTIVE  (1UL) /**< @brief Active State */
/** @} */

/**
    @name BL_ETHER_EtherType
    @{
    @brief Ethernet ethtype field in ETHER II packet

    @trace #BRCM_ARCH_BL_ETHER_ETHER_TYPE #BRCM_REQ_ETHER_TYPE
*/
typedef uint16_t BL_ETHER_EtherType;         /**< @brief ETHER_EtherType typedef */
#define BL_ETHER_ETHERTYPE_IP      (0x0800U) /**< @brief Ether Type IP */
#define BL_ETHER_ETHERTYPE_ARP     (0x0806U) /**< @brief Ether Type ARP */
#define BL_ETHER_ETHERTYPE_GPTP    (0x88F7U) /**< @brief Ether Type GPTP */
#define BL_ETHER_ETHERTYPE_1722    (0x22F0U) /**< @brief Ether Type 1722 */
#define BL_ETHER_ETHERTYPE_VLANTAG (0x8100U) /**< @brief Ether Type VLAN */
/** @} */

/**
    @name BL_ETHER_VLANIDType
    @{
    @brief 802.1Q VLAN ID type
    range: 0 to 4096
    0x000 and 0xFFF are reserved
    All other values may be used as VLAN ID

    @trace #BRCM_ARCH_BL_ETHER_VLANID_TYPE #BRCM_REQ_ETHER_VLANID
*/
typedef uint16_t BL_ETHER_VLANIDType;   /**< @brief typedef for Ethernet VLAN ID */
#define BL_ETHER_VLANID_MIN    (1UL)    /**< @brief VLAN ID minimum value */
#define BL_ETHER_VLANID_MAX    (4094UL) /**< @brief VLAN ID maximum value */
/** @} */

/**
    @name BL_ETHER_PrioType
    @{
    @brief Ethernet packet priority type
    (priority shall be between 0 and  #ETHER_PRIO_MAX)

    @trace #BRCM_ARCH_BL_ETHER_PRIO_TYPE #BRCM_REQ_ETHER_PRIO
*/
typedef uint16_t BL_ETHER_PrioType; /**< @brief typedef for Ethernet priority */
#define BL_ETHER_PRIO_MAX  (0x7UL)  /**< @brief Maximum packet priority */
/** @} */

/** @brief RX client callback type

    Each client shall register a callback in RX callback table to receive RX packets. This callback function is called
    whenever an Ethernet packet is received in the corresponding RX channel.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aBuf        Pointer to the packet buffer
    @param[in]      aLen        Length of the packet received

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_RX_CLIENT_CB_PROC  #BRCM_REQ_ETHER_RX_CLIENT_CB

    @limitations None
*/
typedef void (*BL_ETHER_RxClientCb) (BL_ETHER_HwIDType aID,
                                     const uint8_t *const aBuf,
                                     uint16_t aLen);

/** @brief Initialize Ethernet driver

    This API initializes the Ethernet channel and configures the TX channel descriptors. RX channel is initialized
    automatically during the very first TX channel initialization by the driver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aChan       TX Channel ID

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_INIT_PROC  #BRCM_REQ_ETHER_INIT

    @limitations None
*/
void BL_ETHER_Init(BL_ETHER_HwIDType aID,
                   BL_ETHER_ChanIDType aChan);

/** @brief Get transmit buffer

    This API provides free TX buffer to client.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aChan       TX Channel ID
    @param[out]     aBufIdx     Buffer index
    @param[out]     aBuf        Pointer to TX buffer
    @param[inout]   aLenInout   Length of the requested buffer (in),
                                Length of the allocated buffer (out)

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             TX buffer successfully obtained
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Invalid Ethernet controller ID
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Invalid TX channel ID
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aBufIdx is NULL
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aBuf is NULL
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aLenInOut is NULL
    @retval     #BL_BCM_ERR_NOMEM          No TX buffers are available
    @retval     #BL_BCM_ERR_INVAL_STATE    Invalid controller(Tx channel) state

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_GET_TX_BUFFER_PROC  #BRCM_REQ_ETHER_GET_TX_BUFFER

    @limitations None
*/
int32_t BL_ETHER_GetTxBuffer(BL_ETHER_HwIDType aID,
                             BL_ETHER_ChanIDType aChan,
                             uint32_t *const aBufIdx,
                             uint8_t **const aBuf,
                             uint32_t *const aLenInOut);

/** @brief Transmit an Ethernet packet

    This API transmit the packet on the given Ethernet TX channel.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aChan               TX channel ID
    @param[in]      aBufIdx             Buffer index (received by calling
                                        #ETHER_GetTxBuffer)
    @param[in]      aLen                Length of the data to be sent in bytes
                                        to the Ethernet driver.
    @param[in]      aDestMacAddr        Pointer to destination MAC address

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK                 Packet is transmitted successfully
    @retval     #BL_BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BL_BCM_ERR_INVAL_PARAMS       Invalid TX channel ID
    @retval     #BL_BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BL_BCM_ERR_INVAL_PARAMS       aLen is more than Ethernet frame size
    @retval     #BL_BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state
    @retval     #BL_BCM_ERR_BUSY               No room in DMA descriptor table
    @retval     #BL_BCM_ERR_INVAL_STATE        Invalid controller state

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_SEND_PROC  #BRCM_REQ_ETHER_SEND

    @limitations None
*/
int32_t BL_ETHER_Send(BL_ETHER_HwIDType aID,
                      BL_ETHER_ChanIDType aChan,
                      uint32_t aBufIdx,
                      uint32_t aLen);

/** @brief Ethernet RX packet indication callback

    This interface is called by the Ethernet driver for every RX packet received.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[in]      aBuf            Pointer to packet buffer
    @param[in]      aLen            Length of the packet

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_RX_IND_PROC  #BRCM_REQ_ETHER_RX_IND

    @limitations None
*/
extern void BL_ETHER_RxInd(BL_ETHER_HwIDType aID,
                           const uint8_t *const aBuf,
                           uint16_t aLen);

/** @brief Process the Ethernet packets

    This interface processes the transmitted and received Ethernet packets.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_ETHER_PROCESS_PKTS_PROC  #BRCM_REQ_ETHER_PROCESS_PKTS

    @limitations None
*/
extern void BL_ETHER_ProcessPkts(void);

#endif /* BL_ETHERNET_H */

/** @} */
