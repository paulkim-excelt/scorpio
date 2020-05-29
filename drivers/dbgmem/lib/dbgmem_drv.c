/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_dbgmemdrv_impl
    @{

    @file dbgmem_drv.c
    @brief Implementation
    This header file contains the functions for DBGMEM Driver
    @version 1.0 Changelist1
*/

#include <dbgmem.h>
#include <dbgmem_osil.h>

/**
    @name DBGMEM Driver design IDs
    @{
    @brief Design IDs for DBGMEM Driver
*/
#define BRCM_SWDSGN_DBGMEM_SVCIO_TYPE           (0x90U) /**< @brief #DBGMEM_SVCIOType     */
#define BRCM_SWDSGN_DBGMEM_DRVCOPY8_PROC        (0x91U) /**< @brief #DBGMEM_DrvCopy8      */
#define BRCM_SWDSGN_DBGMEM_DRVCOPY16_PROC       (0x92U) /**< @brief #DBGMEM_DrvCopy16     */
#define BRCM_SWDSGN_DBGMEM_DRVCOPY32_PROC       (0x93U) /**< @brief #DBGMEM_DrvCopy32     */
#define BRCM_SWDSGN_DBGMEM_DRVCOPY_PROC         (0x94U) /**< @brief #DBGMEM_DrvCopy       */
#define BRCM_SWDSGN_DBGMEM_SYSCMDHANDLER_PROC   (0x95U) /**< @brief #DBGMEM_SysCmdHandler */
/** @} */

/**
    @name DBGMEM_SVCIOType
    @{

    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
 */
typedef union _DBGMEM_SVCIOType {
    uint8_t *data;      /**< @brief Pointer to data buffer    */
    DBGMEM_IOType *io;  /**< @brief Pointer to #DBGMEM_IOType */
} DBGMEM_SVCIOType;
/** @} */

/**
    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
*/
static int32_t DBGMEM_DrvCopy8(volatile uint8_t *aDst, volatile uint8_t *aSrc, uint32_t aLength)
{
    int32_t retVal;
    uint32_t i;

    if (0UL == aLength) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0; i< aLength; i++) {
            *aDst++ = *aSrc++;
        }
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
*/
static int32_t DBGMEM_DrvCopy16(volatile uint16_t *aDst, volatile uint16_t *aSrc, uint32_t aLength)
{
    int32_t retVal;
    uint32_t i;

    if (0UL == aLength) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0; i< aLength; i++) {
            *aDst++ = *aSrc++;
        }
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
*/
static int32_t DBGMEM_DrvCopy32(volatile uint32_t *aDst, volatile uint32_t *aSrc, uint32_t aLength)
{
    int32_t retVal;
    uint32_t i;

    if (0UL == aLength) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0; i< aLength; i++) {
            *aDst++ = *aSrc++;
        }
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_DBGMEM_COPY_PROC
    @trace #BRCM_SWREQ_DBGMEM
*/
static int32_t DBGMEM_DrvCopy(uint32_t aDst, uint32_t aSrc,
                                uint32_t aWidth, uint32_t aLength)
{
    int32_t retVal = BCM_ERR_OK;
    DBGMEM_HandleType dst, src;

    dst.u32Val = aDst;
    src.u32Val = aSrc;

    switch(aWidth) {
    case 8UL:
        retVal = DBGMEM_DrvCopy8(dst.u8Ptr, src.u8Ptr, aLength);
        break;
    case 16UL:
        retVal = DBGMEM_DrvCopy16(dst.u16Ptr, src.u16Ptr, aLength);
        break;
    case 32UL:
        retVal = DBGMEM_DrvCopy32(dst.u32Ptr, src.u32Ptr, aLength);
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_DBGMEM_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_DBGMEM_KERNEL_HANDLER
*/
void DBGMEM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO)
{
    DBGMEM_SVCIOType dbgmemSVCIO;
    DBGMEM_IOType *dbgmemIO;

    dbgmemSVCIO.data = aSysIO;

    if ((NULL != aSysIO) && (SVC_MAGIC_MEM_ID == aMagicID)) {
        dbgmemIO = dbgmemSVCIO.io;

        switch(aCmd) {
        case DBGMEM_CMD_COPY:
            dbgmemIO->retVal = DBGMEM_DrvCopy(dbgmemIO->dst, dbgmemIO->src,
                                            dbgmemIO->width, dbgmemIO->length);
            break;
        default:
            dbgmemIO->retVal = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }
}

/** @} */
