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
	@file ee_cpu.h
	@brief CPU-dependent part of HAL. Derived from cpu/cortex_mx/inc/ee_cpu.h
*/

#ifndef __INCLUDE_CORTEX_RX_EE_CPU_H__
#define __INCLUDE_CORTEX_RX_EE_CPU_H__

#include "eecfg.h"

#ifdef __IAR__
#include "cpu/common/inc/ee_compiler_iar.h"
#else	/* __IAR__ */
#ifdef __CCS__
#include "cpu/common/inc/ee_compiler_ccs.h"
#else	/* __CCS__ */
#ifdef __KEIL__
#include "cpu/common/inc/ee_compiler_keil.h"
#else	/* __KEIL__ */
#ifdef __GNU__
#include "cpu/common/inc/ee_compiler_gcc.h"
#else	/* __GNU__ */
#error Unsupported compiler
#endif	/* !__GNU__ */
#endif	/* !__KEIL__ */
#endif	/* !__CCS__ */
#endif	/* !__IAR__ */

/* Initial stack offset (in words): Used in multistack. */
#ifndef CORTEX_RX_INIT_TOS_OFFSET
#define	CORTEX_RX_INIT_TOS_OFFSET	0x11U	/* r4-r12, LR, CPSR */
#endif

/*************************************************************************
 HAL Types and structures
 *************************************************************************/

/* Primitive data types */
#include "cpu/common/inc/ee_types.h"

typedef EE_UINT32 EE_UREG;
typedef EE_INT32  EE_SREG;
typedef EE_UINT32 EE_FREG;
#define EE_UREG_SIZE 4

#define	EE_HWREG_PTR		volatile EE_UREG *
#define	EE_HWREG_ADDR(x)	((EE_HWREG_PTR)(x))
#define	EE_HWREG(x)			(*EE_HWREG_ADDR(x))

#define EE_TIME_UINT64		volatile EE_UINT64

#ifdef __GNU__

/* Get current SP */
__INLINE__ EE_UINT32 __ALWAYS_INLINE__ __current_sp(void)
{
EE_UINT32 temp;
__ASM ("mov %0, sp" : "=r" (temp) : );
return temp;
}

/* Set a breakpoint */
#define __breakpoint(value)	__ASM ("bkpt "#value)

#endif

/* ISR Priority representation type */
typedef EE_UREG EE_TYPEISR2PRIO;

/* Thread IDs */
typedef EE_INT32 EE_TID;

/* Thread IDs - unsigned version*/
typedef EE_UINT32 EE_UTID;

/* Used by the common layer to decide whether to start a new thread */
#define TID_IS_STACKED_MARK 0x80000000

/* EE_TYPEIRQ is currently unused */

/* XXX: define EE_TIME? */

/* ARM Registers */
typedef volatile struct{
	EE_UINT32 r0;
	EE_UINT32 r1;
	EE_UINT32 r2;
	EE_UINT32 r3;
	EE_UINT32 r4;
	EE_UINT32 r5;
	EE_UINT32 r6;
	EE_UINT32 r7;
	EE_UINT32 r8;
	EE_UINT32 r9;
	EE_UINT32 r10;
	EE_UINT32 r11;
	EE_UINT32 r12;
	EE_UINT32 sp;
	EE_UINT32 lr;
	EE_UINT32 pc;
	EE_UINT32 cpsr;
} EE_processor_reg;

/* Use the "standard" implementation */
#include "cpu/common/inc/ee_hal_structs.h"

/******************************************************************************
 Application dependent data types
 ******************************************************************************/

#ifdef	__HAS_TYPES_H__
#include	"types.h"
#endif

/******************************************************************************
 CPU Peripherals
 ******************************************************************************/


/******************************************************************************
 HAL Variables
 ******************************************************************************/

#ifdef __MULTI__

/* Top-of-stack of each private stack */
extern struct EE_TOS EE_cortex_rx_system_tos[];
//#define	EE_std_system_tos EE_cortex_rx_system_tos
#define	EE_cortex_rx_system_tos EE_std_system_tos

/* Index of the current stack */
extern EE_UREG EE_cortex_rx_active_tos;
#define	EE_hal_active_tos EE_cortex_rx_active_tos

#endif /* __MULTI__ */

/*********************************************************************
 Cortex R4 interrupt disabling/enabling
 *********************************************************************/

/* Used to check the value returned by EE_cortex_rx_disableIRQ */
#define EE_cortex_rx_are_IRQs_enabled(ie) ((ie) ^ 0x80)

/**
 * Enable interrupts
 */
__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_enableIRQ(void)
{
	__ASM volatile("cpsie i");
}

/**
 * Disable interrupts
 */
__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_disableIRQ(void)
{
	__ASM volatile("cpsid i");
}

/**
 * Enable FIQ
 */
__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_enableFIQ(void)
{
	__ASM volatile("cpsie f");
}

/**
 * Disable FIQ
 */
__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_disableFIQ(void)
{
	__ASM volatile("cpsid f");
}

/**
 * Resume interrupts
 */
__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_resumeIRQ(EE_FREG f)
{
	__ASM volatile("msr cpsr_c, %0" :: "r" (f));
}

/**
 * Suspend interrupts
 */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_cortex_rx_suspendIRQ(void)
{
	EE_FREG istat;

	__ASM volatile ("mrs %0, cpsr" : "=r" (istat) );
	__ASM volatile ("cpsid i");

	return istat;
}

/**
 * Return true (not 0) if IRQs are enabled, 0 (false) if IRQ are disabled.
 */
__INLINE__ EE_UINT32 __ALWAYS_INLINE__ EE_cortex_rx_get_IRQ_enabled(void)
{
	EE_UREG ie;

	__ASM volatile ("mrs %0, cpsr" : "=r" (ie) );

	return !EE_cortex_rx_are_IRQs_enabled(ie);
}

__INLINE__ EE_TYPEISR2PRIO __ALWAYS_INLINE__ EE_cortex_rx_get_int_prio(void)
{
	return 0;
}

__INLINE__ void __ALWAYS_INLINE__ EE_cortex_rx_set_int_prio(
  EE_TYPEISR2PRIO prio
)
{
}


/*************************************************************************
 Functions exported by the HAL to the kernel
 *************************************************************************/

/* 
 * Interrupt Handling
 */

/** Hal Enable Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_enableIRQ(void)
{
    EE_cortex_rx_enableIRQ();
}

/** Hal Disable Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_disableFIQ(void)
{
    EE_cortex_rx_disableFIQ();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_enableFIQ(void)
{
    EE_cortex_rx_enableFIQ();
}

/** Hal Disable Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_disableIRQ(void)
{
    EE_cortex_rx_disableIRQ();
}

/** Hal Resume Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_resumeIRQ(EE_FREG f)
{
    EE_cortex_rx_resumeIRQ(f);
}

/** Hal Suspend Interrupts */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_suspendIRQ(void)
{
    return EE_cortex_rx_suspendIRQ();
}

/**************************************************************************
 System Initialization
***************************************************************************/

void EE_system_init(void);

/*************************************************************************
 CPU-dependent ORT support (mainly OTM)
 *************************************************************************/

/* Probably, some parts of the OTM code below does not depend on the
 * architecture.  They should be moved somewhere into pkg/cpu/common if this
 * turns out to be the case. */

#define EE_ORTI_OTM_ID_RUNNINGISR2 1
#define EE_ORTI_OTM_ID_SERVICETRACE 2

#ifdef __OO_ORTI_USE_OTM__
void EE_cortex_rx_send_otm8(EE_UINT8 id, EE_UINT8 data);
void EE_cortex_rx_send_otm32(EE_UINT8 id, EE_UINT32 data);

#else /* if __OO_ORTI_USE_OTM__ */
__INLINE__ void EE_cortex_rx_send_otm8(EE_UINT8 id, EE_UINT8 data)
{
	/* OTM disabled */
}

__INLINE__ void EE_cortex_rx_send_otm32(EE_UINT8 id, EE_UINT32 data)
{
	/* OTM disabled */
}
#endif /* else __OO_ORTI_USE_OTM__ */

#ifdef __OO_ORTI_RUNNINGISR2__
__INLINE__ void EE_ORTI_send_otm_runningisr2(EE_ORTI_runningisr2_type isr2)
{
	EE_cortex_rx_send_otm32(EE_ORTI_OTM_ID_RUNNINGISR2, (EE_UINT32)isr2);
}
#endif /* __OO_ORTI_RUNNINGISR2__ */

#ifdef __OO_ORTI_SERVICETRACE__
__INLINE__ void EE_ORTI_send_otm_servicetrace(EE_UINT8 srv)
{
	EE_cortex_rx_send_otm8(EE_ORTI_OTM_ID_SERVICETRACE, srv);
}

#endif /* __OO_ORTI_SERVICETRACE__ */

#endif /* __INCLUDE_CORTEX_RX_EE_CPU_H__ */
