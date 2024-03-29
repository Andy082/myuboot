/*
 * (C) Copyright 2001, 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Flash Routines for Intel devices
 *
 *--------------------------------------------------------------------
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

#include <common.h>
#include <mpc8xx.h>
#include "cpu86.h"

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

/*-----------------------------------------------------------------------
 */
ulong flash_int_get_size (volatile unsigned long *baseaddr,
					  flash_info_t * info)
{
	short i;
	unsigned long flashtest_h, flashtest_l;

	info->sector_count = info->size = 0;
	info->flash_id = FLASH_UNKNOWN;

	/* Write query command sequence and test FLASH answer
	 */
	baseaddr[0] = 0x00980098;
	baseaddr[1] = 0x00980098;

	flashtest_h = baseaddr[0];	/* manufacturer ID	*/
	flashtest_l = baseaddr[1];

	if (flashtest_h != INTEL_MANUFACT || flashtest_l != INTEL_MANUFACT)
		return (0);		/* no or unknown flash	*/

	flashtest_h = baseaddr[2];	/* device ID	        */
	flashtest_l = baseaddr[3];

	if (flashtest_h != flashtest_l)
		return (0);

	switch (flashtest_h) {
	case INTEL_ID_28F160C3B:
		info->flash_id = FLASH_28F160C3B;
		info->sector_count = 39;
		info->size = 0x00800000;	/* 4 * 2 MB = 8 MB	*/
		break;
	case INTEL_ID_28F160F3B:
		info->flash_id = FLASH_28F160F3B;
		info->sector_count = 39;
		info->size = 0x00800000;	/* 4 * 2 MB = 8 MB      */
		break;
	default:
		return (0);			/* no or unknown flash	*/
	}

	info->flash_id |= INTEL_MANUFACT << 16; /* set manufacturer offset */

	if (info->flash_id & FLASH_BTYPE) {
		volatile unsigned long *tmp = baseaddr;

		/* set up sector start adress table (bottom sector type)
		 * AND unlock the sectors (if our chip is 160C3)
		 */
		for (i = 0; i < info->sector_count; i++) {
			if ((info->flash_id & FLASH_TYPEMASK) == FLASH_28F160C3B) {
				tmp[0] = 0x00600060;
				tmp[1] = 0x00600060;
				tmp[0] = 0x00D000D0;
				tmp[1] = 0x00D000D0;
			}
			info->start[i] = (uint) tmp;
			tmp += i < 8 ? 0x2000 : 0x10000; /* pointer arith       */
		}
	}

	memset (info->protect, 0, info->sector_count);

	baseaddr[0] = 0x00FF00FF;
	baseaddr[1] = 0x00FF00FF;

	return (info->size);
}

static ulong flash_amd_get_size (vu_char *addr, flash_info_t *info)
{
	short i;
	uchar vendor, devid;
	ulong base = (ulong)addr;

	/* Write auto select command: read Manufacturer ID */
	addr[0x0555] = 0xAA;
	addr[0x02AA] = 0x55;
	addr[0x0555] = 0x90;

	udelay(1000);

	vendor = addr[0];
	devid = addr[1] & 0xff;

	/* only support AMD */
	if (vendor != 0x01) {
		return 0;
	}

	vendor &= 0xf;
	devid &= 0xff;

	if (devid == AMD_ID_F040B) {
		info->flash_id     = vendor << 16 | devid;
		info->sector_count = 8;
		info->size         = info->sector_count * 0x10000;
	}
	else if (devid == AMD_ID_F080B) {
		info->flash_id     = vendor << 16 | devid;
		info->sector_count = 16;
		info->size         = 4 * info->sector_count * 0x10000;
	}
	else if (devid == AMD_ID_F016D) {
		info->flash_id     = vendor << 16 | devid;
		info->sector_count = 32;
		info->size         = 4 * info->sector_count * 0x10000;
	}
	else {
		printf ("## Unknown Flash Type: %02x\n", devid);
		return 0;
	}

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* sector base address */
		info->start[i] = base + i * (info->size / info->sector_count);
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr = (volatile unsigned char *)(info->start[i]);
		info->protect[i] = addr[2] & 1;
	}

	/*
	 * Prevent writes to uninitialized FLASH.
	 */
	if (info->flash_id != FLASH_UNKNOWN) {
		addr = (vu_char *)info->start[0];
		addr[0] = 0xF0; /* reset bank */
	}

	return (info->size);
}


/*-----------------------------------------------------------------------
 */
unsigned long flash_init (void)
{
	unsigned long size_b0 = 0;
	unsigned long size_b1 = 0;
	int i;

	/* Init: no FLASHes known
	 */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	/* Disable flash protection */
	CPU86_BCR |= (CPU86_BCR_FWPT | CPU86_BCR_FWRE);

	/* Static FLASH Bank configuration here (only one bank) */

	size_b0 = flash_int_get_size ((ulong *) CFG_FLASH_BASE, &flash_info[0]);
	size_b1 = flash_amd_get_size ((uchar *) CFG_BOOTROM_BASE, &flash_info[1]);

	if (size_b0 > 0 || size_b1 > 0) {

		printf("(");

		if (size_b0 > 0) {
			puts ("Bank#1 - ");
			print_size (size_b0, (size_b1 > 0) ? ", " : ") ");
		}

		if (size_b1 > 0) {
			puts ("Bank#2 - ");
			print_size (size_b1, ") ");
		}
	}
	else {
		printf ("## No FLASH found.\n");
		return 0;
	}
	/* protect monitor and environment sectors
	 */

#if CFG_MONITOR_BASE >= CFG_BOOTROM_BASE
	if (size_b1) {
		/* If PPCBoot is booted from ROM the CFG_MONITOR_BASE > CFG_FLASH_BASE
		 * but we shouldn't protect it.
		 */

		flash_protect  (FLAG_PROTECT_SET,
				CFG_MONITOR_BASE,
				CFG_MONITOR_BASE + CFG_MONITOR_LEN - 1, &flash_info[1]
		);
	}
#else
#if CFG_MONITOR_BASE >= CFG_FLASH_BASE
	flash_protect (FLAG_PROTECT_SET,
		       CFG_MONITOR_BASE,
		       CFG_MONITOR_BASE + CFG_MONITOR_LEN - 1, &flash_info[0]
	);
#endif
#endif

#if (CFG_ENV_IS_IN_FLASH == 1) && defined(CFG_ENV_ADDR)
# ifndef  CFG_ENV_SIZE
#  define CFG_ENV_SIZE	CFG_ENV_SECT_SIZE
# endif
# if CFG_ENV_ADDR >= CFG_BOOTROM_BASE
	if (size_b1) {
		flash_protect (FLAG_PROTECT_SET,
				CFG_ENV_ADDR,
				CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[1]);
	}
# else
	flash_protect (FLAG_PROTECT_SET,
		       CFG_ENV_ADDR,
		       CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[0]);
# endif
#endif

	return (size_b0 + size_b1);
}

/*-----------------------------------------------------------------------
 */
void flash_print_info (flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch ((info->flash_id >> 16) & 0xff) {
	case 0x89:
		printf ("INTEL ");
		break;
	case 0x1:
		printf ("AMD ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_28F160C3B:
		printf ("28F160C3B (16 Mbit, bottom sector)\n");
		break;
	case FLASH_28F160F3B:
		printf ("28F160F3B (16 Mbit, bottom sector)\n");
		break;
	case AMD_ID_F040B:
		printf ("AM29F040B (4 Mbit)\n");
		break;
	default:
		printf ("Unknown Chip Type\n");
		break;
	}

	if (info->size < 0x100000)
		printf ("  Size: %ld KB in %d Sectors\n",
				info->size >> 10, info->sector_count);
	else
		printf ("  Size: %ld MB in %d Sectors\n",
				info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i],
			info->protect[i] ? " (RO)" : "     "
		);
	}
	printf ("\n");
}

/*-----------------------------------------------------------------------
 */
int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	vu_char *addr = (vu_char *)(info->start[0]);
	int flag, prot, sect, l_sect;
	ulong start, now, last;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect])
			prot++;
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
				prot);
	} else {
		printf ("\n");
	}

	/* Check the type of erased flash
	 */
	if (info->flash_id >> 16 == 0x1) {
		/* Erase AMD flash
		 */
		l_sect = -1;

		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();

		addr[0x0555] = 0xAA;
		addr[0x02AA] = 0x55;
		addr[0x0555] = 0x80;
		addr[0x0555] = 0xAA;
		addr[0x02AA] = 0x55;

		/* wait at least 80us - let's wait 1 ms */
		udelay (1000);

		/* Start erase on unprotected sectors */
		for (sect = s_first; sect<=s_last; sect++) {
			if (info->protect[sect] == 0) { /* not protected */
				addr = (vu_char *)(info->start[sect]);
				addr[0] = 0x30;
				l_sect = sect;
			}
		}

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* wait at least 80us - let's wait 1 ms */
		udelay (1000);

		/*
		 * We wait for the last triggered sector
		 */
		if (l_sect < 0)
			goto AMD_DONE;

		start = get_timer (0);
		last  = start;
		addr = (vu_char *)(info->start[l_sect]);
		while ((addr[0] & 0x80) != 0x80) {
			if ((now = get_timer(start)) > CFG_FLASH_ERASE_TOUT) {
				printf ("Timeout\n");
				return 1;
			}
			/* show that we're waiting */
			if ((now - last) > 1000) {      /* every second */
				serial_putc ('.');
				last = now;
			}
		}

AMD_DONE:
		/* reset to read mode */
		addr = (volatile unsigned char *)info->start[0];
		addr[0] = 0xF0;     /* reset bank */

	} else {
		/* Erase Intel flash
		 */

		/* Start erase on unprotected sectors
		 */
		for (sect = s_first; sect <= s_last; sect++) {
			volatile ulong *addr =
				(volatile unsigned long *) info->start[sect];

			start = get_timer (0);
			last = start;
			if (info->protect[sect] == 0) {
			/* Disable interrupts which might cause a timeout here
			 */
				flag = disable_interrupts ();

				/* Erase the block
				 */
				addr[0] = 0x00200020;
				addr[1] = 0x00200020;
				addr[0] = 0x00D000D0;
				addr[1] = 0x00D000D0;

				/* re-enable interrupts if necessary
				 */
				if (flag)
					enable_interrupts ();

				/* wait at least 80us - let's wait 1 ms
				 */
				udelay (1000);

				last = start;
				while ((addr[0] & 0x00800080) != 0x00800080 ||
				   (addr[1] & 0x00800080) != 0x00800080) {
					if ((now = get_timer (start)) > CFG_FLASH_ERASE_TOUT) {
						printf ("Timeout (erase suspended!)\n");
						/* Suspend erase
						 */
						addr[0] = 0x00B000B0;
						addr[1] = 0x00B000B0;
						goto DONE;
					}
					/* show that we're waiting
					 */
					if ((now - last) > 1000) {	/* every second */
						serial_putc ('.');
						last = now;
					}
				}
				if (addr[0] & 0x00220022 || addr[1] & 0x00220022) {
					printf ("*** ERROR: erase failed!\n");
					goto DONE;
				}
			}
			/* Clear status register and reset to read mode
			 */
			addr[0] = 0x00500050;
			addr[1] = 0x00500050;
			addr[0] = 0x00FF00FF;
			addr[1] = 0x00FF00FF;
		}
	}

	printf (" done\n");

DONE:
	return 0;
}

static int write_word (flash_info_t *, volatile unsigned long *, ulong);
static int write_byte (flash_info_t *info, ulong dest, uchar data);

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong v;
	int i, l, rc, cc = cnt, res = 0;

	if (info->flash_id >> 16 == 0x1) {

		/* Write to AMD 8-bit flash
		 */
		while (cnt > 0) {
			if ((rc = write_byte(info, addr, *src)) != 0) {
				return (rc);
			}
			addr++;
			src++;
			cnt--;
		}

		return (0);
	} else {

		/* Write to Intel 64-bit flash
		 */
		for (v=0; cc > 0; addr += 4, cc -= 4 - l) {
			l = (addr & 3);
			addr &= ~3;

			for (i = 0; i < 4; i++) {
				v = (v << 8) + (i < l || i - l >= cc ?
					*((unsigned char *) addr + i) : *src++);
			}

			if ((res = write_word (info, (volatile unsigned long *) addr, v)) != 0)
				break;
		}
	}

	return (res);
}

/*-----------------------------------------------------------------------
 * Write a word to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word (flash_info_t * info, volatile unsigned long *addr,
					   ulong data)
{
	int flag, res = 0;
	ulong start;

	/* Check if Flash is (sufficiently) erased
	 */
	if ((*addr & data) != data)
		return (2);

	/* Disable interrupts which might cause a timeout here
	 */
	flag = disable_interrupts ();

	*addr = 0x00400040;
	*addr = data;

	/* re-enable interrupts if necessary
	 */
	if (flag)
		enable_interrupts ();

	start = get_timer (0);
	while ((*addr & 0x00800080) != 0x00800080) {
		if (get_timer (start) > CFG_FLASH_WRITE_TOUT) {
			/* Suspend program
			 */
			*addr = 0x00B000B0;
			res = 1;
			goto OUT;
		}
	}

	if (*addr & 0x00220022) {
		printf ("*** ERROR: program failed!\n");
		res = 1;
	}

OUT:
	/* Clear status register and reset to read mode
	 */
	*addr = 0x00500050;
	*addr = 0x00FF00FF;

	return (res);
}

/*-----------------------------------------------------------------------
 * Write a byte to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_byte (flash_info_t *info, ulong dest, uchar data)
{
	vu_char *addr = (vu_char *)(info->start[0]);
	ulong start;
	int flag;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_char *)dest) & data) != data) {
		return (2);
	}
	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	addr[0x0555] = 0xAA;
	addr[0x02AA] = 0x55;
	addr[0x0555] = 0xA0;

	*((vu_char *)dest) = data;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* data polling for D7 */
	start = get_timer (0);
	while ((*((vu_char *)dest) & 0x80) != (data & 0x80)) {
		if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
			return (1);
		}
	}
	return (0);
}

/*-----------------------------------------------------------------------
 */
