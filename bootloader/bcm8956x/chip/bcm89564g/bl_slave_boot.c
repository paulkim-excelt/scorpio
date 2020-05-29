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
#include <stdint.h>
#include <bl_board.h>
#include <bl_utils.h>
#include <bl_log.h>
#include <bl_bcm_err.h>
#include <bcm_time.h>
#include <mcu.h>
#include <mcu_ext.h>
#include <crc.h>
#include <bl_cntrl.h>
#include <bl_downloader.h>
#include <bl_ipc_downloader.h>
#include <imgl_ipc_cmds.h>
#include <utils.h>
#include <rpc_cmds.h>
#include <ipc.h>
#include <ipc_osil.h>
#include <ipc_hwif.h>

#define BL_GetModuleLogLevel()  (BL_LOG_LVL_INFO)
#define BL_IPC_RESP_TIMEOUT_CNT  (1000UL)
#define CRC32_POLY                       (0x04C11DB7UL)
#define BL_IPC_RESP_TIMEOUT_US   (1UL)

#define BL_STACKINGBITINFO_EN_MASK            (0x1UL)     /**< Stacking enable mask            */
#define BL_STACKINGBITINFO_EN_SHIFT           (0UL)       /**< Stacking enable shift           */
#define BL_STACKINGBITINFO_MST_SLV_MASK       (0x30UL)    /**< Stacking master slave id mask   */
#define BL_STACKINGBITINFO_MST_SLV_SHIFT      (4UL)       /**< Stacking master slave id shift  */
#define BL_STACKINGBITINFO_PORT_0_MASK        (0x780UL)   /**< Stacking Port 0 mask Bit[10:7]  */
#define BL_STACKINGBITINFO_PORT_0_SHIFT       (7UL)       /**< Stacking Port 0 Shift Bit[10:7] */
#define BL_STACKINGBITINFO_PORT_1_MASK        (0x7800UL)  /**< Stacking Port 1 mask Bit[14:11] */
#define BL_STACKINGBITINFO_PORT_1_SHIFT       (11UL)      /**< Stacking Port 1 mask Bit[14:11] */

extern uint8_t BL_DWNLD_StartAddr[];
extern uint8_t bl_bin_size[];

#define MSG_BUF_SIZE            (512UL - 128UL)
#define BL_IMG_SIGNATURE_SIZE   (256UL)

static uint8_t send_msg_buf[MSG_BUF_SIZE];
static uint8_t *snd_msg = send_msg_buf;
static uint8_t recv_msg_buf[MSG_BUF_SIZE];
static uint8_t *recv_msg = recv_msg_buf;

int32_t BL_IpcSendRecvROM(IPC_ChannIDType aID, uint32_t aCmdID, uint8_t * const aCmdBuf, uint32_t aCmdBufLen,
        uint8_t *aRespBuf, uint32_t aRespBufMaxLen, uint32_t *const aRespBufLen)
{
    int32_t retVal = BL_BCM_ERR_OK;
    uint32_t respID = 0;
    uint32_t cnt = BL_IPC_RESP_TIMEOUT_CNT;

    retVal = IPC_Send(aID, aCmdID, aCmdBuf, aCmdBufLen);
    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    do {
        /* Check for response */
        retVal = IPC_Receive(aID, &respID, aRespBuf, aRespBufMaxLen, aRespBufLen);
        if (retVal == BCM_ERR_OK) {
            /* check if it is the desired response */
            if (respID == aCmdID) {
                break;
            }
        }
        BCM_DelayUs(BL_IPC_RESP_TIMEOUT_US);
        cnt--;
    } while (cnt > 0UL);

    if (cnt == 0UL) {
        retVal = BL_BCM_ERR_TIME_OUT;
    }

    if ((*aRespBufLen != sizeof(uint32_t) || (*((uint32_t *)aRespBuf)) != BCM_ERR_OK)) {
        retVal = BCM_ERR_DATA_INTEG;
    }

done:
    return retVal;
}

static int32_t BL_ExecuteBL(IPC_ChannIDType aID, uint8_t* image, uint32_t len)
{
    int32_t retVal;
    uint32_t recv_len;
    uint32_t cmdId;
    uint32_t i, itr, offset, cpy_sz;
    uint32_t len_loc = len;
    uint8_t *image_loc = image;
    BL_DWNLD_ImgHdrType *imgHdrPtr = (BL_DWNLD_ImgHdrType *)snd_msg;

    memset(snd_msg, 0, MSG_BUF_SIZE);
    memset(recv_msg, 0, MSG_BUF_SIZE);

    /* Send Download header with dummy signature */
    cmdId = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);

    imgHdrPtr->magicNum = 0x494D4748UL;
    imgHdrPtr->imgSize = len_loc;
    imgHdrPtr->flags = BL_DWNLD_FLAG_EDC_CRC;
    imgHdrPtr->errDetCode = BCM_CRC32(image_loc, len_loc, CRC32_POLY);

    retVal = BL_IpcSendRecvROM(aID, cmdId, snd_msg,
            sizeof(BL_DWNLD_ImgHdrType) + BL_IMG_SIGNATURE_SIZE, recv_msg,
            MSG_BUF_SIZE, &recv_len);

    if (retVal != BCM_ERR_OK) {
        goto done;
    }

    itr = (len_loc / MSG_BUF_SIZE);
    if (0UL != (len_loc % MSG_BUF_SIZE)) {
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

        retVal = BL_IpcSendRecvROM(aID, cmdId, snd_msg,
                cpy_sz, recv_msg, MSG_BUF_SIZE, &recv_len);
        if (retVal != BCM_ERR_OK) {
            goto done;
        }

        offset += cpy_sz;
    }

done:
    return retVal;
}

void BL_CNTRL_BootSlaves(void)
{
    uint32_t i;
    int32_t ret = BL_BCM_ERR_OK;
    IPC_ChannIDType channID;
    uint32_t stackingInfo = 0;

    for (i = 0; i < IPC_MAX_CHANNELS; i++) {
        if (IPC_CHANN_MODE_MASTER == IPC_ChannCfg[i].mode) {
            break;
        }
    }

    if (IPC_MAX_CHANNELS > i) {
        channID = IPC_ChannCfg[i].ID;
        IPC_Init(channID);

        stackingInfo |= BL_STACKINGBITINFO_EN_MASK;
        stackingInfo |= (MCU_DEVICE_SLAVE_1 << BL_STACKINGBITINFO_MST_SLV_SHIFT);
        stackingInfo |= (0x8UL << BL_STACKINGBITINFO_PORT_0_SHIFT);

        /* write the stacking info */
        ret = IPC_WriteDirect(channID, (uint32_t)MISC_SPARE_SW_REG4, (uint8_t *)&stackingInfo, IPC_ACCESS_WIDTH_16);
        if (BL_BCM_ERR_OK == ret) {
            ret = BL_ExecuteBL(channID, BL_DWNLD_StartAddr, ((uint32_t)bl_bin_size));
            if (BL_BCM_ERR_OK != ret) {
                BL_LOG_ERR("Failed to boot slave: err: %d\n", ret);
            }
        }
    } else {
        BL_LOG_ERR("Invalid IPC configuration\n");
    }
}
