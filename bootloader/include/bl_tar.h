/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited. All rights reserved.

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

/**
    @defgroup grp_bl_tar TAR
    @ingroup grp_bl_downloader

    @addtogroup grp_bl_tar
    @{
    @file bl_tar.h
    @brief TAR APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_TAR_H
#define BL_TAR_H

#include <bl_ptm.h>
#include <bl_cntrl.h>
#include <bl_chip_config.h>

/**
    @name TAR API IDs
    @{
    @brief API IDs for TAR
*/
#define BRCM_ARCH_BL_DWNLD_TAR_UPDATE_FLASH_PROC       (0x00U) /**< @brief #BL_TAR_UpdateFlash           */
/** @} */


/** @brief Function to update the flash with one TAR file as an input

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aFlashID    Flash ID
    @param[in]      aTarFile    Tar file packing all the images to be flashed
    @param[in]      aSize       Tar file size
    @param[in]      aCallback   Progress callback

    Return values are documented in reverse-chronological order
    @retval       #BL_BCM_ERR_OK             Flashing completed
    @retval       #BL_BCM_ERR_INVAL_PARAMS   If the arguments are invalid

    @post None

    @trace  #BRCM_DSGN_BL_CNTRL_GET_NUM_FW_PARTITIONS_PROC  #BRCM_ARCH_BL_CNTRL_GET_NUM_FW_PARTITIONS

    @limitations None
*/
int32_t BL_TAR_UpdateFlash(uint32_t aFlashID,
                             uint8_t *const aTarFile,
                             uint32_t aSize,
                             BL_PTM_ProgressCbType aCallback);

#endif /* BL_TAR_H */

/** @} */
