/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

#include <string.h>
#include <utils.h>
#include <bcm_err.h>
#include <eth_switch.h>

#define ETH_SWITCH_HW_ID        (0UL)

#define HWMAC2NMAC(hwMac, nMac)           \
    nMac[0U] = ((uint8_t)(hwMac >> 40UL & 0xFFU));\
    nMac[1U] = ((uint8_t)(hwMac >> 32UL & 0xFFU)) ;\
    nMac[2U] = ((uint8_t)(hwMac >> 24UL & 0xFFU)) ;\
    nMac[3U] = ((uint8_t)(hwMac >> 16UL & 0xFFU)) ;\
    nMac[4U] = ((uint8_t)(hwMac >> 8UL & 0xFFU))  ;\
    nMac[5U] = ((uint8_t)(hwMac >> 0UL & 0xFFU));

extern ETHERSWT_CfgType SwitchCfgUsrData;

/** @brief Get the Port configuration
 *
 * This API copies the port configuration of the given portID to the location
 * pointed by aPortCfg.
 *
 * @param[in]       aPortID     Port ID
 * @param[inout]    aPortCfg    Pointer to port configuration structure
 *
 * @retval          BCM_ERR_OK              Port configuration successfully
 *                                          found and copied to the location
 *                                          pointed by aPortCfg
 * @retval          BCM_ERR_INVAL_PARAMS    aPortCfg is NULL
 * @retval          BCM_ERR_NOT_FOUND       The given Port ID is not found in
 *                                          the switch configuration
 */
extern int32_t SwitchCfg_GetPortCfg(uint32_t aPortID,
                                    ETHERSWT_PortCfgType * const aPortCfg);

/** @brief Get VLAN information
 *
 * This API queries for VLAN information
 *
 * @param[in]   aVLANID     VLAN ID
 * @param[out]  aPortBitMap Bit mask of ports where given VLAN is enabled
 * @param[out]  aTagBitMap  Bit mask of ports where VLAN forward type is
 *                          set as tagged
 *
 * @retval      BCM_ERR_OK              Successfully retrieved VLAN info
 * @retval      BCM_ERR_INVAL_PARAMS    aPortBitMap in NULL
 * @retval      BCM_ERR_INVAL_PARAMS    aTagBitMap is NULL
 */
extern int32_t SwitchCfg_GetVLAN(ETHERSWT_VLANIDType aVLANID,
                                 uint32_t * const aPortBitMap,
                                 uint32_t * const aTagBitMap);

/** @breif Get Port specific VLAN info
 *
 * This API retrieves port specific VLAN info.
 *
 * @param[in]   aPortID         Port ID
 * @param[out]  aIngressFilter  Ingress Filter value
 * @param[out]  aDefaultVLAN    Default VLAN ID
 * @param[out]  aDefaultPrio    Default priority
 *
 * @retval      BCM_ERR_OK              VLAN info successfully retrieved
 * @retvak      BCM_ERR_INVAL_PARAMS    aIngressFilter in NULL
 * @retvak      BCM_ERR_INVAL_PARAMS    aDefaultVLAN in NULL
 * @retvak      BCM_ERR_INVAL_PARAMS    aDefaultPrio is NULL
 * @retval      BCM_ERR_NOT_FOUND       Port ID not found
 */
extern int32_t SwitchCfg_GetVLANPort(uint32_t aPortID,
                                     uint32_t * const aIngressFilter,
                                     ETHERSWT_VLANIDType * const aDefaultVLAN,
                                     ETHERSWT_PCPType * const aDefaultPrio);

extern int32_t SwitchCfg_JumboFrmGet(uint32_t aPortID, uint32_t *const aJumboFrm);
