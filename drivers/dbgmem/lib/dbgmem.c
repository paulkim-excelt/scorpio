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

/**
    @defgroup grp_dbgmemdrv_impl Implementation
    @ingroup grp_dbgmemdrv

    @addtogroup grp_dbgmemdrv_impl
    @{

    @file dbgmem.c
    @brief Implementation
    This header file contains the interface functions for DBGMEM Driver
    @version 1.0 Changelist1
*/

#include <dbgmem.h>
#include <dbgmem_osil.h>

/**
    @name DBGMEM Driver design IDs
    @{
    @brief Design IDs for DBGMEM Driver
*/
#define BRCM_SWDSGN_DBGMEM_COPY_PROC         (0x80U) /**< @brief #DBGMEM_Copy       */
/** @} */

/**
    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
*/
int32_t DBGMEM_Copy(uint32_t aDst, uint32_t aSrc,
                    uint32_t aWidth, uint32_t  aLength)
{
    int32_t ret;
    DBGMEM_IOType dbgmemIO = {
        .retVal = BCM_ERR_UNKNOWN,
        .dst = aDst,
        .src = aSrc,
        .width = aWidth,
        .length = aLength,
    };

    ret = DBGMEM_SysCmdReq(DBGMEM_CMD_COPY, &dbgmemIO);
    if (BCM_ERR_OK == ret) {
        ret = dbgmemIO.retVal;
    }
    return ret;
}

/** @} */
