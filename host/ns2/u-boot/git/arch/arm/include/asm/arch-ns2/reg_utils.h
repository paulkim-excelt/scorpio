/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef REG_UTILS
#define REG_UTILS

#include <linux/types.h>

#define __REG32(x)      (*((u32 *)(x)))
#define __REG16(x)      (*((u16 *)(x)))
#define __REG8(x)       (*((u8 *)(x)))

extern u32 reg_debug;
#define	REG_DEBUG(val) (reg_debug = val)

static inline void reg32_clear_bits(u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg & ~(value)));
#endif
	*reg &= ~(value);
}

static inline void reg32_set_bits(u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg | value));
#endif
	*reg |= value;
}

static inline void reg32_toggle_bits(u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__, (u32)reg,
		       *reg, (*reg ^ value));
#endif
	*reg ^= value;
}

static inline void reg32_write_masked(u32 *reg, u32 mask, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__, (u32)reg,
		       *reg, (*reg & (~mask)) | (value & mask));
#endif
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg32_write(u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x, 0x%x)\n", __func__, (u32)reg, value);
#endif
	*reg = value;
}

static inline u32 reg32_read(u32 *reg)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x\n", __func__, (u32)reg, *reg);
#endif
	return *reg;
}

/****************************************************************************/
/*
 *  16-bit register access functions
 */
/****************************************************************************/

static inline void reg16_clear_bits(u16 *reg, u16 value)
{
	*reg &= ~(value);
}

static inline void reg16_set_bits(u16 *reg, u16 value)
{
	*reg |= value;
}

static inline void reg16_toggle_bits(u16 *reg, u16 value)
{
	*reg ^= value;
}

static inline void reg16_write_masked(u16 *reg, u16 mask, u16 value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg16_write(u16 *reg, u16 value)
{
	*reg = value;
}

static inline u16 reg16_read(u16 *reg)
{
	return *reg;
}

/****************************************************************************/
/*
 *  8-bit register access functions
 */
/****************************************************************************/

static inline void reg8_clear_bits(u8 *reg, u8 value)
{
	*reg &= ~(value);
}

static inline void reg8_set_bits(u8 *reg, u8 value)
{
	*reg |= value;
}

static inline void reg8_toggle_bits(u8 *reg, u8 value)
{
	*reg ^= value;
}

static inline void reg8_write_masked(u8 *reg, u8 mask, u8 value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg8_write(u8 *reg, u8 value)
{
	*reg = value;
}

static inline u8 reg8_read(u8 *reg)
{
	return *reg;
}
#endif /*REG_UTILS */
