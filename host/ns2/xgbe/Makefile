#obj-$(CONFIG_BCM_XGBE) += bcm-xgbe.o
obj-m += brcm-xgbe.o

obj-$(CONFIG_BRCM_XGBM) += bcm-xgbe-objs := brcm-xgbe-main.o brcm-xgbe-drv.o brcm-xgbe-dev.o \
		 brcm-xgbe-desc.o brcm-xgbe-ethtool.o brcm-xgbe-pci.o
all:
	make ARCH=arm64 CROSS_COMPILE=$(HOST_TOOLCHAIN) -C $(KERNEL) M=$(PWD) modules

clean:
	make -C $(KERNEL) M=$(PWD) clean
