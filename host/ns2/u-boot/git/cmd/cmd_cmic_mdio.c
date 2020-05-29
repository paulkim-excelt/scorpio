#include <common.h>
#include <command.h>
#include <linux/compiler.h>
#include <bcm_miim_mdio.h>

static u32 ext = INTERNAL;
static u32 claus = CLAUS22;
static u32 busid;
static u32 phyaddr;
static u32 reg;
static u16 v;

static int do_cmic_mdio(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	char ch[2];
	u16 ret = 0;

	ch[0] = argv[1][0];
	busid = simple_strtoul(argv[4], NULL, 16);
	phyaddr = simple_strtoul(argv[5], NULL, 16);
	reg = simple_strtoul(argv[6], NULL, 16);

	switch (ch[0]) {
	case 'w':
		v = simple_strtoul(argv[7], NULL, 16);
		printf("cmic_mdio write bus:%x phy:%x reg:%x writing :0x%x\n",
		       busid, phyaddr, reg, v);
		bcm_mdio_write(ext, claus, busid, phyaddr, reg, v);
		/* read back the register */
		ret = bcm_mdio_read(ext, claus, busid, phyaddr, reg);
		printf
		    ("cmic_mdio read bus:%x phy:%x reg:%x --> read:0x%x\n",
		     busid, phyaddr, reg, ret);
		break;

	case 'r':
		ret = bcm_mdio_read(ext, claus, busid, phyaddr, reg);
		printf("cmic_mdio read bus:%x phy:%x reg:%x --> read :0x%x\n",
		       busid, phyaddr, reg, ret);
		break;
	}

	return 0;
}

U_BOOT_CMD(cmic_mdio, 8, 1, do_cmic_mdio,
	   "Broadcom CMIC mdio command",
	   "cmic_mdio r <external? 0/1> <claus45? 0/1> <bus_id> <phy_id> <register>\n "
	   "cmic_mdio w <external? 0/1> <claus45? 0/1> <bus_id> <phy_id> <register> <value>\n"
);
