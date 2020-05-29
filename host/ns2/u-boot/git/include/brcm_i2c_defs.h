/*
 * $Copyright Open Broadcom Corporation$
 */

#ifndef __IPROC_SMBUS_DEFS_H__
#define __IPROC_SMBUS_DEFS_H__

/* Transaction error codes defined in Master command register (0x30) */
#define MSTR_STS_XACT_SUCCESS          0
#define MSTR_STS_LOST_ARB              1
#define MSTR_STS_NACK_FIRST_BYTE       2
 /* NACK on a byte other than the first byte */
#define MSTR_STS_NACK_NON_FIRST_BYTE   3

#define MSTR_STS_TTIMEOUT_EXCEEDED     4
#define MSTR_STS_TX_TLOW_MEXT_EXCEEDED 5
#define MSTR_STS_RX_TLOW_MEXT_EXCEEDED 6

/* SMBUS protocol values defined in register 0x30 */
#define SMBUS_PROT_QUICK_CMD               0
#define SMBUS_PROT_SEND_BYTE               1
#define SMBUS_PROT_RECV_BYTE               2
#define SMBUS_PROT_WR_BYTE                 3
#define SMBUS_PROT_RD_BYTE                 4
#define SMBUS_PROT_WR_WORD                 5
#define SMBUS_PROT_RD_WORD                 6
#define SMBUS_PROT_BLK_WR                  7
#define SMBUS_PROT_BLK_RD                  8
#define SMBUS_PROT_PROC_CALL               9
#define SMBUS_PROT_BLK_WR_BLK_RD_PROC_CALL 10

#define BUS_BUSY_COUNT	100000	/* Number can be changed later */

#endif /* __IPROC_SMBUS_DEFS_H__ */
