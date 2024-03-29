/*
 * (C) Copyright 2002
 * Peter De Schrijver (p2@mind.be), Mind Linux Solutions, NV.
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
 *
 */

#include <asm/ppcboot.h>
#include <asm/processor.h>
#include <common.h>
#include <command.h>
#include <configs/ML2.h>

#if (defined CFG_INIT_CHAN1) || (defined CFG_INIT_CHAN2)
#include <ns16550.h>
#endif

#if 0
#include "serial.h"
#endif

#if (defined CFG_INIT_CHAN1) || (defined CFG_INIT_CHAN2)
const NS16550_t COM_PORTS[] = { (NS16550_t) CFG_NS16550_COM1,
		                (NS16550_t) CFG_NS16550_COM2 };
#endif

int
serial_init (void)
{
		DECLARE_GLOBAL_DATA_PTR;

	    int clock_divisor = CFG_NS16550_CLK / 16 / gd->baudrate;

#ifdef CFG_INIT_CHAN1
	    (void)NS16550_init(COM_PORTS[0], clock_divisor);
#endif 
#ifdef CFG_INIT_CHAN2
	    (void)NS16550_init(COM_PORTS[1], clock_divisor);
#endif
		return 0;

}

void
serial_putc(const char c)
{   
    if (c == '\n')
        NS16550_putc(COM_PORTS[CFG_DUART_CHAN], '\r');

    NS16550_putc(COM_PORTS[CFG_DUART_CHAN], c);
}

int
serial_getc(void)
{
    return NS16550_getc(COM_PORTS[CFG_DUART_CHAN]);
}

int
serial_tstc(void)
{
    return NS16550_tstc(COM_PORTS[CFG_DUART_CHAN]);
}

void
serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;

    int clock_divisor = CFG_NS16550_CLK / 16 / gd->baudrate;

#ifdef CFG_INIT_CHAN1
    NS16550_reinit(COM_PORTS[0], clock_divisor);
#endif
#ifdef CFG_INIT_CHAN2
    NS16550_reinit(COM_PORTS[1], clock_divisor);
#endif
}

void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
void
kgdb_serial_init(void)
{
}

void
putDebugChar (int c)
{
	serial_putc (c);
}

void
putDebugStr (const char *str)
{
	serial_puts (str);
}

int
getDebugChar (void)
{
	return serial_getc();
}

void
kgdb_interruptible (int yes)
{
	return;
}
#endif	/* CFG_CMD_KGDB	*/
