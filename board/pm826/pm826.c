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

#include <common.h>
#include <ioports.h>
#include <mpc8260.h>

/*
 * I/O Port configuration table
 *
 * if conf is 1, then that port pin will be configured at boot time
 * according to the five values podr/pdir/ppar/psor/pdat for that entry
 */

const iop_conf_t iop_conf_tab[4][32] = {

    /* Port A configuration */
    {	/*	      conf ppar psor pdir podr pdat */
	/* PA31 */ {   1,   1,   1,   0,   0,   0   }, /* FCC1 COL */
	/* PA30 */ {   1,   1,   1,   0,   0,   0   }, /* FCC1 CRS */
	/* PA29 */ {   1,   1,   1,   1,   0,   0   }, /* FCC1 TXER */
	/* PA28 */ {   1,   1,   1,   1,   0,   0   }, /* FCC1 TXEN */
	/* PA27 */ {   1,   1,   1,   0,   0,   0   }, /* FCC1 RXDV */
	/* PA26 */ {   1,   1,   1,   0,   0,   0   }, /* FCC1 RXER */
	/* PA25 */ {   0,   0,   0,   1,   0,   0   }, /* PA25 */
	/* PA24 */ {   0,   0,   0,   1,   0,   0   }, /* PA24 */
	/* PA23 */ {   0,   0,   0,   1,   0,   0   }, /* PA23 */
	/* PA22 */ {   0,   0,   0,   1,   0,   0   }, /* PA22 */
	/* PA21 */ {   1,   1,   0,   1,   0,   0   }, /* FCC1 TXD3 */
	/* PA20 */ {   1,   1,   0,   1,   0,   0   }, /* FCC1 TXD2 */
	/* PA19 */ {   1,   1,   0,   1,   0,   0   }, /* FCC1 TXD1 */
	/* PA18 */ {   1,   1,   0,   1,   0,   0   }, /* FCC1 TXD0 */
	/* PA17 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 RXD0 */
	/* PA16 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 RXD1*/
	/* PA15 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 RXD2 */
	/* PA14 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 RXD3 */
	/* PA13 */ {   0,   0,   0,   1,   0,   0   }, /* PA13 */
	/* PA12 */ {   0,   0,   0,   1,   0,   0   }, /* PA12 */
	/* PA11 */ {   0,   0,   0,   1,   0,   0   }, /* PA11 */
	/* PA10 */ {   0,   0,   0,   1,   0,   0   }, /* PA10 */
	/* PA9  */ {   0,   1,   0,   1,   0,   0   }, /* PA9 */
	/* PA8  */ {   0,   1,   0,   0,   0,   0   }, /* PA8 */
	/* PA7  */ {   0,   0,   0,   1,   0,   0   }, /* PA7 */
	/* PA6  */ {   0,   0,   0,   1,   0,   0   }, /* PA6 */
	/* PA5  */ {   0,   0,   0,   1,   0,   0   }, /* PA5 */
	/* PA4  */ {   0,   0,   0,   1,   0,   0   }, /* PA4 */
	/* PA3  */ {   0,   0,   0,   1,   0,   0   }, /* PA3 */
	/* PA2  */ {   0,   0,   0,   1,   0,   0   }, /* PA2 */
	/* PA1  */ {   0,   0,   0,   1,   0,   0   }, /* PA1 */
	/* PA0  */ {   0,   0,   0,   1,   0,   0   }  /* PA0 */
    },

    /* Port B configuration */
    {   /*	      conf ppar psor pdir podr pdat */
	/* PB31 */ {   0,   1,   0,   1,   0,   0   }, /* PB31 */
	/* PB30 */ {   0,   1,   0,   0,   0,   0   }, /* PB30 */
	/* PB29 */ {   0,   1,   1,   1,   0,   0   }, /* PB29 */
	/* PB28 */ {   1,   1,   1,   1,   0,   0   }, /* SCC1 TXD */
	/* PB27 */ {   0,   1,   0,   0,   0,   0   }, /* PB27 */
	/* PB26 */ {   0,   1,   0,   0,   0,   0   }, /* PB26 */
	/* PB25 */ {   0,   1,   0,   1,   0,   0   }, /* PB25 */
	/* PB24 */ {   0,   1,   0,   1,   0,   0   }, /* PB24 */
	/* PB23 */ {   0,   1,   0,   1,   0,   0   }, /* PB23 */
	/* PB22 */ {   0,   1,   0,   1,   0,   0   }, /* PB22 */
	/* PB21 */ {   0,   1,   0,   0,   0,   0   }, /* PB21 */
	/* PB20 */ {   0,   1,   0,   0,   0,   0   }, /* PB20 */
	/* PB19 */ {   0,   1,   0,   0,   0,   0   }, /* PB19 */
	/* PB18 */ {   0,   1,   0,   0,   0,   0   }, /* PB18 */
	/* PB17 */ {   0,   0,   0,   0,   0,   0   }, /* PB17 */
	/* PB16 */ {   0,   0,   0,   0,   0,   0   }, /* PB16 */
	/* PB15 */ {   1,   1,   0,   0,   0,   0   }, /* SCC2 RXD */
	/* PB14 */ {   1,   1,   0,   0,   0,   0   }, /* SCC3 RXD */
	/* PB13 */ {   0,   0,   0,   0,   0,   0   }, /* PB13 */
	/* PB12 */ {   0,   0,   0,   0,   0,   0   }, /* PB12 */
	/* PB11 */ {   0,   0,   0,   0,   0,   0   }, /* PB11 */
	/* PB10 */ {   0,   0,   0,   0,   0,   0   }, /* PB10 */
	/* PB9  */ {   0,   0,   0,   0,   0,   0   }, /* PB9 */
	/* PB8  */ {   1,   1,   1,   1,   0,   0   }, /* SCC3 TXD */
	/* PB7  */ {   0,   0,   0,   0,   0,   0   }, /* PB7 */
	/* PB6  */ {   0,   0,   0,   0,   0,   0   }, /* PB6 */
	/* PB5  */ {   0,   0,   0,   0,   0,   0   }, /* PB5 */
	/* PB4  */ {   0,   0,   0,   0,   0,   0   }, /* PB4 */
	/* PB3  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB2  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB1  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB0  */ {   0,   0,   0,   0,   0,   0   }  /* pin doesn't exist */
    },

    /* Port C */
    {   /*	      conf ppar psor pdir podr pdat */
	/* PC31 */ {   0,   0,   0,   1,   0,   0   }, /* PC31 */
	/* PC30 */ {   0,   0,   0,   1,   0,   0   }, /* PC30 */
	/* PC29 */ {   0,   1,   1,   0,   0,   0   }, /* SCC1 CTS */
	/* PC28 */ {   0,   0,   0,   1,   0,   0   }, /* SCC2 CTS */
	/* PC27 */ {   0,   0,   0,   1,   0,   0   }, /* PC27 */
	/* PC26 */ {   0,   0,   0,   1,   0,   0   }, /* PC26 */
	/* PC25 */ {   0,   0,   0,   1,   0,   0   }, /* PC25 */
	/* PC24 */ {   0,   0,   0,   1,   0,   0   }, /* PC24 */
	/* PC23 */ {   0,   1,   0,   1,   0,   0   }, /* PC23 */
	/* PC22 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 TXCK */
	/* PC21 */ {   1,   1,   0,   0,   0,   0   }, /* FCC1 RXCK */
	/* PC20 */ {   0,   1,   0,   0,   0,   0   }, /* PC20 */
	/* PC19 */ {   0,   1,   0,   0,   0,   0   }, /* PC19 */
	/* PC18 */ {   0,   1,   0,   0,   0,   0   }, /* PC18 */
	/* PC17 */ {   0,   0,   0,   1,   0,   0   }, /* PC17 */
	/* PC16 */ {   0,   0,   0,   1,   0,   0   }, /* PC16 */
	/* PC15 */ {   1,   1,   0,   1,   0,   0   }, /* SMC2 TXD */
	/* PC14 */ {   0,   1,   0,   0,   0,   0   }, /* SCC1 DCD */
	/* PC13 */ {   0,   0,   0,   1,   0,   0   }, /* PC13 */
	/* PC12 */ {   0,   0,   0,   1,   0,   0   }, /* SCC2 DCD */
	/* PC11 */ {   0,   0,   0,   1,   0,   0   }, /* SCC3 CTS */
	/* PC10 */ {   0,   0,   0,   1,   0,   0   }, /* SCC3 DCD */
	/* PC9  */ {   0,   0,   0,   1,   0,   0   }, /* SCC4 CTS */
	/* PC8  */ {   0,   0,   0,   1,   0,   0   }, /* SCC4 DCD */
	/* PC7  */ {   0,   0,   0,   1,   0,   0   }, /* PC7 */
	/* PC6  */ {   0,   0,   0,   1,   0,   0   }, /* PC6 */
	/* PC5  */ {   0,   0,   0,   1,   0,   0   }, /* PC5 */
	/* PC4  */ {   0,   0,   0,   1,   0,   0   }, /* PC4 */
	/* PC3  */ {   0,   0,   0,   1,   0,   0   }, /* PC3 */
	/* PC2  */ {   0,   0,   0,   1,   0,   1   }, /* PC2 */
	/* PC1  */ {   0,   0,   0,   1,   0,   0   }, /* PC1 */
	/* PC0  */ {   0,   0,   0,   1,   0,   0   }, /* PC0 */
    },

    /* Port D */
    {   /*	      conf ppar psor pdir podr pdat */
	/* PD31 */ {   1,   1,   0,   0,   0,   0   }, /* SCC1 RXD */
	/* PD30 */ {   0,   1,   1,   1,   0,   0   }, /* PD30 */
	/* PD29 */ {   0,   1,   0,   1,   0,   0   }, /* SCC1 RTS */
	/* PD28 */ {   0,   0,   0,   1,   0,   0   }, /* PD28 */
	/* PD27 */ {   0,   1,   0,   1,   0,   0   }, /* SCC2 RTS */
	/* PD26 */ {   0,   0,   0,   1,   0,   0   }, /* PD26 */
	/* PD25 */ {   0,   0,   0,   1,   0,   0   }, /* PD25 */
	/* PD24 */ {   0,   0,   0,   1,   0,   0   }, /* PD24 */
	/* PD23 */ {   0,   0,   0,   1,   0,   0   }, /* SCC3 RTS */
	/* PD22 */ {   1,   1,   0,   0,   0,   0   }, /* SCC4 RXD */
	/* PD21 */ {   1,   1,   0,   1,   0,   0   }, /* SCC4 TXD */
	/* PD20 */ {   0,   0,   1,   1,   0,   0   }, /* SCC4 RTS */
	/* PD19 */ {   0,   0,   0,   1,   0,   0   }, /* PD19 */
	/* PD18 */ {   0,   0,   0,   1,   0,   0   }, /* PD18 */
	/* PD17 */ {   0,   1,   0,   0,   0,   0   }, /* PD17 */
	/* PD16 */ {   0,   1,   0,   1,   0,   0   }, /* PD16 */
#if defined(CONFIG_SOFT_I2C)
	/* PD15 */ {   1,   0,   0,   1,   1,   1   }, /* I2C SDA */
	/* PD14 */ {   1,   0,   0,   1,   1,   1   }, /* I2C SCL */
#else
#if defined(CONFIG_HARD_I2C)
	/* PD15 */ {   1,   1,   1,   0,   1,   0   }, /* I2C SDA */
	/* PD14 */ {   1,   1,   1,   0,   1,   0   }, /* I2C SCL */
#else /* normal I/O port pins */
	/* PD15 */ {   0,   1,   1,   0,   1,   0   }, /* I2C SDA */
	/* PD14 */ {   0,   1,   1,   0,   1,   0   }, /* I2C SCL */
#endif
#endif
	/* PD13 */ {   0,   0,   0,   0,   0,   0   }, /* PD13 */
	/* PD12 */ {   0,   0,   0,   0,   0,   0   }, /* PD12 */
	/* PD11 */ {   0,   0,   0,   0,   0,   0   }, /* PD11 */
	/* PD10 */ {   0,   0,   0,   0,   0,   0   }, /* PD10 */
	/* PD9  */ {   0,   1,   0,   1,   0,   0   }, /* PD9 */
	/* PD8  */ {   0,   1,   0,   0,   0,   0   }, /* PD8 */
	/* PD7  */ {   0,   0,   0,   1,   0,   1   }, /* PD7 */
	/* PD6  */ {   0,   0,   0,   1,   0,   1   }, /* PD6 */
	/* PD5  */ {   0,   0,   0,   1,   0,   1   }, /* PD5 */
	/* PD4  */ {   1,   1,   1,   0,   0,   0   }, /* SMC2 RXD */
	/* PD3  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD2  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD1  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD0  */ {   0,   0,   0,   0,   0,   0   }  /* pin doesn't exist */
    }
};

/* ------------------------------------------------------------------------- */

/* Check Board Identity:
 */
int checkboard (void)
{
	puts ("Board: PM826\n");
	return 0;
}

/* ------------------------------------------------------------------------- */


/* Try SDRAM initialization with P/LSDMR=sdmr and ORx=orx
 *
 * This routine performs standard 8260 initialization sequence
 * and calculates the available memory size. It may be called
 * several times to try different SDRAM configurations on both
 * 60x and local buses.
 */
static long int try_init (volatile memctl8260_t * memctl, ulong sdmr,
						  ulong orx, volatile uchar * base)
{
	volatile uchar c = 0xff;
	volatile ulong cnt, val;
	volatile ulong *addr;
	volatile uint *sdmr_ptr;
	volatile uint *orx_ptr;
	int i;
	ulong save[32];				/* to make test non-destructive */
	ulong maxsize;

	/* We must be able to test a location outsize the maximum legal size
	 * to find out THAT we are outside; but this address still has to be
	 * mapped by the controller. That means, that the initial mapping has
	 * to be (at least) twice as large as the maximum expected size.
	 */
	maxsize = (1 + (~orx | 0x7fff)) / 2;

	sdmr_ptr = &memctl->memc_psdmr;
	orx_ptr = &memctl->memc_or2;

	*orx_ptr = orx;

	/*
	 * Quote from 8260 UM (10.4.2 SDRAM Power-On Initialization, 10-35):
	 *
	 * "At system reset, initialization software must set up the
	 *  programmable parameters in the memory controller banks registers
	 *  (ORx, BRx, P/LSDMR). After all memory parameters are configured,
	 *  system software should execute the following initialization sequence
	 *  for each SDRAM device.
	 *
	 *  1. Issue a PRECHARGE-ALL-BANKS command
	 *  2. Issue eight CBR REFRESH commands
	 *  3. Issue a MODE-SET command to initialize the mode register
	 *
	 *  The initial commands are executed by setting P/LSDMR[OP] and
	 *  accessing the SDRAM with a single-byte transaction."
	 *
	 * The appropriate BRx/ORx registers have already been set when we
	 * get here. The SDRAM can be accessed at the address CFG_SDRAM_BASE.
	 */

	*sdmr_ptr = sdmr | PSDMR_OP_PREA;
	*base = c;

	*sdmr_ptr = sdmr | PSDMR_OP_CBRR;
	for (i = 0; i < 8; i++)
		*base = c;

	*sdmr_ptr = sdmr | PSDMR_OP_MRW;
	*(base + CFG_MRS_OFFS) = c;	/* setting MR on address lines */

	*sdmr_ptr = sdmr | PSDMR_OP_NORM | PSDMR_RFEN;
	*base = c;

	/*
	 * Check memory range for valid RAM. A simple memory test determines
	 * the actually available RAM size between addresses `base' and
	 * `base + maxsize'. Some (not all) hardware errors are detected:
	 * - short between address lines
	 * - short between data lines
	 */
	i = 0;
	for (cnt = maxsize / sizeof (long); cnt > 0; cnt >>= 1) {
		addr = (volatile ulong *) base + cnt;	/* pointer arith! */
		save[i++] = *addr;
		*addr = ~cnt;
	}

	addr = (volatile ulong *) base;
	save[i] = *addr;
	*addr = 0;

	if ((val = *addr) != 0) {
		*addr = save[i];
		return (0);
	}

	for (cnt = 1; cnt <= maxsize / sizeof (long); cnt <<= 1) {
		addr = (volatile ulong *) base + cnt;	/* pointer arith! */
		val = *addr;
		*addr = save[--i];
		if (val != ~cnt) {
			/* Write the actual size to ORx
			 */
			*orx_ptr = orx | ~(cnt * sizeof (long) - 1);
			return (cnt * sizeof (long));
		}
	}
	return (maxsize);
}


long int initdram (int board_type)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile memctl8260_t *memctl = &immap->im_memctl;

#ifndef CFG_RAMBOOT
	ulong size8, size9;
#endif
	ulong psize = 32 * 1024 * 1024;

	memctl->memc_psrt = CFG_PSRT;
	memctl->memc_mptpr = CFG_MPTPR;

#ifndef CFG_RAMBOOT
	size8 = try_init (memctl, CFG_PSDMR_8COL, CFG_OR2_8COL,
					  (uchar *) CFG_SDRAM_BASE);
	size9 = try_init (memctl, CFG_PSDMR_9COL, CFG_OR2_9COL,
					  (uchar *) CFG_SDRAM_BASE);

	if (size8 < size9) {
		psize = size9;
		printf ("(60x:9COL) ");
	} else {
		psize = try_init (memctl, CFG_PSDMR_8COL, CFG_OR2_8COL,
						  (uchar *) CFG_SDRAM_BASE);
		printf ("(60x:8COL) ");
	}
#endif
	return (psize);
}

#if (CONFIG_COMMANDS & CFG_CMD_DOC)
extern void doc_probe (ulong physadr);
void doc_init (void)
{
	doc_probe (CFG_DOC_BASE);
}
#endif
