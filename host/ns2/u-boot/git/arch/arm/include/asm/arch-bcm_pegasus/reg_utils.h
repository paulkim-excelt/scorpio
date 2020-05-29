/*
 * $Copyright Open Broadcom Corporation$
 */

#ifndef REG_UTILS
#define REG_UTILS

/* ---- Include Files ---------------------------------------------------- */

#include <linux/types.h>

/* ---- Public Constants and Types --------------------------------------- */

#define __REG32(x)      (*((volatile u32 *)(x)))
#define __REG16(x)      (*((volatile u16 *)(x)))
#define __REG8(x)       (*((volatile u8 *) (x)))

/* ---- Public Variable Externs ------------------------------------------ */
/* ---- Public Function Prototypes --------------------------------------- */

/****************************************************************************/
/*
 *   32-bit register access functions
 */
/****************************************************************************/

extern u32 reg_debug;
#define	REG_DEBUG(val) (reg_debug = val)

/*#define DEBUG_REG*/

static inline void reg32_clear_bits(volatile u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg & ~(value)));
#endif
	*reg &= ~(value);
}

static inline void reg32_set_bits(volatile u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg | value));
#endif
	*reg |= value;
}

static inline void reg32_toggle_bits(volatile u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg ^ value));
#endif
	*reg ^= value;
}

static inline void
reg32_write_masked(volatile u32 *reg, u32 mask, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x 0x%x\n", __func__,
		       (u32)reg, *reg, (*reg & (~mask)) | (value & mask));
#endif
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg32_write(volatile u32 *reg, u32 value)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x, 0x%x)\n", __func__, (u32)reg,
		       value);
#endif
	*reg = value;
}

static inline u32 reg32_read(volatile u32 *reg)
{
#ifdef DEBUG_REG
	if (reg_debug)
		printf("%s reg (0x%x): 0x%x\n", __func__, (u32)reg,
		       *reg);
#endif
	return *reg;
}

/****************************************************************************/
/*
 *   16-bit register access functions
 */
/****************************************************************************/

static inline void reg16_clear_bits(volatile u16 *reg, u16 value)
{
	*reg &= ~(value);
}

static inline void reg16_set_bits(volatile u16 *reg, u16 value)
{
	*reg |= value;
}

static inline void reg16_toggle_bits(volatile u16 *reg, u16 value)
{
	*reg ^= value;
}

static inline void
reg16_write_masked(volatile u16 *reg, u16 mask, u16 value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg16_write(volatile u16 *reg, u16 value)
{
	*reg = value;
}

static inline u16 reg16_read(volatile u16 *reg)
{
	return *reg;
}

/****************************************************************************/
/*
 *   8-bit register access functions
 */
/****************************************************************************/

static inline void reg8_clear_bits(volatile u8 *reg, u8 value)
{
	*reg &= ~(value);
}

static inline void reg8_set_bits(volatile u8 *reg, u8 value)
{
	*reg |= value;
}

static inline void reg8_toggle_bits(volatile u8 *reg, u8 value)
{
	*reg ^= value;
}

static inline void
reg8_write_masked(volatile u8 *reg, u8 mask, u8 value)
{
	*reg = (*reg & (~mask)) | (value & mask);
}

static inline void reg8_write(volatile u8 *reg, u8 value)
{
	*reg = value;
}

static inline u8 reg8_read(volatile u8 *reg)
{
	return *reg;
}
#endif /* REG_UTILS */
