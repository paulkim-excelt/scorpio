/*
 * Copyright (C) 2019 Broadcom Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
/*
 * AMD 10Gb Ethernet driver
 *
 * This file is available to you under your choice of the following two
 * licenses:
 *
 * License 1: GPLv2
 *
 * Copyright (c) 2016 Advanced Micro Devices, Inc.
 *
 * This file is free software; you may copy, redistribute and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or (at
 * your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *     The Synopsys DWC ETHER XGMAC Software Driver and documentation
 *     (hereinafter "Software") is an unsupported proprietary work of Synopsys,
 *     Inc. unless otherwise expressly agreed to in writing between Synopsys
 *     and you.
 *
 *     The Software IS NOT an item of Licensed Software or Licensed Product
 *     under any End User Software License Agreement or Agreement for Licensed
 *     Product with Synopsys or any supplement thereto.  Permission is hereby
 *     granted, free of charge, to any person obtaining a copy of this software
 *     annotated with this license and the Software, to deal in the Software
 *     without restriction, including without limitation the rights to use,
 *     copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *     of the Software, and to permit persons to whom the Software is furnished
 *     to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 *     THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
 *     BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *     PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS
 *     BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *     THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * License 2: Modified BSD
 *
 * Copyright (c) 2016 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *     The Synopsys DWC ETHER XGMAC Software Driver and documentation
 *     (hereinafter "Software") is an unsupported proprietary work of Synopsys,
 *     Inc. unless otherwise expressly agreed to in writing between Synopsys
 *     and you.
 *
 *     The Software IS NOT an item of Licensed Software or Licensed Product
 *     under any End User Software License Agreement or Agreement for Licensed
 *     Product with Synopsys or any supplement thereto.  Permission is hereby
 *     granted, free of charge, to any person obtaining a copy of this software
 *     annotated with this license and the Software, to deal in the Software
 *     without restriction, including without limitation the rights to use,
 *     copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *     of the Software, and to permit persons to whom the Software is furnished
 *     to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 *     THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
 *     BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *     PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS
 *     BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *     THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/log2.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/moduleparam.h>

#include "xgbe.h"
#include "xgbe-common.h"
#include "xgbe-ioctl.h"

#define DTCM_IPC_START  (0x2003C000)
#define DTCM_IPC_END    (0x2003FFFF)

#define DTCM_TOP_BRIDGE_START   (0x4A800000)
#define DTCM_TOP_BRIDGE_END     (0x4AA00000)

#define PCI_BAR1_BASE                   (0x20000000)

#define PCI_BAR1_ITCM_BASE              (0x20000000)
#define PCI_BAR1_DTCM_OFFSET            (0x80000)
#define PCI_BAR1_DTCM_DEV_BASE          (0x20000000)

static struct xgbe_prv_data *gPrivData;

#ifdef DEBUG
#define DBG_LOG    printk
#else
#define DBG_LOG
#endif

static int xgbe_pci_open(struct inode *inode, struct file *file)
{
	printk("%s: Device Opened.\n", __func__);
	return nonseekable_open(inode, file);
}

static int xgbe_pci_release(struct inode *inode, struct file *file)
{
	printk("%s:Device closed.\n", __func__);
	return 0;
}

static long xgbe_pci_ioctl(struct file *file, unsigned int cmd, unsigned long ioctl_arg)
{
    int err = 0;
    struct xgbe_ioctlcmd *iocmd;
    struct pci_dev *pdev;
    uint32_t base;
    uint32_t offset;
    uint32_t *data;
    uint16_t *u16Ptr;
    uint32_t *u32Prt;
    uint32_t val;
    int dev_mem = 0;

    if (NULL == gPrivData) {
        err = EINVAL;
        goto err_exit;
    }
    pdev = gPrivData->pcidev;

    if ((XGBE_IOCTL_RDMEM != cmd) &&
            (XGBE_IOCTL_WRMEM != cmd)) {
        err = EINVAL;
        goto err_exit;
    }

    iocmd = (struct xgbe_ioctlcmd *)ioctl_arg;
    DBG_LOG("%s: mem(%s): %x width:%x\n", __func__, ((cmd == XGBE_IOCTL_RDMEM) ? "read": "write"), iocmd->addr, iocmd->width);

    if ((iocmd->addr >= DTCM_IPC_START) && (iocmd->addr < DTCM_IPC_END)) {
        base = PCI_BAR1_ITCM_BASE;
        offset = (iocmd->addr - PCI_BAR1_DTCM_DEV_BASE) + PCI_BAR1_DTCM_OFFSET;
    } else if ((iocmd->addr >= DTCM_TOP_BRIDGE_START) && (iocmd->addr < DTCM_TOP_BRIDGE_END)) {
        base = 0x22000000;
        offset = iocmd->addr - 0x4A000000;
        offset = (offset & 0xFFFF0000) + (offset & 0xFFFF) * 2;
        dev_mem = 1;
    } else {
        err = EINVAL;
        goto err_exit;
    }
    pci_write_config_dword(pdev, 0x84, base);
    err = pci_read_config_dword(pdev, 0x84, &base);
    if (err)
        goto err_exit;

    DBG_LOG("%s: BAR1Window: %x offset: %x\n", __func__, base, offset);
    switch (cmd) {
        case XGBE_IOCTL_RDMEM:
            switch (iocmd->width) {
            case 32:
                *((uint32_t *)iocmd->data)= ioread32(gPrivData->xpcs_regs + offset);
                break;
            case 16:
                if (dev_mem) {
                    val = ioread32(gPrivData->xpcs_regs + offset);
                    if (iocmd->addr & 0x3) {
                        *((uint16_t *)iocmd->data)= (val >> 16) & 0xFFFF;
                    } else {
                        *((uint16_t *)iocmd->data) = val & 0xFFFF;
                    }
                } else {
                    *((uint16_t *)iocmd->data) = ioread16(gPrivData->xpcs_regs + offset);
                }
                break;
            default:
                err = EINVAL;
                break;
            }
            DBG_LOG("%s: read addr:%x val: %x\n", __func__, (uint32_t)(gPrivData->xpcs_regs + offset), *data);
            break;
        case XGBE_IOCTL_WRMEM:
            switch (iocmd->width) {
            case 32:
                iowrite32(*((uint32_t *)iocmd->data), gPrivData->xpcs_regs + offset);
                break;
            case 16:
                if (dev_mem) {
                    if (iocmd->addr & 0x3) {
                        val = ((*((uint16_t *)iocmd->data)) & 0xFFFF) << 16;
                    } else {
                        val = (*((uint16_t *)iocmd->data)) & 0xFFFF;
                    }
                    iowrite32(val, gPrivData->xpcs_regs + offset);
                } else {
                    iowrite16((*((uint16_t *)iocmd->data)), gPrivData->xpcs_regs + offset);
                }
                break;
            default:
                err = EINVAL;
                break;
            }
            DBG_LOG("%s: write addr:%x val: %x\n", __func__, (uint32_t)(gPrivData->xpcs_regs + offset), *data);
            break;
        default:
            err = EINVAL;
            break;
    }
err_exit:
    return err;
}

static const struct file_operations xgbe_file_ops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = xgbe_pci_ioctl,
	.open = xgbe_pci_open,
	.release = xgbe_pci_release,
	.llseek = noop_llseek,
};

static int xgbe_config_multi_msi(struct xgbe_prv_data *pdata)
{
	unsigned int vector_count;
	unsigned int i, j;
	int ret;

	vector_count = XGBE_MSI_BASE_COUNT;
	vector_count += max(pdata->rx_ring_count,
			    pdata->tx_ring_count);

	ret = pci_alloc_irq_vectors(pdata->pcidev, XGBE_MSI_MIN_COUNT,
				    vector_count, PCI_IRQ_MSI | PCI_IRQ_MSIX);
	if (ret < 0) {
		dev_info(pdata->dev, "multi MSI/MSI-X enablement failed\n");
		return ret;
	}
   else
   {
	dev_info(pdata->dev, "multi MSI/MSI-X enablement passed\n");
   }

	pdata->isr_as_tasklet = 1;
	pdata->irq_count = ret;

	pdata->dev_irq = pci_irq_vector(pdata->pcidev, 0);

	for (i = XGBE_MSI_BASE_COUNT, j = 0; i < ret; i++, j++)
		pdata->channel_irq[j] = pci_irq_vector(pdata->pcidev, i);
	pdata->channel_irq_count = j;

	pdata->per_channel_irq = 1;
	pdata->channel_irq_mode = XGBE_IRQ_MODE_LEVEL;

	if (netif_msg_probe(pdata))
		dev_dbg(pdata->dev, "multi %s interrupts enabled\n",
			pdata->pcidev->msix_enabled ? "MSI-X" : "MSI");

	return 0;
}

static int xgbe_config_irqs(struct xgbe_prv_data *pdata)
{
	int ret;

	ret = xgbe_config_multi_msi(pdata);
	if (!ret)
		goto out;
	ret = pci_alloc_irq_vectors(pdata->pcidev, 1, 1,
				    PCI_IRQ_MSI);
	if (ret < 0) {
		dev_info(pdata->dev, "single IRQ enablement failed\n");
		return ret;
	}

	pdata->isr_as_tasklet = pdata->pcidev->msi_enabled ? 1 : 0;
	pdata->irq_count = 1;
	pdata->channel_irq_count = 1;

	pdata->dev_irq = pci_irq_vector(pdata->pcidev, 0);

	if (netif_msg_probe(pdata))
		dev_dbg(pdata->dev, "single %s interrupt enabled\n",
			pdata->pcidev->msi_enabled ?  "MSI" : "legacy");

out:
	if (netif_msg_probe(pdata)) {
		unsigned int i;

		dev_dbg(pdata->dev, " dev irq=%d\n", pdata->dev_irq);
		for (i = 0; i < pdata->channel_irq_count; i++)
			dev_dbg(pdata->dev, " dma%u irq=%d\n",
				i, pdata->channel_irq[i]);
	}

	return 0;
}

static int xgbe_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct xgbe_prv_data *pdata;
	struct device *dev = &pdev->dev;
	void __iomem * const *iomap_table;
    struct miscdevice *miscdev;
	int bar_mask;
	int ret;
    u32 temp;

	pdata = xgbe_alloc_pdata(dev);
	if (IS_ERR(pdata)) {
		ret = PTR_ERR(pdata);
		goto err_alloc;
	}
    gPrivData = pdata;

	pdata->pcidev = pdev;
	pci_set_drvdata(pdev, pdata);

	/* Get the version data */
	pdata->vdata = (struct xgbe_version_data *)id->driver_data;

	ret = pcim_enable_device(pdev);
	if (ret) {
		dev_err(dev, "pcim_enable_device failed\n");
		goto err_pci_enable;
	}

	/* Obtain the mmio areas for the device */
	bar_mask = pci_select_bars(pdev, IORESOURCE_MEM);
    printk( "bar_mask = %x\n", bar_mask );
	ret = pcim_iomap_regions(pdev, bar_mask, XGBE_DRV_NAME);
	if (ret) {
		dev_err(dev, "pcim_iomap_regions failed\n");
		goto err_pci_enable;
	}

	iomap_table = pcim_iomap_table(pdev);
	if (!iomap_table) {
		dev_err(dev, "pcim_iomap_table failed\n");
		ret = -ENOMEM;
		goto err_pci_enable;
	}

    printk("%s: iomap addresses: BAR0: %x BAR1: %x\n",
            __func__, iomap_table[XGBE_XGMAC_BAR],
            iomap_table[2]);


    pci_write_config_dword(pdev, 0x74, 0x4000 );
    pci_write_config_dword(pdev, 0x78, 0x5000 );
    pci_write_config_dword(pdev, 0x94, 0x0400 );

	pdata->xgmac_regs = iomap_table[XGBE_XGMAC_BAR];

    XGMAC_IOWRITE(pdata, 0x8, 0xe00);

	pdata->xgmac_regs += 0x2000;

    //update SBtoPCIETranslationBigMem
    temp = XGMAC_IOREAD(pdata, 0x108);
    temp &= ~0x80000000;
    XGMAC_IOWRITE(pdata, 0x108, temp);
    temp = XGMAC_IOREAD(pdata, 0x108);
    printk( "SBtoPCIETranslationBigMem = %x\n", temp );

	pdata->xgmac_regs = iomap_table[XGBE_XGMAC_BAR];
	pdata->xgmac_regs += 0x3000; //XGMAC offset from BAR 0
    printk( "xgmac_reg base = %p\n", pdata->xgmac_regs );

    /* Set the PCIBAR1Window */
    pci_write_config_dword(pdev, 0x84, PCI_BAR1_BASE);
    pdata->xpcs_regs = iomap_table[2];
    printk( "xgmac_reg base = %p\n", pdata->xpcs_regs);

	if (!pdata->xgmac_regs) {
		dev_err(dev, "xgmac ioremap failed\n");
		ret = -ENOMEM;
		goto err_pci_enable;
	}
        else
            dev_err(dev, "xgmac ioremap done  \n");

	pci_set_master(pdev);

	/* Clock settings */
	pdata->sysclk_rate = XGBE_V2_DMA_CLOCK_FREQ;
	pdata->ptpclk_rate = XGBE_V2_PTP_CLOCK_FREQ;

	/* Set the DMA coherency values */
	pdata->coherent = 1;
	pdata->arcr = XGBE_DMA_PCI_ARCR;
	pdata->awcr = XGBE_DMA_PCI_AWCR;
	pdata->awarcr = XGBE_DMA_PCI_AWARCR;

	/* Set the hardware channel and queue counts */
	xgbe_set_counts(pdata);

	/* Configure interrupt support */
	ret = xgbe_config_irqs(pdata);
	if (ret)
		goto err_pci_enable;

    miscdev = &pdata->miscdev;
    miscdev->minor = 0;
    miscdev->name = XGBE_DRV_NAME;
    miscdev->nodename = "net/bcm";
    miscdev->fops = &xgbe_file_ops;
	ret = misc_register(miscdev);
	if (ret) {
		printk("%s: Can't register misc device\n", __func__);
		goto err_irq_vectors;
	}

	/* Configure the netdev resource */
	ret = xgbe_config_netdev(pdata);
	if (ret)
		goto err_irq_vectors;

	netdev_notice(pdata->netdev, "net device enabled\n");

	return 0;

err_irq_vectors:
	pci_free_irq_vectors(pdata->pcidev);

err_pci_enable:
	xgbe_free_pdata(pdata);

err_alloc:
	dev_notice(dev, "net device not enabled\n");

	return ret;
}

static void xgbe_pci_remove(struct pci_dev *pdev)
{
	struct xgbe_prv_data *pdata = pci_get_drvdata(pdev);

    misc_deregister(&pdata->miscdev);
	xgbe_deconfig_netdev(pdata);

	pci_free_irq_vectors(pdata->pcidev);

	xgbe_free_pdata(pdata);
}

#ifdef CONFIG_PM
static int xgbe_pci_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct xgbe_prv_data *pdata = pci_get_drvdata(pdev);
	struct net_device *netdev = pdata->netdev;
	int ret = 0;

	if (netif_running(netdev))
		ret = xgbe_powerdown(netdev, XGMAC_DRIVER_CONTEXT);

	pdata->lpm_ctrl = XMDIO_READ(pdata, MDIO_MMD_PCS, MDIO_CTRL1);
	pdata->lpm_ctrl |= MDIO_CTRL1_LPOWER;
	XMDIO_WRITE(pdata, MDIO_MMD_PCS, MDIO_CTRL1, pdata->lpm_ctrl);

	return ret;
}

static int xgbe_pci_resume(struct pci_dev *pdev)
{
	struct xgbe_prv_data *pdata = pci_get_drvdata(pdev);
	struct net_device *netdev = pdata->netdev;
	int ret = 0;

	pdata->lpm_ctrl &= ~MDIO_CTRL1_LPOWER;
	XMDIO_WRITE(pdata, MDIO_MMD_PCS, MDIO_CTRL1, pdata->lpm_ctrl);

	if (netif_running(netdev)) {
		ret = xgbe_powerup(netdev, XGMAC_DRIVER_CONTEXT);

		/* Schedule a restart in case the link or phy state changed
		 * while we were powered down.
		 */
		schedule_work(&pdata->restart_work);
	}

	return ret;
}
#endif /* CONFIG_PM */

static const struct xgbe_version_data xgbe_v2a = {
	.xpcs_access			= XGBE_XPCS_ACCESS_V2,
	.mmc_64bit			= 1,
	.tx_max_fifo_size		= 229376,
	.rx_max_fifo_size		= 229376,
	.tx_tstamp_workaround		= 1,
	.ecc_support			= 0,
	.irq_reissue_support		= 1,
	.tx_desc_prefetch		= 3,
	.rx_desc_prefetch		= 3,
};

static const struct pci_device_id xgbe_pci_table[] = {
	{ PCI_VDEVICE(BROADCOM, 0x4311),
	  .driver_data = (kernel_ulong_t)&xgbe_v2a },
	{ PCI_VDEVICE(BROADCOM, 0xA005),
	  .driver_data = (kernel_ulong_t)&xgbe_v2a },
	/* Last entry must be zero */
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, xgbe_pci_table);

static struct pci_driver xgbe_driver = {
	.name = XGBE_DRV_NAME,
	.id_table = xgbe_pci_table,
	.probe = xgbe_pci_probe,
	.remove = xgbe_pci_remove,
#ifdef CONFIG_PM
	.suspend = xgbe_pci_suspend,
	.resume = xgbe_pci_resume,
#endif
};

int xgbe_pci_init(void)
{
	return pci_register_driver(&xgbe_driver);
}

void xgbe_pci_exit(void)
{
	pci_unregister_driver(&xgbe_driver);
}

