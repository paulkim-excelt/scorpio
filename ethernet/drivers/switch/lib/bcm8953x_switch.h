/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

#define ARL_TBL_BIN_SIZE                    (4UL)
#define ARL_TBL_MAX_ENTRIES                 (4096)
#define COS_QUEUE_0                         (0UL)
#define COS_QUEUE_1                         (1UL)
#define COS_QUEUE_2                         (2UL)
#define COS_QUEUE_3                         (3UL)
#define COS_QUEUE_4                         (4UL)
#define COS_QUEUE_5                         (5UL)
#define COS_QUEUE_6                         (6UL)
#define COS_QUEUE_7                         (7UL)
#define COS_QUEUE_MAX                       (COS_QUEUE_7)

#define QUEUE_FLOW_CTRL_THRSLD_UNIT        (256UL) /**< theshold per page : 256 bytes */
#define QUEUE_SHAPER_BIT_RATE_PER_TOCKEN   (64000UL) /**< (0.5 bits / 7.8125us) = 64kb/s */
#define QUEUE_SHAPER_BUCKET_UNIT           (64UL)  /** Bucket size is in multiples of 64B */
/* register space */

/* switch control registers */
#define CTRL_PAGE                          (0UL)
#define CTRL_SFT_LRN_CTL_OFFSET            (0x3EUL)
#define CTRL_SFT_LRN_CTL_SW_MASK           (0x01ffUL)
#define CTRL_SFT_LRN_CTL_SW_SHIFT          (0UL)
#define CTRL_DIS_LEARN_OFFSET              (0x3CUL)
#define CTRL_DIS_LEARN_MASK                (0x01ffUL)
#define CTRL_DIS_LEARN_DIS_LEARN_SHIFT     (0UL)
#define CTRL_LED_FUNC1_OFFSET              (0x12UL)
#define CTRL_LED_FUNC1_AVB_LINK_MASK       (0x4000UL)
#define CTRL_LED_FUNC1_SPD1G_MASK          (0x0200UL)
#define CTRL_LED_FUNC1_SPD10M_MASK         (0x0100UL)
#define CTRL_LED_FUNC1_LNK_ACT_MASK        (0x0020UL)
#define CTRL_LED_EN_MAP_OFFSET             (0x16UL)
#define CTRL_LED_EN_MAP_MASK               (0x3FUL)
#define CTRL_MII_DUMB_FWD_OFFSET           (0x22UL)
#define CTRL_MII_DUMB_FWD_MASK             (0x40UL)
#define CTRL_LT_STK_CTRL_OFFSET            (0xECUL)
#define CTRL_LT_STK_EN_MASK                (0x0001UL)
#define CTRL_LT_STK_MAST_EN_MASK           (0x0002UL)
#define CTRL_LT_STK_PORT1_EN_MASK          (0x0004UL)
#define CTRL_LT_STK_PORT1_MASK             (0x0070UL)
#define CTRL_LT_STK_PORT1_SHIFT            (4UL)
#define CTRL_LT_STK_PORT0_MASK             (0x0700UL)
#define CTRL_LT_STK_PORT0_SHIFT            (8UL)

/* Switch status registers */
#define STATUS_PAGE                        (1UL)
#define STATUS_LNK_STS_OFFSET              (0x00UL)
#define LNK_STS_PORT_XLD7_MASK             (0x17FUL)
#define STATUS_LNK_STS_CHG_OFFSET          (0x02UL)
#define LNK_STS_CHG_PORT_XLD7_MASK         (0x17FUL)
#define LNK_STS_CHG_PORT_MASK              (0x1FFUL)

/* management mode regsiters */
#define MNGMODE_PAGE                            (0x2UL)
#define MNGMODE_BRCM_HDR_CTRL_OFFSET            (0x03UL)
#define MNGMODE_BRCM_HDR_CTRL_BRCM_HDR_EN_MASK  (0x07UL)
#define BRCM_HDR_CTRL_P7_EN_MASK                (0x4UL)

#define MNGMODE_AGE_TIME_CTRL_OFFSET            (0x06UL)
#define MNGMODE_AGE_CHANGE_EN_MASK              (0x100000UL)
#define MNGMODE_AGE_TIME_MASK                   (0xFFFFFUL)

#define BCM895XX_AGE_TIMER_MAX                  (1048575UL)

#define MNGMODE_MIRCAPCTL_OFFSET                (0x10UL)
#define MNGMODE_MIRCAPCTL_SMIR_CAP_PORT_MASK    (0x000fUL)
#define MNGMODE_MIRCAPCTL_SMIR_CAP_PORT_SHIFT   (0UL)
#define MNGMODE_MIRCAPCTL_BLK_NOT_MIR_MASK      (0x4000UL)
#define MNGMODE_MIRCAPCTL_BLK_NOT_MIR_SHIFT     (14UL)
#define MNGMODE_MIRCAPCTL_MIR_EN_MASK           (0x8000UL)
#define MNGMODE_MIRCAPCTL_MIR_EN_SHIFT          (15UL)
#define MNGMODE_IGMIRCTL_OFFSET                 (0x12UL)
#define MNGMODE_IGMIRDIV_OFFSET                 (0x14UL)
#define MNGMODE_IGMIRDIV_IN_MIR_DIV_MASK        (0x03ffUL)
#define MNGMODE_IGMIRDIV_IN_MIR_DIV_SHIFT       (0UL)
#define MNGMODE_IGMIRMAC_OFFSET                 (0x16UL) /* Ingress Mirror Mac Address Register */
#define MNGMODE_IGMIRMAC_IN_MIR_MAC_MASK        (0x0000ffffffffffffULL)
#define MNGMODE_IGMIRMAC_IN_MIR_MAC_SHIFT       (0UL)
#define MNGMODE_EGMIRCTL_OFFSET                 (0x1cUL) /* Egress Mirror Control Register */
#define MNGMODE_EGMIRCTL_OUT_MIR_MSK_MASK       (0x01ffUL)
#define MNGMODE_EGMIRCTL_OUT_MIR_MSK_SHIFT      (0UL)
#define MNGMODE_EGMIRCTL_OUT_DIV_EN_MASK        (0x2000UL)
#define MNGMODE_EGMIRCTL_OUT_DIV_EN_SHIFT       (13UL)
#define MNGMODE_EGMIRCTL_OUT_MIR_FLTR_MASK      (0xc000UL)
#define MNGMODE_EGMIRCTL_OUT_MIR_FLTR_SHIFT     (14UL)
#define MNGMODE_EGMIRDIV_OFFSET                 (0x1eUL) /* Egress Mirror Divider Register */
#define MNGMODE_EGMIRDIV_OUT_MIR_DIV_MASK       (0x03ffUL)
#define MNGMODE_EGMIRDIV_OUT_MIR_DIV_SHIFT      (0UL)
#define MNGMODE_EGMIRMAC_OFFSET                 (0x20UL) /* Egress Mirror MAC Address Register */
#define MNGMODE_EGMIRMAC_OUT_MIR_MAC_MASK       (0x0000ffffffffffffULL)
#define MNGMODE_EGMIRMAC_OUT_MIR_MAC_SHIFT      (0UL)

#define MNGMODE_MIRCTL_MIR_MSK_MASK             (0x01ffUL)
#define MNGMODE_MIRCTL_MIR_MSK_SHIFT            (0UL)
#define MNGMODE_MIRCTL_DIV_EN_MASK              (0x2000UL)
#define MNGMODE_MIRCTL_DIV_EN_SHIFT             (13UL)
#define MNGMODE_MIRCTL_MIR_FLTR_MASK            (0xc000UL)
#define MNGMODE_MIRCTL_MIR_FLTR_SHIFT           (14UL)
#define MIR_FLTR_ALL_PACKETS                 (0x0UL << MNGMODE_MIRCTL_MIR_FLTR_SHIFT)
#define MIR_FLTR_DA_MATCH                    (0x1UL << MNGMODE_MIRCTL_MIR_FLTR_SHIFT)
#define MIR_FLTR_SA_MATCH                    (0x2UL << MNGMODE_MIRCTL_MIR_FLTR_SHIFT)

/* Control 1 page */
#define CTRL1_PAGE                          (3UL)
#define CTRL1_EXT_HOST_RAW_INT_STAT_OFFSET  (0x0UL)
#define CTRL1_EXT_HOST_RAW_INT_EN_OFFSET    (0x8UL)
#define CTRL1_EXT_HOST_RAW_INT_LNK_SHIFT    (16UL)
#define CTRL1_LNK_STS_INT_EN_OFFSET         (0x24UL)
#define LNK_STS_INT_EN_PORT_MASK            (0x1FFUL)
#define LNK_STS_INT_EN_PORT_XLD7_MASK       (0x17FUL)

/* ARL Access registers */
#define ARLA_PAGE                          (5UL)
#define ARLA_RWCTL_OFFSET                  (0x00UL)
#define ARLA_MAC_OFFSET                    (0x02UL)
#define ARLA_VID_OFFSET                    (0x08UL)
#define ARLA_MACVID_ENTRY0_OFFSET          (0x10UL)
#define ARLA_FWD_ENTRY0_OFFSET             (0x18UL)
#define ARLA_MACVID_ENTRY1_OFFSET          (0x20UL)
#define ARLA_FWD_ENTRY1_OFFSET             (0x28UL)
#define ARLA_MACVID_ENTRY2_OFFSET          (0x30UL)
#define ARLA_FWD_ENTRY2_OFFSET             (0x38UL)
#define ARLA_MACVID_ENTRY3_OFFSET          (0x40UL)
#define ARLA_FWD_ENTRY3_OFFSET             (0x48UL)
#define ARLA_SRCH_CTL_OFFSET               (0x50UL)
#define ARLA_SRCH_RSLT_0_MACVID_OFFSET     (0x60UL)
#define ARLA_SRCH_RSLT_0_OFFSET            (0x68UL)
#define ARLA_SRCH_RSLT_1_MACVID_OFFSET     (0x70UL)
#define ARLA_SRCH_RSLT_1_OFFSET            (0x78UL)
#define ARLA_VTBL_RWCTRL_OFFSET            (0x80UL)
#define ARLA_VTBL_ADDR_OFFSET              (0x81UL)
#define ARLA_VTBL_ENTRY_OFFSET             (0x83UL)

#define ARLA_RWCTL_ARL_RW_MASK             (0x01UL)
#define ARLA_RWCTL_ARL_RW_SHIFT            (0x00UL)
#define ARLA_RWCTL_ARL_READ                (0x01UL)
#define ARLA_RWCTL_ARL_WRITE               (0x00UL)
#define ARLA_RWCTL_ARL_STRTDN_MASK         (0x80UL)
#define ARLA_MAC_MAC_ADDR_INDX_MASK        (0x0000ffffffffffffULL)
#define ARLA_MAC_MAC_ADDR_INDX_SHIFT       (0ULL)
#define ARLA_VIDTAB_INDX_MASK              (0x0fffUL)
#define ARLA_VIDTAB_INDX_SHIFT             (0UL)
#define ARLA_MACVID_ENTRY_MACADDR_MASK    (0x0000ffffffffffffULL)
#define ARLA_MACVID_ENTRY_MACADDR_SHIFT   (0ULL)
#define ARLA_MACVID_ENTRY_VID_MASK        (0x0fff000000000000ULL)
#define ARLA_MACVID_ENTRY_VID_SHIFT       (48ULL)
#define ARLA_FWD_ENTRY_PORTID_MASK        (0x000001ffUL)
#define ARLA_FWD_ENTRY_PORTID_SHIFT       (0UL)
#define ARLA_FWD_ENTRY_ARL_CON_MASK        (0x00000600UL)
#define ARLA_FWD_ENTRY_ARL_MODE_FWD_MAP    (0x00UL << ARLA_FWD_ENTRY_PORTID_SHIFT)
#define ARLA_FWD_ENTRY_ARL_MODE_DROP_DEST  (0x1UL << ARLA_FWD_ENTRY_ARL_CON_SHIFT)
#define ARLA_FWD_ENTRY_ARL_MODE_DROP_SRC   (0x2UL << ARLA_FWD_ENTRY_ARL_CON_SHIFT)
#define ARLA_FWD_ENTRY_ARL_MODE_COPY_CPU   (0x3UL << ARLA_FWD_ENTRY_ARL_CON_SHIFT)

#define ARLA_FWD_ENTRY_ARL_CON_SHIFT       (9UL)
#define ARLA_FWD_ENTRY_ARL_PRI_MASK       (0x00003800UL)
#define ARLA_FWD_ENTRY_ARL_PRI_SHIFT      (11UL)
#define ARLA_FWD_ENTRY_ARL_AGE_MASK       (0x00004000UL)
#define ARLA_FWD_ENTRY_ARL_AGE_SHIFT      (14UL)
#define ARLA_FWD_ENTRY_ARL_STATIC_MASK    (0x00008000UL)
#define ARLA_FWD_ENTRY_ARL_STATIC_SHIFT  (15UL)
#define ARLA_FWD_ENTRY_ARL_VALID_MASK     (0x00010000UL)

#define ARLA_SRCH_CTL_SRCH_VLID_MASK            (0x01UL)
#define ARLA_SRCH_CTL_SRCH_STDN_MASK            (0x80UL)
#define ARLA_SRCH_RSLT_MACVID_MACADDR_MASK      (0x0000ffffffffffffULL)
#define ARLA_SRCH_RSLT_MACVID_MACADDR_SHIFT     (0ULL)
#define ARLA_SRCH_RSLT_MACVID_VID_MASK          (0x0fff000000000000ULL)
#define ARLA_SRCH_RSLT_MACVID_VID_SHIFT         (48ULL)
#define ARLA_SRCH_RSLT_PORTID_MASK              (0x000001ffULL)
#define ARLA_SRCH_RSLT_PORTID_SHIFT             (0ULL)
#define ARLA_SRCH_RSLT_ARL_CON_MASK             (0x00000600ULL)
#define ARLA_SRCH_RSLT_ARL_CON_SHIFT            (9ULL)
#define ARLA_SRCH_RSLT_ARL_PRI_MASK             (0x00003800ULL)
#define ARLA_SRCH_RSLT_ARL_PRI_SHIFT            (11ULL)
#define ARLA_SRCH_RSLT_ARL_VLID_MASK            (0x00010000ULL)

#define ARLA_VTBL_RW_CLR_MASK                   (0x03UL)
#define ARLA_VTBL_WRITE_CMD                     (0x0UL)
#define ARLA_VTBL_READ_CMD                      (0x1UL)
#define ARLA_VTBL_CLRTBL_CMD                    (0x2UL)
#define ARLA_VTBL_RW_CLR_SHIFT                  (0UL)
#define ARLA_VTBL_STDN_MASK                     (0x80UL)
#define ARLA_VTBL_STDN_SHIFT                    (7UL)
#define ARLA_VTBL_ENTRY_FWD_MAP_MASK            (0x000001ffUL)
#define ARLA_VTBL_ENTRY_FWD_MAP_SHIFT           (0UL)
#define ARLA_VTBL_ENTRY_UNTAG_MAP_MASK          (0x0003fe00UL)
#define ARLA_VTBL_ENTRY_UNTAG_MAP_SHIFT         (9UL)

/* Flow control register */

#define FC_PAGE                                    (0x0AUL)
#define FC_CTRL_MODE_OFFSET                        (0x02UL)
#define FC_CTRL_MODE_FC_MODE_MASK                  (0x01UL)
#define FC_CTRL_MODE_FC_MODE_SHIFT                 (0UL)
#define FC_MODE_GLOBAL                             (0x0UL << FC_CTRL_MODE_FC_MODE_SHIFT)
#define FC_MODE_PORT                               (0x1UL << FC_CTRL_MODE_FC_MODE_SHIFT)

#define FC_CTRL_PORT_OFFSET                        (0x03UL)
#define FC_CTRL_PORT_FC_PORT_SEL_MASK              (0x0fUL)

#define FC_LAN_PAGE                                 (0x0BUL)
#define FC_LAN_TXQ_THD_RSV_Q0_OFFSET                (0x00UL)
#define FC_LAN_TXQ_THD_RSV_TXQ_RSV_THD_MASK         (0x07ffUL)
#define FC_LAN_TXQ_THD_RSV_TXQ_RSV_THD_SHIFT        (0UL)
#define FC_LAN_TXQ_THD_RSV_Q1                       (0x02UL)
#define FC_LAN_TXQ_THD_RSV_Q2                       (0x04UL)
#define FC_LAN_TXQ_THD_RSV_Q3                       (0x06UL)
#define FC_LAN_TXQ_THD_RSV_Q4                       (0x08UL)
#define FC_LAN_TXQ_THD_RSV_Q5                       (0x0aUL)
#define FC_LAN_TXQ_THD_RSV_Q6                       (0x0cUL)
#define FC_LAN_TXQ_THD_RSV_Q7                       (0x0eUL)

/* MIB counter registers */
#define MIB_PORT_0_PAGE                            (0x20UL)
#define MIB_PORT_TX_OCTECTS_OFFSET                 (0x0UL)
#define MIB_PORT_TXDROPPKTS_OFFSET                 (0x08UL)
#define MIB_PORT_TXQPKTQ0_OFFSET                   (0x0cUL)
#define MIB_PORT_TXBROADCASTPKTS_OFFSET            (0x10UL)
#define MIB_PORT_TXMULTICASTPKTS_OFFSET            (0x14UL)
#define MIB_PORT_TXUNICASTPKTS_OFFSET              (0x18UL)
#define MIB_PORT_TXCOLLISIONS_OFFSET               (0x1cUL)
#define MIB_PORT_TXSINGLECOLLISION_OFFSET          (0x20UL)
#define MIB_PORT_TXMULTIPLECOLLISION_OFFSET        (0x24UL)
#define MIB_PORT_TXDEFERREDTRANSMIT_OFFSET         (0x28UL)
#define MIB_PORT_TXLATECOLLISION_OFFSET            (0x2cUL)
#define MIB_PORT_TXEXCESSIVECOLLISION_OFFSET       (0x30UL)
#define MIB_PORT_TXFRAMEINDISC_OFFSET              (0x34UL)
#define MIB_PORT_TXPAUSEPKTS_OFFSET                (0x38UL)
#define MIB_PORT_TXQPKTQ1_OFFSET                   (0x3cUL)
#define MIB_PORT_TXQPKTQ2_OFFSET                   (0x40UL)
#define MIB_PORT_TXQPKTQ3_OFFSET                   (0x44UL)
#define MIB_PORT_TXQPKTQ4_OFFSET                   (0x48UL)
#define MIB_PORT_TXQPKTQ5_OFFSET                   (0x4cUL)
#define MIB_PORT_RXOCTETS_OFFSET                   (0x50UL)
#define MIB_PORT_RXUNDERSIZEPKTS_OFFSET            (0x58UL)
#define MIB_PORT_RXPAUSEPKTS_OFFSET                (0x5cUL)
#define MIB_PORT_RXPKTS64OCTETS_OFFSET             (0x60UL)
#define MIB_PORT_RXPKTS65TO127OCTETS_OFFSET        (0x64UL)
#define MIB_PORT_RXPKTS128TO255OCTETS_OFFSET       (0x68UL)
#define MIB_PORT_RXPKTS256TO511OCTETS_OFFSET       (0x6cUL)
#define MIB_PORT_RXPKTS512TO1023OCTETS_OFFSET      (0x70UL)
#define MIB_PORT_RXPKTS1024TOMAXPKTOCTETS_OFFSET   (0x74UL)
#define MIB_PORT_RXOVERSIZEPKTS_OFFSET             (0x78UL)
#define MIB_PORT_RXJABBERS_OFFSET                  (0x7cUL)
#define MIB_PORT_RXALIGNMENTERRORS_OFFSET          (0x80UL)
#define MIB_PORT_RXFCSERRORS_OFFSET                (0x84UL)
#define MIB_PORT_RXGOODOCTETS_OFFSET               (0x88UL)
#define MIB_PORT_RXDROPPKTS_OFFSET                 (0x90UL)
#define MIB_PORT_RXUNICASTPKTS_OFFSET              (0x94UL)
#define MIB_PORT_RXMULTICASTPKTS_OFFSET            (0x98UL)
#define MIB_PORT_RXBROADCASTPKTS_OFFSET            (0x9cUL)
#define MIB_PORT_RXSACHANGES_OFFSET                (0xa0UL)
#define MIB_PORT_RXFRAGMENTS_OFFSET                (0xa4UL)
#define MIB_PORT_RXJUMBOPKT_OFFSET                 (0xa8UL)
#define MIB_PORT_RXSYMBLERR_OFFSET                 (0xacUL)
#define MIB_PORT_INRANGEERRCOUNT_OFFSET            (0xb0UL)
#define MIB_PORT_OUTRANGEERRCOUNT_OFFSET           (0xb4UL)
#define MIB_PORT_EEE_LPI_EVENT                     (0xb8UL)
#define MIB_PORT_EEE_LPI_DURATION                  (0xbcUL)
#define MIB_PORT_RXDISCARD_OFFSET                  (0xc0UL)
#define MIB_PORT_TXQPKTQ6_OFFSET                   (0xc8UL)
#define MIB_PORT_TXQPKTQ7_OFFSET                   (0xccUL)
#define MIB_PORT_TXPKTS64OCTETS_OFFSET             (0xd0UL)
#define MIB_PORT_TXPKTS65TO127OCTETS_OFFSET        (0xd4UL)
#define MIB_PORT_TXPKTS128TO255OCTETS_OFFSET       (0xd8UL)
#define MIB_PORT_TXPKTS256TO511OCTETS_OFFSET       (0xdcUL)
#define MIB_PORT_TXPKTS512TO1023OCTETS_OFFSET      (0xe0UL)
#define MIB_PORT_TXPKTS1024TOMAXPKTOCTETS_OFFSET   (0xe4UL)

/* VLAN control register */
#define VLAN_CTRL_PAGE                            (0x34UL)
#define VLAN_CTRL0_OFFSET                          (0x00UL)
#define VLAN_CTRL0_CHANGE_1P_VID_INNER_MASK        (0x01UL)
#define VLAN_CTRL0_CHANGE_1P_VID_INNER_SHIFT       (0UL)
#define VLAN_CTRL0_CHANGE_1P_VID_OUTER_MASK        (0x02UL)
#define VLAN_CTRL0_CHANGE_1P_VID_OUTER_SHIFT       (1UL)
#define VLAN_CTRL0_CHANGE_1Q_VID_MASK              (0x08UL)
#define VLAN_CTRL0_CHANGE_1Q_VID_SHIFT             (3UL)
#define VLAN_CTRL0_VLAN_LEARN_MODE_MASK            (0x60UL)
#define VLAN_CTRL0_VLAN_LEARN_MODE_SHIFT           (5UL)
#define VLAN_LEARN_MODE_SVL                        (0UL << VLAN_CTRL0_VLAN_LEARN_MODE_SHIFT)
#define VLAN_LEARN_MODE_IVL                        (3UL << VLAN_CTRL0_VLAN_LEARN_MODE_SHIFT)
#define VLAN_CTRL0_VLAN_EN_MASK                    (0x80UL)
#define VLAN_CTRL0_VLAN_EN_SHIFT                   (7UL)

#define VLAN_CTRL3_OFFSET                          (0x03UL)
#define VLAN_CTRL3_EN_DROP_NON1Q_MASK              (0x01ffUL)
#define VLAN_CTRL3_EN_DROP_NON1Q_SHIFT             (0UL)

#define VLAN_CTRL4_OFFSET                          (0x05UL)
#define VLAN_CTRL4_INGR_VID_CHK_MASK               (0x0C0UL)
#define VLAN_CTRL4_INGR_VID_CHK_SHIFT              (6UL)
#define VLAN_CTRL4_INGR_VID_CHK_VID_VIO            (0x1UL)

#define VLAN_DEFAULT_1Q_TAG_PORT0_OFFSET          (0x10UL)
#define DEFAULT_1Q_TAG_VID_0_MASK                 (0x0fffUL)
#define DEFAULT_1Q_TAG_VID_0_SHIFT                (0UL)
#define DEFAULT_1Q_TAG_CFI_0_MASK                 (0x1000UL)
#define DEFAULT_1Q_TAG_CFI_0_SHIFT                (12UL)
#define DEFAULT_1Q_TAG_PRI_0_MASK                 (0xe000UL)
#define DEFAULT_1Q_TAG_PRI_0_SHIFT                (13UL)

/* QoS registers */
#define QOS_PAGE                                   (0x30UL)
#define QOS_1P_EN_OFFSET                           (0x04UL)

#define PN_PCP2TC_DEI0_PORT0_OFFSET                (0x10UL)
#define PN_PCP2TC_DEI0_TAG_PRI_MAP_MASK            (0x7UL)
#define PN_PCP2TC_DEI0_TAG_PRI_MAP_SHIFT           (3UL)
#define PN_PCP2TC_DEI0_TAG000_PRI_MAP_MASK         (0x00000007UL)
#define PN_PCP2TC_DEI0_TAG000_PRI_MAP_SHIFT        (0UL)
#define PN_PCP2TC_DEI0_TAG001_PRI_MAP_MASK         (0x00000038UL)
#define PN_PCP2TC_DEI0_TAG001_PRI_MAP_SHIFT        (3UL)
#define PN_PCP2TC_DEI0_TAG010_PRI_MAP_MASK         (0x000001c0UL)
#define PN_PCP2TC_DEI0_TAG010_PRI_MAP_SHIFT        (6UL)
#define PN_PCP2TC_DEI0_TAG011_PRI_MAP_MASK         (0x00000e00UL)
#define PN_PCP2TC_DEI0_TAG011_PRI_MAP_SHIFT        (9UL)
#define PN_PCP2TC_DEI0_TAG100_PRI_MAP_MASK         (0x00007000UL)
#define PN_PCP2TC_DEI0_TAG100_PRI_MAP_SHIFT        (12UL)
#define PN_PCP2TC_DEI0_TAG101_PRI_MAP_MASK         (0x00038000UL)
#define PN_PCP2TC_DEI0_TAG101_PRI_MAP_SHIFT        (15UL)
#define PN_PCP2TC_DEI0_TAG110_PRI_MAP_MASK         (0x001c0000UL)
#define PN_PCP2TC_DEI0_TAG110_PRI_MAP_SHIFT        (18UL)
#define PN_PCP2TC_DEI0_TAG111_PRI_MAP_MASK         (0x00e00000UL)
#define PN_PCP2TC_DEI0_TAG111_PRI_MAP_SHIFT        (21UL)

#define PID2TC_OFFSET                              (0x4CUL)
#define PID2TC_PID2TC_MASK                         (0x07ffffffUL)
#define PID2TC_PID2TC_SHIFT                        (0UL)
#define PID2TC_PORT_MASK                           (0x7UL)
#define PID2TC_PORT_SHIFT                          (3UL)

#define TC_SEL_TABLE_PORT0_OFFSET                  (0x54UL)
#define TC_SEL_TABLE_TC_SEL_0_0_MASK               (0x0003UL)
#define TC_SEL_TABLE_TC_SEL_0_0_SHIFT              (0UL)
#define TC_SEL_TABLE_TC_SEL_1_0_MASK               (0x000cUL)
#define TC_SEL_TABLE_TC_SEL_1_0_SHIFT              (2UL)
#define TC_SEL_TABLE_TC_SEL_2_0_MASK               (0x0030UL)
#define TC_SEL_TABLE_TC_SEL_2_0_SHIFT              (4UL)
#define TC_SEL_TABLE_TC_SEL_3_0_MASK               (0x00c0UL)
#define TC_SEL_TABLE_TC_SEL_3_0_SHIFT              (6UL)
#define TC_SEL_TABLE_TC_SEL_4_0_MASK               (0x0300UL)
#define TC_SEL_TABLE_TC_SEL_4_0_SHIFT              (8UL)
#define TC_SEL_TABLE_TC_SEL_5_0_MASK               (0x0c00UL)
#define TC_SEL_TABLE_TC_SEL_5_0_SHIFT              (10UL)
#define TC_SEL_TABLE_TC_SEL_6_0_MASK               (0x3000UL)
#define TC_SEL_TABLE_TC_SEL_6_0_SHIFT              (12UL)
#define TC_SEL_TABLE_TC_SEL_7_0_MASK               (0xc000UL)
#define TC_SEL_TABLE_TC_SEL_7_0_SHIFT              (14UL)
#define TC_SEL_TABLE_TC_SEL_SHIFT                  (2UL)
#define TC_SEL_PID2TC_VAL                          (0x3UL)
#define TC_SEL_DA2TC_VAL                           (0x2UL)
#define TC_SEL_PCP2TC_VAL                          (0x1UL)
#define TC_SEL_DSCP2TC_VAL                         (0x0UL)

#define TC2COS_MAP_PORT0_OFFSET                 (0x70UL)
#define TC2COS_MAP_PORT7_OFFSET                 (0x8CUL)
#define TC2COS_MAP_PORT7_VAL                    (0xFAC688UL)
#define TC2COS_MAP_PRT_TO_QID_MASK              (0x7UL)
#define TC2COS_MAP_PRT_TO_QID_SHIFT             (0x3UL)

#define SCHEDULER_PAGE                                (0x46UL)
#define SCHED_PN_QOS_PRI_CTL_PORT0_OFFSET             (0x00UL)
#define SCHED_PN_QOS_PRI_CTL_SCHEDULER_SELECT_MASK    (0x07UL)
#define SCHED_PN_QOS_PRI_CTL_SCHEDULER_SELECT_SHIFT   (0UL)
#define SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_MASK    (0x08UL)
#define SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_SHIFT    (0x08UL)
#define SCHEL_GRANULARITY_BYTE                        (0UL << SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_SHIFT)
#define SCHEL_GRANULARITY_PACKET                      (1UL << SCHED_PN_QOS_PRI_CTL_WDRR_GRANULARITY_SHIFT)

/* jumbo control registers */
#define JUMBO_FRM_CNTRL_PAGE                       (0x40UL)
#define JUMBO_PORT_MASK_OFFSET                     (0x01UL)
#define JUMBO_PORT_MASK_JUMBO_FM_PORT_MASK         (0x000001ffUL)
#define JUMBO_PORT_MASK_EN_10_100_JUMBO_MASK       (0x01000000UL)
#define JUMBO_MIB_GD_FM_MAX_SIZE_OFFSET            (0x05UL)
#define JUMBO_MIB_GD_FM_MAX_SIZE_MAX_SIZE_MASK     (0x3fffUL)
#define JUMBO_MIB_GD_FM_MAX_SIZE                   (0x05EEUL)

/* Egress shaper registers */
#define SHAPER_Q0_CONFIG_PAGE                    (0x48UL)
#define SHAPER_PN_QUEUE0_MAX_REFRESH_PORT0_OFFSET       (0x00UL)
#define SHAPER_QUEUE_MAX_REFRESH_MAX_REFRESH_0_MASK    (0x0003ffffUL)
#define SHAPER_QUEUE_MAX_REFRESH_MAX_REFRESH_0_SHIFT   (0UL)
#define SHAPER_PN_QUEUE0_MAX_THD_SEL_PORT0_OFFSET      (0x30UL)
#define SHAPER_QUEUE_AVB_SHAPING_MODE_OFFSET           (0xE4UL)
#define SHAPER_QUEUE_SHAPER_ENABLE_OFFSET              (0xe6UL)
#define SHAPER_QUEUE_SHAPER_BUCKET_COUNT_SELECT_OFFSET (0xe8UL)

/* CFP TCAM registers */
#define CFP_TCAM_PAGE                                  (0xA0UL)

#define CFP_ACC_OFFSET                                 (0x00UL)
#define CFP_ACC_OP_STR_DONE_MASK                       (0x01UL)
#define CFP_ACC_OP_STR_DONE_SHIFT                      (0UL)
#define CFP_ACC_OP_SEL_MASK                            (0x0EUL)
#define CFP_ACC_OP_SEL_SHIFT                           (1UL)
#define CFP_ACC_OP_SEL_NOOP                            (0UL)
#define CFP_ACC_OP_SEL_RD                              (1UL)
#define CFP_ACC_OP_SEL_WR                              (2UL)
#define CFP_ACC_OP_SEL_SRCH                            (4UL)
#define CFP_ACC_RAM_CLR_MASK                           (0x10UL)
#define CFP_ACC_RAM_CLR_SHIFT                          (4UL)
#define CFP_ACC_RAM_SEL_MASK                           (0x7C00UL)
#define CFP_ACC_RAM_SEL_SHIFT                          (10UL)
#define CFP_ACC_RAM_SEL_RED_STATS                      (0x18UL)
#define CFP_ACC_RAM_SEL_YELLOW_STATS                   (0x10UL)
#define CFP_ACC_RAM_SEL_GREEN_STATS                    (0x08UL)
#define CFP_ACC_RAM_SEL_RATE_METER                     (0x04UL)
#define CFP_ACC_RAM_SEL_ACT_POL                        (0x02UL)
#define CFP_ACC_RAM_SEL_TCAM                           (0x01UL)
#define CFP_ACC_RAM_SEL_NOOP                           (0x00UL)
#define CFP_ACC_TCAM_RST_MASK                          (0x8000UL)
#define CFP_ACC_TCAM_RST_SHIFT                         (15UL)
#define CFP_ACC_XCESS_ADDR_MASK                        (0xFF0000UL)
#define CFP_ACC_XCESS_ADDR_SHIFT                       (16UL)

#define CFP_RATE_METER_GLBL_CTRL_OFFSET                (0x04UL)
#define CFP_RATE_METER_GLBL_CTRL_PKT_LEN_CORR_MASK     (0x03UL)
#define CFP_RATE_METER_GLBL_CTRL_PKT_LEN_CORR_SHIFT    (0UL)
#define CFP_RATE_METER_GLBL_CTRL_RATE_REF_EN_MASK      (0x04UL)
#define CFP_RATE_METER_GLBL_CTRL_RATE_REF_EN_SHIFT     (2UL)

#define CFP_DATA0_OFFSET                               (0x10UL)
#define CFP_DATA0_VALID                                (0x3UL)
#define CFP_DATA0_SLICEID_MASK                         (0xCUL)
#define CFP_DATA0_SLICEID_SHIFT                        (2UL)
#define CFP_DATA0_UDF0_MASK                            (0xFFFF00UL)
#define CFP_DATA0_UDF0_SHIFT                           (8UL)
#define CFP_DATA0_UDF1_MASK                            (0xFF000000UL)
#define CFP_DATA0_UDF1_SHIFT                           (24UL)

#define CFP_MASK0_OFFSET                               (0x30UL)

#define CFP_DATA1_OFFSET                               (0x14UL)
#define CFP_DATA1_UDF1_MASK                            (0xFFUL)
#define CFP_DATA1_UDF1_SHIFT                           (0UL)
#define CFP_DATA1_UDF2_MASK                            (0xFFFF00UL)
#define CFP_DATA1_UDF2_SHIFT                           (8UL)
#define CFP_DATA1_UDF3_MASK                            (0xFF000000UL)
#define CFP_DATA1_UDF3_SHIFT                           (24UL)

#define CFP_MASK1_OFFSET                               (0x34UL)

#define CFP_DATA2_OFFSET                               (0x18UL)
#define CFP_DATA2_UDF3_MASK                            (0xFFUL)
#define CFP_DATA2_UDF3_SHIFT                           (0UL)
#define CFP_DATA2_UDF4_MASK                            (0xFFFF00UL)
#define CFP_DATA2_UDF4_SHIFT                           (8UL)
#define CFP_DATA2_UDF5_MASK                            (0xFF000000UL)
#define CFP_DATA2_UDF5_SHIFT                           (24UL)

#define CFP_MASK2_OFFSET                               (0x38UL)

#define CFP_DATA3_OFFSET                               (0x1CUL)
#define CFP_DATA3_UDF5_MASK                            (0xFFUL)
#define CFP_DATA3_UDF5_SHIFT                           (0UL)
#define CFP_DATA3_UDF6_MASK                            (0xFFFF00UL)
#define CFP_DATA3_UDF6_SHIFT                           (8UL)
#define CFP_DATA3_UDF7_MASK                            (0xFF000000UL)
#define CFP_DATA3_UDF7_SHIFT                           (24UL)

#define CFP_MASK3_OFFSET                               (0x3CUL)

#define CFP_DATA4_OFFSET                               (0x20UL)
#define CFP_DATA4_UDF7_MASK                            (0xFFUL)
#define CFP_DATA4_UDF7_SHIFT                           (0UL)
#define CFP_DATA4_UDF8_MASK                            (0xFFFF00UL)
#define CFP_DATA4_UDF8_SHIFT                           (8UL)
#define CFP_DATA4_CTAG_MASK                            (0xFF000000UL)
#define CFP_DATA4_CTAG_SHIFT                           (24UL)

#define CFP_MASK4_OFFSET                               (0x40UL)

#define CFP_DATA5_OFFSET                               (0x24UL)
#define CFP_DATA5_CTAG_MASK                            (0xFFUL)
#define CFP_DATA5_CTAG_SHIFT                           (0UL)
#define CFP_DATA5_STAG_MASK                            (0xFFFF00UL)
#define CFP_DATA5_STAG_SHIFT                           (8UL)
#define CFP_DATA5_UDFVALID_7_0_MASK                    (0xFF000000UL)
#define CFP_DATA5_UDFVALID_7_0_SHIFT                   (24UL)

#define CFP_MASK5_OFFSET                               (0x44UL)

#define CFP_DATA6_OFFSET                               (0x28UL)
#define CFP_DATA6_UDFVALID_8_MASK                      (0x1UL)
#define CFP_DATA6_UDFVALID_8_SHIFT                     (0UL)
#define CFP_DATA6_ETHTYPE_MASK                         (0xFFFF00UL)
#define CFP_DATA6_ETHTYPE_SHIFT                        (8UL)
#define CFP_DATA6_L3FRAMING_MASK                       (0x3000000UL)
#define CFP_DATA6_L3FRAMING_SHIFT                      (24UL)
#define CFP_DATA6_L2FRAMING_MASK                       (0xC000000UL)
#define CFP_DATA6_L2FRAMING_SHIFT                      (26UL)
#define CFP_DATA6_CTAGSTATUS_MASK                      (0x30000000UL)
#define CFP_DATA6_CTAGSTATUS_SHIFT                     (28UL)
#define CFP_DATA6_STAGSTATUS_MASK                      (0xC0000000UL)
#define CFP_DATA6_STAGSTATUS_SHIFT                     (30UL)
#define CFP_DATA6_TTL_SHIFT                            (3UL)

#define CFP_MASK6_OFFSET                               (0x48UL)

#define CFP_DATA7_OFFSET                               (0x2CUL)
#define CFP_DATA7_SRCPRTMAP_MASK                       (0x1FFUL)
#define CFP_DATA7_SRCPRTMAP_SHIFT                      (0UL)

#define CFP_MASK7_OFFSET                               (0x4CUL)

/* CFP Action Policy registers */
#define CFP_ACT_POL_DATA0_OFFSET                       (0x50UL)
#define CFP_ACT_POL_DATA0_BYPASS_MASK                  (0x7UL)
#define CFP_ACT_POL_DATA0_BYPASS_SHIFT                 (0UL)
#define CFP_ACT_POL_DATA0_REASONCODE_MASK              (0x1F8UL)
#define CFP_ACT_POL_DATA0_REASONCODE_SHIFT             (3UL)
#define CFP_ACT_POL_DATA0_LPBKEN_MASK                  (0x200UL)
#define CFP_ACT_POL_DATA0_LPBKEN_SHIFT                 (9UL)
#define CFP_ACT_POL_DATA0_NEWTC_MASK                   (0x1C00UL)
#define CFP_ACT_POL_DATA0_NEWTC_SHIFT                  (10UL)
#define CFP_ACT_POL_DATA0_TC_MASK                      (0x3C00UL)
#define CFP_ACT_POL_DATA0_CHANGETC_MASK                (0x2000UL)
#define CFP_ACT_POL_DATA0_CHANGETC_SHIFT               (13UL)
#define CFP_ACT_POL_DATA0_DSTIB_MASK                   (0xFFC000UL)
#define CFP_ACT_POL_DATA0_DSTIB_SHIFT                  (14UL)
#define CFP_ACT_POL_DATA0_IB_MASK                      (0x3FFC000UL)
#define CFP_ACT_POL_DATA0_CHANGEMAPIB_MASK             (0x3000000UL)
#define CFP_ACT_POL_DATA0_CHANGEMAPIB_SHIFT            (24UL)
#define CFP_ACT_POL_DATA0_NEWDSCPIB_MASK               (0xFC000000UL)
#define CFP_ACT_POL_DATA0_NEWDSCPIB_SHIFT              (26UL)

#define CFP_ACT_POL_DATA1_OFFSET                       (0x54UL)
#define CFP_ACT_POL_DATA1_CHANGEDSCPIB_MASK            (0x1UL)
#define CFP_ACT_POL_DATA1_CHANGEDSCPIB_SHIFT           (0UL)
#define CFP_ACT_POL_DATA1_DSTOB_MASK                   (0x7FEUL)
#define CFP_ACT_POL_DATA1_DSTOB_SHIFT                  (1UL)
#define CFP_ACT_POL_DATA1_CHANGEMAPOB_MASK             (0x1800UL)
#define CFP_ACT_POL_DATA1_CHANGEMAPOB_SHIFT            (11UL)
#define CFP_ACT_POL_DATA1_OB_MASK                      (0x1FFEUL)
#define CFP_ACT_POL_DATA1_NEWDSCPOB_MASK               (0x7E000UL)
#define CFP_ACT_POL_DATA1_NEWDSCPOB_SHIFT              (13UL)
#define CFP_ACT_POL_DATA1_CHANGEDSCPOB_MASK            (0x80000UL)
#define CFP_ACT_POL_DATA1_CHANGEDSCPOB_SHIFT           (19UL)
#define CFP_ACT_POL_DATA1_TOS_MASK                     (0xFE000UL)
#define CFP_ACT_POL_DATA1_CHAINID_MASK                 (0xFF00000UL)
#define CFP_ACT_POL_DATA1_CHAINID_SHIFT                (20UL)
#define CFP_ACT_POL_DATA1_CHANGECOLOR_MASK             (0x10000000UL)
#define CFP_ACT_POL_DATA1_CHANGECOLOR_SHIFT            (28UL)
#define CFP_ACT_POL_DATA1_NEWCOLOR_MASK                (0x60000000UL)
#define CFP_ACT_POL_DATA1_NEWCOLOR_SHIFT               (29UL)
#define CFP_ACT_POL_DATA1_COLOR_MASK                   (0x70000000UL)
#define CFP_ACT_POL_DATA1_REDDFLT_MASK                 (0x80000000UL)
#define CFP_ACT_POL_DATA1_REDDFLT_SHIFT                (31UL)

#define CFP_ACT_POL_DATA2_OFFSET                       (0x58UL)

/* CFP Rate Meter registers */
#define CFP_RATE_METER0_OFFSET                         (0x60UL)
#define CFP_RATE_METER0_CM_MASK                        (0x1UL)
#define CFP_RATE_METER0_CM_SHIFT                       (0UL)
#define CFP_RATE_METER0_POLICERACTION_MASK             (0x2UL)
#define CFP_RATE_METER0_POLICERACTION_SHIFT            (1UL)
#define CFP_RATE_METER0_CF_MASK                        (0x4UL)
#define CFP_RATE_METER0_CF_SHIFT                       (2UL)
#define CFP_RATE_METER0_POLICERMODE_MASK               (0x18UL)
#define CFP_RATE_METER0_POLICERMODE_SHIFT              (3UL)
#define CFP_RATE_METER0_MASK                           (0x1FUL)

#define CFP_RATE_METER1_OFFSET                         (0x64UL)
#define CFP_RATE_METER1_EIRTKBKT_MASK                  (0x7FFFFFUL)
#define CFP_RATE_METER1_EIRTKBKT_SHIFT                 (0UL)

#define CFP_RATE_METER2_OFFSET                         (0x68UL)
#define CFP_RATE_METER2_EIRBKTSZ_MASK                  (0xFFFFFUL)
#define CFP_RATE_METER2_EIRBKTSZ_SHIFT                 (0UL)

#define CFP_RATE_METER3_OFFSET                         (0x6CUL)
#define CFP_RATE_METER3_EIRRATE_MASK                   (0x7FFFFUL)
#define CFP_RATE_METER3_EIRRATE_SHIFT                  (0UL)

#define CFP_RATE_METER4_OFFSET                         (0x70UL)
#define CFP_RATE_METER4_CIRTKBKT_MASK                  (0x7FFFFFUL)
#define CFP_RATE_METER4_CIRTKBKT_SHIFT                 (0UL)

#define CFP_RATE_METER5_OFFSET                         (0x74UL)
#define CFP_RATE_METER5_CIRBKTSZ_MASK                  (0xFFFFFUL)
#define CFP_RATE_METER5_CIRBKTSZ_SHIFT                 (0UL)

#define CFP_RATE_METER6_OFFSET                         (0x78UL)
#define CFP_RATE_METER6_CIRRATE_MASK                   (0x7FFFFUL)
#define CFP_RATE_METER6_CIRRATE_SHIFT                  (0UL)

/* CFP Statistics registers */
#define CFP_STAT_GREEN_CNTR_OFFSET                     (0x80UL)
#define CFP_STAT_YELLOW_CNTR_OFFSET                    (0x84UL)
#define CFP_STAT_RED_CNTR_OFFSET                       (0x88UL)

/* CFP CTRL registers */
#define CFP_CTRL_PAGE                                  (0xA1UL)
#define CFP_CTL_OFFSET                                 (0x00UL)
#define CFP_CTL_CFP_EN_MAP_MASK                        (0x1FFU)

#define CFP_CTL_UDF_0_A_0_OFFSET                       (0x10UL)
#define CFP_CTL_UDF_BASE_MASK                          (0x7UL)
#define CFP_CTL_UDF_BASE_SHIFT                         (5UL)
#define CFP_CTL_UDF_OFFSET_MASK                        (0x1FUL)
#define CFP_CTL_UDF_OFFSET_SHIFT                       (0UL)


/* Traffic remarking registers */
#define TRREG_PAGE                                     (0x91UL)
#define TRREG_CTRL0_OFFSET                             (0x00UL)
#define TRREG_CTRL0_PCP_RMK_EN_SHIFT                   (16UL)
#define TRREG_PN_EGRESS_PKT_TC2PCP_MAP_OFFSET          (0x10UL)
#define TC2PCP_MAP_PORT0_OFFSET                        (0x10UL)
#define TC2PCP_MAP_MASK                                (0x7UL)
#define TC2PCP_MAP_SHIFT                               (4UL)

/* AVB LED registers */
#define AVB_PAGE                                       (0x90UL)
#define AVB_EAV_LNK_STATUS_OFFSET                      (0xB0UL)
#define AVB_EAV_LNK_STATUS_PT_MASK                     (0x3FUL)

/* BRCM ingress tag size (in bytes) */
#define INGRESS_MGMT_INFO_SIZE                          (4UL)
#define INGRESS_MGMT_INFO_OPCODE1                       (0x20000000UL)
#define INGRESS_MGMT_INFO_OPCODE3                       (0x60000000UL)
#define INGRESS_MGMT_INFO_TC_SHIFT                      (26UL)
#define INGRESS_MGMT_INFO_TE_SHIFT                      (24UL)
/* BRCM egress tag size (in bytes) */
#define EGRESS_MGMT_INFO_SIZE                          (4UL)
#define EGRESS_MGMT_INFO_SRC_PID_MASK                  (0x1FU)

typedef volatile struct {
    uint16_t    CTRL_SER_L16;
    uint16_t    CTRL_SER_H16;
    uint16_t    ADDR_SER_L16;
    uint16_t    ADDR_SER_H16;
    uint16_t    DATA_SER_L_L16;
    uint16_t    DATA_SER_L_H16;
    uint16_t    DATA_SER_H_L16;
    uint16_t    DATA_SER_H_H16;
    uint16_t    CTRL_CPU_L16;
#define CTRL_CPU_L16_SIZE_MASK              (0x0003U)
#define CTRL_CPU_L16_SIZE_SHIFT             (0U)
#define TRANS_SZ_BYTE                       (0U)
#define TRANS_SZ_WORD                       (1U)
#define TRANS_SZ_DWORD                      (2U)
#define TRANS_SZ_QWORD                      (3U)

#define CTRL_CPU_L16_RDB_WR_MASK            (0x0004U)
#define CTRL_CPU_L16_RDB_WR_SHIFT           (2U)
#define CPU_READ                            (0U << CTRL_CPU_L16_RDB_WR_SHIFT)
#define CPU_WRITE                           (1U << CTRL_CPU_L16_RDB_WR_SHIFT)

#define CTRL_CPU_L16_AUTO_INCR_MASK         (0x0008U)
#define CTRL_CPU_L16_AUTO_INCR_SHIFT        (3U)
#define CTRL_CPU_L16_COMMIT_MASK            (0x0010U)
#define CTRL_CPU_L16_COMMIT_SHIFT           (4U)
#define CTRL_CPU_L16_COMMIT_ON_RDWR_MASK    (0x0020U)
#define CTRL_CPU_L16_COMMIT_ON_RDWR_SHIFT   (5U)
#define CTRL_CPU_L16_DONE_MASK              (0x0080U)
#define CTRL_CPU_L16_DONE_SHIFT             (7U)
    uint16_t    RSVD;
    uint16_t    ADDR_CPU_L16;
#define ADDR_CPU_ADDRESS_MASK               (0xffffU)
    uint16_t    ADDR_CPU_H16;
    uint16_t    DATA_CPU_L_L16;
#define DATA_CPU_DATA_MASK                  (0xffffU)
    uint16_t    DATA_CPU_L_H16;
    uint16_t    DATA_CPU_H_L16;
    uint16_t    DATA_CPU_H_H16;
} IND_ACC_RegsType;

