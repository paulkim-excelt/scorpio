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
    @defgroup grp_host_ets Host ETS
    @ingroup grp_host

    @addtogroup grp_host_ets
    @{

    @file host_ets.h
    @brief Host ETS Group API
    This header file contains the host interface functions for ETS Group

    @version 1.0 Initial Version
*/
#ifndef HOST_ETS_H
#define HOST_ETS_H

#include <host_system.h>
#include <ets_osil.h>

/**
    @brief Number of event records per interface

*/
#define ETS_NUM_RECORDS_PER_INTF      (8UL)

/** @brief Get ETS Configuration

    This API is used to obtain the complete ETS configuration

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     config          Pointer to #ETS_ConfigType to be filled

    @return     #BCM_ERR_OK             ETS Configuration obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid config pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSConfigGet(MgmtInfoType *info, ETS_ConfigType *config);

/** @brief Update ETS Configuration

    This API is used to update the complete ETS configuration

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      config          Pointer to #ETS_ConfigType

    @return     #BCM_ERR_OK             ETS Configuration updated successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid config pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSConfigSet(MgmtInfoType *info, ETS_ConfigType *config);

/** @brief Get ETS global status

    This API is used to obtain the global status of ETS

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     globalStatus    Pointer to #ETS_GlobalStatusType to be filled

    @return     #BCM_ERR_OK             ETS global status obtained successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid globalStatus pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSGlobalStatus(MgmtInfoType *info, ETS_GlobalStatusType *globalStatus);

/** @brief Get ETS port status

    This API is used to obtain the port status from ETS

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[out]     portStatus      Pointer to #ETS_PortStatsAndStatusType to be filled

    @return     #BCM_ERR_OK             ETS port status successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid portStatus pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSPortStatus(MgmtInfoType *info, uint32_t port, ETS_PortStatsAndStatusType *portStatus);

/** @brief Clear ETS port statistics

    This API is used to clear ETS statistics specific to a particular port

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number

    @return     #BCM_ERR_OK             ETS port statistics cleared successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSPortStatsClear(MgmtInfoType *info, uint32_t port);

/** @brief Set Network time

    This API is used to set the network time

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      sec_high        Most significant word of seconds value
    @param[in]      sec_high        Least significant word of seconds value
    @param[in]      nanosec         Nanoseconds value

    @return     #BCM_ERR_OK             Network time set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSTimeSet(MgmtInfoType *info, uint32_t sec_high, uint32_t sec_low, uint32_t nanosec);

/** @brief Set ETS global admin mode

    This API is used to set the global admin mode of ETS

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      adminMode       Mode of type #ETS_AdminModeType

    @return     #BCM_ERR_OK             Admin mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid adminMode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSGlobalAdminModeSet(MgmtInfoType *info, ETS_AdminModeType adminMode);

/** @brief Set number of lost responses

    This API is used to set the number of PDelay Request messages for which a
    valid response is not received, above which a port is considered to not be
    exchanging peer delay messages with its neighbor

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      numLostResp     Number of lost responses

    @return     #BCM_ERR_OK             Number of lost responses set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSNumLostRespSet(MgmtInfoType *info, uint32_t port, uint32_t numLostResp);

/** @brief Set Initial Sync Interval

    This API is used to set the Initial Sync interval

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      interval        Interval value

    @return     #BCM_ERR_OK             Initial Sync interval set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSInitSyncIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval);

/** @brief Set Intial Pdelay Interval

    This API is used to set Intial Pdelay interval

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      interval        Interval value

    @return     #BCM_ERR_OK             Intial Pdelay interval set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSInitPdelayIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval);

/** @brief Set Operational Pdelay interval

    This API is used to set Operational Pdelay interval

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      interval        Interval value

    @return     #BCM_ERR_OK             Operational Pdelay interval set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSOperPdelayIntervalSet(MgmtInfoType *info, uint32_t port, int32_t interval);

/** @brief Set neighbor Pdelay

    This API is used to set neighbor Pdelay value

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      pdelay          Pdelay value

    @return     #BCM_ERR_OK             Neighbor Pdelay set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSNbrPdelaySet(MgmtInfoType *info, uint32_t port, uint32_t pdelay);

/** @brief Set asCapable mode

    This API is used to set asCapable mode of a particular port

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      asCapable       Mode as #ETS_AdminModeType

    @return     #BCM_ERR_OK             AsCapable mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid asCapable value
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSAsCapableModeSet(MgmtInfoType *info, uint32_t port, ETS_AdminModeType asCapable);

/** @brief Set port role

    This API is used to set port role

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      port            Port number
    @param[in]      role            Port role as #ETS_RoleType

    @return     #BCM_ERR_OK             Port role set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid role value
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSPortRoleSet(MgmtInfoType *info, uint32_t port, ETS_RoleType role);

/** @brief Set ETS clock mode

    This API is used to set the clock mode of ETS

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mode            Mode of type #ETS_ClockModeType

    @return     #BCM_ERR_OK             Clock mode set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid mode
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSClockModeSet(MgmtInfoType *info, ETS_ClockModeType mode);

/** @brief Set Sync Absence timeout value

    This API is used to set the sync absence timeout value

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      timeout         Value of timeout

    @return     #BCM_ERR_OK             Sync Absence timeout value set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_ETSSyncAbsenceTimeoutSet(MgmtInfoType *info, uint32_t timeout);

/** @brief Get index corresponding to a port

    This API is meant to obtain the index of #ETS_IntfConfigType interface
    configuration corresponding to a particular port number

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      port            Port number
    @param[in]      intfCfg         Pointer to array of #ETS_IntfConfigType

    @return     #ETS_MAX_INTERFACES     Index not found
    @return     others                  Valid index

    @post None

    @limitations None
*/
extern uint32_t HOST_ETSPortToIndex(uint32_t port, ETS_IntfConfigType *intfCfg);

/** @brief Start/Stop transferring ETS  records

    This API is meant to Start/Stop transferring ETS records

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      isStart         boolean start(1) or stop(0)

    @return     #BCM_ERR_OK             ETS send record started/stoped successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_DATA_INTEG     Invalid response length
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations This API is avaialable only when ENABLE_RECORD_NOTIFICATION is enabled
*/
extern int32_t HOST_ETSStartStopSendingRecord(MgmtInfoType *info, uint8_t isStart);

/** @brief Get records corresponding to a port

    This API is meant to obtain the  Records for a port

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      portIdx         Pointer to #port index
    @param[out]     records         Pointer to #ETS_RecordType
    @param[in]      size            Pointer to #size of array records

    @return     #BCM_ERR_OK             Records retrieved successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid records pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid port number

    @post None

    @limitations This API is avaialable only when ENABLE_RECORD_NOTIFICATION is enabled
*/
extern int32_t HOST_ETSGetRecord(uint32_t portIdx, ETS_RecordType *records, uint32_t size);

/** @brief ETS Group Notification Handler

    This API is meant to handle asynchronous notifications sent by the ETS group on target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      currentSlave    Slave ID
    @param[in]      comp            Component ID
    @param[in]      notificationId  Notification ID
    @param[in]      msg             Message buffer pointer
    @param[in]      size            Size of the message buffer "msg" in bytes

    @return     #BCM_ERR_OK             Notification handled successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid comp
    @return     #BCM_ERR_INVAL_PARAMS   Invalid notificationId
    @return     #BCM_ERR_INVAL_PARAMS   Invalid msg pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid size

    @post None

    @limitations None
*/
extern int32_t HOST_ETSNotificationHandler(uint32_t currentSlave,
                ETS_EventType notificationId, uint8_t *msg, uint32_t size);
#endif /* HOST_ETS_H */

/** @} */
