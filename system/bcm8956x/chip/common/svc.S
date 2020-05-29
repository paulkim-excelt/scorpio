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
    .syntax unified
    .arch armv7-m
    .cpu cortex-m7
    .code 16
    .text
    .thumb
/*=====================================================
	Globals
=======================================================*/

    .global SysCmdReq_x80
    .global SysCmdReq_x81
    .global SysCmdReq_x82
    .global SysCmdReq_x83
    .global SysCmdReq_x84
    .global SysCmdReq_x85
    .global SysCmdReq_x86
    .global SysCmdReq_x87
    .global SysCmdReq_x88
    .global SysCmdReq_x89
    .global SysCmdReq_x8A
    .global SysCmdReq_x8B
    .global SysCmdReq_x8C
    .global SysCmdReq_x8D
    .global SysCmdReq_x8E
    .global SysCmdReq_x8F
    .global SysCmdReq_x90
    .global SysCmdReq_x91
    .global SysCmdReq_x92
    .global SysCmdReq_x93
    .global SysCmdReq_x94
    .global SysCmdReq_x95
    .global SysCmdReq_x96
    .global SysCmdReq_x97
    .global SysCmdReq_x98
    .global SysCmdReq_x99
    .global SysCmdReq_x9A
    .global SysCmdReq_x9B
    .global SysCmdReq_x9C
    .global SysCmdReq_x9D
    .global SysCmdReq_x9E
    .global SysCmdReq_x9F
    .global SysCmdReq_xA0
    .global SysCmdReq_xA1
    .global SysCmdReq_xA2
    .global SysCmdReq_xA3
    .global SysCmdReq_xA4
    .global SysCmdReq_xA5
    .global SysCmdReq_xA6
    .global SysCmdReq_xA7
    .global SysCmdReq_xA8
    .global SysCmdReq_xA9
    .global SysCmdReq_xAA
    .global SysCmdReq_xAB
    .global SysCmdReq_xAC
    .global SysCmdReq_xAD
    .global SysCmdReq_xAE
    .global SysCmdReq_xAF
    .global SysCmdReq_xB0
    .global SysCmdReq_xB1
    .global SysCmdReq_xB2
    .global SysCmdReq_xB3
    .global SysCmdReq_xB4
    .global SysCmdReq_xB5
    .global SysCmdReq_xB6
    .global SysCmdReq_xB7
    .global SysCmdReq_xB8
    .global SysCmdReq_xB9
    .global SysCmdReq_xBA
    .global SysCmdReq_xBB
    .global SysCmdReq_xBC
    .global SysCmdReq_xBD
    .global SysCmdReq_xBE
    .global SysCmdReq_xBF
    .global SysCmdReq_xC0
    .global SysCmdReq_xC1
    .global SysCmdReq_xC2
    .global SysCmdReq_xC3
    .global SysCmdReq_xC4
    .global SysCmdReq_xC5
    .global SysCmdReq_xC6
    .global SysCmdReq_xC7
    .global SysCmdReq_xC8
    .global SysCmdReq_xC9
    .global SysCmdReq_xCA
    .global SysCmdReq_xCB
    .global SysCmdReq_xCC
    .global SysCmdReq_xCD
    .global SysCmdReq_xCE
    .global SysCmdReq_xCF

SysCmdReq_x80:
    svc #0x80
    bx lr
SysCmdReq_x81:
    svc #0x81
    bx lr
SysCmdReq_x82:
    svc #0x82
    bx lr
SysCmdReq_x83:
    svc #0x83
    bx lr
SysCmdReq_x84:
    svc #0x84
    bx lr
SysCmdReq_x85:
    svc #0x85
    bx lr
SysCmdReq_x86:
    svc #0x86
    bx lr
SysCmdReq_x87:
    svc #0x87
    bx lr
SysCmdReq_x88:
    svc #0x88
    bx lr
SysCmdReq_x89:
    svc #0x89
    bx lr
SysCmdReq_x8A:
    svc #0x8A
    bx lr
SysCmdReq_x8B:
    svc #0x8B
    bx lr
SysCmdReq_x8C:
    svc #0x8C
    bx lr
SysCmdReq_x8D:
    svc #0x8D
    bx lr
SysCmdReq_x8E:
    svc #0x8E
    bx lr
SysCmdReq_x8F:
    svc #0x8F
    bx lr
SysCmdReq_x90:
    svc #0x90
    bx lr
SysCmdReq_x91:
    svc #0x91
    bx lr
SysCmdReq_x92:
    svc #0x92
    bx lr
SysCmdReq_x93:
    svc #0x93
    bx lr
SysCmdReq_x94:
    svc #0x94
    bx lr
SysCmdReq_x95:
    svc #0x95
    bx lr
SysCmdReq_x96:
    svc #0x96
    bx lr
SysCmdReq_x97:
    svc #0x97
    bx lr
SysCmdReq_x98:
    svc #0x98
    bx lr
SysCmdReq_x99:
    svc #0x99
    bx lr
SysCmdReq_x9A:
    svc #0x9A
    bx lr
SysCmdReq_x9B:
    svc #0x9B
    bx lr
SysCmdReq_x9C:
    svc #0x9C
    bx lr
SysCmdReq_x9D:
    svc #0x9D
    bx lr
SysCmdReq_x9E:
    svc #0x9E
    bx lr
SysCmdReq_x9F:
    svc #0x9F
    bx lr
SysCmdReq_xA0:
    svc #0xA0
    bx lr
SysCmdReq_xA1:
    svc #0xA1
    bx lr
SysCmdReq_xA2:
    svc #0xA2
    bx lr
SysCmdReq_xA3:
    svc #0xA3
    bx lr
SysCmdReq_xA4:
    svc #0xA4
    bx lr
SysCmdReq_xA5:
    svc #0xA5
    bx lr
SysCmdReq_xA6:
    svc #0xA6
    bx lr
SysCmdReq_xA7:
    svc #0xA7
    bx lr
SysCmdReq_xA8:
    svc #0xA8
    bx lr
SysCmdReq_xA9:
    svc #0xA9
    bx lr
SysCmdReq_xAA:
    svc #0xAA
    bx lr
SysCmdReq_xAB:
    svc #0xAB
    bx lr
SysCmdReq_xAC:
    svc #0xAC
    bx lr
SysCmdReq_xAD:
    svc #0xAD
    bx lr
SysCmdReq_xAE:
    svc #0xAE
    bx lr
SysCmdReq_xAF:
    svc #0xAF
    bx lr
SysCmdReq_xB0:
    svc #0xB0
    bx lr
SysCmdReq_xB1:
    svc #0xB1
    bx lr
SysCmdReq_xB2:
    svc #0xB2
    bx lr
SysCmdReq_xB3:
    svc #0xB3
    bx lr
SysCmdReq_xB4:
    svc #0xB4
    bx lr
SysCmdReq_xB5:
    svc #0xB5
    bx lr
SysCmdReq_xB6:
    svc #0xB6
    bx lr
SysCmdReq_xB7:
    svc #0xB7
    bx lr
SysCmdReq_xB8:
    svc #0xB8
    bx lr
SysCmdReq_xB9:
    svc #0xB9
    bx lr
SysCmdReq_xBA:
    svc #0xBA
    bx lr
SysCmdReq_xBB:
    svc #0xBB
    bx lr
SysCmdReq_xBC:
    svc #0xBC
    bx lr
SysCmdReq_xBD:
    svc #0xBD
    bx lr
SysCmdReq_xBE:
    svc #0xBE
    bx lr
SysCmdReq_xBF:
    svc #0xBF
    bx lr
SysCmdReq_xC0:
    svc #0xC0
    bx lr
SysCmdReq_xC1:
    svc #0xC1
    bx lr
SysCmdReq_xC2:
    svc #0xC2
    bx lr
SysCmdReq_xC3:
    svc #0xC3
    bx lr
SysCmdReq_xC4:
    svc #0xC4
    bx lr
SysCmdReq_xC5:
    svc #0xC5
    bx lr
SysCmdReq_xC6:
    svc #0xC6
    bx lr
SysCmdReq_xC7:
    svc #0xC7
    bx lr
SysCmdReq_xC8:
    svc #0xC8
    bx lr
SysCmdReq_xC9:
    svc #0xC9
    bx lr
SysCmdReq_xCA:
    svc #0xCA
    bx lr
SysCmdReq_xCB:
    svc #0xCB
    bx lr
SysCmdReq_xCC:
    svc #0xCC
    bx lr
SysCmdReq_xCD:
    svc #0xCD
    bx lr
SysCmdReq_xCE:
    svc #0xCE
    bx lr
SysCmdReq_xCF:
    svc #0xCF
    bx lr
