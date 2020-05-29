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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_imgl_impl Implementation
    @ingroup grp_imgl

    @addtogroup grp_imgl_impl
    @{

    @file imgl.c
    @brief Image Loader Implementation
    @version 0.1 Initial version
*/

#include <imgl.h>
#include <osil/imgl_osil.h>
#include <stdlib.h>

#if defined(ENABLE_PTM)
#include <ptm.h>
#endif

#if defined(ENABLE_IPC)
#include <rpc_loader.h>
#endif

/**
    @name Design IDs
    @{
    @brief Design IDs
*/
#define BRCM_SWDSGN_IMGL_MODES_GLOBAL         (0x80U)   /**< @brief #IMGL_Modes          */
#define BRCM_SWDSGN_IMGL_LOAD_IMAGE_PROC      (0x81U)   /**< @brief #IMGL_LoadImg        */
#define BRCM_SWDSGN_IMGL_GET_STATUS_PROC      (0x82U)   /**< @brief #IMGL_GetStatus      */
#define BRCM_SWDSGN_IMGL_LOAD_IMG_TABLE_PROC  (0x81U)   /**< @brief #IMGL_GetImgTbl      */
/** @} */

/**
    @brief List of Enabled modes for loading Images

    @trace #BRCM_SWARCH_IMGL_GET_STATUS_PROC
    @trace #BRCM_SWARCH_IMGL_LOAD_IMG_PROC
    @trace #BRCM_SWARCH_IMGL_LOAD_MODE_TYPE
    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
*/
const IMGL_ModeHandlerType * const IMGL_Modes[] = {
#ifdef IMGL_MODE_FUNC_TABLE_PRIO0
&IMGL_MODE_FUNC_TABLE_PRIO0,
#endif
#ifdef IMGL_MODE_FUNC_TABLE_PRIO1
&IMGL_MODE_FUNC_TABLE_PRIO1,
#endif
#ifdef IMGL_MODE_FUNC_TABLE_PRIO2
&IMGL_MODE_FUNC_TABLE_PRIO2,
#endif
};

/** @} */

/**
    @addtogroup grp_imgl_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_IMGL_LOAD_IMG_PROC
    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @code{.c}
    ret = BCM_ERR_NODEV
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else
        for each in IMGL_Modes:
            if each->loadImgTbl(aImgID, aAddr, aBufSize, aClientMask, &aStatus->hdr) == BCM_ERR_OK
                break
    return ret
    @endcode
*/
const ITBL_Type *IMGL_GetImgTbl(uint8_t aImgTblID)
{
    const ITBL_Type *tbl = NULL;
    uint32_t i;

    for (i = 0UL; i < sizeof(IMGL_Modes)/sizeof(IMGL_ModeHandlerType*); i++) {
       if ((NULL != IMGL_Modes[i]) && (NULL != IMGL_Modes[i]->getImgTbl)) {
            tbl = IMGL_Modes[i]->getImgTbl(aImgTblID);
            if (NULL != tbl) {
                break;
            }
        }
    }

    return tbl;
}

/**
    @trace #BRCM_SWARCH_IMGL_LOAD_IMG_PROC
    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @code{.c}
    ret = BCM_ERR_NODEV
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else
        for each in IMGL_Modes:
            if each->loadImage(aImgID, aAddr, aBufSize, aClientMask, &aStatus->hdr) == BCM_ERR_OK
                aStatus->size = 0
                aStatus->mode = each->mode
                break
    return ret
    @endcode
*/
int32_t IMGL_LoadImg(uint16_t aImgID, uint8_t *const aAddr, uint32_t aOffset,
                           uint32_t aBufSize, const uint32_t aClientMask,
                           IMGL_LoadStatusType* const aStatus)
{
    int32_t ret = BCM_ERR_NODEV;
    uint32_t i;
    uint8_t pid;

    if ((NULL == aAddr) || (NULL == aStatus)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else {
        if ((aImgID >> 12U) == 9) {
            pid = PTBL_ID_SYSCFG;
        } else {
            pid = PTBL_ID_FW;
        }

        for (i = 0UL; i < sizeof(IMGL_Modes)/sizeof(IMGL_ModeHandlerType*); i++) {
            if ((NULL != IMGL_Modes[i]) && (NULL != IMGL_Modes[i]->loadImg)) {
                ret = IMGL_Modes[i]->loadImg(pid, aImgID, aAddr, aOffset, aBufSize, aClientMask, &aStatus->hdr);
                if (BCM_ERR_OK == ret) {
                    aStatus->size = 0;
                    aStatus->mode = IMGL_Modes[i]->mode;
                    break;
                }
            }
        }
    }

    return ret;
}


/**
    @trace #BRCM_SWARCH_IMGL_GET_STATUS_PROC
    @trace #BRCM_SWREQ_IMGL_ABSTRACTION

    @code{.c}
    if aStatus is not NULL and aStatus->hdr is not NULL
        for each in IMGL_Modes:
            if each->mode == aStatus->mode:
                ret = each->getStatus(aStatus->hdr, &aStatus->size);
                if ret !=  BCM_ERR_BUSY:
                    aStatus->hdr = NULL
    return ret
    @endcode
*/
extern int32_t IMGL_GetStatus(IMGL_LoadStatusType *const aStatus)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    int32_t i;

    if ((NULL != aStatus) && (NULL != aStatus->hdr)) {
        for (i = 0UL; i < sizeof(IMGL_Modes)/sizeof(IMGL_ModeHandlerType*); i++) {
            if (NULL != IMGL_Modes[i]) {
                if (aStatus->mode == IMGL_Modes[i]->mode) {
                    ret = IMGL_Modes[i]->getStatus(aStatus->hdr, &aStatus->size);
                    if (BCM_ERR_BUSY != ret) {
                        aStatus->hdr = NULL;
                    }
                }
            }
        }
    }
    return ret;
}

/** @} */
