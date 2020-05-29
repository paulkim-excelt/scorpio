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
    constitutes the valuable trade secerrs of Broadcom, and you shall use all
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
#include <bl_cntrl.h>
#include <bl_downloader.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#include <bl_board.h>
#include <bl_log.h>
#include <imgl_ipc_cmds.h>

/**
    @name FW execute API IDs
    @{
    @brief API IDs for FW execute
*/
#define BRCM_DSGN_BL_CNTRL_EXEC_REPORT_ERROR_PROC     (0xE0U) /**< @brief #BL_CNTRL_EXEC_ReportError         */
/** @} */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_ERROR)

static BL_COMP_NEVER_INLINE void BL_CNTRL_EXEC_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_CNTRL_EXEC_REPORT_ERROR_PROC, aErr, 4UL, values);
}

static ITBL_Type ImgTbl;
static IMGL_ImgHdrType ImgHdrs[BL_CNTRL_MAX_IMG_COUNT]; /**< @brief Array of pwImgHdrs to be written to flash */
typedef void (*ImgEntryFuncType)(void);

int32_t BL_CNTRL_GetImgLoadAddr(uint16_t aImgID, uint8_t **aLoadAddr)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t i;

    for (i = 0; i < ITBL_MAX_NUM_ENTRIES; i++) {
        if (ImgTbl.entry[i].imgType == aImgID) {
            *aLoadAddr = (uint8_t *)ImgTbl.entry[i].loadAddr;
            break;
        }
    }
    if (i == ITBL_MAX_NUM_ENTRIES) {
        err = BL_BCM_ERR_NOT_FOUND;
    }
    return err;
}

int32_t BL_CNTRL_ExecuteFW(SYS_UpdateExecCmdType *const aFlsInfo,
                        BL_CNTRL_ImageFetchProcType aImageFetchProc)
{
    ETHXCVR_ConfigType xcvrConfig;
    uint32_t imageCount = aFlsInfo->numImgs;
    IMGL_ImgHdrType *imgHdr = aFlsInfo->imgHdr;
    uint32_t i;
    uint8_t *loadAddr;
    uint32_t entryAddr;
    uint32_t line = __LINE__;
    int32_t err = BL_BCM_ERR_OK;

    if ((0UL == imageCount) ||
        (BL_CNTRL_MAX_IMG_COUNT < imageCount) ||
        (NULL == imgHdr) ||
        (NULL == aImageFetchProc)) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto exit;
    }

    BL_BCM_MemCpy(ImgHdrs, imgHdr, imageCount * sizeof(IMGL_ImgHdrType));

    for (i = 0UL; i < imageCount; i++) {
        /* fetch the XCVR image apply first */
        if ((PTBL_ID_SYSCFG == ImgHdrs[i].pid) && (0x9000 == ImgHdrs[i].imgID)) {
            err = aImageFetchProc(&ImgHdrs[i], (uint8_t*)&xcvrConfig);
            if (BL_BCM_ERR_OK != err) {
                line = __LINE__;
                break;
            }
            BL_CNTRL_XcvrInit(&xcvrConfig);
        }
        if (PTBL_ID_FW == ImgHdrs[i].pid) {
            if (BL_TRUE == ImgHdrs[i].isImgHdr) {
                err = aImageFetchProc(&ImgHdrs[i], (uint8_t*)&ImgTbl);
                if (BL_BCM_ERR_OK != err) {
                    line = __LINE__;
                    break;
                }
            } else {
                if (ImgHdrs[i].imgID < ITBL_ID_EXE_MAX) {
                    err = BL_CNTRL_GetImgLoadAddr(ImgHdrs[i].imgID, &loadAddr);
                    if (BL_BCM_ERR_OK != err) {
                        line = __LINE__;
                        break;
                    }
                    err = aImageFetchProc(&ImgHdrs[i], loadAddr);
                    if (BL_BCM_ERR_OK != err) {
                        line = __LINE__;
                        break;
                    }
                }
            }
        }
    }

    if (BL_BCM_ERR_OK == err) {
#if defined(BL_ENABLE_RAM_DWNLD_MODE)
        BL_RAM_DWNLD_ResetDwnldInfo();
#endif
#if defined(BL_ENABLE_IPC_DWNLD_MODE)
        BL_IPC_DWNLD_ResetDwnldInfo();
#endif
#ifdef BL_ENABLE_DCACHE
#if defined( __CORTEX_MX__)
        CORTEX_MX_DCacheDisable();
#endif
#endif
#ifdef BL_ENABLE_ICACHE
#if defined( __CORTEX_MX__)
        CORTEX_MX_ICacheDisable();
#endif
#endif
        entryAddr = ImgTbl.hdr.entryPoint;
#if defined( __CORTEX_MX__)
        entryAddr |= 0x1; /* set Thumb mode */
#endif
        ((ImgEntryFuncType)(entryAddr))();
    } else {
        BL_CNTRL_EXEC_ReportError(err, 0UL, 0UL, 0UL, 0UL, line);
    }
exit:
    return err;
}
