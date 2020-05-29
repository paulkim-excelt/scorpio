/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
#include <atomic.h>
#include <cache.h>
#include <cortex.h>
#include <compiler.h>
#include <interrupt.h>
#include <utils.h>
#include <eth_osil.h>
#include <system.h>
#include "eth_cntlr.h"
#include "eth_cntlr_buf.h"
#include "gmac_rdb.h"

//#include "eth_cntlr_buf.h"

/**
    @name Ethernet controller driver interfce IDs
    @{
    @brief Interface IDs for Ethernet controller driver
*/
#define BRCM_SWDSGN_ETHER_DMACHANSTATE_TYPE                 ()  /**< @brief #ETHER_DmaChanStateType */
#define BRCM_SWDSGN_ETHER_DMACHANERR_TYPE                   ()  /**< @brief #ETHER_DmaChanErrType */
#define BRCM_SWDSGN_ETHER_DMADESC_TYPE                      ()  /**< @brief #ETHER_DmaDescType */
#define BRCM_SWDSGN_ETHER_DMADESCSIZE_MACRO                 ()  /**< @brief #ETHER_DMADESCSIZE */
#define BRCM_SWDSGN_ETHER_SETDESCFIELDS_MACRO               ()  /**< @brief #ETHER_SETDESCFIELDS */
#define BRCM_SWDSGN_ETHER_SETDESCFIELD_MACRO                ()  /**< @brief #ETHER_SETDESCFIELD */
#define BRCM_SWDSGN_ETHER_INITRXDESC_MACRO                  ()  /**< @brief #ETHER_INITRXDESC */
#define BRCM_SWDSGN_ETHER_AMACRXPKTINFO_TYPE                ()  /**< @brief #ETHER_AMACRxPktInfoType */
#define BRCM_SWDSGN_ETHER_RXPKTTYPESIZE_MACRO               ()  /**< @brief #ETHER_RXPKTTYPESIZE */
#define BRCM_SWDSGN_ETHER_STATS_TYPE                        ()  /**< @brief #ETHER_StatsType */
#define BRCM_SWDSGN_ETHER_INITSTATSZERO_MACRO               ()  /**< @brief #ETHER_INITSTATSZERO */
#define BRCM_SWDSGN_ETHER_TXCHANINFO_TYPE                   ()  /**< @brief #ETHER_TxChanInfoType*/
#define BRCM_SWDSGN_ETHER_RXCHANINFO_TYPE                   ()  /**< @brief #ETHER_RxChanInfoType */
#define BRCM_SWDSGN_ETHER_INITTXCHANNELINFO_MACRO           ()  /**< @brief #ETHER_INITTXCHANNELINFO */
#define BRCM_SWDSGN_ETHER_INITRXCHANNELINFO_MACRO           ()  /**< @brief #ETHER_INITRXCHANNELINFO */
#define BRCM_SWDSGN_ETHER_AMACRESETDELAYCNT_MACRO           ()  /**< @brief #ETHER_AMACRESETDELAYCNT */
#define BRCM_SWDSGN_ETHER_TXPARITY_MACRO                    ()  /**< @brief #ETHER_TXPARITY_EN */
#define BRCM_SWDSGN_ETHER_DMADESCERRMASK_MACRO              ()  /**< @brief #ETHER_DMADESCERRMASK */
#define BRCM_SWDSGN_ETHER_RXDESCOFFSET_MACRO                ()  /**< @brief #ETHER_RXDESCOFFSET */
#define BRCM_SWDSGN_ETHER_TXDESCOFFSET_MACRO                ()  /**< @brief #ETHER_TXDESCOFFSET */
#define BRCM_SWDSGN_ETHER_TXCH0BUFFCOUNT_MACRO              ()  /**< @brief #ETHER_TXCH0BUFFCOUNT */
#define BRCM_SWDSGN_ETHER_TXCH1BUFFCOUNT_MACRO              ()  /**< @brief #ETHER_TXCH1BUFFCOUNT */
#define BRCM_SWDSGN_ETHER_TXCH2BUFFCOUNT_MACRO              ()  /**< @brief #ETHER_TXCH2BUFFCOUNT */
#define BRCM_SWDSGN_ETHER_TXCH3BUFFCOUNT_MACRO              ()  /**< @brief #ETHER_TXCH3BUFFCOUNT */
#define BRCM_SWDSGN_ETHER_RXBUFFCOUNT_MACRO                 ()  /**< @brief #ETHER_RXBUFFCOUNT */
#define BRCM_SWDSGN_ETHER_DESCTABLEDESCCNT_MACRO            ()  /**< @brief #ETHER_DESCTABLEDESCCNT */
#define BRCM_SWDSGN_ETHER_TXCHANDESCCNT_MACRO               ()  /**< @brief #ETHER_TXCHANDESCCNT */
#define BRCM_SWDSGN_ETHER_DESCTABLESIZE_MACRO               ()  /**< @brief #ETHER_DESCTABLESIZE */
#define BRCM_SWDSGN_ETHER_TXCH0DMADESCTBL_GLOBAL            ()  /**< @brief #ETHER_TxCh0DmaDescTbl */
#define BRCM_SWDSGN_ETHER_TXCH1DMADESCTBL_GLOBAL            ()  /**< @brief #ETHER_TxCh1DmaDescTbl */
#define BRCM_SWDSGN_ETHER_TXCH2DMADESCTBL_GLOBAL            ()  /**< @brief #ETHER_TxCh2DmaDescTbl */
#define BRCM_SWDSGN_ETHER_TXCH3DMADESCTBL_GLOBAL            ()  /**< @brief #ETHER_TxCh3DmaDescTbl */
#define BRCM_SWDSGN_ETHER_TXCH0PKTBUF_GLOBAL                ()  /**< @brief #ETHER_TxCh0PktBuf */
#define BRCM_SWDSGN_ETHER_TXCH1PKTBUF_GLOBAL                ()  /**< @brief #ETHER_TxCh1PktBuf */
#define BRCM_SWDSGN_ETHER_TXCH2PKTBUF_GLOBAL                ()  /**< @brief #ETHER_TxCh2PktBuf */
#define BRCM_SWDSGN_ETHER_TXCH3PKTBUF_GLOBAL                ()  /**< @brief #ETHER_TxCh3PktBuf */
#define BRCM_SWDSGN_ETHER_RXPKT_GLOBAL                      ()  /**< @brief #ETHER_RxPkt */
#define BRCM_SWDSGN_ETHER_TXCHPKTBUF_GLOBAL                 ()  /**< @brief #ETHER_TxChPktBuf */
#define BRCM_SWDSGN_ETHER_RXDMADESCTBL_GLOBAL               ()  /**< @brief #ETHER_RxDmaDescTbl */
#define BRCM_SWDSGN_ETHER_AMACREGS_GLOBAL                   ()  /**< @brief #ETHER_AMACREGS */
#define BRCM_SWDSGN_ETHER_AMACCHANREGS_GLOBAL               ()  /**< @brief #ETHER_AMACCHANREGS */
#define BRCM_SWDSGN_ETHER_TXCH0PKTBUFFINFO_GLOBAL           ()  /**< @brief #ETHER_TxCh0PktBuffInfo */
#define BRCM_SWDSGN_ETHER_TXCH1PKTBUFFINFO_GLOBAL           ()  /**< @brief #ETHER_TxCh1PktBuffInfo */
#define BRCM_SWDSGN_ETHER_TXCH2PKTBUFFINFO_GLOBAL           ()  /**< @brief #ETHER_TxCh2PktBuffInfo */
#define BRCM_SWDSGN_ETHER_TXCH3PKTBUFFINFO_GLOBAL           ()  /**< @brief #ETHER_TxCh3PktBuffInfo */
#define BRCM_SWDSGN_ETHER_RXPKTBUFFINFO_GLOBAL              ()  /**< @brief #ETHER_RxPktBuffInfo */
#define BRCM_SWDSGN_ETHER_RXBUFMGRINFO_GLOBAL               ()  /**< @brief #ETHER_RxBufMgrInfo */
#define BRCM_SWDSGN_ETHER_TXBUFMGRINFO_GLOBAL               ()  /**< @brief #ETHER_TxBufMgrInfo */
#define BRCM_SWDSGN_ETHER_STATS_GLOBAL                      ()  /**< @brief #ETHER_Stats */
#define BRCM_SWDSGN_ETHER_RXCHANINFO_GLOBAL                 ()  /**< @brief #ETHER_RxChanInfo */
#define BRCM_SWDSGN_ETHER_TXCHANINFO_GLOBAL                 ()  /**< @brief #ETHER_TxChanInfo */
#define BRCM_SWDSGN_ETHER_CNTLRSTATE_GLOBAL                 ()  /**< @brief #ETHER_CntlrState */
#define BRCM_SWDSGN_ETHER_CNTLRRWDEV_TYPE                   ()  /**< @brief #ETHER_CntlrRWDevType */
#define BRCM_SWDSGN_ETHER_CNTLRRWDEVDATA_GLOBAL             ()  /**< @brief #ETHER_CntlrRWDevData */
#define BRCM_SWDSGN_ETHER_PRIOCHANMAP_GLOBAL                ()  /**< @brief #ETHER_PrioChanMap */
#define BRCM_SWDSGN_ETHER_MACADDR_GLOBAL                    ()  /**< @brief #ETHER_MacAddr */
#define BRCM_SWDSGN_ETHER_MACDEV_TYPE                       ()  /**< @brief #ETHER_MacDevType */
#define BRCM_SWDSGN_ETHER_MACDEV_GLOBAL                     ()  /**< @brief #ETHER_MacDev */
#define BRCM_SWDSGN_ETHER_CNTLRREPORTERROR_PROC             (0xBFU)  /**< @brief #ETHER_CntlrReportError */
#define BRCM_SWDSGN_ETHER_MACCHECKCONFIGPARAMS_PROC         (0xC0U)  /**< @brief #ETHER_MacCheckConfigParams */
#define BRCM_SWDSGN_ETHER_MACENABLETXRESET_PROC             (0xC1U)  /**< @brief #ETHER_MacEnableTxReset */
#define BRCM_SWDSGN_ETHER_MACENABLERXRESET_PROC             (0xC2U)  /**< @brief #ETHER_MacEnableRxReset */
#define BRCM_SWDSGN_ETHER_MACDISABLETXRX_PROC               (0xC3U)  /**< @brief #ETHER_MacDisableTxRx */
#define BRCM_SWDSGN_ETHER_MACENABLETXRX_PROC                (0xC4U)  /**< @brief #ETHER_MacEnableTxRx */
#define BRCM_SWDSGN_ETHER_MACISENABLED_PROC                 (0xC5U)  /**< @brief #ETHER_MacIsEnabled */
#define BRCM_SWDSGN_ETHER_MACSETLOOPBACKMODE_PROC           (0xC6U)  /**< @brief #ETHER_MacSetLoopbackMode */
#define BRCM_SWDSGN_ETHER_MACENABLELOOPBACK_PROC            (0xC7U)  /**< @brief #ETHER_MacEnableLoopback */
#define BRCM_SWDSGN_ETHER_MACGETMACADDR_PROC                (0xC8U)  /**< @brief #ETHER_MacGetMacAddr */
#define BRCM_SWDSGN_ETHER_MACSETMACADDRINTERNAL_PROC        (0xC9U)  /**< @brief #ETHER_MacSetMacAddrInternal */
#define BRCM_SWDSGN_ETHER_MACSETMACADDR_PROC                (0xCAU)  /**< @brief #ETHER_MacSetMacAddr */
#define BRCM_SWDSGN_ETHER_MACINIT_PROC                      (0xCBU)  /**< @brief #ETHER_MacInit */
#define BRCM_SWDSGN_ETHER_MACDEINIT_PROC                    (0xCCU)  /**< @brief #ETHER_MacDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTSETPARITYDISABLE_PROC     (0xCDU)  /**< @brief #ETHER_CntlrXmtSetParityDisable */
#define BRCM_SWDSGN_ETHER_CNTLRXMTENABLE_PROC               (0xCEU)  /**< @brief #ETHER_CntlrXmtEnable */
#define BRCM_SWDSGN_ETHER_CNTLRXMTRESET_PROC                (0xCFU)  /**< @brief #ETHER_CntlrXmtReset */
#define BRCM_SWDSGN_ETHER_XMTSETDESCTABLEADDR_PROC          (0xD0U)  /**< @brief #ETHER_XmtSetDescTableAddr */
#define BRCM_SWDSGN_ETHER_CNTLRRCVENABLE_PROC               (0xD1U)  /**< @brief #ETHER_CntlrRcvEnable */
#define BRCM_SWDSGN_ETHER_CNTLRRCVRESET_PROC                (0xD2U)  /**< @brief #ETHER_CntlrRcvReset */
#define BRCM_SWDSGN_ETHER_RCVSETDESCTABLEADDR_PROC          (0xD3U)  /**< @brief #ETHER_RcvSetDescTableAddr */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTINIT_PROC              (0xD4U)  /**< @brief #ETHER_CntlrIntXmtInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTDEINIT_PROC            (0xD5U)  /**< @brief #ETHER_CntlrIntXmtDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCHANINFOINIT_PROC         (0xD6U)  /**< @brief #ETHER_CntlrXmtChanInfoInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTRCVINIT_PROC              (0xD7U)  /**< @brief #ETHER_CntlrIntRcvInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTRCVDEINIT_PROC            (0xD8U)  /**< @brief #ETHER_CntlrIntRcvDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCHANINFOINIT_PROC         (0xD9U)  /**< @brief #ETHER_CntlrRcvChanInfoInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCHANERRHANDLER_PROC       (0xDAU)  /**< @brief #ETHER_CntlrRcvChanErrHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCHANERRHANDLER_PROC       (0xDBU)  /**< @brief #ETHER_CntlrXmtChanErrHandler */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUERCVCOMPPKTS_PROC      (0xDCU)  /**< @brief #ETHER_CntlrDequeueRcvCompPkts */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUEXMTCOMPPKTS_PROC      (0xDDU)  /**< @brief #ETHER_CntlrDequeueXmtCompPkts */
#define BRCM_SWDSGN_ETHER_CNTLRRCVDMAERRIRQHANDLER_PROC     (0xDEU)  /**< @brief #ETHER_CntlrRcvDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTDMAERRIRQHANDLER_PROC     (0xDFU)  /**< @brief #ETHER_CntlrXmtDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRDMAERRIRQHANDLER_PROC        (0xE0U)  /**< @brief #ETHER_CntlrDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRRCVERRIRQHANDLER_PROC        (0xE1U)  /**< @brief #ETHER_CntlrRcvErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTERRIRQHANDLER_PROC        (0xE2U)  /**< @brief #ETHER_CntlrXmtErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCOMPIRQHANDLER_PROC       (0xE3U)  /**< @brief #ETHER_CntlrRcvCompIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCOMPIRQHANDLER_PROC       (0xE4U)  /**< @brief #ETHER_CntlrXmtCompIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRGPTIMERIRQHANDLER_PROC       (0xE5U)  /**< @brief #ETHER_CntlrGPTimerIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTPUTPKT_PROC               (0xE6U)  /**< @brief #ETHER_CntlrXmtPutPkt */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTGETPKT_PROC            (0xE7U)  /**< @brief #ETHER_CntlrIntXmtGetPkt */
#define BRCM_SWDSGN_ETHER_CNTLRXMTGETPKT_PROC               (0xE8U)  /**< @brief #ETHER_CntlrXmtGetPkt */
#define BRCM_SWDSGN_ETHER_CNTLRINTTXTSDONEIND_PROC          (0xE9U)  /**< @brief #ETHER_CntlrIntTxTSDoneInd */
#define BRCM_SWDSGN_ETHER_CNTLRRCVPUTPKT_PROC               (0xEAU)  /**< @brief #ETHER_CntlrRcvPutPkt */
#define BRCM_SWDSGN_ETHER_CNTLRRCVGETPKT_PROC               (0xEBU)  /**< @brief #ETHER_CntlrRcvGetPkt */
#define BRCM_SWDSGN_ETHER_DISABLEALLCHANNELS_PROC           (0xECU)  /**< @brief #ETHER_DisableAllChannels */
#define BRCM_SWDSGN_ETHER_CNTLRXMTINIT_PROC                 (0xEDU)  /**< @brief #ETHER_CntlrXmtInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVINIT_PROC                 (0xEEU)  /**< @brief #ETHER_CntlrRcvInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTINIT_PROC                 (0xEFU)  /**< @brief #ETHER_CntlrIntInit */
#define BRCM_SWDSGN_ETHER_CNTLRINIT_PROC                    (0xF0U)  /**< @brief #ETHER_CntlrInit */
#define BRCM_SWDSGN_ETHER_CNTLRGETCHANIDX_PROC              (0xF1U)  /**< @brief #ETHER_CntlrGetChanIdx */
#define BRCM_SWDSGN_ETHER_CNTLRXMTGETBUFFER_PROC            (0xF2U)  /**< @brief #ETHER_CntlrXmtGetBuffer */
#define BRCM_SWDSGN_ETHER_CNTLRXMTSEND_PROC                 (0xF3U)  /**< @brief #ETHER_CntlrXmtSend */
#define BRCM_SWDSGN_ETHER_CNTLRRCVDEINIT_PROC               (0xF4U)  /**< @brief #ETHER_CntlrRcvDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTDEINIT_PROC               (0xF5U)  /**< @brief #ETHER_CntlrXmtDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTDEINIT_PROC               (0xF6U)  /**< @brief #ETHER_CntlrIntDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRDEINIT_PROC                  (0xF7U)  /**< @brief #ETHER_CntlrDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRSETMODE_PROC                 (0xF8U)  /**< @brief #ETHER_CntlrSetMode */
#define BRCM_SWDSGN_ETHER_CNTLRGETMODE_PROC                 (0xF9U)  /**< @brief #ETHER_CntlrGetMode */
#define BRCM_SWDSGN_ETHER_CNTLRGETSTATS_PROC                (0xFAU)  /**< @brief #ETHER_CntlrGetStats */
#define BRCM_SWDSGN_ETHER_CNTLRXMTMARKTSPKT_PROC            (0xFBU)  /**< @brief #ETHER_CntlrXmtMarkTSPkt */
#define BRCM_SWDSGN_ETHER_CNTLRXMTVALIDATEBUFFER_PROC       (0xFCU)  /**< @brief #ETHER_CntlrXmtValidateBuffer */
#define BRCM_SWDSGN_ETHER_CNTLRISXMTPKTTSMARKED_PROC        (0xFDU)  /**< @brief #ETHER_CntlrIsXmtPktTSMarked */
#define BRCM_SWDSGN_ETHER_CNTLRSETGPTIMER_PROC              (0xFEU)  /**< @brief #ETHER_CntlrSetGPTimer */
#define BRCM_SWDSGN_ETHER_INTIRQHANDLER_PROC                (0xFFU)  /**< @brief #ETHER_IntIRQHandler */
#define BRCM_SWDSGN_ETHER_CMDHANDLER_PROC                   (0xFFU)  /**< @brief #ETHER_CmdHandler */
#define BRCM_SWDSGN_ETHER_SVCIO_TYPE                        ()  /**< @brief #ETHER_SVCIOType */
#define BRCM_SWDSGN_ETHER_SYSCMDHANDLER_PROC                (0xFFU)  /**< @brief #ETHER_SysCmdHandler */
/** @} */

/* Internal APIs */
#define ETHER_CNTLR_XMT_DEINIT_API_ID               (0xD0U)
#define ETHER_CNTLR_RCV_DEINIT_API_ID               (0xD1U)
#define XMT_INIT_API_ID                             (0xD2U)
#define XMT_ENABLE_API_ID                           (0xD3U)
#define RCV_ENABLE_API_ID                           (0xD4U)
#define XMT_DEINIT_API_ID                           (0xD5U)
#define RCV_DEINIT_API_ID                           (0xD6U)
#define DEQUEUE_RCV_COMP_PKTS_API_ID                (0xD7U)
#define DEQUEUE_XMT_COMP_PKTS_API_ID                (0xD8U)
#define DMA_ERR_IRQ_HANDLER_API_ID                  (0xD9U)
#define RCV_ERR_IRQ_HANDLER_API_ID                  (0xDAU)
#define XMT_ERR_IRQ_HANDLER_API_ID                  (0xDBU)
#define PROCESS_XMTD_PKTS_API_ID                    (0xDCU)
#define PROCESS_RCVD_PKTS_API_ID                    (0xDDU)
#define RCV_INIT_API_ID                             (0xDEU)
#define BUS_ERR_IRQ_HANDLER_API_ID                  (0xDFU)

#define ETHER_RXDMA_PKT_SIZE      ETHER_ALIGN(ETHER_FRAMESIZE, 64)
#define ETHER_TXDMA_PKT_SIZE      ETHER_ALIGN(ETHER_FRAMESIZE, 32)

/**
 * Macro to set DMA descriptor field
 */
#define ETHER_SetDMADescField(aDesc, aField, aValue) \
    do {                                       \
        (aDesc.aField = aValue);               \
        CORTEX_DMB();                          \
        CORTEX_ISB();                          \
    } while (0)

/**
 * GMAC Transmit DMA descriptor
 */
volatile typedef struct {
    /** Number of valid bytes in data buffer: Written by software */
    uint32_t dataBufSize        :16;
#define TX_DMA_DESC_BUF_SIZE_ZERO       (0UL)
    /** (UN) Underrun: Written by hardware */
    uint32_t underrun           :1;
    /** (LCOL) Late Collision: Written by hardware */
    uint32_t lateCollision      :1;
    /** (XCOL) Excessive collision: Written by hardware */
    uint32_t excessCollision    :1;
    uint32_t reserved1          :2;
    /** (CRP) Replace existing CRC: Written by software */
    uint32_t replaceCRC         :1;
#define TX_DMA_DESC_REPLACE_CRC_DIS     (0UL)
#define TX_DMA_DESC_REPLACE_CRC_EN      (1UL)
    /** (CAP) Append hardware generated CRC: Written by software */
    uint32_t appendCRC          :1;
#define TX_DMA_DESC_APPEND_CRC_DIS      (0UL)
#define TX_DMA_DESC_APPEND_CRC_EN       (1UL)
    uint32_t reserved2          :7;
    /** (EOP) End of packet: Written by software */
    uint32_t endOfPacket        :1;
#define TX_DMA_DESC_EOP_DIS             (0UL)
#define TX_DMA_DESC_EOP_EN              (1UL)
    /** (SOP) Start of packet: Written by software */
    uint32_t startOfPacket      :1;
#define TX_DMA_DESC_SOP_DIS             (0UL)
#define TX_DMA_DESC_SOP_EN              (1UL)
    /** Data buffer pointer: Written by software */
    uint8_t *dataBufPtr;
#define TX_DMA_DESC_BUF_PTR_NULL        (NULL)
} ETHER_TxDmaDescType;

#define ETHER_TX_DMA_DESC_SIZE            (sizeof(ETHER_TxDmaDescType))

#define ETHER_SetTxDMADescFields(aDesc, aBufSize, aBufPtr, aRCRC, aACRC, aEOP, aSOP)\
    do {                                                                      \
        aDesc.dataBufSize = aBufSize;                                         \
        aDesc.replaceCRC = aRCRC;                                             \
        aDesc.appendCRC = aACRC;                                              \
        aDesc.endOfPacket = aEOP;                                             \
        aDesc.startOfPacket = aSOP;                                           \
        aDesc.dataBufPtr = aBufPtr;                                           \
        CORTEX_DMB();                                                                \
        CORTEX_ISB();                                                                \
    } while (0)

/**
 * GMAC Receive DMA descriptor
 */
volatile typedef struct {
    /** Size of data buffer: Written by software
     * 0x0UL is in valid and If hardware updates value 0x0, then HW error */
    uint32_t dataBufSize        :16;
    uint32_t reserved1          :2;
    /** (OF) Recieve FIFO overflow: Written by Rx DMA */
    uint32_t rxFifoOverflow     :1;
    /** (ER) Error status: Written by Rx DMA */
    uint32_t errorStatus        :1;
    /** (CF) 802.1 control frame received: Written by Rx DMA */
    uint32_t controlFrame       :1;
    /** (LS) Local station frame received: Written by Rx DMA */
    uint32_t localFrame         :1;
    /** (MC) Multicast frame received: Written by Rx DMA */
    uint32_t multicastFrame         :1;
    /** (BC) Broadcast frame received: Written by Rx DMA */
    uint32_t broadcastFrame         :1;
    uint32_t reserved2              :6;
    /** (EOP) End of packet: Written by Rx DMA */
    uint32_t endOfPacket        :1;
    /** (SOP) Start of packet: Written by Rx DMA */
    uint32_t startOfPacket      :1;
    /** Data buffer pointer: Written by software */
    uint8_t *dataBufPtr;
} ETHER_RxDmaDescType;

/**
 * GMAC Receive DMA descriptor
 */
volatile typedef struct {
    /** Size of data buffer: Written by software
     * 0x0UL is in valid and If hardware updates value 0x0, then HW error */
    uint32_t dataBufSize        :16;
    uint32_t reserved1          :2;
    /** (OF) Recieve FIFO overflow: Written by Rx DMA */
    uint32_t rxFifoOverflow     :1;
    /** (ER) Error status: Written by Rx DMA */
    uint32_t errorStatus        :1;
    /** (CF) 802.1 control frame received: Written by Rx DMA */
    uint32_t controlFrame       :1;
    /** (LS) Local station frame received: Written by Rx DMA */
    uint32_t localFrame         :1;
    /** (MC) Multicast frame received: Written by Rx DMA */
    uint32_t multicastFrame         :1;
    /** (BC) Broadcast frame received: Written by Rx DMA */
    uint32_t broadcastFrame         :1;
    uint32_t reserved2              :6;
    /** (EOP) End of packet: Written by Rx DMA */
    uint32_t endOfPacket        :1;
    /** (SOP) Start of packet: Written by Rx DMA */
    uint32_t startOfPacket      :1;
} ETHER_RxDmaDescStatusType;

#define ETHER_RX_DMA_DESC_SIZE            (sizeof(ETHER_RxDmaDescType))

#define ETHER_SetRxDMADescFields(aDesc, aBufSize, aBufPtr)    \
    do {                                                \
        aDesc.dataBufSize = (aBufSize);                 \
        aDesc.dataBufPtr = (aBufPtr);                   \
        CORTEX_DMB();                                   \
        CORTEX_ISB();                                   \
    } while (0)

#define ETHER_INIT_RXDESC(aBufPtr)                      \
{                                                       \
    .dataBufSize = sizeof(ETHER_RxPktType),             \
    .dataBufPtr = (aBufPtr),                            \
}

/* Buffer counts */
#define ETHER_TX_BUFF_COUNT           (10UL)
#define ETHER_RX_BUFF_COUNT           (10UL)

/**< Structure for Tx Channel info */
typedef struct {
    volatile ETHER_StateType    state;       /**< State of the channel */
    ETHER_TxBufMgrInfoType *const     bufMgrInfo;  /**< Buffer management info */
    uint32_t                    isErr;       /**< Flag to indicate error */
} ETHER_TxChanInfoType;

/**< Structure for RX channel info */
typedef struct {
    volatile ETHER_StateType    state;        /**< State of the channel */
    ETHER_RxBufMgrInfoType *const     bufMgrInfo;   /**< Buffer management info */
    uint32_t                    isErr;        /**< Flag to indicate error */
} ETHER_RxChanInfoType;

#define ETHER_INIT_TX_CHANNEL_INFO(aTxChanState, aBufMgrInfo, aIsErr)         \
{                                                                       \
    .state = aTxChanState,                                              \
    .bufMgrInfo = aBufMgrInfo,                                          \
    .isErr = aIsErr,                                                    \
}

#define ETHER_INIT_RX_CHANNEL_INFO(aRxState, aBufMgrInfo, aIsErr)             \
{                                                                       \
    .state = aRxState,                                                  \
    .bufMgrInfo = aBufMgrInfo,                                          \
    .isErr = aIsErr,                                                    \
}


/**< Structure for holding statistics for Ethernet controller */
typedef struct {
    /**< Transfer complete interrupt count for TX channel */
    uint32_t    txChCompIRQCnt;
    /**< Receive complete interrupt count for RX channel */
    uint32_t    rxChCompIRQCnt;
    uint32_t    lastIRQStatus; /**< Last IRQ status */
} ETHER_StatsType;

#define INIT_ETHER_STATS_ZERO                   \
                {                               \
                    .txChCompIRQCnt = 0UL,      \
                    .rxChCompIRQCnt = 0UL,      \
                    .lastIRQStatus = 0UL,       \
                }

#define ETHER_TX_PARITY_EN        (0UL) /**< TX parity enable */
#define ETHER_TX_PARITY_DIS       (1UL) /**< TX parity disable */

#define ETHER_DMA_DESC_ERR_MASK           (INTSTATUS_DESCRERR_MASK|   \
                                    INTSTATUS_DATAERR_MASK|     \
                                    INTSTATUS_DESCPROTOERR_MASK)

#define ETHER_TX_DMA_ETHER_DESC_TABLE_SIZE      (sizeof(ETHER_TxDmaDescTbl))

#define RX_FREE_BUF_WATERMARK                  (6UL)
#define TX_READY_BUF_WATERMARK                 (0UL)
/* Higihly loaded system */
#define TXFIFO_FULL_THRESHOLD   (4UL)
#define TXFIFO_EMPTY_THRESHOLD  (6UL)

#define TX_DMA_DESC_COUNT       (ETHER_TX_BUFF_COUNT)
#define RX_DMA_DESC_COUNT       (ETHER_RX_BUFF_COUNT)

#define TX_LAST_DESC_IDX        \
    (((GMAC_REGS->tswptr - ((uint32_t)(&ETHER_TxDmaDescTbl[0UL])))/ETHER_TX_DMA_DESC_SIZE) \
     % TX_DMA_DESC_COUNT)
#define TX_CURR_DESC_IDX        \
    (((GMAC_REGS->tbdptr - ((uint32_t)(&ETHER_TxDmaDescTbl[0UL])))/ETHER_TX_DMA_DESC_SIZE) \
     % TX_DMA_DESC_COUNT)

#define RX_LAST_DESC_IDX        \
    (((GMAC_REGS->rswptr - ((uint32_t)(&ETHER_RxDmaDescTbl[0UL])))/ETHER_RX_DMA_DESC_SIZE) \
     % RX_DMA_DESC_COUNT)

#define RX_CURR_DESC_IDX        \
    (((GMAC_REGS->rbdptr - ((uint32_t)(&ETHER_RxDmaDescTbl[0UL])))/ETHER_RX_DMA_DESC_SIZE) \
    % RX_DMA_DESC_COUNT)

#define RX_INIT_PKT_QUEUE_CNT   (ETHER_RX_BUFF_COUNT - 1UL)

/* GMAC:- Rx Buffers to be aligned to 64bytes and Tx buffers to
 *        be aligned to 4Bytes. Document descriptor table description
 *        mentions that on Tx side there is not alignment restrictions.
 */
static ETHER_TxPktType COMP_SECTION(".ethernet.tx0pktbuff")
            TxPkt[ETHER_TX_BUFF_COUNT] COMP_ALIGN(32);
static ETHER_RxPktType COMP_SECTION(".ethernet.rx0pktbuff")
            ETHER_RxPkt[ETHER_RX_BUFF_COUNT] COMP_ALIGN(64);

/* Tx Descriptors = 2n & start address of DMA descriptor table should
 * be aligned to 64Bytes  */
static ETHER_TxDmaDescType COMP_SECTION(".ethernet.tx0desctbl")
            ETHER_TxDmaDescTbl[TX_DMA_DESC_COUNT] COMP_ALIGN(64);

/* Rx Descriptors = 2n & start address of DMA descriptor table should
 * be aligned to 64Bytes  */
static ETHER_RxDmaDescType COMP_SECTION(".ethernet.rx0desctbl")
            ETHER_RxDmaDescTbl[RX_DMA_DESC_COUNT] COMP_ALIGN(64);

/* Global variables */
static GMAC_RDBType * const GMAC_REGS = (GMAC_RDBType *const)GMAC_BASE;

/* Packet buffer information */
static ETHER_TxPktBuffInfoType COMP_SECTION(".data.drivers")
                        TxPktBuffInfo[ETHER_TX_BUFF_COUNT];
static ETHER_RxPktBuffInfoType COMP_SECTION(".data.drivers")
                        ETHER_RxPktBuffInfo[ETHER_RX_BUFF_COUNT];

/* Buffer management information */
static ETHER_TxBufMgrInfoType COMP_SECTION(".data.drivers")
    ETHER_TxBufMgrInfo = ETHER_INITTXBUFMGRINFO(TxPktBuffInfo, ETHER_TX_BUFF_COUNT,
                        TX_DMA_DESC_COUNT, 0UL, 0UL, 0UL, 0UL);

static ETHER_RxBufMgrInfoType COMP_SECTION(".data.drivers")
    ETHER_RxBufMgrInfo = ETHER_INITRXBUFMGRINFO(ETHER_RxPktBuffInfo, ETHER_RX_BUFF_COUNT,
                        0UL, 0UL, 0UL);

static ETHER_StatsType COMP_SECTION(".data.drivers")
    ETHER_Stats = INIT_ETHER_STATS_ZERO;

static ETHER_RxChanInfoType COMP_SECTION(".data.drivers") ETHER_RxChanInfo =
        ETHER_INIT_RX_CHANNEL_INFO(ETHER_CHANSTATE_UNINIT, &ETHER_RxBufMgrInfo, FALSE);

static ETHER_TxChanInfoType COMP_SECTION(".data.drivers") ETHER_TxChanInfo =
        ETHER_INIT_TX_CHANNEL_INFO(ETHER_CHANSTATE_UNINIT, &ETHER_TxBufMgrInfo, FALSE);

static uint32_t COMP_SECTION(".data.drivers") ETHER_CntlrState = ETHER_STATE_UNINIT;
static uint8_t COMP_SECTION(".data.drivers") ETHER_MacAddr[6UL] = {0UL};

/*********************************************************/
#define UMAC_CMDCFG_ETH_SPEED_10MBPS            (0UL)
#define UMAC_CMDCFG_ETH_SPEED_100MBPS           (1UL)
#define UMAC_CMDCFG_ETH_SPEED_1GBPS             (2UL)
#define UMAC_CMDCFG_ETH_SPEED_2_5GBPS           (3UL)

#define UMAC_CMDCFG_HD_HALF_DUPLEX              (1UL)
#define UMAC_CMDCFG_HD_FULL_DUPLEX              (0UL)

/**< Structure for Xcvr device */
typedef struct {
    uint32_t                    init;      /**< initialized? */
    uint32_t                    enabled;    /**< Xcvr enabled? */
    ETHER_SpeedType         speed;      /**< Xcvr speed */
    ETHER_DuplexModeType    duplexMode;   /**< Xcvr Duplex Mode */
    uint8_t                     macAddr[6UL]; /**< MAC Addresss configured */
} ETHER_MacDevType;

static ETHER_MacDevType COMP_SECTION(".data.drivers") ETHER_MacDev = {
    .init = FALSE,
    .enabled = FALSE,
    .speed = 0UL,
    .macAddr = {0UL},
};

COMP_INLINE int32_t ETHER_MacCheckConfigParams(
                    const ETHER_CfgType *const aCfg)
{

    uint8_t macAddrZero[6UL] = {(uint8_t)0};
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    /* check supported speeds */
    if ((aCfg->speed != ETHER_SPEED_10MBPS)
            && (aCfg->speed != ETHER_SPEED_100MBPS)
            && (aCfg->speed != ETHER_SPEED_1000MBPS)) {
        goto err;
    }

    if ((aCfg->duplexMode != ETHER_DUPLEX_MODE_FULL) &&
            (aCfg->duplexMode != ETHER_DUPLEX_MODE_HALF)) {
        goto err;
    }

    if (aCfg->maxRxFrmLen > ETHER_FRAMESIZE) {
        goto err;
    }

    if ((aCfg->macAddr != NULL) &&
            (memcmp(aCfg->macAddr, macAddrZero, 6UL) == 0)) {
        goto err;
    }
    ret = BCM_ERR_OK;

err:
    return ret;
}

static void ETHER_MacEnableTxReset(uint32_t aEnable)
{
    uint32_t regVal = GMAC_REGS->maccfg;

    if (aEnable == TRUE) {
        regVal |= GMAC_MACCFG_SRST_MASK;
        regVal &= ~GMAC_MACCFG_TXEN_MASK;
    } else {
        regVal &= ~GMAC_MACCFG_SRST_MASK;
        regVal |= GMAC_MACCFG_TXEN_MASK;
    }
    GMAC_REGS->maccfg = regVal;
}

static void ETHER_MacEnableRxReset(uint32_t aEnable)
{
    uint32_t regVal = GMAC_REGS->maccfg;

    if (aEnable == TRUE) {
        regVal |= GMAC_MACCFG_SRST_MASK;
        regVal &= ~GMAC_MACCFG_RXEN_MASK;
    } else {
        regVal &= ~GMAC_MACCFG_SRST_MASK;
        regVal |= GMAC_MACCFG_RXEN_MASK;
    }
    GMAC_REGS->maccfg = regVal;
}

static int32_t ETHER_MacDisableTxRx(void)
{
    int32_t retVal = BCM_ERR_OK;

    if (TRUE == ETHER_MacDev.init) {
        ETHER_MacEnableTxReset(TRUE);
        ETHER_MacEnableRxReset(TRUE);
        ETHER_MacDev.enabled = FALSE;
    } else {
        retVal = BCM_ERR_UNINIT;
    }
    return retVal;
}

static int32_t ETHER_MacEnableTxRx(void)
{
    int32_t retVal = BCM_ERR_OK;

    if (TRUE == ETHER_MacDev.init) {
        ETHER_MacEnableTxReset(FALSE);
        ETHER_MacEnableRxReset(FALSE);
        ETHER_MacDev.enabled = TRUE;
    } else {
        retVal = BCM_ERR_UNINIT;
    }
    return retVal;
}

static int32_t ETHER_MacIsEnabled(ETHER_HwIDType aID,
                        uint32_t * const aEnabled)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aEnabled)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (TRUE == ETHER_MacDev.init) {
       *aEnabled = ETHER_MacDev.enabled;
    } else {
        retVal = BCM_ERR_UNINIT;
    }
    return retVal;
}

static void ETHER_MacSetLoopbackMode(uint32_t enable, uint32_t local)
{
    uint32_t regVal = GMAC_REGS->maccfg;

    if ((enable == TRUE) && (local == TRUE)) {
        regVal |= GMAC_MACCFG_LLB_MASK;
    } else if ((enable == TRUE) && (local == FALSE)) {
        regVal |= GMAC_MACCFG_RLB_MASK;
    } else if ((enable == FALSE) && (local == TRUE)) {
        regVal &= ~GMAC_MACCFG_LLB_MASK;
    } else {
        regVal &= ~GMAC_MACCFG_RLB_MASK;
    }

    GMAC_REGS->maccfg = regVal;
}

static void ETHER_MacEnableLoopback(ETHER_HwIDType aID, uint32_t aLocal)
{
    if (ETHER_HW_ID_MAX > aID) {
        ETHER_MacSetLoopbackMode(TRUE, aLocal);
    }
}

static int32_t ETHER_MacGetMacAddr(ETHER_HwIDType aID, uint8_t * const aMacAddr)
{
    int32_t retVal;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aMacAddr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (TRUE == ETHER_MacDev.init) {
            BCM_MemCpy(aMacAddr, ETHER_MacDev.macAddr, 6UL * sizeof(uint8_t));
            retVal = BCM_ERR_OK;
        } else {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

static void ETHER_MacSetMacAddrInternal(const uint8_t *const aMacAddr)
{
    uint32_t macAddr0;
    uint32_t macAddr1;

    macAddr0 = ((((uint32_t)aMacAddr[1UL]) << 8UL)
            | (((uint32_t)aMacAddr[0UL]) << 0UL));

    macAddr1 = ((((uint32_t)aMacAddr[5UL]) << 24UL)
            | (((uint32_t)aMacAddr[4UL]) << 16UL)
            | (((uint32_t)aMacAddr[3UL]) << 8UL)
            | (((uint32_t)aMacAddr[2UL]) << 0UL));

    /* Careful: Register is other way around that it usually looks like */
    GMAC_REGS->macaddr1 = macAddr0;
    GMAC_REGS->macaddr0 = macAddr1;
    BCM_MemCpy(ETHER_MacDev.macAddr, aMacAddr, 6UL * sizeof(uint8_t));
}

static int32_t ETHER_MacSetMacAddr(ETHER_HwIDType aID,
                                const uint8_t *const aMacAddr)
{
    int32_t retVal;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aMacAddr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (0UL == ETHER_MacDev.init) {
        retVal = BCM_ERR_UNINIT;
    } else {
        /* Set ETHER_MacAddr */
        ETHER_MacSetMacAddrInternal(aMacAddr);
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

static int32_t ETHER_MacInit(ETHER_HwIDType aID,
                    const ETHER_CfgType * const aConfig)
{
    int32_t retVal;
    uint32_t regVal;
    uint32_t speedMask;
    uint32_t modeMask;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (TRUE == ETHER_MacDev.init) {
            retVal = BCM_ERR_INVAL_STATE;
            goto err;
        }

        retVal = ETHER_MacCheckConfigParams(aConfig);
        if (retVal != BCM_ERR_OK) {
            goto err;
        }

        /* Disable TxRx */
        ETHER_MacEnableTxReset(TRUE);
        ETHER_MacEnableRxReset(TRUE);

        /* Set max frame length. */
        GMAC_REGS->maxfrm = aConfig->maxRxFrmLen;

        regVal = GMAC_REGS->maccfg;
        if (aConfig->speed == ETHER_SPEED_10MBPS) {
            speedMask = UMAC_CMDCFG_ETH_SPEED_10MBPS;
            ETHER_MacDev.speed = ETHER_SPEED_10MBPS;
        } else if (aConfig->speed == ETHER_SPEED_100MBPS) {
            speedMask = UMAC_CMDCFG_ETH_SPEED_100MBPS;
            ETHER_MacDev.speed = ETHER_SPEED_100MBPS;
        } else {
            speedMask = UMAC_CMDCFG_ETH_SPEED_1GBPS;
            ETHER_MacDev.speed = ETHER_SPEED_1000MBPS;
        }

        if (aConfig->duplexMode == ETHER_DUPLEX_MODE_FULL) {
            modeMask = UMAC_CMDCFG_HD_FULL_DUPLEX;
            ETHER_MacDev.duplexMode = ETHER_DUPLEX_MODE_FULL;
        } else {
            modeMask = UMAC_CMDCFG_HD_HALF_DUPLEX;
            ETHER_MacDev.duplexMode = UMAC_CMDCFG_HD_HALF_DUPLEX;
        }

        /* Set speed */
        regVal &= ~GMAC_MACCFG_ESPD_MASK;
        regVal |= (speedMask << GMAC_MACCFG_ESPD_SHIFT)
                    & GMAC_MACCFG_ESPD_MASK;
        /* Enable mode */
        regVal &= ~GMAC_MACCFG_HDEN_MASK;
        regVal |= (modeMask << GMAC_MACCFG_HDEN_SHIFT)
                    & GMAC_MACCFG_HDEN_MASK;

        /* Enable RX FIFO overflow logic */
        regVal |= GMAC_MACCFG_OFEN_MASK;

        /* Ignore Rx Pause Frames */
        regVal |= GMAC_MACCFG_PDIS_MASK;
        /* Disable Tx Pause Frames */
        regVal |= GMAC_MACCFG_TPD_MASK;

        GMAC_REGS->maccfg = regVal;

        if (NULL != aConfig->macAddr) {
            ETHER_MacSetMacAddrInternal(aConfig->macAddr);
        }

        ETHER_MacDev.init = TRUE;
      }

err:
    return retVal;
}

static int32_t ETHER_MacDeInit(ETHER_HwIDType aID)
{
    int32_t retVal;
    uint8_t macAddrZero[6UL] = {(uint8_t)0};

    if (ETHER_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (TRUE != ETHER_MacDev.init) {
        retVal = BCM_ERR_UNINIT;
    } else {
        /* Disable TxRx */
        ETHER_MacEnableTxReset(TRUE);
        ETHER_MacEnableRxReset(TRUE);

        /* Set max frame length. */
        GMAC_REGS->maxfrm = 0x0UL;

        ETHER_MacSetMacAddrInternal(macAddrZero);
        ETHER_MacDev.init = FALSE;
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/*********************************************************/

static void ETHER_CntlrReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AMC_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

static void ETHER_CntlrXmtEnable(uint32_t aEnable)
{
    if (TRUE == aEnable) {
        /* Enable Tx DMA */
        GMAC_REGS->eth_ctrl &= ~GMAC_CTRL_GTS_MASK;
    } else {
        /* Disable Tx DMA */
        GMAC_REGS->eth_ctrl |= GMAC_CTRL_GTS_MASK;
    }
}

/*
 * TX channel reset sequence:
 * -> disable the channel (clear XmtEn bit in XmtControl register)
 * -> Poll until XmtState field in XmtStatus0 register indicated that channel
 *    is disabled
 * -> enable the channel (set Xmten bit in XmtControl Register)
 */
static void ETHER_CntlrXmtReset(void)
{
    ETHER_CntlrXmtEnable(FALSE);
    ETHER_CntlrXmtEnable(TRUE);

    /* Disable Tx DMA */
}

static void ETHER_CntlrRcvEnable(uint32_t aEnable)
{
    if (TRUE == aEnable) {
        GMAC_REGS->eth_ctrl &= ~GMAC_CTRL_GRS_MASK;
    } else {
        GMAC_REGS->eth_ctrl |= GMAC_CTRL_GRS_MASK;
    }
}

COMP_INLINE void ETHER_CntlrRcvReset(void)
{
    ETHER_CntlrRcvEnable(FALSE);
    ETHER_CntlrRcvEnable(TRUE);
}

static int32_t XmtChanInit(void)
{
    int32_t retVal = BCM_ERR_OK;

    /* Disable TX channel (deferred until ready to send) */
    ETHER_CntlrXmtEnable(FALSE);

    GMAC_REGS->eth_ctrl |= GMAC_CTRL_MEN_MASK;

    /* Program Tx DMA ring */
    GMAC_REGS->tbase = (uint32_t) ETHER_TxDmaDescTbl;
    GMAC_REGS->tbcfg &= ~(GMAC_TBCFG_TLEN_MASK | GMAC_TBCFG_TRBWMRK_MASK);
    GMAC_REGS->tbcfg |= (TX_READY_BUF_WATERMARK << GMAC_TBCFG_TRBWMRK_SHIFT)
                        & GMAC_TBCFG_TRBWMRK_MASK;
    GMAC_REGS->tbcfg |= (TX_DMA_DESC_COUNT << GMAC_TBCFG_TLEN_SHIFT)
                        & GMAC_TBCFG_TLEN_MASK;

    /* Initialize TBDPTR and TSWPTR */
    GMAC_REGS->tswptr = (uint32_t)(&ETHER_TxDmaDescTbl[TX_DMA_DESC_COUNT - 1UL]);
    GMAC_REGS->tbdptr = (uint32_t)ETHER_TxDmaDescTbl;

    /* Set TXFIFO full threshold */
    GMAC_REGS->txfifo_full = TXFIFO_FULL_THRESHOLD;

    /* Set TXFIFO empty threshold */
    GMAC_REGS->txfifo_empty = TXFIFO_EMPTY_THRESHOLD;

    /* Enable interrupts */
    GMAC_REGS->intr_mask |= (GMAC_INTR_MASK_BERR_MASK_MASK
                            | GMAC_INTR_MASK_TUN_MASK_MASK
                            | GMAC_INTR_MASK_PHY_MASK_MASK);

    GMAC_REGS->intr_mask |= GMAC_INTR_MASK_TXF_MASK_MASK;
    /* Clear INTR_CLR bits, these are sticky */
    GMAC_REGS->intr_clr = ~0;
    GMAC_REGS->intr_clr = 0;

    return retVal;
}

static void XmtChanDeInit(ETHER_ChanIDType aChan)
{
    /* Memset descriptor table */
    BCM_MemSet((void *)ETHER_TxDmaDescTbl, 0x0, ETHER_TX_DMA_ETHER_DESC_TABLE_SIZE);

    ETHER_CntlrDeInitTxBuf(ETHER_TxChanInfo.bufMgrInfo);

    if (ETHER_CHANSTATE_INIT != ATOMIC_CheckUpdate(&(ETHER_TxChanInfo.state),
                                                    ETHER_CHANSTATE_INIT,
                                                    ETHER_CHANSTATE_UNINIT)) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0, XMT_DEINIT_API_ID,
                BCM_ERR_INVAL_STATE, aChan, 0UL, 0UL, 0UL);
    }

    return ;
}

static void ETHER_CntlrXmtChanInfoInit(void)
{
    uint32_t i;

    /* Initialise ETHER_TxChanInfo */
    ETHER_TxChanInfo.isErr = FALSE;

    ETHER_CntlrInitTxBuf(ETHER_TxChanInfo.bufMgrInfo, TxPkt);

    BCM_MemSet((void *)ETHER_TxDmaDescTbl, 0x0, ETHER_TX_DMA_ETHER_DESC_TABLE_SIZE);
    for (i = 0UL; i < ETHER_TxChanInfo.bufMgrInfo->pktBuffCnt; i++) {
        ETHER_SetTxDMADescFields(ETHER_TxDmaDescTbl[i],
                        TX_DMA_DESC_BUF_SIZE_ZERO,
                        TX_DMA_DESC_BUF_PTR_NULL,
                        TX_DMA_DESC_REPLACE_CRC_DIS,
                        TX_DMA_DESC_APPEND_CRC_EN,
                        TX_DMA_DESC_EOP_EN,
                        TX_DMA_DESC_SOP_EN);
    }
}

static int32_t RcvChanInit(void)
{
    /* Disable receive channel */
    ETHER_CntlrRcvEnable(FALSE);

    /* Program Rx DMA ring */
    GMAC_REGS->rbase = (uint32_t)ETHER_RxDmaDescTbl;
    GMAC_REGS->rbcfg &= ~(GMAC_RBCFG_RFBWMRK_MASK | GMAC_RBCFG_RLEN_MASK);
    GMAC_REGS->rbcfg |= (RX_DMA_DESC_COUNT << GMAC_RBCFG_RLEN_SHIFT)
                        & GMAC_RBCFG_RLEN_MASK;
    GMAC_REGS->rbcfg |= (RX_FREE_BUF_WATERMARK << GMAC_RBCFG_RFBWMRK_SHIFT)
                        & GMAC_RBCFG_RFBWMRK_MASK;

    /* Enable receive buffer length, Rx buffers in multiple of 32Bytes */
    GMAC_REGS->rbuffctrl = ETHER_RXDMA_PKT_SIZE;

    /* Initialize RBDPTR and RSWPTR */
    GMAC_REGS->rbdptr = (uint32_t)ETHER_RxDmaDescTbl;
    GMAC_REGS->rswptr = (uint32_t)(&ETHER_RxDmaDescTbl[RX_INIT_PKT_QUEUE_CNT - 1UL]);

    GMAC_REGS->intr_mask |= (GMAC_INTR_MASK_RXF_MASK_MASK
                        | GMAC_INTR_MASK_ROV_MASK_MASK);
    /* Enable receive channel */
    ETHER_CntlrRcvEnable(TRUE);

    ETHER_MacEnableRxReset(FALSE);

    return BCM_ERR_OK;
}

static void ETHER_CntlrIntRcvDeInit(void)
{
    ETHER_CntlrDeInitRxBuf(ETHER_RxChanInfo.bufMgrInfo);

    if (ETHER_CHANSTATE_INIT != ATOMIC_CheckUpdate(&(ETHER_RxChanInfo.state),
                                                ETHER_CHANSTATE_INIT,
                                                ETHER_CHANSTATE_UNINIT)) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0, RCV_DEINIT_API_ID,
                BCM_ERR_INVAL_STATE, 0UL, 0UL, 0UL, 0UL);
    }

    return ;
}

static void ETHER_CntlrRcvChanInfoInit(void)
{
    uint32_t j;

    for (j = 0UL; j < ETHER_RxChanInfo.bufMgrInfo->pktBuffCnt; j++) {
        ETHER_SetRxDMADescFields(ETHER_RxDmaDescTbl[j],
                           sizeof(ETHER_RxPktType),
                           ETHER_RxPkt[j].pktBuff);
    }

    ETHER_RxChanInfo.isErr = FALSE;

    ETHER_CntlrInitRxBuf(ETHER_RxChanInfo.bufMgrInfo, ETHER_RxPkt, RX_INIT_PKT_QUEUE_CNT);
}

static int32_t ETHER_CntlrRcvChanErrHandler(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal;

    /* Re-initialise channel info and reset channel */
    ETHER_CntlrRcvChanInfoInit();
    retVal = RcvChanInit();
    if (BCM_ERR_OK == retVal) {
        ETHER_MacEnableRxReset(TRUE);
    }

    return retVal;
}

static int32_t ETHER_CntlrXmtChanErrHandler(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHER_HW_ID_MAX > aID) && (ETHER_TX_CHAN_CNT > aChan)) {

        /* Disable channel */
        ETHER_CntlrXmtEnable(FALSE);
        /* Re-initialise channel info */
        ETHER_CntlrXmtChanInfoInit();
        /* Enable channel */
        ETHER_CntlrXmtEnable(TRUE);

        retVal = BCM_ERR_OK;
    }
    return retVal;
}

static void ETHER_CntlrDequeueRcvCompPkts(void)
{
    uint32_t dequeueIdx;
    uint32_t currDescIdx;
    ETHER_RxPktType *rxPkt;
    int32_t retVal;

    /* Disable receive interrupt */
    GMAC_REGS->intr_mask &= ~GMAC_INTR_MASK_RXF_MASK_MASK;

    currDescIdx = RX_CURR_DESC_IDX;

    while (currDescIdx !=
            ETHER_CntlrPeekRxDequeueBuf(ETHER_RxChanInfo.bufMgrInfo)) {
        retVal = ETHER_CntlrDequeueRxBuf(ETHER_RxChanInfo.bufMgrInfo,
                (uint8_t **)&rxPkt, &dequeueIdx);
        if (BCM_ERR_OK == retVal) {
            DCACHE_SysInvalRange((uint32_t)rxPkt, sizeof(ETHER_RxPktType));
            memcpy(&(rxPkt->rxPktInfo),
                    (void *)&(ETHER_RxDmaDescTbl[dequeueIdx]),
                    sizeof(ETHER_RxDmaDescStatusType));
        } else {
            break;
        }
    }

    (ETHER_Stats.rxChCompIRQCnt)++;

    /* Enable receive interrupt */
    GMAC_REGS->intr_mask |= (GMAC_INTR_MASK_RXF_MASK_MASK);
    return ;
}

static void ETHER_CntlrDequeueXmtCompPkts(void)
{
    uint32_t currDescIdx;
    int32_t retVal = BCM_ERR_OK;

    /* Disable interrupt */
    GMAC_REGS->intr_mask &= ~(GMAC_INTR_MASK_TXF_MASK_MASK);

    currDescIdx = TX_CURR_DESC_IDX;

    while (currDescIdx !=
            ETHER_CntlrPeekTxDequeueBuf(ETHER_TxChanInfo.bufMgrInfo)) {
        retVal = ETHER_CntlrDequeueTxBuf(ETHER_TxChanInfo.bufMgrInfo);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }

    (ETHER_Stats.txChCompIRQCnt)++;
    /* Enable transmit interrupt */
    GMAC_REGS->intr_mask |= GMAC_INTR_MASK_TXF_MASK_MASK;
}

static void ETHER_CntlrRcvErrIRQHandler(uint32_t aIntStatus)
{
    /* Receive DMA descriptor underflow error
     * Reset & deinitialise the receive channel.
     */
    if ((aIntStatus & GMAC_INTR_ROV_MASK) == GMAC_INTR_ROV_MASK) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0, RCV_ERR_IRQ_HANDLER_API_ID,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);
        /* - Assert graceful stop for RX and TX
         * - Clear interrupts
         * - Assert MAC reset
         * - De-assert graceful stop
         * - De-assert MAC reset
         * - Set Error flag
         */
        GMAC_REGS->eth_ctrl |= GMAC_CTRL_GRS_MASK;
        /* TODO: Wait for graceful stop */
        /* TODO: Wait for reaching Tx Threshold */
        ETHER_MacEnableRxReset(TRUE);
        GMAC_REGS->eth_ctrl &= ~GMAC_CTRL_GRS_MASK;
        ETHER_MacEnableRxReset(FALSE);
        ETHER_RxChanInfo.isErr = TRUE;
    }
}

static void ETHER_CntlrXmtErrIRQHandler(uint32_t aIntStatus)
{
    /* transmit fifo overflow? */
    if ((aIntStatus & GMAC_INTR_TUN_MASK) == GMAC_INTR_TUN_MASK) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0, XMT_ERR_IRQ_HANDLER_API_ID,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);
        /* Set isErr flag */
        ETHER_TxChanInfo.isErr = TRUE;
    }
}

static void BusErrIRQHandler(uint32_t aIntStatus)
{
    if ((aIntStatus & GMAC_INTR_BERR_MASK) == GMAC_INTR_BERR_MASK) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0, BUS_ERR_IRQ_HANDLER_API_ID,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);
        /* Set isErr flag */
        ETHER_TxChanInfo.isErr = TRUE;
        ETHER_RxChanInfo.isErr = TRUE;
    }
}

static void ETHER_CntlrRcvCompIRQHandler(uint32_t aIntStatus)
{
    /* reciever interrupt
     * Process the RX packets
     */
    if ((aIntStatus & GMAC_INTR_RXF_MASK) == GMAC_INTR_RXF_MASK) {
        ETHER_CntlrDequeueRcvCompPkts();
    }
}

static void ETHER_CntlrXmtCompIRQHandler(uint32_t aIntStatus)
{
    /* TX complete interrupt
     * free TX packets and give callback
     */
    if ((aIntStatus & GMAC_INTR_TXF_MASK) == GMAC_INTR_TXF_MASK) {
        ETHER_CntlrDequeueXmtCompPkts();
    }
}

static int32_t ETHER_CntlrXmtPutPkt(ETHER_HwIDType aID)
{
    return ETHER_CntlrDeallocTxBuf(ETHER_TxChanInfo.bufMgrInfo);
}

static int32_t ETHER_CntlrXmtGetPkt(ETHER_HwIDType aID,
                            uint32_t *const aTxBufIdx,
                            uint32_t *const aRaiseTxCb)
{
    int32_t retVal;
    uint32_t buffIdx;
    uint32_t flags;

    retVal = ETHER_CntlrGetDequeuedTxBuf(ETHER_TxChanInfo.bufMgrInfo,
                                    &buffIdx, &flags);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    *aTxBufIdx = buffIdx;
    if ((ETHER_PKTFLAGS_CBEN == (flags & ETHER_PKTFLAGS_CBEN)) &&
            (ETHER_PKTFLAGS_1588TXCONF != (flags & ETHER_PKTFLAGS_1588TXCONF))) {
        *aRaiseTxCb = TRUE;
        retVal = BCM_ERR_OK;
    } else {
        *aRaiseTxCb = FALSE;
        retVal = ETHER_CntlrXmtPutPkt(aID);
    }

err:
    if ((BCM_ERR_EAGAIN == retVal) && (TRUE == ETHER_TxChanInfo.isErr)) {
        retVal = ETHER_CntlrXmtChanErrHandler(aID, 0UL);
    }
    return retVal;
}

static int32_t ETHER_CntlrIntTxTSDoneInd(ETHER_HwIDType aID,
                             uint32_t aBufIdx)
{
    int32_t retVal ;
    INTR_FlagType intFlags = INTR_Suspend();
    ETHER_TxPktBuffInfoType pktBuffInfo = ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx];
    ETHER_TxPktBuffInfoType * pktBuffInfoPtr =
                            &(ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx]);
    INTR_Resume(intFlags);

    if (ETHER_PKTFLAGS_1588TXCONF == (pktBuffInfo.flags & ETHER_PKTFLAGS_1588TXCONF)) {
        if (ETHER_PKTBUFFSTATE_FREE == pktBuffInfo.state) {
            pktBuffInfoPtr->flags &= ~ETHER_PKTFLAGS_1588TXCONF;
            retVal = BCM_ERR_OK;
        } else if ((ETHER_PKTBUFFSTATE_DEQUEUED == pktBuffInfo.state)
                || (ETHER_PKTBUFFSTATE_QUEUED == pktBuffInfo.state)) {
            pktBuffInfoPtr->flags &= ~ETHER_PKTFLAGS_1588TXCONF;
            retVal = BCM_ERR_EAGAIN;
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ETHER_CntlrReportError((uint8_t)aID,
                    BRCM_SWDSGN_ETHER_CNTLRINTTXTSDONEIND_PROC,
                    BCM_ERR_UNKNOWN, aBufIdx, pktBuffInfo.state, 0UL, __LINE__);
        }
    } else {
        retVal = BCM_ERR_UNKNOWN;
        ETHER_CntlrReportError((uint8_t)aID,
                BRCM_SWDSGN_ETHER_CNTLRINTTXTSDONEIND_PROC,
                BCM_ERR_UNKNOWN, aBufIdx, pktBuffInfo.state, 0UL, __LINE__);
    }

    return retVal;
}

static void ETHER_CntlrRcvPutPkt(ETHER_HwIDType aID)
{
    uint32_t lastDescIdx;

    ETHER_CntlrDeallocRxBuf(ETHER_RxChanInfo.bufMgrInfo);

    lastDescIdx = RX_LAST_DESC_IDX;

    /* Queue the packet buffer to DMA */
    lastDescIdx = ModInc(lastDescIdx, ETHER_RX_BUFF_COUNT);

    ETHER_CntlrQueueRxBuf(ETHER_RxChanInfo.bufMgrInfo);
    GMAC_REGS->rswptr = (uint32_t)(&ETHER_RxDmaDescTbl[lastDescIdx]);
}

static int32_t ETHER_CntlrRcvGetPkt(ETHER_HwIDType aID,
                            uint32_t *const aBufIdx,
                            uint8_t **const aPkt,
                            uint32_t *const aRaiseRxCb,
                            uint32_t *const aPktLen)
{
    int32_t retVal;
    uint32_t errData[2];
    ETHER_RxPktType *rxPkt;
    ETHER_RxDmaDescStatusType *rxDmaDescStatus;

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo.state) {
        retVal = BCM_ERR_EAGAIN;
        goto err;
    }

    *aRaiseRxCb = FALSE;
    *aPktLen = 0UL;

    retVal = ETHER_CntlrGetDequeuedRxBuf(ETHER_RxChanInfo.bufMgrInfo,
                                    (uint8_t **)&rxPkt, aBufIdx);
    if (BCM_ERR_OK == retVal) {
        DCACHE_SysInvalRange((uint32_t)rxPkt, sizeof(ETHER_RxPktType));

        rxDmaDescStatus = (ETHER_RxDmaDescStatusType *)&(rxPkt->rxPktInfo);
        if ((0UL == rxDmaDescStatus->errorStatus)
                && (ETHER_RxDmaDescTbl[*aBufIdx].dataBufSize > 4U)) {
            *aRaiseRxCb = TRUE;
            *aPkt = &(rxPkt->pktBuff[0]);
            *aPktLen = (ETHER_RxDmaDescTbl[*aBufIdx].dataBufSize - 4U);
        } else {
            errData[0] = rxDmaDescStatus->errorStatus;
            errData[1] = ETHER_RxDmaDescTbl[*aBufIdx].dataBufSize;
            ETHER_CntlrRcvPutPkt(aID);
            ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                    PROCESS_RCVD_PKTS_API_ID, BCM_ERR_DATA_INTEG, errData[0],
                    errData[1], 0UL, 0UL);
        }
    }

err:
    if ((BCM_ERR_EAGAIN == retVal) && (TRUE == ETHER_RxChanInfo.isErr)) {
        retVal = ETHER_CntlrRcvChanErrHandler(aID, 0UL);
    }
    return retVal;
}

/**
 * Reset all channel and keep in disabled state
 * until ETHER_Init is called by respective client
 */
void ETHER_DisableAllChannels(ETHER_HwIDType aID)
{
    if (ETHER_HW_ID_MAX > aID) {

        ETHER_CntlrXmtReset();
        ETHER_CntlrXmtEnable(FALSE);
        /* Disable transmit interrupt */
        GMAC_REGS->intr_mask &= ~(GMAC_INTR_MASK_TXF_MASK_MASK);
        /* Clear transmit interrupt status */
        GMAC_REGS->intr_clr |= GMAC_INTR_CLR_TXF_CLR_MASK;
        GMAC_REGS->intr_clr &= ~(GMAC_INTR_CLR_TXF_CLR_MASK);

        /* keep RX disabled */
        ETHER_CntlrRcvReset();
        ETHER_CntlrRcvEnable(FALSE);
        /* Disable receive interrupt */
        GMAC_REGS->intr_mask &= ~(GMAC_INTR_MASK_RXF_MASK_MASK);
        /* Clear receive interrupt status */
        GMAC_REGS->intr_clr |= GMAC_INTR_CLR_RXF_CLR_MASK;
        GMAC_REGS->intr_clr &= ~(GMAC_INTR_CLR_RXF_CLR_MASK);
    }
}

static int32_t ETHER_CntlrXmtInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_TX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_UNINIT != ETHER_TxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    ETHER_CntlrXmtChanInfoInit();
    ETHER_Stats.txChCompIRQCnt = 0UL;
    retVal = XmtChanInit();
    if (BCM_ERR_OK == retVal) {
        if (ETHER_CHANSTATE_UNINIT != ATOMIC_CheckUpdate(&(ETHER_TxChanInfo.state),
                                                    ETHER_CHANSTATE_UNINIT,
                                                    ETHER_CHANSTATE_INIT)) {
            retVal = BCM_ERR_INVAL_STATE;
            /* This state mis-match shall never happen. Add abort/crash */
        }
    }

err:
    return retVal;
}

static int32_t ETHER_CntlrRcvInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_RX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_UNINIT != ETHER_RxChanInfo.state) {
        goto err;
    }

    ETHER_CntlrRcvChanInfoInit();
    ETHER_Stats.rxChCompIRQCnt = 0UL;
    retVal = RcvChanInit();
    if (BCM_ERR_OK == retVal) {
        if (ETHER_CHANSTATE_UNINIT != ATOMIC_CheckUpdate(&(ETHER_RxChanInfo.state),
                                                    ETHER_CHANSTATE_UNINIT,
                                                    ETHER_CHANSTATE_INIT)) {
            /* Add abort/crash */
            retVal = BCM_ERR_INVAL_STATE;
        }
    }

err:
    return retVal;
}

static int32_t ETHER_CntlrIntInit(ETHER_HwIDType aID)
{
    int32_t retVal;

    retVal = ETHER_CntlrXmtInit(aID, 0UL);
    if (BCM_ERR_OK == retVal) {
        /* Initialise receiver on successful initialisation of transmit
         * channel */
        retVal = ETHER_CntlrRcvInit(aID, 0UL);
    }

    return retVal;
}

static int32_t ETHER_CntlrInit(ETHER_HwIDType aID, const ETHER_CfgType *const aConfig)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHER_HW_ID_MAX > aID)
            && (NULL != aConfig)) {
        if (ETHER_STATE_UNINIT != ETHER_CntlrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            retVal = ETHER_MacInit(aID, aConfig);
            if (BCM_ERR_OK == retVal) {
                ETHER_CntlrState = ETHER_STATE_INIT;
            }
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrXmtGetBuffer(ETHER_HwIDType aID,
                            ETHER_PrioType aPriority,
                            uint32_t *const aBufIdx,
                            uint8_t **const aBuf,
                            uint32_t *const aLenInOut)
{
    int32_t retVal;
    uint32_t bufLen = ETHER_FRMSIZENOCRC - (2UL * ETHER_MAC_ADDR_SIZE);

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_PRIO_MAX < aPriority)
        || (NULL == aBufIdx) || (NULL == aBuf) || (NULL == aLenInOut)
        || (0UL == *aLenInOut)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    retVal = ETHER_CntlrAllocTxBuf(ETHER_TxChanInfo.bufMgrInfo, aBufIdx, aBuf);
    if (BCM_ERR_OK == retVal) {
        *aBuf = &((*aBuf)[2UL * ETHER_MAC_ADDR_SIZE]);
        *aLenInOut = bufLen;
    } else {
        /* No free buffer */
        *aLenInOut = 0UL;
    }
err:
    return retVal;
}

static int32_t ETHER_CntlrXmtSend(ETHER_HwIDType aID,
                         uint32_t aTxConfirmation,
                         uint32_t aBufIdx,
                         uint32_t aLen,
                         const uint8_t *const aDestMacAddr)
{
    int32_t retVal;
    uint8_t *pktBuff;
    uint32_t lastDescIdx = 0UL;
    INTR_FlagType intFlags;
    uint32_t pktLen = aLen;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_FRMSIZENOCRC < aLen)
        || (ETHER_TxChanInfo.bufMgrInfo->pktBuffCnt <= aBufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((NULL == aDestMacAddr) && (0UL != aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_PKTBUFFSTATE_ALLOC !=
            ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* If aLen equal to zero, just free the packet */
    if (0UL == aLen) {
        retVal = ETHER_CntlrFreeTxBuf(ETHER_TxChanInfo.bufMgrInfo, aBufIdx);
        goto err;
    }

    retVal = ETHER_MacGetMacAddr(aID, ETHER_MacAddr);
    if (BCM_ERR_OK != retVal) {
        ETHER_CntlrReportError((uint8_t)aID,
            BRCM_SWDSGN_ETHER_CNTLRXMTSEND_PROC,
            retVal, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    /* Disable interrupt */
    intFlags = INTR_Suspend();

    retVal = ETHER_CntlrQueueTxBuf(ETHER_TxChanInfo.bufMgrInfo, aBufIdx,
                        (uint8_t **)&pktBuff, aTxConfirmation);
    if(BCM_ERR_OK == retVal) {
        pktLen += (2 * ETHER_MAC_ADDR_SIZE);
        BCM_MemCpy(pktBuff, aDestMacAddr, ETHER_MAC_ADDR_SIZE);
        BCM_MemCpy(&pktBuff[ETHER_MAC_ADDR_SIZE], ETHER_MacAddr, ETHER_MAC_ADDR_SIZE);
        lastDescIdx = TX_LAST_DESC_IDX;
        lastDescIdx = ModInc(lastDescIdx, TX_DMA_DESC_COUNT);
        ETHER_SetTxDMADescFields(ETHER_TxDmaDescTbl[lastDescIdx],
                pktLen,
                pktBuff,
                TX_DMA_DESC_REPLACE_CRC_DIS,
                TX_DMA_DESC_APPEND_CRC_EN,
                TX_DMA_DESC_EOP_EN,
                TX_DMA_DESC_SOP_EN);
    }

    /* Enable Interrupt */
    INTR_Resume(intFlags);

    if (BCM_ERR_OK == retVal) {
        /* Program sw pointer after enable interrupts */
        GMAC_REGS->tswptr = (uint32_t)(&ETHER_TxDmaDescTbl[lastDescIdx]);

        /* Enable TX channel */
        ETHER_CntlrXmtEnable(TRUE);
    }

err:
    return retVal;
}

static int32_t ETHER_CntlrRcvDeInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;
    INTR_FlagType intFlags;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_RX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* Disable receive interrupt */
    GMAC_REGS->intr_mask &= ~(GMAC_INTR_MASK_RXF_MASK_MASK);
    /* Clear receive interrupt status */
    GMAC_REGS->intr_clr |= GMAC_INTR_CLR_RXF_CLR_MASK;
    GMAC_REGS->intr_clr &= ~(GMAC_INTR_CLR_RXF_CLR_MASK);

    intFlags = INTR_Suspend();
    GMAC_REGS->eth_ctrl |= GMAC_CTRL_GRS_MASK;
    ETHER_CntlrIntRcvDeInit();
    ETHER_CntlrRcvEnable(FALSE);
    INTR_Resume(intFlags);

err:
    return retVal;
}

static int32_t ETHER_CntlrXmtDeInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;
    INTR_FlagType intFlags;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_TX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* Disable transmit interrupt */
    GMAC_REGS->intr_mask &= ~(GMAC_INTR_MASK_TXF_MASK_MASK);
    /* Clear transmit interrupt status */
    GMAC_REGS->intr_clr |= GMAC_INTR_CLR_TXF_CLR_MASK;
    GMAC_REGS->intr_clr &= ~(GMAC_INTR_CLR_TXF_CLR_MASK);

    intFlags = INTR_Suspend();
    GMAC_REGS->eth_ctrl |= GMAC_CTRL_GTS_MASK;
    ETHER_CntlrXmtEnable(FALSE);
    XmtChanDeInit(aChan);
    INTR_Resume(intFlags);
err:
    return retVal;
}

static int32_t ETHER_CntlrIntDeInit(ETHER_HwIDType aID)
{
    int32_t retVal;

    retVal = ETHER_CntlrXmtDeInit(aID, 0UL);
    if (BCM_ERR_OK == retVal) {
        retVal = ETHER_CntlrRcvDeInit(aID, 0UL);
    }

    return retVal;
}

static int32_t ETHER_CntlrDeInit(ETHER_HwIDType aID)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ETHER_HW_ID_MAX > aID) {
        if (ETHER_STATE_INIT != ETHER_CntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = ETHER_CntlrIntDeInit(aID);
            if (BCM_ERR_OK == retVal) {
                retVal = ETHER_MacDeInit(aID);
                if (BCM_ERR_OK == retVal) {
                    ETHER_CntlrState = ETHER_STATE_UNINIT;
                }
            }
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrSetMode(ETHER_HwIDType aID, ETHER_ModeType aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ETHER_HW_ID_MAX > aID) {
        if (ETHER_STATE_INIT != ETHER_CntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            if (ETHER_MODE_ACTIVE == aMode) {
                retVal = ETHER_CntlrIntInit(aID);
                if (BCM_ERR_OK == retVal) {
                    retVal = ETHER_MacEnableTxRx();
                }
            } else {
                if (ETHER_MODE_DOWN == aMode) {
                    retVal = ETHER_CntlrIntDeInit(aID);
                    if (BCM_ERR_OK == retVal) {
                        retVal = ETHER_MacDisableTxRx();
                    }
                }
            }
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrGetMode(ETHER_HwIDType aID,
                                ETHER_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t enabled;

    if ((ETHER_HW_ID_MAX > aID) && (NULL != aMode)) {
        if (ETHER_STATE_INIT != ETHER_CntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = ETHER_MacIsEnabled(aID, &enabled);
            if (BCM_ERR_OK == retVal) {
                if (TRUE == enabled) {
                    *aMode = ETHER_MODE_ACTIVE;
                } else {
                    *aMode = ETHER_MODE_DOWN;
                }
            }
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrGetStats(ETHER_HwIDType aID, ETHER_RxStatsType *const aStats)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aStats)){
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    aStats->gdPkts = GMAC_REGS->rxfrmgood;
    aStats->octetsLow = GMAC_REGS->rxoctgood;
    aStats->octetsHigh = 0UL;
    aStats->allPkts = GMAC_REGS->rxfrmtotal;
    aStats->brdCast = GMAC_REGS->rxbcastgood;
    aStats->mutCast = GMAC_REGS->rxmcastgood;
    aStats->pkts64 = GMAC_REGS->rx64;
    aStats->pkts65_127 = GMAC_REGS->rx65_127;
    aStats->pkts128_255 = GMAC_REGS->rx128_255;
    aStats->pkts256_511 = GMAC_REGS->rx256_511;
    aStats->pkts512_1023 = GMAC_REGS->rx512_1023;
    aStats->pkts1024_1522 = GMAC_REGS->rx1024_max;
    aStats->pkts1523_2047 = 0UL;
    aStats->pkts2048_4095 = 0UL;
    aStats->pkts4096_8191 = 0UL;
    aStats->pkts8192_MAX = 0UL;
    aStats->pktsJabber = GMAC_REGS->rxjabber;
    aStats->pktsOvrSz = GMAC_REGS->rxoverrun;
    aStats->pktsFrag = GMAC_REGS->rxfrag;
    aStats->pktsRxDrop = 0UL;
    aStats->pktsCrcAlignErr = GMAC_REGS->rxcrcalign;
    aStats->pktsUndSz = GMAC_REGS->rxusize;
    aStats->pktsCrcErr = GMAC_REGS->rxcrc;

err:
    return retVal;
}

static int32_t ETHER_CntlrXmtMarkTSPkt(ETHER_HwIDType aID,
                              uint32_t aBufIdx)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID)
            || (ETHER_TxChanInfo.bufMgrInfo->pktBuffCnt <= aBufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHER_STATE_INIT != ETHER_CntlrState) {
        retVal = BCM_ERR_UNINIT;
    } else if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo.state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else if (ETHER_PKTBUFFSTATE_ALLOC !=
          ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx].flags |=
                                                    ETHER_PKTFLAGS_1588TXCONF;
    }

    return retVal;
}

static int32_t ETHER_CntlrXmtValidateBuffer(ETHER_HwIDType aID,
                                     uint32_t aBufIdx)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) ||
        (ETHER_TxChanInfo.bufMgrInfo->pktBuffCnt <= aBufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHER_STATE_INIT != ETHER_CntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo.state) {
                retVal = BCM_ERR_INVAL_STATE;
            }
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrIsXmtPktTSMarked(ETHER_HwIDType aID,
                                  uint32_t aBufIdx)
{
    int32_t retVal = ETHER_CntlrXmtValidateBuffer(aID, aBufIdx);

    if (BCM_ERR_OK == retVal) {
        if (ETHER_PKTFLAGS_1588TXCONF !=
                (ETHER_TxChanInfo.bufMgrInfo->pktBuffInfo[aBufIdx].flags
                 & ETHER_PKTFLAGS_1588TXCONF)) {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

    return retVal;
}

static int32_t ETHER_CntlrSetGPTimer(ETHER_HwIDType aID,
                            uint32_t       aPeriod)
{
    int32_t  retVal = BCM_ERR_NOSUPPORT;

    if (retVal != BCM_ERR_OK) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                        BRCM_SWDSGN_ETHER_CNTLRSETGPTIMER_PROC,
                        retVal, aID, aPeriod, 0UL, __LINE__);
    }
    return retVal;
}

void ETHER_IntIRQHandler(ETHER_HwIDType aID, ETHER_IRQEventType* const aType)
{
    uint32_t intStatus;

    intStatus = GMAC_REGS->intr;

    /* This return is required to see if the spurious
       interrupts are generted. It is required to capture
       spurious interrupt recieved otherwise it might be
       stalling CPU & will be difficult to debug */
    if (intStatus == 0UL) {
        ETHER_CntlrReportError((uint8_t)aID,
                BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC,
                BCM_ERR_CUSTOM, 0UL, 0UL, 0UL, 0UL);
        goto err;
    }

    /* clear the interrupts */
    GMAC_REGS->intr_clr = intStatus;
    GMAC_REGS->intr_clr = 0;

    if (ETHER_STATE_INIT == ETHER_CntlrState) {
        ETHER_Stats.lastIRQStatus = intStatus;

        /* Transmit and receive completion handlers */
        ETHER_CntlrRcvCompIRQHandler(intStatus);
        ETHER_CntlrXmtCompIRQHandler(intStatus);

        /* Error handlers */
        ETHER_CntlrRcvErrIRQHandler(intStatus);
        ETHER_CntlrXmtErrIRQHandler(intStatus);
        BusErrIRQHandler(intStatus);

        *aType |= ETHER_IRQEVENT_PKT;
    }
err:
    return ;
}

static int32_t ETHER_CmdHandler(ETHER_CntlrIOCmdType aCmd, ETHER_IOType *const aIO)
{
    int32_t retVal;
    const MDIO_CfgType *mdioCfg = NULL;
    uint32_t count = 0UL;
    uint32_t i;

    if (NULL != aIO) {
        switch (aCmd) {
        case ETHER_CNTLRIOCMD_INIT:
            retVal = ETHER_GetMdioConfig(&mdioCfg, &count);
            if ((BCM_ERR_OK == retVal) && (NULL != mdioCfg) && (0UL != count)) {
                for (i = 0; i < count; i++) {
                    MDIO_Init(i, &mdioCfg[i]);
                }
            }
            retVal = ETHER_CntlrInit(aIO->hwID, aIO->ctrlCfg);
            break;
        case ETHER_CNTLRIOCMD_DEINIT:
            retVal = ETHER_GetMdioConfig(&mdioCfg, &count);
            if ((BCM_ERR_OK == retVal) && (NULL != mdioCfg) && (0UL != count)) {
                for (i = 0; i < count; i++) {
                    MDIO_DeInit(i);
                }
            }
            retVal = ETHER_CntlrDeInit(aIO->hwID);
            break;
        case ETHER_CNTLRIOCMD_SET_MODE:
            retVal = ETHER_CntlrSetMode(aIO->hwID, aIO->mode);
            break;
        case ETHER_CNTLRIOCMD_GET_MODE:
            retVal = ETHER_CntlrGetMode(aIO->hwID, &(aIO->mode));
            break;
        case ETHER_CNTLRIOCMD_SET_MACADDR:
            retVal = ETHER_MacSetMacAddr(aIO->hwID, aIO->macAddrIn);
            break;
        case ETHER_CNTLRIOCMD_GET_MACADDR:
            retVal = ETHER_MacGetMacAddr(aIO->hwID, aIO->macAddrOut);
            break;
        case ETHER_CNTLRIOCMD_GET_BUF:
            retVal = ETHER_CntlrXmtGetBuffer(aIO->hwID,
                                          aIO->priority,
                                          aIO->buffIdxInOut,
                                          &(aIO->buf),
                                          aIO->lenInOut);
            break;
        case ETHER_CNTLRIOCMD_SEND:
            retVal = ETHER_CntlrXmtSend(aIO->hwID,
                                   aIO->txConfirmation,
                                   *(aIO->buffIdxInOut),
                                   *(aIO->lenInOut),
                                   aIO->macAddrIn);
            break;
        case ETHER_CNTLRIOCMD_GET_STATS:
            retVal = ETHER_CntlrGetStats(aIO->hwID, aIO->stats);
            break;
        case ETHER_CNTLRIOCMD_ENABLE_LOOP_BACK:
            ETHER_MacEnableLoopback(aIO->hwID, TRUE);
            retVal = BCM_ERR_OK;
            break;
        case ETHER_CNTLRIOCMD_TX_MARK_TS_PKT:
            retVal = ETHER_CntlrXmtMarkTSPkt(aIO->hwID,
                                   *(aIO->buffIdxInOut));
            break;
        case ETHER_CNTLRIOCMD_IS_TX_PKT_TS_MARKED:
            retVal = ETHER_CntlrIsXmtPktTSMarked(aIO->hwID,
                                   *(aIO->buffIdxInOut));
            break;
        case ETHER_CNTLRIOCMD_TX_TS_DONE_IND:
            retVal = ETHER_CntlrIntTxTSDoneInd(aIO->hwID,
                                   *(aIO->buffIdxInOut));
            break;
        case ETHER_CNTLRIOCMD_GET_RX_PKT:
            retVal = ETHER_CntlrRcvGetPkt(aIO->hwID,
                                       aIO->buffIdxInOut,
                                       &(aIO->buf),
                                       aIO->raiseCb,
                                       aIO->lenInOut);
            break;
        case ETHER_CNTLRIOCMD_PUT_RX_PKT:
            ETHER_CntlrRcvPutPkt(aIO->hwID);
            retVal = BCM_ERR_OK;
            break;
        case ETHER_CNTLRIOCMD_GET_TX_PKT:
            retVal = ETHER_CntlrXmtGetPkt(aIO->hwID,
                                        aIO->buffIdxInOut,
                                        aIO->raiseCb);
            break;
        case ETHER_CNTLRIOCMD_PUT_TX_PKT:
            retVal = ETHER_CntlrXmtPutPkt(aIO->hwID);
            break;
        case ETHER_CNTLRIOCMD_SET_GPTIMER:
            retVal = ETHER_CntlrSetGPTimer(aIO->hwID,
                                         aIO->timerPeriod);
            break;
        case ETHER_CNTLRIOCMD_MDIO_READ:
            retVal = MDIO_Read(aIO->mdioHwID, aIO->mdioPkt, &aIO->mdioJobId);
            break;
        case ETHER_CNTLRIOCMD_MDIO_WRITE:
            retVal = MDIO_Write(aIO->mdioHwID, aIO->mdioPkt, &aIO->mdioJobId);
            break;
        case ETHER_CNTLRIOCMD_MDIO_CHECKSTATUS:
            retVal = MDIO_GetJobResult(aIO->mdioHwID, aIO->mdioJobId, aIO->mdioPkt);
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            ETHER_CntlrReportError((uint8_t)aIO->hwID,
                            BRCM_SWDSGN_ETHER_CMDHANDLER_PROC,
                            retVal, aCmd, 0UL, 0UL, 0UL);
            break;
        }
    } else {
        retVal = BCM_ERR_UNKNOWN;
    }

    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _ETHER_SVCIOType {
    uint8_t *data;
    ETHER_IOType *io;
} ETHER_SVCIOType;

/**
    @code{.c}
    if aSysIO.ethIO is not NULL
        if aMagicID is SVC_MAGIC_AMC_ID
            aSysIO.ethIO.retVal = ETHER_CmdHandler(aCmd, aSysIO.ethIO)
        else
            aSysIO.ethIO.retVal = BCM_ERR_INVAL_MAGIC
    @endcode
*/
//! [Usage of ETHER_CmdHandler]
void ETHER_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    ETHER_SVCIOType ether;
    ether.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_AMC_ID == aMagicID) {
            ether.io->retVal = ETHER_CmdHandler(aCmd, ether.io);
        } else {
            ether.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}
//! [Usage of ETHER_CmdHandler]


/* Nothing past this line */
