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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <linux/mtd/doc2000.h>
#include <watchdog.h>
#include <pci.h>

#include "hardware.h"
#include "pcippc2.h"
#include "sconsole.h"
#include "fpga_serial.h"

#if defined(CONFIG_WATCHDOG)

static int pcippc2_wdt_init_done = 0;

void pcippc2_wdt_init (void);

#endif

  /* Check board identity
   */
int checkboard (void)
{
#ifdef CONFIG_PCIPPC2
	puts ("Board: Gespac PCIPPC-2\n");
#else
	puts ("Board: Gespac PCIPPC-6\n");
#endif
	return 0;
}

  /* RAM size is stored in CPC0_RGBAN1
   */
u32 pcippc2_sdram_size (void)
{
	return in32 (REG (CPC0, RGBAN1));
}

long initdram (int board_type)
{
	return cpc710_ram_init ();
}

void do_reset (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	out32 (REG (CPC0, SPOR), 0);
	iobarrier_rw ();
	while (1);
}

int board_pre_init (void)
{
	out32 (REG (CPC0, RSTR), 0xC0000000);
	iobarrier_rw ();

	out32 (REG (CPC0, RSTR), 0xF0000000);
	iobarrier_rw ();

	out32 (REG (CPC0, UCTL), 0x00F80000);

	out32 (REG (CPC0, SIOC0), 0x30000000);

	out32 (REG (CPC0, ABCNTL), 0x00000000);

	out32 (REG (CPC0, SESR), 0x00000000);
	out32 (REG (CPC0, SEAR), 0x00000000);

	/* Detect IBM Avignon CPC710 Revision */
	if ((in32 (REG (CPC0, UCTL)) & 0x000000F0) == CPC710_TYPE_100P)
		out32 (REG (CPC0, PGCHP), 0xA0000040);
	else
		out32 (REG (CPC0, PGCHP), 0x80800040);
		

	out32 (REG (CPC0, ATAS), 0x709C2508);

	iobarrier_rw ();

	return 0;
}

void after_reloc (ulong dest_addr)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* Jump to the main ppcboot board init code
	 */
	board_init_r (gd, dest_addr);
}

int misc_init_r (void)
{
	pcippc2_fpga_init ();

#if defined(CONFIG_WATCHDOG)
	pcippc2_wdt_init ();
#endif

	fpga_serial_init (sconsole_get_baudrate ());

	sconsole_putc   = fpga_serial_putc;
	sconsole_puts   = fpga_serial_puts;
	sconsole_getc   = fpga_serial_getc;
	sconsole_tstc   = fpga_serial_tstc;
	sconsole_setbrg = fpga_serial_setbrg;

	sconsole_flush ();
	return (0);
}

void pci_init (void)
{
	cpc710_pci_init ();

	/* FPGA requires no retry timeouts to be enabled
	 */
	cpc710_pci_enable_timeout ();
}

void doc_init (void)
{
	doc_probe (pcippc2_fpga1_phys + HW_FPGA1_DOC);
}

#if defined(CONFIG_WATCHDOG)

void pcippc2_wdt_init (void)
{
	out16r (FPGA (WDT, PROG), 0xffff);
	out8 (FPGA (WDT, CTRL), 0x1);

	pcippc2_wdt_init_done = 1;
}

void pcippc2_wdt_done (void)
{
	out8 (FPGA (WDT, CTRL), 0x0);

	pcippc2_wdt_init_done = 0;
}

void pcippc2_wdt_reset (void)
{
	if (pcippc2_wdt_init_done == 1)
		out8 (FPGA (WDT, REFRESH), 0x56);
}

void watchdog_reset (void)
{
	int re_enable = disable_interrupts ();

	pcippc2_wdt_reset ();
	if (re_enable)
		enable_interrupts ();
}

#if (CONFIG_COMMANDS & CFG_CMD_BSP)
int do_wd (cmd_tbl_t *cmdtp, bd_t *bd, int flag, int argc, char *argv[])
{
	switch (argc) {
	case 1:
		printf ("Watchdog timer status is %s\n",
			pcippc2_wdt_init_done == 1 ? "on" : "off");

		return 0;
	case 2:
		if (!strcmp(argv[1],"on")) {
			pcippc2_wdt_init();
			printf("Watchdog timer now is on\n");

			return 0;

		} else if (!strcmp(argv[1],"off")) {
			pcippc2_wdt_done();
			printf("Watchdog timer now is off\n");

			return 0;

		} else
			break;
	default:
		break;
	}
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

#endif	/* CFG_CMD_BSP */
#endif	/* CONFIG_WATCHDOG */
