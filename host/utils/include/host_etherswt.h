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
    @defgroup grp_host_etherswt Host Ethernet Switch
    @ingroup grp_host_comms

    @addtogroup grp_host_etherswt
    @{

    @file host_etherswt.h
    @brief Host Ethernet Switch Sub-Group API
    This header file contains the host interface functions for Ethernet Switch Sub-Group

    @version 1.0 Initial Version
*/

#ifndef HOST_ETHERSWT_H
#define HOST_ETHERSWT_H

#include <host_system.h>
#include <etherswt_ipc.h>

/** @brief Get Port Information

    This API is used to retrieve port specific information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     portInfo        Pointer to #ETHERSWT_PortInfoType

    @return     #BCM_ERR_OK             Port information obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid portInfo pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtPortInfoGet(MgmtInfoType *info, uint32_t port,
        ETHERSWT_PortInfoType *portInfo);

/** @brief Set Port information

    This API is used to set port specific information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      portInfo        Pointer to #ETHERSWT_PortInfoType

    @return     #BCM_ERR_OK             Port info set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid portInfo pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtPortInfoSet(MgmtInfoType *info,
        ETHERSWT_PortInfoType *portInfo);

/** @brief Get port MIB counters

    This API is used to retrieve port specific MIB counters

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     mib             Pointer to #ETHERSWT_MibType

    @return     #BCM_ERR_OK             MIB counters obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid mib pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtMib(MgmtInfoType *info, int32_t port,
        ETHERSWT_MibType *mib);

/** @brief Clear port mib counters

    This API is used to clear port specific mib counters

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number

    @return     #BCM_ERR_OK             MIB counters cleared successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtMibClear(MgmtInfoType *info, int32_t port);

/** @brief Read Switch register

    This API is used to read switch register

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      destn           Reg read from master or slave
    @param[in]      addr            Address of register
    @param[in]      len             Number of registers from addr to read
    @param[out]     data            Pointer to buffer to read contents of registers

    @return     #BCM_ERR_OK             Register(s) read successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid data pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations
    - len should be less than or equal to 16
    - Accesses are 64bit wide
*/
extern int32_t HOST_EtherSwtRegRead(MgmtInfoType *info,  MCU_DeviceType destn,
                                    uint32_t addr, uint32_t len, uint64_t *data);

/** @brief Write to switch register

    This API is used to write to switch register

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      destn           Reg read from master or slave
    @param[in]      addr            Address of register
    @param[in]      len             Number of registers from addr to write to
    @param[out]     data            Pointer to data to be written to registers

    @return     #BCM_ERR_OK             Register(s) written successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid data pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations
    - len should be less than or equal to 16
    - Accesses are 64bit wide
*/
extern int32_t HOST_EtherSwtRegWrite(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                                     uint32_t len, uint64_t *data);

/** @brief Get switch age time

    This API is used to retrieve switch age time

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     ageTime         Pointer to get ageTime value

    @return     #BCM_ERR_OK             Agetime obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid ageTime pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtAgeTimeGet(MgmtInfoType *info, uint32_t *ageTime);

/** @brief Get Dumb forwarding mode

    This API is used to retrieve dumb forwarding mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     dumbFwdMode     Pointer to #ETHERSWT_DumbFwdModeType

    @return     #BCM_ERR_OK             Dumb forwarding mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid dumbFwdMode pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtDumbFwdModeGet(MgmtInfoType *info,
        ETHERSWT_DumbFwdModeType *dumbFwdMode);

/** @brief Enable mirroring

    This API is used to enable mirroring

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      portMask        Port bitmask
    @param[in]      probePort       Probe port
    @param[in]      direction       Direction of traffic to mirror

    @return     #BCM_ERR_OK             Mirroring enabled successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid direction
    @return     #BCM_ERR_INVAL_PARAMS   probePort is part of portMask
    @return     #BCM_ERR_INVAL_PARAMS   portMask is 0 and is not a stacked
                                        configuration
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtMirrorEnable(MgmtInfoType *info, uint16_t portMask,
        uint32_t probePort, ETHERSWT_TrafficDirType direction);

/** @brief Disable mirroring

    This API is used to disable mirroring

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Mirroring disabled successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtMirrorDisable(MgmtInfoType *info);

/** @brief Get mirror status

    This API is used to retrieve mirroring status

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     state           Pointer to get mirroring state
    @param[out]     ingressPortMask Pointer to get ingress port bitmask
    @param[out]     egressPortMask  Pointer to get egress port bitmask
    @param[out]     probePort       Pointer to get probe port number

    @return     #BCM_ERR_OK             Mirroring status obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtMirrorStatus(MgmtInfoType *info, uint32_t *state,
        uint16_t *ingressPortMask, uint16_t *egressPortMask, uint32_t *probePort);

/** @brief Set switch age time

    This API is used to set switch age time

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      ageTime         Value of age time

    @return     #BCM_ERR_OK             Age time set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtAgeTimeSet(MgmtInfoType *info, uint32_t ageTime);

/** @brief Get switch information

    This API is used to get global switch information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     switchInfo      Pointer to get switch information

    @return     #BCM_ERR_OK             Switch information obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid switchInfo pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtSwitchInfoGet(MgmtInfoType *info,
        ETHERSWT_SwitchInfoType *switchInfo);

/** @brief Set switch information

    This API is used to set global switch information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      switchInfo      Pointer to set switch information

    @return     #BCM_ERR_OK             Switch information set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid switchInfo pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtSwitchInfoSet(MgmtInfoType *info,
        ETHERSWT_SwitchInfoType *switchInfo);

/** @brief Get port administrative mode

    This API is used to get port administrative mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     adminMode       Pointer to get administrative mode

    @return     #BCM_ERR_OK             Administrative mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adminMode pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrAdminModeGet(MgmtInfoType *info, uint32_t port,
                                             ETHXCVR_ModeType *adminMode);

/** @brief Get port speed

    This API is used to get port speed

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     speed           Pointer to get speed

    @return     #BCM_ERR_OK             Port speed obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid speed pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrSpeedGet(MgmtInfoType *info, uint32_t port,
        ETHXCVR_SpeedType *speed);

/** @brief Get port master/slave mode

    This API is used to get port master/slave mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     masterSlave     Pointer to get master/slave mode

    @return     #BCM_ERR_OK             Port master/slave mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid masterSlave pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrMasterSlaveGet(MgmtInfoType *info,
        uint32_t port, ETHXCVR_BooleanType *masterSlave);

/** @brief Get port phy loop-back mode

    This API is used to get port phy loop-back mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     loopBackMode    Pointer to get loop-back mode

    @return     #BCM_ERR_OK             Loop-back mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid loopBackMode pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrPhyLbGet(MgmtInfoType *info, uint32_t port,
        ETHXCVR_BooleanType *loopBackMode);

/** @brief Get Singal Quality Indicator

    This API is used to get link Signal Quality Indicator

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     sqi             Pointer to obtain SQI value

    @return     #BCM_ERR_OK             Link SQI obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid sqi pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrLinkSQIGet(MgmtInfoType *info, uint32_t port,
        uint32_t *sqi);

/** @brief Get Jumbo frame mode

    This API is used to get port specific jumbo frame mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     jumboFrameMode  Pointer to obtain jumbo frame mode

    @return     #BCM_ERR_OK             Jumbo frame mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid jumboFrameMode pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrJumboFrameModeGet(MgmtInfoType *info,
        uint32_t port, ETHXCVR_BooleanType *jumboFrameMode);

/** @brief Get port type

    This API is used to get port type

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     busMode         Pointer to obtain bus mode
    @param[out]     phyMedia        Pointer to obtain phy media

    @return     #BCM_ERR_OK             Port type obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid type pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrTypeGet(MgmtInfoType *info,
                                        uint32_t port,
                                        ETHXCVR_BusModeType *busMode,
                                        ETHXCVR_PhyMediaType *phyMedia);

/** @brief Set dumb forwarding mode

    This API is used to set dumb forwarding mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      dumbFwdMode     Dumb forwarding mode value

    @return     #BCM_ERR_OK             Dumb forwarding mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid dumbFwdMode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtDumbFwdModeSet(MgmtInfoType *info,
        ETHERSWT_DumbFwdModeType dumbFwdMode);

/** @brief Set port administrative mode

    This API is used to set port administrative mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      admin_mode      Administrative mode

    @return     #BCM_ERR_OK             Port administrative mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid admin_mode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrAdminModeSet(MgmtInfoType *info, uint32_t port,
                                             ETHXCVR_ModeType admin_mode);

/** @brief Set port master/slave mode

    This API is used to set port master/slave mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port numberr
    @param[in]      brMode          Master/slave mode

    @return     #BCM_ERR_OK             Master/slave mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid brMode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrMasterSlaveSet(MgmtInfoType *info,
        uint32_t port, ETHXCVR_BooleanType brMode);

/** @brief Set Phy loop-back mode

    This API is used to set phy loop-back mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     loopBackMode    Loop-back mode

    @return     #BCM_ERR_OK             Loop-back mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid loopBackMode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrPhyLbSet(MgmtInfoType *info, uint32_t port,
                                         ETHXCVR_BooleanType loopBackMode);

/** @brief Set Jumbo frame mode

    This API is used to set port specific jumbo frame mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     jumboFrameMode  Jumbo frame mode

    @return     #BCM_ERR_OK             Jumbo frame mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid jumboFrameMode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtXcvrJumboFrameModeSet(MgmtInfoType *info,
        uint32_t port, ETHXCVR_BooleanType jumboFrameMode);

/** @brief Set Pvid and priority

    This API is used to set Pvid and priority

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      pvid            PVID value
    @param[in]      prio            Priority

    @return     #BCM_ERR_OK             Pvid and priority set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtVlanPvidSet(MgmtInfoType *info, uint32_t port,
        uint32_t pvid, uint32_t prio);

/** @brief Get VLAN information

    This API is used to get VLAN information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      vlan            VLAN ID
    @param[out]     portMask        Pointer to obtain port bitmask
    @param[out]     tagMask         Pointer to obtain tagged port bitmask
    @param[out]     staticPortMask      Pointer to obtain static port bitmask

    @return     #BCM_ERR_OK             VLAN information obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid portMask pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid tagMask pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtVlanGet(MgmtInfoType *info, uint16_t vlan, uint32_t *portMask,
        uint32_t *tagMask, uint32_t *staticPortMask);

/** @brief VLAN addition

    This API is used to add a VLAN

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      vlan            VLAN ID
    @param[in]      portMask        Port bitmask
    @param[in]      tagMask         Tagged port bitmask

    @return     #BCM_ERR_OK             VLAN added successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtVlanPortAdd(MgmtInfoType *info, uint16_t vlan,
        uint32_t portMask, uint32_t tagMask);

/** @brief VLAN deletion

    This API is used to delete a VLAN

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      vlan            VLAN ID
    @param[in]      portMask        Port bitmask

    @return     #BCM_ERR_OK             VLAN deleted successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtVlanPortDel(MgmtInfoType *info, uint16_t vlan, uint32_t portMask);

/** @brief ARL addition

    This API is used to add an ARL entry

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to an character buffer MAC address
    @param[in]      vlan            VLAN ID
    @param[in]      partMask        Port bitmask

    @return     #BCM_ERR_OK             ARL added successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtARLAdd(MgmtInfoType *info, uint8_t *mac_addr,
        uint16_t vlan, uint32_t portMask);

/** @brief ARL Deletion

    This API is used to delete an ARL entry

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to an character buffer MAC address
    @param[in]      vlan            VLAN ID

    @return     #BCM_ERR_OK             ARL deleted successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtARLDelete(MgmtInfoType *info, uint8_t *mac_addr,
        uint16_t vlan);

/** @brief ARL Snapshot

    This API is used to trigger copying the ARL buffer from hardware to target's
    local buffer

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     count           Pointer to get the number of entries cached

    @return     #BCM_ERR_OK             ARL snapshot completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtARLSnapshot(MgmtInfoType *info, uint32_t *count);

/** @brief Get ARL entries

    This API is used to retrieve ARL entries from target's local buffer

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     entry           Pointer to an array of
                                    #ETHERSWT_ARLEntryType entries
    @param[inout]   count           Maximum number of entries that the caller
                                    can accomodate and the actual number of
                                    entries that the callee has filled

    @return     #BCM_ERR_OK             ARL entries obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid entry pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid count pointer
    @return     #BCM_ERR_INVAL_PARAMS   Count is 0
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtARLGet(MgmtInfoType *info,
        ETHERSWT_ARLEntryType *entry, uint32_t *count);

/** @brief Set Ingress Filter mode

    This API is used to set ingress filter mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      iFilter         Ingress Filter mode

    @return     #BCM_ERR_OK             Ingress filter mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid iFilter
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtVlanIfilterSet(MgmtInfoType *info,
        ETHERSWT_VLANIngressFilterModeType iFilter);

/** @brief Get Ingress filter mode

    This API is used to get ingress filter mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]      iFilter         Pointer to obtain ingress filter mode

    @return     #BCM_ERR_OK             Ingress filter mode obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid iFilter pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_EtherSwtVlanIfilterGet(MgmtInfoType *info,
        ETHERSWT_VLANIngressFilterModeType *iFilter);

/** @brief Convert MAC address

    This API is used to convert MAC address from a string of the format
    xx:xx:xx:xx:xx:xx to a character array of hexadecimal numbers

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to input string
    @param[out]     mac_hex         Character buffer pointer to store output
                                    hexadecimal numbers
    @param[out]     status          Pointer to retrieve the status of operation
                                    BCM_ERR_OK : MAC Address conversion
                                    successful
                                    BCM_ERR_INVAL_PARAMS : Invalid input MAC
                                    address string

    @return void

    @post None

    @limitations None
*/
extern void HOST_EtherSwtConvertMac(char *mac_addr, unsigned char *mac_hex, int32_t *status);

/** @brief Ethernet Switch Sub-group Notification Handler

    This API is meant to handle asynchronous notifications sent by the Ethernet
    Switch Sub-group on target

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
extern int32_t HOST_EtherSwtNotificationHandler(uint32_t currentSlave,
        EtherSwt_EventType notificationId, uint8_t *msg, uint32_t size);

/** @brief Stream policer addition

    This API is used to add a stream policer

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to an character buffer MAC address
    @param[in]      vlan            VLAN ID
    @param[in]      rate            Policer rate (Kbps)
    @param[in]      burst           Policer burst (bytes)
    @param[in]      srcMask         Ingress port bitmask
    @param[in]      threshold       Drop threshold (bytes)
    @param[in]      interval        Monitoring interval (ticks)
    @param[in]      report          Flag indicating whether host needs to be notified when
                                    drop threshold is exceeded
    @param[in]      block           Flag indicating whether stream should be blocked when
                                    drop threshold is exceeded
    @param[out]     streamIdx       Stream index

    @return     #BCM_ERR_OK             Stream policer added successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtStreamPolicerAdd(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan,
                                      uint32_t rate, uint32_t burst, uint32_t srcMask,
                                      uint32_t threshold, uint32_t interval, uint32_t report,
                                      uint32_t block, uint32_t *const streamIdx);

/** @brief Stream policer deletion

    This API is used to delete a stream policer

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      streamIdx       Stream index

    @return     #BCM_ERR_OK             Stream policer deleted successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtStreamPolicerDel(MgmtInfoType *info, uint32_t streamIdx);

/** @brief Block stream

    This API is used to block a stream

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      streamIdx       Stream index

    @return     #BCM_ERR_OK             Stream blocked successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtBlockStream(MgmtInfoType *info, uint32_t streamIdx);

/** @brief Resume stream

    This API is used to resume a blocked stream

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      streamIdx       Stream index

    @return     #BCM_ERR_OK             Stream resumed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtResumeStream(MgmtInfoType *info, uint32_t streamIdx);

/** @brief Find stream policer index

    This API is used to find a stream policer index

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to an character buffer MAC address
    @param[in]      vlan            VLAN ID
    @param[in]      srcMask         Ingress port bitmask
    @param[out]     streamIdx       Stream index

    @return     #BCM_ERR_OK             Stream policer index found successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtStreamPolicerFindIdx(MgmtInfoType *info, uint8_t *mac_addr, uint16_t vlan,
                                          uint32_t srcMask, uint32_t *const streamIdx);

/** @brief Get stream policer status

    This API is used to retrieve stream policer status for a particular index

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[inout]   aStatus         Pointer to stream policer status

    @return     #BCM_ERR_OK             Stream policer status retrieved successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtStreamPolicerGetStatus(MgmtInfoType *info, COMMS_StreamPolicerStatusType *const aStatus);

/** @brief Get stream policer snapshot

    This API is used to retrieve stream policer snapshot

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[inout]   aSnapshot       Pointer to stream policer snapshot

    @return     #BCM_ERR_OK             Stream policer snapshot retrieved successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_EtherSwtStreamPolicerSnapshot(MgmtInfoType *info, COMMS_StreamPolicerSnapshotType *const aSnapshot);
#endif /* HOST_ETHERSWT_H */

/** @} */
