/*
 * MDIO routines for CMIC internal/external MDIO bus
 */
enum {
	INTERNAL,
	EXTERNAL
};

enum {
	CLAUS22,
	CLAUS45
};

/*  API */
void bcm_mdio_write(u32 ext, u32 c45, u32 busid,
		    u32 phyaddr, u32 reg, u16 v);
u16 bcm_mdio_read(u32 ext, u32 c45, u32 busid,
		       u32 phyaddr, u32 reg);
