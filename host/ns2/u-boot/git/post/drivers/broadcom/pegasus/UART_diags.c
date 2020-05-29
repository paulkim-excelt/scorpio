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

extern void _serial_enter_loopback(const int port);
extern void _serial_exit_loopback(const int port);
extern void _serial_putc_raw(const char c, const int port);
extern int _serial_getc(const int port);
extern int _serial_tstc(const int port);

static int UART_post_internal_loopback(int uartIndex)
{
	char testChar[] = "Hello World";
	volatile char *p;
	char t;
	int ret = 0;

	p = testChar;
	_serial_enter_loopback(uartIndex);

	while (*p) {
		_serial_putc_raw(*p, uartIndex);
		udelay(10000);
		if (!_serial_tstc(uartIndex)) {
			ret = -1;
			post_log("UART:%d did not receive data!\n", uartIndex);
			break;
		}
		t = _serial_getc(uartIndex);
		if (t != *p) {
			post_log("Expected: %c(0x%2x), Actual:  %c(0x%2x)\n",
				 *p, *p, t, t);
			ret = -1;
			break;
		}
		p++;
	}
	_serial_exit_loopback(uartIndex);

	return ret;
}

static int UART_post_ext_self_loopback(int uartIndex)
{
	char testChar[] = "Goodbye cruel World";
	volatile char *p;
	char t;
	int ret = 0;

	p = testChar;

	while (*p) {
		_serial_putc_raw(*p, uartIndex);
		udelay(10000);
		if (!_serial_tstc(uartIndex)) {
			ret = -1;
			post_log("UART:%d did not receive data!\n", uartIndex);
			break;
		}
		t = _serial_getc(uartIndex);
		if (t != *p) {
			post_log("Expected: %c(0x%2x), Actual:  %c(0x%2x)\n",
				 *p, *p, t, t);
			ret = -1;
			break;
		}
		p++;
	}
	return ret;
}

static void UART_post_init(void)
{
/* configure mux/MFIO here */
}

static void UART_post_end(void)
{
/* FIXME:configure mux/MFIO here */
}

/* Note: When using the NS15660 driver the index must be increased by one */
static void drain_out(void)
{
	int idx;

	for (idx = 1; idx <= CONFIG_CONS_INDEX; idx++) {
		while (_serial_tstc(idx))
			_serial_getc(idx);
	}
}

/* Note: When using the NS15660 driver the index must be increased by one */
int UART_post_test(int flags)
{
	int ret, tot_test = 1;
	int i;
	int num_ports = 4;
	char *s = getenv("board_name");

	if (s) {
		if (!strcmp(s, PEGASUS_XMC_BOARD)) {
			post_log("Only UART0/1 port is available on XMC\n");
			num_ports = 2;
		}
	} else  {
		post_log("Unable to get board name\n");
		ret = -1;
		return ret;
	}

	UART_post_init();

	post_log("\nUART diagnostics loopback test\n");

	for (i = 1; i <= num_ports; i++) {
		/*use UART3 as the default console */
		if (i == CONFIG_CONS_INDEX)
			continue;

		drain_out();
		ret = UART_post_internal_loopback(i);
		if (ret == 0) {
			post_log("Internal loopback on UART Port:%d Passed!\n",
				 i - 1);
		} else {
			post_log("Internal loopback on UART Port:%d Failed!\n",
				 i - 1);
			tot_test = -1;
		}
	}

	drain_out();
	udelay(1000);

	post_log("\nUART External Tx->Rx Loopback test.....\n");
	for (i = 1; i <= num_ports; i++) {
		/*use UART3 as the default console */
		if (i == CONFIG_CONS_INDEX)
			continue;

		drain_out();
		ret = UART_post_ext_self_loopback(i);
		if (ret == 0) {
			post_log("External loopback on UART Port:%d Passed!\n",
				 i - 1);
		} else {
			post_log("External loopback on UART Port:%d Failed!\n",
				 i - 1);
			tot_test = -1;
		}
	}

	UART_post_end();

	if (tot_test == 1) {
		post_log("\nUART diags Passed\n");
	} else {
		post_log("\nUART diags Failed\n");
		return -1;
	}
	return 0;
}
