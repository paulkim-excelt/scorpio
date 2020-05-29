/*
 * CMIC MDIO driver
 */
#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/arch/reg_utils.h>
#include <bcm_miim_mdio.h>

static void ethhw_writel(u32 val, size_t addr)
{
	reg32_write((u32 *)addr, val);
}

static u32 ethhw_readl(size_t addr)
{
	u32 val = reg32_read((u32 *)addr);

	return (u32)val;
}

static void cmicd_miim_initialize(void)
{
	u32 val;

	val = ethhw_readl(ICFG_IPROC_IOPAD_CTRL_11);
	val &= ~(3 << 11);
	val |= (2 << 11);	/*pull down*/
	ethhw_writel(val, ICFG_IPROC_IOPAD_CTRL_11);
	mdelay(10);

	val = ethhw_readl(CMIC_RATE_ADJUST_EXT_MDIO);
	val = 0x00010008;
	ethhw_writel(val, CMIC_RATE_ADJUST_EXT_MDIO);
	mdelay(100);		/*lets wait sometime*/

	val = ethhw_readl(CMIC_RATE_ADJUST_INT_MDIO);
	val = 0x0001000c;
	ethhw_writel(val, CMIC_RATE_ADJUST_INT_MDIO);
	mdelay(100);		/*lets wait sometime*/

	val = ethhw_readl(CMIC_MIIM_SCAN_CTRL);
	val |= (1 << 28);
	ethhw_writel(val, CMIC_MIIM_SCAN_CTRL);
	mdelay(10);
}

static inline u32 cmicd_miim_reg_read(u32 reg)
{
	u32 val;

	val = ethhw_readl((CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg);
	return val;
}

static inline void cmicd_miim_reg_write(u32 reg, u32 data)
{
	/*u32 val; */

	ethhw_writel(data, (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg);
	/* read back and verify */
	/* val = ethhw_readl((CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg); */
}

static inline void cmicd_miim_set_op_read(u32 *data, u32 set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_RD_START_SHIFT,
		      MIIM_CTRL__MIIM_RD_START_MASK, set);
}

static inline void cmicd_miim_set_op_write(u32 *data, u32 set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_WR_START_SHIFT,
		      MIIM_CTRL__MIIM_WR_START_MASK, set);
}

static inline int do_cmicd_miim_op(u32 op, u32 param, u32 addr,
				   u16 *reg_val)
{
	u32 val, op_done;
	int ret = 0;
	int usec = MIIM_OP_MAX_HALT_USEC;

	if (op >= MIIM_OP_MODE_MAX) {
		printf("%s : invalid op code %d\n", __func__, op);
		return SOC_E_INIT;
	}

	/* stop the on going operations if any before
	 * setting other parameter regs
	 */
	cmicd_miim_reg_write(MIIM_CTRL_REG, 0);
	do {
		cmicd_miim_reg_read(MIIM_STAT_REG);
		op_done = GET_REG_FIELD(cmicd_miim_reg_read(MIIM_STAT_REG),
					MIIM_STAT__MIIM_OPN_DONE_SHIFT,
					MIIM_STAT__MIIM_OPN_DONE_MASK);
		if (!op_done)
			break;

		udelay(1);
		usec--;
	} while (usec > 0);
	udelay(100);

	cmicd_miim_reg_write(MIIM_PARAM_REG, param);
	cmicd_miim_reg_write(MIIM_ADDRESS_REG, addr);
	val = cmicd_miim_reg_read(MIIM_CTRL_REG);

	if (op == MIIM_OP_MODE_READ)
		cmicd_miim_set_op_read(&val, 1);
	else
		cmicd_miim_set_op_write(&val, 1);

	cmicd_miim_reg_write(MIIM_CTRL_REG, val);
	usec = MIIM_OP_MAX_HALT_USEC;
	do {
		cmicd_miim_reg_read(MIIM_STAT_REG);
		op_done = GET_REG_FIELD(cmicd_miim_reg_read(MIIM_STAT_REG),
					MIIM_STAT__MIIM_OPN_DONE_SHIFT,
					MIIM_STAT__MIIM_OPN_DONE_MASK);
		if (op_done)
			break;

		udelay(1);
		usec--;
	} while (usec > 0);
	udelay(100);

	if (op_done) {
		if (op == MIIM_OP_MODE_READ)
			*reg_val = cmicd_miim_reg_read(MIIM_READ_DATA_REG);
	} else {
		ret = SOC_E_TIMEOUT;
	}
	cmicd_miim_reg_write(MIIM_CTRL_REG, 0);
	usec = MIIM_OP_MAX_HALT_USEC;
	do {
		cmicd_miim_reg_read(MIIM_STAT_REG);
		op_done = GET_REG_FIELD(cmicd_miim_reg_read(MIIM_STAT_REG),
					MIIM_STAT__MIIM_OPN_DONE_SHIFT,
					MIIM_STAT__MIIM_OPN_DONE_MASK);
		if (!op_done)
			break;

		udelay(1);
		usec--;
	} while (usec > 0);
	udelay(100);

	return ret;
}

static int cmicd_miim_op(struct cmicd_miim_cmd *cmd)
{
	u32 miim_param = 0, miim_addr = 0;
	int rv = 0;

	ISET_REG_FIELD(miim_param, MIIM_PARAM__BUS_ID_SHIFT,
		       MIIM_PARAM__BUS_ID_MASK, cmd->bus_id);

	if (cmd->int_sel)
		ISET_REG_FIELD(miim_param, MIIM_PARAM__INTERNAL_SEL_SHIFT,
			       MIIM_PARAM__INTERNAL_SEL_MASK, 1);

	ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_ID_SHIFT,
		       MIIM_PARAM__PHY_ID_MASK, cmd->phy_id);

	if (cmd->op_mode == MIIM_OP_MODE_WRITE)
		ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_DATA_SHIFT,
			       MIIM_PARAM__PHY_DATA_MASK, cmd->val);

	if (cmd->c45_sel) {
		ISET_REG_FIELD(miim_param, MIIM_PARAM__C45_SEL_SHIFT,
			       MIIM_PARAM__C45_SEL_MASK, 1);

		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT,
			       MIIM_ADDRESS__CLAUSE_45_REGADR_MASK,
			       cmd->regnum);
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT,
			       MIIM_ADDRESS__CLAUSE_45_REGADR_MASK,
			       cmd->regnum >> 16);
	} else {
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT,
			       MIIM_ADDRESS__CLAUSE_22_REGADR_MASK,
			       cmd->regnum);
	}

	rv = do_cmicd_miim_op(cmd->op_mode, miim_param, miim_addr, &cmd->val);

	return rv;
}

void bcm_mdio_write(u32 ext, u32 claus, u32 busid,
		    u32 phyaddr, u32 reg, u16 v)
{
	struct cmicd_miim_cmd cmd = { 0 };
	int rv;

	if (ext == EXTERNAL)
		cmd.int_sel = 0;
	else
		cmd.int_sel = 1;

	if (claus == CLAUS22)
		cmd.c45_sel = 0;
	else
		cmd.c45_sel = 1;

	cmd.bus_id = busid;
	cmd.phy_id = phyaddr;
	cmd.regnum = reg;
	cmd.val = v;
	cmd.op_mode = MIIM_OP_MODE_WRITE;

	rv = cmicd_miim_op(&cmd);
	if (rv < 0) {
		printf("%s : PHY register write is failed! error code is %d\n",
		       __func__, rv);
	}
}

u16 bcm_mdio_read(u32 ext, u32 claus, u32 busid,
		  u32 phyaddr, u32 reg)
{
	struct cmicd_miim_cmd cmd = { 0 };
	int rv;

	if (ext == EXTERNAL)
		cmd.int_sel = 0;
	else
		cmd.int_sel = 1;

	if (claus == CLAUS22)
		cmd.c45_sel = 0;
	else
		cmd.c45_sel = 1;

	cmd.bus_id = busid;
	cmd.phy_id = phyaddr;
	cmd.regnum = reg;
	cmd.op_mode = MIIM_OP_MODE_READ;

	rv = cmicd_miim_op(&cmd);
	if (rv < 0) {
		printf("%s : PHY register read is failed! error code is %d\n",
		       __func__, rv);
	}

	return cmd.val;
}

void bcm_mdio_init(void)
{
	cmicd_miim_initialize();
}
