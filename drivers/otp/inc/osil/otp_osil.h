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
/** @defgroup grp_otpdrv_il Integration Guide
    @ingroup grp_otpdrv

    @addtogroup grp_otpdrv_il
    @{
    @section sec_otp_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes OTP Driver IL in more detail.

    TBD

    @file otp_osil.h
    @brief OTP Integration Interfaces
    This header file contains the integration interface for OTP driver.
    @version 1.31 Imported from docx
*/
#ifndef OTP_OSIL_H
#define OTP_OSIL_H

#include <otp.h>
#include <system.h>

/**
    @name OTP interface IDs
    @{
    @brief OTP integration interface IDs.
*/
#define BRCM_SWARCH_OTP_IO_CMD_TYPE              (0x40U) /**< @brief #OTP_IOCmdType    */
#define BRCM_SWARCH_OTP_IO_TYPE                  (0x41U) /**< @brief #OTP_IOType        */
#define BRCM_SWARCH_OTP_SYSCMDHANDLER_PROC       (0x42U) /**< @brief #OTP_SysCmdHandler */
#define BRCM_SWARCH_OTP_SYSCMDREQ_PROC           (0x43U) /**< @brief #OTP_SysCmdReq     */
/** @} */

/**
   @{
   @brief OTP IO command IDs

   @trace #BRCM_SWREQ_OTP_KERNEL_INTERFACE
*/
typedef uint32_t OTP_IOCmdType;
#define OTP_IO_CMD_INIT              (1UL)   /**< OTP controller Init command */
#define OTP_IO_CMD_READ              (2UL)   /**< OTP data read command */
#define OTP_IO_CMD_WRITE             (3UL)   /**< OTP data write command */
/** @} */

/**
    @brief OTP IO structure

    OTP IO structure to be used for #OTP_SysCmdHandler

    @trace #BRCM_SWREQ_OTP_KERNEL_INTERFACE
 */
typedef struct _OTP_IOType {
    int32_t                 retVal;     /**< @brief Return value from the command operation function */
    OTP_HwIDType            hwID;       /**< @brief OTP controller identifier */
    uint32_t                addr;       /**< @brief OTP row address for operation */
    uint32_t                data;       /**< @brief OTP data read from/ written to OTP row */
} OTP_IOType;

/** @brief OTP system command handler

    Interface to handle OTP system command request processing.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMagicID    OTP system request magic ID
    @param[in]      aCmd        OTP system request command
    @param[in]      aSysIO      Pointer to system request IO for OTP_IOType

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and install it
    as part of #SysCmdHandler.

    @trace #BRCM_SWREQ_OTP_KERNEL_HANDLER

    @limitations None
*/
extern void OTP_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd,
                                uint8_t *aSysIO);

/** @brief OTP system command request

    Interface to raise OTP system command request.

    @behavior Sync, Re-entrants

    @pre TBD

    @param[in]      aCmd        OTP IO request command
    @param[inout]   aIO         Pointer to OTP_IOType

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request success
    @retval     #BCM_ERR_INVAL_PARAMS   (aCmd is invalid) or
                                        (aIO is NULL)

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_OTP_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t OTP_SysCmdReq(OTP_IOCmdType aCmd, OTP_IOType * const aIO);

#endif /* OTP_OSIL_H */
/** @} */
