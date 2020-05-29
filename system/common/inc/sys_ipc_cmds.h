/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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

#ifndef SYS_IPC_CMDS_H
#define SYS_IPC_CMDS_H

#include <stdint.h>
#include <stdlib.h>
#include <mcu.h>
#include <bcm_time.h>
#include <imgl_ipc_cmds.h>
#ifdef ENABLE_DBGMEM
#include <dbgmem.h>
#endif

#define VERSION_STR_LEN                 100

/* System Group Commands*/
typedef uint8_t SYS_CmdIdType;
#define SYS_CMDID_PING                     (0x01U)
#define SYS_CMDID_MEM_WRITE                (0x10U)
#define SYS_CMDID_MEM_READ                 (0x11U)
#define SYS_CMDID_SW_VERSION               (0x20U)
#define SYS_CMDID_CHIP_ID                  (0x21U)
#define SYS_CMDID_REBOOT                   (0x22U)
#define SYS_CMDID_FLASH_UPDATE             (0x23U)
#define SYS_CMDID_EXECUTE                  (0x26U)
#define SYS_CMDID_OTP_RD                   (0x30U)
#define SYS_CMDID_OTP_WR_SEC_KEY           (0x31U)
#define SYS_CMDID_OTP_ENABLE_SEC           (0x32U)
#define SYS_CMDID_OTP_WRITE_CUST           (0x33U)
#define SYS_CMDID_OTP_WR_MAC_ADDR          (0x34U)
#define SYS_CMDID_OTP_RD_MAC_ADDR          (0x35U)
#define SYS_CMDID_FLASH_ERASE              (0x40U)
#define SYS_CMDID_FLASH_WRITE              (0x41U)
#define SYS_CMDID_APPLY_XCVR_CFG           (0x42U)

typedef uint8_t SYS_AsyncIdType;
#define SYS_ASYNCID_KEEPALIVE              (0x01U)

#define SYS_OTP_SECURE_KEY_SIZE_LOC (256)

typedef uint32_t SYS_ResetModeType;
#define SYS_RESETMODE_CHIP     (0x00000001UL)
#define SYS_RESETMODE_CPU      (0x00000002UL)

typedef uint32_t SYS_BootModeType;
#define SYS_BOOTMODE_BROM      (0x42524F4DUL) /* 'B' 'R' 'O' 'M' */
#define SYS_BOOTMODE_BL        (0x424C4452UL) /* 'B' 'L' 'D' 'R' */
#define SYS_BOOTMODE_FW        (0x464D5752UL) /* 'F' 'M' 'W' 'R' */
#define SYS_BOOTMODE_DEFAULT   (0x44464C54UL) /* 'D' 'F' 'L' 'T' */

/* Command Types */
typedef struct _SYS_OtpInfoType {
    uint32_t size;
    uint32_t addr;
    uint8_t  data[SYS_OTP_SECURE_KEY_SIZE_LOC * 4UL];
} SYS_OtpInfoType;

typedef struct _SYS_RebootCmdType {
    SYS_ResetModeType resetMode;    /* Chip/CPU reset */
    SYS_BootModeType bootMode;    /* Reboot into default mode or BL */
    uint32_t delayMs;                /* delay in milliseconds before rebooting */
} SYS_RebootCmdType;

typedef struct _SYS_MemAccessType {
    uint32_t       addr;
    uint32_t       width;
    uint32_t       len;
    MCU_DeviceType deviceID;
    uint8_t        data[128UL];
} SYS_MemAccessType;

/* Response Types */
typedef struct _SYS_PingType {
    SYS_BootModeType mode;      /**< mode: Firmware/Bootloader */
    MCU_VersionType version;   /**< MCU version */
} SYS_PingType ;

typedef struct _SYS_OSSWVersionType {
    char      str[VERSION_STR_LEN];
} SYS_OSSWVersionType;

/**
    @brief Image header identifying an image

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
 */
#define SYS_UPDATE_EXEC_CMD_NUM_HDR    (48UL)

#pragma pack(push, 4)

/**
    @brief Image loader command structure

    This structure should be used as a command for
        -# SYS_CMDID_FLASH_UPDATE
        -# SYS_CMDID_FLASH_ERASE
        -# SYS_CMDID_FLASH_WRITE
        -# SYS_CMDID_EXECUTE

    @limitation The size of this struct should be aligned to 4bytes.
                This is required to transfer this struct completely to
                BCM8953x.

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
 */
typedef struct _SYS_UpdateExecCmdType {
    uint32_t flashId;
    uint32_t flashAddr;
    uint32_t numImgs;
    IMGL_ImgHdrType imgHdr[SYS_UPDATE_EXEC_CMD_NUM_HDR];
} SYS_UpdateExecCmdType;

#pragma pack(pop)

typedef struct _SYS_KeepAliveType {
    BCM_TimeType upTime;
    uint32_t count;
} SYS_KeepAliveType;

typedef union _SYS_HandleType {
    uint8_t *u8Ptr;
    SYS_OtpInfoType *otpInfo;
    SYS_MemAccessType *memAccess;
    MCU_VersionType *mcuVersion;
    SYS_OSSWVersionType *osVersion;
    SYS_PingType *ping;
    SYS_RebootCmdType *reboot;
    SYS_KeepAliveType *keepAlive;
    SYS_UpdateExecCmdType *update;
    SYS_UpdateExecCmdType *exec;
} SYS_HandleType;

#endif /* SYS_IPC_CMDS_H */
