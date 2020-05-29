/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 */

#include <common.h>
#include <post.h>
#include <asm/arch/socregs.h>
#include <linux/types.h>
#include <asm/arch/bcm_tamper.h>

#if CONFIG_POST & CONFIG_SYS_POST_TAMPER
#define TAMPER_DEBUG	0
#define BBL_STATUS_TIMEOUT 1000

#define BBL_ISO_DISABLE_FLAG  (~((1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL) |\
			(1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL_TAMPER)))
#define BBL_ISO_ENABLE_FLAG  ((1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL) |\
		(1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL_TAMPER))
#define RST_CMD (1 << SPRU_BBL_CMD__IND_SOFT_RST_N)
#define WR_CMD  (1 << SPRU_BBL_CMD__IND_WR)
#define RD_CMD  (1 << SPRU_BBL_CMD__IND_RD)

#define rd(x) tamper_mem_read32(x)
#define wr(x, v) tamper_mem_write32(x, v)

#define MS_DELAY		1000
#define SEC_DELAY		1000000
#define BBL_AUTH_CODE		0x12345678
#define FILTER_THRESHOLD1	0x010101
#define FILTER_THRESHOLD2	0x1
#define PULL_DN_N1N2_P1P2	0x0
#define PULL_DN_P2		0xC00
#define PULL_DN_P1N1_P2N2	0x600
#define TIMEBASE		0xff
#define GLITCH			0x1c0000
#define MESH_LFSR_CFG		0xf
#define MESH_EXTERNAL_CFG	0x70
#define TAMPER_ENA_N1N2		0x6
#define TAMPER_ENA_P1P2		0x600
#define TAMPER_ENA_P1		0x400
#define TAMPER_ENA_P2		0x800
#define TAMPER_ENA_EMESH	0x1c0000
#define TAMPERIN_ENABLE		0x3
#define PAD_CONFIG_ALL		0xffffff
#define EMESH_P1N1		0x01
#define EMESH_P2N2		0x02
#define EMESH_CFG		0x7c

static void tamper_status(void);


static char get_user_input(void)
{
	char ch;

	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));

	return ch;
}

static int bbl_wr(uint32_t addr, uint32_t v)
{
	uint32_t cmd, timeout;

	udelay(MS_DELAY);
	wr(SPRU_BBL_WDATA, v);
	cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH))) |
		WR_CMD | RST_CMD;
	wr(SPRU_BBL_CMD, cmd);

	/* Check for status == DONE */
	for (timeout = 0; timeout < BBL_STATUS_TIMEOUT; timeout++) {
		if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE))
			return 0;
	}

	post_log("BBL write failed!  write %x to addr %x timed out.\n",
		 v, addr);
	return -1;
}

static int bbl_rd(uint32_t addr)
{
	uint32_t cmd, timeout;

	udelay(MS_DELAY);
	cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH)))
		| RD_CMD | RST_CMD;
	wr(SPRU_BBL_CMD, cmd);

	/* Check for status == DONE */
	for (timeout = 0; timeout < BBL_STATUS_TIMEOUT; timeout++) {
		if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE))
			return rd(SPRU_BBL_RDATA);
	}

	post_log("BBL read failed!  read of addr %x timed out.\n", addr);
	return -1;
}

static void bbl_init(void)
{
	uint32_t v = 0;

	wr(CRMU_ISO_CELL_CONTROL, v);
	wr(CRMU_BBL_AUTH_CHECK, BBL_AUTH_CODE);
	wr(CRMU_BBL_AUTH_CODE, BBL_AUTH_CODE);
	wr(CRMU_BBL_CLEAR_ENABLE, CRMU_BBL_CLEAR_ENABLE_DATAMASK);
}

static int tamper_clear(void)
{
	uint32_t data, rdata, wdata;

	bbl_wr(BBL_EN_TAMPERIN, 0x0);
	bbl_wr(BBL_TAMPER_SRC_ENABLE, 0x0);
	bbl_wr(BBL_EMESH_CONFIG, 0x0);

	/* BBL Clear Emesh states */
	rdata = bbl_rd(BBL_EMESH_CONFIG);
	wdata = rdata | (0x1 << BBL_EMESH_CONFIG__bbl_mesh_clr);
	bbl_wr(BBL_EMESH_CONFIG, wdata);
	wdata = rdata;
	bbl_wr(BBL_EMESH_CONFIG, wdata);

	data = bbl_rd(BBL_TAMPER_SRC_STAT);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, data);
	__udelay(MS_DELAY);

	bbl_wr(BBL_TAMPER_SRC_CLEAR_1, 0x3f);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, 0);
	bbl_wr(BBL_TAMPER_SRC_CLEAR_1, 0);

	__udelay(MS_DELAY);

#if TAMPER_DEBUG
	printf("Status after tamper clear..\n");
	tamper_status();
#endif

	return 0;
}

static void tamper_status(void)
{
#if TAMPER_DEBUG
	post_log("CRMU_MCU_EVENT_STATUS: 0x%x\n", rd(CRMU_MCU_EVENT_STATUS));
	post_log("BBL_TAMPER_SRC_ENABLE: 0x%x\n",
		 bbl_rd(BBL_TAMPER_SRC_ENABLE));
	post_log("BBL_TAMPER_SRC_ENABLE_1: 0x%x\n",
		 bbl_rd(BBL_TAMPER_SRC_ENABLE_1));
	post_log("BBL_TAMPER_SRC_STAT: 0x%x\n", bbl_rd(BBL_TAMPER_SRC_STAT));
	post_log("BBL_TAMPER_SRC_STAT_1: 0x%x\n",
		 bbl_rd(BBL_TAMPER_SRC_STAT_1));
	post_log("BBL_INPUT_STATUS: 0x%x\n", bbl_rd(BBL_INPUT_STATUS));
	post_log("BBL_EN_TAMPERIN: 0x%x\n", bbl_rd(BBL_EN_TAMPERIN));
	post_log("BBL_EMESH_CONFIG: 0x%x\n", bbl_rd(BBL_EMESH_CONFIG));
	post_log("BBL_MESH_CONFIG: 0x%x\n", bbl_rd(BBL_MESH_CONFIG));
	post_log("BBL_GLITCH_CFG: 0x%x\n", bbl_rd(BBL_GLITCH_CFG));

	post_log("BBL_TAMPER_TIMESTAMP: 0x%x\n", bbl_rd(BBL_TAMPER_TIMESTAMP));
	post_log("TAMPER_INP_TIMEBASE: 0x%x\n", bbl_rd(TAMPER_INP_TIMEBASE));
	post_log("FILTER_THREHOLD_CONFIG1: 0x%x\n",
		 bbl_rd(FILTER_THREHOLD_CONFIG1));
	post_log("FILTER_THREHOLD_CONFIG2: 0x%x\n",
		 bbl_rd(FILTER_THREHOLD_CONFIG2));

	post_log("\n");
#endif
}

static int tamper_static_n1_n2_enable(int tamperno)
{
	char ch;
	int status = 0;

	if (tamperno == 1) {
		post_log("\nFor creating static tamper event on N1 input,");
		post_log(" turn on SW11.1 and SW11.3\n");
	} else {
		post_log("\nFor creating static tamper event on N2 input,");
		post_log(" turn on SW11.1 and SW11.5\n");
	}

	post_log("Press Y/N\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N'))
		return -1;

	bbl_wr(FILTER_THREHOLD_CONFIG1, FILTER_THRESHOLD1);
	bbl_wr(PAD_PULL_DN, PULL_DN_N1N2_P1P2);
	bbl_wr(TAMPER_INP_TIMEBASE, TIMEBASE);
	bbl_wr(BBL_GLITCH_CFG, GLITCH);
	bbl_wr(FILTER_THREHOLD_CONFIG2, FILTER_THRESHOLD2);

	bbl_wr(BBL_EMESH_CONFIG, 0x0);
	bbl_wr(BBL_MESH_CONFIG, MESH_LFSR_CFG);
	bbl_wr(BBL_TAMPER_SRC_ENABLE, TAMPER_ENA_N1N2);
	bbl_wr(BBL_EN_TAMPERIN, TAMPERIN_ENABLE);
	bbl_wr(PAD_CONFIG, PAD_CONFIG_ALL);

	post_log("\nWait for 10 seconds and");
	post_log(" confirm if the tamper LED is glowing (Y/N):\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("TAMPER test failed.\n");
		status = -1;
	}

	tamper_status();
	tamper_clear();

	return status;
}

static int tamper_static_p1_p2_enable(int tamperno)
{
	char ch;
	int status = 0;

	if (tamperno == 1) {
		post_log("\nFor creating static tamper event on P1 input, ");
		post_log("turn on SW11.1 and SW11.4\n");
	} else {
		post_log("\nFor creating static tamper event on P2 input, ");
		post_log("turn on SW11.1 and SW11.6\n");
	}

	post_log("Press Y/N\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N'))
		return -1;

	bbl_wr(FILTER_THREHOLD_CONFIG1, FILTER_THRESHOLD1);
	bbl_wr(PAD_PULL_DN, PULL_DN_N1N2_P1P2);
	bbl_wr(TAMPER_INP_TIMEBASE, TIMEBASE);
	bbl_wr(BBL_GLITCH_CFG, GLITCH);
	bbl_wr(FILTER_THREHOLD_CONFIG2, FILTER_THRESHOLD2);
	bbl_wr(PAD_CONFIG, PAD_CONFIG_ALL);

	bbl_wr(BBL_EMESH_CONFIG, 0x0);
	bbl_wr(BBL_MESH_CONFIG, MESH_LFSR_CFG);
	bbl_wr(BBL_TAMPER_SRC_ENABLE, TAMPER_ENA_P1P2);
	bbl_wr(BBL_EN_TAMPERIN, TAMPERIN_ENABLE);

	post_log("\nWait for 10 seconds and");
	post_log(" confirm if the tamper LED is glowing (Y/N):\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("TAMPER test failed.\n");
		status = -1;
	}

	tamper_status();
	tamper_clear();

	return status;
}

static int tamper_emesh_p1_n1_enable(void)
{
	uint32_t wdata;
	char ch;
	int status = 0;

	/* BBL Clear Emesh states.  BEFORE the user flips switches */
	wdata = (0x1 << BBL_EMESH_CONFIG__bbl_mesh_clr);
	bbl_wr(BBL_EMESH_CONFIG, wdata);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, BBL_TAMPER_SRC_CLEAR_DATAMASK);

	post_log("\nFor creating active (Emesh) tamper event on P1_N1 input, ");
	post_log("turn ON SW11.2 and SW11.7\n");
	post_log("Press Y/N\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N'))
		return -1;

	/* Emesh config */
	bbl_wr(BBL_MESH_CONFIG, MESH_EXTERNAL_CFG);
	bbl_wr(BBL_EMESH_CONFIG, ((1<<BBL_EMESH_CONFIG__en_extmesh_fc) |
	       (1<<BBL_EMESH_CONFIG__en_extmesh_dyn) | EMESH_P1N1));
	bbl_wr(BBL_EMESH_CONFIG_1, EMESH_CFG);
	bbl_wr(BBL_TAMPER_SRC_ENABLE, TAMPER_ENA_EMESH);
	bbl_wr(PAD_PULL_DN, PULL_DN_P1N1_P2N2);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, (bbl_rd(BBL_TAMPER_SRC_STAT)));

	tamper_status();

	post_log("\nWait for 10 seconds and");
	post_log(" confirm if the tamper LED is glowing (Y/N):\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("TAMPER test failed.\n");
		status = -1;
	}

	tamper_status();
	tamper_clear();

	return status;
}

static int tamper_emesh_p2_n2_enable(void)
{
	uint32_t wdata;
	char ch;
	int status = 0;

	/* BBL Clear Emesh states.  BEFORE the user flips switches */
	wdata = (0x1 << BBL_EMESH_CONFIG__bbl_mesh_clr);
	bbl_wr(BBL_EMESH_CONFIG, wdata);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, BBL_TAMPER_SRC_CLEAR_DATAMASK);

	post_log("\nFor creating active (Emesh) tamper event on P2_N2 input, ");
	post_log("turn ON SW11.2 and SW11.8\n");
	post_log("Press Y/N\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N'))
		return -1;

	/* Emesh config  */
	bbl_wr(BBL_MESH_CONFIG, MESH_EXTERNAL_CFG);
	bbl_wr(BBL_EMESH_CONFIG, ((1<<BBL_EMESH_CONFIG__en_extmesh_fc) |
	       (1<<BBL_EMESH_CONFIG__en_extmesh_dyn) | EMESH_P2N2));
	bbl_wr(BBL_EMESH_CONFIG_1, EMESH_CFG);
	bbl_wr(BBL_TAMPER_SRC_ENABLE, TAMPER_ENA_EMESH);
	bbl_wr(PAD_PULL_DN, PULL_DN_P1N1_P2N2);
	bbl_wr(BBL_TAMPER_SRC_CLEAR, (bbl_rd(BBL_TAMPER_SRC_STAT)));

	post_log("\nWait for 10 seconds and");
	post_log(" confirm if the tamper LED is glowing (Y/N):\n");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("TAMPER test failed.\n");
		status = -1;
	}

	tamper_status();
	tamper_clear();

	return status;
}

int tamper_test(void)
{
	int status = 0;
	char ch;
	uint32_t v;

	/* Do BBL INIT and get the present status of Tamper status registers */
	bbl_init();
	tamper_clear();

	post_log("\nTamper test..\n");

	/* Active tamper p1_n1 */
	__udelay(SEC_DELAY);
	if (tamper_emesh_p1_n1_enable() != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switch 11.7 to OFF position. Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}

	tamper_clear();

	/* Active tamper p2_n2 */
	__udelay(SEC_DELAY);
	if (tamper_emesh_p2_n2_enable() != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switches 11.8 and 11.2 to OFF position.");
	post_log(" Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}
	post_log("\nTamper Clear..\n");
	tamper_clear();

	/* Static tamper n1 */
	if (tamper_static_n1_n2_enable(1) != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switch 11.3 to OFF position. Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}
	post_log("\nTamper Clear..\n");
	tamper_clear();

	/* Static tamper p1 */
	__udelay(SEC_DELAY);
	if (tamper_static_p1_p2_enable(1) != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switch 11.4 to OFF position. Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}
	post_log("\nTamper Clear..\n");
	tamper_clear();

	/* Static tamper n2 */
	__udelay(SEC_DELAY);
	if (tamper_static_n1_n2_enable(2) != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switch 11.5 to OFF position. Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}
	post_log("\nTamper Clear..\n");
	tamper_clear();

	/* Static tamper p2 */
	__udelay(SEC_DELAY);
	if (tamper_static_p1_p2_enable(2) != 0)
		status = -1;

	/* Clear the tamper and get the status of tamper registers */
	post_log("\nTurn switch 11.6 and 11.1 to OFF position.");
	post_log(" Press Y to continue");
	ch = get_user_input();
	if ((ch == 'n') || (ch == 'N')) {
		post_log("Test aborted by user.\n");
		status = -1;
		return status;
	}
	post_log("\nTamper Clear..\n");
	tamper_clear();


	v = rd(CRMU_ISO_CELL_CONTROL);
	v &= BBL_ISO_DISABLE_FLAG;
	wr(CRMU_ISO_CELL_CONTROL, v);

	return status;
}

static void help(void)
{
	post_log("\n -----------------------\n");
	post_log("| TAMPER DIAG HELP MENU |\n");
	post_log(" -----------------------\n");
}

int TAMPER_post_test(int flags)
{
	int status = 0;
	char ch;

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	post_log("\nEnsure all switches of SW11 are in OFF position before");
	post_log("start of test (Y/N):\n");

	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y') &&
		(ch != 'n') && (ch != 'N'));

	if ((ch == 'Y') || (ch == 'y')) {
		status = tamper_test();
	} else {
		post_log("Test aborted by user.\n");
		status = -1;
	}

	return status;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_TAMPER */
