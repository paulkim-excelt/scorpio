/* ###*B*###
; ERIKA Enterprise - a tiny RTOS for small microcontrollers
;
; Copyright (C) 2002-2011  Evidence Srl
;
; This file is part of ERIKA Enterprise.
;
; ERIKA Enterprise is free software; you can redistribute it
; and/or modify it under the terms of the GNU General Public License
; version 2 as published by the Free Software Foundation,
; (with a special exception described below).
;
; Linking this code statically or dynamically with other modules is
; making a combined work based on this code.  Thus, the terms and
; conditions of the GNU General Public License cover the whole
; combination.
;
; As a special exception, the copyright holders of this library give you
; permission to link this code with independent modules to produce an
; executable, regardless of the license terms of these independent
; modules, and to copy and distribute the resulting executable under
; terms of your choice, provided that you also meet, for each linked
; independent module, the terms and conditions of the license of that
; module.  An independent module is a module which is not derived from
; or based on this library.  If you modify this code, you may extend
; this exception to your version of the code, but you are not
; obligated to do so.  If you do not wish to do so, delete this
; exception statement from your version.
;
; ERIKA Enterprise is distributed in the hope that it will be
; useful, but WITHOUT ANY WARRANTY; without even the implied warranty
; of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License version 2 for more details.
;
; You should have received a copy of the GNU General Public License
; version 2 along with ERIKA Enterprise; if not, write to the
; Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
; Boston, MA 02110-1301 USA.
; ###*E*###

; /**
; 	@file ee_keil_oo.S
; 	@brief Functions to save and restore registers for OSEK TerminateTask().
; 	@author Gianluca Franchino
; 	@author Giuseppe Serano
; 	@author Alessandro Biondi
; 	@date 2013
*/
@*******************************************************************************
@                         PUBLIC FUNCTIONS
@*******************************************************************************
	@ Functions declared in this file
	.global	EE_hal_terminate_savestk	@ void EE_hal_terminate_savestk(EE_TID tid)
	.global	EE_hal_terminate_task		@ void EE_hal_terminate_task(EE_TID tid);

	.global	EE_terminate_real_th_body
	.global	EE_terminate_data

@*******************************************************************************
@                              EQUATES
@*******************************************************************************
#define	EPSR_T_BIT_VAL 0x01000000	@ Value to set the T-bit in EPSR (always Thumb mode)


@*******************************************************************************
@                              CODE SECTION
@*******************************************************************************

	.text

@ kernel code is in ARM-mode
	.syntax unified
	.arch armv7e-m
#ifdef __CORTEX_M4__
	.cpu cortex-m4
#endif
#ifdef __CORTEX_M7__
	.cpu cortex-m7
#endif


@void EE_hal_terminate_savestk(EE_TID tid);
        .type   EE_hal_terminate_savestk, #function
EE_hal_terminate_savestk:
	MRS	R12, PSP			@ Get the stack pointer
	@ Save all callee-saved registers
	@ R0-R3 and R12 are scratch registers, R13 ->(SP), R14 ->(LR), R15 -> (PC)
	STMFD   R12!, {R4-R7}		@ Store R4, R5, R6, R7 onto stack
	STMFD   R12!, {R8-R11}	    @ Store R8, R9, R10, R11 onto stack
	STMFD   R12!, {LR}			@ Store link register (return address)
	MRS	R3, PSR			        @ Store xPSR to 8-bytes stack aligment
	STMFD   R12!, {R3}
    MSR     PSP, R12            @ update PSP

	@ Save the stack pointer (including space for registers)
	@ R2 == & EE_terminate_data[tid]
	LSLS	R0, R0, #2		@ R0 = tid << 2
	LDR	R2, =EE_terminate_data
	ADD	R2, R2, R0
	STR	R12, [R2]		@ Save stack pointer

    BX LR               @ return to caller

	.size	EE_hal_terminate_savestk, . - EE_hal_terminate_savestk

@*******************************************************************************

@void EE_hal_terminate_task(EE_TID tid)
        .type   EE_hal_terminate_task, #function
EE_hal_terminate_task:
    PUSH {LR}
    PUSH {R12}
    PUSH {R0-R3}
    PUSH {R4-R7}
    PUSH {R8-R11}

	@ R0 == tid

	@ Restore the stack pointer
	@ R1 == & EE_terminate_data[tid]
	LSLS	R0, R0, #2		@ R0 = tid << 2
	LDR	R1, =EE_terminate_data	@ R1 == & EE_terminate_data[tid]
	ADD	R1, R1, R0
	LDR	R2, [R1]

    MOV R12, R2             @ R12 = SP

	LDMIA R12!, {R2}			@ Get xPSR from stack
	LDR	R0, =EPSR_T_BIT_VAL	@ R0 = 0x01000000
	ORRS	R2, R2, R0		@ R2 = (xPSR OR 0x01000000). This guarantees that Thumbs bit is set
					@ to avoid an hard_fault exception
	MSR	XPSR_NZCVQ, R2		@ Restore xPSR register
	LDMIA R12!, {R3}			@ Get link register from stack and ignore it
	LDMIA R12!, {R8-R11}    @ Restore R8, R9, R10, R11 from stack
	LDMIA R12!, {R4-R7}	    @ Restore R4, R5, R6, R7 from stack

	MSR	PSP, R12                @ PSP = R12

	BLX	EE_thread_end_instance  @ Call EE_thread_end_instance

    LDR R1, =EE_std_endcycle_next_tid   @ R1 = &EE_std_endcycle_next_tid
    LDR R0, [R1]                        @ R0 = EE_std_endcycle_next_tid

    @ EE_cortex_mx_change_context(EE_std_endcycle_next_tid)
    BL EE_cortex_mx_change_context

    POP {R8-R11}
    POP {R4-R7}
    POP {R0-R3}
    POP {R12}
    POP {PC}

	.size	EE_hal_terminate_task, . - EE_hal_terminate_task


@******************************************************************************
@
@ Tell the assembler that we're done.
@
@******************************************************************************
	.end
