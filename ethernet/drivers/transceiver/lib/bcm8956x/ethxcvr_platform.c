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
    @defgroup grp_eth_xcvrdrv_plat_impl Ethernet Transceiver platform layer
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_plat_impl
    @{

    @file platform.c
    @brief This file implements the platform layer for Ethernet Transceiver driver.

    @version 0.1 Initial draft
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <eth_xcvr.h>
#include <ethxcvr_osil.h>
#include "ethxcvr_platform.h"
#include "brphy_osil.h"
#include "sgmii_osil.h"
#include "rgmii_osil.h"
#include "dummyphy_osil.h"

/**
    @name Transceiver driver platform Design IDs
    @{
    @brief Design IDs for Transceiver driver platform layer
*/
#define BRCM_SWDSGN_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC  (0x80U) /**< @brief #ETHXCVR_PlatformGetFnTblsCntxt */
/** @} */

/**
    @trace #BRCM_SWARCH_ETHXCVR_PLATFORMGETFNTBLSCNTXT_PROC
    @trace #BRCM_SWREQ_ETHXCVR_KERNEL_HANDLER
*/
int32_t ETHXCVR_PlatformGetFnTblsCntxt(
                                const ETHXCVR_PortConfigType *const aConfig,
                                const ETHXCVR_FuncTblType **const aBusFnTbl,
                                const ETHXCVR_FuncTblType **const aPhyFnTbl,
                                ETHXCVR_PhyCntxtType **const aPhyCntxt)
{
    int32_t retVal = BCM_ERR_OK;

    /* FIXME: Populate PHY context */

    switch (aConfig->id) {
        case 0U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_SGMII) &&
                (aConfig->bus.cntrlID == 1U)) {
                *aBusFnTbl = &SGMII_FuncTbl;
                /* Invoke Board for external PHY*/
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case 1U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_SGMII) &&
                (aConfig->bus.cntrlID == 2U)) {
                *aBusFnTbl = &SGMII_FuncTbl;
                /* Invoke Board for external PHY*/
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else if ((aConfig->busMode == ETHXCVR_BUSMODE_INTG) &&
                       (aConfig->phy.hwID == 0U) &&
                       (aConfig->phy.accessMode == ETHXCVR_ACCESSMODE_MMAP)) {
                *aBusFnTbl = NULL;
                *aPhyFnTbl = &BRPHY_FuncTbl;
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case 2U:
        case 3U:
        case 4U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_INTG) &&
                (aConfig->phy.hwID > 0U) && (aConfig->phy.hwID < 4U) &&
                (aConfig->phy.accessMode == ETHXCVR_ACCESSMODE_MMAP)) {
                *aBusFnTbl = NULL;
                *aPhyFnTbl = &BRPHY_FuncTbl;
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case 5U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_SGMII) &&
                (aConfig->bus.cntrlID == 3U)) {
                *aBusFnTbl = &SGMII_FuncTbl;
                /* Invoke Board for external PHY*/
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else if ((aConfig->busMode == ETHXCVR_BUSMODE_RGMII) &&
                       (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &RGMII_FuncTbl;
                /* Invoke Board for external PHY */
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case 6U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_SGMII) &&
                (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &SGMII_FuncTbl;
                /* Invoke Board for external PHY*/
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else if ((aConfig->busMode == ETHXCVR_BUSMODE_PCIE) &&
                       (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &DUMMYPHY_FuncTbl;
                *aPhyFnTbl = NULL;
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case 8U:
            if ((aConfig->busMode == ETHXCVR_BUSMODE_SGMII) &&
                (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &SGMII_FuncTbl;
                /* Invoke Board for external PHY*/
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else if ((aConfig->busMode == ETHXCVR_BUSMODE_PCIE) &&
                       (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &DUMMYPHY_FuncTbl;
                *aPhyFnTbl = NULL;
            } else if ((aConfig->busMode == ETHXCVR_BUSMODE_RGMII) &&
                       (aConfig->bus.cntrlID == 0U)) {
                *aBusFnTbl = &RGMII_FuncTbl;
                /* Invoke Board for external PHY */
                retVal = ETHXCVR_BoardGetPhyFnTbl(aConfig, aPhyFnTbl);
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }

    return retVal;
}

/** @} */
