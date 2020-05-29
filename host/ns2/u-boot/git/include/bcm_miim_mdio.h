#include <asm/arch/socregs.h>
#include <bcmiproc_phy.h>

#define CMICD_BASE_ADDRESS		CMIC_RATE_ADJUST_EXT_MDIO
#define CMC2_OFFSET			    (0x00)

#define MIIM_PARAM_REG			(0x23c)
#define MIIM_PARAM__MIIM_CYCLE_SHIFT	CMIC_MIIM_PARAM__MIIM_CYCLE_R
#define MIIM_PARAM__MIIM_CYCLE_MASK	\
	((1 << CMIC_MIIM_PARAM__MIIM_CYCLE_WIDTH) - 1)
#define MIIM_PARAM__INTERNAL_SEL_SHIFT CMIC_MIIM_PARAM__INTERNAL_SEL
#define MIIM_PARAM__INTERNAL_SEL_MASK	\
	((1 << CMIC_MIIM_PARAM__INTERNAL_SEL_WIDTH) - 1)
#define MIIM_PARAM__BUS_ID_SHIFT CMIC_MIIM_PARAM__BUS_ID_R
#define MIIM_PARAM__BUS_ID_MASK	\
	((1 << CMIC_MIIM_PARAM__BUS_ID_WIDTH) - 1)
#define MIIM_PARAM__C45_SEL_SHIFT	CMIC_MIIM_PARAM__C45_SEL
#define MIIM_PARAM__C45_SEL_MASK	\
	((1 << CMIC_MIIM_PARAM__INTERNAL_SEL_WIDTH) - 1)
#define MIIM_PARAM__PHY_ID_SHIFT CMIC_MIIM_PARAM__PHY_ID_R
#define MIIM_PARAM__PHY_ID_MASK	\
	((1 << CMIC_MIIM_PARAM__PHY_ID_WIDTH) - 1)
#define MIIM_PARAM__PHY_DATA_SHIFT CMIC_MIIM_PARAM__PHY_DATA_R
#define MIIM_PARAM__PHY_DATA_MASK	\
	((1 << CMIC_MIIM_PARAM__PHY_DATA_WIDTH) - 1)

#define MIIM_READ_DATA_REG		CMIC_MIIM_READ_DATA_BASE
#define MIIM_READ_DATA__DATA_SHIFT	CMIC_MIIM_READ_DATA__DATA_R
#define MIIM_READ_DATA__DATA_MASK	\
	((1 << CMIC_MIIM_READ_DATA__DATA_WIDTH) - 1)

#define MIIM_ADDRESS_REG		CMIC_MIIM_ADDRESS_BASE
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT	\
	CMIC_MIIM_ADDRESS__CLAUSE_45_DTYPE_R
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_MASK	\
	((1 << CMIC_MIIM_ADDRESS__CLAUSE_45_DTYPE_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT	\
	CMIC_MIIM_ADDRESS__CLAUSE_45_REGADR_R
#define MIIM_ADDRESS__CLAUSE_45_REGADR_MASK	\
	((1 << CMIC_MIIM_ADDRESS__CLAUSE_45_REGADR_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT	0
#define MIIM_ADDRESS__CLAUSE_22_REGADR_MASK	(0x1F)

#define MIIM_CTRL_REG			CMIC_MIIM_CTRL_BASE
#define MIIM_CTRL__MIIM_RD_START_SHIFT  CMIC_MIIM_CTRL__MIIM_RD_START
#define MIIM_CTRL__MIIM_RD_START_MASK	\
	((1 << CMIC_MIIM_CTRL__MIIM_RD_START_WIDTH) - 1)
#define MIIM_CTRL__MIIM_WR_START_SHIFT CMIC_MIIM_CTRL__MIIM_WR_START
#define MIIM_CTRL__MIIM_WR_START_MASK	\
	((1 << CMIC_MIIM_CTRL__MIIM_WR_START_WIDTH) - 1)

#define MIIM_STAT_REG			CMIC_MIIM_STAT_BASE
#define MIIM_STAT__MIIM_OPN_DONE_SHIFT CMIC_MIIM_STAT__MIIM_OPN_DONE
#define MIIM_STAT__MIIM_OPN_DONE_MASK	\
	((1 << CMIC_MIIM_STAT__MIIM_OPN_DONE_WIDTH) - 1)

#define SET_REG_FIELD(reg_value, fshift, fmask, fvalue)	\
	(reg_value) = ((reg_value) & ~((fmask) << (fshift))) |  \
			(((fvalue) & (fmask)) << (fshift))
#define ISET_REG_FIELD(reg_value, fshift, fmask, fvalue) \
		(reg_value) = (reg_value) | (((fvalue) & (fmask)) << (fshift))
#define GET_REG_FIELD(reg_value, fshift, fmask)	\
	(((reg_value) & ((fmask) << (fshift))) >> (fshift))

#define MIIM_OP_MAX_HALT_USEC	500

enum {
	MIIM_OP_MODE_READ,
	MIIM_OP_MODE_WRITE,
	MIIM_OP_MODE_MAX
};

struct cmicd_miim_cmd {
	int bus_id;
	int int_sel;
	int phy_id;
	int regnum;
	int c45_sel;
	u16 op_mode;
	u16 val;
};
extern void bcm_mdio_write(u32 ext, u32 claus, u32 busid,
			   u32 phyaddr, u32 reg, u16 v);
extern u16 bcm_mdio_read(u32 ext, u32 claus, u32 busid, u32 phyaddr, u32 reg);
#include <asm/arch/bcm_mdio.h>
