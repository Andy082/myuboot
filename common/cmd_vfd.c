/*
 * (C) Copyright 2001
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
 * Command to load a splash screen to the VFDs.
 * NOTE that this will be controlled by a key combination when
 * the keyboard stuff works. For now the user has to enter a
 * bitmap number (only VFD_TEST_LOGO is supported now - 16.10.2002).
 * Added VFD_REMOTE_LOGO (same as VFD_TEST_LOGO but a different color)
 * on 20.10.2002.
 *
 * This rather crudely requires that each bitmap be included as a
 * header file.
 */
#include <common.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_VFD)
#ifdef VFD_TEST_LOGO
#include <vfd_logo.h>
#define VFD_TEST_LOGO_BMPNR 0
#define VFD_REMOTE_LOGO_BMPNR 1
#endif

extern int transfer_pic(unsigned char, unsigned char *, int, int);

int trab_vfd (ulong bitmap);

int do_vfd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong bitmap;

	if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	bitmap = simple_strtoul(argv[1], NULL, 10);

	return (trab_vfd(bitmap));
}
#endif	/* CFG_CMD_VFD */

#ifdef CONFIG_VFD
int trab_vfd (ulong bitmap)
{
	switch (bitmap) {
#ifdef VFD_TEST_LOGO
		case VFD_TEST_LOGO_BMPNR:
			transfer_pic(1, &vfd_test_logo_bitmap[0],
				VFD_TEST_LOGO_HEIGHT, VFD_TEST_LOGO_WIDTH);
			return 0;
		case VFD_REMOTE_LOGO_BMPNR:
			transfer_pic(1, &vfd_remote_logo_bitmap[0],
				VFD_TEST_LOGO_HEIGHT, VFD_TEST_LOGO_WIDTH);
			return 0;
#endif
		default:
			printf("Unknown bitmap %ld\n", bitmap);
			return 1;
	}
	/* NOTREACHED */
}
#endif	/* CONFIG_VFD */
