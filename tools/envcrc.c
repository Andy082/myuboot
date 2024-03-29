/*
 * (C) Copyright 2001
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define	__ASSEMBLY__	/* Dirty trick to get only #defines */
#include <config.h>
#undef	__ASSEMBLY__

#if defined(CFG_ENV_IS_IN_FLASH)
# ifndef  CFG_ENV_ADDR
#  define CFG_ENV_ADDR	(CFG_FLASH_BASE + CFG_ENV_OFFSET)
# endif
# ifndef  CFG_ENV_OFFSET
#  define CFG_ENV_OFFSET (CFG_ENV_ADDR - CFG_FLASH_BASE)
# endif
# ifndef  CFG_ENV_SIZE
#  define CFG_ENV_SIZE	CFG_ENV_SECT_SIZE
# endif
# if ((CFG_ENV_ADDR >= CFG_MONITOR_BASE) && \
     ((CFG_ENV_ADDR+CFG_ENV_SIZE) <= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)))
#  define ENV_IS_EMBEDDED
# endif
#endif	/* CFG_ENV_IS_IN_FLASH */

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

#ifdef	ENV_IS_EMBEDDED
extern unsigned int env_size;
extern unsigned char environment;
#endif	/* ENV_IS_EMBEDDED */

int main (int argc, char **argv)
{
#ifdef	ENV_IS_EMBEDDED
    int crc ;
    unsigned char 	*envptr 	= &environment,
			*dataptr 	= envptr + sizeof(unsigned int) + 1;
    unsigned int	datasize 	= env_size - (dataptr - envptr) ;


    crc = crc32(0, dataptr, datasize) ;

    /* Check if verbose mode is activated passing a parameter to the program */
    if (argc > 1) {
	printf("CRC32 from offset %08X to %08X of environment = %08X\n",
	    (unsigned int)(dataptr - envptr),
	    (unsigned int)(dataptr - envptr) + datasize,
	    crc);
    } else {
	printf("0x%08X\n", crc);
    }
#else
	printf("0\n");
#endif
    return EXIT_SUCCESS;
}

