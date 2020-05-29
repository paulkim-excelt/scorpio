/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_host_ether Host Ethernet
    @ingroup grp_host_comms

    @addtogroup grp_host_ether
    @{

    @file host_ether.h
    @brief Host Ethernet Sub-group API
    This header file contains the host interface functions for Ethernet Sub-group

    @version 1.0 Initial Version
*/

#ifndef HOST_ETHER_H
#define HOST_ETHER_H

#include <host_system.h>
#include <ether_ipc.h>

/** @brief Synchronise all the stacked targets

    This API is meant to synchronise the time blocks of all the stacked boards

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Stacked targets synchronised successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid spiId pointer
    @return     #BCM_ERR_INVAL_STATE    Unable to configure GPIO
    @return     #BCM_ERR_NOSUPPORT      Not enough active stacked boards to synchronise
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherStackSync(MgmtInfoType *info);

/** @brief Read a register through MDIO

    This API is meant to read a register through MDIO in CL22 or CL45 modes

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      access          Access type - CL22 or CL45
    @param[in]      mdioHwID        MDIO controller Hardware ID
    @param[in]      phy             Phy address
    @param[in]      dev             Dev address
    @param[in]      reg             Register address
    @param[out]     val             Pointer to store read value
    @param[in]      destn           Master or Slave device

    @return     #BCM_ERR_OK             Register value read succesfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_MDIORead(MgmtInfoType *info, MDIO_RegAccessType access,
                             MDIO_HwIDType mdioHwID, MDIO_SlaveAddrType phy,
                             MDIO_DeviceType dev, uint16_t reg, uint16_t *val,
                             MCU_DeviceType destn);

/** @brief Write to a register through MDIO

    This API is meant to write to a register through MDIO in CL22 or CL45 modes

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      access          Access type - CL22 or CL45
    @param[in]      mdioHwID        MDIO controller Hardware ID
    @param[in]      phy             Phy address
    @param[in]      dev             Dev address
    @param[in]      reg             Register address
    @param[in]      val             Value to be written to the register
    @param[in]      destn           Master or Slave device

    @return     #BCM_ERR_OK             Register written succesfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_MDIOWrite(MgmtInfoType *info, MDIO_RegAccessType access,
                              MDIO_HwIDType mdioHwID, MDIO_SlaveAddrType phy,
                              MDIO_DeviceType dev, uint16_t reg, uint16_t val,
                              MCU_DeviceType destn);

/** @brief Ethernet Sub-group Notification Handler

    This API is meant to handle asynchronous notifications sent by the Ethernet Sub-group on target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      currentSlave    Slave ID
    @param[in]      notificationId  Notification ID
    @param[in]      msg             Message buffer pointer
    @param[in]      size            Size of the message buffer "msg" in bytes

    @return     #BCM_ERR_OK             Notification handled successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid notificationId
    @return     #BCM_ERR_INVAL_PARAMS   Invalid msg pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid size

    @post None

    @limitations None
*/
extern int32_t HOST_EtherNotificationHandler(uint32_t currentSlave,
                uint8_t notificationId, uint8_t *msg, uint32_t size);

#endif /* HOST_ETHER_H */

/** @} */
