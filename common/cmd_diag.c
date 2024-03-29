/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Diagnostics support
 */
#include <common.h>
#include <command.h>
#include <cmd_diag.h>
#include <post.h>

#if (CONFIG_COMMANDS & CFG_CMD_DIAG) && defined(CONFIG_POST)

int do_diag (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	unsigned int i;

	if (argc == 1 || strcmp (argv[1], "run") != 0) {
		/* List test info */
		if (argc == 1) {
			printf ("Available hardware tests:\n");
			post_info (NULL);
			printf ("Use 'diag [<test1> [<test2> ...]]'"
					" to get more info.\n");
			printf ("Use 'diag run [<test1> [<test2> ...]]'"
					" to run tests.\n");
		} else {
			for (i = 1; i < argc; i++) {
			    if (post_info (argv[i]) != 0)
				printf ("%s - no such test\n", argv[i]);
			}
		}
	} else {
		/* Run tests */
		if (argc == 2) {
			post_run (NULL, POST_RAM | POST_MANUAL);
		} else {
			for (i = 2; i < argc; i++) {
			    if (post_run (argv[i], POST_RAM | POST_MANUAL) != 0)
				printf ("%s - unable to execute the test\n",
					argv[i]);
			}
		}
	}

	return 0;
}

#endif /* CFG_CMD_DIAG */
