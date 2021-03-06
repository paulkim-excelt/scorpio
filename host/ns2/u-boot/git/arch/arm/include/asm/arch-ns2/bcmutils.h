/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _bcm_utils_h_
#define _bcm_utils_h_

#include <malloc.h>
#include <stdbool.h>
#include <config.h>
#include <common.h>

#define TRUE	true
#define FALSE	false

#define uint8	uint8_t
#define uint16	uint16_t
#define uint32	uint32_t

#define int8	int8_t
#define int16	int16_t
#define int32	int32_t

/* general purpose memory allocation */
#define	MALLOC(osh, size)	malloc((size))
#define	MFREE(osh, addr, size)	free((addr))
#define	MALLOCED(osh)		(0)
#define	BZERO_SM(r, len)	memset((r), 0, (len))

/* host/bus architecture-specific address byte swap */
#define BUS_SWAP32(v)		(v)
#define ltoh16(i) (i)

#define	ROUNDUP(x, y)		((((x) + ((y) - 1)) / (y)) * (y))

#define osl_t void
#define si_t void

/*
 * Spin at most 'us' microseconds while 'exp' is true.
 * Caller should explicitly test 'exp' when this completes
 * and take appropriate error action if 'exp' is still true.
 */
#define SPINWAIT(exp, us) { \
	uint countdown = (us) + 9; \
	while ((exp) && (countdown >= 10)) {\
		udelay(10); \
		countdown -= 10; \
	} \
}

#endif	/* _bcm_utils_h_ */
