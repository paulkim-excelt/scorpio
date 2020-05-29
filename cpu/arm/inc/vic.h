/*****************************************************************************
Copyright 2017 Broadcom Limited.  All rights reserved.
This program is the proprietary software of Broadcom Limited and/or its
licensors, and may only be used, duplicated, modified or distributed pursuant
to the terms and conditions of a separate, written license agreement executed
between you and Broadcom (an "Authorized License").

Except as set forth in an Authorized License, Broadcom grants no
license(express or implied), right to use, or waiver of any kind with respect
to the Software, and Broadcom expressly reserves all rights in and to the
Software and all intellectual property rights therein.  IF YOU HAVE NO
AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,
1. This program, including its structure, sequence and organization,
constitutes the valuable trade secrets of Broadcom, and you shall use all
reasonable efforts to protect the confidentiality thereof, and to use this
information only in connection with your use of Broadcom integrated circuit
products.

2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND
WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE,
MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
PERFORMANCE OF THE SOFTWARE.

3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
ANY LIMITED REMEDY.
***************************************************************************/
#ifndef VIC_H
#define VIC_H

#include <stdint.h>

typedef void (*IRQ_Handler)(void);

typedef uint32_t VIC_IntrType;
#define VIC_INTR_TYPE_IRQ    (0UL)
#define VIC_INTR_TYPE_FIQ    (1UL)

typedef volatile struct {
    uint32_t    VICIRQSTATUS;
    uint32_t    VICFIQSTATUS;
    uint32_t    VICRAWINTR;
    uint32_t    VICINTSELECT;
    uint32_t    VICINTENABLE;
    uint32_t    VICINTENCLEAR;
    uint32_t    VICSOFTINT;
    uint32_t    VICSOFTINTCLEAR;
    uint32_t    VICPROTECTION;
    uint32_t    VICSWPRIORITYMASK;
    uint32_t    VICSWPRIORITYDAISY;
    uint32_t    RSVD[53];
    uint32_t    VICVECTADDR[32UL];
    uint32_t    RSVD1[32];
    uint32_t    VICVECTPRIORITY[32UL];
    uint32_t    RSVD2[800];
    uint32_t    VICADDRESS;
    uint32_t    RSVD3[55];
    uint32_t    VICPERIPHID0;
    uint32_t    VICPERIPHID1;
    uint32_t    VICPERIPHID2;
    uint32_t    VICPERIPHID3;
    uint32_t    VICPCELLID0;
    uint32_t    VICPCELLID1;
    uint32_t    VICPCELLID2;
    uint32_t    VICPCELLID3;
} VIC_RegsType;

void VIC_SetIRQAddr(uint32_t aIRQNum, uint32_t aIRQAddr);
void VIC_SetPriority(uint32_t aIRQNum, uint32_t aPrio);
void VIC_IRQEnable(uint32_t aIRQNum);
void VIC_IRQDisable(uint32_t aIRQNum);
void VIC_IRQDisableAll(void);
uint32_t VIC_GetActIRQ(void);
void VIC_ClrActIRQ(void);
void VIC_SetIntrType(uint32_t aIRQNum, VIC_IntrType aIntrType);

#endif /* VIC_H */
