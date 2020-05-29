/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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

#include "ee.h"
#include <string.h>
#include <ulog.h>
#include <utils.h>
#include <bcm_err.h>
#include <chip_config.h>
#include <eth_switch.h>
#include <console.h>
#include <osil/eth_switch_osil.h>
#include <board.h>
#include <switch_cfg.h>

#define GetModuleLogLevel()     (ULOG_LVL_INFO)
#define ETH_SWITCH_HW_ID        (0UL)

extern ETHERSWT_CfgType SwitchCfgUsrData;

static ETHERSWT_PortMirrorCfgType PortMirrorCfgIngress;
static ETHERSWT_PortMirrorCfgType PortMirrorCfgEgress;
static uint32_t MirrorCapturePort = ETHERSWT_PORT_ID_MAX;

static int32_t GetPortCfgListIdx(uint32_t aPortID, uint32_t *const aIdx)
{
    int32_t retVal;
    uint32_t i;

    retVal = BCM_ERR_NOT_FOUND;

    if (NULL == aIdx) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    for(i = 0UL; i< SwitchCfgUsrData.portCfgListSz; ++i) {
        if (aPortID == SwitchCfgUsrData.portCfgList[i].portID) {
            aIdx[0] = i;
            retVal = BCM_ERR_OK;
            break;
        }
    }

err:
    return retVal;
}

int32_t SwitchCfg_GetPortCfg(uint32_t aPortID,
                             ETHERSWT_PortCfgType * const aPortCfg)
{
    int32_t retVal;
    uint32_t listIdx;

    if (NULL == aPortCfg) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = GetPortCfgListIdx(aPortID, &listIdx);
    if (BCM_ERR_OK == retVal) {
        BCM_MemCpy((void *)aPortCfg,
                   (void *)&SwitchCfgUsrData.portCfgList[listIdx],
                   sizeof(ETHERSWT_PortCfgType));
    }

err:
    return retVal;
}

int32_t SwitchCfg_GetVLAN(ETHERSWT_VLANIDType aVLANID,
                          uint32_t * const aPortBitMap,
                          uint32_t * const aTagBitMap)
{
    int32_t retVal;
    uint32_t portCfgListIdx;
    uint32_t vlanMemListIdx;
    ETHERSWT_PortCfgType * portCfg;

    portCfg = NULL;

    if ((NULL == aPortBitMap) || (NULL == aTagBitMap)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    aPortBitMap[0] = 0UL;
    aTagBitMap[0] = 0UL;

    for (portCfgListIdx = 0UL; portCfgListIdx < SwitchCfgUsrData.portCfgListSz;
         ++portCfgListIdx) {
       portCfg = &SwitchCfgUsrData.portCfgList[portCfgListIdx];

       for (vlanMemListIdx = 0UL; vlanMemListIdx < portCfg->vlanMemListSz;
            ++vlanMemListIdx) {
           if (aVLANID == portCfg->vlanMemList[vlanMemListIdx].vlanID) {
               aPortBitMap[0] |= (1UL << portCfg->portID);
               if (ETHERSWT_VLAN_FRWRD_TAGGED == portCfg->vlanMemList[vlanMemListIdx].forward) {
                   aTagBitMap[0] |= (1UL <<  portCfg->portID);
               }
           }
       }
    }

    retVal = BCM_ERR_OK;

err:
    return retVal;
}

int32_t SwitchCfg_GetVLANPort(uint32_t aPortID, uint32_t * const aIngressFilter,
                              ETHERSWT_VLANIDType * const aDefaultVLAN,
                              ETHERSWT_PCPType * const aDefaultPrio)
{
    int32_t retVal;
    uint32_t portCfgListIdx;
    ETHERSWT_PortCfgType * portCfg;

    portCfg = NULL;

    if ((NULL == aIngressFilter) || (NULL == aDefaultVLAN) || (NULL == aDefaultPrio)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    retVal = GetPortCfgListIdx(aPortID, &portCfgListIdx);
    if (BCM_ERR_OK == retVal) {
        portCfg = &SwitchCfgUsrData.portCfgList[portCfgListIdx];
        aDefaultVLAN[0] = portCfg->ingressCfg.defaultVLAN;
        aDefaultPrio[0] = portCfg->ingressCfg.defaultPrio;
        aIngressFilter[0] = 1UL;
    }

err:
    return retVal;
}

int32_t SwitchCfg_JumboFrmGet(uint32_t aPortID, uint32_t *const aJumboFrm)
{
    int32_t retVal;
    uint32_t idx;

    retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHERSWT_PORT_ID_MAX <= aPortID)
        || (NULL == aJumboFrm)) {
        goto err;
    }

    retVal = GetPortCfgListIdx(aPortID, &idx);
    if (BCM_ERR_OK == retVal) {
        aJumboFrm[0] = SwitchCfgUsrData.portCfgList[idx].enableJumboFrm;
    }

err:
    return retVal;
}

static void PrintRxStats(ETHER_RxStatsType *rxStats)
{
    ULOG_ERR("*** RX PKT STATS ***\n");
    ULOG_ERR("\t brdCast: %d\n", rxStats->brdCast);
    ULOG_ERR("\t multicast: %d\n", rxStats->mutCast);
    ULOG_ERR("\t unicast: %d\n", rxStats->uniCast);
    ULOG_ERR("\t pkts64: %d\n", rxStats->pkts64);
    ULOG_ERR("\t pkts65_127: %d\n", rxStats->pkts65_127);
    ULOG_ERR("\t pkts128_255: %d\n", rxStats->pkts128_255);
    ULOG_ERR("\t pkts256_511: %d\n", rxStats->pkts256_511);
    ULOG_ERR("\t pkts512_1023: %d\n", rxStats->pkts512_1023);
    ULOG_ERR("\t pkts1024_1522: %d\n", rxStats->pkts1024_1522);
    ULOG_ERR("\t pkts1523_2047: %d\n", rxStats->pkts1523_2047);
    ULOG_ERR("\t pkts2048_4095: %d\n", rxStats->pkts2048_4095);
    ULOG_ERR("\t pkts4096_8191: %d\n", rxStats->pkts4096_8191);
    ULOG_ERR("\t pkts8192_MAX: %d\n", rxStats->pkts8192_MAX);
    ULOG_ERR("\t pktsOvrSz: %d\n", rxStats->pktsOvrSz);
    ULOG_ERR("\t pktsRxDrop: %d\n", rxStats->pktsRxDrop);
    ULOG_ERR("\t pktsCrcErr: %d\n", rxStats->pktsCrcErr);
    ULOG_ERR("\t pktsCrcAlignErr: %d\n", rxStats->pktsCrcAlignErr);
    ULOG_ERR("\t pktsJabber: %d\n", rxStats->pktsJabber);
    ULOG_ERR("\t pktsFrag: %d\n", rxStats->pktsFrag);
    ULOG_ERR("\t pktsUndSz: %d\n", rxStats->pktsUndSz);
    ULOG_ERR("\t pktsRxDiscard: %d\n", rxStats->pktsRxDiscard);
}

static void PrintTxStats(ETHER_TxStatsType *txStats)
{
    ULOG_ERR("*** TX PKT STATS ***\n");
    ULOG_ERR("\t octets: %d\n", txStats->octets);
    ULOG_ERR("\t brdCast: %d\n", txStats->brdCast);
    ULOG_ERR("\t multicast: %d\n", txStats->mutCast);
    ULOG_ERR("\t unicast: %d\n", txStats->uniCast);
    ULOG_ERR("\t txDropped: %d\n", txStats->txDropped);
    ULOG_ERR("\t txCollision: %d\n", txStats->txCollision);
    ULOG_ERR("\t txFrameInDiscard: %d\n", txStats->txFrameInDiscard);
}

static void PrintSwitchConfig(uint32_t portID)
{
    uint32_t j;
    uint32_t portCfgListIdx;
    int32_t retVal;
    ETHERSWT_PortCfgType *port;

    retVal = GetPortCfgListIdx(portID, &portCfgListIdx);
    if (BCM_ERR_OK == retVal) {
        port = &SwitchCfgUsrData.portCfgList[portCfgListIdx];

        ULOG_ERR("*** Switch Configuration ***\n");
        ULOG_ERR("*** PORT: %d***\n", port->portID);
        ULOG_ERR("role: %d\n", port->role);
        ULOG_ERR("xcvrID: %d\n", port->xcvrID);
        ULOG_ERR("enableTimeStamp: %d\n", port->enableTimeStamp);
        ULOG_ERR("enableJumboFrm: %d\n\n", port->enableJumboFrm);
        ULOG_ERR("**** Fixed MAC Addresses ****\n");
        for (j = 0; j < port->macAddrListSz; j++) {
            uint8_t *mac = &port->fixedMacAddrList[j][0];
            ULOG_ERR("\t%02x.%02x.%02x.%02x.%02x.%02x\n",
                    mac[0],
                    mac[1],
                    mac[2],
                    mac[3],
                    mac[4],
                    mac[5]);
        }
        ULOG_ERR("\n**** VLANs ****\n");
        ULOG_ERR("\tID \t forwardType\n");
        for (j = 0UL; j < port->vlanMemListSz; j++) {
            ULOG_ERR("\t%04x \t %02d\n", port->vlanMemList[j].vlanID,
                    port->vlanMemList[j].forward);
        }

        ULOG_ERR("\n**** Ingress configuration ****\n");
        ULOG_ERR("\tDefault VLAN: %04x\n", port->ingressCfg.defaultVLAN);
        ULOG_ERR("\tDefault priority: %d\n", port->ingressCfg.defaultPrio);
        ULOG_ERR("\tdrop untagged:%d\n", port->ingressCfg.dropUntagged);
        ULOG_ERR("\tfixed TC: %d\n", port->ingressCfg.tc);
        ULOG_ERR("\t**** PCP2TC MAP ****\n");
        ULOG_ERR("\tPCP\t\tTC\n");
        for (j = 0; j < 8UL; j++) {
            ULOG_ERR("\t%02d\t\t%02d\n", j, port->ingressCfg.pcp2tcMap[j]);
        }
        ULOG_ERR("\n**** Egress configuration ****\n");
        ULOG_ERR("\t**** TC2PCP MAP ****\n");
        ULOG_ERR("\tTC\t\tPCP\n");
        for (j = 0; j < 8UL; j++) {
            ULOG_ERR("\t%02d\t\t%02d\n", j, port->egressCfg.tc2pcpMap[j]);
        }
        ULOG_ERR("\t**** FIFO configuration ****\n");
        ULOG_ERR("\tID \t minLen \t tc \t rateBPS \t AVBShapingMode\n");
        for (j = 0; j < port->egressCfg.fifoListSz; j++) {
            ULOG_ERR("\t%02d \t %04d \t %02d %04d \t %02d\n", port->egressCfg.fifoList[j].id,
                    port->egressCfg.fifoList[j].minLen,
                    port->egressCfg.fifoList[j].tc,
                    port->egressCfg.fifoList[j].shaper.rateBps,
                    port->egressCfg.fifoList[j].shaper.avbShapingModeEn);
        }
    } else {
        ULOG_ERR("Error: Port %lu not found (%ld)\n", portID, retVal);
    }
}

static void PrintMirrorConfig(ETHERSWT_PortMirrorCfgType *aConfig)
{
    ULOG_ERR("\tPortMask: 0x%x\n", aConfig->portMask);
    ULOG_ERR("\tpacketDivider: %d\n", aConfig->packetDivider);
    ULOG_ERR("\tsrcMacAddrFilter:");
    ULOG_ERR("%02x.%02x.%02x.%02x.%02x.%02x\n",
           aConfig->srcMacAddrFilter[0],
           aConfig->srcMacAddrFilter[1],
           aConfig->srcMacAddrFilter[2],
           aConfig->srcMacAddrFilter[3],
           aConfig->srcMacAddrFilter[4],
           aConfig->srcMacAddrFilter[5]);
    ULOG_ERR("\tdestMacAddrFilter:");
    ULOG_ERR("%02x.%02x.%02x.%02x.%02x.%02x\n",
            aConfig->destMacAddrFilter[0],
            aConfig->destMacAddrFilter[1],
            aConfig->destMacAddrFilter[2],
            aConfig->destMacAddrFilter[3],
            aConfig->destMacAddrFilter[4],
            aConfig->destMacAddrFilter[5]);
}

static int32_t ReadARLTbl(void)
{
    uint32_t i;
    int32_t ret;
    ETHERSWT_ARLEntryType arlTbl[50];
    uint16_t arlTblSz = sizeof(arlTbl)/sizeof(ETHERSWT_ARLEntryType);

    memset(arlTbl, 0x0, sizeof(arlTbl));
    ret = ETHERSWT_GetARLTable(ETH_SWITCH_HW_ID, arlTbl, &arlTblSz);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("ETHERSWT_GetARLTable() returned error: %d\n", ret);
        goto err_exit;
    }

    ULOG_ERR("#### ARL Table #### \n");
    ULOG_ERR("MACADDR\t\t\t VLANID\t PortID\t Reserved\t\n");
    for (i = 0; i < arlTblSz; i++) {
        ULOG_ERR("%02x.%02x.%02x.%02x.%02x.%02x\t %04x\t %x\t %01x\t\n",
                arlTbl[i].macAddr[0],
                arlTbl[i].macAddr[1],
                arlTbl[i].macAddr[2],
                arlTbl[i].macAddr[3],
                arlTbl[i].macAddr[4],
                arlTbl[i].macAddr[5],
                arlTbl[i].vlanID,
                arlTbl[i].portMask,
                arlTbl[i].reserved);
    }
err_exit:
    return ret;
}


static void Shell_ConfigCmdHandler(uint32_t aConsoleID, char* aArgString,
        uint32_t aArgStringLen)
{
    char *remain = NULL;
    uint32_t remLen = 0UL;
    char *endptr = NULL;
    uint32_t portID;

    CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
    if (remLen == 0UL) {
        CONSOLE_Write(aConsoleID, "config show <portnum> ==> show switch configuration\n");
        goto err_exit;
    }

    if (0UL == strncmp(remain, "show", 4UL)) {
        CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
        portID = strtoul(remain, &endptr, 0);
        PrintSwitchConfig(portID);
    }
err_exit:
    return;
}


static void Shell_MirrorCmdHandler(uint32_t aConsoleID, char* aArgString,
        uint32_t aArgStringLen)
{
    char *remain = NULL;
    uint32_t remLen = 0UL;
    char *endptr = NULL;
    uint64_t srcMac;
    uint64_t destMac;
    uint32_t portMask;
    uint32_t divider;
    uint32_t capturePort;
    int32_t ret = BCM_ERR_OK;

    CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
    if (remLen == 0UL) {
        CONSOLE_Write(aConsoleID, "mirror port <ingress/egress> <captureport> <portmask>     ==> Configure Capture traffic for ports\n");
        CONSOLE_Write(aConsoleID, "mirror filter mac <ingress/egress> <srcmac> <destmac>     ==> configure mirror filter for ingress/egress based on macaddr\n");
        CONSOLE_Write(aConsoleID, "mirror filter packetrate <ingress/egress> <divider>       ==> configure mirror filter for ingress/egress packet divider\n");
        CONSOLE_Write(aConsoleID, "mirror config show                                        ==> show mirror configuration\n");
        CONSOLE_Write(aConsoleID, "mirror <enable/disable>                                   ==> Enable/Disable port mirroring with configured values\n");
        goto err_exit;
    }
    if (0UL == strncmp(remain, "port", 4UL)) {
        CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
        if (0UL == strncmp(remain, "ingress", 7UL)) {
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            capturePort = strtoul(remain, &endptr, 0);
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            portMask = strtoul(remain, &endptr, 0);
            if (capturePort < ETHERSWT_PORT_ID_MAX) {
                PortMirrorCfgIngress.portMask = portMask;
                MirrorCapturePort = capturePort;
            }

        } else if (0UL == strncmp(remain, "egress", 6UL)) {
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            capturePort = strtoul(remain, &endptr, 0);
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            portMask = strtoul(remain, &endptr, 0);
            if (capturePort < ETHERSWT_PORT_ID_MAX) {
                PortMirrorCfgEgress.portMask = portMask;
                MirrorCapturePort = capturePort;
            }
        } else {
            CONSOLE_Write(aConsoleID, "invalid option\n");
            goto err_exit;
        }
    } else if (0UL == strncmp(remain, "filter", 6UL)) {
        CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
        if (0UL == strncmp(remain, "mac", 3UL)) {
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            if (0UL == strncmp(remain, "ingress", 7UL)) {
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                srcMac = strtoull(remain, &endptr, 0);
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                destMac = strtoull(remain, &endptr, 0);
                if ((srcMac != 0UL) && (destMac != 0UL)) {
                    CONSOLE_Write(aConsoleID, "cannot apply src/dest mac filter at same time\n");
                    goto err_exit;
                }
                if (srcMac != 0UL) {
                    HWMAC2NMAC(srcMac, PortMirrorCfgIngress.srcMacAddrFilter);
                } else if (destMac != 0UL) {
                    HWMAC2NMAC(destMac, PortMirrorCfgIngress.destMacAddrFilter);
                }
            } else if (0UL == strncmp(remain, "egress", 6UL)) {
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                srcMac = strtoull(remain, &endptr, 0);
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                destMac = strtoull(remain, &endptr, 0);
                if ((srcMac != 0UL) && (destMac != 0UL)) {
                    CONSOLE_Write(aConsoleID, "cannot apply src/dest mac filter at same time\n");
                    goto err_exit;
                }
                if (srcMac != 0UL) {
                    HWMAC2NMAC(srcMac, PortMirrorCfgEgress.srcMacAddrFilter);
                } else if (destMac != 0UL) {
                    HWMAC2NMAC(destMac, PortMirrorCfgEgress.destMacAddrFilter);
                }
            }
        } else if(0UL == strncmp(remain, "packetrate", 3UL)) {
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            if (0UL == strncmp(remain, "ingress", 7UL)) {
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                divider = strtoul(remain, &endptr, 0);
                PortMirrorCfgIngress.packetDivider = divider;
            } else if (0UL == strncmp(remain, "egress", 6UL)) {
                CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
                divider = strtoul(remain, &endptr, 0);
                PortMirrorCfgEgress.packetDivider = divider;
            }
        }
    } else if (0UL == strncmp(remain, "enable", 9UL)) {
        if (MirrorCapturePort < ETHERSWT_PORT_ID_MAX) {
            if (PortMirrorCfgIngress.portMask != 0UL) {
                ret = ETHERSWT_SetPortMirrorConfig(0UL, MirrorCapturePort,
                        ETHERSWT_TRAFFICDIR_INGRESS,
                        &PortMirrorCfgIngress);
            }
            if (PortMirrorCfgEgress.portMask != 0UL) {
                ret = ETHERSWT_SetPortMirrorConfig(0UL, MirrorCapturePort,
                        ETHERSWT_TRAFFICDIR_EGRESS,
                        &PortMirrorCfgEgress);
            }
            if (ret == BCM_ERR_OK) {
                ret = ETHERSWT_SetPortMirrorState(0UL, MirrorCapturePort,
                        ETHERSWT_PORT_MIRROR_STATE_ENABLED);
                if (ret != BCM_ERR_OK) {
                    CONSOLE_Write(aConsoleID, "ETHERSWT_SetPortMirrorState() failed\n");
                }
            } else {
                CONSOLE_Write(aConsoleID, "ETHERSWT_SetPortMirrorConfig() failed\n");
            }
        }
    } else if (0UL == strncmp(remain, "disable", 7UL)) {
        if (MirrorCapturePort < ETHERSWT_PORT_ID_MAX) {
            ret = ETHERSWT_SetPortMirrorState(0UL, MirrorCapturePort,
                    ETHERSWT_PORT_MIRROR_STATE_DISABLED);
            if (ret != BCM_ERR_OK) {
                CONSOLE_Write(aConsoleID, "ETHERSWT_SetPortMirrorState() failed\n");
            }
        }
    } else  if (0UL == strncmp(remain, "config", 6UL)) {
        CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
        if (0UL == strncmp(remain, "show", 4UL)) {
            ULOG_ERR("Capture port: %d\n", MirrorCapturePort);
            ULOG_ERR("**** Ingress Configuration ****\n");
            PrintMirrorConfig(&PortMirrorCfgIngress);
            ULOG_ERR("**** Egress Configuration ****\n");
            PrintMirrorConfig(&PortMirrorCfgEgress);
        }
    }
err_exit:
    return;
}

static void Shell_ARLCmdHandler(uint32_t aConsoleID, char* aArgString,
        uint32_t aArgStringLen)
{
    char *remain = NULL;
    uint32_t remLen = 0UL;

    CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);

    if (remLen == 0UL) {
        CONSOLE_Write(aConsoleID, "arl dump                   ==> Dump ARL table\n");
        goto err_exit;
    }

    if (0UL == strncmp(remain, "dump", 4UL)) {
        ReadARLTbl();
    }
err_exit:
    return;
}

static void Shell_MiscCmdHandler(uint32_t aConsoleID, char* aArgString,
        uint32_t aArgStringLen)
{
    char *remain = NULL;
    uint32_t remLen = 0UL;
    char *endptr = NULL;
    uint32_t portMask;
    uint32_t i;
    ETHER_RxStatsType rxstat;
    ETHER_TxStatsType txstat;
    int32_t ret;

    if (0UL == strncmp(aArgString, "mibs", 4UL)) {
        CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
        if (0UL == strncmp(remain, "dump", 4UL)) {
            CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
            portMask = strtoul(remain, &endptr, 0);
            for (i = 0UL; i < ETHERSWT_PORT_ID_MAX; i++) {
                if ((portMask & (0x1 << i)) != 0UL) {
                    ret = ETHERSWT_GetRxStat(0UL, i, &rxstat);
                    if (ret == BCM_ERR_OK) {
                        PrintRxStats(&rxstat);
                    }
                    ret = ETHERSWT_GetTxStat(0UL, i, &txstat);
                    if (ret == BCM_ERR_OK) {
                        PrintTxStats(&txstat);
                    }
                }
            }
        }
    }
}

static void Shell_PrintHelp(uint32_t aConsoleID)
{
    CONSOLE_Write(aConsoleID, "Switch Commands:\n");
    CONSOLE_Write(aConsoleID, "arl                               ==> manage ARL entries\n");
    CONSOLE_Write(aConsoleID, "mirror                            ==> manage mirror configuration\n");
    CONSOLE_Write(aConsoleID, "config                            ==> show/save/reset switch configuration\n");
    CONSOLE_Write(aConsoleID, "mibs dump <portmask>              ==> Dump mibs counter for ports\n");
}

void Shell_Switch(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        if (0UL == strncmp(aArgString, "arl", 3UL)) {
            Shell_ARLCmdHandler(aConsoleID, aArgString, aArgStringLen);
        } else if (0UL == strncmp(aArgString, "mirror", 6UL)) {
            Shell_MirrorCmdHandler(aConsoleID, aArgString, aArgStringLen);
        } else if (0UL == strncmp(aArgString, "config", 6UL)) {
            Shell_ConfigCmdHandler(aConsoleID, aArgString, aArgStringLen);
        } else if (0UL == strncmp(aArgString, "qos", 3UL)) {
        } else {
            Shell_MiscCmdHandler(aConsoleID, aArgString, aArgStringLen);
        }
    } else {
        Shell_PrintHelp(aConsoleID);
    }
}

