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
/**
    @file wdt_rdb.h
    @brief RDB File for WDT

    @version 2019Jan21_rdb
*/

#ifndef WDT_RDB_H
#define WDT_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t WDT_WDOGLOAD_TYPE;
#define WDT_WDOGLOAD_WDOGLOAD_MASK (0xffffffffUL)
#define WDT_WDOGLOAD_WDOGLOAD_SHIFT (0UL)




typedef uint32_t WDT_WDOGVALUE_TYPE;
#define WDT_WDOGVALUE_WDOGVALUE_MASK (0xffffffffUL)
#define WDT_WDOGVALUE_WDOGVALUE_SHIFT (0UL)




typedef uint32_t WDT_WDOGCONTROL_TYPE;
#define WDT_WDOGCONTROL_RESEN_MASK (0x2UL)
#define WDT_WDOGCONTROL_RESEN_SHIFT (1UL)
#define WDT_WDOGCONTROL_INTEN_MASK (0x1UL)
#define WDT_WDOGCONTROL_INTEN_SHIFT (0UL)




typedef uint32_t WDT_WDOGINTCLR_TYPE;
#define WDT_WDOGINTCLR_WDOGINTCLR_MASK (0x1UL)
#define WDT_WDOGINTCLR_WDOGINTCLR_SHIFT (0UL)




typedef uint32_t WDT_WDOGRIS_TYPE;
#define WDT_WDOGRIS_WDOGRIS_MASK (0x1UL)
#define WDT_WDOGRIS_WDOGRIS_SHIFT (0UL)




typedef uint32_t WDT_WDOGMIS_TYPE;
#define WDT_WDOGMIS_WDOGMIS_MASK (0x1UL)
#define WDT_WDOGMIS_WDOGMIS_SHIFT (0UL)




typedef uint8_t WDT_RESERVED_TYPE;




typedef uint32_t WDT_WDOGLOCK_TYPE;
#define WDT_WDOGLOCK_WDOGLOCK_MASK (0xffffffffUL)
#define WDT_WDOGLOCK_WDOGLOCK_SHIFT (0UL)




typedef uint32_t WDT_WDOGITCR_TYPE;
#define WDT_WDOGITCR_ITEN_MASK (0x1UL)
#define WDT_WDOGITCR_ITEN_SHIFT (0UL)




typedef uint32_t WDT_WDOGITOP_TYPE;
#define WDT_WDOGITOP_WDOGINT_MASK (0x2UL)
#define WDT_WDOGITOP_WDOGINT_SHIFT (1UL)
#define WDT_WDOGITOP_WDOGRES_MASK (0x1UL)
#define WDT_WDOGITOP_WDOGRES_SHIFT (0UL)




typedef uint32_t WDT_WDOGPERIPHID0_TYPE;
#define WDT_WDOGPERIPHID0_PARTNUMBER0_MASK (0xffUL)
#define WDT_WDOGPERIPHID0_PARTNUMBER0_SHIFT (0UL)




typedef uint32_t WDT_WDOGPERIPHID1_TYPE;
#define WDT_WDOGPERIPHID1_DESIGNER0_MASK (0xf0UL)
#define WDT_WDOGPERIPHID1_DESIGNER0_SHIFT (4UL)
#define WDT_WDOGPERIPHID1_PARTNUMBER1_MASK (0xfUL)
#define WDT_WDOGPERIPHID1_PARTNUMBER1_SHIFT (0UL)




typedef uint32_t WDT_WDOGPERIPHID2_TYPE;
#define WDT_WDOGPERIPHID2_REVISION_MASK (0xf0UL)
#define WDT_WDOGPERIPHID2_REVISION_SHIFT (4UL)
#define WDT_WDOGPERIPHID2_DESIGNER1_MASK (0xfUL)
#define WDT_WDOGPERIPHID2_DESIGNER1_SHIFT (0UL)




typedef uint32_t WDT_WDOGPERIPHID3_TYPE;
#define WDT_WDOGPERIPHID3_CONFIGURATION_MASK (0xffUL)
#define WDT_WDOGPERIPHID3_CONFIGURATION_SHIFT (0UL)




typedef uint32_t WDT_WDOGPCELLID0_TYPE;
#define WDT_WDOGPCELLID0_WDOGPCELLID0_MASK (0xffUL)
#define WDT_WDOGPCELLID0_WDOGPCELLID0_SHIFT (0UL)




typedef uint32_t WDT_WDOGPCELLID1_TYPE;
#define WDT_WDOGPCELLID1_WDOGPCELLID1_MASK (0xffUL)
#define WDT_WDOGPCELLID1_WDOGPCELLID1_SHIFT (0UL)




typedef uint32_t WDT_WDOGPCELLID2_TYPE;
#define WDT_WDOGPCELLID2_WDOGPCELLID2_MASK (0xffUL)
#define WDT_WDOGPCELLID2_WDOGPCELLID2_SHIFT (0UL)




typedef uint32_t WDT_WDOGPCELLID3_TYPE;
#define WDT_WDOGPCELLID3_WDOGPCELLID3_MASK (0xffUL)
#define WDT_WDOGPCELLID3_WDOGPCELLID3_SHIFT (0UL)




typedef volatile struct COMP_PACKED _WDT_RDBType {
    WDT_WDOGLOAD_TYPE wdogload; /* OFFSET: 0x0 */
    WDT_WDOGVALUE_TYPE wdogvalue; /* OFFSET: 0x4 */
    WDT_WDOGCONTROL_TYPE wdogcontrol; /* OFFSET: 0x8 */
    WDT_WDOGINTCLR_TYPE wdogintclr; /* OFFSET: 0xc */
    WDT_WDOGRIS_TYPE wdogris; /* OFFSET: 0x10 */
    WDT_WDOGMIS_TYPE wdogmis; /* OFFSET: 0x14 */
    WDT_RESERVED_TYPE rsvd0[3048]; /* OFFSET: 0x18 */
    WDT_WDOGLOCK_TYPE wdoglock; /* OFFSET: 0xc00 */
    WDT_RESERVED_TYPE rsvd1[764]; /* OFFSET: 0xc04 */
    WDT_WDOGITCR_TYPE wdogitcr; /* OFFSET: 0xf00 */
    WDT_WDOGITOP_TYPE wdogitop; /* OFFSET: 0xf04 */
    WDT_RESERVED_TYPE rsvd2[216]; /* OFFSET: 0xf08 */
    WDT_WDOGPERIPHID0_TYPE wdogperiphid0; /* OFFSET: 0xfe0 */
    WDT_WDOGPERIPHID1_TYPE wdogperiphid1; /* OFFSET: 0xfe4 */
    WDT_WDOGPERIPHID2_TYPE wdogperiphid2; /* OFFSET: 0xfe8 */
    WDT_WDOGPERIPHID3_TYPE wdogperiphid3; /* OFFSET: 0xfec */
    WDT_WDOGPCELLID0_TYPE wdogpcellid0; /* OFFSET: 0xff0 */
    WDT_WDOGPCELLID1_TYPE wdogpcellid1; /* OFFSET: 0xff4 */
    WDT_WDOGPCELLID2_TYPE wdogpcellid2; /* OFFSET: 0xff8 */
    WDT_WDOGPCELLID3_TYPE wdogpcellid3; /* OFFSET: 0xffc */
} WDT_RDBType;


#define WDT_BASE                        (0x40145000UL)

#define M3_WDT_BASE                     (0x4C015000UL)



#define WDT_MAX_HW_ID                   (2UL)


#define WDT0_BASE                       (WDT_BASE)


#define WDT_CLOCK                       (6250000)


#define WDT_NUM_HW_ID                   (1UL)


#define WDT_REG_UNLOCK                  (0x1ACCE551UL)


#define WDT_REG_LOCK                    (0x0UL)

#endif /* WDT_RDB_H */
