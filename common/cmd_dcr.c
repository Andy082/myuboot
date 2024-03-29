/*
 * (C) Copyright 2001
 * Erik Theisen,  Wave 7 Optics, etheisen@mindspring.com.
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
 * IBM 4XX DCR Functions
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <cmd_dcr.h>

#if defined(CONFIG_4xx) && defined(CFG_CMD_SETGETDCR)

/* ======================================================================
 * Interpreter command to retrieve an IBM PPC 4xx Device Control Register
 * ======================================================================
 */
int do_getdcr ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
    unsigned short dcrn;                     /* Device Control Register Num */
    unsigned long value;                     /* DCR's value */

    /* Validate arguments */
    if (argc < 2) {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    /* Get a DCR */
    dcrn = (unsigned short)simple_strtoul(argv[ 1 ], NULL, 16);
    value = get_dcr(dcrn);

    printf("%04x: %08lx\n", dcrn, value);

    return 0;
} /* do_getdcr */


/* ======================================================================
 * Interpreter command to set an IBM PPC 4xx Device Control Register
 * ======================================================================
*/
int do_setdcr ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned short dcrn;                     /* Device Control Register Num */
    unsigned long value;                     /* DCR's value */
    int nbytes;
    extern char console_buffer[];

    /* Validate arguments */
    if (argc < 2) {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }

    /* Set a DCR */
    dcrn = (unsigned short)simple_strtoul(argv[1], NULL, 16);
    do {
        value = get_dcr(dcrn);
        printf("%04x: %08lx", dcrn, value);
        nbytes = readline(" ? ");
        if (nbytes == 0) {
            /*
             * <CR> pressed as only input, don't modify current
             * location and exit command.
             */
            nbytes = 1;
            return 0;
        } else {
            unsigned long i;
            char *endp;
            i = simple_strtoul(console_buffer, &endp, 16);
            nbytes = endp - console_buffer;
            if (nbytes)
                set_dcr(dcrn, i);
        }
    } while (nbytes);

    return 0;
} /* do_setdcr */

#endif /* CONFIG_4xx & CFG_CMD_SETGETDCR */
