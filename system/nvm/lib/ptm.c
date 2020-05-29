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
#include <ptm.h>
#include <mcu.h>
#include <bcm_err.h>
#include <utils.h>
#include <osil/ptm_osil.h>
#include <flsmgr.h>
#include <chip_config.h>
#include <system.h>
#include <cache.h>
#include <nvm_pt.h>
#include <crc.h>

#include <ulog.h>
#define GetModuleLogLevel()     (ULOG_LVL_INFO)

/* Internal interface ID */
#define PTM_INIT_STATE_HANDLER_API_ID       (0x80U)

#define IMG_TABLE_SIZE                      (2UL * 1024UL) /* Max 55 Image entries can be cached */

#define PTM_OP_STATE_RESET                  (0UL)
#define PTM_OP_STATE_PTBL_REQ               (1UL)
#define PTM_OP_STATE_SYS_CONFIG_IMG_REQ     (2UL)
#define PTM_OP_STATE_CONFIG_IMG_REQ         (3UL)
#define PTM_OP_STATE_USER_IMG_REQ           (4UL)

#define PTM_USER_DATA_MAX_SIZE  (4096UL)
/** PTM PT header type  */
typedef PTBL_HdrType PTM_PTHdrType;

/** PTM PT entry type */
typedef PTBL_EntryType PTM_PTEntryType;

/** PTM Image header type */
typedef ITBL_HdrType PTM_ImgHdrType;

/** PTM Image entry type */
typedef ITBL_EntryType PTM_ImgEntryType;

typedef struct _PTM_PTable {
    PTBL_HdrType hdr;
    PTBL_EntryType entry[11UL];
} PTM_PTable;

typedef struct {
    PTM_LookupTblEntType lookupTbl[FLASH_MAX_HW_ID];
    uint32_t lookupTblSize;
    /** Index of lookupTbl entry for valid PT.
     * The lookupTbl entry specifies flash ID & start address of
     * PT copies in the flash. All the PT copies are assumed to
     * placed consecutive in flash */
    uint32_t lookupIdx;
    /** PT copy index in flash(specified by lookupTbl entry) for valid PT. */
    uint32_t ptCopyIdx;
    uint32_t cfgPartitionIdx;
    uint32_t cfgImgCnt;
    PTM_StateType state;
    uint32_t imgHdrAddr;
    uint32_t opState;
    PTM_PTable pTable[PTBL_NUM_COPIES];
    ITBL_Type fwCfgImage;
    uint32_t fwCfgImageAddr;
    ITBL_Type sysCfgImage;
    uint32_t sysCfgImageAddr;
    ITBL_Type userImage;
    uint32_t userImageAddr;
    /** This entry is corresponding to ImgID passed by the client */
    const MSGQ_MsgHdrType * msgHdr0;
    const MSGQ_MsgHdrType * msgHdr1;
    PTM_MsgType * currMsg;
} PTM_CompType;

static PTM_CompType COMP_SECTION(".data.ptm") PTM_Comp =
{
    .state = PTM_STATE_UNINIT,
    .opState = PTM_OP_STATE_RESET,
    .lookupIdx = 0UL,
    .ptCopyIdx = 0UL,
    .cfgImgCnt = 0UL,
    .cfgPartitionIdx = 0UL,
    .msgHdr0 = NULL,
    .msgHdr1 = NULL,
    .currMsg = NULL,
};


typedef struct _PTM_UserDataHdr {
    uint32_t magic;
#define PTM_USER_DATA_MAGIC (0x55535244UL)
    uint32_t size;
    uint64_t errDetCode;
    uint32_t reserved[4UL];
} PTM_UserDataHdr;

typedef struct _PTM_UserDataImg {
    PTM_UserDataHdr hdr;
    uint8_t data[PTM_USER_DATA_MAX_SIZE - sizeof(PTM_UserDataHdr)];
} PTM_UserDataImg;

static PTM_UserDataImg COMP_SECTION(".data.ptm") PTM_UserData;

static void PTM_ReportError(uint8_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
        uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_PTM_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

static int32_t ValidateCRC(const uint8_t *const aAddr,
                           uint32_t aLen,
                           uint64_t aCRC)
{
    int32_t err;

    uint32_t crc = BCM_CRC32(aAddr, aLen, PTBL_CRC32_POLY);
    if (crc != aCRC) {
        err = BCM_ERR_DATA_INTEG;
    } else {
        err = BCM_ERR_OK;
    }

    return err;
}

static int32_t ValidateChksum(uint8_t *const aAddr,
                              uint32_t aLen,
                              uint64_t aChcksm)
{
    int32_t err;
    uint32_t i;
    uint32_t remain;
    uint32_t count;
    uint8_t *addr = aAddr;
    uint32_t *temp = (uint32_t *)aAddr;
    uint64_t sum = 0ULL;

    count = aLen / sizeof(uint32_t);
    remain = aLen & 0x3UL;
    for (i = 0UL; i < count; i++) {
        sum += *(temp++);
    }
    addr = &addr[count * sizeof(uint32_t)];
    if (remain != 0UL) {
        for(i = 0UL; i < remain; i++) {
            sum += *(addr++);
        }
    }
    if (~sum != aChcksm) {
        err = BCM_ERR_DATA_INTEG;
    } else {
        err = BCM_ERR_OK;
    }

    return err;
}

static int32_t PTM_ValidatePT(PTM_PTable *const aPTable)
{
    int32_t err;
    uint32_t edcFlag;

    if (PTBL_MAGIC != aPTable->hdr.magicNumber) {
        err = BCM_ERR_INVAL_MAGIC;
    } else {
        edcFlag = ((aPTable->hdr.flags & PTBL_ERR_DET_MASK) >>
                PTBL_ERR_DET_SHIFT);
        if (edcFlag == PTBL_ERR_DET_CRC) {
            err = ValidateCRC(((uint8_t*)aPTable) + PTBL_HDR_SIZE,
                    (aPTable->hdr.numEntries * PTBL_ENTRY_SIZE),
                    aPTable->hdr.errDetCode);
        } else if (edcFlag == PTBL_ERR_DET_CHECKSUM) {
            err = ValidateChksum(((uint8_t*)aPTable) + PTBL_HDR_SIZE,
                    (aPTable->hdr.numEntries * PTBL_ENTRY_SIZE),
                    aPTable->hdr.errDetCode);
        } else {
            err = BCM_ERR_OK;
        }
    }

    return err;
}

static int32_t PTM_ValidateImgPartHdr(ITBL_Type *const aPartHdr)
{
    int32_t err;
    uint32_t edcFlag;

    if (ITBL_MAGIC != aPartHdr->hdr.magicNumber) {
        err = BCM_ERR_INVAL_MAGIC;
    } else {
        edcFlag = ((aPartHdr->hdr.flags & ITBL_ERR_DET_HDR_MASK) >>
                ITBL_ERR_DET_HDR_SHIFT);
        if (edcFlag == PTBL_ERR_DET_CRC) {
            err = ValidateCRC((uint8_t*)aPartHdr->entry,
                    (aPartHdr->hdr.numImgs * ITBL_ENTRY_SIZE),
                    aPartHdr->hdr.errDetCode);
        } else if (edcFlag == PTBL_ERR_DET_CHECKSUM) {
            err = ValidateChksum((uint8_t*)aPartHdr->entry,
                    (aPartHdr->hdr.numImgs * ITBL_ENTRY_SIZE),
                    aPartHdr->hdr.errDetCode);
        } else {
            err = BCM_ERR_OK;
        }
    }

    return err;
}

static PTM_ImgEntryType * getImageEntry(ITBL_Type *aPartHdr,
                                        PTM_ImgIDType aId)
{
    uint32_t i;
    PTM_ImgEntryType *retVal = NULL;
    PTM_ImgEntryType *imgEntry = aPartHdr->entry;

    for (i = 0UL; i < aPartHdr->hdr.numImgs; i++) {
        if (imgEntry->imgType == aId) {
            retVal = imgEntry;
            break;
        }
        imgEntry++;
    }

    return retVal;
}

static int32_t PTM_ValidateImg(PTM_ImgEntryType *aImgEntry, PTM_MsgType *const aMsg)
{
    int32_t err;
    uint32_t edcFlag;

    edcFlag = ((aImgEntry->flags & ITBL_ERR_DET_ENTRY_MASK) >>
            ITBL_ERR_DET_ENTRY_SHIFT);
    if (edcFlag == PTBL_ERR_DET_CRC) {
        err = ValidateCRC(aMsg->imgLoadAddrBuf, aMsg->imgActualLen,
                aImgEntry->errDetCode);
    } else if (edcFlag == PTBL_ERR_DET_CHECKSUM) {
        err = ValidateChksum(aMsg->imgLoadAddrBuf, aMsg->imgActualLen,
                aImgEntry->errDetCode);
    } else {
        err = BCM_ERR_OK;
    }

    return err;
}

static int32_t PTM_ValidateUserData(PTM_MsgType *const aMsg)
{
    int32_t err;

    if (PTM_USER_DATA_MAGIC == PTM_UserData.hdr.magic) {
        if (PTM_UserData.hdr.size < PTM_USER_DATA_MAX_SIZE - sizeof(PTM_UserDataHdr)) {
            err = ValidateCRC(PTM_UserData.data, PTM_UserData.hdr.size,
                        PTM_UserData.hdr.errDetCode);
            if (BCM_ERR_OK == err) {
                if (aMsg->inLen < PTM_UserData.hdr.size) {
                    aMsg->result = BCM_ERR_NOMEM;
                    err = BCM_ERR_NOMEM;
                } else if (0UL == PTM_UserData.hdr.size) {
                    aMsg->result = BCM_ERR_NOT_FOUND;
                    err = BCM_ERR_NOT_FOUND;
                } else {
                    err = BCM_ERR_OK;
                    aMsg->imgActualLen = PTM_UserData.hdr.size;
                    BCM_MemCpy(aMsg->imgLoadAddrBuf, PTM_UserData.data, PTM_UserData.hdr.size);
                }
            }
        } else {
            aMsg->result = BCM_ERR_NOMEM;
            err = BCM_ERR_NOMEM;
        }
    } else {
        err = BCM_ERR_INVAL_MAGIC;
    }

    return err;
}

static int32_t PTM_ReadPTable(void)
{
    int32_t err;
    uint32_t flashAddr;
    PTM_FlashIDType flashID;

    flashAddr = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashAddr;
    flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
    err = PTM_ReadFlash(flashID, flashAddr, sizeof(PTM_PTable) * PTBL_NUM_COPIES,
                (uint8_t*)PTM_Comp.pTable, &PTM_Comp.msgHdr0);

    return err;
}

static int32_t PTM_ReadImgPartHdr(uint16_t aPid, uint8_t* aImgPartHdr)
{
    uint32_t i;
    int32_t err = BCM_ERR_OK;
    PTM_FlashIDType flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;;
    for (i = 0UL; i < PTM_Comp.pTable[PTM_Comp.ptCopyIdx].hdr.numEntries; i++) {
        if (PTM_Comp.pTable[PTM_Comp.ptCopyIdx].entry[i].pid == aPid) {
            err = PTM_ReadFlash(flashID, PTM_Comp.pTable[PTM_Comp.ptCopyIdx].entry[i].addr,
                    sizeof(ITBL_Type), aImgPartHdr, &PTM_Comp.msgHdr0);
            if (BCM_ERR_OK == err) {
                if ((aPid & PTBL_ID_TYPE_MASK) == PTBL_ID_USER_DATA) {
                    PTM_Comp.userImageAddr = PTM_Comp.pTable[PTM_Comp.ptCopyIdx].entry[i].addr;
                } else if ((aPid & PTBL_ID_TYPE_MASK) == PTBL_ID_SYSCFG) {
                    PTM_Comp.sysCfgImageAddr = PTM_Comp.pTable[PTM_Comp.ptCopyIdx].entry[i].addr;
                } else {
                    PTM_Comp.fwCfgImageAddr = PTM_Comp.pTable[PTM_Comp.ptCopyIdx].entry[i].addr;
                }
                break;
            }
        }
    }
    if (PTM_Comp.pTable[PTM_Comp.ptCopyIdx].hdr.numEntries == i) {
        err = BCM_ERR_NODEV;
    }

    return err;
}

static int32_t PTM_ReadImg(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_OK;
    uint32_t flashOffset;
    uint32_t imgSize;
    uint32_t addr;
    PTM_ImgEntryType *imgEntry = NULL;
    PTM_FlashIDType flashID;

    flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
    if (NULL != aMsg) {
        if (PTM_CMD_LOAD_IMG == aMsg->cmd) {
            if (PTBL_ID_SYSCFG == aMsg->pid) {
                imgEntry = getImageEntry(&PTM_Comp.sysCfgImage, aMsg->imgID);
                addr = PTM_Comp.sysCfgImageAddr;
            } else if (PTBL_ID_FW == aMsg->pid) {
                imgEntry = getImageEntry(&PTM_Comp.fwCfgImage, aMsg->imgID);
                addr = PTM_Comp.fwCfgImageAddr;
            } else {
                /* Unhandled partition */
            }
            if (NULL != imgEntry) {
                flashOffset = imgEntry->flashOffset;
                imgSize = imgEntry->actualSize;
                if (aMsg->offset < imgSize) {
                    flashOffset += aMsg->offset;
                    imgSize -= aMsg->offset;
                }
                if (aMsg->inLen < imgSize) {
                    aMsg->result = BCM_ERR_NOMEM;
                    aMsg->imgActualLen = aMsg->inLen;
                    err = PTM_ReadFlash(flashID,
                            addr + flashOffset,
                            aMsg->inLen,
                            aMsg->imgLoadAddrBuf,
                            &PTM_Comp.msgHdr0);
                } else if (0UL == imgSize) {
                    aMsg->result = BCM_ERR_NOT_FOUND;
                    err = BCM_ERR_NOT_FOUND;
                } else {
                    aMsg->result = BCM_ERR_BUSY;
                    aMsg->imgActualLen = imgSize;
                    err = PTM_ReadFlash(flashID,
                            addr + flashOffset,
                            imgSize,
                            aMsg->imgLoadAddrBuf,
                            &PTM_Comp.msgHdr0);
                }
            } else {
                aMsg->result = BCM_ERR_NOT_FOUND;
                err = BCM_ERR_NODEV;
            }
        }
        if (PTM_CMD_LOAD_USERDATA == aMsg->cmd) {
            PTM_ImgEntryType *imgEntry = getImageEntry(&PTM_Comp.userImage, aMsg->imgID);
            if (NULL != imgEntry) {
                flashOffset = imgEntry->flashOffset;
                err = PTM_ReadFlash(flashID,
                        PTM_Comp.userImageAddr + flashOffset,
                        PTM_USER_DATA_MAX_SIZE,
                        (uint8_t*)&PTM_UserData,
                        &PTM_Comp.msgHdr0);
            } else {
                aMsg->result = BCM_ERR_NOT_FOUND;
                err = BCM_ERR_NODEV;
            }
        }
    }

    return err;
}


static int32_t PTM_WriteUD(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_NOT_FOUND;
    PTM_FlashIDType flashID;

    flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
    PTM_ImgEntryType *imgEntry = getImageEntry(&PTM_Comp.userImage, aMsg->imgID);
    if (NULL != imgEntry) {
        if (aMsg->inLen <= (PTM_USER_DATA_MAX_SIZE - sizeof(PTM_UserDataHdr))) {
            PTM_UserData.hdr.magic = PTM_USER_DATA_MAGIC;
            PTM_UserData.hdr.size = aMsg->inLen;
            PTM_UserData.hdr.errDetCode = BCM_CRC32(aMsg->imgLoadAddrBuf, aMsg->inLen, ITBL_CRC32_POLY);
            PTM_UserData.hdr.reserved[0UL] = 0UL;
            PTM_UserData.hdr.reserved[1UL] = 0UL;
            PTM_UserData.hdr.reserved[2UL] = 0UL;
            PTM_UserData.hdr.reserved[3UL] = 0UL;
            BCM_MemCpy(PTM_UserData.data, aMsg->imgLoadAddrBuf, PTM_UserData.hdr.size);
            PTM_Comp.msgHdr1 = NULL;
            PTM_Comp.msgHdr0 = NULL;
            err = PTM_EraseFlash(flashID,
                    PTM_Comp.userImageAddr + imgEntry->flashOffset,
                    PTM_USER_DATA_MAX_SIZE,
                    &PTM_Comp.msgHdr1);
            if (BCM_ERR_OK == err) {
                err = PTM_WriteFlash(flashID,
                        PTM_Comp.userImageAddr + imgEntry->flashOffset,
                        PTM_USER_DATA_MAX_SIZE,
                        (uint8_t*)&PTM_UserData,
                        &PTM_Comp.msgHdr0);
            }
        } else {
            err = BCM_ERR_NOMEM;
        }
    }

    return err;
}

static int32_t PTM_EraseUD(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_NOT_FOUND;
    PTM_FlashIDType flashID;

    flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
    PTM_ImgEntryType *imgEntry = getImageEntry(&PTM_Comp.userImage, aMsg->imgID);
    if (NULL != imgEntry) {
        PTM_Comp.msgHdr0 = NULL;
        err = PTM_EraseFlash(flashID,
                PTM_Comp.userImageAddr + imgEntry->flashOffset,
                PTM_USER_DATA_MAX_SIZE,
                &PTM_Comp.msgHdr0);
    }

    return err;
}

static int32_t PTM_InitStateHandler(void)
{
    int32_t err = BCM_ERR_BUSY;

    switch (PTM_Comp.opState) {
    case PTM_OP_STATE_RESET:
        err = PTM_ReadPTable();
        if (BCM_ERR_OK == err) {
            PTM_Comp.opState = PTM_OP_STATE_PTBL_REQ;
            err = BCM_ERR_BUSY;
        } else {
            err = BCM_ERR_UNKNOWN;
        }
        break;
    case PTM_OP_STATE_PTBL_REQ:
        {
            uint32_t i;
            for (i = 0UL; i < PTBL_NUM_COPIES; i++) {
                err = PTM_ValidatePT(&PTM_Comp.pTable[i]);
                if (BCM_ERR_OK == err) {
                    MCU_FWBootInfoType bootInfo;
                    uint16_t copyID = 0x1;
                    uint16_t pid = (PTBL_ID_FW);
                    PTM_Comp.ptCopyIdx = i;
                    err = MCU_GetFWBootInfo(&bootInfo);
                    if (BCM_ERR_OK == err) {
                        copyID = bootInfo & MCU_FW_BOOT_INFO_FW_COPY_ID_MASK;
                        copyID >>= MCU_FW_BOOT_INFO_FW_COPY_ID_SHIFT;
                    }
                    pid |= ((copyID << PTBL_ID_COPY_SHIFT) & PTBL_ID_COPY_MASK);
                    err = PTM_ReadImgPartHdr(pid, (uint8_t*)&PTM_Comp.fwCfgImage);
                    if (BCM_ERR_OK == err) {
                        PTM_Comp.opState = PTM_OP_STATE_SYS_CONFIG_IMG_REQ;
                        err = BCM_ERR_BUSY;
                        break;
                    }
                }
            }
            if (PTBL_NUM_COPIES == i) {
                err= BCM_ERR_UNKNOWN;
            }
        }
        break;
    case PTM_OP_STATE_SYS_CONFIG_IMG_REQ:
        err = PTM_ValidateImgPartHdr(&PTM_Comp.fwCfgImage);
        if (BCM_ERR_OK == err) {
            MCU_FWBootInfoType bootInfo;
            uint16_t copyID = 0x1;
            uint16_t pid = (PTBL_ID_SYSCFG);
            err = MCU_GetFWBootInfo(&bootInfo);
            if (BCM_ERR_OK == err) {
                copyID = bootInfo & MCU_FW_BOOT_INFO_SYSCFG_COPY_ID_MASK;
                copyID >>= MCU_FW_BOOT_INFO_SYSCFG_COPY_ID_SHIFT;
            }
            pid |= ((copyID << PTBL_ID_COPY_SHIFT) & PTBL_ID_COPY_MASK);
            err = PTM_ReadImgPartHdr(pid, (uint8_t*)&PTM_Comp.sysCfgImage);
            if (BCM_ERR_OK == err) {
                PTM_Comp.opState = PTM_OP_STATE_CONFIG_IMG_REQ;
                err = BCM_ERR_BUSY;
            } else {
                err = BCM_ERR_UNKNOWN;
            }
        } else {
            err = BCM_ERR_UNKNOWN;
        }
        break;
    case PTM_OP_STATE_CONFIG_IMG_REQ:
        err = PTM_ValidateImgPartHdr(&PTM_Comp.sysCfgImage);
        if (BCM_ERR_OK == err) {
            err = PTM_ReadImgPartHdr((0x1U << PTBL_ID_COPY_SHIFT) | PTBL_ID_USER_DATA ,
                                    (uint8_t*)&PTM_Comp.userImage);
            if (BCM_ERR_OK == err) {
                PTM_Comp.opState = PTM_OP_STATE_USER_IMG_REQ;
                err = BCM_ERR_BUSY;
            } else {
                err = BCM_ERR_UNKNOWN;
            }
        } else {
            err = BCM_ERR_UNKNOWN;
        }
        break;
    case PTM_OP_STATE_USER_IMG_REQ:
        err = PTM_ValidateImgPartHdr(&PTM_Comp.userImage);
        if (BCM_ERR_OK != err) {
            BCM_MemSet(&PTM_Comp.userImage, 0, sizeof(ITBL_Type));
            PTM_ReportError((uint8_t)0, PTM_INIT_STATE_HANDLER_API_ID,
                    err, 0UL, 0UL, 0UL, __LINE__);
            /* Error is over-written, so that PTM doesn't go in error
             * state if user data is corrupted, as User data is not
             * critical */
            err = BCM_ERR_OK;
        }
        break;
    default:
        break;
    }

    return err;
}

static int32_t PTM_UnInitStateHandler(void)
{
    int32_t err;
    PTM_FlashIDType flashID;
    FLSMGR_OpResultType opsRes;

    flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
    opsRes = PTM_FlashGetOpsResult(flashID, PTM_Comp.msgHdr0);
    if (FLSMGR_OP_RESULT_SUCCESS == opsRes) {
        do {
            err = PTM_InitStateHandler();
            if (BCM_ERR_OK == err) {
                PTM_Comp.state = PTM_STATE_IDLE;
            } else if ((BCM_ERR_BUSY != err) && (BCM_ERR_EAGAIN != err)) {
                PTM_Comp.state = PTM_STATE_ERROR;
            } else {
                /* Continue with Init sequence */
            }
        } while (BCM_ERR_EAGAIN == err);
    } else {
        PTM_Comp.state = PTM_STATE_ERROR;
        err = BCM_ERR_UNKNOWN;
    }

    return err;
}

static int32_t PTM_IdleStateHandler(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_UNKNOWN;

    if (NULL == aMsg) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if ((aMsg->cmd == PTM_CMD_LOAD_IMG)
        || (aMsg->cmd == PTM_CMD_LOAD_USERDATA)) {
        err = PTM_ReadImg(aMsg);
    } else if (aMsg->cmd == PTM_CMD_WRITE_USERDATA) {
        err = PTM_WriteUD(aMsg);
    } else if (aMsg->cmd == PTM_CMD_ERASE_USERDATA) {
        err = PTM_EraseUD(aMsg);
    }
    if (BCM_ERR_OK == err) {
        PTM_Comp.state = PTM_STATE_BUSY;
        PTM_Comp.currMsg = aMsg;
        err = BCM_ERR_BUSY;
    } else {
        err = BCM_ERR_NOT_FOUND;
    }

    return err;
}

static int32_t PTM_BusyStateHandler(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_BUSY;
    PTM_FlashIDType flashID;
    FLSMGR_OpResultType opsRes;
    PTM_ImgEntryType *imgEntry = NULL;

    if (NULL == aMsg) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if (PTM_Comp.currMsg == aMsg) {
        flashID = PTM_Comp.lookupTbl[PTM_Comp.lookupIdx].flashID;
        opsRes = PTM_FlashGetOpsResult(flashID, PTM_Comp.msgHdr0);
        if (FLSMGR_OP_RESULT_PENDING != opsRes) {
            PTM_Comp.msgHdr0 = NULL;
            if (NULL != PTM_Comp.msgHdr1) {
                opsRes = PTM_FlashGetOpsResult(flashID, PTM_Comp.msgHdr1);
                PTM_Comp.msgHdr1 = NULL;
            }
            if (FLSMGR_OP_RESULT_FAILED == opsRes) {
                aMsg->result = BCM_ERR_NODEV;
                err = BCM_ERR_UNKNOWN;
            } else {
                if (PTM_CMD_LOAD_IMG == aMsg->cmd) {
                    if (PTBL_ID_SYSCFG == aMsg->pid) {
                        imgEntry = getImageEntry(&PTM_Comp.sysCfgImage, aMsg->imgID);
                    } else if (PTBL_ID_FW == aMsg->pid) {
                        imgEntry = getImageEntry(&PTM_Comp.fwCfgImage, aMsg->imgID);
                    } else {
                        /* Unhandled partition */
                    }
                    /* CERT-C 699925 */
                    if ((NULL != imgEntry) && (BCM_ERR_NOMEM != aMsg->result) && (0UL == aMsg->offset)) {
                        err = PTM_ValidateImg(imgEntry, aMsg);
                    } else if (BCM_ERR_NOMEM == aMsg->result) {
                        err = BCM_ERR_NOMEM;
                    } else if (0UL != aMsg->offset) {
                        err = BCM_ERR_OK;
                    } else {
                        err = BCM_ERR_NOT_FOUND;
                    }
                } else  if (PTM_CMD_LOAD_USERDATA == aMsg->cmd) {
                    err = PTM_ValidateUserData(aMsg);
                } else {
                    err = BCM_ERR_OK;
                }
                if (BCM_ERR_OK == err) {
#ifdef __ENABLE_DCACHE__
                    DCACHE_CleanAll();
#endif
                    aMsg->result = BCM_ERR_OK;
                } else if (BCM_ERR_NOMEM == err) {
#ifdef __ENABLE_DCACHE__
                    DCACHE_CleanAll();
#endif
                } else {
                    aMsg->result = BCM_ERR_DATA_INTEG;
                }
            }
            PTM_Comp.state = PTM_STATE_IDLE;
            PTM_Comp.currMsg = NULL;
        }
    }

    return err;
}

int32_t PTM_Init(const PTM_CfgType *const aConfig)
{
    int32_t retVal;
    int32_t errVal;
    uint32_t i;
    const PTM_LookupTblEntType *cfgtbl;
    PTM_PTHdrType *ptHdr;
    PTM_LookupTblEntType *lookupTbl = &PTM_Comp.lookupTbl[0];
    MCU_FWBootInfoType bootInfo;
    PTM_FlashIDType flashID = 0UL;

    if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (PTM_STATE_UNINIT != PTM_Comp.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (NULL != aConfig->ptLoadAddr) {
        ptHdr = (PTM_PTHdrType *)(aConfig->ptLoadAddr);
        if (PTBL_MAGIC == ptHdr->magicNumber) {
            BCM_MemCpy(&PTM_Comp.pTable[0UL], aConfig->ptLoadAddr,
                    ptHdr->totalSize);
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
            goto err;
        }
    } else {
        if (NULL != aConfig->lookupTbl) {
            errVal = MCU_GetFWBootInfo(&bootInfo);
            if (BCM_ERR_OK == errVal) {
                flashID = (bootInfo & MCU_FW_BOOT_INFO_FLASH_ID_MASK) >> MCU_FW_BOOT_INFO_FLASH_ID_SHIFT;
            }
            cfgtbl = aConfig->lookupTbl;
            for (i = 0UL; i < aConfig->lookupTblSize; i++) {
                lookupTbl[i].flashID = cfgtbl[i].flashID;
                lookupTbl[i].flashAddr = cfgtbl[i].flashAddr;
                if (flashID == lookupTbl[i].flashID) {
                    PTM_Comp.lookupIdx = i;
                }
            }
            PTM_Comp.lookupTblSize = aConfig->lookupTblSize;
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
            goto err;
        }
    }
    do {
        retVal = PTM_InitStateHandler();
        if ((BCM_ERR_BUSY != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            PTM_Comp.state = PTM_STATE_ERROR;
        }
    } while (BCM_ERR_EAGAIN == retVal);

    if (BCM_ERR_BUSY == retVal) {
        retVal = BCM_ERR_OK;
    }
err:
    return retVal;
}

void PTM_DeInit(void)
{
    if (PTM_STATE_UNINIT != PTM_Comp.state) {
        BCM_MemSet(PTM_Comp.pTable, 0, PTBL_NUM_COPIES * PTBL_SIZE);
        BCM_MemSet(&PTM_Comp.sysCfgImage, 0, sizeof(ITBL_Type));
        BCM_MemSet(&PTM_Comp.fwCfgImage, 0, sizeof(ITBL_Type));
        BCM_MemSet(&PTM_Comp.userImage, 0, sizeof(ITBL_Type));
        PTM_Comp.state = PTM_STATE_UNINIT;
        PTM_Comp.opState = PTM_OP_STATE_RESET;
        PTM_Comp.lookupIdx = 0UL;
    }
}

int32_t PTM_ProcessMsg(PTM_MsgType *const aMsg)
{
    int32_t err = BCM_ERR_BUSY;

    switch (PTM_Comp.state) {
    case PTM_STATE_UNINIT:
        err = PTM_UnInitStateHandler();
        break;
    case PTM_STATE_IDLE:
        err = PTM_IdleStateHandler(aMsg);
        break;
    case PTM_STATE_BUSY:
        err = PTM_BusyStateHandler(aMsg);
        break;
    case PTM_STATE_ERROR:
        if (NULL != aMsg) {
            aMsg->result = BCM_ERR_INVAL_STATE;
        }
        err = BCM_ERR_UNKNOWN;
        break;
    default:
        break;
    }

    return err;
}

static const ITBL_Type *PTM_GetImgTbl(uint8_t aPID)
{
    const ITBL_Type *tbl = NULL;

    if ((PTM_STATE_IDLE == PTM_Comp.state) || (PTM_STATE_BUSY == PTM_Comp.state)) {
        switch (aPID) {
        case PTBL_ID_SYSCFG:
            tbl = &PTM_Comp.sysCfgImage;
            break;
        break;
        case PTBL_ID_FW:
            tbl = &PTM_Comp.fwCfgImage;
            break;
        break;
        default:
        break;
        }
    }

    return tbl;
}

int32_t PTM_LoadImg(uint8_t aPID,
                    PTM_ImgIDType aImgID,
                    PTM_AddrType const aAddr,
                    uint32_t aOffset,
                    PTM_SizeType aBufSize,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    int32_t err = BCM_ERR_OK;
    PTM_MsgType msg;

    if ((NULL == aAddr) || (0UL == aBufSize) || (NULL == aHdr)) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if ((PTM_STATE_IDLE == PTM_Comp.state) || (PTM_STATE_BUSY == PTM_Comp.state)) {
        msg.pid = aPID;
        msg.imgID = aImgID;
        msg.cmd = PTM_CMD_LOAD_IMG;
        msg.imgLoadAddrBuf = aAddr;
        msg.offset = aOffset;
        msg.inLen = aBufSize;
        msg.result = BCM_ERR_UNKNOWN;
        err = PTM_SendMsg(&msg, aClientMask, aHdr);
    } else {
        err = BCM_ERR_INVAL_STATE;
    }

    return err;
}

int32_t PTM_LoadUserData(PTM_ImgIDType aImgID,
                    PTM_AddrType const aAddr,
                    PTM_SizeType aBufSize,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    int32_t err = BCM_ERR_OK;
    PTM_MsgType msg;

    if ((NULL == aAddr) || (0UL == aBufSize) || (NULL == aHdr)) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if ((PTM_STATE_IDLE == PTM_Comp.state) || (PTM_STATE_BUSY == PTM_Comp.state)) {
        msg.imgID = aImgID;
        msg.cmd = PTM_CMD_LOAD_USERDATA;
        msg.imgLoadAddrBuf = aAddr;
        msg.inLen = aBufSize;
        msg.result = BCM_ERR_UNKNOWN;
        err = PTM_SendMsg(&msg, aClientMask, aHdr);
    } else {
        err = BCM_ERR_INVAL_STATE;
    }

    return err;
}

int32_t PTM_WriteUserData(PTM_ImgIDType aImgID,
                    PTM_AddrType const aAddr,
                    PTM_SizeType aBufSize,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    int32_t err = BCM_ERR_OK;
    PTM_MsgType msg;

    if ((NULL == aAddr) || (0UL == aBufSize) || (NULL == aHdr)) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if ((PTM_STATE_IDLE == PTM_Comp.state) || (PTM_STATE_BUSY == PTM_Comp.state)) {
        msg.imgID = aImgID;
        msg.cmd = PTM_CMD_WRITE_USERDATA;
        msg.imgLoadAddrBuf = aAddr;
        msg.inLen = aBufSize;
        msg.result = BCM_ERR_UNKNOWN;
        err = PTM_SendMsg(&msg, aClientMask, aHdr);
    } else {
        err = BCM_ERR_INVAL_STATE;
    }

    return err;
}

int32_t PTM_EraseUserData(PTM_ImgIDType aImgID,
                    const uint32_t aClientMask,
                    const MSGQ_MsgHdrType** const aHdr)
{
    int32_t err = BCM_ERR_OK;
    PTM_MsgType msg;

    if (NULL == aHdr) {
        err = BCM_ERR_INVAL_PARAMS;
    } else if ((PTM_STATE_IDLE == PTM_Comp.state) || (PTM_STATE_BUSY == PTM_Comp.state)) {
        msg.imgID = aImgID;
        msg.cmd = PTM_CMD_ERASE_USERDATA;
        msg.result = BCM_ERR_UNKNOWN;
        err = PTM_SendMsg(&msg, aClientMask, aHdr);
    } else {
        err = BCM_ERR_INVAL_STATE;
    }

    return err;
}


int32_t PTM_GetStatus(const MSGQ_MsgHdrType *const aHdr,
                             PTM_SizeType *const aImgSize)
{
    int32_t err;
    PTM_MsgType msg;
    int32_t status;

    err = PTM_RecvMsg(aHdr, &msg);
    if (BCM_ERR_OK == err) {
        *aImgSize = msg.imgActualLen;
        status = msg.result;
    } else {
        status = BCM_ERR_BUSY;
    }

    return status;
}

PTM_StateType PTM_GetState(void)
{
    return PTM_Comp.state;
}


const IMGL_ModeHandlerType PTM_ImageHandler = {
    .mode = IMGL_LOAD_MODE_NVM,
    .getImgTbl = PTM_GetImgTbl,
    .loadImg = PTM_LoadImg,
    .getStatus = PTM_GetStatus,
};
