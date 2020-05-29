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

    @defgroup grp_host_ipc_plat Platform specific Host IPC
    @ingroup grp_host_ipc

    @addtogroup grp_host_ipc_plat
    @{

    @file hipc_plat.h
    @brief Platform specific Host IPC API
    This header file contains the host interface functions for IPC specific to
    target platform

    @version 1.0 Initial Version
*/


#ifndef HIPC_PLAT_H
#define HIPC_PLAT_H

/**
    @name Target Features
    @{
    @brief Target feature bit masks
*/
typedef uint32_t HIPC_TargetFeatureType;    /**< @brief Target Feature type */
#define HIPC_TARGETFEATURE_SELFREBOOT         (0x00000001UL) /**< @brief Support Self-Reboot */
#define HIPC_TARGETFEATURE_RESTRICTEDACCESS   (0x00000002UL) /**< @brief Restricts register accesses to limited set */
/** @} */

/** @brief Notify Target

    This API is used to notify the target of a new IPC/event to be processed

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId         Slave ID

    @return     none

    @post None

    @limitations None
*/
extern void HIPC_PlatNotifyTarget(uint32_t slaveId);

/** @brief Reboot

    This API is used to reboot the target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId             Slave ID
    @param[in]      currTargetBootMode  Current boot mode of target
    @param[in]      currTargetFeatures  Current features of target

    @return     #BCM_ERR_OK         Success

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatReboot(uint32_t slaveId, SYS_BootModeType currTargetBootMode,
        HIPC_TargetFeatureType currTargetFeatures);

/** @brief Reset CPU and wait

    This API is used to reset the CPU and wait for commands. This will let the
    target boot-up with any existing bootloader/updater image from flash or
    relevant bootable device

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId         Slave ID

    @return     #BCM_ERR_OK         Success
    @return     #BCM_ERR_UNKNOWN    Memory access error

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatResetCPUWait(uint32_t slaveId);

/** @brief Execute on target

    This API is used to execute a specific binary on target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId         Slave ID
    @param[in]      image           Pointer to binary
    @param[in]      size            Size of the image

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Size is 0
    @return     #BCM_ERR_INVAL_PARAMS   image is NULL
    @return     #BCM_ERR_UNKNOWN        Memory access error

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatExecuteOnTarget(uint32_t slaveId, const uint8_t* image, uint32_t size);

/** @brief Get target information

    This API is used to get target information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId         Slave ID
    @param[out]     device_id       Device ID
    @param[out]     revision        Revision

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   device_id is NULL
    @return     #BCM_ERR_INVAL_PARAMS   revision is NULL
    @return     #BCM_ERR_UNKNOWN        Memory access error

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatGetTargetInfo(uint32_t slaveId, uint32_t *const device_id,
                            uint32_t *const revision);

/** @brief Get Buffer information

    This API is used to obtain IPC buffer information from target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      slaveId         Slave ID
    @param[in]      buff            Pointer to buffer
    @param[in]      cnt             Number of buffers
    @param[in]      cntRollOverMask Mask of virtual index
    @param[in]      sz              Size of buffers

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_UNINIT         IPC is uninitialised

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatGetBuff(uint32_t slaveId, uint8_t **buff, uint16_t *cnt,
                         uint8_t *cntRollOverMask, uint16_t *sz);

/** @brief Compute Even Parity

    This API is used to get Even Parity

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      val             Value to compute parity

    @return         parity          Parity value

    @post None

    @limitations None
*/
extern uint16_t HIPC_EvenParity(uint16_t val);

/** @brief Obtain target features

    This API is used to obtain target features based on the version

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      version         Version of type #MCU_VersionType
    @param[out]     features        Supported features of type #HIPC_TargetFeatureType

    @return         #BCM_ERR_OK             Features obtained successfully
    @return         #BCM_ERR_INVAL_PARAMS   features is NULL
    @return         #BCM_ERR_INVAL_PARAMS   version is NULL

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatGetTargetFeature(MCU_VersionType *version, HIPC_TargetFeatureType *features);

/** @brief Send a command and receive its response (BootROM mode)

    This API is used to send a command to target in BootROM mode and receive a
    response to the same command

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      id              Command ID of message
    @param[in]      cmd             Pointer to command message payload
    @param[in]      cmd_len         Size of command message payload
    @param[out]     reply           Pointer to copy response payload
    @param[in]      reply_len_max   Maximum size of reponse payload
    @param[out]     reply_len_act   Actual size of reponse payload copied

    @return     #BCM_ERR_OK             IPC message received successfully
    @return     #BCM_ERR_DATA_INTEG     IPC message's data integrity failed
    @return     #BCM_ERR_NOMEM          IPC message's length is incorrect
    @return     #BCM_ERR_UNINIT         IPC mechanism is not yet initialised
    @return     #BCM_ERR_UNKNOWN        Error reported by target
    @return     #BCM_ERR_UNKNOWN        Memory access failure

    @post None

    @limitations None
*/
extern int32_t HIPC_PlatSendRecvRom(uint32_t id, uint8_t *cmd, uint32_t cmd_len,
            uint8_t *reply, uint32_t reply_len_max, uint32_t *reply_len_act);

#endif /* HIPC_PLAT_H */

/** @} */

