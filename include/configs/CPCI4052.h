/*
 * (C) Copyright 2001
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
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
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_405GP		1	/* This is a PPC405 CPU		*/
#define CONFIG_4xx		1	/* ...member of PPC4xx family   */
#define CONFIG_CPCI405		1	/* ...on a CPCI405 board	*/
#define CONFIG_CPCI405_VER2     1       /* ...version 2                 */

#define CONFIG_BOARD_PRE_INIT   1       /* call board_pre_init()        */

#define CONFIG_SYS_CLK_FREQ     33333333 /* external frequency to pll   */

#define CONFIG_BAUDRATE		9600
#define CONFIG_BOOTDELAY	3	/* autoboot after 3 seconds	*/

#if 0
#define CONFIG_PREBOOT                                                          \
        "crc32 f0207004 ffc 0;"                                                 \
        "if cmp 0 f0207000 1;"                                                  \
        "then;echo Old CRC is correct;crc32 f0207004 ff4 f0207000;"             \
        "else;echo Old CRC is bad;fi"
#endif

#undef	CONFIG_BOOTARGS
#define CONFIG_RAMBOOTCOMMAND							\
	"setenv bootargs root=/dev/ram rw nfsroot=$(serverip):$(rootpath) "	\
	"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask):$(hostname)::off;"	\
	"bootm ffc00000 ffca0000"
#define CONFIG_NFSBOOTCOMMAND							\
	"setenv bootargs root=/dev/nfs rw nfsroot=$(serverip):$(rootpath) "	\
	"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask):$(hostname)::off;"	\
	"bootm ffc00000"
#define CONFIG_BOOTCOMMAND CONFIG_RAMBOOTCOMMAND

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#define CFG_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

#define CONFIG_MII		1	/* MII PHY management		*/
#define	CONFIG_PHY_ADDR		0	/* PHY address			*/

#define CONFIG_RTC_M48T35A	1		/* ST Electronics M48 timekeeper */

#define CONFIG_COMMANDS	      ( CONFIG_CMD_DFL	| \
				CFG_CMD_PCI	| \
				CFG_CMD_IRQ	| \
				CFG_CMD_IDE	| \
				CFG_CMD_ELF	| \
				CFG_CMD_DATE	| \
				CFG_CMD_JFFS2	| \
				CFG_CMD_I2C	| \
				CFG_CMD_EEPROM  )

#define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#undef  CONFIG_WATCHDOG			/* watchdog disabled		*/

#define	CONFIG_SDRAM_BANK0	1	/* init onboard SDRAM bank 0	*/

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP			/* undef to save memory		*/
#define CFG_PROMPT	"=> "		/* Monitor Command Prompt	*/

#undef	CFG_HUSH_PARSER			/* use "hush" command parser	*/
#ifdef	CFG_HUSH_PARSER
#define	CFG_PROMPT_HUSH_PS2	"> "
#endif

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
#define	CFG_CBSIZE	256		/* Console I/O Buffer Size	*/
#endif
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS	16		/* max number of command args	*/
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_DEVICE_NULLDEV      1       /* include nulldev device       */

#define CFG_CONSOLE_INFO_QUIET  1       /* don't print console @ startup*/

#define CFG_MEMTEST_START	0x0400000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x0C00000	/* 4 ... 12 MB in DRAM	*/

#undef  CFG_EXT_SERIAL_CLOCK           /* no external serial clock used */
#define CFG_IGNORE_405_UART_ERRATA_59   /* ignore ppc405gp errata #59   */
#define CFG_BASE_BAUD       691200

/* The following table includes the supported baudrates */
#define CFG_BAUDRATE_TABLE      \
        { 300, 600, 1200, 2400, 4800, 9600, 19200, 38400,     \
         57600, 115200, 230400, 460800, 921600 }

#define CFG_LOAD_ADDR	0x100000	/* default load address */
#define CFG_EXTBDINFO	1		/* To use extended board_into (bd_t) */

#define	CFG_HZ		1000		/* decrementer freq: 1 ms ticks	*/

#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */

/*-----------------------------------------------------------------------
 * PCI stuff
 *-----------------------------------------------------------------------
 */
#define PCI_HOST_ADAPTER 0              /* configure as pci adapter     */
#define PCI_HOST_FORCE  1               /* configure as pci host        */
#define PCI_HOST_AUTO   2               /* detected via arbiter enable  */

#define CONFIG_PCI			/* include pci support	        */
#define CONFIG_PCI_HOST	PCI_HOST_AUTO   /* select pci host function     */
#define CONFIG_PCI_PNP			/* do pci plug-and-play         */
                                        /* resource configuration       */

#define CONFIG_PCI_SCAN_SHOW            /* print pci devices @ startup  */

#define CFG_PCI_SUBSYS_VENDORID 0x12FE  /* PCI Vendor ID: esd gmbh      */
#define CFG_PCI_SUBSYS_DEVICEID 0x0405  /* PCI Device ID: CPCI-405      */
#define CFG_PCI_SUBSYS_DEVICEID2 0x0406 /* PCI Device ID: CPCI-405-A    */
#define CFG_PCI_CLASSCODE       0x0b20  /* PCI Class Code: Processor/PPC*/
#define CFG_PCI_PTM1LA  0x00000000      /* point to sdram               */
#define CFG_PCI_PTM1MS  0xfc000001      /* 64MB, enable hard-wired to 1 */
#define CFG_PCI_PTM1PCI 0x00000000      /* Host: use this pci address   */
#define CFG_PCI_PTM2LA  0xffc00000      /* point to flash               */
#define CFG_PCI_PTM2MS  0xffc00001      /* 4MB, enable                  */
#define CFG_PCI_PTM2PCI 0x04000000      /* Host: use this pci address   */

/*-----------------------------------------------------------------------
 * IDE/ATA stuff
 *-----------------------------------------------------------------------
 */
#undef  CONFIG_IDE_8xx_DIRECT               /* no pcmcia interface required */
#undef  CONFIG_IDE_LED                  /* no led for ide supported     */
#define CONFIG_IDE_RESET	1	/* reset for ide supported	*/

#define	CFG_IDE_MAXBUS	        1		/* max. 1 IDE busses	*/
#define	CFG_IDE_MAXDEVICE	(CFG_IDE_MAXBUS*1) /* max. 1 drives per IDE bus */

#define	CFG_ATA_BASE_ADDR	0xF0100000
#define	CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_DATA_OFFSET	0x0000	/* Offset for data I/O			*/
#define	CFG_ATA_REG_OFFSET	0x0000	/* Offset for normal register accesses	*/
#define CFG_ATA_ALT_OFFSET	0x0000	/* Offset for alternate registers	*/

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define CFG_SDRAM_BASE		0x00000000
#define CFG_FLASH_BASE		0xFFFC0000
#define CFG_MONITOR_BASE	CFG_FLASH_BASE
#define CFG_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for Monitor	*/
#define CFG_MALLOC_LEN		(128 * 1024)	/* Reserve 128 kB for malloc()	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS	2	/* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	256	/* max number of sectors on one chip	*/

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define CFG_FLASH_WORD_SIZE     unsigned short  /* flash word size (width)      */
#define CFG_FLASH_ADDR0         0x5555  /* 1st address for flash config cycles  */
#define CFG_FLASH_ADDR1         0x2AAA  /* 2nd address for flash config cycles  */
/*
 * The following defines are added for buggy IOP480 byte interface.
 * All other boards should use the standard values (CPCI405 etc.)
 */
#define CFG_FLASH_READ0         0x0000  /* 0 is standard                        */
#define CFG_FLASH_READ1         0x0001  /* 1 is standard                        */
#define CFG_FLASH_READ2         0x0002  /* 2 is standard                        */

#define CFG_FLASH_EMPTY_INFO            /* print 'E' for empty sector on flinfo */

#define CFG_JFFS2_FIRST_BANK    0           /* use for JFFS2 */
#define CFG_JFFS2_NUM_BANKS     1           /* ! second bank contains ppcboot */

#if 0 /* Use NVRAM for environment variables */
/*-----------------------------------------------------------------------
 * NVRAM organization
 */
#define CFG_ENV_IS_IN_NVRAM	1	/* use NVRAM for environment vars	*/
#define CFG_ENV_SIZE		0x0ff8		/* Size of Environment vars	*/
#define CFG_ENV_ADDR		\
	(CFG_NVRAM_BASE_ADDR+CFG_NVRAM_SIZE-(CFG_ENV_SIZE+8))	/* Env	*/  

#else /* Use EEPROM for environment variables */

#define CFG_ENV_IS_IN_EEPROM    1       /* use EEPROM for environment vars */
#define CFG_ENV_OFFSET          0x000   /* environment starts at the beginning of the EEPROM */
#define CFG_ENV_SIZE            0x800   /* 2048 bytes may be used for env vars*/
                                   /* total size of a CAT24WC16 is 2048 bytes */
#endif

#define CFG_NVRAM_BASE_ADDR	0xf0200000		/* NVRAM base address	*/
#define CFG_NVRAM_SIZE		(32*1024)		/* NVRAM size		*/
#define CFG_NVRAM_VXWORKS_OFFS	0x6900		/* Offset for VxWorks eth-addr	*/

/*-----------------------------------------------------------------------
 * I2C EEPROM (CAT24WC16) for environment
 */
#define CONFIG_HARD_I2C			/* I2c with hardware support */
#define CFG_I2C_SPEED		400000	/* I2C speed and slave address */
#define CFG_I2C_SLAVE		0x7F

#define CFG_I2C_EEPROM_ADDR	0x50	/* EEPROM CAT28WC08		*/
#define CFG_I2C_EEPROM_ADDR_LEN	1	/* Bytes of address		*/
/* mask of address bits that overflow into the "EEPROM chip address"    */
#define CFG_I2C_EEPROM_ADDR_OVERFLOW	0x07
#define CFG_EEPROM_PAGE_WRITE_BITS 4	/* The Catalyst CAT24WC08 has	*/
					/* 16 byte page write mode using*/
					/* last	4 bits of the address	*/
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS	10   /* and takes up to 10 msec */
#define CFG_EEPROM_PAGE_WRITE_ENABLE 

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		16384	/* For IBM 405 CPUs, older 405 ppc's    */
                                        /* have only 8kB, 16kB is save here     */
#define CFG_CACHELINE_SIZE	32	/* ...			*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT	5	/* log base 2 of the above value	*/
#endif

/*
 * Init Memory Controller:
 *
 * BR0/1 and OR0/1 (FLASH)
 */

#define FLASH_BASE0_PRELIM	0xFF800000	/* FLASH bank #0	*/
#define FLASH_BASE1_PRELIM	0xFFC00000	/* FLASH bank #1	*/

/*-----------------------------------------------------------------------
 * External Bus Controller (EBC) Setup
 */

/* Memory Bank 0 (Flash Bank 0) initialization                                  */
#define CFG_EBC_PB0AP           0x92015480
#define CFG_EBC_PB0CR           0xFFC5A000  /* BAS=0xFFC,BS=4MB,BU=R/W,BW=16bit */

/* Memory Bank 1 (Flash Bank 1) initialization                                  */
#define CFG_EBC_PB1AP           0x92015480
#define CFG_EBC_PB1CR           0xFF85A000  /* BAS=0xFF8,BS=4MB,BU=R/W,BW=16bit */

/* Memory Bank 2 (CAN0, 1) initialization                                       */
#define CFG_EBC_PB2AP           0x010053C0  /* BWT=2,WBN=1,WBF=1,TH=1,RE=1,SOR=1,BEM=1 */
#define CFG_EBC_PB2CR           0xF0018000  /* BAS=0xF00,BS=1MB,BU=R/W,BW=8bit  */

/* Memory Bank 3 (CompactFlash IDE) initialization                              */
#define CFG_EBC_PB3AP           0x010053C0  /* BWT=2,WBN=1,WBF=1,TH=1,RE=1,SOR=1,BEM=1 */
#define CFG_EBC_PB3CR           0xF011A000  /* BAS=0xF01,BS=1MB,BU=R/W,BW=16bit */

/* Memory Bank 4 (NVRAM/RTC) initialization                                     */
#define CFG_EBC_PB4AP           0x01005280  /* TWT=2,WBN=1,WBF=1,TH=1,SOR=1     */
#define CFG_EBC_PB4CR           0xF0218000  /* BAS=0xF02,BS=1MB,BU=R/W,BW=8bit  */

/* Memory Bank 5 (optional Quart) initialization                                */
#define CFG_EBC_PB5AP           0x04005B80  /* TWT=8,WBN=1,WBF=1,TH=5,RE=1,SOR=1*/
#define CFG_EBC_PB5CR           0xF0318000  /* BAS=0xF03,BS=1MB,BU=R/W,BW=8bit  */

/* Memory Bank 6 (FPGA internal) initialization                                 */
#define CFG_EBC_PB6AP           0x010053C0  /* BWT=2,WBN=1,WBF=1,TH=1,RE=1,SOR=1,BEM=1 */
#define CFG_EBC_PB6CR           0xF041A000  /* BAS=0xF01,BS=1MB,BU=R/W,BW=16bit */
#define CFG_FPGA_BASE_ADDR      0xF0400000

/*-----------------------------------------------------------------------
 * FPGA stuff
 */
/* FPGA internal regs */
#define CFG_FPGA_MODE           0x00
#define CFG_FPGA_STATUS         0x02
#define CFG_FPGA_TS             0x04
#define CFG_FPGA_TS_LOW         0x06
#define CFG_FPGA_TS_CAP0        0x10
#define CFG_FPGA_TS_CAP0_LOW    0x12
#define CFG_FPGA_TS_CAP1        0x14
#define CFG_FPGA_TS_CAP1_LOW    0x16
#define CFG_FPGA_TS_CAP2        0x18
#define CFG_FPGA_TS_CAP2_LOW    0x1a
#define CFG_FPGA_TS_CAP3        0x1c
#define CFG_FPGA_TS_CAP3_LOW    0x1e

/* FPGA Mode Reg */
#define CFG_FPGA_MODE_CF_RESET  0x0001
#define CFG_FPGA_MODE_TS_IRQ_ENABLE 0x0100
#define CFG_FPGA_MODE_TS_IRQ_CLEAR  0x1000
#define CFG_FPGA_MODE_TS_CLEAR  0x2000

/* FPGA Status Reg */
#define CFG_FPGA_STATUS_DIP0    0x0001
#define CFG_FPGA_STATUS_DIP1    0x0002
#define CFG_FPGA_STATUS_DIP2    0x0004
#define CFG_FPGA_STATUS_FLASH   0x0008
#define CFG_FPGA_STATUS_TS_IRQ  0x1000

#define CFG_FPGA_SPARTAN2       1           /* using Xilinx Spartan 2 now    */
#define CFG_FPGA_MAX_SIZE       32*1024     /* 32kByte is enough for XC2S15  */

/* FPGA program pin configuration */
#define CFG_FPGA_PRG            0x04000000  /* FPGA program pin (ppc output) */
#define CFG_FPGA_CLK            0x02000000  /* FPGA clk pin (ppc output)     */
#define CFG_FPGA_DATA           0x01000000  /* FPGA data pin (ppc output)    */
#define CFG_FPGA_INIT           0x00010000  /* FPGA init pin (ppc input)     */
#define CFG_FPGA_DONE           0x00008000  /* FPGA done pin (ppc input)     */

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in data cache)
 */
#if 1 /* test-only */
#define CFG_INIT_DCACHE_CS      7       /* use cs # 7 for data cache memory    */

#define CFG_INIT_RAM_ADDR       0x40000000  /* use data cache                  */
#else
#define CFG_INIT_RAM_ADDR	0x00df0000 /* inside of SDRAM                   */
#endif
#define CFG_INIT_RAM_END        0x2000  /* End of used area in RAM             */
#define CFG_GBL_DATA_SIZE      128  /* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET    (CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET      CFG_GBL_DATA_OFFSET


/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH	*/
#define BOOTFLAG_WARM	0x02		/* Software reboot			*/

#endif	/* __CONFIG_H */
