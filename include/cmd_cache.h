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
#ifndef	_CMD_CACHE_H
#define	_CMD_CACHE_H

#if (CONFIG_COMMANDS & CFG_CMD_CACHE)
#define	CMD_TBL_ICACHE	MK_CMD_TBL_ENTRY(					\
	"icache",	2,	2,	1,	do_icache,			\
	"icache  - enable or disable instruction cache\n",			\
	"[on, off]\n"								\
	"    - enable or disable instruction cache\n"				\
),

#define CMD_TBL_DCACHE	MK_CMD_TBL_ENTRY(					\
	"dcache",	2,	2,	1,	do_dcache,			\
	"dcache  - enable or disable data cache\n",				\
	"[on, off]\n"								\
	"    - enable or disable data (writethrough) cache\n"			\
),
int do_icache (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_dcache (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#else
#define CMD_TBL_ICACHE
#define CMD_TBL_DCACHE
#endif	/* CFG_CMD_CACHE */

#endif	/* _CMD_CACHE_H */
