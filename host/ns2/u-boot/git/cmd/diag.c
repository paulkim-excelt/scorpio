/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Diagnostics support
 */
#include <common.h>
#include <command.h>
#include <post.h>

int do_diag (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int i;

	if (argc == 1 || strcmp (argv[1], "run") != 0) {
		/* List test info */
		if (argc == 1) {
			post_log("Available hardware tests:\n");
			post_info(NULL);
			post_log("Use 'diag [<test1> [<test2> ...]]'");
			post_log(" to get more info.\n");
			post_log("Use 'diag run [<test1> [<test2> ...]]'");
			post_log(" to run tests.\n");
			post_log("Use 'diag help [<test1> [<test2> ...]]'");
			post_log(" to see the help menu for each test.\n");
		} else if (strcmp(argv[1], "help") == 0) {
			if (argc == 2)
				post_run(NULL,
					 POST_RAM | POST_MANUAL | POST_HELP);
			for (i = 2; i < argc; i++) {
				if (post_info(argv[i]) != 0)
					printf("%s: does not exist\n", argv[i]);
				else
					post_run(argv[i],
						 POST_RAM |
						 POST_MANUAL |
						 POST_HELP);
			}
		} else {
			for (i = 1; i < argc; i++) {
				if (post_info(argv[i]) != 0)
					printf("%s: no such test\n", argv[i]);
			}
		}
	} else {
		/* Run tests */
		if (argc == 2) {
			post_run (NULL, POST_RAM | POST_MANUAL);
		} else if ((argc == 3) && (strcmp(argv[2], "auto") == 0)) {
			post_run(NULL, POST_RAM | POST_AUTO);
		} else {
			for (i = 2; i < argc; i++) {
				if (post_run(argv[i],
					     POST_RAM | POST_MANUAL) != 0)
					printf("%s: unable to execute test\n",
					       argv[i]);

			}
		}
	}
	return 0;
}
/***************************************************/

U_BOOT_CMD(
	diag,	CONFIG_SYS_MAXARGS,	0,	do_diag,
	"perform board diagnostics",
	     "    - print list of available tests\n"
	"diag [test1 [test2]]\n"
	"         - print information about specified tests\n"
	"diag run - run all available tests\n"
	"diag run [test1 [test2]]\n"
	"         - run specified tests"
);
