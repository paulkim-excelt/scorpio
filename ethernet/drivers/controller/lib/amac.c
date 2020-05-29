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
/**
    @defgroup grp_eth_cntlrdrv_amac_impl Ethernet Controller Driver Design
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_amac_impl
    @{

    @file ethernet/drivers/controller/lib/amac.c
    @brief Ethernet Controller Driver Design
    @version 1.26 Imported from docx
*/

#include <string.h>
#include <atomic.h>
#include <cache.h>
#include <compiler.h>
#include <interrupt.h>
#include <utils.h>
#include <eth_osil.h>
#include <system.h>
#include "eth_cntlr.h"
#include "eth_cntlr_buf.h"
#include <stddef.h>
#include <inttypes.h>
#include "amac_rdb.h"
#include <cortex.h>


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
#define BRCM_SWDSGN_ETHER_STATS_TYPE                        ()  /**< @brief #ETHER_IRQStatsType */
#define BRCM_SWDSGN_ETHER_TXCHANINFO_TYPE                   ()  /**< @brief #ETHER_TxChanInfoType*/
#define BRCM_SWDSGN_ETHER_RXCHANINFO_TYPE                   ()  /**< @brief #ETHER_RxChanInfoType */
#define BRCM_SWDSGN_ETHER_INITTXCHANNELINFO_MACRO           ()  /**< @brief #ETHER_INITTXCHANNELINFO */
#define BRCM_SWDSGN_ETHER_INITRXCHANNELINFO_MACRO           ()  /**< @brief #ETHER_INITRXCHANNELINFO */
#define BRCM_SWDSGN_ETHER_AMACRESETDELAYCNT_MACRO           ()  /**< @brief #ETHER_AMACRESETDELAYCNT */
#define BRCM_SWDSGN_ETHER_TXPARITY_MACRO                    ()  /**< @brief #ETHER_TXPARITY_EN */
#define BRCM_SWDSGN_ETHER_DMADESCERRMASK_MACRO              ()  /**< @brief #ETHER_DMADESCERRMASK */
#define BRCM_SWDSGN_ETHER_RXDESCOFFSET_MACRO                ()  /**< @brief #ETHER_RXDESCOFFSET */
#define BRCM_SWDSGN_ETHER_TXDESCOFFSET_MACRO                ()  /**< @brief #ETHER_TXDESCOFFSET */
#define BRCM_SWDSGN_ETHER_DESCTABLEDESCCNT_MACRO            ()  /**< @brief #ETHER_DESCTABLEDESCCNT */
#define BRCM_SWDSGN_ETHER_DMA_DESC_TBL_ALIGN_MACRO          ()  /**< @brief #ETHER_DMA_DESC_TBL_ALIGN */
#define BRCM_SWDSGN_ETHER_BUF_ALIGN_MACRO                   ()  /**< @brief #ETHER_BUF_ALIGN */
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
#define BRCM_SWDSGN_ETHER_RXCHANINFO_GLOBAL                 ()  /**< @brief #ETHER_RxChanInfo */
#define BRCM_SWDSGN_ETHER_TXCHANINFO_GLOBAL                 ()  /**< @brief #ETHER_TxChanInfo */
#define BRCM_SWDSGN_ETHER_CNTLRRWDEV_TYPE                   ()  /**< @brief #ETHER_CntlrRWDevType */
#define BRCM_SWDSGN_ETHER_CNTLRRWDEVDATA_GLOBAL             ()  /**< @brief #ETHER_CntlrRWDevData */
#define BRCM_SWDSGN_ETHER_MACDEV_TYPE                       ()  /**< @brief #ETHER_MacDevType */
#define BRCM_SWDSGN_ETHER_MACDEV_GLOBAL                     ()  /**< @brief #ETHER_MacDev */
#define BRCM_SWDSGN_ETHER_XMTPTRLASTDSCRINITMASK_GLOBAL     ()  /**< @brief #ETHER_XmtPtrLastDscrOffset */
#define BRCM_SWDSGN_ETHER_RCVPTRRCVPTRINITMASK_GLOBAL       ()  /**< @brief #ETHER_RcvPtrOffset */
#define BRCM_SWDSGN_ETHER_CNTLRREPORTERROR_PROC             (0xBFU)  /**< @brief #ETHER_CntlrReportError */
#define BRCM_SWDSGN_ETHER_MACCHECKCONFIGPARAMS_PROC         (0xC0U)  /**< @brief #ETHER_MacCheckConfigParams */
#define BRCM_SWDSGN_ETHER_MACENABLETXRESET_PROC             (0xC1U)  /**< @brief #ETHER_MacEnableTxReset */
#define BRCM_SWDSGN_ETHER_MACENABLERXRESET_PROC             (0xC2U)  /**< @brief #ETHER_MacEnableRxReset */
#define BRCM_SWDSGN_ETHER_MACDISABLETXRX_PROC               (0xC3U)  /**< @brief #ETHER_MacDisableTxRx */
#define BRCM_SWDSGN_ETHER_MACENABLETXRX_PROC                (0xC4U)  /**< @brief #ETHER_MacEnableTxRx */
#define BRCM_SWDSGN_ETHER_MACENABLELOOPBACK_PROC            (0xC5U)  /**< @brief #ETHER_MacEnableLoopback */
#define BRCM_SWDSGN_ETHER_MACGETMACADDR_PROC                (0xC6U)  /**< @brief #ETHER_MacGetMacAddr */
#define BRCM_SWDSGN_ETHER_MACSETMACADDRINTERNAL_PROC        (0xC7U)  /**< @brief #ETHER_MacSetMacAddrInternal */
#define BRCM_SWDSGN_ETHER_MACSETMACADDR_PROC                (0xC8U)  /**< @brief #ETHER_MacSetMacAddr */
#define BRCM_SWDSGN_ETHER_MACINIT_PROC                      (0xC9U)  /**< @brief #ETHER_MacInit */
#define BRCM_SWDSGN_ETHER_MACDEINIT_PROC                    (0xCAU)  /**< @brief #ETHER_MacDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTSETPARITYDISABLE_PROC     (0xCBU)  /**< @brief #ETHER_CntlrXmtSetParityDisable */
#define BRCM_SWDSGN_ETHER_CNTLRXMTENABLE_PROC               (0xCCU)  /**< @brief #ETHER_CntlrXmtEnable */
#define BRCM_SWDSGN_ETHER_CNTLRXMTRESET_PROC                (0xCDU)  /**< @brief #ETHER_CntlrXmtReset */
#define BRCM_SWDSGN_ETHER_XMTSETDESCTABLEADDR_PROC          (0xCEU)  /**< @brief #ETHER_XmtSetDescTableAddr */
#define BRCM_SWDSGN_ETHER_CNTLRRCVENABLE_PROC               (0xCFU)  /**< @brief #ETHER_CntlrRcvEnable */
#define BRCM_SWDSGN_ETHER_CNTLRRCVRESET_PROC                (0xD0U)  /**< @brief #ETHER_CntlrRcvReset */
#define BRCM_SWDSGN_ETHER_RCVSETDESCTABLEADDR_PROC          (0xD1U)  /**< @brief #ETHER_RcvSetDescTableAddr */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTINIT_PROC              (0xD2U)  /**< @brief #ETHER_CntlrIntXmtInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTDEINIT_PROC            (0xD3U)  /**< @brief #ETHER_CntlrIntXmtDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCHANINFOINIT_PROC         (0xD4U)  /**< @brief #ETHER_CntlrXmtChanInfoInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTRCVINIT_PROC              (0xD5U)  /**< @brief #ETHER_CntlrIntRcvInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTRCVDEINIT_PROC            (0xD6U)  /**< @brief #ETHER_CntlrIntRcvDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCHANINFOINIT_PROC         (0xD7U)  /**< @brief #ETHER_CntlrRcvChanInfoInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCHANERRHANDLER_PROC       (0xD8U)  /**< @brief #ETHER_CntlrRcvChanErrHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCHANERRHANDLER_PROC       (0xD9U)  /**< @brief #ETHER_CntlrXmtChanErrHandler */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUERCVCOMPPKTS_PROC      (0xDAU)  /**< @brief #ETHER_CntlrDequeueRcvCompPkts */
#define BRCM_SWDSGN_ETHER_CNTLRDEQUEUEXMTCOMPPKTS_PROC      (0xDBU)  /**< @brief #ETHER_CntlrDequeueXmtCompPkts */
#define BRCM_SWDSGN_ETHER_CNTLRRCVDMAERRIRQHANDLER_PROC     (0xDCU)  /**< @brief #ETHER_CntlrRcvDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTDMAERRIRQHANDLER_PROC     (0xDDU)  /**< @brief #ETHER_CntlrXmtDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRDMAERRIRQHANDLER_PROC        (0xDEU)  /**< @brief #ETHER_CntlrDMAErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRRCVERRIRQHANDLER_PROC        (0xDFU)  /**< @brief #ETHER_CntlrRcvErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTERRIRQHANDLER_PROC        (0xE0U)  /**< @brief #ETHER_CntlrXmtErrIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRRCVCOMPIRQHANDLER_PROC       (0xE1U)  /**< @brief #ETHER_CntlrRcvCompIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTCOMPIRQHANDLER_PROC       (0xE2U)  /**< @brief #ETHER_CntlrXmtCompIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRGPTIMERIRQHANDLER_PROC       (0xE3U)  /**< @brief #ETHER_CntlrGPTimerIRQHandler */
#define BRCM_SWDSGN_ETHER_CNTLRXMTPUTPKT_PROC               (0xE4U)  /**< @brief #ETHER_CntlrXmtPutPkt */
#define BRCM_SWDSGN_ETHER_CNTLRINTXMTGETPKT_PROC            (0xE5U)  /**< @brief #ETHER_CntlrIntXmtGetPkt */
#define BRCM_SWDSGN_ETHER_CNTLRXMTGETPKT_PROC               (0xE6U)  /**< @brief #ETHER_CntlrXmtGetPkt */
#define BRCM_SWDSGN_ETHER_CNTLRINTTXTSDONEIND_PROC          (0xE7U)  /**< @brief #ETHER_CntlrIntTxTSDoneInd */
#define BRCM_SWDSGN_ETHER_CNTLRRCVPUTPKT_PROC               (0xE8U)  /**< @brief #ETHER_CntlrRcvPutPkt */
#define BRCM_SWDSGN_ETHER_CNTLRRCVGETPKT_PROC               (0xE9U)  /**< @brief #ETHER_CntlrRcvGetPkt */
#define BRCM_SWDSGN_ETHER_DISABLEALLCHANNELS_PROC           (0xEAU)  /**< @brief #ETHER_DisableAllChannels */
#define BRCM_SWDSGN_ETHER_CNTLRXMTINIT_PROC                 (0xEBU)  /**< @brief #ETHER_CntlrXmtInit */
#define BRCM_SWDSGN_ETHER_CNTLRRCVINIT_PROC                 (0xECU)  /**< @brief #ETHER_CntlrRcvInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTINIT_PROC                 (0xEDU)  /**< @brief #ETHER_CntlrIntInit */
#define BRCM_SWDSGN_ETHER_CNTLRINIT_PROC                    (0xEEU)  /**< @brief #ETHER_CntlrInit */
#define BRCM_SWDSGN_ETHER_CNTLRGETCHANIDX_PROC              (0xEFU)  /**< @brief #ETHER_CntlrGetChanIdx */
#define BRCM_SWDSGN_ETHER_CNTLRXMTGETBUFFER_PROC            (0xF0U)  /**< @brief #ETHER_CntlrXmtGetBuffer */
#define BRCM_SWDSGN_ETHER_CNTLRXMTSEND_PROC                 (0xF1U)  /**< @brief #ETHER_CntlrXmtSend */
#define BRCM_SWDSGN_ETHER_CNTLRRCVDEINIT_PROC               (0xF2U)  /**< @brief #ETHER_CntlrRcvDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRXMTDEINIT_PROC               (0xF3U)  /**< @brief #ETHER_CntlrXmtDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRINTDEINIT_PROC               (0xF4U)  /**< @brief #ETHER_CntlrIntDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRDEINIT_PROC                  (0xF5U)  /**< @brief #ETHER_CntlrDeInit */
#define BRCM_SWDSGN_ETHER_CNTLRSETMODE_PROC                 (0xF6U)  /**< @brief #ETHER_CntlrSetMode */
#define BRCM_SWDSGN_ETHER_CNTLRGETMODE_PROC                 (0xF7U)  /**< @brief #ETHER_CntlrGetMode */
#define BRCM_SWDSGN_ETHER_CNTLRGETSTATS_PROC                (0xF8U)  /**< @brief #ETHER_CntlrGetStats */
#define BRCM_SWDSGN_ETHER_CNTLRXMTMARKTSPKT_PROC            (0xF9U)  /**< @brief #ETHER_CntlrXmtMarkTSPkt */
#define BRCM_SWDSGN_ETHER_CNTLRXMTVALIDATEBUFFER_PROC       (0xFAU)  /**< @brief #ETHER_CntlrXmtValidateBuffer */
#define BRCM_SWDSGN_ETHER_CNTLRISXMTPKTTSMARKED_PROC        (0xFBU)  /**< @brief #ETHER_CntlrIsXmtPktTSMarked */
#define BRCM_SWDSGN_ETHER_CNTLRSETGPTIMER_PROC              (0xFCU)  /**< @brief #ETHER_CntlrSetGPTimer */
#define BRCM_SWDSGN_ETHER_INTIRQHANDLER_PROC                (0xFDU)  /**< @brief #ETHER_IntIRQHandler */
#define BRCM_SWDSGN_ETHER_CMDHANDLER_PROC                   (0xFEU)  /**< @brief #ETHER_CmdHandler */
#define BRCM_SWDSGN_ETHER_SVCIO_TYPE                        ()  /**< @brief #ETHER_SVCIOType */
#define BRCM_SWDSGN_ETHER_SYSCMDHANDLER_PROC                (0xFFU)  /**< @brief #ETHER_SysCmdHandler */
/** @} */

/* FIX:
 * Workaround for hardware JIRA: HWBCM89100-288
 * Align TX to 32-byte boundary and RX buffer to 28-byte(actually AMAC
 * DMA uses first 4-byte in the RX buffer to packet status, so actual
 * data write starts at buff+4
 */

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef uint32_t ETHER_DmaChanStateType;

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
typedef uint32_t ETHER_DmaChanErrType;

/**
 * @brief AMAC DMA descriptor
 *
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
volatile typedef struct _ETHER_DmaDescType {
    uint32_t rsvd1          :20;    /**< @brief reserved */
    uint32_t flags          :8;     /**< @brief used by core */
    uint32_t eot            :1;     /**< @brief End of table */
    uint32_t intOnComp      :1;     /**< @brief interrupt on completion */
    uint32_t eof            :1;     /**< @brief end of frame */
    uint32_t sof            :1;     /**< @brief start of frame */
    uint32_t bufCount       :15;    /**< @brief buffer count */
    uint32_t rsvd2          :1;     /**< @brief reserved */
    uint32_t addrExt        :2;     /**< @brief  address external (PCIE) */
    uint32_t rsvd3          :1;     /**< @brief reserved */
    uint32_t rsvd4          :13;    /**< @brief reserved */
    uint32_t dataPtrLow;            /**< @brief data buffer pointer low address */
    uint32_t dataPtrHigh;           /**< @brief data buffer pointer high address */
} ETHER_DmaDescType;

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_DMADESCSIZE        sizeof(ETHER_DmaDescType)

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_SETDESCFIELDS(desc, _flags, _eot, _ic, _eof, _sof, _bc, _dl, _dh) \
    do {                                                                    \
        (desc).flags = (_flags);                                            \
        (desc).eot = (_eot);                                                \
        (desc).intOnComp = (_ic);                                           \
        (desc).sof = (_sof);                                                \
        (desc).eof = (_eof);                                                \
        (desc).bufCount = (_bc);                                            \
        (desc).dataPtrLow = (_dl);                                          \
        (desc).dataPtrHigh = (_dh);                                         \
        CORTEX_DMB();                                                       \
        CORTEX_ISB();                                                       \
    } while (0UL)

/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_SETDESCFIELD(desc, field, value)                              \
    do {                                                                    \
        (desc).field = (value);                                             \
        CORTEX_DMB();                                                       \
        CORTEX_ISB();                                                       \
    } while (0UL)

/**
 * @brief AMAC RX packet information
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
typedef struct _ETHER_AMACRxPktInfoType {
    uint16_t frameLen;
    uint16_t pktType    :2;
    uint16_t vlanTagDet :1;
    uint16_t crcErr     :1;
    uint16_t overSz     :1;
    uint16_t ctfHit     :1;
    uint16_t ctfErr     :1;
    uint16_t pktOvflw   :1;
    uint16_t descCntr   :4;
    uint16_t dataType   :4;
} ETHER_AMACRxPktInfoType;

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_RXPKTTYPESIZE    (sizeof(ETHER_RxPktType))

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_INITRXDESC(aEot, aRxPktPtr)                           \
{                                                                   \
    .flags = 0UL,                                                   \
    .eot = (aEot),                                                  \
    .intOnComp = AMAC_DMA_DESC_IOC_DIS,                             \
    .eof = AMAC_DMA_DESC_EOF_DIS,                                   \
    .sof = AMAC_DMA_DESC_SOF_DIS,                                   \
    .bufCount = ETHER_RXPKTTYPESIZE,                                \
    .addrExt = 0UL,                                                 \
    .dataPtrLow = (uint32_t)(aRxPktPtr) + AMAC_DMA_MEM_VIEW_BASE,   \
    .dataPtrHigh = AMAC_DMA_DESC_DATAPTRHIGH_ZERO                   \
}

/**
 * @brief Structure for holding statistics for Ethernet controller
 *
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef struct _ETHER_IRQStatsType {
    /**< Transfer complete interrupt count for each TX channel */
    uint32_t    txChCompIRQCnt[ETHER_TX_CHAN_CNT];
    /**< Receive complete interrupt count for RX channel */
    uint32_t    rxChCompIRQCnt[ETHER_RX_CHAN_CNT];
    uint32_t    lastIRQStatus; /**< Last IRQ status */
} ETHER_IRQStatsType;

/**
 * @brief Structure for Tx Channel info
 *
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef struct _ETHER_TxChanInfoType {
    volatile ETHER_StateType        state;       /**< State of the channel */
    ETHER_DmaDescType * const       dmaDescTbl;  /**< DMA descriptor table */
    ETHER_TxBufMgrInfoType *const   bufMgrInfo;  /**< Buffer management info */
    uint32_t                        isErr;       /**< Flag to indicate error */
} ETHER_TxChanInfoType;

/**
 * @brief Structure for RX channel info
 *
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef struct _ETHER_RxChanInfoType {
    volatile ETHER_StateType        state;        /**< State of the channel */
    const ETHER_DmaDescType *const  dmaDescTbl;   /**< DMA descriptor table */
    ETHER_RxBufMgrInfoType *const   bufMgrInfo;   /**< Buffer management info */
    uint32_t                        isErr;        /**< Flag to indicate error */
} ETHER_RxChanInfoType;

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_INITTXCHANNELINFO(aTxChanState, aBufMgrInfo,           \
                            aDmaDescTbl, aIsErr)                        \
{                                                                       \
    .state = (aTxChanState),                                            \
    .bufMgrInfo = (aBufMgrInfo),                                        \
    .dmaDescTbl = (aDmaDescTbl),                                        \
    .isErr = (aIsErr),                                                  \
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_INITRXCHANNELINFO(aRxState, aBufMgrInfo, aDmaDescTbl, aIsErr)\
{                                                                       \
    .state = (aRxState),                                                \
    .bufMgrInfo = (aBufMgrInfo),                                        \
    .dmaDescTbl = (aDmaDescTbl),                                        \
    .isErr = (aIsErr),                                                  \
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONFIGURATION
 */
#define ETHER_AMACRESETDELAYCNT    (100UL)

/**
 * @{
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_TXPARITY_EN            (0UL) /**< TX parity enable */
#define ETHER_TXPARITY_DIS           (1UL) /**< TX parity disable */
/** @} */

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
#define ETHER_DMADESCERRMASK       (AMAC_GMAC0_INTSTATUS_DESCRERR_MASK|   \
                                AMAC_GMAC0_INTSTATUS_DATAERR_MASK|     \
                                AMAC_GMAC0_INTSTATUS_DESCPROTOERR_MASK)

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_RXDESCOFFSET(descIdx) ((sizeof(ETHER_DmaDescType) * (descIdx)) & 0x1FFFUL)
/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
#define ETHER_TXDESCOFFSET(descIdx) ((sizeof(ETHER_DmaDescType) * (descIdx)) & 0x1FFFUL)

/* Descriptor table for all the channels are allocated for 16 entries making
   total descriptor table size to 256Bytes */
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_DESCTABLEDESCCNT     (16UL)

/* Descriptors count for a Tx channel is 1 extra than the buffer count.
  This is required as the DMA descriptor queue expects last
  DMA descriptor + 1 = current DMA descriptor when queue is full &
  last descriptor shall not equal to current descriptor otherwise
  DMA will become IDLE. So, to satisfy above both condition and in
  case of queue all the buffer immediately. It is required to allocate
  1 extra DMA descriptor slot. */
#if ETHER_TX_CHAN_CNT >= 1
#if (ETHER_TXCH0BUFFCOUNT >= ETHER_DESCTABLEDESCCNT)
#error "Not enough space in DMA descriptor table for allocated buffers: ETHER_TXCH0BUFFCOUNT >= ETHER_DESCTABLEDESCCNT"
#endif
#endif
#if ETHER_TX_CHAN_CNT >= 2
#if (ETHER_TXCH1BUFFCOUNT >= ETHER_DESCTABLEDESCCNT)
#error "Not enough space in DMA descriptor table for allocated buffers: ETHER_TXCH1BUFFCOUNT >= ETHER_DESCTABLEDESCCNT"
#endif
#endif
#if ETHER_TX_CHAN_CNT >= 3
#if (ETHER_TXCH2BUFFCOUNT >= ETHER_DESCTABLEDESCCNT)
#error "Not enough space in DMA descriptor table for allocated buffers: ETHER_TXCH2BUFFCOUNT >= ETHER_DESCTABLEDESCCNT"
#endif
#endif
#if ETHER_TX_CHAN_CNT >= 4
#if (ETHER_TXCH3BUFFCOUNT >= ETHER_DESCTABLEDESCCNT)
#error "Not enough space in DMA descriptor table for allocated buffers: ETHER_TXCH3BUFFCOUNT >= ETHER_DESCTABLEDESCCNT"
#endif
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_DESCTABLESIZE         (ETHER_DESCTABLEDESCCNT * ETHER_DMADESCSIZE)

/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
#define ETHER_TXCHANDESCCNT(txChanIdx) (ETHER_TxChanInfo[txChanIdx].bufMgrInfo->pktBuffCnt + 1UL)

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_DMA_DESC_TBL_ALIGN  (16UL)

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#define ETHER_BUF_ALIGN     (32UL)

/* Start address of DMA descriptor table shall be 8KByte aligned.
   NOTE: With 4K alignment DMA Descriptor protocol error is observed */
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 0
ETHER_DmaDescType ETHER_TxCh0DmaDescTbl[ETHER_DESCTABLEDESCCNT] COMP_SECTION(".ethernet.tx0desctbl") COMP_ALIGN(ETHER_DMA_DESC_TBL_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 1
ETHER_DmaDescType ETHER_TxCh1DmaDescTbl[ETHER_DESCTABLEDESCCNT] COMP_SECTION(".ethernet.tx1desctbl") COMP_ALIGN(ETHER_DMA_DESC_TBL_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 2
ETHER_DmaDescType ETHER_TxCh2DmaDescTbl[ETHER_DESCTABLEDESCCNT] COMP_SECTION(".ethernet.tx2desctbl") COMP_ALIGN(ETHER_DMA_DESC_TBL_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 3
ETHER_DmaDescType ETHER_TxCh3DmaDescTbl[ETHER_DESCTABLEDESCCNT] COMP_SECTION(".ethernet.tx3desctbl") COMP_ALIGN(ETHER_DMA_DESC_TBL_ALIGN);
#endif

/* Buffer are to be aligned to 32byte boundary and should have 32byte
   aligned size */
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#if ETHER_TX_CHAN_CNT > 0
ETHER_TxPktType ETHER_TxCh0PktBuf[ETHER_TXCH0BUFFCOUNT] COMP_SECTION(".ethernet.tx0pktbuff") COMP_ALIGN(ETHER_BUF_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 1
ETHER_TxPktType ETHER_TxCh1PktBuf[ETHER_TXCH1BUFFCOUNT] COMP_SECTION(".ethernet.tx1pktbuff") COMP_ALIGN(ETHER_BUF_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 2
ETHER_TxPktType ETHER_TxCh2PktBuf[ETHER_TXCH2BUFFCOUNT] COMP_SECTION(".ethernet.tx2pktbuff") COMP_ALIGN(ETHER_BUF_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT > 3
ETHER_TxPktType ETHER_TxCh3PktBuf[ETHER_TXCH3BUFFCOUNT] COMP_SECTION(".ethernet.tx3pktbuff") COMP_ALIGN(ETHER_BUF_ALIGN);
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
ETHER_RxPktType ETHER_RxPkt[ETHER_RXBUFFCOUNT] COMP_SECTION(".ethernet.rx0pktbuff") COMP_ALIGN(ETHER_BUF_ALIGN);

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_TxPktType *ETHER_TxChPktBuf[ETHER_TX_CHAN_CNT] = {
#if ETHER_TX_CHAN_CNT > 0
    ETHER_TxCh0PktBuf,
#endif
#if ETHER_TX_CHAN_CNT > 1
    ETHER_TxCh1PktBuf,
#endif
#if ETHER_TX_CHAN_CNT > 2
    ETHER_TxCh2PktBuf,
#endif
#if ETHER_TX_CHAN_CNT > 3
    ETHER_TxCh3PktBuf
#endif
#if ETHER_TX_CHAN_CNT > 4
#error "ETHER_TX_CHAN_CNT > 4"
#endif
};

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
ETHER_DmaDescType ETHER_RxDmaDescTbl[ETHER_RXBUFFCOUNT] COMP_SECTION(".ethernet.rx0desctbl") COMP_ALIGN(ETHER_DMA_DESC_TBL_ALIGN) =
{
#if ETHER_RXBUFFCOUNT == 0
#error "ETHER_RXBUFFCOUNT == 0"
#endif
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[0UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[1UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[2UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[3UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[4UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[5UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[6UL].rxPktInfo)),
#if ETHER_RXBUFFCOUNT == 8UL
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_EN,  &(ETHER_RxPkt[7UL].rxPktInfo)),
#elif ETHER_RXBUFFCOUNT > 8UL
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[7UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[8UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[9UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_DIS, &(ETHER_RxPkt[10UL].rxPktInfo)),
    ETHER_INITRXDESC(AMAC_DMA_DESC_EOT_EN,  &(ETHER_RxPkt[11UL].rxPktInfo)),
#endif
#if (ETHER_RXBUFFCOUNT != 12UL) && (ETHER_RXBUFFCOUNT != 8UL)
#error "ETHER_RXBUFFCOUNT != 12UL && ETHER_RXBUFFCOUNT != 8UL"
#endif
};

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static AMAC_RDBType* const ETHER_AMACREGS = (AMAC_RDBType *)AMAC_BASE;

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static AMAC_TX_CHAN0_RDBType* const ETHER_AMACCHANREGS[4] = {
    (AMAC_TX_CHAN0_RDBType*) AMAC_TX_CHAN0_RDBTYPE_OFFSET,
    (AMAC_TX_CHAN0_RDBType*) AMAC_TX_CHAN1_RDBTYPE_OFFSET,
    (AMAC_TX_CHAN0_RDBType*) AMAC_TX_CHAN2_RDBTYPE_OFFSET,
    (AMAC_TX_CHAN0_RDBType*) AMAC_TX_CHAN3_RDBTYPE_OFFSET
};

/**
 * @brief Packet buffer information
 *
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT >= 1
static ETHER_TxPktBuffInfoType ETHER_TxCh0PktBuffInfo[ETHER_TXCH0BUFFCOUNT] COMP_SECTION(".data.drivers");
#endif
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT >= 2
static ETHER_TxPktBuffInfoType ETHER_TxCh1PktBuffInfo[ETHER_TXCH1BUFFCOUNT] COMP_SECTION(".data.drivers");
#endif
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT >= 3
static ETHER_TxPktBuffInfoType ETHER_TxCh2PktBuffInfo[ETHER_TXCH2BUFFCOUNT] COMP_SECTION(".data.drivers");
#endif
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_TX_CHAN_CNT >= 4
static ETHER_TxPktBuffInfoType ETHER_TxCh3PktBuffInfo[ETHER_TXCH3BUFFCOUNT] COMP_SECTION(".data.drivers");
#endif
/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
#if ETHER_RX_CHAN_CNT >= 1
static ETHER_RxPktBuffInfoType ETHER_RxPktBuffInfo[ETHER_RXBUFFCOUNT] COMP_SECTION(".data.drivers");
#endif

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_RxBufMgrInfoType ETHER_RxBufMgrInfo[ETHER_RX_CHAN_CNT] COMP_SECTION(".data.drivers") =
    {
#if ETHER_RX_CHAN_CNT == 0UL
#error "ETHER_RX_CHAN_CNT == 0UL"
#endif
        ETHER_INITRXBUFMGRINFO(ETHER_RxPktBuffInfo, ETHER_RXBUFFCOUNT, 0UL, 0UL, 0UL),
#if ETHER_RX_CHAN_CNT > 1UL
#error "ETHER_RX_CHAN_CNT > 1UL"
#endif
    };

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_TxBufMgrInfoType ETHER_TxBufMgrInfo[ETHER_TX_CHAN_CNT] COMP_SECTION(".data.drivers") =
    {
#if ETHER_TX_CHAN_CNT == 0UL
#error "ETHER_TX_CHAN_CNT == 0UL"
#endif
        ETHER_INITTXBUFMGRINFO(ETHER_TxCh0PktBuffInfo, ETHER_TXCH0BUFFCOUNT ,
                            (ETHER_TXCH0BUFFCOUNT  + 1UL), 0UL, 0UL, 0UL, 0UL),
#if ETHER_TX_CHAN_CNT > 1UL
        ETHER_INITTXBUFMGRINFO(ETHER_TxCh1PktBuffInfo, ETHER_TXCH1BUFFCOUNT,
                            (ETHER_TXCH1BUFFCOUNT + 1UL), 0UL, 0UL, 0UL, 0UL),
#endif
#if ETHER_TX_CHAN_CNT > 2UL
        ETHER_INITTXBUFMGRINFO(ETHER_TxCh2PktBuffInfo, ETHER_TXCH2BUFFCOUNT,
                            (ETHER_TXCH2BUFFCOUNT + 1UL), 0UL, 0UL, 0UL, 0UL),
#endif
#if ETHER_TX_CHAN_CNT > 3UL
        ETHER_INITTXBUFMGRINFO(ETHER_TxCh3PktBuffInfo, ETHER_TXCH3BUFFCOUNT,
                            (ETHER_TXCH3BUFFCOUNT + 1UL), 0UL, 0UL, 0UL, 0UL),
#endif
#if ETHER_TX_CHAN_CNT > 4UL
#error "ETHER_TX_CHAN_CNT > 4UL"
#endif
    };

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_RxChanInfoType ETHER_RxChanInfo[ETHER_RX_CHAN_CNT] COMP_SECTION(".data.drivers") =
    {
#if ETHER_RX_CHAN_CNT == 0UL
#error "ETHER_RX_CHAN_CNT == 0UL"
#endif
        ETHER_INITRXCHANNELINFO(ETHER_CHANSTATE_UNINIT, ETHER_RxBufMgrInfo,
                            ETHER_RxDmaDescTbl, FALSE),
 #if ETHER_RX_CHAN_CNT > 1UL
#error "ETHER_RX_CHAN_CNT > 1UL"
#endif
   };

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_TxChanInfoType ETHER_TxChanInfo[ETHER_TX_CHAN_CNT] COMP_SECTION(".data.drivers") =
    {
#if ETHER_TX_CHAN_CNT == 0UL
#error "ETHER_TX_CHAN_CNT == 0UL"
#endif
        ETHER_INITTXCHANNELINFO(ETHER_CHANSTATE_UNINIT, &ETHER_TxBufMgrInfo[0UL],
                            ETHER_TxCh0DmaDescTbl, FALSE),
#if ETHER_TX_CHAN_CNT > 1UL
        ETHER_INITTXCHANNELINFO(ETHER_CHANSTATE_UNINIT, &ETHER_TxBufMgrInfo[1UL],
                            ETHER_TxCh1DmaDescTbl, FALSE),
#endif
#if ETHER_TX_CHAN_CNT > 2UL
        ETHER_INITTXCHANNELINFO(ETHER_CHANSTATE_UNINIT, &ETHER_TxBufMgrInfo[2UL],
                            ETHER_TxCh2DmaDescTbl, FALSE),
#endif
#if ETHER_TX_CHAN_CNT > 3UL
        ETHER_INITTXCHANNELINFO(ETHER_CHANSTATE_UNINIT, &ETHER_TxBufMgrInfo[3UL],
                            ETHER_TxCh3DmaDescTbl, FALSE),
#endif
#if ETHER_TX_CHAN_CNT > 4UL
#error "ETHER_TX_CHAN_CNT > 4UL"
#endif
   };

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_ENABLETXTIMESTAMP_PROC
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef struct _ETHER_CntlrRWDevType {
    uint32_t cntlrState;
    ETHER_IRQStatsType irqStats;
    uint32_t chanID;
    uint32_t bufIdx;
    ETHER_ChanIDType prioChanMap[ETHER_PRIO_CHAN_MAP_SZ];
} ETHER_CntlrRWDevType;

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_ENABLETXTIMESTAMP_PROC
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
ETHER_CntlrRWDevType ETHER_CntlrRWDevData COMP_SECTION(".data.drivers") =
{
    .cntlrState = ETHER_STATE_UNINIT,
    .irqStats = {{0UL}},
    .chanID = 0UL,
    .bufIdx = 0UL,
    .prioChanMap = {0UL},
};

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
typedef struct _ETHER_MacDevType {
    uint32_t                    init;      /**< initialized? */
    uint32_t                    enabled;    /**< MAC enabled? */
    ETHER_SpeedType             speed;      /**< MAC speed */
    ETHER_DuplexModeType        duplexMode;   /**< MAC Duplex Mode */
    uint8_t                     macAddr[6UL]; /**< MAC Addresss configured */
} ETHER_MacDevType;

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static ETHER_MacDevType ETHER_MacDev COMP_SECTION(".data.drivers") = {
    .init = FALSE,
    .enabled = FALSE,
    .speed = 0UL,
    .macAddr = {(uint8_t)0},
};

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static uint32_t ETHER_XmtPtrLastDscrOffset[ETHER_TX_CHAN_CNT] COMP_SECTION(".data.drivers") = {0UL};

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static uint32_t ETHER_RcvPtrOffset[ETHER_RX_CHAN_CNT] COMP_SECTION(".data.drivers") = {0UL};

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
 * @trace #BRCM_SWARCH_ETHER_SYSCMDHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
       uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AMC_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_MacCheckConfigParams(const ETHER_CfgType *const aCfg)
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

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_MacEnableTxReset(uint32_t aEnable)
{
    uint32_t regVal = ETHER_AMACREGS->unimac0_command_config;

    if (aEnable == TRUE) {
        regVal |= AMAC_U0CC_COMMAND_CONFIG_SW_RESET_MASK;
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_TX_ENA_MASK;
    } else {
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_SW_RESET_MASK;
        regVal |= AMAC_U0CC_COMMAND_CONFIG_TX_ENA_MASK;
    }
    ETHER_AMACREGS->unimac0_command_config = regVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_MacEnableRxReset(uint32_t aEnable)
{
    uint32_t regVal = ETHER_AMACREGS->unimac0_command_config;

    if (aEnable == TRUE) {
        regVal |= AMAC_U0CC_COMMAND_CONFIG_SW_RESET_MASK;
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_RX_ENA_MASK;
    } else {
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_SW_RESET_MASK;
        regVal |= AMAC_U0CC_COMMAND_CONFIG_RX_ENA_MASK;
    }
    ETHER_AMACREGS->unimac0_command_config = regVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
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

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
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

/**
 * @trace #BRCM_SWARCH_ETHER_ENABLELOOPBACK_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static void ETHER_MacEnableLoopback(ETHER_HwIDType aID, uint32_t aLocal)
{
    uint32_t regVal = ETHER_AMACREGS->unimac0_command_config;

    if (ETHER_HW_ID_MAX > aID) {
        if (aLocal == TRUE) {
            regVal |= AMAC_U0CC_COMMAND_CONFIG_LOOP_ENA_MASK;
        } else  {
            regVal |= AMAC_U0CC_COMMAND_CONFIG_LINE_LOOPBACK_MASK;
        }

        ETHER_AMACREGS->unimac0_command_config = regVal;
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
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

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
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
    ETHER_AMACREGS->unimac0_mac_1 = macAddr0;
    ETHER_AMACREGS->unimac0_mac_0 = macAddr1;
    BCM_MemCpy(ETHER_MacDev.macAddr, aMacAddr, 6UL * sizeof(uint8_t));
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMACADDR_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_MacSetMacAddr(ETHER_HwIDType aID, const uint8_t *const aMacAddr)
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

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
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
        ETHER_AMACREGS->unimac0_frm_length = aConfig->maxRxFrmLen;

        regVal = ETHER_AMACREGS->unimac0_command_config;
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
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_ETH_SPEED_MASK;
        regVal |= (speedMask << AMAC_U0CC_COMMAND_CONFIG_ETH_SPEED_SHIFT)
                    & AMAC_U0CC_COMMAND_CONFIG_ETH_SPEED_MASK;
        /* Enable mode */
        regVal &= ~AMAC_U0CC_COMMAND_CONFIG_HD_ENA_MASK;
        regVal |= (modeMask << AMAC_U0CC_COMMAND_CONFIG_HD_ENA_SHIFT)
                    & AMAC_U0CC_COMMAND_CONFIG_HD_ENA_MASK;

        /* Enable RX FIFO overflow logic */
        regVal |= AMAC_U0CC_COMMAND_CONFIG_OVERFLOW_EN_MASK;

        /* Ignore Rx Pause Frames */
        regVal |= AMAC_U0CC_COMMAND_CONFIG_PAUSE_IGNORE_MASK;
        /* Disable Tx Pause Frames */
        regVal |= AMAC_U0CC_COMMAND_CONFIG_IGNORE_TX_PAUSE_MASK;

        ETHER_AMACREGS->unimac0_command_config = regVal;

        if (NULL != aConfig->macAddr) {
            ETHER_MacSetMacAddrInternal(aConfig->macAddr);
        }

        ETHER_MacDev.init = TRUE;
      }

err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
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
        ETHER_AMACREGS->unimac0_frm_length = 0x0UL;

        ETHER_MacSetMacAddrInternal(macAddrZero);
        ETHER_MacDev.init = FALSE;
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrXmtSetParityDisable(ETHER_ChanIDType aXmtCh,
                                    uint32_t aTxParityDis)
{
    uint32_t xmtCntrl;

    /* Set Tx Parity check */
    xmtCntrl = ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtcontrol_0;
    if (ETHER_TXPARITY_DIS == aTxParityDis) {
        xmtCntrl |= AMAC_G0X0_XMTCONTROL_0_TXPARITYCHECKDISABLE_0_MASK;
    } else {
        xmtCntrl &= ~AMAC_G0X0_XMTCONTROL_0_TXPARITYCHECKDISABLE_0_MASK;
    }

    ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtcontrol_0 = xmtCntrl;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrXmtEnable(ETHER_ChanIDType aXmtCh, uint32_t aEnable)
{
    uint32_t xmtCntrl;
    uint32_t count = ETHER_AMACRESETDELAYCNT;
    ETHER_DmaChanStateType refState = (TRUE == aEnable)?
                            AMAC_CHAN_STATE_IDLE : AMAC_CHAN_STATE_DISABLED;

    xmtCntrl = ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtcontrol_0;
    if (TRUE == aEnable) {
        xmtCntrl |= AMAC_G0X0_XMTCONTROL_0_XMTEN_0_MASK;
    } else {
        xmtCntrl &= ~AMAC_G0X0_XMTCONTROL_0_XMTEN_0_MASK;
    }
    ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtcontrol_0 = xmtCntrl;

    do  {
        if (((ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtstatus0_0
                        & AMAC_GMAC0_XMTSTATUS0_0_XMTSTATE_0_MASK)
                    >> AMAC_GMAC0_XMTSTATUS0_0_XMTSTATE_0_SHIFT) == refState) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                BRCM_SWDSGN_ETHER_CNTLRXMTENABLE_PROC,
                BCM_ERR_TIME_OUT, aXmtCh, 0UL, 0UL, 0UL);
    }
}

/**
 * TX channel reset sequence:
 * -> disable the channel (clear XmtEn bit in XmtControl register)
 * -> Poll until XmtState field in XmtStatus0 register indicated that channel
 *    is disabled
 * -> enable the channel (set Xmten bit in XmtControl Register)
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrXmtReset(ETHER_ChanIDType aXmtCh)
{
    ETHER_CntlrXmtEnable(aXmtCh, FALSE);
    ETHER_CntlrXmtEnable(aXmtCh, TRUE);
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_XmtSetDescTableAddr(ETHER_ChanIDType aXmtCh,
                uint32_t aAddrHigh, uint32_t aAddrLow)
{
    int32_t retVal = BCM_ERR_OK;

    /* Is transmit channel already enabled */
    if ((ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtcontrol_0
                & AMAC_G0X0_XMTCONTROL_0_XMTEN_0_MASK)
                     == AMAC_G0X0_XMTCONTROL_0_XMTEN_0_MASK) {
       retVal = BCM_ERR_BUSY;
       goto err;
    }

    if ((aAddrLow & 0xFUL) != 0UL) {
       retVal = BCM_ERR_INVAL_PARAMS;
       goto err;
    }

    ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtaddr_low_0 = (aAddrLow + AMAC_DMA_MEM_VIEW_BASE);
    ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtaddr_high_0 = aAddrHigh;

    /* Fix: ETHER_XmtPtrLastDscrOffset used to make sure correct offset calculation
     * with SUB-8KB aligned descriptor address ie: 1KB, 2KB and 4KB case.
     */
    ETHER_XmtPtrLastDscrOffset[aXmtCh] = (ETHER_AMACCHANREGS[aXmtCh]->gmac0_xmtptr_0 & AMAC_GMAC0_XMTPTR_0_LASTDSCR_0_MASK);
err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrRcvEnable(ETHER_ChanIDType aRcvCh, uint32_t aEnable)
{
    uint32_t rcvCntrl;
    ETHER_DmaChanStateType refState = (TRUE == aEnable) ?
                            AMAC_CHAN_STATE_IDLE : AMAC_CHAN_STATE_DISABLED;
    uint32_t count = ETHER_AMACRESETDELAYCNT;

    rcvCntrl = ETHER_AMACREGS->gmac0_rcvcontrol;
    if (TRUE == aEnable) {
        rcvCntrl |= AMAC_GMAC0_RCVCONTROL_REVEN_MASK;
    } else {
        rcvCntrl &= ~AMAC_GMAC0_RCVCONTROL_REVEN_MASK;
    }
    ETHER_AMACREGS->gmac0_rcvcontrol = rcvCntrl;

    do {
        if (((ETHER_AMACREGS->gmac0_rcvstatus0
                & AMAC_GMAC0_RCVSTATUS0_RCVSTATE_MASK)
             >> AMAC_GMAC0_RCVSTATUS0_RCVSTATE_SHIFT) == refState) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                BRCM_SWDSGN_ETHER_CNTLRRCVENABLE_PROC,
                BCM_ERR_TIME_OUT, aRcvCh, 0UL, 0UL, 0UL);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrRcvReset(ETHER_ChanIDType aRcvCh)
{
    ETHER_CntlrRcvEnable(aRcvCh, FALSE);
    ETHER_CntlrRcvEnable(aRcvCh, TRUE);
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_RcvSetDescTableAddr(ETHER_ChanIDType aRcvCh,
                uint32_t aAddrHigh, uint32_t aAddrLow)
{
    int32_t retVal = BCM_ERR_OK;

    /* Is receive channel already enabled */
    if ((ETHER_AMACREGS->gmac0_rcvcontrol & AMAC_GMAC0_RCVCONTROL_REVEN_MASK)
                            == AMAC_GMAC0_RCVCONTROL_REVEN_MASK) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

    if ((aAddrLow & 0xFUL) != 0UL) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    ETHER_AMACREGS->gmac0_rcvaddr_low = (aAddrLow + AMAC_DMA_MEM_VIEW_BASE);
    ETHER_AMACREGS->gmac0_rcvaddr_high = aAddrHigh;

    /* Fix: ETHER_RcvPtrOffset used to make sure correct offset
     * calculation with SUB-8KB aligned descriptor address ie: 1KB, 2KB
     * and 4KB case.
     */
    ETHER_RcvPtrOffset[aRcvCh] = (ETHER_AMACREGS->gmac0_rcvptr
                                     & AMAC_GMAC0_RCVPTR_RCVPTR_MASK);
err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntXmtInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal;

    ETHER_CntlrXmtEnable(aChan, FALSE);

    /* Set Descriptor table addr */
    retVal = ETHER_XmtSetDescTableAddr(aChan,
                                AMAC_DMA_DESC_DATAPTRHIGH_ZERO,
                                (uint32_t)ETHER_TxChanInfo[aChan].dmaDescTbl);
    if (BCM_ERR_OK == retVal) {
        ETHER_CntlrXmtSetParityDisable(aChan, ETHER_TXPARITY_DIS);
        /* Clear transmit interrupt status */
        ETHER_AMACREGS->gmac0_intstatus |=
                    (0x1UL << (AMAC_GMAC0_INTSTATUS_XMTINT_0_SHIFT + aChan));
        /* Enable tranmit interrupt */
        ETHER_AMACREGS->gmac0_intmask |=
                ((0x1UL << (AMAC_GMAC0_INTMASK_XMTINTEN_0_SHIFT + aChan))
                | (AMAC_GMAC0_INTMASK_XMTUFEN_MASK
                | AMAC_GMAC0_INTMASK_DESCRERREN_MASK
                | AMAC_GMAC0_INTMASK_DATAERREN_MASK
                | AMAC_GMAC0_INTMASK_DESCPROTOERREN_MASK
                | AMAC_GMAC0_INTMASK_TXQECCCORRECTEDEN_MASK
                | AMAC_GMAC0_INTMASK_TXQECCUNCORRECTEDEN_MASK));
        ETHER_CntlrXmtEnable(aChan, TRUE);
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntXmtDeInit(ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;

    /* Clear descriptor table */
    BCM_MemSet((void *)ETHER_TxChanInfo[aChan].dmaDescTbl, 0x0,
            ETHER_DESCTABLESIZE);

    /* Reset buffer info */
    ETHER_CntlrDeInitTxBuf(ETHER_TxChanInfo[aChan].bufMgrInfo);

    /* Update channel state */
    if (ETHER_CHANSTATE_INIT != ATOMIC_CheckUpdate(&(ETHER_TxChanInfo[aChan].state),
                                                ETHER_CHANSTATE_INIT,
                                                ETHER_CHANSTATE_UNINIT)) {
        retVal = BCM_ERR_INVAL_STATE;
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrXmtChanInfoInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    uint32_t i;

    ETHER_TxChanInfo[aChan].isErr = FALSE;

    /* Set buffer info */
    ETHER_CntlrInitTxBuf(ETHER_TxChanInfo[aChan].bufMgrInfo, ETHER_TxChPktBuf[aChan]);

    /* Set descriptor table */
    BCM_MemSet((void *)ETHER_TxChanInfo[aChan].dmaDescTbl, 0x0, ETHER_DESCTABLESIZE);

    for (i = 0UL; i <= ETHER_TxChanInfo[aChan].bufMgrInfo->pktBuffCnt; i++) {
        ETHER_SETDESCFIELDS(ETHER_TxChanInfo[aChan].dmaDescTbl[i],
                        AMAC_TXDMA_FLAG_CRC_MASK,
                        AMAC_DMA_DESC_EOT_EN,
                        AMAC_DMA_DESC_IOC_DIS,
                        AMAC_DMA_DESC_EOF_DIS,
                        AMAC_DMA_DESC_SOF_DIS,
                        AMAC_DMA_DESC_BUFCOUNT_ZERO,
                        AMAC_DMA_DESC_DATAPTRLOW_ZERO,
                        AMAC_DMA_DESC_DATAPTRHIGH_ZERO);
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntRcvInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal;
    uint32_t regVal = 0UL;
    const ETHER_DmaDescType *dmaDescTbl = ETHER_RxChanInfo[aChan].dmaDescTbl;

    ETHER_CntlrRcvEnable(aChan, FALSE);

    /* Set Descriptor table addr */
    retVal = ETHER_RcvSetDescTableAddr(aChan,
                               AMAC_DMA_DESC_DATAPTRHIGH_ZERO,
                               (uint32_t)dmaDescTbl);
    if (BCM_ERR_OK == retVal) {
        /* Reciever control configuration:
         * -> disable parity
         * -> set rcvoffset = 4
         * -> clear overflow continue
         */
        regVal = ETHER_AMACREGS->gmac0_rcvcontrol;
        regVal &= ~AMAC_GMAC0_RCVCONTROL_RCVOFFSET_MASK;
        regVal |= (0x4UL << AMAC_GMAC0_RCVCONTROL_RCVOFFSET_SHIFT);
        regVal |= AMAC_GMAC0_RCVCONTROL_RXPARITYCHECKDISABLE_MASK;
        regVal &= ~AMAC_GMAC0_RCVCONTROL_OFLOWCONTINUE_MASK;
        ETHER_AMACREGS->gmac0_rcvcontrol = regVal;

        /* Receiver atleast 1 frame before generating lazy interrupt */
        ETHER_AMACREGS->gmac0_intrcvlazy =
                            0x1UL << AMAC_GMAC0_INTRCVLAZY_FRAMECOUNT_SHIFT;

        /* Clear receive interrupt status */
        ETHER_AMACREGS->gmac0_intstatus |= AMAC_GMAC0_INTSTATUS_RCVINT_MASK;

        /* Enable receive interrupt */
        ETHER_AMACREGS->gmac0_intmask |= (AMAC_GMAC0_INTMASK_RCVINTEN_MASK
                                | AMAC_GMAC0_INTMASK_RCVFIFOOFEN_MASK
                                | AMAC_GMAC0_INTMASK_DESCRERREN_MASK
                                | AMAC_GMAC0_INTMASK_DATAERREN_MASK
                                | AMAC_GMAC0_INTMASK_DESCPROTOERREN_MASK
                                | AMAC_GMAC0_INTMASK_RXQECCCORRECTEDEN_MASK
                                | AMAC_GMAC0_INTMASK_RXQECCUNCORRECTEDEN_MASK);

        ETHER_CntlrRcvEnable(aChan, TRUE);

        ETHER_AMACREGS->gmac0_rcvptr = (ETHER_RXDESCOFFSET((ETHER_RxChanInfo[aChan].bufMgrInfo->pktBuffCnt - 1UL)) \
                                       + ETHER_RcvPtrOffset[0UL]) & AMAC_GMAC0_RCVPTR_RCVPTR_MASK;
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntRcvDeInit(ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;

    ETHER_CntlrDeInitRxBuf(ETHER_RxChanInfo[aChan].bufMgrInfo);

    if (ETHER_CHANSTATE_INIT != ATOMIC_CheckUpdate(&(ETHER_RxChanInfo[aChan].state),
                                                ETHER_CHANSTATE_INIT,
                                                ETHER_CHANSTATE_UNINIT)) {
        retVal = BCM_ERR_INVAL_STATE;
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static void ETHER_CntlrRcvChanInfoInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    ETHER_RxChanInfo[aChan].isErr = FALSE;
    ETHER_RxChanInfo[aChan].bufMgrInfo->pktBuffCnt = ETHER_RXBUFFCOUNT;
    ETHER_CntlrInitRxBuf(ETHER_RxChanInfo[aChan].bufMgrInfo,
                        ETHER_RxPkt, ETHER_RXBUFFCOUNT - 1UL);
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrRcvChanErrHandler(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHER_HW_ID_MAX > aID) && (ETHER_RX_CHAN_CNT > aChan)) {

        /* Re-initialise channel info and reset channel */
        ETHER_CntlrRcvChanInfoInit(aID, aChan);
        retVal = ETHER_CntlrIntRcvInit(aID, aChan);
        if (BCM_ERR_OK == retVal) {
            ETHER_MacEnableRxReset(FALSE);
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrXmtChanErrHandler(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    /* Re-initialise channel info and reset channel */
    ETHER_CntlrXmtChanInfoInit(aID, aChan);
    ETHER_CntlrXmtReset(aChan);
    return BCM_ERR_OK;
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrDequeueRcvCompPkts(ETHER_ChanIDType aChan)
{
    int32_t retVal;
    uint32_t currDesc;
    uint32_t currDescIdx;
    uint32_t dequeueIdx;
    uint8_t *buf;

    /* Disable receive interrupt */
    ETHER_AMACREGS->gmac0_intmask &= ~AMAC_GMAC0_INTMASK_RCVINTEN_MASK;

    currDesc = ((ETHER_AMACREGS->gmac0_rcvstatus0
                & AMAC_GMAC0_RCVSTATUS0_CURRDSCR_MASK)
               >> AMAC_GMAC0_RCVSTATUS0_CURRDSCR_SHIFT);
    currDesc = currDesc - ETHER_RcvPtrOffset[aChan];
    currDescIdx = (currDesc/ETHER_DMADESCSIZE);

    while (currDescIdx !=
            ETHER_CntlrPeekRxDequeueBuf(ETHER_RxChanInfo[aChan].bufMgrInfo)) {
        retVal = ETHER_CntlrDequeueRxBuf(ETHER_RxChanInfo[aChan].bufMgrInfo,
                                    &buf,
                                    &dequeueIdx);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }

    (ETHER_CntlrRWDevData.irqStats.rxChCompIRQCnt[aChan])++;

    /* Enable receive interrupt */
    ETHER_AMACREGS->gmac0_intmask|= (AMAC_GMAC0_INTMASK_RCVINTEN_MASK
                            | AMAC_GMAC0_INTMASK_RCVFIFOOFEN_MASK
                            | AMAC_GMAC0_INTMASK_DESCRERREN_MASK
                            | AMAC_GMAC0_INTMASK_DATAERREN_MASK
                            | AMAC_GMAC0_INTMASK_DESCPROTOERREN_MASK
                            | AMAC_GMAC0_INTMASK_RXQECCCORRECTEDEN_MASK
                            | AMAC_GMAC0_INTMASK_RXQECCUNCORRECTEDEN_MASK);
    return ;
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrDequeueXmtCompPkts(ETHER_ChanIDType aChan)
{
    int32_t retVal;
    uint32_t currDesc;
    uint32_t currDescIdx;

    /* Disable interrupt */
    ETHER_AMACREGS->gmac0_intmask &=
                    ~(0x1UL << (AMAC_GMAC0_INTMASK_XMTINTEN_0_SHIFT + aChan));

    currDesc = ((ETHER_AMACCHANREGS[aChan]->gmac0_xmtstatus0_0
                & AMAC_GMAC0_XMTSTATUS0_0_CURRDSCR_0_MASK)
                >> AMAC_GMAC0_XMTSTATUS0_0_CURRDSCR_0_SHIFT);
    currDesc = currDesc - ETHER_XmtPtrLastDscrOffset[aChan];
    currDescIdx = (currDesc/ETHER_DMADESCSIZE);

    while (currDescIdx !=
            ETHER_CntlrPeekTxDequeueBuf(ETHER_TxChanInfo[aChan].bufMgrInfo)) {
        retVal = ETHER_CntlrDequeueTxBuf(ETHER_TxChanInfo[aChan].bufMgrInfo);
        if (BCM_ERR_OK != retVal) {
            goto err;
        }
    }

err:
    (ETHER_CntlrRWDevData.irqStats.txChCompIRQCnt[aChan])++;
    /* Enable transmit interrupt */
    ETHER_AMACREGS->gmac0_intmask |=
                ((0x1UL << (AMAC_GMAC0_INTMASK_XMTINTEN_0_SHIFT + aChan))
                | (AMAC_GMAC0_INTMASK_XMTUFEN_MASK
                | AMAC_GMAC0_INTMASK_DESCRERREN_MASK
                | AMAC_GMAC0_INTMASK_DATAERREN_MASK
                | AMAC_GMAC0_INTMASK_DESCPROTOERREN_MASK
                | AMAC_GMAC0_INTMASK_TXQECCCORRECTEDEN_MASK
                | AMAC_GMAC0_INTMASK_TXQECCUNCORRECTEDEN_MASK));
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrRcvDMAErrIRQHandler(ETHER_ChanIDType aChan)
{
    ETHER_DmaChanErrType channErr;

    channErr = (ETHER_AMACREGS->gmac0_rcvstatus1
                & AMAC_GMAC0_RCVSTATUS1_RCVERR_MASK)
               >> AMAC_GMAC0_RCVSTATUS1_RCVERR_SHIFT;
    if ((channErr <= AMAC_CHAN_ERR_MAX) && (channErr >= AMAC_CHAN_ERR_MIN)) {
        /* 1. Reset UniMAC
         * 2. Set isErr flag
         */
        ETHER_MacEnableRxReset(TRUE);
        ETHER_RxChanInfo[0UL].isErr = TRUE;
    }
    return ;
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrXmtDMAErrIRQHandler(ETHER_ChanIDType aChan)
{
    ETHER_DmaChanErrType channErr;

    channErr = ((ETHER_AMACCHANREGS[aChan]->gmac0_xmtstatus1_0
                & AMAC_GMAC0_XMTSTATUS1_0_XMTERR_0_MASK)
               >> AMAC_GMAC0_XMTSTATUS1_0_XMTERR_0_SHIFT);
    if ((channErr <= AMAC_CHAN_ERR_MAX) && (channErr >= AMAC_CHAN_ERR_MIN)) {
        /* Set isErr flag */
        ETHER_TxChanInfo[aChan].isErr = TRUE;
    }
    return ;
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrDMAErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t i;

    /* check for DMA errors
     * In case of DMA error, reset the free packets and reset
     * the channel
     */
    if ((aIntStatus & ETHER_DMADESCERRMASK) != 0UL) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                BRCM_SWDSGN_ETHER_CNTLRDMAERRIRQHANDLER_PROC,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);

        for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
            ETHER_CntlrXmtDMAErrIRQHandler(i);
        }
        ETHER_CntlrRcvDMAErrIRQHandler(0UL);
    }
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrRcvErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t mask = (AMAC_GMAC0_INTSTATUS_RCVDESCUF_MASK |
                     AMAC_GMAC0_INTSTATUS_RCVFIFOOF_MASK);

    /* Receive DMA descriptor underflow error
     * Reset & deinitialise the receive channel.
     */
    if ((aIntStatus & mask) != 0UL) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                BRCM_SWDSGN_ETHER_CNTLRRCVERRIRQHANDLER_PROC,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);
        /* 1. Reset UniMAC
         * 2. Set isErr flag
         */
        ETHER_MacEnableRxReset(TRUE);
        ETHER_RxChanInfo[0UL].isErr = TRUE;
    }
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrXmtErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t i;

    /* transmit fifo overflow?
    */
    if ((aIntStatus & AMAC_GMAC0_INTSTATUS_XMTFIFOUF_MASK) != 0UL) {
        ETHER_CntlrReportError((uint8_t)ETHER_HW_ID_0,
                BRCM_SWDSGN_ETHER_CNTLRXMTERRIRQHANDLER_PROC,
                BCM_ERR_CUSTOM, aIntStatus, 0UL, 0UL, 0UL);
        /* Set isErr flag */
        for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
            ETHER_TxChanInfo[i].isErr = TRUE;
        }
    }
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrRcvCompIRQHandler(uint32_t aIntStatus,
                              ETHER_IRQEventType* const aType)
{
    /* reciever interrupt
     * Process the RX packets
     */
    if ((aIntStatus & AMAC_GMAC0_INTSTATUS_RCVINT_MASK) != 0UL) {
        ETHER_CntlrDequeueRcvCompPkts(0UL);
        *aType |= ETHER_IRQEVENT_PKT;
    }
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrXmtCompIRQHandler(uint32_t aIntStatus,
                              ETHER_IRQEventType* const aType)
{
    uint32_t i = 0UL;

    /* TX complete interrupt
     * free TX packets and give callback
     */
    for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
        if ((aIntStatus & (1UL << (AMAC_GMAC0_INTSTATUS_XMTINT_0_SHIFT + i))) != 0UL) {
            ETHER_CntlrDequeueXmtCompPkts(i);
            *aType |= ETHER_IRQEVENT_PKT;
        }
    }
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
static void ETHER_CntlrGPTimerIRQHandler(uint32_t aIntStatus,
                              ETHER_IRQEventType* const aType)
{
    /* GPTimer timeout interrupt */
    if ((aIntStatus & AMAC_GMAC0_INTSTATUS_INT_TIMEOUT_MASK) != 0UL) {
        *aType |= ETHER_IRQEVENT_TIMER;
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrXmtPutPkt(ETHER_HwIDType aID)
{
    uint32_t chanID = ETHER_CntlrRWDevData.chanID;
    return ETHER_CntlrDeallocTxBuf(ETHER_TxChanInfo[chanID].bufMgrInfo);
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrIntXmtGetPkt(ETHER_HwIDType aID,
                            ETHER_ChanIDType aChan,
                            uint32_t *const aTxBufIdx,
                            uint32_t *const aRaiseTxCb)
{
    int32_t retVal;
    uint32_t buffIdx = 0UL;
    uint32_t flags = 0UL;

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[aChan].state) {
        retVal = BCM_ERR_EAGAIN;
        goto err;
    }

    retVal = ETHER_CntlrGetDequeuedTxBuf(ETHER_TxChanInfo[aChan].bufMgrInfo,
                                        &buffIdx, &flags);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    if ((ETHER_PKTFLAGS_CBEN == (flags & ETHER_PKTFLAGS_CBEN)) &&
            (ETHER_PKTFLAGS_1588TXCONF != (flags & ETHER_PKTFLAGS_1588TXCONF))) {
        *aTxBufIdx = buffIdx;
        *aRaiseTxCb = TRUE;
        retVal = BCM_ERR_OK;
    } else {
        *aRaiseTxCb = FALSE;
        retVal = ETHER_CntlrXmtPutPkt(aID);
    }

err:
    if ((BCM_ERR_EAGAIN == retVal) && (TRUE == ETHER_TxChanInfo[aChan].isErr)) {
        retVal = ETHER_CntlrXmtChanErrHandler(aID, aChan);
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrXmtGetPkt(ETHER_HwIDType aID,
                            uint32_t *const aTxBufIdx,
                            uint32_t *const aRaiseTxCb)
{
    int32_t retVal = BCM_ERR_EAGAIN;
    uint32_t i;

    for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
        ETHER_CntlrRWDevData.chanID = i;
        do {
            retVal = ETHER_CntlrIntXmtGetPkt(aID, i, aTxBufIdx, aRaiseTxCb);
        } while ((BCM_ERR_OK == retVal) && (FALSE == *aRaiseTxCb));
        if (BCM_ERR_OK == retVal) {
            *aTxBufIdx = ETHER_GETCLIENTBUFIDX(i, *aTxBufIdx);
            break;
        } else {
            if (BCM_ERR_EAGAIN != retVal) {
                break;
            }
        }
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSTIMESTAMPS_PROC
 * @trace #BRCM_SWREQ_ETHER_TIME
 */
static int32_t ETHER_CntlrIntTxTSDoneInd(ETHER_HwIDType aID,
                             uint32_t aBufIdx)
{
    int32_t retVal;
    uint32_t bufIdx = ETHER_CLIENTBUFIDX2CHANBUFIDX(aBufIdx);
    ETHER_ChanIDType chan = ETHER_CLIENTBUFIDX2CHANIDX(aBufIdx);
    INTR_FlagType intFlags = INTR_Suspend();
    ETHER_TxPktBuffInfoType pktBuffInfo =
                        ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx];
    ETHER_TxPktBuffInfoType * pktBuffInfoPtr =
                        &(ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx]);
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

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static void ETHER_CntlrRcvPutPkt(ETHER_HwIDType aID)
{
    uint32_t lastDesc;
    uint32_t lastDescIdx;

    ETHER_CntlrDeallocRxBuf(ETHER_RxChanInfo[0UL].bufMgrInfo);

    lastDesc = ((ETHER_AMACREGS->gmac0_rcvptr & AMAC_GMAC0_RCVPTR_RCVPTR_MASK)
                >> AMAC_GMAC0_RCVPTR_RCVPTR_SHIFT);
    lastDesc = lastDesc - ETHER_RcvPtrOffset[0UL];
    lastDescIdx = lastDesc/sizeof(ETHER_DmaDescType);

    /* Queue the packet buffer to DMA */
    lastDescIdx = ModInc(lastDescIdx, ETHER_RxChanInfo[0UL].bufMgrInfo->pktBuffCnt);

    ETHER_AMACREGS->gmac0_rcvptr = ((ETHER_RXDESCOFFSET(lastDescIdx) \
                                    + ETHER_RcvPtrOffset[0UL]) \
                                    & AMAC_GMAC0_RCVPTR_RCVPTR_MASK);

    ETHER_CntlrQueueRxBuf(ETHER_RxChanInfo[0UL].bufMgrInfo);
}

/**
 * @trace #BRCM_SWARCH_ETHER_PROCESSPKTS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrRcvGetPkt(ETHER_HwIDType aID,
                            uint32_t *const aBufIdx,
                            uint8_t **const aPkt,
                            uint32_t *const aRaiseRxCb,
                            uint32_t *const aPktLen)
{
    int32_t retVal;
    uint32_t errData[4];
    ETHER_AMACRxPktInfoType rxPktInfo;
    ETHER_RxPktType *rxPkt;
    ETHER_ChanIDType chan = 0x0UL;

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo[chan].state) {
        retVal = BCM_ERR_EAGAIN;
        goto err;
    }

    *aRaiseRxCb = FALSE;
    *aPktLen = 0UL;

    retVal = ETHER_CntlrGetDequeuedRxBuf(ETHER_RxChanInfo[chan].bufMgrInfo,
                                    (uint8_t **)(&rxPkt),
                                    aBufIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    DCACHE_SysInvalRange((uint32_t)rxPkt, sizeof(ETHER_RxPktType));
    BCM_MemCpy((void *)&rxPktInfo, (void *)&rxPkt->rxPktInfo,
            sizeof(rxPktInfo));

    if ((0UL == rxPktInfo.crcErr)
            && (0UL == rxPktInfo.overSz)
            && (0UL == rxPktInfo.pktOvflw)
            && (0UL == rxPktInfo.ctfErr)) {
        *aRaiseRxCb = TRUE;
        *aPkt = &(rxPkt->pktBuff[0]);
        *aPktLen = (rxPktInfo.frameLen - 4UL);
    } else {
        errData[0] = rxPktInfo.crcErr;
        errData[1] = rxPktInfo.overSz;
        errData[2] = rxPktInfo.pktOvflw;
        errData[3] = rxPktInfo.ctfErr;
        ETHER_CntlrRcvPutPkt(aID);
        ETHER_CntlrReportError((uint8_t)aID,
                BRCM_SWDSGN_ETHER_CNTLRRCVGETPKT_PROC, BCM_ERR_DATA_INTEG, errData[0],
                errData[1], errData[2], errData[3]);
    }
    retVal = BCM_ERR_OK;
err:
    if ((BCM_ERR_EAGAIN == retVal) && (TRUE == ETHER_RxChanInfo[chan].isErr)) {
        retVal = ETHER_CntlrRcvChanErrHandler(aID, chan);
    }
    return retVal;
}

/**
 * Reset all channel and keep in disabled state
 * until ETHER_Init is called by respective client
 * @trace #BRCM_SWARCH_ETHER_DISABLEALLCHANNELS_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
void ETHER_DisableAllChannels(ETHER_HwIDType aID)
{
    uint32_t i;

    if (ETHER_HW_ID_MAX > aID) {

        for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
            ETHER_CntlrXmtReset(i);
            ETHER_CntlrXmtEnable(i, FALSE);
            /* Disable transmit interrupt */
            ETHER_AMACREGS->gmac0_intmask &=
                ~(0x1UL << (AMAC_GMAC0_INTMASK_XMTINTEN_0_SHIFT + i));
            /* Clear transmit interrupt status */
            ETHER_AMACREGS->gmac0_intstatus |=
                (0x1UL << (AMAC_GMAC0_INTSTATUS_XMTINT_0_SHIFT + i));
        }

        /* keep RX disabled */
        ETHER_CntlrRcvReset(0UL);
        ETHER_CntlrRcvEnable(0UL, FALSE);
        /* Disable receive interrupt */
        ETHER_AMACREGS->gmac0_intmask &= ~AMAC_GMAC0_INTMASK_RCVINTEN_MASK;
        /* Clear receive interrupt status */
        ETHER_AMACREGS->gmac0_intstatus |= AMAC_GMAC0_INTSTATUS_RCVINT_MASK;
    }
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrXmtInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_TX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_UNINIT != ETHER_TxChanInfo[aChan].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    ETHER_CntlrXmtChanInfoInit(aID, aChan);
    ETHER_CntlrRWDevData.irqStats.txChCompIRQCnt[aChan] = 0UL;
    retVal = ETHER_CntlrIntXmtInit(aID, aChan);
    if (BCM_ERR_OK == retVal) {
        if (ETHER_CHANSTATE_UNINIT !=
                        ATOMIC_CheckUpdate(&(ETHER_TxChanInfo[aChan].state),
                                                    ETHER_CHANSTATE_UNINIT,
                                                    ETHER_CHANSTATE_INIT)) {
            retVal = BCM_ERR_INVAL_STATE;
            /* This state mis-match shall never happen. Add abort/crash */
            goto err;
        }
    }

err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrRcvInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_RX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_UNINIT != ETHER_RxChanInfo[aChan].state) {
        goto err;
    }

    ETHER_CntlrRcvChanInfoInit(aID, aChan);
    ETHER_CntlrRWDevData.irqStats.rxChCompIRQCnt[aChan] = 0UL;
    retVal = ETHER_CntlrIntRcvInit(aID, aChan);
    if (BCM_ERR_OK == retVal) {
        if (ETHER_CHANSTATE_UNINIT !=
                        ATOMIC_CheckUpdate(&(ETHER_RxChanInfo[aChan].state),
                                                    ETHER_CHANSTATE_UNINIT,
                                                    ETHER_CHANSTATE_INIT)) {
            /* Add abort/crash */
            retVal = BCM_ERR_INVAL_STATE;
            goto err;
        }
    }
    ETHER_MacEnableRxReset(FALSE);

err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntInit(ETHER_HwIDType aID)
{
    uint32_t i;
    int32_t retVal;

    /* Disable MAC Tx and RX */
    retVal = ETHER_MacDisableTxRx();
    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < ETHER_TX_CHAN_CNT; i++) {
            retVal = ETHER_CntlrXmtInit(aID, i);
            if (BCM_ERR_OK != retVal) {
                break;
            }
        }

        if (BCM_ERR_OK == retVal) {
            /* Initialise receiver on successful initialisation of transmit
             * channel */
            retVal = ETHER_CntlrRcvInit(aID, 0UL);
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_INIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrInit(ETHER_HwIDType aID,
                            const ETHER_CfgType *const aConfig)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHER_HW_ID_MAX > aID)
            && (NULL != aConfig)) {
        if (ETHER_STATE_INIT == ETHER_CntlrRWDevData.cntlrState) {
            retVal = BCM_ERR_INVAL_STATE;
        } else {
            BCM_MemCpy(ETHER_CntlrRWDevData.prioChanMap, aConfig->prioChanMap,
                    (sizeof(ETHER_ChanIDType) * ETHER_PRIO_CHAN_MAP_SZ));
            retVal = ETHER_MacInit(aID, aConfig);
            if (BCM_ERR_OK == retVal) {
                ETHER_CntlrRWDevData.cntlrState = ETHER_STATE_INIT;
            }
        }
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static ETHER_ChanIDType ETHER_CntlrGetChanIdx(ETHER_PrioType aPriority)
{
    ETHER_ChanIDType chan = 3UL;
    if (aPriority < ETHER_PRIO_CHAN_MAP_SZ) {
        chan = ETHER_CntlrRWDevData.prioChanMap[aPriority];
    }
    return chan;
}

/**
 * @trace #BRCM_SWARCH_ETHER_GETTXBUFFER_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrXmtGetBuffer(ETHER_HwIDType aID,
                            ETHER_PrioType aPriority,
                            uint32_t *const aBufIdx,
                            uint8_t **const aBuf,
                            uint32_t *const aLenInOut)
{
    int32_t retVal;
    ETHER_ChanIDType chan;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_PRIO_MAX < aPriority)
        || (NULL == aBufIdx) || (NULL == aBuf) || (NULL == aLenInOut)
        || (0UL == *aLenInOut)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    chan = ETHER_CntlrGetChanIdx(aPriority);

    if (ETHER_TX_CHAN_CNT <= chan) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[chan].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    retVal = ETHER_CntlrAllocTxBuf(ETHER_TxChanInfo[chan].bufMgrInfo, aBufIdx, aBuf);
    if (BCM_ERR_OK == retVal) {
        *aLenInOut = ETHER_FRMSIZENOCRC - (2UL * ETHER_MAC_ADDR_SIZE);
        /* Generate buffer index using channel number and channels bufer index */
        *aBuf = &((*aBuf)[2UL * ETHER_MAC_ADDR_SIZE]);
        ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[*aBufIdx].clientPktBuff = *aBuf;
        *aBufIdx = ETHER_GETCLIENTBUFIDX(chan, *aBufIdx);
    } else {
        /* No free buffer */
        *aLenInOut = 0UL;
    }
err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SEND_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
 */
static int32_t ETHER_CntlrXmtSend(ETHER_HwIDType aID,
                         uint32_t aTxConfirmation,
                         uint32_t aBufIdx,
                         uint32_t aLen,
                         const uint8_t *const aDestMacAddr)
{
    int32_t retVal;
    uint32_t lastDesc;
    uint32_t currDesc;
    ETHER_DmaDescType *dmaDescTbl;
    uint8_t *pktBuff;
    uint32_t lastDescIdx;
    uint32_t currDescIdx;
    INTR_FlagType intFlags;
    uint32_t pktLen;
    ETHER_ChanIDType chan = ETHER_CLIENTBUFIDX2CHANIDX(aBufIdx);
    uint32_t bufIdx = ETHER_CLIENTBUFIDX2CHANBUFIDX(aBufIdx);

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_FRMSIZENOCRC < aLen)
        || (ETHER_TX_CHAN_CNT <= chan)
        || (ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffCnt <= bufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((NULL == aDestMacAddr) && (0UL != aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_PKTBUFFSTATE_ALLOC !=
        ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[chan].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* If aLen equal to zero, just free the packet */
    if (0UL == aLen) {
        retVal = ETHER_CntlrFreeTxBuf(ETHER_TxChanInfo[chan].bufMgrInfo, bufIdx);
        goto err;
    }

    /* Check for room in DMA Desc Table */
    lastDesc = (ETHER_AMACCHANREGS[chan]->gmac0_xmtptr_0
                & AMAC_GMAC0_XMTPTR_0_LASTDSCR_0_MASK);
    currDesc = ((ETHER_AMACCHANREGS[chan]->gmac0_xmtstatus0_0
                & AMAC_GMAC0_XMTSTATUS0_0_CURRDSCR_0_MASK)
                >> AMAC_GMAC0_XMTSTATUS0_0_CURRDSCR_0_SHIFT);
    /* Fix: ETHER_XmtPtrLastDscrOffset Used to make sure correct offset
     * calculation for SUB-8KB aligned descriptor address ie: 1KB, 2KB
     * and 4KB case.
     */
    lastDesc = lastDesc - ETHER_XmtPtrLastDscrOffset[chan];
    currDesc = currDesc - ETHER_XmtPtrLastDscrOffset[chan];

    lastDescIdx = lastDesc/sizeof(ETHER_DmaDescType);
    currDescIdx = currDesc/sizeof(ETHER_DmaDescType);

    if ((ModInc(lastDescIdx, ETHER_TXCHANDESCCNT(chan))) == currDescIdx) {
        /* TODO: Add assert/crash. */
        retVal = BCM_ERR_BUSY;
        ETHER_CntlrReportError((uint8_t)aID,
                BRCM_SWDSGN_ETHER_CNTLRXMTSEND_PROC,
                retVal, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    dmaDescTbl = ETHER_TxChanInfo[chan].dmaDescTbl;

    pktLen =  aLen + (2UL * ETHER_MAC_ADDR_SIZE);
    if (pktLen < ETHER_MINFRMSIZENOCRC) {
        pktLen =  ETHER_MINFRMSIZENOCRC;
    }

    /* Disable interrupt */
    intFlags = INTR_Suspend();

    retVal = ETHER_CntlrQueueTxBuf(ETHER_TxChanInfo[chan].bufMgrInfo, bufIdx,
                                &pktBuff, aTxConfirmation);
    if (BCM_ERR_OK == retVal) {

        BCM_MemCpy(pktBuff, aDestMacAddr, ETHER_MAC_ADDR_SIZE);
        BCM_MemCpy(&pktBuff[ETHER_MAC_ADDR_SIZE], ETHER_MacDev.macAddr,
                    ETHER_MAC_ADDR_SIZE);

        lastDesc = (ETHER_AMACCHANREGS[chan]->gmac0_xmtptr_0
                    & AMAC_GMAC0_XMTPTR_0_LASTDSCR_0_MASK);
        /* Fix: ETHER_XmtPtrLastDscrOffset Used to make sure correct offset
         * calculation for SUB-8KB aligned descriptor address ie: 1KB, 2KB
         * and 4KB case.
         */
        lastDesc = lastDesc - ETHER_XmtPtrLastDscrOffset[chan];
        lastDescIdx = lastDesc/ETHER_DMADESCSIZE;

        ETHER_SETDESCFIELDS(dmaDescTbl[lastDescIdx],
                AMAC_TXDMA_FLAG_CRC_APPEND,
                AMAC_DMA_DESC_EOT_DIS,
                AMAC_DMA_DESC_IOC_EN,
                AMAC_DMA_DESC_EOF_EN,
                AMAC_DMA_DESC_SOF_EN,
                pktLen,
                (((uint32_t)(pktBuff)) + AMAC_DMA_MEM_VIEW_BASE),
                AMAC_DMA_DESC_DATAPTRHIGH_ZERO);

        lastDescIdx = ModInc(lastDescIdx, ETHER_TXCHANDESCCNT(chan));
        if (lastDescIdx == 0UL) {
            ETHER_SETDESCFIELD(
                    dmaDescTbl[ModDec(lastDescIdx, ETHER_TXCHANDESCCNT(chan))],
                    eot, AMAC_DMA_DESC_EOT_EN);
        }

        /* Fix: ETHER_XmtPtrLastDscrOffset Used to make sure correct offset
         * calculation for SUB-8KB aligned descriptor address ie: 1KB, 2KB
         * and 4KB case.
         */
        ETHER_AMACCHANREGS[chan]->gmac0_xmtptr_0 = (ETHER_TXDESCOFFSET(lastDescIdx) \
                                                   + ETHER_XmtPtrLastDscrOffset[chan]) \
                                                   & AMAC_GMAC0_XMTPTR_0_LASTDSCR_0_MASK;
    }

    /* Enable Interrupt */
    INTR_Resume(intFlags);

err:
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrRcvDeInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;
    INTR_FlagType intFlags;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_RX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo[aChan].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }
    intFlags = INTR_Suspend();
    ETHER_MacEnableRxReset(TRUE);
    ETHER_CntlrRcvEnable(aChan, FALSE);
    retVal = ETHER_CntlrIntRcvDeInit(aChan);
    INTR_Resume(intFlags);

    /* Disable receive interrupt */
    ETHER_AMACREGS->gmac0_intmask &= ~AMAC_GMAC0_INTMASK_RCVINTEN_MASK;

err:
    if (BCM_ERR_OK != retVal) {
        ETHER_CntlrReportError((uint8_t)aID,
            BRCM_SWDSGN_ETHER_CNTLRRCVDEINIT_PROC, retVal, aChan, 0UL, 0UL, 0UL);
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrXmtDeInit(ETHER_HwIDType aID, ETHER_ChanIDType aChan)
{
    int32_t retVal = BCM_ERR_OK;
    INTR_FlagType intFlags;

    if ((ETHER_HW_ID_MAX <= aID) || (ETHER_TX_CHAN_CNT <= aChan)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[aChan].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    intFlags = INTR_Suspend();
    ETHER_CntlrXmtEnable(aChan, FALSE);
    retVal = ETHER_CntlrIntXmtDeInit(aChan);
    INTR_Resume(intFlags);

    /* Disable transmit interrupt */
    ETHER_AMACREGS->gmac0_intmask &=
                   ~(0x1UL << (AMAC_GMAC0_INTMASK_XMTINTEN_0_SHIFT + aChan));

err:
    if (BCM_ERR_OK != retVal) {
        ETHER_CntlrReportError((uint8_t)aID,
            BRCM_SWDSGN_ETHER_CNTLRXMTDEINIT_PROC, retVal, aChan, 0UL, 0UL, 0UL);
    }
    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrIntDeInit(ETHER_HwIDType aID)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    for (i = 0UL; i < ETHER_TX_CHAN_CNT ; i++) {
        retVal = ETHER_CntlrXmtDeInit(aID, i);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }

    if (BCM_ERR_OK == retVal) {
        retVal = ETHER_CntlrRcvDeInit(aID, 0UL);
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_DEINIT_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrDeInit(ETHER_HwIDType aID)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ETHER_HW_ID_MAX > aID) {
        if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = ETHER_CntlrIntDeInit(aID);
            if (BCM_ERR_OK == retVal) {
                retVal = ETHER_MacDeInit(aID);
                if (BCM_ERR_OK == retVal) {
                    ETHER_CntlrRWDevData.cntlrState = ETHER_STATE_UNINIT;
                }
            }
        }
    }

    return retVal;
}

/**
 * @trace #BRCM_SWARCH_ETHER_SETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrSetMode(ETHER_HwIDType aID, ETHER_ModeType aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ETHER_HW_ID_MAX > aID) {
        if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
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

/**
 * @trace #BRCM_SWARCH_ETHER_GETMODE_PROC
 * @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
 */
static int32_t ETHER_CntlrGetMode(ETHER_HwIDType aID,
                                ETHER_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((ETHER_HW_ID_MAX > aID) && (NULL != aMode)) {
        if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            if ((TRUE == ETHER_MacDev.init) && (TRUE == ETHER_MacDev.enabled)) {
                *aMode = ETHER_MODE_ACTIVE;
            } else {
                *aMode = ETHER_MODE_DOWN;
            }
            retVal = BCM_ERR_OK;
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_GETRXSTATS_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
static int32_t ETHER_CntlrGetStats(ETHER_HwIDType aID, ETHER_RxStatsType *const aStats)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHER_HW_ID_MAX <= aID) || (NULL == aStats)){
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (ETHER_CHANSTATE_INIT != ETHER_RxChanInfo[0UL].state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    aStats->gdPkts = ETHER_AMACREGS->gmac0_rx_gd_pkts;
    aStats->octetsLow = ETHER_AMACREGS->gmac0_rx_gd_octets_lo;
    aStats->octetsHigh = ETHER_AMACREGS->gmac0_rx_gd_octets_hi;
    aStats->allPkts = ETHER_AMACREGS->gmac0_rx_all_pkts;
    aStats->brdCast = ETHER_AMACREGS->gmac0_rx_brdcast;
    aStats->mutCast = ETHER_AMACREGS->gmac0_rx_mult;
    aStats->pkts64 = ETHER_AMACREGS->gmac0_rx_64;
    aStats->pkts65_127 = ETHER_AMACREGS->gmac0_rx_65_127;
    aStats->pkts128_255 = ETHER_AMACREGS->gmac0_rx_128_255;
    aStats->pkts256_511 = ETHER_AMACREGS->gmac0_rx_256_511;
    aStats->pkts512_1023 = ETHER_AMACREGS->gmac0_rx_512_1023;
    aStats->pkts1024_1522 = ETHER_AMACREGS->gmac0_rx_1024_1522;
    aStats->pkts1523_2047 = ETHER_AMACREGS->gmac0_rx_1523_2047;
    aStats->pkts2048_4095 = ETHER_AMACREGS->gmac0_rx_2048_4095;
    aStats->pkts4096_8191 = ETHER_AMACREGS->gmac0_rx_4096_8191;
    aStats->pkts8192_MAX = ETHER_AMACREGS->gmac0_rx_8192_max;
    aStats->pktsJabber = ETHER_AMACREGS->gmac0_rx_jab;
    aStats->pktsOvrSz = ETHER_AMACREGS->gmac0_rx_ovr;
    aStats->pktsFrag = ETHER_AMACREGS->gmac0_rx_frag;
    aStats->pktsRxDrop = ETHER_AMACREGS->gmac0_rx_drop;
    aStats->pktsCrcAlignErr = ETHER_AMACREGS->gmac0_rx_crc_align;
    aStats->pktsUndSz = ETHER_AMACREGS->gmac0_rx_und;
    aStats->pktsCrcErr = ETHER_AMACREGS->gmac0_rx_crc;

err:
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_ENABLETXTIMESTAMP_PROC
    @trace  #BRCM_SWREQ_ETHER_TIME
*/
static int32_t ETHER_CntlrXmtMarkTSPkt(ETHER_HwIDType aID,
                             uint32_t aBufIdx)
{
    int32_t retVal = BCM_ERR_OK;
    ETHER_ChanIDType chan = ETHER_CLIENTBUFIDX2CHANIDX(aBufIdx);
    uint32_t bufIdx = ETHER_CLIENTBUFIDX2CHANBUFIDX(aBufIdx);

    if ((ETHER_HW_ID_MAX <= aID)
        || (ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffCnt <= bufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
        retVal = BCM_ERR_UNINIT;
    } else if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[chan].state) {
        retVal = BCM_ERR_INVAL_STATE;
    } else if (ETHER_PKTBUFFSTATE_ALLOC !=
          ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx].state) {
        retVal = BCM_ERR_INVAL_BUF_STATE;
    } else {
        ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx].flags |=
            ETHER_PKTFLAGS_1588TXCONF;
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_SEND_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
static int32_t ETHER_CntlrXmtValidateBuffer(ETHER_HwIDType aID,
                                     uint32_t aBufIdx)
{
    int32_t retVal = BCM_ERR_OK;
    ETHER_ChanIDType chan = ETHER_CLIENTBUFIDX2CHANIDX(aBufIdx);
    uint32_t bufIdx = ETHER_CLIENTBUFIDX2CHANBUFIDX(aBufIdx);

    if ((ETHER_HW_ID_MAX <= aID) ||
        (ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffCnt <= bufIdx)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            if (ETHER_CHANSTATE_INIT != ETHER_TxChanInfo[chan].state) {
                retVal = BCM_ERR_INVAL_STATE;
            }
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_SEND_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_COMMUNICATION
*/
static int32_t ETHER_CntlrIsXmtPktTSMarked(ETHER_HwIDType aID,
                                  uint32_t aBufIdx)
{
    ETHER_ChanIDType chan = ETHER_CLIENTBUFIDX2CHANIDX(aBufIdx);
    uint32_t bufIdx = ETHER_CLIENTBUFIDX2CHANBUFIDX(aBufIdx);
    int32_t retVal = ETHER_CntlrXmtValidateBuffer(aID, aBufIdx);

    if (BCM_ERR_OK == retVal) {
        if (ETHER_PKTFLAGS_1588TXCONF !=
                (ETHER_TxChanInfo[chan].bufMgrInfo->pktBuffInfo[bufIdx].flags
                 & ETHER_PKTFLAGS_1588TXCONF)) {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_SETGPTIMER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_GPTIME
*/
static int32_t ETHER_CntlrSetGPTimer(ETHER_HwIDType aID,
                            uint32_t       aPeriod)
{
    int32_t  retVal = BCM_ERR_OK;

    if (ETHER_HW_ID_MAX <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHER_STATE_INIT != ETHER_CntlrRWDevData.cntlrState) {
            retVal = BCM_ERR_UNINIT;
        } else {
            if (((aPeriod * AMAC_TIMER_FREQ_MHZ) % 1000UL) == 0UL) {
                ETHER_AMACREGS->gmac0_gptimer =
                                        (AMAC_TIMER_FREQ_MHZ * aPeriod)/1000UL;

                /* Enable/Disable the interrupt according to the period */
                if (0UL == aPeriod) {
                    ETHER_AMACREGS->gmac0_intmask &=
                                        ~AMAC_GMAC0_INTMASK_INT_TIMEOUTEN_MASK;
                } else {
                    ETHER_AMACREGS->gmac0_intmask |=
                                        AMAC_GMAC0_INTMASK_INT_TIMEOUTEN_MASK;
                }
            } else {
                retVal = BCM_ERR_INVAL_PARAMS;
            }
        }
    }

    if (retVal != BCM_ERR_OK) {
        ETHER_CntlrReportError((uint8_t)aID,
                         BRCM_SWDSGN_ETHER_CNTLRSETGPTIMER_PROC,
                         retVal, aID, aPeriod, 0UL, __LINE__);
    }
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
void ETHER_IntIRQHandler(ETHER_HwIDType aID, ETHER_IRQEventType* const aType)
{
    uint32_t intStatus;

    intStatus = ETHER_AMACREGS->gmac0_intstatus;

    /* This return is required to see if the spurious
       interrupts are generated. It is required to capture
       spurious interrupt recieved otherwise it might be
       stalling CPU & will be difficult to debug */
    if (intStatus == 0UL) {
        ETHER_CntlrReportError((uint8_t)aID,
                BRCM_SWARCH_ETHER_INTIRQHANDLER_PROC, BCM_ERR_CUSTOM,
                intStatus, 0UL, 0UL, __LINE__);
        goto err;
    }

    /* clear the interrupts */
    ETHER_AMACREGS->gmac0_intstatus = intStatus;

    if (ETHER_STATE_INIT == ETHER_CntlrRWDevData.cntlrState) {
        ETHER_CntlrRWDevData.irqStats.lastIRQStatus = intStatus;

        /* GP timer timeout handler */
        ETHER_CntlrGPTimerIRQHandler(intStatus, aType);

        /* Transmit and receive completion handlers */
        ETHER_CntlrRcvCompIRQHandler(intStatus, aType);
        ETHER_CntlrXmtCompIRQHandler(intStatus, aType);

        /* Error handlers */
        ETHER_CntlrRcvErrIRQHandler(intStatus);
        ETHER_CntlrDMAErrIRQHandler(intStatus);
        ETHER_CntlrXmtErrIRQHandler(intStatus);
    }

err:
    return ;
}

/**
    @trace  #BRCM_SWARCH_ETHER_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_CONTROLLER_KERNEL_HANDLER
*/
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

    @trace  #BRCM_SWARCH_ETHER_SYSCMDREQ_PROC
    @trace  #BRCM_SWARCH_ETHER_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
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

    @trace  #BRCM_SWARCH_ETHER_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_ETHER_KERNEL_HANDLER
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

/** @} */
/* Nothing past this line */
