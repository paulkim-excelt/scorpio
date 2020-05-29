/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
#include <inttypes.h>
#include <string.h>
#include <bl_compiler.h>
#include <bl_utils.h>
#include <bl_bcm_err.h>
#include <bl_ptm.h>
#include <bl_cntrl.h>
#include <bl_mmi.h>
#include <bl_log.h>
#include <swr_cfg.h>

#define BRCM_DSGN_BL_SWR_REPORT_ERROR_PROC          (0xC0U) /**< @brief #BL_SWR_ReportError */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

SYSCFG_SWRConfigType BL_CNTRL_SWRConfig;

static BL_COMP_NEVER_INLINE void BL_SWR_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_SWR_REPORT_ERROR_PROC, aErr, 4UL, values);
}

/**
    @code{.c}
    Load switching regulator configuration using PTM_LoadBLConfig()
    Set Switching regulator config using MMI_Write()
    @endcode
*/
void BL_CNTRL_SWRConfigure()
{
#if defined(BL_ENABLE_PTM) && defined(BL_ENABLE_SWR_CONFIG)
    uint32_t i;
    uint16_t val;
    int32_t ret = BL_BCM_ERR_OK;
    int lineNo;
    uint32_t size = (uint32_t)sizeof(BL_CNTRL_SWRConfig);
    ret = BL_PTM_LoadSysCfgImg(0x9010, (BL_PTM_AddrType)&BL_CNTRL_SWRConfig, &size);
    if (BL_BCM_ERR_OK == ret) {
        if ((SWR_CONFIG_MAGIC == BL_CNTRL_SWRConfig.magic)
            && (BL_CNTRL_SWRConfig.count < SWR_CONFIG_COUNT)) {
            BL_MMI_Init();
            for (i = 0UL; i < BL_CNTRL_SWRConfig.count; i++) {
                BL_MMI_Write(BL_MMI_ACCESSMODE_CL22, BL_CNTRL_SWRConfig.mmi[i].pAddr, 0U, BL_CNTRL_SWRConfig.mmi[i].rAddr, BL_CNTRL_SWRConfig.mmi[i].value);
                /* read back and confirm */
                val = BL_MMI_Read(BL_MMI_ACCESSMODE_CL22, BL_CNTRL_SWRConfig.mmi[i].pAddr, 0U, BL_CNTRL_SWRConfig.mmi[i].rAddr);
                if (val != BL_CNTRL_SWRConfig.mmi[i].value) {
                    ret = BL_BCM_ERR_DATA_INTEG;
                }
            }
            BL_MMI_DeInit();
        } else {
            ret = BL_BCM_ERR_INVAL_MAGIC;
        }
    } else {
        lineNo = __LINE__;
    }

    if (BL_BCM_ERR_OK != ret) {
        BL_SWR_ReportError(ret, 0UL, 0UL,  0UL, 0UL, lineNo);
    }
#endif
}
