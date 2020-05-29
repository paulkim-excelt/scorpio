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
/******************************************************************************
 File Name: comms_osil.c
 Descritpion: This file implements common function for communication devices.
******************************************************************************/
#if defined(ENABLE_CAN_MCAN)
#include <osil/can_osil.h>
#endif /* ENABLE_CAN_MCAN */
#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#include <osil/ethxcvr_osil.h>
#endif /* ENABLE_ETH */
#include <osil/comms_osil.h>
#ifdef ENABLE_INET
#include <osil/inet_osil.h>
#endif
#ifdef ENABLE_ETH_SWITCH
#include <osil/eth_switch_osil.h>
#include <comms_ipc.h>
#endif
#ifdef ENABLE_NIF
#include <nif.h>
#endif
#include <msg_queue.h>
#include <bcm_utils.h>
#include "ee.h"

#if defined(ENABLE_AVT)
#include <avt.h>
#endif
#include <board.h>
#include <syscfg.h>
#include <ulog.h>
#include <utils.h>
#include <init.h>
#include <stdint.h>
#include <ee.h>
#include <imgl.h>
#include <ptm.h>
#include <utils.h>
#include <bcm_err.h>
#ifdef ENABLE_ETH_SWITCH
#include <eth_switch.h>
#ifdef ENABLE_CFP
#include <cfp.h>
#endif
#endif
#if defined(ENABLE_NIF)
#include <nif.h>
#endif
#include <ipc.h>
#include <rpc_async.h>
#include <comms_ipc.h>
#include "ee.h"
#if defined(ENABLE_OTP)
#include <otp.h>
#endif
#include <chip_config.h>
#include <osil/bcm_osil.h>
#if defined(ENABLE_ETH_SWITCH)
#include <comms_platform.h>
#endif

#if defined(ENABLE_ETS)
#include <ets_ipc.h>
#endif

#include <system.h>
#include <comms.h>

/**
    @name COMMS Design IDs
    @{
    @brief COMMS Design IDs
*/
#define BRCM_SWDSGN_COMMS_RECVRESP_PROC                    (0x80) /**< @brief #COMMS_RecvResp */
#define BRCM_SWDSGN_COMMS_CMDSENDCMD_PROC                  (0x81) /**< @brief #COMMS_SendCmd */
#define BRCM_SWDSGN_COMMS_GETCMDSTATUS_PROC                (0x82) /**< @brief #COMMS_GetCmdStatus */
#define BRCM_SWDSGN_COMMS_PROCESSCOMMANDS_PROC             (0x83) /**< @brief #COMMS_ProcessCommands */
#define BRCM_SWDSGN_COMMS_INIT_PROC                        (0x84) /**< @brief #COMMS_Init */
#define BRCM_SWDSGN_COMMS_GETSTACKINGINFO_PART_PROC        (0x85) /**< @brief #COMMS_GetStackingInfo */
#define BRCM_SWDSGN_COMMS_LOADXCVRIMAGEANDINIT_PART_PROC   (0x86) /**< @brief #COMMS_LoadXcvrImageAndInit */
#define BRCM_SWDSGN_COMMS_XCVRINIT_PART_PROC               (0x87) /**< @brief #COMMS_XcvrInit */
#define BRCM_SWDSGN_COMMS_LOADCOMMSIMAGEANDINIT_PART_PROC  (0x88) /**< @brief #COMMS_LoadCommsImageAndInit */
#define BRCM_SWDSGN_COMMS_SWITCHINIT_PART_PROC             (0x89) /**< @brief #COMMS_SwitchInit */
#define BRCM_SWDSGN_COMMS_ADDTOCMDQ_PROC                   (0x8A) /**< @brief #COMMS_AddToCmdQ */

/** @} */

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define SYS_CFG_EVENT           (SystemEvent1)
#define COMMS_CFG_LOAD_EVENT    (SystemEvent1)
#define CFP_CFG_LOAD_EVENT      (SystemEvent1)
#define ETS_MSG_EVENT           (SystemEvent10)

#if defined(ENABLE_ETH)
#if defined(ENABLE_AVT)
uint64_t sys1588TimeDiff_ns = 0ULL;
#else
uint32_t COMMS_EtherTimeInit0Done = FALSE;
#endif
#endif

/**
    @code{.unparsed}
    BCM_ReportError();
    @endcode
*/
static void COMMS_ErrorReport(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                              uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_COM_ID & BCM_LOGMASK_USER), 0U, aApiID,
                    aErr, 4UL, values);
}

void COMMS_GetMacAddr(uint8_t *const aMacAddr)
{
    uint32_t data1 = 0UL;
    uint32_t data2 = 0UL;
    uint32_t isMacAddrValid = FALSE;

#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
    int32_t retVal = BCM_ERR_OK;

    /* Read MAC address from OTP memory */
    /* Read MAC address @ location 0 */
    retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_0_OCTET123_ADDR, &data1);
    if (BCM_ERR_OK == retVal) {
        retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_0_OCTET456_ADDR, &data2);
        if (BCM_ERR_OK == retVal) {
            if (!((0UL == data1) && (0UL == data2))) {
                isMacAddrValid = TRUE;
                ULOG_ERR("%s:Applying MAC address from OTP location-0\n",
                        __func__);
            }
        }
    }

    if (FALSE == isMacAddrValid) {
        /* Read MAC address copy @ location 1 */
        retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_1_OCTET123_ADDR, &data1);
        if (BCM_ERR_OK == retVal) {
            retVal = OTP_Read(OTP_HW_ID_0, OTP_MAC_ADDR_1_OCTET456_ADDR, &data2);
            if (BCM_ERR_OK == retVal) {
                if (!((0UL == data1) && (0UL == data2))) {
                    isMacAddrValid = TRUE;
                    ULOG_ERR("%s:Applying MAC address from OTP location-1\n",
                            __func__);
                }
            }
        }
    }
#endif /* ENABLE_OTP */
    if (TRUE == isMacAddrValid) {
        aMacAddr[0] = (uint8_t)((data1 >> 16UL) & 0xFFUL);
        aMacAddr[1] = (uint8_t)((data1 >> 8UL) & 0xFFUL);
        aMacAddr[2] = (uint8_t)(data1 & 0xFFUL);
        aMacAddr[3] = (uint8_t)((data2 >> 16UL) & 0xFFUL);
        aMacAddr[4] = (uint8_t)((data2 >> 8UL) & 0xFFUL);
        aMacAddr[5] = (uint8_t)(data2 & 0xFFUL);
    } else {
        /* Read default MAC address */
        BCM_MemCpy(aMacAddr, DefaultMacAddr, 6UL);
        ULOG_ERR("%s:Applying default MAC address\n", __func__);
    }

    ULOG_ERR("Configuring MAC address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            (uint32_t)aMacAddr[0], (uint32_t)aMacAddr[1], (uint32_t)aMacAddr[2],
            (uint32_t)aMacAddr[3], (uint32_t)aMacAddr[4], (uint32_t)aMacAddr[5]);

    return;
}

#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST) && defined(ENABLE_ETH_SWITCH)
static void COMMS_GetStackingInfo(MCU_ExtendedInfoType *const aStackingInfo)
{
     int32_t retVal = MCU_GetExtendedInfo(aStackingInfo);
     if (BCM_ERR_OK != retVal) {
         COMMS_ErrorReport(BRCM_SWDSGN_COMMS_GETSTACKINGINFO_PART_PROC, retVal,
                 (uint32_t)__LINE__, 0UL, 0UL, 0UL);
     }
}
#endif

#if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST)
void ETHXCVR_ModeIndication(ETHXCVR_IDType aID, ETHXCVR_ModeType aMode)
{
}

static void COMMS_XcvrInit(const ETHXCVR_ConfigType *const aConfig)
{
    uint32_t               i;
    int32_t                ret;
    uint32_t               portNum;
#ifdef ENABLE_ETH_SWITCH
    uint32_t               isStackingPort = FALSE;
    ETHXCVR_PortConfigType portCfg;
    MCU_ExtendedInfoType   stackingInfo = {0U};

#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
    COMMS_GetStackingInfo(&stackingInfo);
#endif
#endif

    for (i = 0UL; i < aConfig->numPorts; i++) {
#ifdef ENABLE_ETH_SWITCH
        portNum = COMMS_ConvertToLocalPort(&stackingInfo, aConfig->port[i].id, &isStackingPort);

        if ((COMMS_INVALID_PORT != portNum) && (FALSE == isStackingPort)) {
            BCM_MemCpy(&portCfg, &aConfig->port[i], sizeof(ETHXCVR_PortConfigType));
            portCfg.id = portNum;
            ETHXCVR_Init(portNum, &portCfg);
#ifdef __BCM8956X__
            if ((aConfig->port[i].busMode == ETHXCVR_BUSMODE_SGMII) &&
                    (aConfig->port[i].bus.cntrlID == 3U)) {
                ret = ETHXCVR_SetMode(portNum,
                        (ETHXCVR_BOOLEAN_TRUE == aConfig->port[i].portEnable) ?
                        ETHXCVR_MODE_ACTIVE: ETHXCVR_MODE_DOWN);
                if (BCM_ERR_OK != ret) {
                    ULOG_ERR("ETHXCVR_SetMode failed(%d) for port %u!!\n", ret, portNum);
                }
            }
#endif
        }
#else
        ETHXCVR_Init(aConfig->port[i].id, &aConfig->port[i]);
#endif
    }

    for (i = 0UL; i < aConfig->numPorts; i++) {
#ifdef ENABLE_ETH_SWITCH
        portNum = COMMS_ConvertToLocalPort(&stackingInfo, aConfig->port[i].id, &isStackingPort);
#ifdef __BCM8956X__
        if ((COMMS_INVALID_PORT != portNum) &&
            (FALSE == isStackingPort) &&
            (aConfig->port[i].busMode == ETHXCVR_BUSMODE_SGMII) &&
            (aConfig->port[i].bus.cntrlID != 3U)) {
#else
        if ((COMMS_INVALID_PORT != portNum) &&
            (FALSE == isStackingPort)) {
#endif
#else
            portNum = aConfig->port[i].id;
#endif
            ret = ETHXCVR_SetMode(portNum,
                    (ETHXCVR_BOOLEAN_TRUE == aConfig->port[i].portEnable) ?
                    ETHXCVR_MODE_ACTIVE: ETHXCVR_MODE_DOWN);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("ETHXCVR_SetMode failed(%d) for port %u!!\n", ret, portNum);
            }
#ifdef ENABLE_ETH_SWITCH
        }
#endif
    }

#ifdef ENABLE_ETH_SWITCH
    COMMS_InitXcvrStackingPorts(&stackingInfo);
#endif
    /*FIXME: Need to block here waiting for the ETHXCVR Mode indication */
}

#if defined(ENABLE_PTM) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)
ETHXCVR_ConfigType    XcvrConfig;
static void COMMS_LoadXcvrImageAndInit()
{
    int32_t               ret;
    int32_t               status;
    IMGL_LoadStatusType   imglStatus;

    /* Read XCVR config from flash */
    BCM_MemSet(&XcvrConfig, 0x0U, sizeof(ETHXCVR_ConfigType));
    ret = IMGL_LoadImg(0x9000U, (uint8_t *)&XcvrConfig, 0UL,
                    sizeof(ETHXCVR_ConfigType), SYS_CFG_EVENT, &imglStatus);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("COMMS_LoadXcvrImageAndInit: IMGL_LoadImg err:%d for XcvrConfig\n", ret);
    } else {
        (void)BCM_WaitEvent(SYS_CFG_EVENT);
        (void)BCM_ClearEvent(SYS_CFG_EVENT);
        status = IMGL_GetStatus( &imglStatus);
        if (BCM_ERR_OK != status) {
            ULOG_ERR("COMMS_LoadXcvrImageAndInit: PTM error: %d\n", status);
        } else {
            COMMS_XcvrInit(&XcvrConfig);
        }
    }
}
#endif
#endif

#if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST)
static void COMMS_InitEthernet(void)
{
    int32_t            retVal;
    ETHER_CfgType      ethCfg;
    uint8_t            macAddr[6UL];

    BCM_MemCpy(&ethCfg, ETHER_Cfg, sizeof(ETHER_CfgType));

    /* Retrieve MAC address */
    COMMS_GetMacAddr(macAddr);
    ethCfg.macAddr = macAddr;

    ETHER_Init(ethCfg.hwID, &ethCfg);

    retVal = ETHER_SetMode(ETHER_Cfg->hwID, ETHER_MODE_ACTIVE);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ETHER_SetMode failed(%d)!!\n", retVal);
    }

}
#endif

#if defined(ENABLE_ETH_SWITCH)
ETHERSWT_CfgType SwitchCfgUsrData;
COMMS_ConfigType CommsConfig;

#if defined(ENABLE_CFP)
void COMMS_StreamMonitorAlarmCb(void)
{
    (void)SetEvent(CommsTask, COMMS_STREAM_MONITOR_EVENT);
}
#endif

void COMMS_SwitchInit(const MCU_ExtendedInfoType *const aStackingInfo,
                      const COMMS_ConfigType *const aCommsCfg,
                      ETHERSWT_CfgType *const aSwitchCfg)
{
#if !defined(ENABLE_ETH_SWITCH_TEST)
    if (NULL != aCommsCfg) {
        COMMS_GetSwitchConfig(aStackingInfo, aCommsCfg, aSwitchCfg);
    }
    ETHERSWT_Init(0UL, aSwitchCfg);
    ETHERSWT_MgmtInit(0UL);
    if (NULL != aCommsCfg) {
        COMMS_AddSwitchEntries(aStackingInfo, aCommsCfg);
    }
#endif
}

void COMMS_LoadCommsImageAndInit(const MCU_ExtendedInfoType *const aStackingInfo)
{
    int32_t ret = BCM_ERR_UNKNOWN;
    BCM_EventMaskType mask;
    IMGL_LoadStatusType imglStatus;
    int32_t status;

#if defined(ENABLE_PTM) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)
    uint32_t lSize = 0UL;
    const MSGQ_MsgHdrType *pHdr = NULL;

    ret = PTM_LoadUserData(BRCM_COMMS_CONFIG, (PTM_AddrType)&CommsConfig, sizeof(CommsConfig),
            COMMS_CFG_LOAD_EVENT, &pHdr);

    if (ret == BCM_ERR_OK) {
        BCM_WaitEvent(COMMS_CFG_LOAD_EVENT);
        BCM_GetEvent(CommsTask, &mask);
        if ((mask & COMMS_CFG_LOAD_EVENT) == COMMS_CFG_LOAD_EVENT) {
            (void)BCM_ClearEvent(COMMS_CFG_LOAD_EVENT);
        }
        status = PTM_GetStatus(pHdr, &lSize);
        if (status != BCM_ERR_OK) {
            ULOG_ERR("COMMS_LoadCommsImageAndInit: comms config user image not found: %d\n", status);
            ret = BCM_ERR_UNKNOWN;
        }
    }
#endif
    if (ret != BCM_ERR_OK) {
        ret = IMGL_LoadImg(BRCM_COMMS_CONFIG, (PTM_AddrType)&CommsConfig, 0UL, sizeof(CommsConfig),
                           COMMS_CFG_LOAD_EVENT, &imglStatus);

        if (ret == BCM_ERR_OK) {
            BCM_WaitEvent(COMMS_CFG_LOAD_EVENT);
            BCM_GetEvent(CommsTask, &mask);
            if ((mask & COMMS_CFG_LOAD_EVENT) == COMMS_CFG_LOAD_EVENT) {
                (void)BCM_ClearEvent(COMMS_CFG_LOAD_EVENT);
            }
            status = IMGL_GetStatus(&imglStatus);
            if (status != BCM_ERR_OK) {
                ULOG_ERR("COMMS_LoadCommsImageAndInit: comms config image not found: %d\n", status);
                ret = BCM_ERR_UNKNOWN;
            }
        }
    }

    if (ret != BCM_ERR_OK) {
        /* use the const table */
        ULOG_ERR("COMMS_LoadCommsImageAndInit: Using configuration from board.c\n");
        BCM_MemCpy(&SwitchCfgUsrData, &SwitchConfigConst, sizeof(ETHERSWT_CfgType));
        COMMS_SwitchInit(aStackingInfo, NULL, &SwitchCfgUsrData);
    } else {
        if (CommsConfig.magicId == COMMS_CONFIG_MAGIC_ID) {
            COMMS_SwitchInit(aStackingInfo, &CommsConfig, &SwitchCfgUsrData);
        } else {
            ULOG_ERR("COMMS_LoadCommsImageAndInit: comms config image has invalid magic: 0x%x\n", CommsConfig.magicId);
        }
    }

}
#endif

#if defined(ENABLE_ETH_SWITCH) && defined(ENABLE_CFP) && !defined (ENABLE_CFP_TEST)
CFP_ConfigType CFPConfig;
void COMMS_CFPInit(const MCU_ExtendedInfoType *const aStackingInfo)
{
    int32_t ret = BCM_ERR_UNKNOWN;

#if defined(ENABLE_PTM)
    BCM_EventMaskType mask;
    int32_t status;
    IMGL_LoadStatusType imglStatus;

    ret = IMGL_LoadImg(BRCM_CFP_CONFIG, (PTM_AddrType)&CFPConfig, 0UL,
                      sizeof(CFPConfig), CFP_CFG_LOAD_EVENT, &imglStatus);

    if (ret == BCM_ERR_OK) {
        BCM_WaitEvent(CFP_CFG_LOAD_EVENT);
        BCM_GetEvent(CommsTask, &mask);
        if ((mask & CFP_CFG_LOAD_EVENT) == CFP_CFG_LOAD_EVENT) {
            BCM_ClearEvent(CFP_CFG_LOAD_EVENT);
        }
        status = IMGL_GetStatus(&imglStatus);
        if (status != BCM_ERR_OK) {
            ULOG_ERR("COMMS_CFPInit: CFP config image not found, status = %d\n", status);
            ret = BCM_ERR_UNKNOWN;
        }
    } else {
        ULOG_ERR("COMMS_CFPInit: CFP load image failed with error %d\n", ret);
    }
#endif

    if (ret == BCM_ERR_OK) {
        COMMS_UpdateCFPConfig(aStackingInfo, &CFPConfig);
        ret = CFP_Init(0UL, &CFPConfig);
        if (ret != BCM_ERR_OK) {
            ULOG_ERR("COMMS_CFPInit: CFP initialization failed with error %d\n", ret);
        }
    }
}
#endif

#if defined(ENABLE_NIF) && !defined(ENABLE_ETHER_TEST)
static void COMMS_InitNif(void)
{
    int32_t ret = NIF_Init(&NIF_Config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("NIF_Init failed(%d)!!\n", ret);
    }
}
#endif

static void COMMS_Init(void)
{
    int32_t retVal;


#if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST)
    COMMS_InitEthernet();
#if defined(ENABLE_PTM) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)
    COMMS_LoadXcvrImageAndInit();
#endif
#if defined(ENABLE_NIF)
    COMMS_InitNif();
#endif
#endif

#if defined(ENABLE_ETH_SWITCH)
    {
        MCU_ExtendedInfoType stackingInfo = {0U};
#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
        COMMS_GetStackingInfo(&stackingInfo);
#endif
        COMMS_LoadCommsImageAndInit(&stackingInfo);
#if defined(ENABLE_CFP) && !defined (ENABLE_CFP_TEST)
        COMMS_CFPInit(&stackingInfo);
        COMMS_AddStreams(&stackingInfo, &CommsConfig);
        COMMS_SetStreamMonitorAlarm(&CommsConfig);
#endif
    }
#endif

    /* Raise async notification for comms ready */
    retVal = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_COM_ID,
                                COMMS_EVENT_COMMS_READY, NULL, 0UL);
    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_INIT_PROC, retVal,
                (uint32_t)__LINE__, 0UL, 0UL, 0UL);
    }

#if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST)
#if defined(ENABLE_ETH_BRPHY)
#if defined(ENABLE_ETH_TIME)
    ETHER_TimeCfgType ethTimeCfg;

    BCM_MemCpy(&ethTimeCfg, ETHER_TimeCfg, sizeof(ETHER_TimeCfgType));

    ETHER_TimeInit(ETHER_HW_ID_0, &ethTimeCfg);
    ETHER_TimeInitStateHandler(ETHER_HW_ID_0, ETHER_TIMEINITCMD_0, 0ULL,  NULL);
    if (ETHER_TIMESYNC_EXTERNAL == ethTimeCfg.sync) {
        /* Raise async notification for ready for timesync */
        retVal = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_COM_ID,
                COMMS_EVENT_READY_FOR_TIMESYNC, NULL, 0UL);
        if (BCM_ERR_OK != retVal) {
            COMMS_ErrorReport(BRCM_SWDSGN_COMMS_INIT_PROC, retVal,
                    (uint32_t)__LINE__, 0UL, 0UL, 0UL);
        }
    }

#if defined(ENABLE_AVT)
    AVT_Reset1588(&sys1588TimeDiff_ns);
    /* Set sys1588TimeDiff_ns to Zero, when Ethernet Time block uses heart beat
     * timer for Ethernet time calculation & as Heart beat timeer is already
     * synchronised with 1588 block. As System timer is currently used for Ethernet
     * time calculation, it will not be set to Zero.
     */
    ETHER_TimeInitStateHandler(ETHER_HW_ID_0, ETHER_TIMEINITCMD_2, sys1588TimeDiff_ns, NULL);
#if defined(ENABLE_ETS)
    COMMS_InitETS();
#endif  /* ENABLE_ETS */
#else /* ENABLE_AVT */
    COMMS_EtherTimeInit0Done = TRUE;
    if (ETHER_TIMESYNC_INTERNAL == ethTimeCfg.sync) {
#if defined(ENABLE_ETH_BRPHY)
        ETHER_TimeInitStateHandler(ETHER_HW_ID_0, ETHER_TIMEINITCMD_2, 0ULL, NULL);
#endif

#if defined(ENABLE_ETS)
        COMMS_InitETS();
#endif  /* ENABLE_ETS */
    } else {
        COMMS_EtherTimeInit0Done = TRUE;
    }
#endif /* ENABLE_AVT */
#endif /* ENABLE_ETH_TIME */
#endif /* ENABLE_ETH_BRPHY */
#endif /* ENABLE_ETH */

    ULOG_ERR("COMMS_Init: Exiting\n");
}

#ifdef ENABLE_ETH_SWITCH

/**
    @brief Get the i-th message from server

    Retrieve the i-th message from server.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]  idx         Index of the message

    @return                 Pointer to the i-th message

    @post None

    @trace #TBD

    @limitations None
*/
void* COMMS_CmdGetMessage(uint32_t idx);

/**
    @brief COMMS command memory
*/
COMMS_CmdRespType  commsCmds[MSGQ_SIZE];

/**
    @brief #MSGQ_HdrQType instance of COMMS command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #TBD
 */
MSGQ_DEFINE_HDRQ(COMMS_CmdHdrQ);

/**
    @brief Command Message Queue

    The message queue for the switch command queue. All commands
    for the switch commands are posted to this message queue.

    @trace #TBD
*/
MSGQ_DEFINE((COMMS_CmdQ), (CommsTask), (COMMS_MSGQ_CMD_EVENT),
            (MSGQ_CLIENT_SIGNAL_ENABLE), COMMS_CmdRespType, (&COMMS_CmdHdrQ),
            (COMMS_CmdGetMessage));

/**
    @code{.unparsed}
    @endcode
*/
void* COMMS_CmdGetMessage(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {

        pRet = (void *)(&commsCmds[idx]);
    }
    return pRet;
}

/**
    @code{.unparsed}
    MSGQ_RecvMsg(COMMS_CmdQ);
    @endcode
*/
int32_t COMMS_RecvResp (const MSGQ_MsgHdrType* const aMsgHdr,
                        COMMS_CmdRespType* const aResp)
{
    int32_t retVal = BCM_ERR_OK;

    /* Check input params */
    if((NULL == aMsgHdr) || (NULL == aResp)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = MSGQ_RecvMsg(&COMMS_CmdQ, aMsgHdr, aResp);
    }

    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_RECVRESP_PROC, retVal,
                (uint32_t) aMsgHdr, (uint32_t) aResp,
                (NULL == aResp) ? 0UL:aResp->cmdID, 0UL);
    }
    return retVal;
}

int32_t COMMS_AddToCmdQ(COMMS_CmdRespType *aCmd,
                        const uint32_t aClientMask,
                        const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_OK;
    retVal = MSGQ_SendMsg(&COMMS_CmdQ, aCmd, aClientMask, aHdr);
    if ((BCM_ERR_OK != retVal) || (*aHdr == NULL)) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_ADDTOCMDQ_PROC, retVal,(uint32_t) aClientMask,
                         (uint32_t) aHdr, 0UL, 0UL);
    }
    return retVal;
}

/** @brief Send commands to comms task

    This API queues the management commands to comms module

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Management command information
    @param[in]      aLen                Length of the management command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     pHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command posted successfully

    @post None

    @limitations This API may not be supported on all the chipsets
*/
static int32_t COMMS_SendCmd(uint8_t aCmdID, BCM_CompIDType aCompID,
                             uint8_t *aCmd, uint32_t aLen,
                             const uint32_t aClientMask,
                             const MSGQ_MsgHdrType** const pHdr,
                             uint32_t *aForwardToSlaves)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aCmd != NULL) && (pHdr != NULL) && (aForwardToSlaves != NULL)
        && (aClientMask != 0UL) && (RPC_RESPPAYLOADSZ > aLen)){
        switch(aCompID) {
            case BCM_SWT_ID:
                retVal = COMMS_EtherSwtSendCmd(aCmdID, aCompID, aCmd, aLen,
                                               aClientMask, pHdr, aForwardToSlaves);
                break;
            case BCM_AMC_ID:
                retVal = COMMS_EtherSendCmd(aCmdID, aCompID, aCmd, aLen,
                                            aClientMask, pHdr, aForwardToSlaves);
                break;
#ifdef ENABLE_ETS
            case BCM_ETS_ID:
                if (ETS_STATE_RUNNING == ETS_GetState()) {
                    retVal = ETS_CmdHandler(aCmdID, aCompID, aCmd, aLen, aClientMask, pHdr, aForwardToSlaves);
                } else {
                    retVal = BCM_ERR_INVAL_STATE;
                }
                break;
#endif
            default:
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != retVal) {
        COMMS_ErrorReport(BRCM_SWDSGN_COMMS_CMDSENDCMD_PROC, retVal,(uint32_t) aClientMask,
                         (uint32_t) pHdr, aCmdID, aCompID);
    }

    return retVal;
}

/** @brief Retreive status of management commands from comms task

    This API queries the status of completed management commands which were
    previously issued to comms task

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      pHdr        Pointer to the message queue header used to
                                retrieve the status
    @param[out]     aCmdID      Pointer to the command id to be filled by
                                comms task
    @param[in]      aCompID     Pointer to the component id to be filled by
                                comms task
    @param[out]     aReply      Pointer to reply message to be filled by
                                comms task
    @param[in]      aReplyLen   Pointer to length of reply message to be
                                filled by comms task

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command status retrieved successfully

    @post None

    @limitations This API may not be supported on all the chipsets
*/
static int32_t COMMS_GetCmdStatus(const MSGQ_MsgHdrType* const pHdr,
                                  BCM_CompIDType aCompID, uint8_t* const aCmdID,
                                  uint8_t *aReply, uint32_t *aReplyLen)
{
    int32_t           retVal = BCM_ERR_OK;

    if ((pHdr != NULL) && (aReply != NULL) && (aReplyLen != NULL)) {
        switch(aCompID) {
            case BCM_SWT_ID:
                retVal = COMMS_EtherSwtCmdStatus(pHdr, aCmdID, aCompID, aReply, aReplyLen);
                break;
            case BCM_AMC_ID:
                retVal = COMMS_EtherCmdStatus(pHdr, aCmdID, aCompID, aReply, aReplyLen);
                break;
#ifdef ENABLE_ETS
            case BCM_ETS_ID:
                retVal = ETS_GetCmdStatus(pHdr, aCompID, aCmdID, aReply, aReplyLen);
                break;
#endif
            default:
                retVal = BCM_ERR_NOSUPPORT;
                break;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
int32_t COMMS_MergeResult(uint8_t aCmdID,
                          BCM_CompIDType aCompID,
                          uint32_t aLen,
                          uint8_t * aOutResponse,
                          int32_t * aOutResult,
                          const uint8_t * aInResponse1,
                          int32_t aInResult1,
                          const uint8_t * aInResponse2,
                          int32_t aInResult2)
{
    int32_t retVal = BCM_ERR_OK;
    switch(aCompID){
        case BCM_SWT_ID:
            retVal = COMMS_EtherSwtMergeResult(aCmdID, aCompID, aLen,
                                               aOutResponse, aOutResult,
                                               aInResponse1, aInResult1,
                                               aInResponse2, aInResult2);
            break;
        case BCM_AMC_ID:
            retVal = COMMS_EtherMergeResult(aCmdID, aCompID, aLen,
                                            aOutResponse, aOutResult,
                                            aInResponse1, aInResult1,
                                            aInResponse2, aInResult2);
            break;
#if defined(ENABLE_ETS)
        case BCM_ETS_ID:
            retVal = ETS_MergeResult(aCmdID, aCompID, aLen,
                                     aOutResponse, aOutResult,
                                     aInResponse1, aInResult1,
                                     aInResponse2, aInResult2);
            break;
#endif
        default:
            retVal = BCM_ERR_NOSUPPORT;
            break;
    }
    return retVal;
}

/**
    @code{.unparsed}
    @endcode
*/
static void COMMS_ProcessCommands(void)
{
    uint32_t idx = 0UL;
    int32_t    retVal = BCM_ERR_OK;
    COMMS_CmdRespType *cmdResPtr;

    /* Try to fetch messages from the command message queue */
    do {
        retVal = MSGQ_GetMsgIdx(&COMMS_CmdQ, &idx);

        if(BCM_ERR_OK == retVal) {
            cmdResPtr = &commsCmds[idx];

            switch(cmdResPtr->compID){
            /* Commands to switch driver */
            case BCM_SWT_ID:
                retVal = COMMS_EtherSwtProcessCommand(cmdResPtr->cmdID,
                                                      &cmdResPtr->ret,
                                                      cmdResPtr->payload,
                                                      &cmdResPtr->payloadLen);
                break;
            /* Commands to ethernet driver */
            case BCM_AMC_ID:
                retVal = COMMS_EtherProcessCommand(cmdResPtr->cmdID,
                                                   &cmdResPtr->ret,
                                                   cmdResPtr->payload,
                                                   &cmdResPtr->payloadLen);
                break;
            default:
                cmdResPtr->ret = BCM_ERR_NOSUPPORT;
                break;
            }

            if((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
                COMMS_ErrorReport(BRCM_SWDSGN_COMMS_PROCESSCOMMANDS_PROC, cmdResPtr->ret,
                        (uint32_t)__LINE__, (uint32_t)cmdResPtr->cmdID,
                        retVal, 0UL);
            }

            if (BCM_ERR_EAGAIN != retVal) {
                /* Update the command processing as complete */
                retVal = MSGQ_CompleteMsgIdx(&COMMS_CmdQ, idx);
            }
        }

    } while(BCM_ERR_OK == retVal);

    return;
}
#endif

#ifdef ENABLE_ETS
void COMMS_InitETS(void)
{
    COMMS_SubsystemIDType id;
    COMMS_SubsystemStateType state;

    (void)BCM_ActivateTask(ETS_ProtocolTask);
    do {
        (void)BCM_WaitEvent(COMMS_NOTIF_EVENT);
        (void)BCM_ClearEvent(COMMS_NOTIF_EVENT);
        COMMS_GetNotification(&id, &state);
        (void)id;
        (void)state;
    } while (id != COMMS_SUBSYSTEM_ETS);
}
#endif

#ifdef ENABLE_ETH_SWITCH
void COMMS_ARLRefreshAlarmCb(void)
{
    (void)SetEvent(CommsTask, ETHERSWT_ARL_REFRESH_EVENT);
}
#endif


/* CommsTask: Common task for all the communication controllers
   for bottom half processing */

TASK(CommsTask)
{
    BCM_EventMaskType eventMask = 0UL;
    int32_t status = BCM_ERR_OK;

#if defined(ENABLE_IPC) || defined(ENABLE_ETS)
    COMMS_SubsystemIDType id;
    COMMS_SubsystemStateType state;
#endif

#if defined(ENABLE_ETS)
    EE_TYPESTATUS taskStatus = SUSPENDED;
#endif

#ifdef ENABLE_RPC
    (void)BCM_ActivateTask(RPC_ServerTask);
    do {
        (void)BCM_WaitEvent(COMMS_NOTIF_EVENT);
        (void)BCM_ClearEvent(COMMS_NOTIF_EVENT);
        COMMS_GetNotification(&id, &state);
        (void)id;
        (void)state;
    } while (id != COMMS_SUBSYSTEM_IPC);
#endif

    COMMS_Init();
    SYS_NotifyState(SYS_SUBSYSTEM_COMMS, SYS_SUBSYSTEM_STATE_INIT);

    do {

        /* Wait on the event */
        status = BCM_WaitEvent(COMMS_EVENT
                             | ShutdownEvent);
        if (BCM_ERR_OK != status) {
            break;
        }
        status = BCM_GetEvent(CommsTask, &eventMask);
        if (BCM_ERR_OK != status) {
            break;
        }

#ifdef ENABLE_ETH_SWITCH
        if (0UL != (eventMask & (COMMS_MSGQ_CMD_EVENT
                                    | MDIO_STATE_HANDLER_EVENT))){
            status = BCM_ClearEvent(eventMask & (COMMS_MSGQ_CMD_EVENT
                                    | MDIO_STATE_HANDLER_EVENT));
            if (BCM_ERR_OK != status) {
                break;
            }
            COMMS_ProcessCommands();
        }

        if ((eventMask & ETHERSWT_ARL_REFRESH_EVENT)
                                == ETHERSWT_ARL_REFRESH_EVENT){
            status = BCM_ClearEvent(ETHERSWT_ARL_REFRESH_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            COMMS_EtherSwtARLRefresh();
        }
#endif
        /* Note: Maintain order of ETHER_ProcessPkts call first
         * then ETHER_TimeEventHandler call.
         */
#ifdef ENABLE_ETH
        if ((eventMask & ETHER_TIMER_EVENT) == ETHER_TIMER_EVENT) {
            status = BCM_ClearEvent(ETHER_TIMER_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process Ethernet Timer */
#if defined(ENABLE_NIF) && !defined(ENABLE_ETHER_TEST)
            NIF_ProcessEthernetTimer(0UL);
#endif
        }
//! [Usage of ETHER_ProcessPkts]
        if ((eventMask & ETHER_PKT_EVENT) == ETHER_PKT_EVENT) {

            status = BCM_ClearEvent(ETHER_PKT_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process Ethernet */
            ETHER_ProcessPkts(0UL);
        }
//! [Usage of ETHER_ProcessPkts]
#endif
#ifdef ENABLE_ETH_BRPHY
//! [Usage of ETHER_ProcessTimestamps]
        if ((eventMask & ETHER_TIMESTAMP_EVENT) == ETHER_TIMESTAMP_EVENT) {

            status = BCM_ClearEvent(ETHER_TIMESTAMP_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process Ethernet timestamped packets */
            ETHER_ProcessTimestamps(0UL);
        }
//! [Usage of ETHER_ProcessTimestamps]

//! [Usage of ETHXCVR_LinkChgIndHandler]
        if ((eventMask & ETHER_XCVR_LINK_EVENT) == ETHER_XCVR_LINK_EVENT) {

            status = BCM_ClearEvent(ETHER_XCVR_LINK_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process Ethernet xcvr link change indication handler */
            ETHXCVR_LinkChgIndHandler(0UL);
        }
//! [Usage of ETHXCVR_LinkChgIndHandler]
#endif

//! [Usage of ETHXCVR_StateHandler]
        if ((eventMask & MDIO_STATE_HANDLER_EVENT) == MDIO_STATE_HANDLER_EVENT) {

            status = BCM_ClearEvent(MDIO_STATE_HANDLER_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* TODO: Process Ethernet xcvr state handler */
        }
//! [Usage of ETHXCVR_StateHandler]

#ifdef ENABLE_ETH_SWITCH
//! [Usage of ETHERSWT_LinkStatChgIndHandler]
        if ((eventMask & ETHERSWT_PORT_LINK_EVENT)
                == ETHERSWT_PORT_LINK_EVENT) {
            int32_t i;
            status = BCM_ClearEvent(ETHERSWT_PORT_LINK_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process Ethernet switch port link change indication */
            for (i = 0UL; i < ETHERSWT_PORT_ID_MAX; i++) {
                ETHERSWT_LinkStatChgIndHandler(0UL, i);
            }
        }
//! [Usage of ETHERSWT_LinkStatChgIndHandler]
#endif

#ifdef ENABLE_CAN_MCAN
//! [Usage of CAN_ProcessPkts]
        if ((eventMask & CAN_EVENT) == CAN_EVENT) {

            status = BCM_ClearEvent(CAN_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process CAN */
            CAN_ProcessPkts(0UL);
        }
//! [Usage of CAN_ProcessPkts]
#endif

#ifdef ENABLE_INET
        if ((eventMask & INET_TIMER_EVENT) == INET_TIMER_EVENT) {

            status = BCM_ClearEvent(INET_TIMER_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            /* Process internet stack */
            INET_ProcessTimers();
        }
#endif

#if (defined ENABLE_ETH_SWITCH) && defined(ENABLE_CFP)
        if ((eventMask & COMMS_STREAM_MONITOR_EVENT)
                == COMMS_STREAM_MONITOR_EVENT) {
            status = BCM_ClearEvent(COMMS_STREAM_MONITOR_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
            COMMS_MonitorStreams(&CommsConfig);
        }
#endif
        if ((eventMask & ShutdownEvent) == ShutdownEvent) {
            (void)BCM_ClearEvent(ShutdownEvent);
            break;
        }

    } while (BCM_ERR_OK == status);

#ifdef ENABLE_ETS
    /* send an event to IPC server to shutdown */
    (void)GetTaskState(ETS_ProtocolTask, &taskStatus);
    if (SUSPENDED != taskStatus) {
        (void)BCM_SetEvent(ETS_ProtocolTask, ShutdownEvent);
        (void)BCM_WaitEvent(COMMS_NOTIF_EVENT);
        (void)BCM_ClearEvent(COMMS_NOTIF_EVENT);
        COMMS_GetNotification(&id, &state);
        (void)id;
        (void)state;
    }

#endif

#ifdef ENABLE_RPC
    /* send an event to RPC server to shutdown */
    (void)BCM_SetEvent(RPC_ServerTask, ShutdownEvent);
    (void)BCM_WaitEvent(COMMS_NOTIF_EVENT);
    (void)BCM_ClearEvent(COMMS_NOTIF_EVENT);
    COMMS_GetNotification(&id, &state);
    (void)id;
    (void)state;
#endif

#if defined(ENABLE_ETH_SWITCH)
        (void)ETHERSWT_MgmtDeInit(0UL);
#endif

    /* send shutdown ack to system task */
    SYS_NotifyState(SYS_SUBSYSTEM_COMMS, SYS_SUBSYSTEM_STATE_UNINIT);

    (void)BCM_TerminateTask();
}

#ifdef ENABLE_ETH_SWITCH
const RPC_CompType COMMS_RpcIfc = {
    .sendCmd = COMMS_SendCmd,
    .getStatus = COMMS_GetCmdStatus,
    .mergeResult = COMMS_MergeResult,
};
#endif
/* Nothing past this line */
