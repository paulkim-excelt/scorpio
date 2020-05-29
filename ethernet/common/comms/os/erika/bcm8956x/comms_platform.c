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
    @defgroup grp_comms_plat_bcm8953x Communication Subsystem platform layer for bcm8953x
    @ingroup grp_comms_sw

    @addtogroup grp_comms_plat_bcm8953x
    @{

    @file comms_platform.c
    @brief This file implements platform layer for the Communication Subsystem

    @version 0.1 Initial draft
*/

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <utils.h>
#include "eth_xcvr.h"
#include "comms.h"
#include "comms_platform.h"
#include "comms_cfg.h"

/**
    @name Communication Subsystem platform Design IDs
    @{
    @brief Design IDs for Communication Subsystem platform layer
*/
#define BRCM_SWDSGN_COMMS_CONVERTTOLOCALPORT_PROC       (0x80U) /**< @brief #COMMS_ConvertToLocalPort */
#define BRCM_SWDSGN_COMMS_CONVERTTOUNIFIEDPORT_PROC     (0x81U) /**< @brief #COMMS_ConvertToUnifiedPort */
#define BRCM_SWDSGN_COMMS_CONVERTTOUNIFIEDPORTMASK_PROC (0x82U) /**< @brief #COMMS_ConvertToUnifiedPortMask */
#define BRCM_SWDSGN_COMMS_INITXCVRSTACKINGPORTS_PROC    (0x83U) /**< @brief #COMMS_InitXcvrStackingPorts */
#define BRCM_SWDSGN_COMMS_UPDATEXCVRCONFIG_PART_PROC    (0x84U) /**< @brief #COMMS_UpdateXcvrConfig */
/** @} */

#define COMMS_ETHERSWT_MASTER_PORT_MASK   (0x1FFU)
#define COMMS_ETHERSWT_SLAVE_PORT_MASK    (0x7FU)
#define COMMS_ETHERSWT_ALL_PORT_MASK      (0xFF3FU)
#define COMMS_ETHERSWT_SLAVE1_PORT_OFFSET (0x9U)

/**
    @trace #BRCM_SWARCH_COMMS_CONVERTTOLOCALPORT_PROC
*/
uint32_t COMMS_ConvertToLocalPort(const MCU_ExtendedInfoType *const aStackInfo,
                                  uint32_t aPortNum,
                                  uint32_t *const aIsStackingPort)
{
    uint32_t localPortNum = aPortNum;

    if ((NULL != aStackInfo) && (NULL != aIsStackingPort)) {
        *aIsStackingPort = FALSE;
        if (aStackInfo->stackingEn == 0U) {
            if (aPortNum > 8UL) {
                localPortNum = COMMS_INVALID_PORT;
            }
        } else {
#ifdef __BCM89564G__
            if (aPortNum <= 8UL) {
                switch (aStackInfo->mstSlvMode) {
                    case MCU_DEVICE_MASTER:
                        if ((aPortNum != 0UL) &&
                            ((aPortNum == aStackInfo->stackPort0) ||
                            (aPortNum == aStackInfo->stackPort1))) {
                            localPortNum = COMMS_INVALID_PORT;
                        }
                        break;
                    default:
                        localPortNum = 8UL;
                        *aIsStackingPort = TRUE;
                        break;
                }
            } else if (aPortNum <= 15UL) {
                switch (aStackInfo->mstSlvMode) {
                    case MCU_DEVICE_MASTER:
                        localPortNum = aStackInfo->stackPort0;
                        *aIsStackingPort = TRUE;
                        break;
                    case MCU_DEVICE_SLAVE_1:
                        localPortNum = aPortNum - 9UL;
                        break;
                    case MCU_DEVICE_SLAVE_2:
                        localPortNum = aStackInfo->stackPort1;
                        *aIsStackingPort = TRUE;
                        break;
                    default:
                        break;
                }
            } else if (aPortNum <= 23UL) {
                switch (aStackInfo->mstSlvMode) {
                    case MCU_DEVICE_MASTER:
                    case MCU_DEVICE_SLAVE_1:
                        if (aStackInfo->stackPort1 != 0U) {
                            localPortNum = aStackInfo->stackPort1;
                            *aIsStackingPort = TRUE;
                        } else {
                            localPortNum = COMMS_INVALID_PORT;
                        }
                        break;
                    case MCU_DEVICE_SLAVE_2:
                        localPortNum = aPortNum - 18UL;
                        break;
                    default:
                        break;
                }
            } else {
                localPortNum = COMMS_INVALID_PORT;
            }
#endif
        }
    }

    return localPortNum;
}

uint32_t COMMS_ConvertToUnifiedPort(const MCU_ExtendedInfoType *const aStackingInfo,
                                  uint32_t aPortNum,
                                  uint32_t *const aIsStackingPort)
{
    uint32_t unifiedPortNum = aPortNum;

    *aIsStackingPort = FALSE;
    if (aPortNum > 8UL) {
        unifiedPortNum = COMMS_INVALID_PORT;
    } else {
        if (1U == aStackingInfo->stackingEn) {
            if ((aPortNum == aStackingInfo->stackPort0) ||
                    ((0UL != aStackingInfo->stackPort1) && (aPortNum == aStackingInfo->stackPort1))) {
                *aIsStackingPort = TRUE;
                unifiedPortNum = COMMS_INVALID_PORT;
            } else {
                switch (aStackingInfo->mstSlvMode) {
                    case MCU_DEVICE_SLAVE_1:
                        unifiedPortNum = aPortNum + 9UL;
                        break;
                    case MCU_DEVICE_SLAVE_2:
                        unifiedPortNum = aPortNum + 18UL;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return unifiedPortNum;
}

uint16_t COMMS_ConvertToUnifiedPortMask(const MCU_ExtendedInfoType *const aStackingInfo,
                                        uint16_t const aPortMask)
{
    uint16_t portMask = aPortMask;
    if (1U == aStackingInfo->stackingEn) {
        switch (aStackingInfo->mstSlvMode) {
            case MCU_DEVICE_MASTER:
                portMask = (aPortMask & COMMS_ETHERSWT_MASTER_PORT_MASK) &
                                COMMS_ETHERSWT_ALL_PORT_MASK;
                break;
            case MCU_DEVICE_SLAVE_1:
                portMask = ((aPortMask & COMMS_ETHERSWT_SLAVE_PORT_MASK) <<
                                        COMMS_ETHERSWT_SLAVE1_PORT_OFFSET) &
                                        COMMS_ETHERSWT_ALL_PORT_MASK;
                break;
            default:
                portMask = (uint16_t)COMMS_INVALID_PORT;
                break;
        }
    }
    return portMask;
}

static void COMMS_UpdateXcvrConfig(uint8_t mstSlvMode, uint32_t portNum, ETHXCVR_PortConfigType *const config)
{
    switch (portNum) {
        case 5U:
            config->id = 5U;
            config->bus.cntrlID = 3U;
            config->speed = ETHXCVR_SPEED_2500MBPS;
            break;
        case 6U:
            config->id = 6U;
            config->bus.cntrlID = 0U;
            config->speed = ETHXCVR_SPEED_5000MBPS;
            break;
        case 8U:
            config->id = 8U;
            config->bus.cntrlID = 0U;
            if (mstSlvMode == MCU_DEVICE_SLAVE_1) {
                config->speed = ETHXCVR_SPEED_5000MBPS;
            } else {
                config->speed = ETHXCVR_SPEED_2500MBPS;
            }
            break;
        default:
            break;
    }
}

void COMMS_InitXcvrStackingPorts(const MCU_ExtendedInfoType *const aStackInfo)
{
#ifdef __BCM89564G__
    ETHXCVR_PortConfigType config = {
        .id         = 0U,
        .portEnable = ETHXCVR_BOOLEAN_TRUE,
        .speed      = ETHXCVR_SPEED_1000MBPS,
        .autoNeg    = ETHXCVR_BOOLEAN_FALSE,
        .duplex     = ETHXCVR_DUPLEXMODE_FULL,
        .flowControl= ETHXCVR_FLOWCONTROL_NONE,
        .jumbo      = ETHXCVR_BOOLEAN_TRUE,
        .busMode    = ETHXCVR_BUSMODE_SGMII,
        .bus = {
            .cntrlID = 0U,
            .instID  = 0U,
            .driverParams = {{0U}},
        },
        .phy = {
            .phyMedia = ETHXCVR_PHYMEDIA_NONE,
            .driverParams = {{0U}},
        }
    };

    if (aStackInfo->stackingEn == 1U) {
        COMMS_UpdateXcvrConfig(aStackInfo->mstSlvMode, aStackInfo->stackPort0, &config);
        ETHXCVR_Init(config.id, &config);
        ETHXCVR_SetMode(config.id, ETHXCVR_MODE_ACTIVE);
        if (aStackInfo->stackPort1 != 0U) {
            COMMS_UpdateXcvrConfig(aStackInfo->mstSlvMode, aStackInfo->stackPort1, &config);
            ETHXCVR_Init(config.id, &config);
            ETHXCVR_SetMode(config.id, ETHXCVR_MODE_ACTIVE);
        }
    }
#endif
}

/** @} */
