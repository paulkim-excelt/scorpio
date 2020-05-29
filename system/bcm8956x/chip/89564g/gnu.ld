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
INCLUDE chip/common/gnu.ld
INCLUDE app_gnu.ld

/* NOTE: Any changes made here must be reflected in the MPU table as well */
main_stack_size = 0x1000;
irq_stack_size = 0x800;
init_thread_stack_size = 0x100;
code_ro_region_start = 0x00008000;
code_ro_region_size = 0x00050000;       /* 320KB */
privileged_region_start = code_ro_region_start + code_ro_region_size;
privileged_region_size = 0x00018000;    /* 96KB */
data_region_start = 0x20000000;
data_region_size = 0x0003C000;          /* 240KB */
ipc_region_start = 0x2003C000;
ipc_region_size = 0x00004000;

/* Define output sections */
SECTIONS
{
   . = ORIGIN(ITCM);
   .text BLOCK(code_ro_region_start):
    {
        __text_start__ = .;
        . = ALIGN(1024);
        KEEP(*(.vector_tbl))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __version_data_start__ = .;
        KEEP(*(.version*))
        __version_data_end__ = .;
        __text_end__ = .;
        size_check = (__text_end__  <= code_ro_region_size);
        ASSERT(size_check, "***LINKER ERROR: Code(RO) Section overflows the limit.");
    } > ITCM
}

SECTIONS
{
    .data.privileged BLOCK(privileged_region_start):
    {
        __stack_start__ = .;
        . = ALIGN(8);
        . = . + main_stack_size;
        __main_stack__ = .;
        *(.stack.16k.*)
        *(.stack.8k.*)
        *(.stack.4k.*)
        *(.stack.2k.*)
        *(.stack.1k.*)
        *(.stack.512.*)
        *(.stack.256.*)
        . = . + init_thread_stack_size;
        __init_thread_stack__ = .;
        __stack_end__ = .;
        __data_os_start__ = .;
        *(.data.os.*)
        __data_os_end__ = .;
        __data_drivers_start__ = .;
        *(.data.drivers*)
        __data_drivers_end__ = .;
        size_check = (__data_drivers_end__ <= privileged_region_size);
        ASSERT(size_check, "***LINKER ERROR: .data.privileged section overflows the limit.");
    } > ITCM
}

SECTIONS
{
    .data BLOCK(data_region_start):
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
        size_check = (__data_end__ <= data_region_size) ;
        ASSERT(size_check , "***LINKER ERROR: Data Section overflows the limit.");
    } > DTCM

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        *(*bcm.test*)
        __bss_end__ = .;
    } > DTCM

    .ipc BLOCK(ipc_region_start) (NOLOAD):
    {
        __ipc_start__ = .;
        *(.ipc*)
        __ipc_end__ = .;
        size_check = (__ipc_end__ <= ipc_region_size) ;
        ASSERT(size_check , "***LINKER ERROR: IPC Section overflows the limit.");
    } > IPC

    . = ORIGIN(SCRATCHMEM);
    .eth_ro :
    {
        __eth_ro_start__ = .;
        *(.ethernet.rx0desctbl)
        __eth_ro_end__ = .;
    } > SCRATCHMEM

    .eth_rw (NOLOAD):
    {
        __eth_rw_start__ = .;
        . = ALIGN(256);
        *(.ethernet.tx0desctbl)
        *(.ethernet.tx0pktbuff)
        *(.ethernet.tx1desctbl)
        *(.ethernet.tx1pktbuff)
        *(.ethernet.tx2desctbl)
        *(.ethernet.tx2pktbuff)
        *(.ethernet.tx3desctbl)
        *(.ethernet.tx3pktbuff)
        *(.ethernet.rx0desctbl)
        *(.ethernet.rx0pktbuff)
        __eth_rw_end__ = .;
    } > SCRATCHMEM
}
