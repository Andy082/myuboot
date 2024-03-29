/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Gregory E. Allen, gallen@arlut.utexas.edu
 * Matthew E. Karger, karger@arlut.utexas.edu
 * Applied Research Laboratories, The University of Texas at Austin
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

/*
 *
 * Configuration settings for the utx8245 board.
 *
 */

/* ------------------------------------------------------------------------- */

/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_MPC824X		1
#define CONFIG_MPC8245		1
#define CONFIG_UTX8245		1
#define DEBUG				1

#define CONFIG_CONS_INDEX	1
#define CONFIG_BAUDRATE		57600
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_BOOTDELAY	5
#define CONFIG_AUTOBOOT_PROMPT 	"autoboot in %d seconds\n"
#define CONFIG_BOOTCOMMAND	"bootm FF920000 FF800000"	/* autoboot command	*/
#define CONFIG_BOOTARGS		"root=/dev/ram console=ttyS0,57600" /* RAMdisk */
#define CONFIG_ETHADDR		41:52:4c:61:00:01	/* MAC address */
#define CONFIG_SERVERIP		10.8.17.105
#define CONFIG_ENV_OVERWRITE

#define CONFIG_COMMANDS		(CFG_CMD_DFL | CFG_CMD_BDI | CFG_CMD_PCI \
								| CFG_CMD_FLASH | CFG_CMD_MEMORY \
								| CFG_CMD_ENV | CFG_CMD_CONSOLE \
								| CFG_CMD_LOADS | CFG_CMD_LOADB \
								| CFG_CMD_IMI | CFG_CMD_CACHE \
								| CFG_CMD_RUN | CFG_CMD_ECHO \
								| CFG_CMD_REGINFO | CFG_CMD_NET\
								| CFG_CMD_DHCP)

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any)
 */
#include <cmd_confdefs.h>


/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP			/* undef to save memory		*/
#define CFG_PROMPT	"=> "		/* Monitor Command Prompt	*/
#define CFG_CBSIZE	256		/* Console I/O Buffer Size	*/

/* Print Buffer Size */
#define CFG_PBSIZE	(CFG_CBSIZE + sizeof(CFG_PROMPT) + 16)

#define	CFG_MAXARGS	16		/* max number of command args	*/
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/
#define CFG_LOAD_ADDR	0x00100000	/* Default load address		*/


/*-----------------------------------------------------------------------
 * PCI configuration
 *-----------------------------------------------------------------------
 */
#define CONFIG_PCI				/* include pci support		*/
#undef CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_NET_MULTI
#define CONFIG_EEPRO100

#define PCI_ENET0_IOADDR	0x80000000
#define PCI_ENET0_MEMADDR	0x80000000
#define PCI_FIREWIRE_IOADDR	0x81000000
#define PCI_FIREWIRE_MEMADDR	0x81000000


/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define CFG_SDRAM_BASE	    0x00000000
#define CFG_MAX_RAM_SIZE    0x10000000	/* amount of SDRAM  */


/* even though FLASHP_BASE is FF800000, with 2MB on RCS0, the
 * reset vector is actually located at FF800100, but the 8245
 * takes care of us.
 */
#define CFG_RESET_ADDRESS   0xFFF00100

#define CFG_EUMB_ADDR	    0xFC000000

#define CFG_MONITOR_BASE    TEXT_BASE

#define CFG_MONITOR_LEN	    (256 << 10) /* Reserve 256 kB for Monitor	*/
#define CFG_MALLOC_LEN	    (128 << 10) /* Reserve 128 kB for malloc()	*/

/*#define CFG_DRAM_TEST		1 */
#define CFG_MEMTEST_START   0x00003000	/* memtest works on	0...256 MB	*/
#define CFG_MEMTEST_END	    0x0ff8ffa8	/* in SDRAM, skips exception */
										/* vectors and PPCBoot */


/*--------------------------------------------------------------------
 * Definitions for initial stack pointer and data area
 *------------------------------------------------------------------*/
#define CFG_GBL_DATA_SIZE    128	/* Size in bytes reserved for */
									/* initial data */
#define CFG_INIT_RAM_ADDR     0x40000000
#define CFG_INIT_RAM_END      0x1000
#define CFG_GBL_DATA_OFFSET  (CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)

/*--------------------------------------------------------------------
 * NS16550 Configuration
 *------------------------------------------------------------------*/
#define CFG_NS16550		
#define CFG_NS16550_SERIAL

#define CFG_NS16550_REG_SIZE	1

#define CFG_NS16550_CLK		get_bus_freq(0)

#define CFG_NS16550_COM1	(CFG_EUMB_ADDR + 0x4500)
#define CFG_NS16550_COM2	(CFG_EUMB_ADDR + 0x4600)

/*--------------------------------------------------------------------
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 * For the detail description refer to the MPC8240 user's manual.
 *------------------------------------------------------------------*/

#define CONFIG_SYS_CLK_FREQ  33000000
#define CFG_HZ				1000

#define CFG_ETH_DEV_FN	     0x7800
#define CFG_ETH_IOBASE	     0x00104000


/*--------------------------------------------------------------------
 *	Memory Control Configuration Register values
 *	- see sec. 4.12 of MPC8245 UM
 *------------------------------------------------------------------*/

/* MCCR1 */
#define CFG_ROMNAL	    0
#define CFG_ROMFAL	    2			/* (tacc=70ns)*mem_freq - 2 */
#define CFG_BANK0_ROW	2			/* SDRAM bank 7-0 row address */
#define CFG_BANK1_ROW	2			/* 	bit count */
#define CFG_BANK2_ROW	0
#define CFG_BANK3_ROW	0
#define CFG_BANK4_ROW	0
#define CFG_BANK5_ROW	0
#define CFG_BANK6_ROW	0
#define CFG_BANK7_ROW	0

/* MCCR2, refresh interval clock cycles */
#define CFG_REFINT	    480	    /* 33 MHz SDRAM clock was 480 */

/* Burst To Precharge. Bits of this value go to MCCR3 and MCCR4 */
#define CFG_BSTOPRE	    1023	/* burst to precharge[0..9], */
								/* sets open page interval */
																		
/* MCCR3 */
#define CFG_REFREC	    5	    /* Refresh to activate interval, trc */
								
/* MCCR4 */
#define CFG_PRETOACT	    2	    /* trp */
#define CFG_ACTTOPRE	    7	    /* trcd + (burst length - 1) + tdrl */
#define CFG_SDMODE_CAS_LAT  3	    /* SDMODE CAS latancy */
#define CFG_SDMODE_WRAP	    0	    /* SDMODE wrap type, sequential */
#define CFG_ACTORW	    	2		/* trcd min */
#define CFG_DBUS_SIZE2		1		/* set for 8-bit RCS1, clear for 32,64 */
#define CFG_REGISTERD_TYPE_BUFFER 1
#define CFG_EXTROM	    1
#define CFG_REGDIMM	    0

/* calculate according to formula in sec. 6-22 of 8245 UM */
#define CFG_PGMAX           50		/* how long the 8245 retains the */
									/* currently accessed page in memory */
									/* was 45 */
									
#define CFG_SDRAM_DSCD	0x20	/* SDRAM data in sample clock delay - note */
								/* bottom 3 bits MUST be 0 */
								
#define CFG_DLL_MAX_DELAY	0x04
#define CFG_DLL_EXTEND	0x80
#define CFG_PCI_HOLD_DEL 0x20
									

/* Memory bank settings.
 * Only bits 20-29 are actually used from these values to set the
 * start/end addresses. The upper two bits will always be 0, and the lower
 * 20 bits will be 0x00000 for a start address, or 0xfffff for an end
 * address. Refer to the MPC8245 user manual.
 */

#define CFG_BANK0_START	    0x00000000
#define CFG_BANK0_END	    (CFG_MAX_RAM_SIZE/2 - 1)
#define CFG_BANK0_ENABLE    1
#define CFG_BANK1_START	    CFG_MAX_RAM_SIZE/2
#define CFG_BANK1_END	    (CFG_MAX_RAM_SIZE - 1)
#define CFG_BANK1_ENABLE    1
#define CFG_BANK2_START	    0x3ff00000		/* not available in this design */
#define CFG_BANK2_END	    0x3fffffff
#define CFG_BANK2_ENABLE    0
#define CFG_BANK3_START	    0x3ff00000
#define CFG_BANK3_END	    0x3fffffff
#define CFG_BANK3_ENABLE    0
#define CFG_BANK4_START	    0x3ff00000
#define CFG_BANK4_END	    0x3fffffff
#define CFG_BANK4_ENABLE    0
#define CFG_BANK5_START	    0x3ff00000
#define CFG_BANK5_END	    0x3fffffff
#define CFG_BANK5_ENABLE    0
#define CFG_BANK6_START	    0x3ff00000
#define CFG_BANK6_END	    0x3fffffff
#define CFG_BANK6_ENABLE    0
#define CFG_BANK7_START	    0x3ff00000
#define CFG_BANK7_END	    0x3fffffff
#define CFG_BANK7_ENABLE    0

/*--------------------------------------------------------------------
 * 4.4 - Output Driver Control Register
 *------------------------------------------------------------------*/
#define CFG_ODCR	    0xe5

/*--------------------------------------------------------------------
 * 4.8 - Error Handling Registers
 *------------------------------------------------------------------*/
#define CFG_ERRENR1	0x11	/* enable SDRAM refresh overflow error */

/* SDRAM 0-256 MB */
#define CFG_IBAT0L  (CFG_SDRAM_BASE | BATL_PP_10 | BATL_MEMCOHERENCE)
#define CFG_IBAT0U  (CFG_SDRAM_BASE | BATU_BL_256M | BATU_VS | BATU_VP)

/* stack in dcache */
#define CFG_IBAT1L  (CFG_INIT_RAM_ADDR | BATL_PP_10 | BATL_MEMCOHERENCE)
#define CFG_IBAT1U  (CFG_INIT_RAM_ADDR | BATU_BL_128K | BATU_VS | BATU_VP)

/* PCI memory */
#define CFG_IBAT2L  (0x80000000 | BATL_PP_10 | BATL_CACHEINHIBIT)
#define CFG_IBAT2U  (0x80000000 | BATU_BL_256M | BATU_VS | BATU_VP)

/* Flash, config addrs, etc. */
#define CFG_IBAT3L  (0xF0000000 | BATL_PP_10 | BATL_CACHEINHIBIT)
#define CFG_IBAT3U  (0xF0000000 | BATU_BL_256M | BATU_VS | BATU_VP)

#define CFG_DBAT0L  CFG_IBAT0L
#define CFG_DBAT0U  CFG_IBAT0U
#define CFG_DBAT1L  CFG_IBAT1L
#define CFG_DBAT1U  CFG_IBAT1U
#define CFG_DBAT2L  CFG_IBAT2L
#define CFG_DBAT2U  CFG_IBAT2U
#define CFG_DBAT3L  CFG_IBAT3L
#define CFG_DBAT3U  CFG_IBAT3U

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CFG_BOOTMAPSZ	    (8 << 20)	/* Initial Memory map for Linux */

/*-----------------------------------------------------------------------
 * FLASH organization (AMD AM29LV116D)
 */
#define CFG_FLASH_BASE	    0xFF800000

#define CFG_MAX_FLASH_BANKS	1	/* Max number of flash banks */
#define CFG_MAX_FLASH_SECT	35	/* Max number of sectors in one bank */

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500		/* Timeout for Flash Write (in ms)	*/

	/* Warning: environment is not EMBEDDED in the ppcboot code.
	 * It's stored in flash separately.
	 */
#define CFG_ENV_IS_IN_FLASH	    1

#define CFG_ENV_ADDR		0xFF9FA000	/* flash sector SA33 */
#define CFG_ENV_SIZE		0x2000		/* Size of the Environment */
#define CFG_ENV_OFFSET		0			/* starting right at the beginning */
#define CFG_ENV_SECT_SIZE	0x2000 		/* Size of the Environment Sector */


#if CFG_MONITOR_BASE >= CFG_FLASH_BASE
#undef CFG_RAMBOOT
#else
#define CFG_RAMBOOT
#endif


/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_CACHELINE_SIZE	32
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#  define CFG_CACHELINE_SHIFT	5	/* log base 2 of the above value	*/
#endif

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD		0x01	/* Normal Power-On: Boot from FLASH	*/
#define BOOTFLAG_WARM		0x02	/* Software reboot			*/


#endif	/* __CONFIG_H */
