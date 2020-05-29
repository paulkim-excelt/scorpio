/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _ETHHW_H
#define _ETHHW_H

#ifdef __cplusplus
extern "C"
{
#endif


#define ETHHW_MAC_0		0
#define ETHHW_MAC_1		1
#define ETHHW_MAC_2		2
#define ETHHW_MAC_3		3

#define ETHHW_PORT_0             0
#define ETHHW_PORT_1             1
#define ETHHW_PORT_INT           8
#define ETHHW_PORT_SNAPSHOT      0x51
#define ETHHW_PORT_ALL           -1

#define ETHHW_PORT_MIN           ETHHW_PORT_0
#define ETHHW_PORT_MAX           ETHHW_PORT_1
#define ETHHW_PORT_NUM           (ETHHW_PORT_MAX - ETHHW_PORT_MIN + 1)
#define ETHHW_PORT_MASK(p)       (1 << (p))

#define ETHHW_PORTMASK_NONE      0
#define ETHHW_PORTMASK_PORT_0    ETHHW_PORT_MASK(ETHHW_PORT_0)
#define ETHHW_PORTMASK_PORT_1    ETHHW_PORT_MASK(ETHHW_PORT_1)
#define ETHHW_PORTMASK_INT       ETHHW_PORT_MASK(ETHHW_PORT_INT)
#define ETHHW_PORTMASK_EXT       (ETHHW_PORT_MASK(ETHHW_PORT_0) | \
					ETHHW_PORT_MASK(ETHHW_PORT_1))
#define ETHHW_PORTMASK_ALL       (ETHHW_PORTMASK_INT | ETHHW_PORTMASK_EXT)

#define ETHHW_BRCM_HDR_LEN       sizeof(u32)

typedef enum {
	ETHHW_STP_STATE_NONE = 0,
	ETHHW_STP_STATE_DISABLED,
	ETHHW_STP_STATE_BLOCKING,
	ETHHW_STP_STATE_LISTENING,
	ETHHW_STP_STATE_LEARNING,
	ETHHW_STP_STATE_FORWARDING,
} ETHHW_STP_STATE;

typedef enum {
	ETHHW_TC_0 = 0,
	ETHHW_TC_1 = 1,
	ETHHW_TC_2 = 2,
	ETHHW_TC_3 = 3,
	ETHHW_TC_4 = 4,
	ETHHW_TC_5 = 5,
	ETHHW_TC_6 = 6,
	ETHHW_TC_7 = 7,
} ETHHW_TC;

#define ETHHW_TC_LOWEST          ETHHW_TC_0
#define ETHHW_TC_HIGHEST         ETHHW_TC_7
#define ETHHW_TC_MASK            0x7

typedef enum {
	ETHHW_TE_FOLLOW = 0,
	ETHHW_TE_UNTAG = 1,
	ETHHW_TE_KEEP = 2,
	ETHHW_TE_RSVD = 3,
} ETHHW_TE;

#define ETHHW_TE_MASK            0x3

#define ETHHW_MII_FLAGS_EXT      0x80000000     /* External PHY */
#define ETHHW_MII_FLAGS_SERDES   0x40000000     /* SERDES PHY */
typedef u32 ETHHW_MII_FLAGS;

/* Note that this assumes the buffers are 16-bit aligned */
#define ethHw_macCopy(dp, sp) \
	{ \
		(((u16 *)(dp))[0] = ((u16 *)(sp))[0]); \
		(((u16 *)(dp))[1] = ((u16 *)(sp))[1]); \
		(((u16 *)(dp))[2] = ((u16 *)(sp))[2]); \
	}

#define ethHw_macIsSame(m1p, m2p) \
	((((u16 *)(m1p))[0] == ((u16 *)(m2p))[0]) && \
	(((u16 *)(m1p))[1] == ((u16 *)(m2p))[1]) && \
	(((u16 *)(m1p))[2] == ((u16 *)(m2p))[2]))

#define ETHHW_VALID_PORT_EXT(port)	(((port) == ETHHW_PORT_0) ||	\
						((port) == ETHHW_PORT_1))
#define ETHHW_VALID_PORT_INT(port)           ((port) == ETHHW_PORT_INT)
#define ETHHW_VALID_PORT_ALL(port)           ((port) == ETHHW_PORT_ALL)
#define ETHHW_VALID_PORT(port)               (ETHHW_VALID_PORT_EXT((port)) || \
						ETHHW_VALID_PORT_INT((port)))
#define ETHHW_VALID_PORT_MIB(port)           (ETHHW_VALID_PORT((port)) ||      \
						((port) == ETHHW_PORT_SNAPSHOT))
#define ETHHW_VALID_PORTMAP(p)			\
				(((p) & ~(ETHHW_PORT_MASK(ETHHW_PORT_0) |   \
				ETHHW_PORT_MASK(ETHHW_PORT_1) |		    \
				ETHHW_PORT_MASK(ETHHW_PORT_INT))) == 0)
#define ETHHW_VALID_STP_STATE(state)		\
				(((state) >= ETHHW_STP_STATE_DISABLED) &&    \
				((state) <= ETHHW_STP_STATE_FORWARDING))

#define ETHHW_VAR_UNINITIALIZED              -1

/* Define result codes to maintain backward compatibility */
typedef enum {
	ETHHW_RC_NONE       = 0,
	ETHHW_RC_PARAM      = -4,
	ETHHW_RC_FULL       = -6,
	ETHHW_RC_TIMEOUT    = -9,
	ETHHW_RC_UNAVAIL    = -16,
	ETHHW_RC_PORT       = -18
} ETHHW_RC;

#define ETHHW_RC_SUCCESS(rc)        ((rc) >= 0)
#define ETHHW_RC_FAILURE(rc)        ((rc) < 0)

#define ETHHW_BRCM_TYPE             0x8874
#define ETHHW_BRCM_TYPE_MSB         ((ETHHW_BRCM_TYPE >> 8) & 0xff)
#define ETHHW_BRCM_TYPE_LSB         ((ETHHW_BRCM_TYPE >> 0) & 0xff)

/* Broadcom header/tag macros */
#define ethHw_brcmTagInit(tagp, tc, te) \
{ \
	(tagp)[0] = (((tc) & ETHHW_TC_MASK) << 2) | ((te) & ETHHW_TE_MASK); \
	(tagp)[1] = 0; \
	(tagp)[2] = 0; \
	(tagp)[3] = 0; \
}

#define ethHw_brcmTagInitRaw(tagp) \
{ \
	tagp[0] = tagp[2]; \
	tagp[1] = 0; \
	tagp[2] = 0; \
	tagp[3] = 0; \
}

#define ethHw_brcmTagSetPortMask(tagp, portMask) \
{ \
	(tagp)[0] |= 0x20; \
	(tagp)[3] = ((portMask) & ETHHW_PORTMASK_EXT); \
}

#define ethHw_brcmTagGetPort(tagp)     ((tagp)[3] & 0x1)

int ethHw_Init(void);
int ethHw_Exit(void);

int ethHw_arlEntrySet(struct eth_device *dev);

int ethHw_macEnableSet(int port, int en);
int ethHw_macEnableGet(int port, int *txp, int *rxp);

int ethHw_dmaTx(size_t len, u8 *tx_buf);
int ethHw_dmaTxWait(void);
int ethHw_dmaRx(void);
int ethHw_dmaDisable(int dir);
int ethHw_dmaEnable(int dir);

u64 ethHw_mibTxOctets(int port);
u32 ethHw_mibTxDropPkts(int port);
u32 ethHw_mibTxQ0Pkt(int port);
u32 ethHw_mibTxBroadcastPkts(int port);
u32 ethHw_mibTxMulticastPkts(int port);
u32 ethHw_mibTxUnicastPkts(int port);
u32 ethHw_mibTxCollisions(int port);
u32 ethHw_mibTxSingleCollision(int port);
u32 ethHw_mibTxMultipleCollision(int port);
u32 ethHw_mibTxDeferredTransmit(int port);
u32 ethHw_mibTxLateCollision(int port);
u32 ethHw_mibTxExcessiveCollision(int port);
u32 ethHw_mibTxFrameInDisc(int port);
u32 ethHw_mibTxPausePkts(int port);
u32 ethHw_mibTxQ1Pkt(int port);
u32 ethHw_mibTxQ2Pkt(int port);
u32 ethHw_mibTxQ3Pkt(int port);
u32 ethHw_mibTxQ4Pkt(int port);
u32 ethHw_mibTxQ5Pkt(int port);
u64 ethHw_mibRxOctets(int port);
u32 ethHw_mibRxUndersizePkts(int port);
u32 ethHw_mibRxPausePkts(int port);
u32 ethHw_mibPkts64Octets(int port);
u32 ethHw_mibPkts65To127Octets(int port);
u32 ethHw_mibPkts128To255Octets(int port);
u32 ethHw_mibPkts256To511Octets(int port);
u32 ethHw_mibPkts512To1023Octets(int port);
u32 ethHw_mibPkts1024ToMaxPktOctets(int port);
u32 ethHw_mibRxOversizePkts(int port);
u32 ethHw_mibRxJabbers(int port);
u32 ethHw_mibRxAlignmentErrors(int port);
u32 ethHw_mibRxFcsErrors(int port);
u64 ethHw_mibRxGoodOctets(int port);
u32 ethHw_mibRxDropPkts(int port);
u32 ethHw_mibRxUnicastPkts(int port);
u32 ethHw_mibRxMulticastPkts(int port);
u32 ethHw_mibRxBroadcastPkts(int port);
u32 ethHw_mibRxSaChanges(int port);
u32 ethHw_mibRxFragments(int port);
u32 ethHw_mibJumboPktCount(int port);
u32 ethHw_mibRxSymbolError(int port);
u32 ethHw_mibInRangeErrorCount(int port);
u32 ethHw_mibOutRangeErrorCount(int port);
u32 ethHw_mibRxDiscard(int port);
u32 ethHw_mibDosAssertCount(int port);
u32 ethHw_mibDosDropCount(int port);
u32 ethHw_mibIngressRateAssertCount(int port);
u32 ethHw_mibIngressRateDropCount(int port);
u32 ethHw_mibEgressRateAssertCount(int port);

static inline int ethHw_rxFrameFormat(char **bufpp, int *lenp)
{
	char *srcp;
	char *dstp;
	int i;

	 /*
	  *  *bufp in:
	  *     byte  0  5 6 11 12      15 16  19
	  *           [DA] [SA] [BRCM_HDR] [TYPE]
	  *
	  *  *bufp out (Broadcom header removed):
	  *     byte  0  5 6 11 12  15
	  *           [DA] [SA] [TYPE]
	  *
	  *  Broadcom header contains source port information
	  */

	srcp = *bufpp + 11;
	dstp = srcp + ETHHW_BRCM_HDR_LEN;

	/* Update length to remove Broadcom header and ethernet FCS */
	*lenp -= (ETHHW_BRCM_HDR_LEN + sizeof(u32));

	 /* Update start of buffer */
	*bufpp += ETHHW_BRCM_HDR_LEN;

	/* Move DA and SA */
	for (i = 0; i < 12; i++)
		*dstp-- = *srcp--;

	return ETHHW_BRCM_HDR_LEN;
}


static inline int ethHw_rxFrameFormatRaw(char **bufpp, int *lenp)
{
	char *bufp;
	u8 *tagp;

	/*
	 *  *bufp in:
	 *     byte  0  5 6 11 12      15 16  19
	 *           [DA] [SA] [BRCM_HDR] [TYPE]
	 *
	 *  *bufp out (Broadcom header removed):
	 *     byte  0  5 6 11 12      15 16  19
	 *           [DA] [SA] [RAW_HDR] [TYPE]
	 *
	 *  Raw socket requires ingress specific information, so format
	 *  slightly to add a custom L2 protocol type
	 *
	 *  RAW_HDR[0]:  ETHHW_BRCM_TYPE_MSB
	 *  RAW_HDR[1]:  ETHHW_BRCM_TYPE_LSB
	 *  RAW_HDR[2]:  0
	 *  RAW_HDR[3]:  port
	 *
	 *  Broadcom header contains source port information
	 */

	bufp = *bufpp;
	tagp = (u8 *)&bufp[12];
	tagp[0] = ETHHW_BRCM_TYPE_MSB;
	tagp[1] = ETHHW_BRCM_TYPE_LSB;
	tagp[2] = 0;
	tagp[3] = tagp[3];

	/* Update length to remove ethernet FCS */
	*lenp -= sizeof(u32);

	return 0;
}


static inline int ethHw_txFrameFormat(char **bufpp, int *lenp, int portMask,
				      ETHHW_TC tc, ETHHW_TE te)
{
	char *bufp;
	char *srcp;
	char *dstp;
	u8 *tagp;
	register int i;
	int offset;

	/*
	 *  *bufp out (Broadcom header inserted):
	 * byte  0  5 6 11 12      15 16  19
	 *	[DA] [SA] [BRCM_HDR] [TYPE]
	 *
	 *  Broadcom header contains destination port information
	 */

	bufp = *bufpp;

	if ((bufp[12] == ETHHW_BRCM_TYPE_MSB) &&
			(bufp[13] == ETHHW_BRCM_TYPE_LSB)) {
	/*
	 *  *bufp in:
	 *     byte  0  5 6 11 12     15 16  19
	 *           [DA] [SA] [RAW_HDR] [TYPE]
	 *
	 *  Custom L2 protocol type detected, so raw socket has inserted egress
	 *  specific information, but type needs to be formatted slightly for
	 *  ASIC.  The portMask, tc, and te arguments will be overridden since
	 *  the RAW_HDR already contains this information
	 *
	 *  RAW_HDR[0]:  ETHHW_BRCM_TYPE_MSB
	 *  RAW_HDR[1]:  ETHHW_BRCM_TYPE_LSB
	 *  RAW_HDR[2]:  tc[4:2], te[1:0]
	 *  RAW_HDR[3]:  portMask
	 */
		tagp = (u8 *)&bufp[12];
		portMask = (int)tagp[3];
#if 1
	/*
	 *  Optimized tag initialization (tc and te do not need to be decoded,
	 *  then encoded since the bit positions are aligned and only require a
	 *  byte shift)
	 */
		 ethHw_brcmTagInitRaw(tagp);
#else
		 tc = (int)((tagp[2] >> 2) & ETHHW_TC_MASK);
		 te = (int)(tagp[2] & ETHHW_TE_MASK);
		 ethHw_brcmTagInit(tagp, tc, te);
#endif
		if (portMask != 0)
			ethHw_brcmTagSetPortMask(tagp, portMask);

		/* Update length to include FCS */
		*lenp += sizeof(u32);
		offset = 0;
	} else {
		/*
		 *  *bufp in:
		 * byte 0  5 6 11 12  15
		 *	[DA] [SA] [TYPE]
		 */

		srcp = bufp;
		dstp = srcp - ETHHW_BRCM_HDR_LEN;

		/* Move DA and SA */
		for (i = 0; i < 12; i++)
			*dstp++ = *srcp++;

		tagp = (u8 *)dstp;

		/* Set Broadcom tag */
		ethHw_brcmTagInit(tagp, tc, te);
		if (portMask >= 0)
			ethHw_brcmTagSetPortMask(tagp, portMask);

		/* Update length to include Broadcom header and FCS */
		*lenp += (ETHHW_BRCM_HDR_LEN + sizeof(u32));

		/* Update start of buffer */
		*bufpp -= ETHHW_BRCM_HDR_LEN;
		offset = ETHHW_BRCM_HDR_LEN;
	}

	return offset;
}


#ifdef __cplusplus
 /* end extern "C" */
}
#endif

#endif /* _ETHHW_H */
