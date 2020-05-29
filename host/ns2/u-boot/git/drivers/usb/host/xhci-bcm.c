/*
 * $Copyright Open Broadcom Corporation$
 *
 * BROADCOM XHCI Host Controller.
 */

#include <common.h>
#include <pci.h>
#include <usb.h>
#include <asm/io.h>
#include <asm/arch/socregs.h>
#include <asm/arch/bcm_otp.h>
#include "xhci.h"

static int bcm_xhci_core_init(int index)
{
	int status = 0;

	/* Make platform specific inits under ifdef from here */

#ifdef CONFIG_PEGASUS
	if (!is_usb_subsys_supported()) {
		printf("SKU doesn't support usb\n");
		status = -ENODEV;
	}
	/* Pegasus USB 2.0/3.0 PHYs are initialized in ATF.
	 * psus_usb2h_phy_init and psus_usb3h_phy_init routines are
	 * implemented in ATF.
	 */
#endif	/* CONFIG_PEGASUS */

	return status;
}

static void bcm_xhci_core_exit(int index)
{
	/* Make platform specific exits under ifdef from here */

#ifdef CONFIG_PEGASUS
	/* For Pegasus platform, PHY registers are accesible from ATF.
	 * Not implementing psus_usb2h/3h_phy_exit routines here.
	 */
#endif	/* CONFIG_PEGASUS */
}

/*
 * Function to initialize USB xHC host related low level hardware including PHY,
 * clocks, etc.
 */
int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	int status = 0;

	status = bcm_xhci_core_init(index);

	if (!status) {
		/* Map registers */
		*hccr = (struct xhci_hccr *)(XHC_XHC_CPLIVER);
		*hcor = (struct xhci_hcor *)((phys_addr_t)(*hccr) +
				HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));
	}
	return status;
}

void xhci_hcd_stop(int index)
{
	bcm_xhci_core_exit(index);
}

