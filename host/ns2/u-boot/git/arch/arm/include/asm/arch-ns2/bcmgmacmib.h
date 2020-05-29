/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef	_bcmgmacmib_h_
#define	_bcmgmacmib_h_

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/* GMAC MIB structure */

struct gmacmib {
	u32 tx_good_octets;		/* 0x300 */
	u32 tx_good_octets_high;	/* 0x304 */
	u32 tx_good_pkts;		/* 0x308 */
	u32 tx_octets;			/* 0x30c */
	u32 tx_octets_high;		/* 0x310 */
	u32 tx_pkts;			/* 0x314 */
	u32 tx_broadcast_pkts;		/* 0x318 */
	u32 tx_multicast_pkts;		/* 0x31c */
	u32 tx_len_64;			/* 0x320 */
	u32 tx_len_65_to_127;		/* 0x324 */
	u32 tx_len_128_to_255;		/* 0x328 */
	u32 tx_len_256_to_511;		/* 0x32c */
	u32 tx_len_512_to_1023;		/* 0x330 */
	u32 tx_len_1024_to_1522;	/* 0x334 */
	u32 tx_len_1523_to_2047;	/* 0x338 */
	u32 tx_len_2048_to_4095;	/* 0x33c */
	u32 tx_len_4095_to_8191;	/* 0x340 */
	u32 tx_len_8192_to_max;		/* 0x344 */
	u32 tx_jabber_pkts;		/* 0x348 */
	u32 tx_oversize_pkts;		/* 0x34c */
	u32 tx_fragment_pkts;		/* 0x350 */
	u32 tx_underruns;		/* 0x354 */
	u32 tx_total_cols;		/* 0x358 */
	u32 tx_single_cols;		/* 0x35c */
	u32 tx_multiple_cols;		/* 0x360 */
	u32 tx_excessive_cols;		/* 0x364 */
	u32 tx_late_cols;		/* 0x368 */
	u32 tx_defered;			/* 0x36c */
	u32 tx_carrier_lost;		/* 0x370 */
	u32 tx_pause_pkts;		/* 0x374 */
	u32 tx_uni_pkts;		/* 0x378 */
	u32 tx_q0_pkts;			/* 0x37c */
	u32 tx_q0_octets;		/* 0x380 */
	u32 tx_q0_octets_high;		/* 0x384 */
	u32 tx_q1_pkts;			/* 0x388 */
	u32 tx_q1_octets;		/* 0x38c */
	u32 tx_q1_octets_high;		/* 0x390 */
	u32 tx_q2_pkts;			/* 0x394 */
	u32 tx_q2_octets;		/* 0x398 */
	u32 tx_q2_octets_high;		/* 0x39c */
	u32 tx_q3_pkts;			/* 0x3a0 */
	u32 tx_q3_octets;		/* 0x3a4 */
	u32 tx_q3_octets_high;		/* 0x3a8 */
	u32 PAD;			/* 0x3ac */
	u32 rx_good_octets;		/* 0x3b0 */
	u32 rx_good_octets_high;	/* 0x3b4 */
	u32 rx_good_pkts;		/* 0x3b8 */
	u32 rx_octets;			/* 0x3bc */
	u32 rx_octets_high;		/* 0x3c0 */
	u32 rx_pkts;			/* 0x3c4 */
	u32 rx_broadcast_pkts;		/* 0x3c8 */
	u32 rx_multicast_pkts;		/* 0x3cc */
	u32 rx_len_64;			/* 0x3d0 */
	u32 rx_len_65_to_127;		/* 0x3d4 */
	u32 rx_len_128_to_255;		/* 0x3d8 */
	u32 rx_len_256_to_511;		/* 0x3dc */
	u32 rx_len_512_to_1023;		/* 0x3e0 */
	u32 rx_len_1024_to_1522;	/* 0x3e4 */
	u32 rx_len_1523_to_2047;	/* 0x3e8 */
	u32 rx_len_2048_to_4095;	/* 0x3ec */
	u32 rx_len_4095_to_8191;	/* 0x3f0 */
	u32 rx_len_8192_to_max;		/* 0x3f4 */
	u32 rx_jabber_pkts;		/* 0x3f8 */
	u32 rx_oversize_pkts;		/* 0x3fc */
	u32 rx_fragment_pkts;		/* 0x400 */
	u32 rx_missed_pkts;		/* 0x404 */
	u32 rx_crc_align_errs;		/* 0x408 */
	u32 rx_undersize;		/* 0x40c */
	u32 rx_crc_errs;		/* 0x410 */
	u32 rx_align_errs;		/* 0x414 */
	u32 rx_symbol_errs;		/* 0x418 */
	u32 rx_pause_pkts;		/* 0x41c */
	u32 rx_nonpause_pkts;		/* 0x420 */
	u32 rx_sachanges;		/* 0x424 */
	u32 rx_uni_pkts;		/* 0x428 */
};

#define	GM_MIB_BASE		0x300
/*#define	GM_MIB_LIMIT	0x800*/
#define	GM_MIB_LIMIT	0x42c

#endif	/* _bcmgmacmib_h_ */
