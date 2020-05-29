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
; 	@file ee_gnu_irq_stack.S
; 	@brief Context switch function for multistack on Cortex_RX
;   @brief Derived from cpu/cortex_mx/src/ee_gnu_irq_stack.S
; 	@brief Stack switch for ISRs on Cortex_RX.
*/

@*******************************************************************************
@                         PUBLIC FUNCTIONS
@*******************************************************************************
	.global	EE_cortex_rx_change_IRQ_stack
	.global	EE_cortex_rx_change_IRQ_stack_back

	.global	EE_cortex_rx_IRQ_tos


@*******************************************************************************
@                              DATA SECTION
@*******************************************************************************
	.data

EE_cortex_rx_tmp_tos:	.word 	0	@ EE_UREG EE_cortex_rx_tmp_tos;

@*******************************************************************************
@                              CODE SECTION
@*******************************************************************************
	.text

@ kernel code is in ARM-mode
	.syntax unified
	.arch armv7-r
	.cpu cortex-r4

@void EE_cortex_rx_change_IRQ_stack(void);
        .type   EE_cortex_rx_change_IRQ_stack, #function
EE_cortex_rx_change_IRQ_stack:

	MOV R0, SP						@ R0 = Current stack pointer
	LDR	R1, =EE_cortex_rx_tmp_tos	@ R1 = address of EE_cortex_rx_tmp_tos
	STR	R0, [R1]					@ Save current stack in EE_cortex_rx_tmp_tos
	LDR	R0, =EE_std_IRQ_tos 		@ R0 = address of EE_cortex_rx_IRQ_tos
	LDR	R0, [R0]					@ R0 = IRQ new stack pointer
	MOV	SP, R0						@ change IRQ stack

	BX LR 							@ return

	.size	EE_cortex_rx_change_IRQ_stack, . - EE_cortex_rx_change_IRQ_stack

@*******************************************************************************

@void EE_cortex_rx_change_IRQ_stack_back(void);
        .type   EE_cortex_rx_change_IRQ_stack_back, #function
EE_cortex_rx_change_IRQ_stack_back:

	LDR	R0, =EE_cortex_rx_tmp_tos	@ R0 = address of EE_cortex_rx_tmp_tos
	LDR	R0, [R0]					@ R0 = old SP
	MOV	SP, R0						@ Restore the stack pointer

	BX LR							@ return

	.size	EE_cortex_rx_change_IRQ_stack_back, . - EE_cortex_rx_change_IRQ_stack_back

@******************************************************************************
@
@ Tell the assembler that we're done.
@
@******************************************************************************
	.end

