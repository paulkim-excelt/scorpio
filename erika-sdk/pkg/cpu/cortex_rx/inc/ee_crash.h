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

#ifndef __INCLUDE_CPU_CORTEX_RX_EE_CRASH__
#define __INCLUDE_CPU_CORTEX_RX_EE_CRASH__

/* Size of EE_processor_reg in bytes*/
#define EE_REG_FRAME_SIZE				0x44

#define EE_ROM_REENTRY					0x0
#define EE_UNDEF_INSTR					0x1
#define EE_SOFTWARE_INTR				0x2
#define EE_PREFETCH_ABORT				0x3
#define EE_DATA_ABORT					0x4

#define EE_PABT_PC_OFFSET				0x4
#define EE_DABT_PC_OFFSET				0x8
#define EE_UNDEF_PC_OFFSET				0x2

#endif /* __INCLUDE_CPU_CORTEX_RX_EE_CRASH__ */
