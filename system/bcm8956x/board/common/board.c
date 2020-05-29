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
#include <stdint.h>
#include <chip_config.h>
#include <board.h>
#include <bcm_err.h>
#include <utils.h>
#include <osil/log_osil.h>
#include <mcu.h>
#include <clk.h>
#include <ulog.h>
#include <flash.h>
#include <gpio.h>
#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
#include <ipc_osil.h>
#include <mcu_ext.h>
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
#ifdef ENABLE_PTM
#include <imgl.h>
#include <ptm_osil.h>
#endif
#if defined(ENABLE_NIF)
#include <nif.h>
#endif
#if defined(ENABLE_ETS) || defined(ENABLE_ETS_TEST)
#include <ets_osil.h>
#endif
#if defined(ENABLE_ETH_BRPHY)
#include <brphy_osil.h>
#endif
#if defined(ENABLE_ETH_SGMII)
#include <sgmii_osil.h>
#endif
#include <dummyphy_osil.h>

#define ETHERCFG_RX_MAX_FRM_SIZE    (1522UL)

#define BRPHY_PORTS     (4UL)

#define FREQ_MHZ(x)         (1000000UL * (x))

#if defined(ENABLE_SHELL) && !defined(ENABLE_UART_TEST)
extern void UARTConsole_UARTRxCb(char *aData, uint32_t aSize);
#endif

#if defined(ENABLE_UART_TEST)
extern uint32_t Test_UARTTxCb(char *const aData, uint32_t aSize);
extern void Test_UARTRxCb(char *aData, uint32_t aSize);
#endif

#if defined(ENABLE_WATCHDOG_SP805)
const WDT_CfgType WDT_Config[] = {
    {
        WDT_DISABLE_ALLOWED,
        WDT_MODE_OFF,
        10000UL, /* 10seconds */
    },
};
#endif

#if defined(ENABLE_MAC0)
const uint8_t DefaultMacAddr[] = {0x02, 0x01, 0x02, 0x03, 0x04, 0x05};
#elif defined(ENABLE_MAC1)
const uint8_t DefaultMacAddr[] = {0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
#else
const uint8_t DefaultMacAddr[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

#if defined(ENABLE_ETH)
#if !defined(ENABLE_ETHER_TEST)
static const ETHER_CfgType ETHER_Config = {
    .hwID = ETHER_HW_ID_0,
    .speed = ETHER_SPEED_1000MBPS,
    .duplexMode = ETHER_DUPLEX_MODE_FULL,
    .maxRxFrmLen = ETHERCFG_RX_MAX_FRM_SIZE,
    .macAddr = DefaultMacAddr,
    .prioChanMap[0] = 0UL,
    .prioChanMap[1] = 0UL,
    .prioChanMap[2] = 0UL,
    .prioChanMap[3] = 0UL,
    .prioChanMap[4] = 0UL,
    .prioChanMap[5] = 0UL,
    .prioChanMap[6] = 0UL,
    .prioChanMap[7] = 0UL,
};

const ETHER_CfgType  *ETHER_Cfg = &ETHER_Config;

#endif /* if !defined(ENABLE_ETHER_TEST) */
#endif /* if defined(ENABLE_ETH) */

#if defined(ENABLE_ETH_SWITCH)
const ETHERSWT_CfgType SwitchConfigConst = {
    .portCfgList = {
#if (SGMII_MAX_HW_ID > 1UL)
        {
            .portID = 0UL,
            .xcvrID = 0UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (BRPHY_PORTS > 0UL)
        {
            .portID = 1UL,
            .xcvrID = 1UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (BRPHY_PORTS > 1UL)
        {
            .portID = 2UL,
            .xcvrID = 2UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (BRPHY_PORTS > 2UL)
        {
            .portID = 3UL,
            .xcvrID = 3UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (BRPHY_PORTS > 3UL)
        {
            .portID = 4UL,
            .xcvrID = 3UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (BRPHY_PORTS > 4UL)
#error "BRPHY_PORTS > 4 not supported"
#endif
#if (SGMII_MAX_HW_ID > 3UL)
        {
            .portID = 5UL,
            .xcvrID = 5UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (SGMII_MAX_HW_ID > 0UL)
        {
            .portID = 6UL,
            .xcvrID = 6UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (SGMII_MAX_HW_ID > 4UL)
        {
            .portID = 8UL,
            .xcvrID = 8UL,
            .enableTimeStamp = TRUE,
            .enableJumboFrm = FALSE,
            .role = ETHERSWT_STANDARD_PORT,
            .fixedMacAddrList = {{0}},
            .macAddrListSz = 0UL,
            .vlanMemList[0] =
            {
                .vlanID = 1U,
                .macAddrList = 0x0000UL,
                .defaultPri = ETHERSWT_PCP_0,
                .forward = ETHERSWT_VLAN_FRWRD_UNTAGGED,
            },
            .vlanMemListSz = 1UL,
            .ingressCfg =
            {
                .defaultVLAN = 1UL,
                .defaultPrio = ETHERSWT_PCP_0,
                .dropUntagged = FALSE,
                .tc = ETHERSWT_TC_INVALID,
                .policerEn = FALSE,
                .pcp2tcMap = {ETHERSWT_TC_0, ETHERSWT_TC_1, ETHERSWT_TC_2,
                    ETHERSWT_TC_3, ETHERSWT_TC_4, ETHERSWT_TC_5,
                    ETHERSWT_TC_6, ETHERSWT_TC_7}
            },
            .egressCfg =
            {
                .fifoList = {
                    {
                        .id = 0UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_0,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 1UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_1,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 2UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_2,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 3UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_3,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 4UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_4,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 5UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_5,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 6UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_6,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                    {
                        .id = 7UL,
                        .minLen = 0UL,
                        .tc = ETHERSWT_TC_7,
                        .shaper = {
                            .rateBps = 0UL,
                            .burstBytes = 0UL,
                            .avbShapingModeEn = FALSE,
                        }
                    },
                },
                .fifoListSz  = 8UL,
                .scheduler = {
                    .algo = {
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                        ETHERSWT_SCHED_ALGO_SP, ETHERSWT_SCHED_ALGO_SP,
                    },
                },
                .pcpRemarkEn = TRUE,
                .tc2pcpMap = {
                    ETHERSWT_PCP_1, ETHERSWT_PCP_0, ETHERSWT_PCP_4,
                    ETHERSWT_PCP_5, ETHERSWT_PCP_6, ETHERSWT_PCP_7,
                    ETHERSWT_PCP_2, ETHERSWT_PCP_3
                },
            },
        },
#endif
#if (SGMII_MAX_HW_ID > 5UL)
#error "SGMII_MAX_HW_ID > 5 not supported"
#endif
    },
    .portCfgListSz = BRPHY_PORTS + SGMII_MAX_HW_ID - 1,
    .switchType = ETHERSWT_SWITCH_STANDARD,
};
#endif

#if defined(ENABLE_NIF)
const NIF_CbTblType NIF_CtrlCbTbl[] = {
#if defined(ENABLE_ETS) || defined(ENABLE_ETS_TEST)
    {
        .clientID = NIF_CLIENT_ID(BCM_ETS_ID, 0U),
        .etherType = ETHER_ETHERTYPE_GPTP,
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
        .rxCb = ETS_RxPktIndication,
        .txCb = ETS_TxPktConfirmation,
        .rxTSCb = ETS_RxPktTSIndication,
        .txTSCb = ETS_TxPktTSIndication,
        .rxMgmtInfoCb = ETS_RxMgmtInfoIndication,
        .linkStateChgCb = NULL,
        .bufAvailCb = NULL,
    },
#endif
};

static const NIF_EthIntfType NIF_EthIntfTbl[] = {
    {
        .getTxBuf = ETHER_GetTxBuffer,
        .sendTxBuf = ETHER_Send,
        .getTime = ETHER_GetTime,
        .getRxTS = ETHER_GetRxTimestamp,
        .getTxTs = ETHER_GetTxTimestamp,
        .enableTs = ETHER_EnableTxTimestamp,
        .getMode = ETHER_GetMode,
        .setTime = ETHER_SetTime,
        .getMacAddr = ETHER_GetMacAddr,
        .setMode = ETHER_SetMode,
        .setCorrectionTime = ETHER_SetCorrectionTime,
        .setGPTimer = ETHER_SetGPTimer,
    },
};

static const NIF_EthIntfType NIF_WLEthIntfTbl[] = {
};

static const NIF_XcvrIntfType NIF_XcvrIntfTbl[] = {
#if (ETHXCVR_HW_ID_MAX > 0UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 1UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 2UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 3UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 4UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 5UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 6UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
#if (ETHXCVR_HW_ID_MAX > 7UL)
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
#endif
};

static const NIF_XcvrIntfType NIF_WLXcvrIntfTbl[] = {
};

static const NIF_SwtIntfType NIF_SwtIntfTbl[] = {
#ifdef ENABLE_ETH_SWITCH
    {
        .enableTxTs = ETHERSWT_EnableTxTimestamp,
    },
#endif
};

static const NIF_ClientIDType NIF_ClientStreamsTbl[] = {};

static const ETHERSWT_VLANIDType NIF_CtrlVlanArray[] = {};

const NIF_CtrlCfgType NIF_CtrlConfig = {
    .hwIdx = 0UL,
    .vlanArray = NIF_CtrlVlanArray,
    .vlanArraySize = sizeof(NIF_CtrlVlanArray)/sizeof(ETHERSWT_VLANIDType),
    .cbTbl = NIF_CtrlCbTbl,
    .cbTblSize = sizeof(NIF_CtrlCbTbl)/sizeof(NIF_CbTblType),
    .streamTbl = NIF_ClientStreamsTbl,
    .streamTblSize = sizeof(NIF_ClientStreamsTbl)/sizeof(NIF_ClientIDType),
    /* Ethernet driver interface function table */
    .ethIntfTbl = NIF_EthIntfTbl,
    .ethIntfTblSize = sizeof(NIF_EthIntfTbl)/sizeof(NIF_EthIntfType),
    /* Wireless ethernet driver interface function table */
    .wlEthIntfTbl = NIF_WLEthIntfTbl,
    .wlEthIntfTblSize = sizeof(NIF_WLEthIntfTbl)/sizeof(NIF_EthIntfType),
    /* XCVR interface function table */
    .xcvrIntfTbl = NIF_XcvrIntfTbl,
    .xcvrIntfTblSize = sizeof(NIF_XcvrIntfTbl)/sizeof(NIF_XcvrIntfType),
    /* Wireless XCVR interface function table */
    .wlXcvrIntfTbl = NIF_WLXcvrIntfTbl,
    .wlXcvrIntfTblSize = sizeof(NIF_WLXcvrIntfTbl)/sizeof(NIF_XcvrIntfType),
    /* Switch interface function table */
    .swtIntfTbl = NIF_SwtIntfTbl,
    .swtIntfTblSize = sizeof(NIF_SwtIntfTbl)/sizeof(NIF_SwtIntfType),
};

const NIF_CfgType NIF_Config = {
    .ctrlCfg = &NIF_CtrlConfig,
};
#endif /* if defined(ENABLE_NIF) */

static const MCU_ClkCfgType ClkCfgTbl[] = {
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ,   /* QSPI @ 50MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(50),
            .cntrl = {[0] = 0x3, [1] = 0x7},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ,   /* QSPI @ 25MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(25),
            .cntrl = {[0] = 0x2, [1] = 0x7},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_MDIO,   /* MDIO @ 100MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_MDIO,
            .freq = FREQ_MHZ(100),
        },
    },
};

static const MCU_ConfigType MCU_Config = {
    .clkSrcFailNotification = MCU_CLK_SRC_NOTIFICATION_EN,
    .clkCfgTbl = ClkCfgTbl,
    .clkCfgTblSize = (sizeof(ClkCfgTbl) / sizeof(MCU_ClkCfgType)),
    .ramCfgTbl = NULL,
    .ramCfgTblSize = 0UL,
};

const MCU_ConfigType *mcu_cfg_table = &MCU_Config;

#if !defined(ENABLE_UART_TEST)

#include <uart_osil.h>
#include <uconsole_osil.h>
/**
 * UART configuration
 */
const UART_ConfigType UART_Config[] = {
    {
        .txFifoLvl = UART_FIFO_LVL_1DIV8,
        .rxFifoLvl = UART_FIFO_LVL_7DIV8,
        .baud = UART_BAUD_115200,
        .stopBits = UART_STOP_BITS1,
        .parity = UART_PARITY_NONE,
        .loopBackMode = 0UL,
        .txCb = UCONSOLE_TxConfirmation,
        .rxCb = UCONSOLE_RcvIndication,
        .errCb = UCONSOLE_ErrIndication,
    },
};
#endif

#if defined(ENABLE_FLASH) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)
/* Board flash configurations */
static const FLASH_FlsableSecType flasableSecList[] = {
    {FLASH0_FLSABLE_SEC_START_ADDR, 32UL}
};

static const FLASH_CfgTblType flash_cfg_tbl[] = {
    {
        .hwID = FLASH_HW_ID_0,
        .config = {
            .size = FLASH0_SIZE, /* 4MB */
            .pageSize = 256UL,
            .sectorSize = (64UL * 1024UL), /* 64KB */
            .subSectorSize = FLASH0_SUBSECTOR_SIZE,
            .SPIMode = FLASH_SPI_MODE_3,
            .speed = FLASH0_SPEED,
            .readLane = FLASH0_READ_LANE_CFG,
            .flsableSecListSize = (sizeof(flasableSecList)/
                                    sizeof(FLASH_FlsableSecType)),
            .flsableSecList = flasableSecList,
            .flsID = FLASH0_FLASH_ID,
        },
    },
};

const FLASH_CfgTblType  *flash_cfg_table = flash_cfg_tbl;
const uint32_t flash_cfg_table_sz = (sizeof(flash_cfg_tbl)/
                                    sizeof(FLASH_CfgTblType));

const MCU_ClkCfgIDType flash_clk_tbl[] = {
    FLASH0_CLK_CFG_ID,
};

const MCU_ClkCfgIDType *flash_clk_table = flash_clk_tbl;
const uint32_t flash_clk_table_sz = (sizeof(flash_clk_tbl) /
                                    sizeof(MCU_ClkCfgIDType));
#endif

/* Board flash manager configurations */
#if defined(ENABLE_FLSMGR) && !defined(ENABLE_FLSMGR_TEST) && !defined(ENABLE_FLASH_TEST)
static const FLSMGR_CfgType flsmgr_cfg_tbl[] = {
    {.flashCfg = &flash_cfg_tbl[0]},
};

const FLSMGR_CfgType  * const flsmgr_cfg_table = flsmgr_cfg_tbl;
const uint32_t flsmgr_cfg_table_sz = (sizeof(flsmgr_cfg_tbl)/
                                    sizeof(FLSMGR_CfgType));
#endif

#ifdef ENABLE_PTM
static const PTM_LookupTblEntType LookupTbl[] = {
    {
        .flashID = FLASH_HW_ID_0,
        .flashAddr = PT_LOOKUP_FLASH_ADDR,
    },
};

static const PTM_CfgType PTM_CfgTbl = {
    .ptLoadAddr = NULL,
    .lookupTbl = LookupTbl,
    .lookupTblSize = (sizeof(LookupTbl)/sizeof(PTM_LookupTblEntType)),
};

const PTM_CfgType *PTM_Cfg = &PTM_CfgTbl;
#endif /* ENABLE_PTM */

#ifdef ENABLE_IPC
#ifdef __BCM89564G__
extern const IPC_BusFnTblType IPC_SpiFnTbl;
#endif

const IPC_ChannCfgType IPC_ChannCfg[IPC_MAX_CHANNELS] = {
    {
        .ID = 0UL,
        .mode = IPC_CHANN_MODE_SLAVE,
        .sizeLog2 = 9U,
        .cntLog2 = 3U,
        .busInfo = {
            .hwID = 0UL,
            .busType = IPC_BUS_MEMMAP,
            .slaveID = 0UL,
            .fnTbl = NULL,
        },
    },
#ifdef __BCM89564G__
    {
        .ID = 1UL,
        .mode = IPC_CHANN_MODE_MASTER,
        .sizeLog2 = 9U,
        .cntLog2 = 3U,
        .busInfo = {
            .hwID = 1UL,
            .busType = IPC_BUS_SPI,
            .slaveID = 0UL,
            .fnTbl = &IPC_SpiFnTbl,
        },
    },
#endif
};
#endif

#if !defined(ENABLE_PINMUX_TEST) && !defined(ENABLE_GPIO_TEST)

#ifdef ENABLE_IIC_BSC
static const PINMUX_PinModeCfgType IIC0PinModeCfg[] = {
    {PINMUX_PIN_MODE_IIC, 0UL},
};
#endif

static const PINMUX_PinModeCfgType GPIOPinModeCfg[] = {
    {PINMUX_PIN_MODE_GPIO, 0UL},
};

static const PINMUX_PinCfgType GIO0PinMuxPins[] = {
#ifdef ENABLE_IIC_BSC
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_0, PINMUX_PIN_MODE_IIC, 0U, 1UL, &IIC0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* IIC-0 SCL */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_1, PINMUX_PIN_MODE_IIC, 0U, 1UL, &IIC0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif
#ifdef ENABLE_IPC
    {PINMUX_PIN_DIRECTION_OUT, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_4, PINMUX_PIN_MODE_GPIO,
     PINMUX_PIN_LEVEL_HIGH, 1UL, &GPIOPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_5, PINMUX_PIN_MODE_GPIO, 0U, 1UL, &GPIOPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
    {PINMUX_PIN_DIRECTION_IN, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_GPIO, PINMUX_PIN_LEVEL_LOW, 1UL, &GPIOPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE, PINMUX_PIN_INTR_TRIG_DUAL_EDG},
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
};

const PINMUX_ConfigType PINMUX_Config[] = {
    {
        sizeof(GIO0PinMuxPins) / sizeof(PINMUX_PinCfgType),
        &GIO0PinMuxPins[0],
    },
};

#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
void BOARD_GpioIrqHandlerCb(GPIO_ChannelType aChannelId)
{
    if (GPIO_CHANNEL_6 == aChannelId) {
        IPC_IRQChann1Handler();
    }
}
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */

const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS] = {
    NULL,                   /* Channel 0 */
    NULL,                   /* Channel 1 */
    NULL,                   /* Channel 2 */
    NULL,                   /* Channel 3 */
    NULL,                   /* Channel 4 */
    NULL,                   /* Channel 5 */
#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
    BOARD_GpioIrqHandlerCb, /* Channel 6 */
#else /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
    NULL,                   /* Channel 6 */
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
    NULL,                   /* Channel 7 */
    NULL,                   /* Channel 8 */
};
#endif /* defined(ENABLE_PINMUX_TEST) && !defined(ENABLE_GPIO_TEST) */

#if defined(ENABLE_ETH)
int32_t ETHXCVR_BoardGetPhyFnTbl(const ETHXCVR_PortConfigType *const aConfig,
                                 const ETHXCVR_FuncTblType **const aPhyFnTbl)
{
    *aPhyFnTbl = NULL;
    return BCM_ERR_OK;
}

#if !defined(ENABLE_ETHER_TEST)
const ETHER_TimeCfgType ETHER_TimeConfig[] = {
    {
        .hwIdx = 0UL,
        .portEnableMask = P1588_PORT_MASK,
        .sync = ETHER_TIMESYNC_DEFAULT,
    },
};

const ETHER_TimeCfgType *const ETHER_TimeCfg = ETHER_TimeConfig;
const uint32_t ETHER_TimeCfgSize = sizeof(ETHER_TimeConfig)/
                                    sizeof(ETHER_TimeCfgType);

#endif /* if !defined(ENABLE_ETHER_TEST) */
#endif /* if defined(ENABLE_ETH) */

void Board_Init(void)
{
#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
    int32_t ret;
    MCU_ExtendedInfoType stackingInfo;
    ret = MCU_GetExtendedInfo(&stackingInfo);
    if ((BCM_ERR_OK == ret) && (TRUE == stackingInfo.stackingEn) &&
        (MCU_DEVICE_MASTER == stackingInfo.mstSlvMode)) {
        GPIO_EnableInterrupt(GPIO_CHANNEL_6);   /* GPIO_2 in 89564g */
    }
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
}

