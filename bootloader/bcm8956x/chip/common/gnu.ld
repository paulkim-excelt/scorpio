/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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

GROUP(libc.a libg.a libnosys.a libm.a)

MEMORY
{
  TCM (xw)      : ORIGIN = 0x00008000, LENGTH = 480K
  IPC (rw)      : ORIGIN = 0x2003c000, LENGTH = 16K
}

main_stack_size = 0x2000; /* 5KB */
bl_sz = 0x8000;/* code + RO */
bss_size = 0x3000;
ipc_mem_start = ORIGIN(IPC);
ipc_size = LENGTH(IPC);
scratch_size = 0x1000;
text_start = (LENGTH(TCM) - bl_sz) + ORIGIN(TCM);
rsa_signature_size = 0x100;

/* Define output sections */
SECTIONS
{
    . = ORIGIN(TCM);
    /* Untill the last 4K(4K for scratch, 32k for code, data, bss and stack) */
    .download (NOLOAD):
    {
        BL_DWNLD_StartAddr = .;
        . += (LENGTH(TCM) - scratch_size - bl_sz - main_stack_size - bss_size);
        BL_DWNLD_EndAddr = .;
        BL_Scratch_StartAddr = .;
        . += (scratch_size);
        BL_Scratch_EndAddr = .;
    } > TCM

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        *(.data.drivers*)
        *(.data.err*)
        . = ALIGN(4);
        __bss_end__ = .;
    } > TCM

    .stack (NOLOAD):
    {
        __stack_start__ = .;
        . = ALIGN(8);
        . = . + main_stack_size;
        __main_stack__ = .;
        __stack_end__ = .;
    } > TCM

   .text BLOCK(text_start):
    {
        __text_start__ = .;
        KEEP(*(.reset_func1*))
        KEEP(*(.vector_tbl))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __text_end__ = .;
    } > TCM

    .data :
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
    } > TCM
    bl_bin_size = __data_end__ - __text_start__;

    .ipc BLOCK(ipc_mem_start) (NOLOAD):
    {
        BL_IPC_StartAddr = .;
        *(.ipc*)
        BL_IPC_EndAddr = BL_IPC_StartAddr + ipc_size;
    } > IPC
}
