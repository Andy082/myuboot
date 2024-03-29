/*
 * (C) Copyright 2000-2002
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
 *
 ********************************************************************
 *
 * Lots of code copied from:
 *
 * m8xx_pcmcia.c - Linux PCMCIA socket driver for the mpc8xx series.
 * (C) 1999-2000 Magnus Damm <damm@bitsmart.com>
 *
 * "The ExCA standard specifies that socket controllers should provide
 * two IO and five memory windows per socket, which can be independently
 * configured and positioned in the host address space and mapped to
 * arbitrary segments of card address space. " - David A Hinds. 1999
 *
 * This controller does _not_ meet the ExCA standard.
 *
 * m8xx pcmcia controller brief info:
 * + 8 windows (attrib, mem, i/o)
 * + up to two slots (SLOT_A and SLOT_B)
 * + inputpins, outputpins, event and mask registers.
 * - no offset register. sigh.
 *
 * Because of the lacking offset register we must map the whole card.
 * We assign each memory window PCMCIA_MEM_WIN_SIZE address space.
 * Make sure there is (PCMCIA_MEM_WIN_SIZE * PCMCIA_MEM_WIN_NO
 * * PCMCIA_SOCKETS_NO) bytes at PCMCIA_MEM_WIN_BASE.
 * The i/o windows are dynamically allocated at PCMCIA_IO_WIN_BASE.
 * They are maximum 64KByte each...
 */

/* #define DEBUG	1	*/

/*
 * PCMCIA support
 */
#include <common.h>
#include <command.h>
#include <config.h>
#include <pcmcia.h>
#include <cmd_pcmcia.h>
#if defined(CONFIG_IDE_8xx_PCCARD) && defined(CONFIG_8xx)
#include <mpc8xx.h>
#endif
#if defined(CONFIG_LWMON)
#include <i2c.h>
#endif

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA) || \
    ((CONFIG_COMMANDS & CFG_CMD_IDE) && defined(CONFIG_IDE_8xx_PCCARD))

int pcmcia_on (void);

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int  pcmcia_off (void);
static int  hardware_disable(int slot);
#endif
static int  hardware_enable (int slot);
static int  voltage_set(int slot, int vcc, int vpp);
#ifdef CONFIG_IDE_8xx_PCCARD
static void print_funcid (int func);
static void print_fixed  (volatile uchar *p);
static int  identify     (volatile uchar *p);
static int  check_ide_device (void);
#endif	/* CONFIG_IDE_8xx_PCCARD */

static u_int m8xx_get_graycode(u_int size);
#if 0
static u_int m8xx_get_speed(u_int ns, u_int is_io);
#endif

/* ------------------------------------------------------------------------- */

/* look up table for pgcrx registers */

static u_int *pcmcia_pgcrx[2] = {
	&((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_pgcra,
	&((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_pgcrb,
};

#define PCMCIA_PGCRX(slot)	(*pcmcia_pgcrx[slot])

const char *indent = "\t   ";

/* ------------------------------------------------------------------------- */

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)

int do_pinit (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rcode = 0;

	if (argc != 2) {
		printf ("Usage: pinit {on | off}\n");
		return 1;
	}
	if (strcmp(argv[1],"on") == 0) {
	     	rcode = pcmcia_on ();
	} else if (strcmp(argv[1],"off") == 0) {
		rcode = pcmcia_off ();
	} else {
		printf ("Usage: pinit {on | off}\n");
		return 1;
	}

	return rcode;
}
#endif	/* CFG_CMD_PCMCIA */

/* ------------------------------------------------------------------------- */

#if defined(CONFIG_LWMON)
# define  CFG_PCMCIA_TIMING	(PCMCIA_SHT(9) | PCMCIA_SST(3) | PCMCIA_SL(12))
#else
# define  CFG_PCMCIA_TIMING	(PCMCIA_SHT(2) | PCMCIA_SST(4) | PCMCIA_SL(9))
#endif

int pcmcia_on (void)
{
	int i;
	u_long reg, base;
	pcmcia_win_t *win;

	debug ("Enable PCMCIA " PCMCIA_SLOT_MSG "\n");

	/* intialize the fixed memory windows */
	win = (pcmcia_win_t *)(&((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_pbr0);
	base = CFG_PCMCIA_MEM_ADDR;

	if((reg = m8xx_get_graycode(CFG_PCMCIA_MEM_SIZE)) == -1) {
		printf ("Cannot set window size to 0x%08x\n",
			CFG_PCMCIA_MEM_SIZE);
		return (1);
	}

	for (i=0; i<PCMCIA_MEM_WIN_NO; ++i) {
		win->br = base;

		switch (i) {
#ifdef CONFIG_IDE_8xx_PCCARD
		case 0:	{	/* map attribute memory */
			win->or = (	PCMCIA_BSIZE_64M
				|	PCMCIA_PPS_8
				|	PCMCIA_PRS_ATTR
				|	PCMCIA_SLOT_x
				|	PCMCIA_PV
				|	CFG_PCMCIA_TIMING );
			break;
		    }

		case 1: {	/* map I/O window for data reg */
			win->or = (	PCMCIA_BSIZE_1K
				|	PCMCIA_PPS_16
				|	PCMCIA_PRS_IO
				|	PCMCIA_SLOT_x
				|	PCMCIA_PV
				|	CFG_PCMCIA_TIMING );
			break;
		    }

		case 2: {	/* map I/O window for command/ctrl reg block */
			win->or = (	PCMCIA_BSIZE_1K
				|	PCMCIA_PPS_8
				|	PCMCIA_PRS_IO
				|	PCMCIA_SLOT_x
				|	PCMCIA_PV
				|	CFG_PCMCIA_TIMING );
			break;
		    }
#endif	/* CONFIG_IDE_8xx_PCCARD */
		default:	/* set to not valid */
			win->or = 0;
			break;
		}

		debug ("MemWin %d: PBR 0x%08lX  POR %08lX\n",
			i, win->br, win->or);
		base += CFG_PCMCIA_MEM_SIZE;
		++win;
	}

	/* turn off voltage */
	if (voltage_set(_slot_, 0, 0))
		return (1);

	/* Enable external hardware */
	if (hardware_enable(_slot_))
		return (1);

#ifdef CONFIG_IDE_8xx_PCCARD
	if (check_ide_device())
		return (1);
#endif
	return (0);
}

/* ------------------------------------------------------------------------- */

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)

static int pcmcia_off (void)
{
	int i;
	pcmcia_win_t *win;

	printf ("Disable PCMCIA " PCMCIA_SLOT_MSG "\n");

	/* clear interrupt state, and disable interrupts */
	((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_pscr =  PCMCIA_MASK(_slot_);
	((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* turn off interrupt and disable CxOE */
	PCMCIA_PGCRX(_slot_) = __MY_PCMCIA_GCRX_CXOE;

	/* turn off memory windows */
	win = (pcmcia_win_t *)(&((immap_t *)CFG_IMMR)->im_pcmcia.pcmc_pbr0);

	for (i=0; i<PCMCIA_MEM_WIN_NO; ++i) {
		/* disable memory window */
		win->or = 0;
		++win;
	}

	/* turn off voltage */
	voltage_set(_slot_, 0, 0);

	/* disable external hardware */
	printf ("Shutdown and Poweroff " PCMCIA_SLOT_MSG "\n");
	hardware_disable(_slot_);
	return 0;
}

#endif	/* CFG_CMD_PCMCIA */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_8xx_PCCARD

#define	MAX_TUPEL_SZ	512
#define MAX_FEATURES	4

static int check_ide_device (void)
{
	volatile uchar *ident = NULL;
	volatile uchar *feature_p[MAX_FEATURES];
	volatile uchar *p, *start;
	int n_features = 0;
	uchar func_id = ~0;
	uchar code, len;
	ushort config_base = 0;
	int found = 0;
	int i;

	debug ("PCMCIA MEM: %08X\n", CFG_PCMCIA_MEM_ADDR);

	start = p = (volatile uchar *) CFG_PCMCIA_MEM_ADDR;

	while ((p - start) < MAX_TUPEL_SZ) {

		code = *p; p += 2;

		if (code == 0xFF) { /* End of chain */
			break;
		}

		len = *p; p += 2;
#if defined(DEBUG) && (DEBUG > 1)
		{ volatile uchar *q = p;
			printf ("\nTuple code %02x  length %d\n\tData:",
				code, len);

			for (i = 0; i < len; ++i) {
				printf (" %02x", *q);
				q+= 2;
			}
		}
#endif	/* DEBUG */
		switch (code) {
		case CISTPL_VERS_1:
			ident = p + 4;
			break;
		case CISTPL_FUNCID:
			/* Fix for broken SanDisk which may have 0x80 bit set */
			func_id = *p & 0x7F;
			break;
		case CISTPL_FUNCE:
			if (n_features < MAX_FEATURES)
				feature_p[n_features++] = p;
			break;
		case CISTPL_CONFIG:
			config_base = (*(p+6) << 8) + (*(p+4));
			debug ("\n## Config_base = %04x ###\n", config_base);
		default:
			break;
		}
		p += 2 * len;
	}

	found = identify (ident);

	if (func_id != ((uchar)~0)) {
		print_funcid (func_id);

		if (func_id == CISTPL_FUNCID_FIXED)
			found = 1;
		else
			return (1);	/* no disk drive */
	}

	for (i=0; i<n_features; ++i) {
		print_fixed (feature_p[i]);
	}

	if (!found) {
		printf ("unknown card type\n");
		return (1);
	}

	/* set I/O area in config reg -> only valid for ARGOSY D5!!! */
	*((uchar *)(CFG_PCMCIA_MEM_ADDR + config_base)) = 1;

	return (0);
}
#endif	/* CONFIG_IDE_8xx_PCCARD */

/* ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/* board specific stuff:							*/
/* voltage_set(), hardware_enable() and hardware_disable()			*/
/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/* RPX Boards from Embedded Planet						*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_RPXCLASSIC) || defined(CONFIG_RPXLITE)

/* The RPX boards seems to have it's bus monitor timeout set to 6*8 clocks.
 * SYPCR is write once only, therefore must the slowest memory be faster
 * than the bus monitor or we will get a machine check due to the bus timeout.
 */

#define PCMCIA_BOARD_MSG "RPX CLASSIC or RPX LITE"

#undef PCMCIA_BMT_LIMIT
#define PCMCIA_BMT_LIMIT (6*8)

static int voltage_set(int slot, int vcc, int vpp)
{
	u_long reg = 0;

	switch(vcc) {
	case 0: break;
	case 33: reg |= BCSR1_PCVCTL4; break;
	case 50: reg |= BCSR1_PCVCTL5; break;
	default: return 1;
	}

	switch(vpp) {
	case 0: break;
	case 33:
	case 50:
		if(vcc == vpp)
			reg |= BCSR1_PCVCTL6;
		else
			return 1;
		break;
	case 120:
		reg |= BCSR1_PCVCTL7;
	default: return 1;
	}

	if(vcc == 120)
	   return 1;

	/* first, turn off all power */

	*((uint *)RPX_CSR_ADDR) &= ~(BCSR1_PCVCTL4 | BCSR1_PCVCTL5
				     | BCSR1_PCVCTL6 | BCSR1_PCVCTL7);

	/* enable new powersettings */

	*((uint *)RPX_CSR_ADDR) |= reg;

	return 0;
}

#define socket_get(_slot_) PCMCIA_SOCKET_KEY_5V
static int hardware_enable (int slot)
{
	return 0;	/* No hardware to enable */
}
#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	return 0;	/* No hardware to disable */
}
#endif	/* CFG_CMD_PCMCIA */
#endif	/* CONFIG_RPXCLASSIC */

/* ---------------------------------------------------------------------------- */
/* (F)ADS Boards from Motorola							*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_ADS) || defined(CONFIG_FADS)

#ifdef CONFIG_ADS
#define PCMCIA_BOARD_MSG "ADS"
#define PCMCIA_GLITCHY_CD  /* My ADS board needs this */
#else
#define PCMCIA_BOARD_MSG "FADS"
#endif

static int voltage_set(int slot, int vcc, int vpp)
{
	u_long reg = 0;

	switch(vpp) {
	case 0: reg = 0; break;
	case 50: reg = 1; break;
	case 120: reg = 2; break;
	default: return 1;
	}

	switch(vcc) {
	case 0: reg = 0; break;
#ifdef CONFIG_ADS
	case 50: reg = BCSR1_PCCVCCON; break;
#endif
#ifdef CONFIG_FADS
	case 33: reg = BCSR1_PCCVCC0 | BCSR1_PCCVCC1; break;
	case 50: reg = BCSR1_PCCVCC1; break;
#endif
	default: return 1;
	}

	/* first, turn off all power */

#ifdef CONFIG_ADS
	*((uint *)BCSR1) |= BCSR1_PCCVCCON;
#endif
#ifdef CONFIG_FADS
	*((uint *)BCSR1) &= ~(BCSR1_PCCVCC0 | BCSR1_PCCVCC1);
#endif
	*((uint *)BCSR1) &= ~BCSR1_PCCVPP_MASK;

	/* enable new powersettings */

#ifdef CONFIG_ADS
	*((uint *)BCSR1) &= ~reg;
#endif
#ifdef CONFIG_FADS
	*((uint *)BCSR1) |= reg;
#endif

 	*((uint *)BCSR1) |= reg << 20;

	return 0;
}

#define socket_get(_slot_) PCMCIA_SOCKET_KEY_5V

static int hardware_enable(int slot)
{
	*((uint *)BCSR1) &= ~BCSR1_PCCEN;
	return 0;
}

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	*((uint *)BCSR1) &= ~BCSR1_PCCEN;
	return 0;
}
#endif	/* CFG_CMD_PCMCIA */

#endif	/* (F)ADS */

/* ---------------------------------------------------------------------------- */
/* TQM8xxL Boards by TQ Components						*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_TQM8xxL)

#define PCMCIA_BOARD_MSG "TQM8xxL"


static int hardware_enable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	volatile sysconf8xx_t	*sysp;
	uint reg, mask;

	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	udelay(10000);

	immap = (immap_t *)CFG_IMMR;
	sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/*
	 * Configure SIUMCR to enable PCMCIA port B
	 * (VFLS[0:1] are not used for debugging, we connect FRZ# instead)
	 */
	sysp->sc_siumcr &= ~SIUMCR_DBGC11;	/* set DBGC to 00 */

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX(_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Port C pins for
	 * 5 Volts Enable and 3 Volts enable
	 */
	immap->im_ioport.iop_pcpar &= ~(0x0002 | 0x0004);
	immap->im_ioport.iop_pcso  &= ~(0x0002 | 0x0004);
	/* remove all power */

	immap->im_ioport.iop_pcdat &= ~(0x0002 | 0x0004);

	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay(10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On.
	 */
	mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
	reg  = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		reg,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");
	if ((reg & mask) == mask) {
		immap->im_ioport.iop_pcdat |= 0x0004;
		puts (" 5.0V card found: ");
	} else {
		immap->im_ioport.iop_pcdat |= 0x0002;
		puts (" 3.3V card found: ");
	}
	immap->im_ioport.iop_pcdir |=  (0x0002 | 0x0004);
#if 0
	/*  VCC switch error flag, PCMCIA slot INPACK_ pin */
	cp->cp_pbdir &= ~(0x0020 | 0x0010);
	cp->cp_pbpar &= ~(0x0020 | 0x0010);
	udelay(500000);
#endif
	udelay(1000);
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}



#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("hardware_disable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));

	/* remove all power */
	immap->im_ioport.iop_pcdat &= ~(0x0002 | 0x0004);

	/* Configure PCMCIA General Control Register */
	PCMCIA_PGCRX(_slot_) = 0;

	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(10000);

	return (0);
}
#endif	/* CFG_CMD_PCMCIA */



static int voltage_set(int slot, int vcc, int vpp)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("voltage_set: "
		PCMCIA_BOARD_MSG
		" Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		'A'+slot, vcc/10, vcc%10, vpp/10, vcc%10);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Port C pins for
	 * 5 Volts Enable and 3 Volts enable,
	 * Turn off all power
	 */
	debug ("PCMCIA power OFF\n");
	immap->im_ioport.iop_pcpar &= ~(0x0002 | 0x0004);
	immap->im_ioport.iop_pcso  &= ~(0x0002 | 0x0004);
	immap->im_ioport.iop_pcdat &= ~(0x0002 | 0x0004);

	reg = 0;
	switch(vcc) {
	case  0: 		break;
	case 33: reg |= 0x0002;	break;
	case 50: reg |= 0x0004;	break;
	default: 		goto done;
	}

	/* Checking supported voltages */

	debug ("PIPR: 0x%x --> %s\n",
		pcmp->pcmc_pipr,
		(pcmp->pcmc_pipr & 0x00008000) ? "only 5 V" : "can do 3.3V");

	immap->im_ioport.iop_pcdat |= reg;
	immap->im_ioport.iop_pcdir |=  (0x0002 | 0x0004);
	if (reg) {
		debug ("PCMCIA powered at %sV\n",
			(reg&0x0004) ? "5.0" : "3.3");
	} else {
		debug ("PCMCIA powered down\n");
	}

done:
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	debug ("voltage_set: " PCMCIA_BOARD_MSG " Slot %c, DONE\n",
		slot+'A');
	return (0);
}

#endif	/* TQM8xxL */


/* ---------------------------------------------------------------------------- */
/* LWMON Board									*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_LWMON)

#define PCMCIA_BOARD_MSG "LWMON"

/* #define's for MAX1604 Power Switch */
#define MAX1604_OP_SUS		0x80
#define MAX1604_VCCBON		0x40
#define MAX1604_VCC_35		0x20
#define MAX1604_VCCBHIZ		0x10
#define MAX1604_VPPBON		0x08
#define MAX1604_VPPBPBPGM	0x04
#define MAX1604_VPPBHIZ		0x02
/* reserved			0x01	*/

static int hardware_enable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	volatile sysconf8xx_t	*sysp;
	uint reg, mask;
	uchar val;


	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	/* Switch on PCMCIA port in PIC register 0x60 */
	reg = pic_read  (0x60);
	debug ("[%d] PIC read: reg_60 = 0x%02x\n", __LINE__, reg);
	reg &= ~0x10;
	/* reg |=  0x08; Vpp not needed */
	pic_write (0x60, reg);
#ifdef DEBUG
	reg = pic_read  (0x60);
	printf ("[%d] PIC read: reg_60 = 0x%02x\n", __LINE__, reg);
#endif
	udelay(10000);

	immap = (immap_t *)CFG_IMMR;
	sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/*
	 * Configure SIUMCR to enable PCMCIA port B
	 * (VFLS[0:1] are not used for debugging, we connect FRZ# instead)
	 */
	sysp->sc_siumcr &= ~SIUMCR_DBGC11;	/* set DBGC to 00 */

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX(_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay(10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On.
	 */
	mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
	reg  = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		reg,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");
	if ((reg & mask) == mask) {
		val = 0;		/* VCCB3/5 = 0 ==> use Vx = 5.0 V */
		puts (" 5.0V card found: ");
	} else {
		val = MAX1604_VCC_35;	/* VCCB3/5 = 1 ==> use Vy = 3.3 V */
		puts (" 3.3V card found: ");
	}

	/*  switch VCC on */
	val |=  MAX1604_OP_SUS | MAX1604_VCCBON;
	i2c_init  (CFG_I2C_SPEED, CFG_I2C_SLAVE);
	i2c_write (CFG_I2C_POWER_A_ADDR, 0, 0, &val, 1);

	udelay(500000);

	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}



#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;
	uchar val;

	debug ("hardware_disable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));

	/* remove all power, put output in high impedance state */
	val  = MAX1604_VCCBHIZ | MAX1604_VPPBHIZ;
	i2c_init  (CFG_I2C_SPEED, CFG_I2C_SLAVE);
	i2c_write (CFG_I2C_POWER_A_ADDR, 0, 0, &val, 1);

	/* Configure PCMCIA General Control Register */
	PCMCIA_PGCRX(_slot_) = 0;

	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	/* Switch off PCMCIA port in PIC register 0x60 */
	reg = pic_read  (0x60);
	debug ("[%d] PIC read: reg_60 = 0x%02x\n", __LINE__, reg);
	reg |=  0x10;
	reg &= ~0x08;
	pic_write (0x60, reg);
#ifdef DEBUG
	reg = pic_read  (0x60);
	printf ("[%d] PIC read: reg_60 = 0x%02x\n", __LINE__, reg);
#endif
	udelay(10000);

	return (0);
}
#endif	/* CFG_CMD_PCMCIA */



static int voltage_set(int slot, int vcc, int vpp)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;
	uchar val;

	debug ("voltage_set: "
		PCMCIA_BOARD_MSG
		" Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		'A'+slot, vcc/10, vcc%10, vpp/10, vcc%10);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Turn off all power (switch to high impedance)
	 */
	debug ("PCMCIA power OFF\n");
	val  = MAX1604_VCCBHIZ | MAX1604_VPPBHIZ;
	i2c_init  (CFG_I2C_SPEED, CFG_I2C_SLAVE);
	i2c_write (CFG_I2C_POWER_A_ADDR, 0, 0, &val, 1);

	val = 0;
	switch(vcc) {
	case  0: 			break;
	case 33: val = MAX1604_VCC_35;	break;
	case 50: 			break;
	default: 			goto done;
	}

	/* Checking supported voltages */

	debug ("PIPR: 0x%x --> %s\n",
		pcmp->pcmc_pipr,
		(pcmp->pcmc_pipr & 0x00008000) ? "only 5 V" : "can do 3.3V");

	i2c_write (CFG_I2C_POWER_A_ADDR, 0, 0, &val, 1);
	if (val) {
		debug ("PCMCIA powered at %sV\n",
			(val & MAX1604_VCC_35) ? "3.3" : "5.0");
	} else {
		debug ("PCMCIA powered down\n");
	}

done:
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	debug ("voltage_set: " PCMCIA_BOARD_MSG " Slot %c, DONE\n",
		slot+'A');
	return (0);
}

#endif	/* LWMON */

/* ---------------------------------------------------------------------------- */
/* GTH board by Corelatus AB                                                    */
/* ---------------------------------------------------------------------------- */
#if defined(CONFIG_GTH)

#define PCMCIA_BOARD_MSG "GTH COMPACT FLASH"

static int voltage_set(int slot, int vcc, int vpp)
{  /* Do nothing */
  return 0;
}

static int hardware_enable (int slot)
{
  volatile immap_t	*immap;
  volatile cpm8xx_t	*cp;
  volatile pcmconf8xx_t	*pcmp;
  volatile sysconf8xx_t	*sysp;
  uint reg, mask;

  debug ("hardware_enable: GTH Slot %c\n", 'A'+slot);

  immap = (immap_t *)CFG_IMMR;
  sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
  pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
  cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

  /* clear interrupt state, and disable interrupts */
  pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
  pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

  /* disable interrupts & DMA */
  PCMCIA_PGCRX(_slot_) = 0;

  /*
   * Disable PCMCIA buffers (isolate the interface)
   * and assert RESET signal
   */
  debug ("Disable PCMCIA buffers and assert RESET\n");
  reg  =  PCMCIA_PGCRX(_slot_);
  reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
  reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
  PCMCIA_PGCRX(_slot_) = reg;
  udelay(500);

  /*
   * Make sure there is a card in the slot, then configure the interface.
   */
  udelay(10000);
  debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
  if (pcmp->pcmc_pipr & 0x98000000) {
    printf ("   No Card found\n");
    return (1);
  }

  mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
  reg  = pcmp->pcmc_pipr;
  debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		reg,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");

  debug ("Enable PCMCIA buffers and stop RESET\n");
  reg  =  PCMCIA_PGCRX(_slot_);
  reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
  reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
  PCMCIA_PGCRX(_slot_) = reg;

  udelay(250000);	/* some cards need >150 ms to come up :-( */

  debug ("# hardware_enable done\n");

  return 0;
}
#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	return 0;	/* No hardware to disable */
}
#endif	/* CFG_CMD_PCMCIA */
#endif	/* CONFIG_GTH */

/* ---------------------------------------------------------------------------- */
/* ICU862 Boards by Cambridge Broadband Ltd.					*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_ICU862)

#define PCMCIA_BOARD_MSG "ICU862"

static void cfg_port_B (void);

static int hardware_enable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	volatile sysconf8xx_t	*sysp;
	uint reg, pipr, mask;
	int i;

	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	udelay(10000);

	immap = (immap_t *)CFG_IMMR;
	sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/* Configure Port B for TPS2205 PC-Card Power-Interface Switch */
	cfg_port_B ();

	/*
	 * Configure SIUMCR to enable PCMCIA port B
	 * (VFLS[0:1] are not used for debugging, we connect FRZ# instead)
	 */
	sysp->sc_siumcr &= ~SIUMCR_DBGC11;	/* set DBGC to 00 */

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX(_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay(10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On: Set VAVCC to 3.3V or 5V, set VAVPP to Hi-Z
	 */
	mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
	pipr = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		pipr,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");

	reg  = cp->cp_pbdat;
	if ((pipr & mask) == mask) {
		reg |=  (TPS2205_VPP_PGM | TPS2205_VPP_VCC |	/* VAVPP => Hi-Z */
			 TPS2205_VCC3);				/* 3V off	*/
		reg &= ~(TPS2205_VCC5);				/* 5V on	*/
		puts (" 5.0V card found: ");
	} else {
		reg |=  (TPS2205_VPP_PGM | TPS2205_VPP_VCC |	/* VAVPP => Hi-Z */
			 TPS2205_VCC5);				/* 5V off	*/
		reg &= ~(TPS2205_VCC3);				/* 3V on	*/
		puts (" 3.3V card found: ");
	}

	debug ("\nPB DAT: %08x -> 3.3V %s 5.0V %s VPP_PGM %s VPP_VCC %s\n",
		reg,
		(reg & TPS2205_VCC3)    ? "off" : "on",
		(reg & TPS2205_VCC5)    ? "off" : "on",
		(reg & TPS2205_VPP_PGM) ? "off" : "on",
		(reg & TPS2205_VPP_VCC) ? "off" : "on" );

	cp->cp_pbdat = reg;

	/*  Wait 500 ms; use this to check for over-current */
	for (i=0; i<5000; ++i) {
		if ((cp->cp_pbdat & TPS2205_OC) == 0) {
			printf ("   *** Overcurrent - Safety shutdown ***\n");
			cp->cp_pbdat &= ~(TPS2205_SHDN);
			return (1);
		}
		udelay (100);
	}

	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}



#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("hardware_disable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	immap = (immap_t *)CFG_IMMR;
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));

	/* Shut down */
	cp->cp_pbdat &= ~(TPS2205_SHDN);

	/* Configure PCMCIA General Control Register */
	PCMCIA_PGCRX(_slot_) = 0;

	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(10000);

	return (0);
}
#endif	/* CFG_CMD_PCMCIA */



static int voltage_set(int slot, int vcc, int vpp)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("voltage_set: "
		PCMCIA_BOARD_MSG
		" Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		'A'+slot, vcc/10, vcc%10, vpp/10, vcc%10);

	immap = (immap_t *)CFG_IMMR;
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Port C pins for
	 * 5 Volts Enable and 3 Volts enable,
	 * Turn all power pins to Hi-Z
	 */
	debug ("PCMCIA power OFF\n");
	cfg_port_B ();	/* Enables switch, but all in Hi-Z */

	reg  = cp->cp_pbdat;

	switch(vcc) {
	case  0: 			break;	/* Switch off		*/
	case 33: reg &= ~TPS2205_VCC3;	break;	/* Switch on 3.3V	*/
	case 50: reg &= ~TPS2205_VCC5;	break;	/* Switch on 5.0V	*/
	default: 			goto done;
	}

	/* Checking supported voltages */

	debug ("PIPR: 0x%x --> %s\n",
		pcmp->pcmc_pipr,
		(pcmp->pcmc_pipr & 0x00008000) ? "only 5 V" : "can do 3.3V");

	cp->cp_pbdat = reg;

#ifdef DEBUG
    {
	char *s;

	if ((reg & TPS2205_VCC3) == 0) {
		s = "at 3.3V";
	} else if ((reg & TPS2205_VCC5) == 0) {
		s = "at 5.0V";
	} else {
		s = "down";
	}
	printf ("PCMCIA powered %s\n", s);
    }
#endif

done:
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	debug ("voltage_set: " PCMCIA_BOARD_MSG " Slot %c, DONE\n",
		slot+'A');
	return (0);
}

static void cfg_port_B (void)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	uint reg;

	immap = (immap_t *)CFG_IMMR;
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/*
	 * Configure Port B for TPS2205 PC-Card Power-Interface Switch
	 *
	 * Switch off all voltages, assert shutdown
	 */
	reg  = cp->cp_pbdat;
	reg |=  (TPS2205_VPP_PGM | TPS2205_VPP_VCC |	/* VAVPP => Hi-Z */
		 TPS2205_VCC3    | TPS2205_VCC5    |	/* VAVCC => Hi-Z */
		 TPS2205_SHDN);				/* enable switch */
	cp->cp_pbdat = reg;

	cp->cp_pbpar &= ~(TPS2205_INPUTS | TPS2205_OUTPUTS);

	reg = cp->cp_pbdir & ~(TPS2205_INPUTS);
	cp->cp_pbdir = reg | TPS2205_OUTPUTS;

	debug ("Set Port B: PAR: %08x DIR: %08x DAT: %08x\n",
		cp->cp_pbpar, cp->cp_pbdir, cp->cp_pbdat);
}

#endif	/* ICU862 */


/* ---------------------------------------------------------------------------- */
/* C2MON Boards by TTTech Computertechnik AG					*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_C2MON)

#define PCMCIA_BOARD_MSG "C2MON"

static void cfg_ports (void);

static int hardware_enable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	volatile sysconf8xx_t	*sysp;
	uint reg, pipr, mask;
	ushort sreg;
	int i;

	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	udelay(10000);

	immap = (immap_t *)CFG_IMMR;
	sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/* Configure Ports for TPS2211A PC-Card Power-Interface Switch */
	cfg_ports ();

	/*
	 * Configure SIUMCR to enable PCMCIA port B
	 * (VFLS[0:1] are not used for debugging, we connect FRZ# instead)
	 */
	sysp->sc_siumcr &= ~SIUMCR_DBGC11;	/* set DBGC to 00 */

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX(_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay(10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On: Set VAVCC to 3.3V or 5V, set VAVPP to Hi-Z
	 */
	mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
	pipr = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		pipr,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");

	sreg = immap->im_ioport.iop_pcdat;
	if ((pipr & mask) == mask) {
		sreg |=  (TPS2211_VPPD0 | TPS2211_VPPD1 |	/* VAVPP => Hi-Z */
			  TPS2211_VCCD1);			/* 5V on	*/
		sreg &= ~(TPS2211_VCCD0);			/* 3V off	*/
		puts (" 5.0V card found: ");
	} else {
		sreg |=  (TPS2211_VPPD0 | TPS2211_VPPD1 |	/* VAVPP => Hi-Z */
			  TPS2211_VCCD0);			/* 3V on	*/
		sreg &= ~(TPS2211_VCCD1);			/* 5V off	*/
		puts (" 3.3V card found: ");
	}

	debug ("\nPC DAT: %04x -> 3.3V %s 5.0V %s\n",
		sreg,
		( (sreg & TPS2211_VCCD0) && !(sreg & TPS2211_VCCD1)) ? "on" : "off",
		(!(sreg & TPS2211_VCCD0) &&  (sreg & TPS2211_VCCD1)) ? "on" : "off"
	);

	immap->im_ioport.iop_pcdat = sreg;

	/*  Wait 500 ms; use this to check for over-current */
	for (i=0; i<5000; ++i) {
		if ((cp->cp_pbdat & TPS2211_OC) == 0) {
		    printf ("   *** Overcurrent - Safety shutdown ***\n");
		    immap->im_ioport.iop_pcdat &= ~(TPS2211_VCCD0|TPS2211_VCCD1);
		    return (1);
		}
		udelay (100);
	}

	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}



#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("hardware_disable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	immap = (immap_t *)CFG_IMMR;
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));

	/* Configure PCMCIA General Control Register */
	PCMCIA_PGCRX(_slot_) = 0;

	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	/* ALl voltages off / Hi-Z */
	immap->im_ioport.iop_pcdat |= (TPS2211_VPPD0 | TPS2211_VPPD1 |
				       TPS2211_VCCD0 | TPS2211_VCCD1 );

	udelay(10000);

	return (0);
}
#endif	/* CFG_CMD_PCMCIA */



static int voltage_set(int slot, int vcc, int vpp)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;
	ushort sreg;

	debug ("voltage_set: "
		PCMCIA_BOARD_MSG
		" Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		'A'+slot, vcc/10, vcc%10, vpp/10, vcc%10);

	immap = (immap_t *)CFG_IMMR;
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Port C pins for
	 * 5 Volts Enable and 3 Volts enable,
	 * Turn all power pins to Hi-Z
	 */
	debug ("PCMCIA power OFF\n");
	cfg_ports ();	/* Enables switch, but all in Hi-Z */

	sreg  = immap->im_ioport.iop_pcdat;
	sreg |= TPS2211_VPPD0 | TPS2211_VPPD1;		/* VAVPP always Hi-Z */

	switch(vcc) {
	case  0: 			break;	/* Switch off		*/
	case 33: sreg |=  TPS2211_VCCD0;	/* Switch on 3.3V	*/
		 sreg &= ~TPS2211_VCCD1;
		 			break;
	case 50: sreg &= ~TPS2211_VCCD0;	/* Switch on 5.0V	*/
		 sreg |=  TPS2211_VCCD1;
		 			break;
	default: 			goto done;
	}

	/* Checking supported voltages */

	debug ("PIPR: 0x%x --> %s\n",
		pcmp->pcmc_pipr,
		(pcmp->pcmc_pipr & 0x00008000) ? "only 5 V" : "can do 3.3V");

	immap->im_ioport.iop_pcdat = sreg;

#ifdef DEBUG
    {
	char *s;

	if ((sreg & TPS2211_VCCD0) && !(sreg & TPS2211_VCCD1)) {
		s = "at 3.3V";
	} else if (!(sreg & TPS2211_VCCD0) &&  (sreg & TPS2211_VCCD1)) {
		s = "at 5.0V";
	} else {
		s = "down";
	}
	printf ("PCMCIA powered %s\n", s);
    }
#endif

done:
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	debug ("voltage_set: " PCMCIA_BOARD_MSG " Slot %c, DONE\n",
		slot+'A');
	return (0);
}

static void cfg_ports (void)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	ushort sreg;

	immap = (immap_t *)CFG_IMMR;
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/*
	 * Configure Port C for TPS2211 PC-Card Power-Interface Switch
	 *
	 * Switch off all voltages, assert shutdown
	 */
	sreg = immap->im_ioport.iop_pcdat;
	sreg |=  (TPS2211_VPPD0 | TPS2211_VPPD1);	/* VAVPP => Hi-Z */
	sreg &= ~(TPS2211_VCCD0 | TPS2211_VCCD1);	/* 3V and 5V off */
	immap->im_ioport.iop_pcdat = sreg;

	immap->im_ioport.iop_pcpar &= ~(TPS2211_OUTPUTS);
	immap->im_ioport.iop_pcdir |=   TPS2211_OUTPUTS;

	debug ("Set Port C: PAR:     %04x DIR:     %04x DAT:     %04x\n",
		immap->im_ioport.iop_pcpar,
		immap->im_ioport.iop_pcdir,
		immap->im_ioport.iop_pcdat);

	/*
	 * Configure Port B for TPS2211 PC-Card Power-Interface Switch
	 *
	 * Over-Current Input only
	 */
	cp->cp_pbpar &= ~(TPS2211_INPUTS);
	cp->cp_pbdir &= ~(TPS2211_INPUTS);

	debug ("Set Port B: PAR: %08x DIR: %08x DAT: %08x\n",
		cp->cp_pbpar, cp->cp_pbdir, cp->cp_pbdat);
}

#endif	/* C2MON */

/* ----------------------------------------------------------------------------
   MBX board from Morotola	
   ---------------------------------------------------------------------------- */

#if defined( CONFIG_MBX )
#include <../board/mbx8xx/csr.h>

/* A lot of this has been taken from the RPX code in this file it works from me.
   I have added the voltage selection for the MBX board. */

/* MBX voltage bit in control register #2 */
#define CR2_VPP12       ((uchar)0x10)
#define CR2_VPPVDD      ((uchar)0x20)
#define CR2_VDD5        ((uchar)0x40)
#define CR2_VDD3        ((uchar)0x80)

#define PCMCIA_BOARD_MSG "MBX860"

static int voltage_set (int slot, int vcc, int vpp)
{
	uchar reg = 0;

	debug ("voltage_set: PCMCIA_BOARD_MSG Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		 'A' + slot, vcc / 10, vcc % 10, vpp / 10, vcc % 10);

	switch (vcc) {
	case 0:
		break;
	case 33:
		reg |= CR2_VDD3;
		break;
	case 50:
		reg |= CR2_VDD5;
		break;
	default:
		return 1;
	}

	switch (vpp) {
	case 0:
		break;
	case 33:
	case 50:
		if (vcc == vpp) {
			reg |= CR2_VPPVDD;
		} else {
			return 1;
		}
		break;
	case 120:
		reg |= CR2_VPP12;
		break;
	default:
		return 1;
	}

	/* first, turn off all power */
	MBX_CSR2 &= ~(CR2_VDDSEL | CR2_VPPSEL);

	/* enable new powersettings */
	MBX_CSR2 |= reg;
	debug ("MBX_CSR2 read = 0x%02x\n", MBX_CSR2);

	return (0);
}

static int hardware_enable (int slot)
{
	volatile immap_t *immap;
	volatile cpm8xx_t *cp;
	volatile pcmconf8xx_t *pcmp;
	volatile sysconf8xx_t *sysp;
	uint reg, mask;

	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n",
				  'A' + slot);

	udelay (10000);

	immap = (immap_t *) CFG_IMMR;
	sysp = (sysconf8xx_t *) (&(((immap_t *) CFG_IMMR)->im_siu_conf));
	pcmp = (pcmconf8xx_t *) (&(((immap_t *) CFG_IMMR)->im_pcmcia));
	cp = (cpm8xx_t *) (&(((immap_t *) CFG_IMMR)->im_cpm));

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr = PCMCIA_MASK (_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK (_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX (_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg = PCMCIA_PGCRX (_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;	/* active low  */
	PCMCIA_PGCRX (_slot_) = reg;
	udelay (500);

	/* remove all power */
	voltage_set (slot, 0, 0);
	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay (10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n", __LINE__, __FUNCTION__,
		  &(pcmp->pcmc_pipr), pcmp->pcmc_pipr);

	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On.
	 */
	mask = PCMCIA_VS1 (_slot_) | PCMCIA_VS2 (_slot_);
	reg = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n", reg,
		  (reg & PCMCIA_VS1 (slot)) ? "n" : "ff",
		  (reg & PCMCIA_VS2 (slot)) ? "n" : "ff");

	if ((reg & mask) == mask) {
		voltage_set (_slot_, 50, 0);
		printf (" 5.0V card found: ");
	} else {
		voltage_set (_slot_, 33, 0);
		printf (" 3.3V card found: ");
	}

	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg = PCMCIA_PGCRX (_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;	/* active low  */
	PCMCIA_PGCRX (_slot_) = reg;

	udelay (250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}

#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable (int slot)
{
	return 0;	/* No hardware to disable */
}
#endif /* CFG_CMD_PCMCIA */
#endif /* CONFIG_MBX */
/* ---------------------------------------------------------------------------- */
/* R360MPI Board								*/
/* ---------------------------------------------------------------------------- */

#if defined(CONFIG_R360MPI)

#define PCMCIA_BOARD_MSG "R360MPI"


static int hardware_enable(int slot)
{
	volatile immap_t	*immap;
	volatile cpm8xx_t	*cp;
	volatile pcmconf8xx_t	*pcmp;
	volatile sysconf8xx_t	*sysp;
	uint reg, mask;

	debug ("hardware_enable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	udelay(10000);

	immap = (immap_t *)CFG_IMMR;
	sysp  = (sysconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_siu_conf));
	pcmp  = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	cp    = (cpm8xx_t *)(&(((immap_t *)CFG_IMMR)->im_cpm));

	/*
	 * Configure SIUMCR to enable PCMCIA port B
	 * (VFLS[0:1] are not used for debugging, we connect FRZ# instead)
	 */
	sysp->sc_siumcr &= ~SIUMCR_DBGC11;	/* set DBGC to 00 */

	/* clear interrupt state, and disable interrupts */
	pcmp->pcmc_pscr =  PCMCIA_MASK(_slot_);
	pcmp->pcmc_per &= ~PCMCIA_MASK(_slot_);

	/* disable interrupts & DMA */
	PCMCIA_PGCRX(_slot_) = 0;

	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Ports A, B & C pins for
	 * 5 Volts Enable and 3 Volts enable
	 */
	immap->im_ioport.iop_pcpar &= ~(0x0400);
	immap->im_ioport.iop_pcso  &= ~(0x0400);/*
	immap->im_ioport.iop_pcdir |= 0x0400;*/

	immap->im_ioport.iop_papar &= ~(0x0200);/*
	immap->im_ioport.iop_padir |= 0x0200;*/
#if 0
	immap->im_ioport.iop_pbpar &= ~(0xC000);
	immap->im_ioport.iop_pbdir &= ~(0xC000);
#endif
	/* remove all power */

	immap->im_ioport.iop_pcdat |= 0x0400;
	immap->im_ioport.iop_padat |= 0x0200;

	/*
	 * Make sure there is a card in the slot, then configure the interface.
	 */
	udelay(10000);
	debug ("[%d] %s: PIPR(%p)=0x%x\n",
		__LINE__,__FUNCTION__,
		&(pcmp->pcmc_pipr),pcmp->pcmc_pipr);
	if (pcmp->pcmc_pipr & 0x00001800) {
		printf ("   No Card found\n");
		return (1);
	}

	/*
	 * Power On.
	 */
	mask = PCMCIA_VS1(slot) | PCMCIA_VS2(slot);
	reg  = pcmp->pcmc_pipr;
	debug ("PIPR: 0x%x ==> VS1=o%s, VS2=o%s\n",
		reg,
		(reg&PCMCIA_VS1(slot))?"n":"ff",
		(reg&PCMCIA_VS2(slot))?"n":"ff");
	if ((reg & mask) == mask) {
		immap->im_ioport.iop_pcdat &= ~(0x4000);
		puts (" 5.0V card found: ");
	} else {
		immap->im_ioport.iop_padat &= ~(0x0002);
		puts (" 3.3V card found: ");
	}
	immap->im_ioport.iop_pcdir |= 0x0400;
	immap->im_ioport.iop_padir |= 0x0200;
#if 0
	/*  VCC switch error flag, PCMCIA slot INPACK_ pin */
	cp->cp_pbdir &= ~(0x0020 | 0x0010);
	cp->cp_pbpar &= ~(0x0020 | 0x0010);
	udelay(500000);
#endif
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(250000);	/* some cards need >150 ms to come up :-( */

	debug ("# hardware_enable done\n");

	return (0);
}



#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
static int hardware_disable(int slot)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("hardware_disable: " PCMCIA_BOARD_MSG " Slot %c\n", 'A'+slot);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));

	/* remove all power */
	immap->im_ioport.iop_pcdat |= 0x0400;
	immap->im_ioport.iop_padat |= 0x0200;

	/* Configure PCMCIA General Control Register */
	PCMCIA_PGCRX(_slot_) = 0;

	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |= __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |= __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;

	udelay(10000);

	return (0);
}
#endif	/* CFG_CMD_PCMCIA */



static int voltage_set(int slot, int vcc, int vpp)
{
	volatile immap_t	*immap;
	volatile pcmconf8xx_t	*pcmp;
	u_long reg;

	debug ("voltage_set: "
		PCMCIA_BOARD_MSG
		" Slot %c, Vcc=%d.%d, Vpp=%d.%d\n",
		'A'+slot, vcc/10, vcc%10, vpp/10, vcc%10);

	immap = (immap_t *)CFG_IMMR;
	pcmp = (pcmconf8xx_t *)(&(((immap_t *)CFG_IMMR)->im_pcmcia));
	/*
	 * Disable PCMCIA buffers (isolate the interface)
	 * and assert RESET signal
	 */
	debug ("Disable PCMCIA buffers and assert RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg |=  __MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg |=  __MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	/*
	 * Configure Ports A & C pins for
	 * 5 Volts Enable and 3 Volts enable,
	 * Turn off all power
	 */
	debug ("PCMCIA power OFF\n");
	immap->im_ioport.iop_pcpar &= ~(0x0400);
	immap->im_ioport.iop_pcso  &= ~(0x0400);/*
	immap->im_ioport.iop_pcdir |= 0x0400;*/

	immap->im_ioport.iop_papar &= ~(0x0200);/*
	immap->im_ioport.iop_padir |= 0x0200;*/

	immap->im_ioport.iop_pcdat |= 0x0400;
	immap->im_ioport.iop_padat |= 0x0200;

	reg = 0;
	switch(vcc) {
	case  0: 		break;
	case 33: reg |= 0x0200;	break;
	case 50: reg |= 0x0400;	break;
	default: 		goto done;
	}

	/* Checking supported voltages */

	debug ("PIPR: 0x%x --> %s\n",
		pcmp->pcmc_pipr,
		(pcmp->pcmc_pipr & 0x00008000) ? "only 5 V" : "can do 3.3V");

	if (reg & 0x0200)
		immap->im_ioport.iop_pcdat &= !reg;
	if (reg & 0x0400)
		immap->im_ioport.iop_padat &= !reg;
	immap->im_ioport.iop_pcdir |=  0x0200;
	immap->im_ioport.iop_padir |=  0x0400;
	if (reg) {
		debug ("PCMCIA powered at %sV\n",
			(reg&0x0400) ? "5.0" : "3.3");
	} else {
		debug ("PCMCIA powered down\n");
	}

done:
	debug ("Enable PCMCIA buffers and stop RESET\n");
	reg  =  PCMCIA_PGCRX(_slot_);
	reg &= ~__MY_PCMCIA_GCRX_CXRESET;	/* active high */
	reg &= ~__MY_PCMCIA_GCRX_CXOE;		/* active low  */
	PCMCIA_PGCRX(_slot_) = reg;
	udelay(500);

	debug ("voltage_set: " PCMCIA_BOARD_MSG " Slot %c, DONE\n",
		slot+'A');
	return (0);
}

#endif	/* R360MPI */


/* ---------------------------------------------------------------------------- */
/* End of Board Specific Stuff							*/
/* ---------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/* MPC8xx Specific Stuff - should go to MPC8xx directory			*/
/* ---------------------------------------------------------------------------- */

/*
 * Search this table to see if the windowsize is
 * supported...
 */

#define M8XX_SIZES_NO 32

static const u_int m8xx_size_to_gray[M8XX_SIZES_NO] =
{ 0x00000001, 0x00000002, 0x00000008, 0x00000004,
  0x00000080, 0x00000040, 0x00000010, 0x00000020,
  0x00008000, 0x00004000, 0x00001000, 0x00002000,
  0x00000100, 0x00000200, 0x00000800, 0x00000400,

  0x0fffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0x01000000, 0x02000000, 0xffffffff, 0x04000000,
  0x00010000, 0x00020000, 0x00080000, 0x00040000,
  0x00800000, 0x00400000, 0x00100000, 0x00200000 };


/* ---------------------------------------------------------------------------- */

static u_int m8xx_get_graycode(u_int size)
{
	u_int k;

	for (k = 0; k < M8XX_SIZES_NO; k++) {
		if(m8xx_size_to_gray[k] == size)
			break;
	}

	if((k == M8XX_SIZES_NO) || (m8xx_size_to_gray[k] == -1))
		k = -1;

	return k;
}

/* ------------------------------------------------------------------------- */

#if 0
static u_int m8xx_get_speed(u_int ns, u_int is_io)
{
	u_int reg, clocks, psst, psl, psht;

	if(!ns) {

		/*
		 * We get called with IO maps setup to 0ns
		 * if not specified by the user.
		 * They should be 255ns.
		 */

		if(is_io)
			ns = 255;
		else
			ns = 100;  /* fast memory if 0 */
	}

	/*
	 * In PSST, PSL, PSHT fields we tell the controller
	 * timing parameters in CLKOUT clock cycles.
	 * CLKOUT is the same as GCLK2_50.
	 */

/* how we want to adjust the timing - in percent */

#define ADJ 180 /* 80 % longer accesstime - to be sure */

	clocks = ((M8XX_BUSFREQ / 1000) * ns) / 1000;
	clocks = (clocks * ADJ) / (100*1000);

	if(clocks >= PCMCIA_BMT_LIMIT) {
		DEBUG(0, "Max access time limit reached\n");
		clocks = PCMCIA_BMT_LIMIT-1;
	}

	psst = clocks / 7;          /* setup time */
	psht = clocks / 7;          /* hold time */
	psl  = (clocks * 5) / 7;    /* strobe length */

	psst += clocks - (psst + psht + psl);

	reg =  psst << 12;
	reg |= psl  << 7;
	reg |= psht << 16;

	return reg;
}
#endif

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_8xx_PCCARD
static void print_funcid (int func)
{
	puts (indent);
	switch (func) {
	case CISTPL_FUNCID_MULTI:
		puts (" Multi-Function");
		break;
	case CISTPL_FUNCID_MEMORY:
		puts (" Memory");
		break;
	case CISTPL_FUNCID_SERIAL:
		puts (" Serial Port");
		break;
	case CISTPL_FUNCID_PARALLEL:
		puts (" Parallel Port");
		break;
	case CISTPL_FUNCID_FIXED:
		puts (" Fixed Disk");
		break;
	case CISTPL_FUNCID_VIDEO:
		puts (" Video Adapter");
		break;
	case CISTPL_FUNCID_NETWORK:
		puts (" Network Adapter");
		break;
	case CISTPL_FUNCID_AIMS:
		puts (" AIMS Card");
		break;
	case CISTPL_FUNCID_SCSI:
		puts (" SCSI Adapter");
		break;
	default:
		puts (" Unknown");
		break;
	}
	puts (" Card\n");
}
#endif	/* CONFIG_IDE_8xx_PCCARD */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_8xx_PCCARD
static void print_fixed (volatile uchar *p)
{
	if (p == NULL)
		return;

	puts(indent);

	switch (*p) {
	case CISTPL_FUNCE_IDE_IFACE:
	    {   uchar iface = *(p+2);

		puts ((iface == CISTPL_IDE_INTERFACE) ? " IDE" : " unknown");
		puts (" interface ");
		break;
	    }
	case CISTPL_FUNCE_IDE_MASTER:
	case CISTPL_FUNCE_IDE_SLAVE:
	    {   uchar f1 = *(p+2);
		uchar f2 = *(p+4);

		puts ((f1 & CISTPL_IDE_SILICON) ? " [silicon]" : " [rotating]");

		if (f1 & CISTPL_IDE_UNIQUE)
			puts (" [unique]");

		puts ((f1 & CISTPL_IDE_DUAL) ? " [dual]" : " [single]");

		if (f2 & CISTPL_IDE_HAS_SLEEP)
			puts (" [sleep]");

		if (f2 & CISTPL_IDE_HAS_STANDBY)
			puts (" [standby]");

		if (f2 & CISTPL_IDE_HAS_IDLE)
			puts (" [idle]");

		if (f2 & CISTPL_IDE_LOW_POWER)
			puts (" [low power]");

		if (f2 & CISTPL_IDE_REG_INHIBIT)
			puts (" [reg inhibit]");

		if (f2 & CISTPL_IDE_HAS_INDEX)
			puts (" [index]");

		if (f2 & CISTPL_IDE_IOIS16)
			puts (" [IOis16]");

		break;
	    }
	}
	putc ('\n');
}
#endif	/* CONFIG_IDE_8xx_PCCARD */

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_8xx_PCCARD

#define MAX_IDENT_CHARS		64
#define	MAX_IDENT_FIELDS	4

static uchar *known_cards[] = {
	"ARGOSY PnPIDE D5",
	NULL
};

static int identify  (volatile uchar *p)
{
	uchar id_str[MAX_IDENT_CHARS];
	uchar data;
	uchar *t;
	uchar **card;
	int i, done;

	if (p == NULL)
		return (0);	/* Don't know */

	t = id_str;
	done =0;

	for (i=0; i<=4 && !done; ++i, p+=2) {
		while ((data = *p) != '\0') {
			if (data == 0xFF) {
				done = 1;
				break;
			}
			*t++ = data;
			if (t == &id_str[MAX_IDENT_CHARS-1]) {
				done = 1;
				break;
			}
			p += 2;
		}
		if (!done)
			*t++ = ' ';
	}
	*t = '\0';
	while (--t > id_str) {
		if (*t == ' ')
			*t = '\0';
		else
			break;
	}
	puts (id_str);
	putc ('\n');

	for (card=known_cards; *card; ++card) {
		debug ("## Compare against \"%s\"\n", *card);
		if (strcmp(*card, id_str) == 0) {	/* found! */
			debug ("## CARD FOUND ##\n");
			return (1);
		}
	}

	return (0);	/* don't know */
}
#endif	/* CONFIG_IDE_8xx_PCCARD */

/* ------------------------------------------------------------------------- */

#endif /* CFG_CMD_PCMCIA || (CFG_CMD_IDE && CONFIG_IDE_8xx_PCCARD) */
