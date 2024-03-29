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
#ifndef _POST_H
#define _POST_H

#ifndef	__ASSEMBLY__
#include <common.h>
#endif

#ifdef CONFIG_POST

#define POST_POWERON		0x01	/* test runs on power-on booting */
#define POST_POWERNORMAL	0x02	/* test runs on normal booting */
#define POST_POWERFAIL		0x04	/* test runs on power-fail booting */
#define POST_POWERTEST		0x08	/* test runs after watchdog reset */

#define POST_ROM		0x0100	/* test runs in ROM */
#define POST_RAM		0x0200	/* test runs in RAM */
#define POST_MANUAL		0x0400	/* test runs on diag command */
#define POST_REBOOT		0x0800	/* test may cause rebooting */

#define POST_MEM		(POST_RAM | POST_ROM)
#define POST_ALWAYS		(POST_POWERNORMAL | \
				 POST_POWERFAIL	  | \
				 POST_MANUAL	  | \
				 POST_POWERON	)

#ifndef	__ASSEMBLY__

struct post_test {
	char *name;
	char *cmd;
	char *desc;
	int flags;
	int (*test) (int flags);
};
void post_bootmode_init (void);
int post_bootmode_get (unsigned int * last_test);
void post_bootmode_clear (void);
int post_run (char *name, int flags);
int post_info (char *name);
int post_log (char *format, ...);
void post_reloc (void);

extern struct post_test post_list[];
extern unsigned int post_list_size;

#endif /* __ASSEMBLY__ */

#define CFG_POST_RTC		0x00000001
#define CFG_POST_WATCHDOG	0x00000002
#define CFG_POST_MEMORY		0x00000004
#define CFG_POST_CPU		0x00000008
#define CFG_POST_I2C		0x00000010
#define CFG_POST_CACHE		0x00000020
#define CFG_POST_UART		0x00000040
#define CFG_POST_ETHER		0x00000080
#define CFG_POST_SPI		0x00000100
#define CFG_POST_USB		0x00000200
#define CFG_POST_SPR		0x00000400

#endif /* CONFIG_POST */

#endif /* _POST_H */
