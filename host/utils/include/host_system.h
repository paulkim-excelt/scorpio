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
    @defgroup grp_host_system Host System Commands
    @ingroup grp_host

    @addtogroup grp_host_system
    @{

    @file host_system.h
    @brief Host System Group API
    This header file contains the host interface functions for System Group

    @version 1.0 Initial Version
*/
#ifndef HOST_SYSTEM_H
#define HOST_SYSTEM_H

#include <stdint.h>
#include <sys_ipc_cmds.h>

/**
    @name Connection
    @{
    @brief Connection type
*/
typedef uint32_t mgmt_connection_t;     /**< @brief Connection Type */
#define MGMT_NO_CONNECTION      (0x00000001UL)  /**< @brief No Connection */
#define MGMT_SPI_CONNECTION     (0x00000002UL)  /**< @brief SPI Connection */
#define MGMT_PCIE_CONNECTION    (0x00000003UL)  /**< @brief PCIE connection */
/** @} */

/**
    @brief Management information type
*/
typedef struct mgmt_info_s {
    mgmt_connection_t  connection_type; /**< @brief Connection type */
} MgmtInfoType;


#define INSTALL_MODE_ALL    (1)
#define INSTALL_MODE_CUSTOM (2)
#define INSTALL_MODE_BL     (3)

/** @brief Reboot target

    This API is used to reboot target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Reboot completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations On a stacking board, reboot is issued to
    only one device instead of rebooting all of them
*/
extern int32_t HOST_SysReboot(MgmtInfoType *info);

/** @brief Get OS version

    This API is used to retrieve Operating System version

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     ver             Pointer to a character array

    @return     #BCM_ERR_OK             Obtained OS version successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid ver pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysOSVersionGet(MgmtInfoType *info, char *ver);

/** @brief Get MCU Information

    This API is used to retrieve MCU information

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     manuf           Manufacterer ID
    @param[out]     madel           Model ID
    @param[out]     rev             Revision ID

    @return     #BCM_ERR_OK             Obtained MCU Information successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid manuf pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid model pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid rev pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysMCUInfoGet(MgmtInfoType *info, uint32_t *manuf, uint32_t *model, uint32_t *rev);

/** @brief Erase Flash

    This API is used to erase the contents of flash

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      image           Pointer to image
    @param[in]      len             Length of the image

    @return     #BCM_ERR_OK     Flash erase successfully
    @return     others          Failure

    @post None

    @limitations None
*/
int32_t HOST_SysFlashErase(MgmtInfoType *info, const uint8_t *image, uint32_t len);

/** @brief Install image in flash

    This API is used to install one or more images in flash

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mode            Mode of installation
                                    INSTALL_MODE_ALL    All images will be installed
                                    INSTALL_MODE_CUSTOM Custom image will be installed
                                    INSTALL_MODE_BL     Bootloader will be installed
    @param[in]      image           Pointer to image
    @param[in]      len             Length of the image

    @return     #BCM_ERR_OK     Image(s) installed successfully
    @return     others          Failure

    @post None

    @limitations None
*/
int32_t HOST_SysFirmwareUpdate(MgmtInfoType *info, uint32_t mode, const uint8_t *image, uint32_t len);

/** @brief Execute Bootloader and Firmware (without flashing)

    This API is used to execute bootloader and firmware without Flashing (Flash less mode of operation)

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      image           Pointer to image
    @param[in]      len             Length of the image

    @return     #BCM_ERR_OK     Image(s) installed successfully
    @return     others          Failure

    @post None

    @limitations None
*/
extern int32_t HOST_SysFirmwareExecute(MgmtInfoType *info, const uint8_t *image, uint32_t len);

/** @brief Execute Bootloader and apply xcvr configuration (without flashing)

    This API is used to execute bootloader, apply the transceiver configuration
    (This does not execute the firmware)

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      image           Pointer to image
    @param[in]      len             Length of the image

    @return     #BCM_ERR_OK     Image(s) installed successfully
    @return     others          Failure

    @post None

    @limitations None
*/

extern int32_t HOST_SysBLExecute(MgmtInfoType *info, const uint8_t *image, uint32_t len);

/** @brief Flash write

    This API is used to write to flash. This is needed for verification of
    Failsafe feature.

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      flash_addr      Flash Address

    @return     #BCM_ERR_OK     Flash write successful
    @return     others          Failure

    @post None

    @limitations None
*/
int32_t HOST_SysFlashWrite(MgmtInfoType *info, uint32_t flash_addr);

/** @brief Get Keep-Alive information

    This API is used to query the number of Keep-Alive messages sent by target
    (all the targets on a stacked board) and the latest Up-Time

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     keepAlive       Pointer to an array of #SYS_KeepAliveType

    @return     #BCM_ERR_OK             KeepAlive information copied successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid keepAlive pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysKeepAliveGet(MgmtInfoType *info, uint32_t slaveID,
                    SYS_KeepAliveType *keepAlive);

/** @brief OTP Read

    This API is used to read from OTP

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      row_num         Row number in OTP
    @param[out]     value           Pointer to obtain read value

    @return     #BCM_ERR_OK             OTP read completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations Read data is not returned to caller and instead printed on console
*/
extern int32_t HOST_SysOTPRead(MgmtInfoType *info, uint32_t row_num, uint32_t *value);

/** @brief OTP Write

    This API is used to write to OTP

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      row_num         Row number in OTP
    @param[in]      data            Data to be written

    @return     #BCM_ERR_OK             OTP write completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysOTPWrite(MgmtInfoType *info, uint32_t row_num, uint32_t data);

/** @brief Enable secure boot flow

    This API is used to enable secure boot flow

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Secure boot enabled successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysOTPEnableSec(MgmtInfoType *info);

/** @brief OTP key write

    This API is used to write KEY information into OTP

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      image           Pointer to buffer containing the KEY
    @param[in]      len             Size of image buffer

    @return     #BCM_ERR_OK             OTP key write was successful
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   len > (OTP_SECURE_KEY_SIZE_LOC * 4)
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysOTPKeyWrite(MgmtInfoType *info, uint8_t *image, uint32_t len);

/** @brief OTP MAC address write

    This API is used to write device MAC address into OTP memory

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mac_addr        Pointer to buffer containing MAC address
    @param[in]      loc             MAC address location

    @return     #BCM_ERR_OK             OTP MAC address write was successful
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   mac_addr pointer is NULL
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HOST_SysOTPMacAddrWrite(MgmtInfoType *info, uint8_t *const mac_addr, uint32_t loc);

/** @brief OTP MAC address read

    This API is used to read MAC address from OTP memory

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[out]     mac_addr1       Pointer to the buffer to read 1st copy of
                                    MAC address
    @param[out]     mac_addr2       Pointer to the buffer to read 2nd copy of
                                    MAC address
    @param[out]     valid           Pointer to the retrieve valid MAC address
                                    mask

    @return     #BCM_ERR_OK             OTP MAC address read completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid mac_addr1 pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid mac_addr2 pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid valid pointer
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
int32_t HOST_SysOTPMacAddrRead(MgmtInfoType *info, uint8_t *mac_addr1,
        uint8_t *mac_addr2i, uint32_t *valid);

/** @brief System Group Notification Handler

    This API is meant to handle asynchronous notifications sent by the System group on target

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
extern int32_t HOST_SysNotificationHandler(uint32_t currentSlave,
    BCM_CompIDType comp, SYS_AsyncIdType notificationId, uint8_t *msg,
    uint32_t size);

/** @brief Initialize a connection

    This API is used to initialise a connection

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer

    @post None

    @limitations None
*/
extern int32_t HOST_SysInit(MgmtInfoType *info);

/** @brief Deinitialize a connection

    This API is used to deinitialise a connection

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer

    @post None

    @limitations None
*/
extern int32_t HOST_SysDeinit(MgmtInfoType *info);

#ifdef ENABLE_DBGMEM
/** @brief Memory Write

    This API is used to write to target's memory

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      destn           Access from master or slave device
    @param[in]      addr            Base address of memory to be updated
    @param[in]      width           Access width
    @param[in]      len             Number of accesses of the specified width
    @param[in]      data            Pointer to source memory

    @return     #BCM_ERR_OK             Write completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid data pointer
    @return     #BCM_ERR_INVAL_PARAMS   len is 0
    @return     #BCM_ERR_INVAL_PARAMS   width is not one of 8, 16 or 32
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations 64bit access is not allowed
*/
extern int32_t HOST_MemoryWrite(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                                uint32_t width, uint32_t len, volatile uint8_t *data);

/** @brief Memory Read

    This API is used to read from target's memory

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      destn           Access from master or slave device
    @param[in]      addr            Base address of memory to be read
    @param[in]      width           Access width
    @param[in]      len             Number of accesses of the specified width
    @param[out]     data            Pointer to destination memory

    @return     #BCM_ERR_OK             Read completed successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid data pointer
    @return     #BCM_ERR_INVAL_PARAMS   len is 0
    @return     #BCM_ERR_INVAL_PARAMS   width is not one of 8, 16 or 32
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations 64bit access is not allowed
*/
extern int32_t HOST_MemoryRead(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                               uint32_t width, uint32_t len, volatile uint8_t  *data);
#endif /* ENABLE_DBGMEM */

#endif /* HOST_COMON_H */

/** @} */
