/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ipc_hwif.h"
#include "hipc.h"
#include "bus_xfer.h"
#include "spi.h"
#include "spi_xfer.h"
#include <bcm_err.h>
#include "bl_downloader.h"
#include "bl_ipc_downloader.h"
#include "bl_chip_config.h"
#include "mcu.h"
#include "utils.h"
#include <rpc_cmds.h>
#include <hipc_plat.h>
#include <sys_ipc_cmds.h>
#include <gpio.h>
#include <crc.h>
#include <hlog.h>
#include <imgl_ipc_cmds.h>
#include <rpc_cmds.h>
#ifdef HIPC_ENABLE_PCIE
#include "pcie_xfer.h"
#endif

#define SPI_SPEED                   (12500000) /* Minimum 80ns as per DS */
#define USEC_PER_MSEC               (1000)

#define HIPC_TARGET_REBOOT_DELAY_MS      (10UL)    /* 10ms */
#define HIPC_BOOTLOADER_BOOTUP_TIME_MS   (1000UL)  /* 1sec */
#define CRC32_POLY                       (0x04C11DB7UL)

/* SPI_ID */
typedef uint32_t SPI_ID_t;
#define SPI_ID_0            (0UL)
#define SPI_ID_1            (1UL)
#define SPI_ID_2            (2UL)
#define SPI_ID_3            (3UL)
#define SPI_ID_MIN          (SPI_ID_0)
#define SPI_ID_MAX          (SPI_ID_3) /* Limited by the two bits[7:6] in OpCode */
#define SPI_ID_INVAL        (0xFFFFFFFFUL)
#define SPI_ID_DEFAULT      (SPI_ID_2)

#define SPI_SLAVES_MAX      (3UL) /* Limited by light-stacking */

#define BOOT_INFO_SPARE_REG             (MISC_SPARE_SW_REG3)

#define BL_IMG_SIGNATURE_SIZE   (256UL)
#define MSG_BUF_SIZE    (512UL - 128UL)

#define HIPC_ENABLE_PCIE 1/***paul ****/

static uint8_t send_msg_buf[MSG_BUF_SIZE];
static uint8_t *snd_msg = send_msg_buf;
static uint8_t recv_msg_buf[MSG_BUF_SIZE];
static uint8_t *recv_msg = recv_msg_buf;

/* SPI Driver will use this for all transactions */
static uint32_t CurrentSpiId = SPI_ID_DEFAULT;

#ifdef HIPC_ENABLE_PCIE 
static uint32_t PCIESlavePresent = 0UL;
#endif

/* All IDs which have been probed during start-up gets registered here */
static uint32_t ActiveSpiId[SPI_SLAVES_MAX];
static mgmt_connection_t CurrentMode = MGMT_SPI_CONNECTION;

static int32_t HIPC_PingInternal(MgmtInfoType *info, uint32_t *const mode, uint32_t * const targetFeatures, uint8_t aGroupId)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];
    MCU_VersionType version;

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (NULL == mode) || (NULL == targetFeatures)) {
        HOST_Log("%s :: Invalid input parameter(info:%p) mode:%p targetFeatures:%p\n",
            __FUNCTION__, info, mode, targetFeatures);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(aGroupId, BCM_RSV_ID, SYS_CMDID_PING);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            0UL, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(SYS_PingType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *mode = uswap32(respHdl.ping->mode);
        version.manuf = uswap32(respHdl.ping->version.manuf);
        version.model = uswap32(respHdl.ping->version.model);
        version.rev = uswap32(respHdl.ping->version.rev);
        version.reserved = uswap32(respHdl.ping->version.reserved);

        retVal = HIPC_PlatGetTargetFeature(&version, targetFeatures);
        if (retVal != BCM_ERR_OK) {
            retVal = BCM_ERR_UNKNOWN;
        }
    }

done:
    return retVal;
}

int32_t HIPC_Ping(MgmtInfoType *info, uint32_t *const mode, uint32_t * const targetFeatures)
{
    return HIPC_PingInternal(info, mode, targetFeatures, BCM_GROUPID_SYS);
}

static int32_t HIPC_PingRom(MgmtInfoType *info)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t replyId = 0;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if (info == NULL) {
        HOST_Log("%s :: Invalid input parameter(info:%p)\n",
            __FUNCTION__, info);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_PING);

    /*
        - Send a PING message to target
        - In response, if the target responds with a replyId =
          BCM_ERR_INVAL_MAGIC, then it is a BCM89559 board in ROM mode
        - However if the replyId matches asynchronous messages, then process
          them as usual
    */

    retVal = HIPC_Send(cmdId, cmdHdl.u8Ptr, 4UL);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("%s Failed to send command, err:%d\n", __func__, retVal);
        goto done;
    }

    /* Check for response */
    do {
        retVal = HIPC_Recv(&replyId, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);
        if (retVal == BCM_ERR_OK) {
            if (RPC_CMD_ASYNC_MASK == (RPC_CMD_ASYNC_MASK & replyId)) {
                HOST_Log("%s Async msg received\n", __func__);
                retVal = IPC_QueueAsyncMsg(CurrentSpiId, replyId, (uint8_t *)&resp, replyLen);
                if (retVal != BCM_ERR_OK) {
                    HOST_Log("%s failed to queue async msg, err:%d\n", __func__, retVal);
                }
                /* we have still not received a response to the msg sent */
                retVal = BCM_ERR_NOT_FOUND;
            } else if (replyId != BCM_ERR_INVAL_MAGIC) {
                retVal = BCM_ERR_NOSUPPORT;
            }
        }
    } while (BCM_ERR_NOT_FOUND == retVal);

done:
    return retVal;
}

int32_t HIPC_GetTargetSWMode(MgmtInfoType *info, SYS_BootModeType *const targetBootMode, HIPC_TargetFeatureType *const targetFeatures)
{
    int32_t retVal;

    /* Ping target device to identify the software mode of target device.
     * HIPC_PingRom - ROM specific Ping
     *   If target device is in ROM mode, it would return
          - BCM_ERR_OK => ROM mode with reboot capability
          - BCM_ERR_UNINIT => ROM mode without reboot capability
          - BCM_ERR_NOSUPPORT => use HIPC_PingInternal to identify
     */
    retVal = HIPC_PingRom(info);
    if (BCM_ERR_OK == retVal) {
        *targetBootMode = SYS_BOOTMODE_BROM;
        *targetFeatures = HIPC_TARGETFEATURE_SELFREBOOT;
    } else if (BCM_ERR_UNINIT == retVal) {
        *targetBootMode = SYS_BOOTMODE_BROM;
        *targetFeatures = 0UL;
        retVal = BCM_ERR_OK;
    } else {
        retVal = HIPC_PingInternal(info, targetBootMode, targetFeatures, BCM_GROUPID_IMGL);
        if (BCM_ERR_OK == retVal) {
            /* Newer versions of BootROM */
        } else {
            /* FW/Bootloader.. query using SYS Ping */
            retVal = HIPC_PingInternal(info, targetBootMode, targetFeatures, BCM_GROUPID_SYS);
        }
    }

    return retVal;
}

static int32_t HIPC_RebootIssueCommand(MgmtInfoType *info, SYS_ResetModeType resetMode, SYS_BootModeType bootMode, uint32_t delayMs)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t replyLen;
    uint32_t cmdId;

    SYS_RebootCmdType reboot;
    SYS_HandleType cmdHdl;
    cmdHdl.reboot = &reboot;

    RPC_ResponseType resp;

    /* Issue REBOOT command to TARGET */
    cmdHdl.reboot->delayMs = uswap32(delayMs); /* defer */
    cmdHdl.reboot->resetMode = uswap32(resetMode);
    cmdHdl.reboot->bootMode = uswap32(bootMode);
    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_REBOOT);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr, sizeof(SYS_RebootCmdType),
            (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);
    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != MGMT_STATUS_LEN) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HIPC_Reboot(MgmtInfoType *info)
{
    int32_t retVal;
    HIPC_TargetFeatureType currTargetFeatures;
    SYS_BootModeType currTargetBootMode;

    retVal = HIPC_GetTargetSWMode(info, &currTargetBootMode, &currTargetFeatures);
    if (BCM_ERR_OK != retVal) {
        goto err_GetSWMode;
    }

    /* If target can't reboot on it's own, it shall prepare for reboot instead */
    retVal = HIPC_RebootIssueCommand(info, SYS_RESETMODE_CHIP, SYS_BOOTMODE_DEFAULT, HIPC_TARGET_REBOOT_DELAY_MS);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to Reboot target through IPC command, err:0x%x\n", __func__, retVal);
        goto err_RebootIssueCmd;
    }

    retVal = HIPC_PlatReboot(CurrentSpiId, currTargetBootMode, currTargetFeatures);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to perform platform specific reboot, err:0x%x\n", __func__, retVal);
    }

err_RebootIssueCmd:
err_GetSWMode:
    return retVal;
}

int32_t HIPC_EnterUpdateMode(MgmtInfoType *info, HIPC_InstallType mode)
{
    int32_t retVal;
    HIPC_TargetFeatureType currTargetFeatures;
    SYS_BootModeType currTargetBootMode;
    uint16_t reg16;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    /* Check if the target is already in Bootloader mode */
    retVal = HIPC_GetTargetSWMode(info, &currTargetBootMode, &currTargetFeatures);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to get target SW mode\n", __func__);
        goto done;
    }
    if (SYS_BOOTMODE_BL == currTargetBootMode) {
        HOST_Log("%s Already in Update mode\n", __func__);
        retVal = BCM_ERR_OK;
        goto done;
    }

    if (SYS_BOOTMODE_FW != currTargetBootMode) {
        HOST_Log("%s Can't enter update mode, current mode:0x%x\n", __func__, currTargetBootMode);
        retVal = BCM_ERR_NOSUPPORT;
        goto done;
    }

    /* Instruct bootloader to enter/halt-at Updater mode */
    reg16 = MCU_FW_BOOT_INFO_SW_RESET_MASK
        | (MCU_FW_BOOT_INFO_DWNLD_MODE_IPC << MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT);

    if (HIPC_INSTALL_FACTORY != mode) {
        reg16 |= MCU_FW_BOOT_INFO_DWNLD_MODE_PARTIAL_MASK;
    }
    HIPC_BusXferWrite(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)BL_BOOT_MODE_SPARE_REG, (uint8_t *)&reg16, 16UL);

    /* Try to enter bootloader mode independently. If target can't reboot on
       it's own, it shall prepare for reboot instead */
    retVal = HIPC_RebootIssueCommand(info, SYS_RESETMODE_CPU, SYS_BOOTMODE_BL,
                                    HIPC_TARGET_REBOOT_DELAY_MS);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Failed to Reboot target through IPC command, err:0x%x\n", __func__, retVal);
        goto done;
    }

    /* On boards which don't support self-reboot, try a soft-reset from host itself */
    if (0UL == (currTargetFeatures & HIPC_TARGETFEATURE_SELFREBOOT)) {
        retVal = HIPC_PlatResetCPUWait(CurrentSpiId);
        if (BCM_ERR_OK != retVal) {
            HOST_Log("%s Failed to Soft-reset target, err:0x%x\n", __func__, retVal);
            goto done;
        }
    } else {
        /* Wait for reboot command to be processed */
        usleep(HIPC_TARGET_REBOOT_DELAY_MS * USEC_PER_MSEC);
    }

    /* Wait for bootloader to boot-up */
    usleep(HIPC_BOOTLOADER_BOOTUP_TIME_MS * USEC_PER_MSEC);

    /* Verify that the target has entered Bootloader mode indeed */
    retVal = HIPC_GetTargetSWMode(info, &currTargetBootMode, &currTargetFeatures);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

    if (SYS_BOOTMODE_BL == currTargetBootMode) {
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_NOSUPPORT;
    }

done:
    return retVal;
}

uint32_t HIPC_IsReady(void)
{
    uint16_t regVal16 = 0;
    uint32_t isReady = FALSE;
    uint16_t prio;

    HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_BUFF_INFO_REG, (uint8_t *)&regVal16, 16UL);
    if ((regVal16 == 0xFFFF)
            || (regVal16 == 0UL) || (HIPC_EvenParity(regVal16) != 0U)) {
        isReady = FALSE;
        goto done;
    }

    HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_TARGET_STAT_REG, (uint8_t *)&regVal16, 16UL);

    prio = (regVal16 & IPC_TARGET_STAT_PRI_MASK) >> IPC_TARGET_STAT_PRI_SHIFT;
    if ((prio == IPC_TARGET_STAT_PRI_REBOOT)
        || (prio == IPC_TARGET_STAT_PRI_PAUSE)) {
        isReady = FALSE;
    } else {
        isReady = TRUE;
    }

done:
    return isReady;
}

uint32_t HIPC_IsTargetReady()
{
    uint32_t isReady = FALSE;
    uint16_t reg16 = 0;
    int32_t retVal;

    if (TRUE == HIPC_IsReady()) {
        retVal = HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)BL_DWNLD_TARGET_SPARE_REG, (uint8_t *)&reg16, 16UL);
        if (retVal) {
            goto done;
        }
        if ((reg16 & BL_IPC_DWNLD_BL_READY_MASK) == BL_IPC_DWNLD_BL_READY_MASK) {
            isReady = TRUE;
        }
    }
done:
    return isReady;
}

static void HIPC_ProgressLog(const uint32_t skipTimeIntervalCheck)
{
    static uint32_t firstCall = 1U; /* Ugh!! */
    static struct timeval lastLogTime;
    static uint32_t lastLogPercent = 0UL;

    struct timeval curTime;
    struct timeval elapsedTime;

    if (1U == firstCall) {
        gettimeofday(&lastLogTime, NULL);
        firstCall = 0U;
    }

    gettimeofday(&curTime, NULL);
    timersub(&curTime, &lastLogTime, &elapsedTime);

    if ((0U != skipTimeIntervalCheck) || (elapsedTime.tv_sec > 0) || (elapsedTime.tv_usec > 100000)) { /* 100ms */

        uint16_t target_reg = 0U;
        HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)BL_DWNLD_TARGET_SPARE_REG, (uint8_t *)&target_reg, 16UL);

        if (HIPC_IsTargetReady() == TRUE) {
            uint16_t percent =
                (target_reg & BL_IPC_DWNLD_FLASHING_PERCENT_MASK) >>
                    BL_IPC_DWNLD_FLASHING_PERCENT_SHIFT;

            if ((percent / 10) != (lastLogPercent / 10)) {
                HOST_Log("\n");
            }
            HOST_Log("\r%03u%%", percent);

            fflush(stdout);
            lastLogPercent = percent;
        }

        lastLogTime = curTime;
    }

    return;
}

/* In multiples of 32bit and assumes that buf[] is so aligned & accessible.
   Also, the buffer has to be in target's endianness - Little Endian */
static int32_t SPI_Read32Bulk(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint32_t data;
    int i;
    int end = addr + len;
    uint8_t *dst = buf;
    int32_t retVal = BCM_ERR_OK;

    /* Check if length is a multiple of 4 */
    if (len & 0x3) {
        HOST_Log("%s length is not aligned to 32bits\n", __func__);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    for (i = addr; i < end; i = i + sizeof(uint32_t)) {
        HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_INDIRECT, i, (uint8_t *)&data, 32UL);

        /* store in Little Endian format */
        dst[3] = (uint8_t)((data >> 24UL) & 0xFF);
        dst[2] = (uint8_t)((data >> 16UL) & 0xFF);
        dst[1] = (uint8_t)((data >>  8UL) & 0xFF);
        dst[0] = (uint8_t)(data & 0xFF);

        dst += 4UL;
    }

done:
    return retVal;
}

/* In multiples of 32bit and assumes that buf[] is so aligned & accessible
   Also, the buffer has to be in target's endianness - Little Endian */
static int32_t SPI_Write32Bulk(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int i;
    int end = addr + len;
    uint32_t data;
    uint8_t *src = buf;
    int32_t retVal = BCM_ERR_OK;

    /* Check if length is a multiple of 4 */
    if (len & 0x3) {
        HOST_Log("%s length is not aligned to 32bits\n", __func__);
        retVal = BCM_ERR_INVAL_PARAMS;
        goto done;
    }

    for (i = addr; i < end; ) {
        /* Retrieve in Little Endian format */
        data =    ((uint32_t)src[3] << 24UL)
                | ((uint32_t)src[2] << 16UL)
                | ((uint32_t)src[1] <<  8UL)
                | src[0];

        HIPC_BusXferWrite(CurrentSpiId, HIPC_BUS_ACCESS_INDIRECT, i, (uint8_t *)&data, 32UL);
        src += 4UL;
        i += 4UL;
    }
done:
    return retVal;
}

uint16_t HIPC_EvenParity(uint16_t val)
{
    uint16_t par = 0U;
    uint16_t i = sizeof(val) * 8U;

    while (i--) {
        par ^= ((val >> i) & 0x1U);
    }
    return par;
}

static uint32_t HIPC_GetChksum(uint32_t magic, uint32_t cmd, uint32_t *msg, uint32_t msglen, uint32_t len)
{
    uint32_t i;
    uint32_t chksum = 0UL;
    for (i = 0UL; i < ((msglen >> 2UL) - IPC_HDR_LAST_INDEX); i++) {
        chksum += uswap32(msg[i]);
    }
    chksum += magic;
    chksum += len;
    chksum += cmd;
    return (~chksum) + 1UL;
}

int32_t HIPC_Send(uint32_t cmd, uint8_t *msg, uint32_t len)
{
    int32_t retVal;
    uint16_t regVal16;
    uint16_t wPtr;
    uint16_t rPtr;
    uint8_t *ipcBuff;
    uint16_t ipcMsgCnt;
    uint16_t ipcMsgSz;
    uint8_t cntRollOverMask;
    uint16_t prio;
    uint32_t idx;
    uint32_t buffer[128];

    if ((sizeof(buffer) - IPC_MSG_HDR_SIZE)
            < len) {
        retVal = BCM_ERR_NOSUPPORT;
        goto done;
    }

    retVal = HIPC_PlatGetBuff(CurrentSpiId, &ipcBuff, &ipcMsgCnt, &cntRollOverMask, &ipcMsgSz);
    if (retVal != BCM_ERR_OK) {
        /*HOST_Log("IPC Buffer is not yet initialised(%d)\n", retVal);*/
        goto done;
    }
    if ((ipcMsgSz - IPC_MSG_HDR_SIZE) < len) {
        HOST_Log("IPC Buffer is not sufficient\n");
        retVal = BCM_ERR_NOSUPPORT;
        goto done;
    }

    HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_TARGET_STAT_REG, (uint8_t *)&regVal16, 16UL);

    prio = (regVal16 & IPC_TARGET_STAT_PRI_MASK) >> IPC_TARGET_STAT_PRI_SHIFT;
    if (prio == IPC_TARGET_STAT_PRI_REBOOT){
        retVal = BCM_ERR_NODEV;
    } else if (prio == IPC_TARGET_STAT_PRI_PAUSE) {
        retVal = BCM_ERR_BUSY;
    } else {
        /* Get pointers */
        rPtr = (regVal16 & IPC_TARGET_STAT_RD_MASK) >> IPC_TARGET_STAT_RD_SHIFT;

        HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_HOST_STAT_REG, (uint8_t *)&regVal16, 16UL);
        wPtr = (regVal16 & IPC_HOST_STAT_WR_MASK) >> IPC_HOST_STAT_WR_SHIFT;
        /* Retain regVal16 for further use */

        /* Check if feasible */
        if (((rPtr + ipcMsgCnt) & IPC_MAX_PTR_MASK) != wPtr) {

            /* The SPI_Write32Bulk API expects the transfer length to be aligned to 32 bytes */
            const uint32_t transferSize = (len + IPC_MSG_HDR_SIZE + 31U) & (~(31U));

            idx = (wPtr & (ipcMsgCnt - 1));
            memset(buffer, 0, sizeof(buffer));
            memcpy(&buffer[IPC_HDR_LAST_INDEX], msg, len);
            buffer[IPC_HDR_MAGIC_INDEX] = IPC_COMMAND_MAGIC;
            buffer[IPC_HDR_CHKSUM_INDEX] =
                uswap32(HIPC_GetChksum(IPC_COMMAND_MAGIC, cmd,
                        &buffer[IPC_HDR_LAST_INDEX],
                        len + IPC_MSG_HDR_SIZE, len));
            buffer[IPC_HDR_COMMAND_INDEX] = uswap32(cmd);
            buffer[IPC_HDR_LENGTH_INDEX] = uswap32(len);

            /* Memcpy */
            retVal = SPI_Write32Bulk((uint32_t)(intptr_t)&ipcBuff[idx*ipcMsgSz],
                    (uint8_t*)buffer,
                    transferSize);
            if (retVal != BCM_ERR_OK) {
                retVal = BCM_ERR_UNKNOWN;
                goto done;
            }

            /* Increment write pointer */
            wPtr++;
            wPtr &= cntRollOverMask;

            /* Update HOST register */
            regVal16 &= ~IPC_HOST_STAT_WR_MASK;
            regVal16 |= ((wPtr << IPC_HOST_STAT_WR_SHIFT) & IPC_HOST_STAT_WR_MASK);
            HIPC_BusXferWrite(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_HOST_STAT_REG, (uint8_t *)&regVal16, 16UL);

            HIPC_PlatNotifyTarget(CurrentSpiId);
        } else {
            retVal = BCM_ERR_NOMEM;
        }
    }

done:
    return retVal;
}

int32_t HIPC_Recv(uint32_t *cmd, uint8_t *msg, uint32_t len_max, uint32_t *len)
{
    int32_t retVal;
    uint16_t regVal16;
    uint16_t wPtr;
    uint16_t rPtr;
    uint8_t *ipcBuff;
    uint16_t ipcMsgCnt;
    uint16_t ipcMsgSz;
    uint8_t cntRollOverMask;
    uint32_t idx;
    uint32_t header[IPC_HDR_LAST_INDEX];
    uint32_t chksum;

    retVal = HIPC_PlatGetBuff(CurrentSpiId, &ipcBuff, &ipcMsgCnt, &cntRollOverMask, &ipcMsgSz);
    if (retVal != BCM_ERR_OK) {
        /* HOST_Log("IPC Buffer is not yet initialised\n"); */
        goto done;
    }

    HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_TARGET_STAT_REG, (uint8_t *)&regVal16, 16UL);
    wPtr = (regVal16 & IPC_TARGET_STAT_WR_MASK) >> IPC_TARGET_STAT_WR_SHIFT;

    HIPC_BusXferRead(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_HOST_STAT_REG, (uint8_t *)&regVal16, 16UL);
    rPtr = (regVal16 & IPC_HOST_STAT_RD_MASK) >> IPC_HOST_STAT_RD_SHIFT;
    /* Retain regVal16 for further use */

    /* Check if there is a new message */
    if (rPtr != wPtr) {
        idx = (rPtr & (ipcMsgCnt - 1)) + ipcMsgCnt;

        /* Memcpy */
        retVal = SPI_Read32Bulk((uint8_t*)header, (uint32_t)(intptr_t)&ipcBuff[idx*ipcMsgSz], sizeof(header));
        if (retVal != BCM_ERR_OK) {
            retVal = BCM_ERR_UNKNOWN;
            goto done;
        }

        *cmd = uswap32(header[IPC_HDR_COMMAND_INDEX]);
        *len = uswap32(header[IPC_HDR_LENGTH_INDEX]);

        if (*len <= (ipcMsgSz - sizeof(header))) {
            retVal = SPI_Read32Bulk(msg, ((uint32_t)(intptr_t)&ipcBuff[idx*ipcMsgSz]) + sizeof(header), *len);
            if (retVal != BCM_ERR_OK) {
                retVal = BCM_ERR_UNKNOWN;
                goto done;
            }

            chksum = HIPC_GetChksum(uswap32(header[IPC_HDR_MAGIC_INDEX]),
                    uswap32(header[IPC_HDR_COMMAND_INDEX]),
                    (uint32_t*)msg, *len + sizeof(header), *len);
            if (IPC_REPLY_MAGIC != uswap32(header[IPC_HDR_MAGIC_INDEX])) {
                retVal = BCM_ERR_DATA_INTEG;
                HOST_Log("IPC: Invalid Magic\n");
            } else if (chksum != uswap32(header[IPC_HDR_CHKSUM_INDEX])) {
                retVal = BCM_ERR_DATA_INTEG;
                HOST_Log("IPC: Failed to validate chksum\n");
            } else {
                retVal = BCM_ERR_OK;
            }
        } else {
            HOST_Log("IPC Receive: incorrect length %u\n", *len);
            retVal = BCM_ERR_NOMEM;
        }

        /* Increment read pointer */
        rPtr++;
        rPtr &= cntRollOverMask;

        /* Update HOST register */
        regVal16 &= ~IPC_HOST_STAT_RD_MASK;
        regVal16 |= ((rPtr << IPC_HOST_STAT_RD_SHIFT) & IPC_HOST_STAT_RD_MASK);
        HIPC_BusXferWrite(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)IPC_HOST_STAT_REG, (uint8_t *)&regVal16, 16UL);

        HIPC_PlatNotifyTarget(CurrentSpiId);
    } else {
        retVal = BCM_ERR_NOT_FOUND;
    }

done:
    return retVal;
}

int32_t HIPC_SendRecv(uint32_t id, uint8_t *cmd, uint32_t cmd_len,
            uint8_t *reply, uint32_t reply_len_max, uint32_t *reply_len_act)
{
    int32_t retVal;
    uint32_t reply_id = 0;
    const uint32_t logProgress = ((id == RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_FLASH_UPDATE))
                                || (id == RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_FLASH_ERASE))
                                || (id == RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_FLASH_WRITE)));

    retVal = HIPC_Send(id, cmd, cmd_len);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("%s: Failed to send command id:0x%x, err:%d\n",
                __func__, id, retVal);
        goto done;
    }

    do {
        /* Check for response */
        retVal = HIPC_Recv(&reply_id, reply, reply_len_max, reply_len_act);
        if (retVal == BCM_ERR_OK) {
            if (reply_id != id) {
                retVal = IPC_QueueAsyncMsg(CurrentSpiId, reply_id, reply, *reply_len_act);
                if (retVal != BCM_ERR_OK) {
                    HOST_Log("%s failed to queue async msg, err:%d\n", __func__, retVal);
                }
                /* we have still not received a response to the msg sent */
                retVal = BCM_ERR_NOT_FOUND;
            }
        }

        if (0U != logProgress) {
            HIPC_ProgressLog(retVal != BCM_ERR_NOT_FOUND);
        }

        if (retVal != BCM_ERR_NOT_FOUND) {
            break;
        }

    } while (retVal != BCM_ERR_OK);

done:
    return retVal;
}

int32_t HIPC_BusXferRead(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr,
                        uint8_t *data, uint32_t width)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (CurrentMode == MGMT_SPI_CONNECTION) {
#if defined(HIPC_ENABLE_SPI)
        retVal = SPI_XferRead(CurrentSpiId, access, addr, data, width);
#endif
#if defined(HIPC_ENABLE_PCIE)
    } else if (CurrentMode == MGMT_PCIE_CONNECTION) {
        retVal = PCIE_XferRead(0UL, access, addr, data, width);
#endif
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
        HOST_Log("HIPC_BusXferRead: invalid bus mode\n");
    }
    return retVal;
}

int32_t HIPC_BusXferWrite(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr,
                        uint8_t *data, uint32_t width)
{
    int32_t retVal = BCM_ERR_OK;

    if (CurrentMode == MGMT_SPI_CONNECTION) {
#if defined(HIPC_ENABLE_SPI)
        retVal = SPI_XferWrite(CurrentSpiId, access, addr, data, width);
#endif
#if defined(HIPC_ENABLE_PCIE)
    } else if (CurrentMode == MGMT_PCIE_CONNECTION) {
        retVal = PCIE_XferWrite(0UL, access, addr, data, width);
#endif
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
        HOST_Log("HIPC_BusXferWrite: invalid bus mode\n");
    }
    return retVal;
}

int32_t HIPC_BusInit(void)
{
    int32_t retVal = BCM_ERR_OK;

#ifdef HIPC_ENABLE_SPI
    /* Initialise GPIO before SPI */
    retVal = GPIO_Init();
    if (BCM_ERR_OK == retVal) {
        retVal = SPI_Init(0UL, SPI_SPEED);
    } else {
        HOST_Log("Could not initialize GPIO interface\n");
    }
#endif
#ifdef HIPC_ENABLE_PCIE
    /* when target is not flashed, PCIE can not be enumarated
     * so we just print error log here but let the server
     * initialize with SPI
     */
    retVal = PCIE_Init(0UL, 0xFFFFFFFUL);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Could not initialize PCIE bus\n");
        retVal = BCM_ERR_OK;
    }
#endif
    return retVal;
}

int32_t HIPC_BusDeInit(void)
{
#ifdef HIPC_ENABLE_SPI
    GPIO_Deinit();
    SPI_Deinit();
#endif
#ifdef HIPC_ENABLE_PCIE
    PCIE_DeInit(0UL);
#endif
    return BCM_ERR_OK;
}

int32_t HIPC_Init(void)
{
    return HIPC_BusInit();
}

void HIPC_Deinit(void)
{
    HIPC_BusDeInit();
}

void HIPC_ProbeSlaves()
{
    int32_t retVal = -1;
    int32_t i;
    uint32_t spiIdIndex = 0;
    uint8_t *buff;
    uint16_t cnt, sz;
    uint8_t cntRollOverMask;
    uint32_t originalSpiId = CurrentSpiId;

#ifdef HIPC_ENABLE_SPI
/*    for (i = SPI_ID_MIN; i <= SPI_ID_MAX; i++) { */
      for (i = SPI_ID_MAX; i >= SPI_ID_MIN; i--) { 
        CurrentSpiId = i;
        retVal = HIPC_PlatGetBuff(CurrentSpiId, &buff, &cnt, &cntRollOverMask, &sz);
        if (BCM_ERR_OK == retVal) {
            /* Store found ID */
            if (ActiveSpiId[spiIdIndex] != i) {	
                ActiveSpiId[spiIdIndex] = i;
                HOST_Log("Found slave with SPI ID:%u\n", i);
            }
            spiIdIndex++;
        }
    }

    /* Invalidate remaining IDs*/
    while (spiIdIndex < SPI_SLAVES_MAX) {
        ActiveSpiId[spiIdIndex++] = SPI_ID_INVAL;
    }
    CurrentSpiId = originalSpiId;
#endif

#ifdef HIPC_ENABLE_PCIE
    retVal = HIPC_PlatGetBuff(0UL, &buff, &cnt, &cntRollOverMask, &sz);
    if (BCM_ERR_OK == retVal) {
        PCIESlavePresent = 1UL;
    }
#endif
}

int32_t HIPC_GetSlave(uint32_t *id)
{
    int32_t retVal;
    if (NULL == id) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *id = CurrentSpiId;
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

int32_t HIPC_SetSlave(uint32_t id)
{
    int32_t retVal;
    if ((id >= SPI_ID_MIN) && (id <= SPI_ID_MAX)) {
        CurrentSpiId = id;
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

int32_t HIPC_GetActiveSlaves(uint32_t *id, uint32_t *count)
{
    int32_t retVal;
    int32_t i = 0;
    uint32_t actual;

    if ((NULL != id) && (NULL != count)) {
        for (i = 0, actual = 0; (i < *count) && (i < SPI_SLAVES_MAX); i++) {
            if (SPI_ID_INVAL == ActiveSpiId[i]) {
                break;
            }
            id[i] = ActiveSpiId[i];
            actual += 1;
        }

        /* Invalidate remaining IDs*/
        while (i < *count) {
            id[i++] = SPI_ID_INVAL;
        }

        *count = actual;
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

int32_t HIPC_IsStacked(void)
{
    int32_t retval = 0;

    if (ActiveSpiId[1] != SPI_ID_INVAL) {
        retval = 1;
    }

    return retval;
}

int32_t HIPC_SetConnMode(mgmt_connection_t mode)
{
    int32_t retVal = BCM_ERR_OK;

    if (MGMT_SPI_CONNECTION == mode) {
#if HIPC_ENABLE_SPI
        CurrentMode = mode;
#else
        retVal = BCM_ERR_NOSUPPORT;
#endif
    }
    if (MGMT_PCIE_CONNECTION == mode) {
#ifdef HIPC_ENABLE_PCIE
        CurrentMode = mode;
#else
        retVal = BCM_ERR_NOSUPPORT;
#endif
    }
    return retVal;
}

mgmt_connection_t HIPC_GetConnMode(void)
{
    return CurrentMode;
}

static int32_t HIPC_TransferBl(const uint8_t* image, uint32_t len)
{
    int32_t retVal;
    uint32_t recv_len;
    uint32_t cmdId;
    uint32_t i, itr, offset, cpy_sz;
    uint32_t len_loc = len - BL_IMG_SIGNATURE_SIZE;
    const uint8_t *image_loc = &image[BL_IMG_SIGNATURE_SIZE];
    BL_DWNLD_ImgHdrType *imgHdrPtr = (BL_DWNLD_ImgHdrType *)snd_msg;

    memset(snd_msg, 0, MSG_BUF_SIZE);
    memset(recv_msg, 0, MSG_BUF_SIZE);

    /* Send Download header */
    cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);

    imgHdrPtr->magicNum = uswap32(0x494D4748UL);
    imgHdrPtr->imgSize = uswap32(len_loc);
    imgHdrPtr->flags = uswap32(BL_DWNLD_FLAG_EDC_CRC);
    imgHdrPtr->errDetCode = uswap64(BCM_CRC32(image_loc, len_loc, CRC32_POLY));

    /* Copy BL signature after header */
    memcpy(&snd_msg[sizeof(BL_DWNLD_ImgHdrType)], image, BL_IMG_SIGNATURE_SIZE);

    retVal = HIPC_PlatSendRecvRom(cmdId, snd_msg,
            sizeof(BL_DWNLD_ImgHdrType) + BL_IMG_SIGNATURE_SIZE, recv_msg,
            MSG_BUF_SIZE, &recv_len);

    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    itr = (len_loc / MSG_BUF_SIZE);
    if (0UL != (len_loc%MSG_BUF_SIZE)) {
        itr++;
    }

    offset = 0UL;
    for (i = 0UL; i < itr; i++) {
        memset(snd_msg, 0, MSG_BUF_SIZE);
        memset(recv_msg, 0, MSG_BUF_SIZE);

        cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
        /* copy image chunk */
        cpy_sz = len_loc - offset;
        if (cpy_sz > MSG_BUF_SIZE) {
            cpy_sz = MSG_BUF_SIZE;
        }
        memcpy(snd_msg, &image_loc[offset], cpy_sz);

        retVal = HIPC_PlatSendRecvRom(cmdId, snd_msg,
                cpy_sz, recv_msg, MSG_BUF_SIZE, &recv_len);
        if (retVal != BCM_ERR_OK) {
            goto done;
        }

        offset += cpy_sz;
    }

done:
    return retVal;
}

int32_t HIPC_ExecuteOnTarget(MgmtInfoType *info, HIPC_InstallType mode, const uint8_t* image, uint32_t len)
{
    int32_t retVal;
    HIPC_TargetFeatureType currTargetFeatures;
    SYS_BootModeType currTargetBootMode;
    volatile uint32_t isReady = FALSE;
    uint16_t reg16;

    retVal = HIPC_GetTargetSWMode(info, &currTargetBootMode, &currTargetFeatures);
    if (BCM_ERR_OK != retVal) {
        goto err_GetSWMode;
    }

    /* Instruct bootloader to enter/halt-at Updater mode */
    reg16 = MCU_FW_BOOT_INFO_SW_RESET_MASK
        | (MCU_FW_BOOT_INFO_DWNLD_MODE_IPC << MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT);

    if (HIPC_INSTALL_FACTORY != mode) {
        reg16 |= MCU_FW_BOOT_INFO_DWNLD_MODE_PARTIAL_MASK;
    }
    HIPC_BusXferWrite(CurrentSpiId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)BL_BOOT_MODE_SPARE_REG, (uint8_t *)&reg16, 16UL);

    if (0UL == (currTargetFeatures & HIPC_TARGETFEATURE_SELFREBOOT)) {
        /* Try pushing the updater/bootloader to TCM directly */
        retVal = HIPC_PlatExecuteOnTarget(CurrentSpiId, image, len);
    } else {
        /* Push the updater/bootloader through IPC */
        retVal = HIPC_TransferBl(image,len);
    }

    if (BCM_ERR_OK == retVal) {
        do {
            isReady = HIPC_IsTargetReady();
        } while (TRUE != isReady);
    }

err_GetSWMode:
    return retVal;
}

void HIPC_GenerateFrameSyncPulse(void)
{
    GPIO_Set(GPIO_SYNC_OUT, GPIO_LEVEL_HIGH);
    GPIO_Set(GPIO_SYNC_OUT, GPIO_LEVEL_LOW);
}

