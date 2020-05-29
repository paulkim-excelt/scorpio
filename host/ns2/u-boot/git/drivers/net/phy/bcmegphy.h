#ifndef _BCM_EGPHY
#define _BCMEGPHY

#define MIIM_BCMEGPHY_AUXCNTL                    0x18
#define MIIM_BCMEGPHY_AUXCNTL_ENCODE(val) (((val & 0x7) << 12) | (val & 0x7))
#define MIIM_BCMEGPHY_AUXSTATUS                  0x19
#define MIIM_BCMEGPHY_AUXSTATUS_LINKMODE_MASK    0x0700
#define MIIM_BCMEGPHY_AUXSTATUS_LINKMODE_SHIFT   8

#define MIIM_BCMEGPHY_SHD                        0x1c
#define MIIM_BCMEGPHY_SHD_WRITE                  0x8000
#define MIIM_BCMEGPHY_SHD_VAL(x)                 ((x & 0x1f) << 10)
#define MIIM_BCMEGPHY_SHD_DATA(x)                ((x & 0x3ff) << 0)
#define MIIM_BCMEGPHY_SHD_WR_ENCODE(val, data)   \
	(MIIM_BCMEGPHY_SHD_WRITE | MIIM_BCMEGPHY_SHD_VAL(val) | \
	 MIIM_BCMEGPHY_SHD_DATA(data))

#define MIIM_BCMEGPHY_EXP_DATA           0x15    /* Expansion register data */
#define MIIM_BCMEGPHY_EXP_SEL            0x17    /* Expansion register select */
#define MIIM_BCMEGPHY_EXP_SEL_SSD        0x0e00  /* Secondary SerDes select */
#define MIIM_BCMEGPHY_EXP_SEL_ER         0x0f00  /* Expansion register select */

#define BCM_EGPHY28_PHYID 0x600d8441

#endif
