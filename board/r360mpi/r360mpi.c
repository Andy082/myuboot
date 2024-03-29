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
#include <config.h>
#include <mpc8xx.h>
#include <i2c.h>

#include <commproc.h>
#include <command.h>
#include <cmd_bsp.h>
#include <malloc.h>

#include <linux/types.h>
#include <linux/string.h>       /* for strdup */


/*
 *  Memory Controller Using
 *
 *  CS0 - Flash memory            (0x40000000)
 *  CS1 - SDRAM                   (0x00000000}
 *  CS2 - 
 *  CS3 - 
 *  CS4 - 
 *  CS5 - 
 *  CS6 - PCMCIA device
 *  CS7 - PCMCIA device
 */

/* ------------------------------------------------------------------------- */

#define _not_used_	0xffffffff

const uint sdram_table[]=
{
        /* single read. (offset 0 in upm RAM) */
        0x1f07fc04, 0xeeaefc04, 0x11adfc04, 0xefbbbc00,
        0x1ff77c47,

        /* MRS initialization (offset 5) */

        0x1ff77c34, 0xefeabc34, 0x1fb57c35,

        /* burst read. (offset 8 in upm RAM) */
        0x1f07fc04, 0xeeaefc04, 0x10adfc04, 0xf0affc00,
        0xf0affc00, 0xf1affc00, 0xefbbbc00, 0x1ff77c47,
        _not_used_, _not_used_, _not_used_, _not_used_,
        _not_used_, _not_used_, _not_used_, _not_used_,

        /* single write. (offset 18 in upm RAM) */
        0x1f27fc04, 0xeeaebc00, 0x01b93c04, 0x1ff77c47,
        _not_used_, _not_used_, _not_used_, _not_used_,

        /* burst write. (offset 20 in upm RAM) */
        0x1f07fc04, 0xeeaebc00, 0x10ad7c00, 0xf0affc00,
        0xf0affc00, 0xe1bbbc04, 0x1ff77c47, _not_used_,
        _not_used_, _not_used_, _not_used_, _not_used_,
        _not_used_, _not_used_, _not_used_, _not_used_,

        /* refresh. (offset 30 in upm RAM) */
        0x1ff5fc84, 0xfffffc04, 0xfffffc04, 0xfffffc04,
        0xfffffc84, 0xfffffc07, _not_used_, _not_used_,
        _not_used_, _not_used_, _not_used_, _not_used_,

        /* exception. (offset 3c in upm RAM) */
        0x7ffffc07, _not_used_, _not_used_, _not_used_ };

/* ------------------------------------------------------------------------- */

/*
 * Check Board Identity:
 */

int checkboard (void)
{
	puts ("Board: R360 MPI Board\n");
	return 0;
}

/* ------------------------------------------------------------------------- */

static long int dram_size (long int, long int *, long int);

/* ------------------------------------------------------------------------- */

long int initdram (int board_type)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	long int size8, size9;
	long int size_b0 = 0;
	unsigned long reg;

	upmconfig (UPMA, (uint *) sdram_table,
			   sizeof (sdram_table) / sizeof (uint));

	/*
	 * Preliminary prescaler for refresh (depends on number of
	 * banks): This value is selected for four cycles every 62.4 us
	 * with two SDRAM banks or four cycles every 31.2 us with one
	 * bank. It will be adjusted after memory sizing.
	 */
	memctl->memc_mptpr = CFG_MPTPR_2BK_8K;

	memctl->memc_mar = 0x00000088;

	/*
	 * Map controller bank 1 to the SDRAM bank at
	 * preliminary address - these have to be modified after the
	 * SDRAM size has been determined.
	 */
	memctl->memc_or1 = CFG_OR1_PRELIM;
	memctl->memc_br1 = CFG_BR1_PRELIM;

	memctl->memc_mamr = CFG_MAMR_8COL & (~(MAMR_PTAE));	/* no refresh yet */

	udelay (200);

	/* perform SDRAM initializsation sequence */

	memctl->memc_mcr = 0x80002105;	/* SDRAM bank 0 */
	udelay (200);
	memctl->memc_mcr = 0x80002230;	/* SDRAM bank 0 - execute twice */
	udelay (200);

	memctl->memc_mamr |= MAMR_PTAE;	/* enable refresh */

	udelay (1000);

	/*
	 * Check Bank 0 Memory Size for re-configuration
	 *
	 * try 8 column mode
	 */
	size8 = dram_size (CFG_MAMR_8COL, (ulong *) SDRAM_BASE1_PRELIM,
					   SDRAM_MAX_SIZE);

	udelay (1000);

	/*
	 * try 9 column mode
	 */
	size9 = dram_size (CFG_MAMR_9COL, (ulong *) SDRAM_BASE1_PRELIM,
					   SDRAM_MAX_SIZE);

	if (size8 < size9) {		/* leave configuration at 9 columns */
		size_b0 = size9;
/*	debug ("SDRAM Bank 0 in 9 column mode: %ld MB\n", size >> 20);	*/
	} else {					/* back to 8 columns            */
		size_b0 = size8;
		memctl->memc_mamr = CFG_MAMR_8COL;
		udelay (500);
/*	debug ("SDRAM Bank 0 in 8 column mode: %ld MB\n", size >> 20);	*/
	}

	udelay (1000);

	/*
	 * Adjust refresh rate depending on SDRAM type, both banks
	 * For types > 128 MBit leave it at the current (fast) rate
	 */
	if ((size_b0 < 0x02000000)) {
		/* reduce to 15.6 us (62.4 us / quad) */
		memctl->memc_mptpr = CFG_MPTPR_2BK_4K;
		udelay (1000);
	}

	/*
	 * Final mapping
	 */

	memctl->memc_or1 = ((-size_b0) & 0xFFFF0000) | CFG_OR_TIMING_SDRAM;
	memctl->memc_br1 = (CFG_SDRAM_BASE & BR_BA_MSK) | BR_MS_UPMA | BR_V;

	/* adjust refresh rate depending on SDRAM type, one bank */
	reg = memctl->memc_mptpr;
	reg >>= 1;		/* reduce to CFG_MPTPR_1BK_8K / _4K */
	memctl->memc_mptpr = reg;

	udelay (10000);

	return (size_b0);
}

/* ------------------------------------------------------------------------- */

/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'. Some (not all) hardware errors are detected:
 * - short between address lines
 * - short between data lines
 */

static long int dram_size (long int mamr_value, long int *base,
						   long int maxsize)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	volatile long int *addr;
	ulong cnt, val;
	ulong save[32];			/* to make test non-destructive */
	unsigned char i = 0;

	memctl->memc_mamr = mamr_value;

	for (cnt = maxsize / sizeof (long); cnt > 0; cnt >>= 1) {
		addr = base + cnt;	/* pointer arith! */

		save[i++] = *addr;
		*addr = ~cnt;
	}

	/* write 0 to base address */
	addr = base;
	save[i] = *addr;
	*addr = 0;

	/* check at base address */
	if ((val = *addr) != 0) {
		*addr = save[i];
		return (0);
	}

	for (cnt = 1; cnt <= maxsize / sizeof (long); cnt <<= 1) {
		addr = base + cnt;	/* pointer arith! */
		val = *addr;
		*addr = save[--i];

		if (val != (~cnt)) {
			return (cnt * sizeof (long));
		}
	}
	return (maxsize);
}

/* ------------------------------------------------------------------------- */

void r360_pwm_write (uchar reg, uchar val)
{
	if (i2c_write (CFG_I2C_PWM_ADDR, reg, 1, &val, 1)) {
		printf ("Can't write PWM register 0x%02X.\n", reg);
	}
}

/* ------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------
 * Keyboard Controller
 */

/* Number of bytes returned from Keyboard Controller */
#define KEYBD_KEY_MAX		20				/* maximum key number */
#define KEYBD_DATALEN		((KEYBD_KEY_MAX + 7) / 8)	/* normal key scan data */

static uchar kbd_addr = CFG_I2C_KBD_ADDR;

static uchar *key_match (uchar *);

int misc_init_r (void)
{
	uchar kbd_data[KEYBD_DATALEN];
	uchar keybd_env[2 * KEYBD_DATALEN + 1];
	uchar *str;
	int i;

	i2c_init (CFG_I2C_SPEED, CFG_I2C_SLAVE);

	i2c_read (kbd_addr, 0, 0, kbd_data, KEYBD_DATALEN);

	for (i = 0; i < KEYBD_DATALEN; ++i) {
		sprintf (keybd_env + i + i, "%02X", kbd_data[i]);
	}
	setenv ("keybd", keybd_env);

	str = strdup (key_match (kbd_data));	/* decode keys */

#ifdef CONFIG_PREBOOT	/* automatically configure "preboot" command on key match */
	setenv ("preboot", str);	/* set or delete definition */
#endif /* CONFIG_PREBOOT */
	if (str != NULL) {
		free (str);
	}

	return (0);
}

/*-----------------------------------------------------------------------
 * Check if pressed key(s) match magic sequence,
 * and return the command string associated with that key(s).
 *
 * If no key press was decoded, NULL is returned.
 *
 * Note: the first character of the argument will be overwritten with
 * the "magic charcter code" of the decoded key(s), or '\0'.
 *
 *
 * Note: the string points to static environment data and must be
 * saved before you call any function that modifies the environment.
 */
#ifdef CONFIG_PREBOOT

static uchar kbd_magic_prefix[] = "key_magic";
static uchar kbd_command_prefix[] = "key_cmd";

static uchar *key_match (uchar * kbd_data)
{
	uchar compare[KEYBD_DATALEN];
	uchar magic[sizeof (kbd_magic_prefix) + 1];
	uchar cmd_name[sizeof (kbd_command_prefix) + 1];
	uchar key_mask;
	uchar *str, *nxt, *suffix;
	uchar *kbd_magic_keys;
	char *cmd;
	int i;

	/*
	 * The following string defines the characters that can pe appended
	 * to "key_magic" to form the names of environment variables that
	 * hold "magic" key codes, i. e. such key codes that can cause
	 * pre-boot actions. If the string is empty (""), then only
	 * "key_magic" is checked (old behaviour); the string "125" causes
	 * checks for "key_magic1", "key_magic2" and "key_magic5", etc.
	 */
	if ((kbd_magic_keys = getenv ("magic_keys")) == NULL)
		kbd_magic_keys = "";

	/* loop over all magic keys;
	 * use '\0' suffix in case of empty string
	 */
	for (suffix=kbd_magic_keys; *suffix || suffix==kbd_magic_keys; ++suffix) {
		sprintf (magic, "%s%c", kbd_magic_prefix, *suffix);
#if 0
		printf ("### Check magic \"%s\"\n", magic);
#endif

		memcpy(compare, kbd_data, KEYBD_DATALEN);

		for (str = getenv(magic); str != NULL; str = (*nxt) ? nxt+1 : nxt) {
			uchar c;

			c = (uchar) simple_strtoul (str, (char **) (&nxt), 16);

			if (str == nxt)				/* invalid character */
				break;

			if (c >= KEYBD_KEY_MAX)			/* bad key number */
				goto next_magic;

			key_mask = 0x80 >> (c % 8);

			if (!(compare[c / 8] & key_mask))	/* key not pressed */
				goto next_magic;

			compare[c / 8] &= ~key_mask;
		}

		for (i=0; i<KEYBD_DATALEN; i++)
			if (compare[i])			/* key(s) not released */
				goto next_magic;

		sprintf (cmd_name, "%s%c", kbd_command_prefix, *suffix);

		cmd = getenv (cmd_name);
#if 0
		printf ("### Set PREBOOT to $(%s): \"%s\"\n",
			cmd_name, cmd ? cmd : "<<NULL>>");
#endif
		*kbd_data = *suffix;
		return (cmd);

	next_magic:;
	}
#if 0
	printf ("### Delete PREBOOT\n");
#endif
	*kbd_data = '\0';
	return (NULL);
}
#endif							/* CONFIG_PREBOOT */

/* Read Keyboard status */
int do_kbd (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	uchar kbd_data[KEYBD_DATALEN];
	uchar keybd_env[2 * KEYBD_DATALEN + 1];
	int i;

	i2c_init (CFG_I2C_SPEED, CFG_I2C_SLAVE);

	/* Read keys */
	i2c_read (kbd_addr, 0, 0, kbd_data, KEYBD_DATALEN);
	
	puts ("Keys:");
	for (i = 0; i < KEYBD_DATALEN; ++i) {
		sprintf (keybd_env + i + i, "%02X", kbd_data[i]);
		printf (" %02x", kbd_data[i]);
	}
	putc ('\n');
	setenv ("keybd", keybd_env);
	return 0;
}
