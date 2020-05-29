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

#include <stddef.h>
#include <string.h>
#include <imgl.h>
#include <bcm_err.h>
#include <utils.h>
#include <system.h>
#include <nvm_pt.h>
#include <rpc_loader_osil.h>
#include <rpc_cmds.h>
#include <imgl_ipc_cmds.h>
#include <crc.h>

#include "ee.h"

#pragma pack(push, 4)
/**
    @brief Download image header

    @trace #BRCM_ARCH_RPC_IPC_LOADER_IMG_HDR_TYPE #BRCM_REQ_DWNLD_IMG_HDR
 */
typedef struct {
    uint32_t magicNum;      /**< @brief Magic number [IMGH] */
#define RPC_IPC_LOADER_TAR_MAGIC_NUM     (0x54415248UL)
    uint32_t imgSize;       /**< @brief Size of the image */
    uint32_t flags;         /**< @brief bits[1:0]: edc flag (0 for no edc / 1 for CRC / 2 for checksum)
                                  bit[2]: flash id (0 for internal flash or 1 for external flash)
                                  bits[10:3]: PID (Partition ID)
                                  bits[26:11]: Image ID (used for config image)
                                  bits[31:25]: reserved for future use */
#define RPC_IPC_LOADER_FLAG_EDC_MASK             (0x3UL)
#define RPC_IPC_LOADER_FLAG_EDC_NONE             (0x0UL)
#define RPC_IPC_LOADER_FLAG_EDC_CRC              (0x1UL)
#define RPC_IPC_LOADER_FLAG_EDC_CHECKSUM         (0x2UL)
#define RPC_IPC_LOADER_FLAG_EDC_SHIFT            (0UL)
#define RPC_IPC_LOADER_FLAG_FLASH_ID_MASK        (0x4UL)
#define RPC_IPC_LOADER_FLAG_FLASH_ID_SHIFT       (2UL)
#define RPC_IPC_LOADER_FLAG_PID_MASK             (0x7F8UL)
#define RPC_IPC_LOADER_FLAG_PID_SHIFT            (3UL)
#define RPC_IPC_LOADER_FLAG_IMG_ID_MASK          (0x7FFF800UL)
#define RPC_IPC_LOADER_FLAG_IMG_ID_SHIFT         (11UL)
#define RPC_IPC_LOADER_FLAG_COPY_ID_MASK         (0x18000000UL)
#define RPC_IPC_LOADER_FLAG_COPY_ID_SHIFT        (27UL)

    uint64_t errDetCode;    /**< @brief Error detection code */
} RPC_IpcImgHdrType;

#pragma pack(pop)

/**
    @name RPC_IPC Loader Status
    @{
    @brief RPC_IPC Loader Status
*/
typedef uint32_t RPC_IpcLoaderStatusType;
#define RPC_IPC_LOADER_STATUS_INIT          (0UL)
#define RPC_IPC_LOADER_STATUS_STARTED         (1UL)
#define RPC_IPC_LOADER_STATUS_RUNNING         (2UL)
#define RPC_IPC_LOADER_STATUS_COMPLETED       (3UL)
#define RPC_IPC_LOADER_STATUS_ERROR           (4UL)
#define RPC_IPC_LOADER_STATUS_ACK_PENDING     (5UL)
/** @} */

/**
    @brief Message Type for RPC_IPC Loader Message Queue

    @trace #BRCM_SWREQ_RPC_IL_MSG
*/
typedef struct {
    RPC_IpcLoaderStatusType status;/**< @brief Status of the request */
    uint8_t  pid;               /**< @brief Partition ID */
    uint16_t imgID;             /**< @brief Image ID */
    uint8_t* imgLoadAddrBuf;    /**< @brief Address buffer where image shall be
                                     loaded by RPC_IPC (shall be set to address
                                     passed in #RPC_IpcLoadImg API) */
    uint32_t offset;            /**< @brief offset from where the image need to be loaded */
    uint32_t inLen;             /**< @brief Length of the buffer (shall be set
                                     to length passed in #RPC_IpcLoadImg API) */
    uint32_t imgActualLen;      /**< @brief Actual length of the image (actual
                                     length of the image loaded by RPC_IPC). */
    int32_t result;             /**< @brief Result of the last request */
    uint32_t curOffset;         /**< @brief Current offset */
    uint32_t expSize;           /**< @brief expected size of the image */
    uint32_t prevStatus;
    uint32_t prevCmd;
    uint32_t prevErr;
} RPC_IpcLoaderMsgType;

static void RPC_IpcLoaderReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                  uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RPC_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief RPC_IPC loader messages memory
*/
static RPC_IpcLoaderMsgType RPC_IpcLoaderMsgs[MSGQ_SIZE];

/**
    @brief Get the i-th message from server

    Retrieve the i-th message from server.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]  idx         Index of the message

    @return                 Pointer to the i-th message
    @return                 NULL if idx >= MSGQ_SIZE

    @post None

    @trace #TBD

    @limitations None
*/
void* RPC_IpcLoaderGetMessage(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {
        pRet = (void *)(&RPC_IpcLoaderMsgs[idx]);
    }
    return pRet;
}

/**
    @brief #MSGQ_HdrQType instance of RPC_IPC command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(RPC_IpcLoaderHdrQ);

/**
    @brief Command Message Queue

    The message queue for the events notified to switch. System task
    processes them asynchronously.

    @trace #TBD
*/
MSGQ_DEFINE((RPC_IpcLoaderQ), (IPC_SERVER_TASK), (RPC_LOADER_EVENT),
            (MSGQ_CLIENT_SIGNAL_ENABLE), RPC_IpcLoaderMsgType, (&RPC_IpcLoaderHdrQ),
            (RPC_IpcLoaderGetMessage));


#define RPC_IPC_LOADER_BUFFER_LEN   (384UL)
/**
    @brief Message Type for RPC_IPC Loader Master mode state

    @trace #BRCM_SWREQ_RPC_IL_MSG
*/
typedef struct _RPC_IpcLoaderMasterModeStateType {
    RPC_IpcLoaderStatusType status;        /**< @brief Status of the request       */
    uint8_t              isImgTbl;      /**< @brief Is current transfer Image Table */
    uint8_t              pid;           /**< @brief pid of image table, valid only
                                                when isImgTbl is TRUE           */
    IMGL_LoadStatusType  loaderStatus;  /**< @brief MSGQ Handle                 */
    uint16_t             imgID;         /**< @brief Image ID                    */
    uint8_t img[RPC_IPC_LOADER_BUFFER_LEN]; /**< @brief buffer where image shall
                                                be loaded by RPC_IPC                */
    uint32_t             offset;        /**< @brief current offset from where
                                                the image need to be loaded     */
    uint32_t             imgLen;        /**< @brief Actual length of the image  */
    uint32_t             dataPending;   /**< @brief Pending data transmission   */
    RPC_IpcLoaderStatusType prevStatus;    /**< @brief Unused                      */
    uint32_t             prevCmd;       /**< @brief Unused                      */
    uint32_t             prevErr;       /**< @brief Unused                      */
} RPC_IpcLoaderMasterModeStateType;

static RPC_IpcLoaderMasterModeStateType RPC_IpcLoaderMasterModeState[RPC_MAX_CHANNELS];


static void RPC_IpcLoaderMasterModeStateReset(RPC_IpcLoaderMasterModeStateType *aState)
{
    aState->status = RPC_IPC_LOADER_STATUS_INIT;
    aState->isImgTbl = FALSE;
    aState->loaderStatus.hdr = NULL;
    aState->pid = 0U;
    aState->imgID = 0U;
    aState->offset = 0UL;
    aState->imgLen = 0UL;
    aState->dataPending = 0UL;
    aState->prevStatus = RPC_IPC_LOADER_STATUS_INIT;
    aState->prevCmd = 0UL;
    aState->prevErr = 0UL;
}

/**
*/
void RPC_IpcLoaderInit(void)
{
    uint32_t i = 0UL;
    for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
        RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[i]);
    }
}

static void RPC_IpcLoaderSendImgTbl(RPC_LoaderRpcMsgType *aMsg)
{
    const ITBL_Type * imageTbl = NULL;
    uint32_t payLoadSize = 0UL;
    uint32_t cmd;
    int32_t retVal;
    uint32_t offset = RPC_IpcLoaderMasterModeState[aMsg->id].offset;
    if (PTBL_ID_SYSCFG == RPC_IpcLoaderMasterModeState[aMsg->id].pid) {
        imageTbl = IMGL_GetImgTbl(PTBL_ID_SYSCFG);;
    } else if (PTBL_ID_FW == RPC_IpcLoaderMasterModeState[aMsg->id].pid) {
        imageTbl = IMGL_GetImgTbl(PTBL_ID_FW);;
    } else {
    }
    if ((NULL != imageTbl) && (offset < sizeof(ITBL_Type))) {
        uint8_t *buffer = (uint8_t*)imageTbl;
        buffer += offset;
        payLoadSize = sizeof(ITBL_Type) - offset;
        if (payLoadSize > RPC_IPC_LOADER_BUFFER_LEN) {
            payLoadSize = RPC_IPC_LOADER_BUFFER_LEN;
        }
        cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
        retVal = RPC_Send(aMsg->id, cmd, buffer, payLoadSize);
        if (BCM_ERR_OK == retVal) {
            RPC_IpcLoaderMasterModeState[aMsg->id].offset += payLoadSize;
        } else {
            RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                    (uint32_t)__LINE__, RPC_IpcLoaderMasterModeState[aMsg->id].offset, aMsg->cmd, 0UL);
        }
    } else if (offset == sizeof(ITBL_Type)) {
        /* Transfer complete. Reset the state*/
        RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[aMsg->id]);
    }
}

/**
*/
void RPC_IpcLoaderMasterModeMsgHandler(RPC_LoaderRpcMsgType *aMsg)
{
    uint32_t idx = 0UL;
    int32_t  retVal = BCM_ERR_OK;
    RPC_IpcLoaderMsgType *msg = NULL;
    uint32_t i = 0UL;
    uint32_t cmd;
    int32_t cmdRespStatus;
    const ITBL_Type * imageTbl = NULL;
    RPC_IpcImgHdrType dwnldImgHdr;

    retVal = MSGQ_GetMsgIdx(&RPC_IpcLoaderQ, &idx);
    if (BCM_ERR_OK == retVal) {
        msg = &RPC_IpcLoaderMsgs[idx];
        (void)msg;
    }

    if ((NULL != aMsg) && (NULL != aMsg->payLoad)) {
        cmdRespStatus = *((uint32_t *)aMsg->payLoad);
        if (IMGL_CMDID_DWNLD_IMG_HDR == RPC_GET_CMDID(aMsg->cmd)) {
            if ((4UL == aMsg->payLoadSize) && (BCM_ERR_OK == cmdRespStatus)) {
                /* Download header response - Master Mode */
                if (RPC_IpcLoaderMasterModeState[aMsg->id].isImgTbl == TRUE) {
                    RPC_IpcLoaderSendImgTbl(aMsg);
                }
            } else {
                /* Unknown format */
                RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                           (uint32_t)__LINE__, aMsg->payLoadSize, 0UL, 0UL);
                /* report error and reset the master mode state machine */
                RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[i]);
            }
        } else if (IMGL_CMDID_DWNLD_IMG == RPC_GET_CMDID(aMsg->cmd)) {
            if ((4UL == aMsg->payLoadSize) && (BCM_ERR_OK == cmdRespStatus)) {
                /* Download data response - Master Mode */
                i = aMsg->id;
                if (RPC_IpcLoaderMasterModeState[aMsg->id].isImgTbl == TRUE) {
                    RPC_IpcLoaderSendImgTbl(aMsg);
                } else {
                    if (RPC_IpcLoaderMasterModeState[i].dataPending == 1UL) {
                        cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
                        retVal = RPC_Send(i, cmd, RPC_IpcLoaderMasterModeState[i].img,
                                RPC_IpcLoaderMasterModeState[i].loaderStatus.size);
                        if (BCM_ERR_OK == retVal) {
                            RPC_IpcLoaderMasterModeState[i].offset += RPC_IpcLoaderMasterModeState[i].loaderStatus.size;
                            if (RPC_IpcLoaderMasterModeState[i].offset == RPC_IpcLoaderMasterModeState[i].imgLen) {
                                RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[i]);
                            } else {
                                retVal = IMGL_LoadImg(RPC_IpcLoaderMasterModeState[i].imgID,
                                        RPC_IpcLoaderMasterModeState[i].img,
                                        RPC_IpcLoaderMasterModeState[i].offset,
                                        RPC_IPC_LOADER_BUFFER_LEN,
                                        RPC_LOADER_EVENT,
                                        &RPC_IpcLoaderMasterModeState[i].loaderStatus);
                            }
                            RPC_IpcLoaderMasterModeState[i].dataPending = 0UL;
                        } else {
                            /* Retry sending the packet later */
                        }
                    }
                }
            } else {
                /* Unknown format
                   Todo: decide to send a response or ignore based on mode? */
                RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                           (uint32_t)__LINE__, aMsg->payLoadSize, 0UL, 0UL);
                RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[i]);
            }
        } else if (IMGL_CMDID_IMAGE_FETCH == RPC_GET_CMDID(aMsg->cmd)) {
            IMGL_ImgHdrType *imglHdr = (IMGL_ImgHdrType *)aMsg->payLoad;
            RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[aMsg->id]);
            if (PTBL_ID_SYSCFG == imglHdr->pid) {
                imageTbl = IMGL_GetImgTbl(PTBL_ID_SYSCFG);
            } else if (PTBL_ID_FW == imglHdr->pid) {
                imageTbl = IMGL_GetImgTbl(PTBL_ID_FW);;
            } else {
            }
            if (NULL != imageTbl) {
                if (imglHdr->isImgHdr == TRUE) {
                    /* supply the image table to slave */
                    dwnldImgHdr.magicNum = ITBL_MAGIC;
                    dwnldImgHdr.imgSize = sizeof(ITBL_Type);
                    dwnldImgHdr.errDetCode = BCM_CRC32((uint8_t *)imageTbl, sizeof(ITBL_Type), PTBL_CRC32_POLY);
                    dwnldImgHdr.flags = PTBL_ERR_DET_CRC;
                    RPC_IpcLoaderMasterModeState[aMsg->id].status = RPC_IPC_LOADER_STATUS_STARTED;
                    RPC_IpcLoaderMasterModeState[aMsg->id].isImgTbl= TRUE;
                    RPC_IpcLoaderMasterModeState[aMsg->id].pid = imglHdr->pid;
                    cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);
                    retVal = RPC_Send(aMsg->id, cmd, (uint8_t *)&dwnldImgHdr, sizeof(RPC_IpcImgHdrType));
                    if (BCM_ERR_OK == retVal) {
                    } else {
                        RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                                (uint32_t)__LINE__, aMsg->payLoadSize, aMsg->cmd, 0UL);
                    }
                } else {
                    /* supply the image to the slave */
                    for (i = 0UL; i < imageTbl->hdr.numImgs; i++) {
                        if (imglHdr->imgID == imageTbl->entry[i].imgType) {
                            dwnldImgHdr.magicNum = ITBL_MAGIC;
                            dwnldImgHdr.imgSize = imageTbl->entry[i].actualSize;
                            dwnldImgHdr.errDetCode = imageTbl->entry[i].errDetCode;
                            dwnldImgHdr.flags = imageTbl->entry[i].flags;
                            RPC_IpcLoaderMasterModeState[aMsg->id].status = RPC_IPC_LOADER_STATUS_STARTED;
                            RPC_IpcLoaderMasterModeState[aMsg->id].imgID = imglHdr->imgID;
                            RPC_IpcLoaderMasterModeState[aMsg->id].imgLen = dwnldImgHdr.imgSize;
                            cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);
                            retVal = RPC_Send(aMsg->id, cmd, (uint8_t *)&dwnldImgHdr, sizeof(RPC_IpcImgHdrType));
                            if (BCM_ERR_OK == retVal) {
                                retVal = IMGL_LoadImg(RPC_IpcLoaderMasterModeState[aMsg->id].imgID,
                                        RPC_IpcLoaderMasterModeState[aMsg->id].img,
                                        RPC_IpcLoaderMasterModeState[aMsg->id].offset,
                                        RPC_IPC_LOADER_BUFFER_LEN,
                                        RPC_LOADER_EVENT,
                                        &RPC_IpcLoaderMasterModeState[aMsg->id].loaderStatus);
                                if (BCM_ERR_OK == retVal) {
                                    RPC_IpcLoaderMasterModeState[aMsg->id].status = RPC_IPC_LOADER_STATUS_RUNNING;
                                }
                            }
                            break;
                        }
                    }
                    if (i == imageTbl->hdr.numImgs) {
                        dwnldImgHdr.magicNum = ITBL_MAGIC;
                        dwnldImgHdr.imgSize = 0UL;
                        dwnldImgHdr.errDetCode = 0UL;
                        dwnldImgHdr.flags = 0UL;
                        cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);
                        retVal = RPC_Send(aMsg->id, cmd, (uint8_t *)&dwnldImgHdr, sizeof(RPC_IpcImgHdrType));
                        if (BCM_ERR_OK != retVal) {
                            RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                                                (uint32_t)__LINE__, imglHdr->imgID, aMsg->cmd, 0UL);
                        }
                    }
                }
            /* Todo: Prepare to send a request to IMGL(PTM) to get data and send to Slave device */
            } else {
                dwnldImgHdr.magicNum = ITBL_MAGIC;
                dwnldImgHdr.imgSize = 0UL;
                dwnldImgHdr.errDetCode = 0UL;
                dwnldImgHdr.flags = 0UL;
                cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG_HDR);
                retVal = RPC_Send(aMsg->id, cmd, (uint8_t *)&dwnldImgHdr, sizeof(RPC_IpcImgHdrType));
                if (BCM_ERR_OK != retVal) {
                    RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                                        (uint32_t)__LINE__, imglHdr->imgID, aMsg->cmd, 0UL);
                }
            }
        } else {
            /*  Unknown response
                Todo: decide to send a response or ignore based on mode? */
            RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                       (uint32_t)__LINE__, aMsg->payLoadSize, aMsg->cmd, 0UL);
        }
    }
}


/**
*/
void RPC_IpcLoaderSlaveModeMsgHandler(RPC_LoaderRpcMsgType *aMsg)
{
    uint32_t idx = 0UL;
    int32_t  retVal = BCM_ERR_OK;
    RPC_IpcLoaderMsgType *msg = NULL;
    int32_t lErr = BCM_ERR_OK;

    retVal = MSGQ_GetMsgIdx(&RPC_IpcLoaderQ, &idx);
    if (BCM_ERR_OK == retVal) {
        msg = &RPC_IpcLoaderMsgs[idx];
    }

    if ((NULL != aMsg) && (NULL != aMsg->payLoad)) {
        if (IMGL_CMDID_DWNLD_IMG_HDR == RPC_GET_CMDID(aMsg->cmd)) {
            if ((sizeof(RPC_IpcImgHdrType) == aMsg->payLoadSize)
                && (NULL != msg) && (RPC_IPC_LOADER_STATUS_STARTED == msg->status)) {
                /* Download header command - Slave Mode
                   Validate img hdr */
                RPC_IpcImgHdrType *imgHdr = (RPC_IpcImgHdrType *)aMsg->payLoad;
                if (ITBL_MAGIC != imgHdr->magicNum) {
                    lErr = BCM_ERR_INVAL_MAGIC;
                } else if (imgHdr->imgSize > msg->inLen) {
                    lErr = BCM_ERR_NOMEM;
                } else {
                    msg->curOffset = 0UL;
                    msg->expSize = imgHdr->imgSize;
                    lErr = BCM_ERR_OK;
                }
                retVal = RPC_Send(aMsg->id, aMsg->cmd, (uint8_t *)&lErr, sizeof(lErr));
                if (BCM_ERR_OK == retVal) {
                    msg->status = RPC_IPC_LOADER_STATUS_RUNNING;
                } else {
                    /*Todo: preserve the response and send later */
                }
                if (0UL == msg->expSize) {
                    msg->status = RPC_IPC_LOADER_STATUS_ERROR;
                    msg->result = BCM_ERR_UNKNOWN;
                    msg->imgActualLen = 0UL;
                    retVal = MSGQ_CompleteMsgIdx(&RPC_IpcLoaderQ, idx);
                    if (BCM_ERR_OK != retVal) {
                        /* Todo: Decide what action to be taken */
                    }
                }
            } else {
                /* Unknown format
                   Todo: decide to send a response or ignore based on mode? */
                RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                           (uint32_t)__LINE__, aMsg->payLoadSize, aMsg->cmd, 0UL);
            }
        } else if (IMGL_CMDID_DWNLD_IMG == RPC_GET_CMDID(aMsg->cmd)) {
            if ((NULL != msg) && (RPC_IPC_LOADER_STATUS_RUNNING == msg->status)) {
                /* Download data command - Slave Mode:  copy the data */
                if ((msg->curOffset + aMsg->payLoadSize) > msg->inLen) {
                    lErr = BCM_ERR_NOMEM;
                } else {
                    BCM_MemCpy(msg->imgLoadAddrBuf + msg->curOffset, aMsg->payLoad, aMsg->payLoadSize);
                    msg->curOffset += aMsg->payLoadSize;
                    if (msg->curOffset == msg->expSize) {
                        msg->status = RPC_IPC_LOADER_STATUS_COMPLETED;
                        msg->result = BCM_ERR_OK;
                        msg->imgActualLen = msg->curOffset;
                        lErr = BCM_ERR_OK;
                    } else if (msg->curOffset > msg->expSize) {
                        msg->status = RPC_IPC_LOADER_STATUS_ERROR;
                        msg->result = BCM_ERR_UNKNOWN;
                        msg->imgActualLen = 0UL;
                        lErr = BCM_ERR_UNKNOWN;
                    } else {
                        lErr = BCM_ERR_OK;
                    }
                }
                retVal = RPC_Send(aMsg->id, aMsg->cmd, (uint8_t *)&lErr, sizeof(lErr));
                if ((BCM_ERR_OK == retVal) && (RPC_IPC_LOADER_STATUS_RUNNING != msg->status)) {
                    retVal = MSGQ_CompleteMsgIdx(&RPC_IpcLoaderQ, idx);
                    if (BCM_ERR_OK != retVal) {
                        /* Todo: Decide what action to be taken */
                    }
                } else {
                    /*Todo: preserve the response and send later */
                }
            } else {
                /* Unknown format
                   Todo: decide to send a response or ignore based on mode? */
                RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                           (uint32_t)__LINE__, aMsg->payLoadSize, aMsg->cmd, 0UL);
            }
        } else {
            /*  Unknown command
                Todo: decide to send a response or ignore based on mode? */
            lErr = BCM_ERR_UNKNOWN;
            retVal = RPC_Send(aMsg->id, aMsg->cmd, (uint8_t *)&lErr, sizeof(lErr));
            RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOADER_PROCESS_PROC, BCM_ERR_UNKNOWN,
                       (uint32_t)__LINE__, aMsg->payLoadSize, aMsg->cmd, retVal);
        }
    }
}

/**
*/
void RPC_IpcLoaderEventHandler(void)
{
    uint32_t idx = 0UL;
    int32_t  retVal = BCM_ERR_OK;
    uint32_t i;
    uint32_t cmd;

    retVal = MSGQ_GetMsgIdx(&RPC_IpcLoaderQ, &idx);
    if (BCM_ERR_OK == retVal) {
        RPC_IpcLoaderMsgType *msg;
        msg = &RPC_IpcLoaderMsgs[idx];
        if (RPC_IPC_LOADER_STATUS_INIT == msg->status) {
            uint32_t imgFetchRpcCmd = RPC_ASYNCID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_IMAGE_FETCH);
            IMGL_ImgHdrType imgHdr;
            imgHdr.isImgHdr = FALSE;
            imgHdr.pid = msg->pid;
            imgHdr.imgID = msg->imgID;
            imgHdr.imgSize = msg->inLen;
            retVal = RPC_Send(0UL, imgFetchRpcCmd, (uint8_t*)&imgHdr, sizeof(IMGL_ImgHdrType));
            if (BCM_ERR_OK == retVal) {
                msg->status = RPC_IPC_LOADER_STATUS_STARTED;
            }
        }
    }

    for (i = 0UL; i < RPC_MAX_CHANNELS; i++) {
        if (RPC_IPC_LOADER_STATUS_RUNNING == RPC_IpcLoaderMasterModeState[i].status) {
            if (NULL != RPC_IpcLoaderMasterModeState[i].loaderStatus.hdr) {
                retVal = IMGL_GetStatus(&RPC_IpcLoaderMasterModeState[i].loaderStatus);
                if (BCM_ERR_NOMEM == retVal) {
                    cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
                    retVal = RPC_Send(i, cmd, RPC_IpcLoaderMasterModeState[i].img,
                                            RPC_IpcLoaderMasterModeState[i].loaderStatus.size);
                    if (BCM_ERR_OK == retVal) {
                        RPC_IpcLoaderMasterModeState[i].offset += RPC_IpcLoaderMasterModeState[i].loaderStatus.size;
                        retVal = IMGL_LoadImg(RPC_IpcLoaderMasterModeState[i].imgID,
                                                RPC_IpcLoaderMasterModeState[i].img,
                                                RPC_IpcLoaderMasterModeState[i].offset,
                                                RPC_IPC_LOADER_BUFFER_LEN,
                                                RPC_LOADER_EVENT,
                                                &RPC_IpcLoaderMasterModeState[i].loaderStatus);
                    } else {
                        /* Retry sending the packet later */
                        RPC_IpcLoaderMasterModeState[i].dataPending = 1UL;
                    }
                } else if (BCM_ERR_OK == retVal) {
                    /* Last data.. Handle it, No more request to IMGL */
                    cmd = RPC_CMDID(BCM_GROUPID_IMGL, BCM_RSV_ID, IMGL_CMDID_DWNLD_IMG);
                    retVal = RPC_Send(i, cmd, RPC_IpcLoaderMasterModeState[i].img,
                                            RPC_IpcLoaderMasterModeState[i].loaderStatus.size);
                    if (BCM_ERR_OK == retVal) {
                        /* Transfer complete.. Reset state */
                        RPC_IpcLoaderMasterModeStateReset(&RPC_IpcLoaderMasterModeState[i]);
                    } else {
                        /* Error sending. Retry sending later */
                        RPC_IpcLoaderMasterModeState[i].dataPending = 1UL;
                    }
                } else if (BCM_ERR_BUSY != retVal) {
                    /* Do nothing.. IMGL is busy */
                } else {
                    /* Error case.. What to do? */
                }
            }
        }
    }

}

/**
*/
void RPC_IpcLoaderDeInit(void)
{
}


/** @} */

/**
    @addtogroup grp_rpc_server_async
    @{
*/

/**
    Detailed design of COMP_Function
    <BR>Insert flowchart or pseudocode based on the complexity of the function.
    Any function which may spread beyond 100 lines of code shall have a flowchart.
    Others can be done with a pseudocode depending on the complexity
    @image html comp_flowchart.jpg "Component Flowchart"

    @trace #BRCM_SWARCH_COMP_FUNCTION_PROC
    @trace #BRCM_SWREQ_COMP_FUNCTIONALITY

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/

static int32_t RPC_IpcLoadImg(uint8_t aPID,
                    uint16_t aImgID,
                    uint8_t* const aAddr,
                    uint32_t aOffset,
                    uint32_t aBufSize,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    int32_t err = BCM_ERR_OK;
    RPC_IpcLoaderMsgType msg;

    if ((NULL == aAddr) || (0UL == aBufSize) || (NULL == aHdr)) {
        err = BCM_ERR_INVAL_PARAMS;
    } else {
        msg.pid =  aPID;
        msg.imgID = aImgID;
        msg.imgLoadAddrBuf = aAddr;
        msg.offset = aOffset;
        msg.inLen = aBufSize;
        msg.result = BCM_ERR_UNKNOWN;
        msg.status = RPC_IPC_LOADER_STATUS_INIT;
        msg.curOffset = 0UL;
        err = MSGQ_SendMsg(&RPC_IpcLoaderQ, &msg, aClientMask, aHdr);
        if ((BCM_ERR_OK == err) && (NULL == *aHdr)) {
            RPC_IpcLoaderReportError(BRCM_SWARCH_RPC_IPC_LOAD_IMG_PROC, err, (uint32_t)__LINE__, (uint32_t) msg.imgID,
                            (uint32_t) msg.imgLoadAddrBuf, (uint32_t) msg.inLen);
        }
    }

    return err;
}


static int32_t RPC_IpcGetLoaderStatus(const MSGQ_MsgHdrType *const aHdr,
                             uint32_t *const aImgSize)
{
    int32_t err;
    RPC_IpcLoaderMsgType msg;
    int32_t status;

    err = MSGQ_RecvMsg(&RPC_IpcLoaderQ, aHdr, &msg);
    if (BCM_ERR_OK == err) {
        *aImgSize = msg.imgActualLen;
        status = msg.result;
    } else {
        status = BCM_ERR_BUSY;
    }

    return status;
}

const IMGL_ModeHandlerType RPC_IpcImageHandler = {
    .mode = IMGL_LOAD_MODE_IPC,
    .getImgTbl = NULL,
    .loadImg = RPC_IpcLoadImg,
    .getStatus = RPC_IpcGetLoaderStatus,
};
