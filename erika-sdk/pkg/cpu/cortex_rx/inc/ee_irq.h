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

 /** 
	@file ee_irq.h
	@brief Prestub and postub macros and related stuffs
    @      Derived from cpu/cortex_mx/inc/ee_irq.h
*/ 

#ifndef __INCLUDE_CORTEX_RX_IRQ_H__
#define __INCLUDE_CORTEX_RX_IRQ_H__

#define EE_std_change_context(x) 	EE_cortex_rx_change_context(x)

/* Use angled parenthesis to include the main "ee_internal.h" */
#include "cpu/cortex_rx/inc/ee_cpu.h"
#include "cpu/cortex_rx/inc/ee_context.h"
#include "cpu/common/inc/ee_irqstub.h"

#ifdef __ALLOW_NESTED_IRQ__

extern struct EE_TOS EE_std_IRQ_tos;

#define EE_std_enableIRQ_nested() EE_cortex_rx_enableIRQ()
#define EE_std_disableIRQ_nested() EE_cortex_rx_disableIRQ()

#else	/* else __ALLOW_NESTED_IRQ__*/

#define EE_std_enableIRQ_nested() ((void)0)
#define EE_std_disableIRQ_nested() ((void)0)

#endif /* end __ALLOW_NESTED_IRQ__*/

#if defined(__MULTI__) && defined(__IRQ_STACK_NEEDED__)

extern void EE_cortex_rx_change_IRQ_stack(void);
extern void EE_cortex_rx_change_IRQ_stack_back(void);

/*save the stack pointer*/ /*Load new stack pointer*/
#define EE_cortex_rx_change_stack()\
do {\
	if(EE_IRQ_nesting_level==1) {\
	EE_cortex_rx_change_IRQ_stack();\
	}\
} while(0)

#define EE_cortex_rx_stack_back()\
	EE_cortex_rx_change_IRQ_stack_back()

#else	/* else __MULTI__ && __IRQ_STACK_NEEDED__*/

#define EE_cortex_rx_change_stack() ((void)0)
#define EE_cortex_rx_stack_back() ((void)0)

#endif	/* end __MULTI__ && __IRQ_STACK_NEEDED__*/
 
extern void EE_std_irq_handler(void);

#define EE_ISR2_prestub(void)\
/* Defined as Macro */\
do {\
}\
while(0)

extern EE_UREG	EE_cortex_rx_change_context_active;

#define EE_ISR2_poststub(void)\
/* Defined as Macro */\
do{\
}\
while(0)



#define ISR1(f)			\
void ISR1_ ## f(void);		\
__IRQ void f(void) \
{				\
	ISR1_ ## f();		\
}				\
void ISR1_ ## f(void)


#define ISR2(f)			\
void ISR2_ ## f(void);		\
__IRQ void f(void) \
{				\
	EE_ISR2_prestub();	\
	ISR2_ ## f();		\
	EE_ISR2_poststub();	\
}				\
void ISR2_ ## f(void)

/* Standard Macro to declare an ISR (2) */
#define ISR(f) ISR2(f)


/* ISR priorities */
#define EE_ISR_PRI_0 0x00000000U
#define EE_ISR_PRI_1 0x00000001U
#define EE_ISR_PRI_2 0x00000002U
#define EE_ISR_PRI_3 0x00000003U
#define EE_ISR_PRI_4 0x00000004U
#define EE_ISR_PRI_5 0x00000005U
#define EE_ISR_PRI_6 0x00000006U
#define EE_ISR_PRI_7 0x00000007U
#define EE_ISR_PRI_8 0x00000008U
#define EE_ISR_PRI_9 0x00000009U
#define EE_ISR_PRI_10 0x0000000AU
#define EE_ISR_PRI_11 0x0000000BU
#define EE_ISR_PRI_12 0x0000000CU
#define EE_ISR_PRI_13 0x0000000DU
#define EE_ISR_PRI_14 0x0000000EU
#define EE_ISR_PRI_15 0x0000000FU

#endif /* __INCLUDE_CORTEX_RX_IRQ_H__ */
