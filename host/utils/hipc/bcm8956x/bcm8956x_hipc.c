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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "ipc_hwif.h"
#include "hipc.h"
#include "spi.h"
#include <bcm_err.h>
#include "bl_downloader.h"
#include "bl_ipc_downloader.h"
#include "bl_chip_config.h"
#include "mcu.h"
#include "utils.h"
#include <rpc_cmds.h>
#include <host_imgl.h>
#include <hipc_plat.h>
#include <bl_ipc_hwif.h>
#include <hlog.h>
#include "bus_xfer.h"

int32_t HIPC_PlatReboot(uint32_t slaveId, SYS_BootModeType currTargetBootMode,
        HIPC_TargetFeatureType currTargetFeatures)
{
    /* Nothing to be done by the host */
    return BCM_ERR_OK;
}

int32_t HIPC_PlatResetCPUWait(uint32_t slaveId)
{
    return BCM_ERR_NOSUPPORT;
}

int32_t HIPC_PlatExecuteOnTarget(uint32_t slaveId, const uint8_t* image, uint32_t size)
{
    return BCM_ERR_NOSUPPORT;
}

int32_t HIPC_PlatGetTargetInfo(uint32_t slaveId, uint32_t *const device_id,
                            uint32_t *const revision)
{
    int32_t retVal;
    uint16_t tmp = 0U;

    if ((NULL == device_id) || (NULL == revision)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    *device_id = 0UL;
    *revision = 0UL;

    retVal = HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)MISC_DEVICEID_LO, (uint8_t *)&tmp, 16UL);
    if (BCM_ERR_OK != retVal) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    *device_id = (uint32_t)(tmp & CHIPMISC_DEVICEID_LO_DEVICE_ID_LO_MASK);

    tmp = 0U;
    retVal = HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)MISC_DEVICEID_HI, (uint8_t *)&tmp, 16UL);
    if (BCM_ERR_OK != retVal) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }

    *device_id |= (uint32_t)((tmp & CHIPMISC_DEVICEID_HI_DEVICE_ID_HI_MASK)
            << 12UL);

    retVal = HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)MISC_MODEL_REV_NUM, (uint8_t *)&tmp, 16UL);
    if (BCM_ERR_OK != retVal) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }

    *revision = (uint32_t)(tmp & CHIPMISC_MODEL_REV_NUM_REV_NUM_MASK);
err:
    return retVal;
}


void HIPC_PlatNotifyTarget(uint32_t slaveId)
{
    uint16_t regVal16;

    /* Trigger IPC interrupt to target */
    HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)MISC_CPUSYS_MISC, (uint8_t *)&regVal16, 16UL);
    regVal16 |= 1U;
    HIPC_BusXferWrite(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)MISC_CPUSYS_MISC, (uint8_t *)&regVal16, 16UL);
}

int32_t HIPC_PlatGetBuff(uint32_t slaveId, uint8_t **buff, uint16_t *cnt,
                         uint8_t *cntRollOverMask, uint16_t *sz)
{
    uint16_t reg1Val16;
    uint16_t reg2Val16;
    int32_t retVal = BCM_ERR_OK;

    HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT,
        (uint32_t)(intptr_t)IPC_BUFF_INFO_REG, (uint8_t *)&reg1Val16, 16UL);

    HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT,
        (uint32_t)(intptr_t)IPC_BUFF_INFO2_REG, (uint8_t *)&reg2Val16, 16UL);

    if ((reg1Val16 != 0xFFFF) && (reg1Val16 != 0)
        && (HIPC_EvenParity(reg1Val16) == 0U)
        && (HIPC_EvenParity(reg2Val16) == 0U)) {
        *cnt = 1 << ((reg1Val16 & BL_IPC_BUFF_INFO_CNT_MASK) >> BL_IPC_BUFF_INFO_CNT_SHIFT);
        *sz = 1 << ((reg1Val16 & BL_IPC_BUFF_INFO_SZ_MASK) >> BL_IPC_BUFF_INFO_SZ_SHIFT);

        *buff = (uint8_t *)(intptr_t)((((reg1Val16 & BL_IPC_BUFF_INFO_BASE_MASK)
                        >> BL_IPC_BUFF_INFO_BASE_SHIFT)
                    << BL_IPC_BUFF_INFO_BASE_ALIGN_SHIFT)
                | (((reg2Val16 & BL_IPC_BUFF_INFO2_BASE_MASK)
                        >> BL_IPC_BUFF_INFO2_BASE_SHIFT)
                    << BL_IPC_BUFF_INFO2_BASE_ALIGN_SHIFT));

        HIPC_BusXferRead(slaveId, HIPC_BUS_ACCESS_DIRECT, (uint32_t)(intptr_t)BL_DWNLD_TARGET_SPARE_REG, (uint8_t *)&reg1Val16, 16UL);
        if ((1U == *cnt)    /* BROM and BL */
            && (BL_IPC_DWNLD_BL_READY_MASK != (reg1Val16 & BL_IPC_DWNLD_BL_READY_MASK))) {
            /* BootROM only */
            *cntRollOverMask = 0x1U; /* 0-1 */
        } else {
            *cntRollOverMask = 0xFU; /* 0-15 */
        }
    } else {
        *cnt = 0UL;
        *sz = 0UL;
        *buff = 0UL;
        retVal = BCM_ERR_UNINIT;
    }

    return retVal;
}

int32_t HIPC_PlatGetTargetFeature(MCU_VersionType *version, HIPC_TargetFeatureType *features)
{
    int32_t retVal;

    if (features != NULL) {
        *features = HIPC_TARGETFEATURE_SELFREBOOT | HIPC_TARGETFEATURE_RESTRICTEDACCESS;
        retVal = BCM_ERR_OK;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

int32_t HIPC_PlatSendRecvRom(uint32_t id, uint8_t *cmd, uint32_t cmd_len,
            uint8_t *reply, uint32_t reply_len_max, uint32_t *reply_len_act)
{
    int32_t retVal;

    retVal = HIPC_SendRecv(id, cmd, cmd_len, reply, reply_len_max, reply_len_act);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("%s Failed to send command err=%d\n", __func__, retVal);
        goto done;
    }

    if (retVal == BCM_ERR_OK) {
        if (*reply_len_act != sizeof(uint32_t)) {
            HOST_Log("%s recv_len:0x%x\n", __func__, *reply_len_act);
            retVal = BCM_ERR_DATA_INTEG;
        } else if (BCM_ERR_OK != uswap32(*((int32_t *)reply))) {
            HOST_Log("%s status:%d\n", __func__, uswap32(*((int32_t *)reply)));
            retVal = BCM_ERR_UNKNOWN;
        }
    }

done:
    return retVal;
}

