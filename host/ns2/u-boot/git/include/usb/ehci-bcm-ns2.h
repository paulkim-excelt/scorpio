/*
 * Copyright 2016 Broadcom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation (the "GPL").
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 (GPLv2) for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 (GPLv2) along with this source code.
 */

#ifndef __EHCI_BCM_NS2_H
#define __EHCI_BCM_NS2_H

/* USB2 controller on DRD port */
void usb2_drd_clock_disable(void);

/* USB2 controller on USB3 port_0 */
void usbh_clock_disable_m0(void);

/* USB2 controller on USB3 port_1 */
void usbh_clock_disable_m1(void);

#endif /* __EHCI_BCM_NS2_H */
