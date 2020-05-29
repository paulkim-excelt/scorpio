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
    @addtogroup grp_nif_impl
    @{
    @file nif_wireless.c
    @brief NIF wireless Implementation file
    This source file contains the implementation of NIF wireless layer
    @version 0.1 Initial version
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <system.h>
#include <utils.h>
#include <nif.h>
#include <eth_osil.h>
#include <wl_eth.h>
#include <wl_eth_xcvr.h>

/**
    @name NIF Design IDs
    @{
    @brief Design IDs for NIF
*/
#define BRCM_SWDSGN_WLETH_RXIND_PROC                     (0xB8U) /**< @brief #WLETH_RxInd */
#define BRCM_SWDSGN_WLETH_TXDONEIND_PROC                 (0xB9U) /**< @brief #WLETH_TxDoneInd */
/** @} */

#if !defined(ENABLE_WLAN_TEST)
/**
    Wireless Ethernet RX packet indication callback

    @trace #BRCM_SWARCH_WLETH_RXIND_PROC
    @trace #BRCM_SWREQ_WLETH_NIF_NETWORK_RX_AND_TXCONF
*/
void WLETH_RxInd(ETHER_HwIDType aID, const uint8_t *const aBuf,
                 uint32_t aLen, const uint8_t *const aSrcMacAddr,
                 const uint8_t *const aDestMacAddr)
{
    ETHER_RxInd(aID, aBuf, aLen, aSrcMacAddr, aDestMacAddr);
}

/**
    Wireless Ethernet TX done callback

    @trace #BRCM_SWARCH_WLETH_TXDONEIND_PROC
    @trace #BRCM_SWREQ_WLETH_NIF_NETWORK_RX_AND_TXCONF
*/
void WLETH_TxDoneInd(ETHER_HwIDType aID, uint32_t aBufIdx,
                     ETHER_ResultType aResult)
{
    ETHER_TxDoneInd(aID, aBufIdx, aResult);
}
#endif

/** @} */
