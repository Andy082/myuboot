/*
 * (C) Copyright 2000
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
 * Cache support: switch on or off, get status
 */
#include <common.h>
#include <command.h>
#include <cmd_cache.h>

#if (CONFIG_COMMANDS & CFG_CMD_CACHE)

static int on_off (const char *);

int do_icache ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (on_off(argv[1])) {
#if 0	/* prevented by varargs handling; FALLTROUGH is harmless, too */
		default: printf ("Usage:\n%s\n", cmdtp->usage);
			return;
#endif
		case 0:	icache_disable();
			break;
		case 1:	icache_enable ();
			break;
		}
		/* FALL TROUGH */
	case 1:			/* get status */
		printf ("Instruction Cache is %s\n",
			icache_status() ? "ON" : "OFF");
		return 0;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	return 0;
}

int do_dcache ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (on_off(argv[1])) {
#if 0	/* prevented by varargs handling; FALLTROUGH is harmless, too */
		default: printf ("Usage:\n%s\n", cmdtp->usage);
			return;
#endif
		case 0:	dcache_disable();
			break;
		case 1:	dcache_enable ();
			break;
		}
		/* FALL TROUGH */
	case 1:			/* get status */
		printf ("Data (writethrough) Cache is %s\n",
			dcache_status() ? "ON" : "OFF");
		return 0;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	return 0;

}

static int on_off (const char *s)
{
	if (strcmp(s, "on") == 0) {
		return (1);
	} else if (strcmp(s, "off") == 0) {
		return (0);
	}
	return (-1);
}

#endif	/* CFG_CMD_CACHE */
