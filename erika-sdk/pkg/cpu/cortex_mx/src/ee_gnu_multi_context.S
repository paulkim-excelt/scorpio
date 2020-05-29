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
; 	@file ee_keil_multi_context.s
; 	@brief Context switch function for multistack on Cortex MX
; 	Implementation of EE_cortex_mx_change_context as described in
; 	pkg/cpu/common/inc/ee_context.h
; 	@author Gianluca Franchino
; 	@author Giuseppe Serano
; 	@author Alessandro Biondi
; 	@date 2013
*/

@*******************************************************************************
@                         PUBLIC FUNCTIONS
@*******************************************************************************
	.global	EE_cortex_mx_change_context

	.global	EE_std_run_task_code
	.global	EE_std_thread_tos
	.global	EE_cortex_mx_active_tos
	.global	EE_cortex_mx_system_tos

	.global	EE_terminate_real_th_body
    .global BCM_ThreadNotTerminated

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
	.fpu fpv5-sp-d16
#endif

#define _EE_cortex_mx_cust_run_task_code_addr EE_cortex_mx_cust_run_task_code

#define	EPSR_T_BIT_VAL	0x01000000	@ Value to set the T-bit in EPSR
					@ (always Thumb mode)
#define	EXC_RETURN		0xFFFFFFFD	@ No-FPU, Thread-Mode, PSP.
#define	EXC_RETURN_FPU	0xFFFFFFED	@ FPU, Thread-Mode, PSP.

@Pseudo code for EE_cortex_mx_change_context():
@  begin:
@    tos_index = EE_std_thread_tos[tid+1];
@    if is_not_the_current_stack(tos_index) {
@      save_caller_saved_registers();
@      switch_stacks(tos_index);
@      restore_caller_saved_registers();
@    }
@    if (is_not_marked_stacked(tid)) {
@      tid = EE_std_run_task_code(tid);
@    }
@

@uint32_t EE_cortex_mx_change_context(EE_TID tid);
        .type   EE_cortex_mx_change_context, #function
EE_cortex_mx_change_context:
	@ R0 == tid
	@ RET == Portion of EXC_RETURN to be used during exception return
	@ tos_index = EE_std_thread_tos[tid+1];
	ADDS	R2, R0, #1		@ R2 = tid+1
	LSLS	R2, R2, #2		@ R2 = (tid+1)*4= correct offset in  EE_std_thread_tos
							@ The last shift, also gets rid of the stacked mark
	LDR	R3, =EE_std_thread_tos
	ADD	R2, R3, R2
	LDR	R2, [R2]			@ R2 == tos_index

	@*
	@* if is_not_the_current_stack(tos_index) {
	@*
	LDR	R3, =EE_cortex_mx_active_tos
	LDR	R3, [R3]						@ R3 = EE_cortex_mx_active_tos;
	CMP	R2, R3
	BEQ	end_change_stacks

    @ although PSP has been accounted for saving registers in svc_isr, registers
    @ are pushed only during exit. Hence R4-R11 need to be saved in case they
    @ are being used.
    PUSH {R4}
	@ switch_stacks(tos_index);
	LDR	R4, =EE_cortex_mx_system_tos	@ R4 = & cortex_mx_system_tos[0];
	@ EE_cortex_mx_system_tos[R3] = SP;
	LSLS	R3, R3, #2
	ADD	R3, R4, R3
	@ Note STR can only use the register range -> R0 to R4.
    MRS R12, PSP
	STR	R12, [R3]		@ save stack pointer

	@ EE_cortex_mx_active_tos = tos_index;
	LDR R3, =EE_cortex_mx_active_tos
	STR	R2, [R3]
	@ SP= EE_cortex_mx_system_tos[R2];
	LSLS	R2, R2, #2
	ADD	R2, R4, R2
	LDR	R12, [R2]
	MSR     PSP, R12
    POP {R4}

#ifdef ENABLE_THREAD_PROTECTION
    PUSH {R0}
    PUSH {LR}
#ifdef ENABLE_FPU
    VLDM R12, {s0}          @ dummy load to trigger lazy push to take effect
#endif
    BFC R0, #31, #1         @ clear the stacked mark
    @ BCM_AddTaskMPUEntry(tid);
    BL BCM_AddTaskMPUEntry  @ Add an entry in MPU for current task's stack
    POP {LR}
    POP {R0}
#endif

	@ *
	@ * }
	@ *

end_change_stacks:
	@ R0 == tid

	@ *
	@ *if (is_not_marked_stacked(tid)) {
	@ *

	CMP	R0, #0
	BLT	end_run_thread

EE_cortex_mx_std_run_task_code:
    PUSH	{R0}
	PUSH	{LR}
	BLX EE_oo_thread_stub
	POP {R1}        @ Get back LR
    MOV LR, R1
    POP {R0}        @ Get back R0 needed by EE_cortex_mx_cust_run_task_code

@ Build a SVC stack frame on new task's PSP to jump into the Thread-body of new task upon SVC return
@ R3 = 0x01000000 (xPSR) (ARM Mode)
	LDR	R3, =EPSR_T_BIT_VAL
@ R2 = EE_cortex_mx_cust_run_task_code (PC)
	LDR	R2, =_EE_cortex_mx_cust_run_task_code_addr
@ R1 = 0 (LR). Never return from here
	MOV	R1, #0
@ R0 = next tid

@|xPSR|-> xPSR AND 0xFFFFFE0
@| PC |-> EE_cortex_mx_cust_run_task_code
@| LR |-> 0
@| R12| -> don't care
	MRS     R12, PSP
	STMFD   R12!, {R0-R3}

@| R3 | -> don't care
@| R2 | -> don't care
@| R1 | -> don't care
@| R0 | -> tid
	STMFD   R12!, {R0-R3}

@ Build a dummy stack of registers on PSP as stored/restored in EE_cortex_mx_svc_ISR
@| R7 | -> don't care
@| R6 | -> don't care
@| R5 | -> don't care
@| R4 | -> don't care
	STMFD   R12!, {R4-R7}
@| R11| -> don't care
@| R10| -> don't care
@| R9 | -> don't care
@| R8 | -> don't care
	STMFD   R12!, {R0-R3}
@| LR | -> EXC_RETURN
	LDR R1, =EXC_RETURN
    STMFD	R12!, {R1}

    MSR     PSP, R12            @ update PSP

    @ *
	@ * }
	@ *

end_run_thread:
	BX	LR			@ Return

	.size	EE_cortex_mx_change_context, . - EE_cortex_mx_change_context

@void EE_cortex_mx_cust_run_task_code(EE_TID tid)  NORETURN
        .type   EE_cortex_mx_cust_run_task_code, #function
EE_cortex_mx_cust_run_task_code:
	@R0 == tid
	LSLS R0, R0, #2		@ R1 = tid << 2
	LDR	R1, =EE_terminate_real_th_body	@ R1 == EE_terminate_real_th_body[tid]
	ADD	R1, R1, R0
	LDR	R1, [R1]

    @Start the thread body
    BLX R1

    @ The task terminated with a return: do the usual cleanup
    @Raise SVC with ID ThreadNotTerminated
    LDR R1, =BCM_ThreadNotTerminated
    BX R1
    @ does not return

	.size	EE_cortex_mx_cust_run_task_code, . - EE_cortex_mx_cust_run_task_code

@******************************************************************************
@
@ Tell the assembler that we're done.
@
@******************************************************************************
	.end

