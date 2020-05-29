/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

#include "ee_internal.h"
#include "ee_irq.h"
#include "cpu/cortex_rx/inc/ee_crash.h"

/*
 * TODO: Allocate fixed memory in TCM to expose
 * exception/crash register info to external host
 */
extern void system_fiq_handler(void);
extern void irq_generic_handler(void);

void EE_cortex_rx_irq_handler(void)
{
	EE_increment_IRQ_nesting_level();

	/* If irq nesting level is one, switch to irq stack */
	EE_cortex_rx_change_stack();

	/* Enable IRQ if nesting  is allowed */
	//EE_std_enableIRQ_nested();

	irq_generic_handler();

	//EE_std_disableIRQ_nested();
	EE_decrement_IRQ_nesting_level();

	if (!EE_is_inside_ISR_call()) {
		EE_cortex_rx_stack_back();
		EE_std_after_IRQ_schedule();
	}
}

void EE_cortex_rx_fiq_handler(void)
{
}

static inline EE_UINT32 cortex_rx_dfsr()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (val));

    return val;
}

static inline EE_UINT32 cortex_rx_adfsr()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c5, c1, 0" : "=r" (val));

    return val;
}

static inline EE_UINT32 cortex_rx_dfar()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (val));

    return val;
}

static inline EE_UINT32 cortex_rx_ifsr()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c5, c0, 1" : "=r" (val));

    return val;
}

static inline EE_UINT32 cortex_rx_aifsr()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c5, c1, 1" : "=r" (val));

    return val;
}

static inline EE_UINT32 cortex_rx_ifar()
{
	EE_UINT32 val;

    asm volatile("mrc p15, 0, %0, c6, c0, 2" : "=r" (val));

    return val;
}

#if 0
void EE_cortex_rx_exception_handler (EE_processor_reg *regs, EE_UINT32 exception_type)
{

	/* offset PC based on exception type */
	if (exception_type == EE_PREFETCH_ABORT)
		regs->pc -= EE_PABT_PC_OFFSET;

	if (exception_type == EE_DATA_ABORT)
		regs->pc -= EE_DABT_PC_OFFSET;

	if (exception_type == EE_UNDEF_INSTR)
		regs->pc -= EE_UNDEF_PC_OFFSET;

    /* switch uart to polling mode */
	printf_setmode(PRINTF_POLL_MODE);

	/* CPU registers of faulting code */
	LOG_CRIT("Exception type: %s\n", EE_bcm8950x_exceptions[exception_type]);

	LOG_CRIT("pc : [<%08lx>]    lr : [<%08lx>]    cpsr: [<%08lx>]\n"
           "sp : [<%08lx>]    r12 : [<%08lx>]    r11 : [<%08lx>]\n",
           regs->pc, regs->lr, regs->cpsr,
           regs->sp, regs->r12, regs->r11);

	LOG_CRIT("r10 : [<%08lx>]    r9 : [<%08lx>]    r8: [<%08lx>]\n"
           "r7 : [<%08lx>]    r6 : [<%08lx>]    r5 : [<%08lx>]\n",
           regs->r10, regs->r9, regs->r8,
           regs->r7, regs->r6, regs->r5);

	LOG_CRIT("r4 : [<%08lx>]    r3 : [<%08lx>]    r2: [<%08lx>]\n"
           "r1 : [<%08lx>]    r0 : [<%08lx>]\n",
           regs->r4, regs->r3, regs->r2,
           regs->r1, regs->r0);

	LOG_CRIT("\n");

    /* Fault Status and Address Registers for abort exception type*/
    if (exception_type == EE_DATA_ABORT) {
	LOG_CRIT("DFSR : [<%08lx>]    ADFSR : [<%08lx>]    DFAR: [<%08lx>]\n",
			cortex_rx_dfsr(), cortex_rx_adfsr(), cortex_rx_dfar());
    }

    if (exception_type == EE_PREFETCH_ABORT) {
	LOG_CRIT("IFSR : [<%08lx>]    AIFSR : [<%08lx>]    IFAR: [<%08lx>]\n",
			cortex_rx_ifsr(), cortex_rx_aifsr(), cortex_rx_ifar());
    }

    while (1) {
   		/* TODO: Enter low power mode? */
    	;
    }
}
#endif
