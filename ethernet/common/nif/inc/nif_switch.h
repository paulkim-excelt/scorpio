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

/**
    @addtogroup grp_nif_ifc
    @{

    @file nif_switch.h
    @brief NIF Switch Interface File
    This header file contains the NIF functions for switch driver
    @version 0.1 Initial Version
*/

#ifndef NIF_SWITCH_H
#define NIF_SWITCH_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <nif.h>
#include <eth_switch.h>

/**
    @name NIF Architecture IDs
    @{
    @brief Architecture IDs for NIF
*/
#define BRCM_SWARCH_NIF_SWTMGMTINIT_PROC                 (0x42U) /**< @brief #NIF_SwtMgmtInit */
#define BRCM_SWARCH_NIF_SWTSETMGMTINFO_PROC              (0x43U) /**< @brief #NIF_SwtSetMgmtInfo */
/** @} */

/** @brief Initialize switch management

    This API initializes the switch management functionality. Switch management
    functionality control the ethernet frame regarding port ingress/egress
    funtionality and timestamp feature of the port

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aSwtIdx                 Switch index

    @retval          #BCM_ERR_OK              Successful
    @return          #BCM_ERR_INVAL_PARAMS    Invalid switch index
    @return          #BCM_ERR_UNINIT          NIF is not initialized

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_SWITCH_MANAGEMENT_INFO
*/
int32_t NIF_SwtMgmtInit(ETHERSWT_HwIDType aSwtIdx);

/** @brief Set the management information

    This API inserts the management information into the indexed
    ethernet buffer which was previously adapted for the transmission
    by calling ETHERSWT_TxAdaptBuffer() to achieve the tramission
    on specific ports of the switch

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]       aCtrlIdx            NIF controller index
    @param[in]       aBufIdx             Adapted Ethernet buffer index
    @param[in]       aMgmtinfo           Pointer to the management information

    @retval          #BCM_ERR_OK              Successful
    @return          #BCM_ERR_INVAL_PARAMS    Invalid buffer index (this buffer
                                             was not adapted)
    @return          #BCM_ERR_INVAL_PARAMS    aMgmtInfo is NULL
    @return          #BCM_ERR_UNINIT          Switch management functionality is

    @post describe post condition (status after execution of the function)

    @trace #BRCM_SWREQ_NIF_SWITCH_MANAGEMENT_INFO
*/
int32_t NIF_SwtSetMgmtInfo(NIF_CntrlIDType aCtrlIdx,
                           uint32_t aBufIdx,
                           const ETHERSWT_MgmtInfoType *const aMgmtInfo);

#endif /* NIF_SWITCH_H */
/** @} */
