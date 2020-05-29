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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hipc.h"
#include <rpc_cmds.h>
#include <sys_ipc_cmds.h>
#include <bcm_err.h>
#include <host_system.h>
#include <host_etherswt.h>
#include <utils.h>
#include <host_imgl.h>
#include <hlog.h>
#include <time.h>
#include <sys/time.h>


static SYS_KeepAliveType LatestKeepAlive[3UL];

#define MGMT_OTP_READ_CMD_SIZE                  (12UL)
#define MGMT_OTP_READ_REPLY_SIZE                (16UL)
#define MGMT_OTP_CMD_DATA_SIZE                  (4UL)
#define MGMT_OTP_WRITE_CMD_SIZE                 (16UL)
#define MGMT_OTP_KEY_WRITE_OTP_INFO_SIZE        (12UL)
#define MGMT_OTP_WRITE_MAC_ADDR_INFO_SIZE       (16UL)
#define MGMT_OTP_READ_MACADDR_REPLY_SIZE        (28UL)


static int32_t HOST_SysOTPReadInt(MgmtInfoType *info, uint32_t row_num, uint32_t *value)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
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

    cmdHdl.otpInfo->size = uswap32(MGMT_OTP_CMD_DATA_SIZE);
    cmdHdl.otpInfo->addr = uswap32(row_num);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_RD);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            MGMT_OTP_READ_CMD_SIZE, (uint8_t *)&resp,
            RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != MGMT_OTP_READ_REPLY_SIZE) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *value = ((uint32_t)(respHdl.otpInfo->data[0UL])) & 0xFFUL;
        *value |= ((((uint32_t)(respHdl.otpInfo->data[1UL])) & 0xFFUL) << 8UL);
        *value |= ((((uint32_t)(respHdl.otpInfo->data[2UL])) & 0xFFUL) << 16UL);
        *value |= ((((uint32_t)(respHdl.otpInfo->data[3UL])) & 0xFFUL) << 24UL);
    }

done:
    return retVal;
}

static int32_t HOST_SysOTPWriteInt(MgmtInfoType *info,
                            uint32_t row_num,
                            uint32_t data)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
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

    cmdHdl.otpInfo->size = uswap32(MGMT_OTP_CMD_DATA_SIZE);
    cmdHdl.otpInfo->addr = uswap32(row_num);
    cmdHdl.otpInfo->data[0] = (uint8_t)(data & 0xFFUL);
    cmdHdl.otpInfo->data[1] = (uint8_t)((data >> 8UL) & 0xFFUL);
    cmdHdl.otpInfo->data[2] = (uint8_t)((data >> 16UL) & 0xFFUL);
    cmdHdl.otpInfo->data[3] = (uint8_t)((data >> 24UL) & 0xFFUL);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_WRITE_CUST);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            MGMT_OTP_WRITE_CMD_SIZE, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

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

static int32_t HOST_SysOTPEnableSecInt(MgmtInfoType *info)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
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

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_ENABLE_SEC);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            0UL, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

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

static int32_t HOST_SysOTPKeyWriteInt(MgmtInfoType *info, uint8_t *image, uint32_t len)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
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
    if ((SYS_OTP_SECURE_KEY_SIZE_LOC * 4UL) < len) {
        HOST_Log("%s :: Key length(%u) too big\n",
            __FUNCTION__, len);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdHdl.otpInfo->size = uswap32(len);
    cmdHdl.otpInfo->addr = uswap32(0UL);

    memcpy(cmdHdl.otpInfo->data, image, len);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_WR_SEC_KEY);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            (len + MGMT_OTP_KEY_WRITE_OTP_INFO_SIZE),
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

static int32_t HOST_SysOTPMacAddrWriteInt(MgmtInfoType *info, uint8_t *const mac_addr, uint32_t loc)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
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

    cmdHdl.otpInfo->size = uswap32(6UL);
    cmdHdl.otpInfo->addr = uswap32(loc);

    memcpy(cmdHdl.otpInfo->data, mac_addr, 6UL);
    HOST_Log("OTP MAC addr: %x:%x:%x:%x:%x:%x\n",
            (uint32_t)(mac_addr[0]),
            (uint32_t)(mac_addr[1]),
            (uint32_t)(mac_addr[2]),
            (uint32_t)(mac_addr[3]),
            (uint32_t)(mac_addr[4]),
            (uint32_t)(mac_addr[5]));

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_WR_MAC_ADDR);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            (MGMT_OTP_WRITE_MAC_ADDR_INFO_SIZE),
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

static int32_t HOST_SysOTPMacAddrReadInt(MgmtInfoType *info, uint8_t *mac_addr1, uint8_t *mac_addr2, uint32_t *valid)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (NULL == mac_addr1) || (NULL == mac_addr2)
            || (NULL == valid)) {
        HOST_Log("%s :: Invalid input parameter(info:%p, mac_addr1:%p,"
                " mac_addr2:%p, valid:%p)\n", __FUNCTION__, info, mac_addr1,
                mac_addr2, valid);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_OTP_RD_MAC_ADDR);
    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            0UL, (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != MGMT_OTP_READ_MACADDR_REPLY_SIZE) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *valid = uswap32(*((uint32_t *)(&respHdl.otpInfo->data[0])));
        memcpy(mac_addr1, &(respHdl.otpInfo->data[4]), 6UL);
        memcpy(mac_addr2, &(respHdl.otpInfo->data[10]), 6UL);
    }

done:
    return retVal;
}

int32_t HOST_SysOTPRead(MgmtInfoType *info, uint32_t row_num, uint32_t *value)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HOST_SysOTPReadInt(info, row_num, value);

exit:
    return retVal;
}

int32_t HOST_SysOTPWrite(MgmtInfoType *info, uint32_t row_num, uint32_t data)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't enter update mode!\n", __func__);
        goto exit;
    }

    retVal = HOST_SysOTPWriteInt(info, row_num, data);

exit:
    return retVal;
}

int32_t HOST_SysOTPEnableSec(MgmtInfoType *info)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't enter update mode!\n", __func__);
        goto exit;
    }

    retVal = HOST_SysOTPEnableSecInt(info);

exit:
    return retVal;
}

int32_t HOST_SysOTPKeyWrite(MgmtInfoType *info, uint8_t *image, uint32_t len)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't enter update mode!\n", __func__);
        goto exit;
    }

    retVal = HOST_SysOTPKeyWriteInt(info, image, len);

exit:
    return retVal;
}

int32_t HOST_SysOTPMacAddrWrite(MgmtInfoType *info, uint8_t *const mac_addr, uint32_t loc)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }
    if (mac_addr == NULL) {
        HOST_Log("%s :: mac_addr pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't enter update mode!\n", __func__);
        goto exit;
    }

    retVal = HOST_SysOTPMacAddrWriteInt(info, mac_addr, loc);

exit:
    return retVal;
}

int32_t HOST_SysOTPMacAddrRead(MgmtInfoType *info, uint8_t *mac_addr1, uint8_t *mac_addr2, uint32_t *valid)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto exit;
    }

    retVal = HOST_SysOTPMacAddrReadInt(info, mac_addr1, mac_addr2, valid);

exit:
    return retVal;
}

int32_t HOST_SysPing(MgmtInfoType *info, uint32_t *const mode, uint32_t * const targetFeatures)
{
    return HIPC_Ping(info, mode, targetFeatures);
}

int32_t HOST_SysReboot(MgmtInfoType *info)
{
    return HIPC_Reboot(info);
}

int32_t HOST_SysOSVersionGet(MgmtInfoType *info, char *ver)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (ver == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info:%p, ver:%p)\n",
            __FUNCTION__, info, ver);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_SW_VERSION);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(SYS_OSSWVersionType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(SYS_OSSWVersionType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        memcpy(ver, &respHdl.osVersion->str[0], sizeof(SYS_OSSWVersionType));
    }

done:
    return retVal;
}

int32_t HOST_SysMCUInfoGet(MgmtInfoType *info, uint32_t *manuf, uint32_t *model,
                        uint32_t *rev)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;

    if ((info == NULL) || (manuf == NULL) || (model == NULL) ||
        (rev == NULL)) {
        HOST_Log("%s :: Invalid input parameter(info:%p, manuf:%p model:%p rev:%p)\n",
                __FUNCTION__, info, manuf, model, rev);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_CHIP_ID);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(MCU_VersionType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(MCU_VersionType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        *manuf = uswap32(respHdl.mcuVersion->manuf);
        *model = uswap32(respHdl.mcuVersion->model);
        *rev = uswap32(respHdl.mcuVersion->rev);
    }

done:
    return retVal;
}

static int32_t HOST_SysSendCmd(uint32_t cmd, SYS_UpdateExecCmdType *sendMsg)
{
    int32_t retVal = -1;
    uint32_t recv_len;
    uint32_t cmdId;
    uint8_t buffer[RPC_RESPPAYLOADSZ];

    cmdId = (BCM_GROUPID_SYS << RPC_CMD_GROUP_SHIFT);
    cmdId |= (cmd << RPC_CMD_ID_SHIFT);

    retVal = HIPC_SendRecv(cmdId, (uint8_t*)sendMsg,
            sizeof(SYS_UpdateExecCmdType), (uint8_t*)buffer,
            sizeof(buffer), &recv_len);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (BCM_ERR_OK != uswap32(*(uint32_t*)buffer)) {
        HOST_Log("%s status:%d\n", __func__, uswap32(*(uint32_t*)buffer));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (recv_len != sizeof(int32_t)) {
        HOST_Log("%s len:0x%x\n", __func__, recv_len);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}


int32_t HOST_SysFirmwareUpdate(MgmtInfoType *info, uint32_t mode, const uint8_t *image, uint32_t len)
{
    int32_t retVal = -1;
    uint32_t offset = 0;
    uint32_t size = 0;
    struct timeval tv_start;
    struct timeval tv_end, tv_diff;
    const uint8_t *tar = image + 20UL;
    HIPC_InstallType hipcInstallMode;
    SYS_UpdateExecCmdType sendMsgBuf;

    memset(&sendMsgBuf, 0, sizeof(SYS_UpdateExecCmdType));
    gettimeofday(&tv_start, NULL);

    if (INSTALL_MODE_ALL == mode){
        hipcInstallMode = HIPC_INSTALL_FACTORY;
    } else {
        hipcInstallMode = HIPC_INSTALL_OTA;
    }

    retVal = HOST_ImglInit(tar, len);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Failed to parse the TAR image\n");
        goto tar_parse_error;
    }

    retVal = HIPC_EnterUpdateMode(info, hipcInstallMode);
    if (BCM_ERR_OK != retVal ) {
        retVal = HOST_ImglGetBootloader(&offset, &size);
        if ((retVal == -1) || (size == 0) || (offset == 0)) {
            HOST_Log("failed to get bootloader\n");
            goto get_bl_err;
        }

        HOST_Log("%s Couldn't enter update mode, transferring updater\n",
                __func__);
        retVal = HIPC_ExecuteOnTarget(info, hipcInstallMode, &tar[offset], size);
        if (BCM_ERR_OK != retVal) {
            goto execute_err;
        }
    }

    if (INSTALL_MODE_BL == mode) {
        retVal = BCM_ERR_OK;
        goto dl_all_err;
    }

    retVal = HOST_ImglGetParsedImgHdrs(&sendMsgBuf);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't get Parsed Image Headers, \n", __func__);
        goto tar_parse_error;
    }
    sendMsgBuf.flashId = uswap32(0UL);

    retVal = HOST_SysSendCmd(SYS_CMDID_FLASH_UPDATE, &sendMsgBuf);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("Failed to program the flash memory err=%d\n", retVal);
        goto dl_all_err;
    }

    gettimeofday(&tv_end, NULL);
    timersub(&tv_end, &tv_start, &tv_diff);
    HOST_Log("\nProgrammed flash memory successfully in %lus\n", tv_diff.tv_sec);

    HOST_ImglDeInit();

dl_all_err:
execute_err:
get_bl_err:
tar_parse_error:
    return retVal;
}

int32_t HOST_SysFirmwareExecute(MgmtInfoType *info, const uint8_t *image, uint32_t len)
{
    int32_t retVal = -1;
    uint32_t offset = 0;
    uint32_t size = 0;
    const uint8_t *tar = image + 20UL;
    struct timeval tv_start;
    struct timeval tv_end, tv_diff;
    SYS_UpdateExecCmdType sendMsgBuf;

    memset(&sendMsgBuf, 0, sizeof(SYS_UpdateExecCmdType));
    gettimeofday(&tv_start, NULL);

    retVal = HOST_ImglInit(tar, len);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Failed to parse the TAR image\n");
        goto err_exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal ) {
        retVal = HOST_ImglGetBootloader(&offset, &size);
        if ((retVal == -1) || (size == 0) || (offset == 0)) {
            HOST_Log("failed to get bootloader\n");
            goto err_exit;
        }

        HOST_Log("%s Couldn't enter update mode, transferring updater\n",
                __func__);
        retVal = HIPC_ExecuteOnTarget(info, HIPC_INSTALL_OTA, &tar[offset], size);
        if (BCM_ERR_OK != retVal) {
            goto err_exit;
        }
    }

    retVal = HOST_ImglGetParsedImgHdrs(&sendMsgBuf);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't get Parsed Image Headers, \n", __func__);
        goto err_exit;
    }
    sendMsgBuf.flashId = uswap32(0UL);

    retVal = HOST_SysSendCmd(SYS_CMDID_EXECUTE, &sendMsgBuf);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("Failed to execute firmware err=%d\n", retVal);
    } else {
        gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_start, &tv_diff);
        HOST_Log("\nExecuted Firmware in %lus\n", tv_diff.tv_sec);
    }
    HOST_ImglDeInit();

err_exit:
    return retVal;
}

int32_t HOST_SysBLExecute(MgmtInfoType *info, const uint8_t *image, uint32_t len)
{
    int32_t retVal = -1;
    uint32_t offset = 0;
    uint32_t size = 0;
    const uint8_t *tar = image + 20UL;
    struct timeval tv_start;
    struct timeval tv_end, tv_diff;
    SYS_UpdateExecCmdType sendMsgBuf;

    memset(&sendMsgBuf, 0, sizeof(SYS_UpdateExecCmdType));
    gettimeofday(&tv_start, NULL);

    retVal = HOST_ImglInit(tar, len);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Failed to parse the TAR image\n");
        goto err_exit;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal ) {
        retVal = HOST_ImglGetBootloader(&offset, &size);
        if ((retVal == -1) || (size == 0) || (offset == 0)) {
            HOST_Log("failed to get bootloader\n");
            goto err_exit;
        }

        HOST_Log("%s Couldn't enter update mode, transferring updater\n",
                __func__);
        retVal = HIPC_ExecuteOnTarget(info, HIPC_INSTALL_OTA, &tar[offset], size);
        if (BCM_ERR_OK != retVal) {
            goto err_exit;
        }
    }

    retVal = HOST_ImglGetParsedImgHdrs(&sendMsgBuf);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't get Parsed Image Headers, \n", __func__);
        goto err_exit;
    }
    sendMsgBuf.flashId = uswap32(0UL);

    retVal = HOST_SysSendCmd(SYS_CMDID_APPLY_XCVR_CFG, &sendMsgBuf);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("Failed to apply xcvr config. err=%d\n", retVal);
    } else {
        gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_start, &tv_diff);
        HOST_Log("\nExecuted BL+XCVR in %lus\n", tv_diff.tv_sec);
    }
    HOST_ImglDeInit();

err_exit:
    return retVal;
}

int32_t HOST_SysFlashErase(MgmtInfoType *info, const uint8_t *image, uint32_t len)
{
    int32_t retVal = -1;
    uint32_t offset = 0;
    uint32_t size = 0;
    struct timeval tv_start, tv_end, tv_diff;
    const uint8_t *tar = image + 20UL;
    SYS_UpdateExecCmdType sendMsgBuf;

    memset(&sendMsgBuf, 0, sizeof(SYS_UpdateExecCmdType));
    gettimeofday(&tv_start, NULL);

    retVal = HOST_ImglInit(tar, len);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("Failed to parse the TAR image\n");
        goto get_bl_err;
    }

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal ) {
        retVal = HOST_ImglGetBootloader(&offset, &size);
        if ((retVal == -1) || (size == 0) || (offset == 0)) {
            HOST_Log("failed to get bootloader\n");
            goto get_bl_err;
        }

        HOST_Log("%s Couldn't enter update mode, transferring updater\n",
                __func__);
        retVal = HIPC_ExecuteOnTarget(info, HIPC_INSTALL_OTA, &tar[offset], size);
        if (BCM_ERR_OK != retVal) {
            goto execute_err;
        }
    }

    retVal = HOST_SysSendCmd(SYS_CMDID_FLASH_ERASE, &sendMsgBuf);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("Failed to erase err=%d\n", retVal);
        goto erase_all_err;
    } else {
        gettimeofday(&tv_end, NULL);
        timersub(&tv_end, &tv_start, &tv_diff);
        HOST_Log("\nErased flash memory successfully in %lus\n", tv_diff.tv_sec);
    }

erase_all_err:
execute_err:
get_bl_err:
    return retVal;
}

int32_t HOST_SysFlashWrite(MgmtInfoType *info, uint32_t flash_addr)
{
    int32_t retVal = -1;
    SYS_UpdateExecCmdType sendMsgBuf;
    SYS_UpdateExecCmdType *flsInfo = &sendMsgBuf;

    memset(&sendMsgBuf, 0, sizeof(SYS_UpdateExecCmdType));

    retVal = HIPC_EnterUpdateMode(info, HIPC_INSTALL_OTA);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s Couldn't enter update mode\n", __func__);
        goto err;
    }

    flsInfo->flashId = uswap32(0UL);
    flsInfo->flashAddr = uswap32(flash_addr);
    retVal = HOST_SysSendCmd(SYS_CMDID_FLASH_WRITE, &sendMsgBuf);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("Failed to write to flash err=%d\n", retVal);
        goto err;
    } else {
        HOST_Log("Flash write succeeded\n");
    }

err:
    return retVal;
}


int32_t HOST_SysNotificationHandler(uint32_t currentSlave,
            BCM_CompIDType comp, SYS_AsyncIdType notificationId,
            uint8_t *msg, uint32_t size)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    SYS_HandleType notificationHdl;

    notificationHdl.u8Ptr = msg;

    if (BCM_RSV_ID != comp) {
        HOST_Log("%s SPI-Id:%u Invalid parameter comp:0x%x notificationId:0x%x size=%d\n",
            __func__, currentSlave, comp, notificationId, size);
        goto done;
    }

    switch (notificationId) {
    case SYS_ASYNCID_KEEPALIVE:
        LatestKeepAlive[currentSlave].upTime.s = uswap32(notificationHdl.keepAlive->upTime.s);
        LatestKeepAlive[currentSlave].upTime.ns = uswap32(notificationHdl.keepAlive->upTime.ns);
        LatestKeepAlive[currentSlave].count = uswap32(notificationHdl.keepAlive->count);
        retVal = BCM_ERR_OK;
        break;
    default:
        HOST_Log("%s SPI-Id:%u Invalid parameter comp:0x%x notificationId:0x%x size=%d\n",
            __func__, currentSlave, comp, notificationId, size);
        break;
    }

done:
    return retVal;
}

int32_t HOST_SysKeepAliveGet(MgmtInfoType *info, uint32_t slaveID,
                    SYS_KeepAliveType *keepAlive)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((info == NULL) || (keepAlive == NULL)) {
        HOST_Log("%s Invalid parameters info:%p keepAlive:%p\n", __func__, info, keepAlive);
        goto done;
    }

    memcpy((uint8_t *)keepAlive, (uint8_t *)&LatestKeepAlive[slaveID], sizeof(SYS_KeepAliveType));
    retVal = BCM_ERR_OK;

done:
    return retVal;
}

#ifdef ENABLE_DBGMEM
int32_t HOST_MemoryWrite(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                         uint32_t width, uint32_t len, volatile uint8_t *data)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;
    DBGMEM_HandleType dstMemHdl, srcMemHdl;
    int i;

    srcMemHdl.u8Ptr = data;
    dstMemHdl.u8Ptr = &cmdHdl.memAccess->data[0];

    if ((info == NULL) || (data == NULL) || (len == 0UL)
        || ((width != 8) && (width != 16) && (width != 32))) {
        HOST_Log("%s Invalid parameters info:%p data:%p len:%u\n", __func__, info, data, len);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_MEM_WRITE);

    cmdHdl.memAccess->addr = uswap32(addr);
    cmdHdl.memAccess->width = uswap32(width);
    cmdHdl.memAccess->len = uswap32(len);
    cmdHdl.memAccess->deviceID = uswap32(destn);

    for (i = 0; i < len; i++) {
        if (width == 8) {
            *dstMemHdl.u8Ptr = *srcMemHdl.u8Ptr;
            srcMemHdl.u8Ptr++;
            dstMemHdl.u8Ptr++;
        } else if (width == 16) {
            *dstMemHdl.u16Ptr = uswap16(*srcMemHdl.u16Ptr);
            srcMemHdl.u16Ptr++;
            dstMemHdl.u16Ptr++;
        } else if (width == 32) {
            *dstMemHdl.u32Ptr = uswap32(*srcMemHdl.u32Ptr);
            srcMemHdl.u32Ptr++;
            dstMemHdl.u32Ptr++;
        }
    }

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(SYS_MemAccessType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(SYS_MemAccessType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    }

done:
    return retVal;
}

int32_t HOST_MemoryRead(MgmtInfoType *info, MCU_DeviceType destn, uint32_t addr,
                        uint32_t width, uint32_t len, volatile uint8_t *data)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint8_t cmdPayload[RPC_CMDPAYLOADSZ];

    SYS_HandleType cmdHdl;
    cmdHdl.u8Ptr = &cmdPayload[0];

    RPC_ResponseType resp;
    SYS_HandleType respHdl;
    respHdl.u8Ptr = &resp.payload[0];

    uint32_t replyLen;
    uint32_t cmdId;
    DBGMEM_HandleType dstMemHdl, srcMemHdl;
    int i;

    dstMemHdl.u8Ptr = data;
    srcMemHdl.u8Ptr = &respHdl.memAccess->data[0];

    if ((info == NULL) || (data == NULL) || (len == 0UL)
        || ((width != 8) && (width != 16) && (width != 32))) {
        HOST_Log("%s Invalid parameters info:%p data:%p len:%u\n", __func__, info, data, len);
        goto done;
    }

    memset(cmdHdl.u8Ptr, 0, RPC_CMDPAYLOADSZ);
    memset(respHdl.u8Ptr, 0, RPC_RESPPAYLOADSZ);

    cmdId = RPC_CMDID(BCM_GROUPID_SYS, BCM_RSV_ID, SYS_CMDID_MEM_READ);

    cmdHdl.memAccess->addr = uswap32(addr);
    cmdHdl.memAccess->width = uswap32(width);
    cmdHdl.memAccess->len = uswap32(len);
    cmdHdl.memAccess->deviceID = uswap32(destn);

    retVal = HIPC_SendRecv(cmdId, cmdHdl.u8Ptr,
            sizeof(SYS_MemAccessType), (uint8_t *)&resp, RPC_CMDPAYLOADSZ, &replyLen);

    if (retVal != BCM_ERR_OK) {
        goto done;
    } else if (uswap32(resp.status) != BCM_ERR_OK) {
        HOST_Log("%s status:%d\n", __func__, uswap32(resp.status));
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    } else if (replyLen != (MGMT_STATUS_LEN + sizeof(SYS_MemAccessType))) {
        HOST_Log("%s len:0x%x\n", __func__, replyLen);
        retVal = BCM_ERR_DATA_INTEG;
        goto done;
    } else {
        for (i = 0; i < len; i++) {
            if (width == 8) {
                *dstMemHdl.u8Ptr = *srcMemHdl.u8Ptr;
                srcMemHdl.u8Ptr++;
                dstMemHdl.u8Ptr++;
            } else if (width == 16) {
                *dstMemHdl.u16Ptr = uswap16(*srcMemHdl.u16Ptr);
                srcMemHdl.u16Ptr++;
                dstMemHdl.u16Ptr++;
            } else if (width == 32) {
                *dstMemHdl.u32Ptr = uswap32(*srcMemHdl.u32Ptr);
                srcMemHdl.u32Ptr++;
                dstMemHdl.u32Ptr++;
            }
        }
    }

done:
    return retVal;
}
#endif

int32_t HOST_SysInit(MgmtInfoType *info)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    memset(info, 0, sizeof(MgmtInfoType));

    /* Connect through SPI */
    info->connection_type = MGMT_SPI_CONNECTION;

    /*initialize spi*/
    retVal = HIPC_Init();

done:
    return retVal;
}

int32_t HOST_SysDeinit(MgmtInfoType *info)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (info == NULL) {
        HOST_Log("%s :: MgmtInfoType pointer is NULL\n", __FUNCTION__);
        goto done;
    }
    memset(info, 0, sizeof(MgmtInfoType));

    /* Connect through SPI */
    info->connection_type = MGMT_NO_CONNECTION;

    HIPC_Deinit();
    retVal = BCM_ERR_OK;

done:
    return retVal;
}
