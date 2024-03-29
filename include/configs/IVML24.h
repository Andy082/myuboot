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

/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_MPC860		1	/* This is a MPC860 CPU		*/
#define CONFIG_IVML24		1	/* ...on a IVML24 board		*/

#if defined (CONFIG_IVML24_16M)
# define CONFIG_IDENT_STRING     " IVML24"
#elif defined (CONFIG_IVML24_32M)
# define CONFIG_IDENT_STRING     " IVML24_128"
#elif defined (CONFIG_IVML24_64M)
# define CONFIG_IDENT_STRING     " IVML24_256"
#endif

#define	CONFIG_8xx_CONS_SMC1	1	/* Console is on SMC1		*/
#undef	CONFIG_8xx_CONS_SMC2
#undef	CONFIG_8xx_CONS_NONE
#define CONFIG_BAUDRATE		115200

#define	CONFIG_CLOCKS_IN_MHZ	1	/* clocks passsed to Linux in MHz */
#define CONFIG_8xx_GCLK_FREQ    50331648

#define	CONFIG_SHOW_BOOT_PROGRESS 1	/* Show boot progress on LEDs	*/

#if 0
#define CONFIG_BOOTDELAY	-1	/* autoboot disabled		*/
#else
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#endif
#define CONFIG_BOOTCOMMAND	"bootp" /* autoboot command		*/

#define CONFIG_BOOTARGS		"root=/dev/nfs rw "			\
				"nfsroot=10.0.0.2:/opt/eldk/ppc_8xx "	\
				"nfsaddrs=10.0.0.99:10.0.0.2"

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#undef	CFG_LOADS_BAUD_CHANGE		/* don't allow baudrate change	*/

#undef	CONFIG_WATCHDOG			/* watchdog disabled		*/

#define	CONFIG_STATUS_LED	1	/* Status LED enabled		*/

#define CONFIG_COMMANDS		(CONFIG_CMD_DFL | CFG_CMD_IDE)
#define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION

#define CONFIG_BOOTP_MASK \
    ((CONFIG_BOOTP_DEFAULT | CONFIG_BOOTP_BOOTFILESIZE) & ~CONFIG_BOOTP_GATEWAY)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/*----------------------------------------------------------------------*/

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP			/* undef to save memory		*/
#define	CFG_PROMPT	"=> "		/* Monitor Command Prompt	*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
#define	CFG_CBSIZE	256		/* Console I/O Buffer Size	*/
#endif
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS	16		/* max number of command args	*/
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x00100000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x00F00000	/* 1 ... 15MB in DRAM	*/

#define	CFG_LOAD_ADDR		0x00100000	/* default load address	*/

#define	CFG_PIO_MODE		0	/* IDE interface in PIO Mode 0	*/

#define CFG_PB_12V_ENABLE	0x00002000		/* PB 18	*/
#define CFG_PB_ILOCK_SWITCH	0x00004000		/* PB 17	*/
#define CFG_PB_SDRAM_CLKE	0x00008000		/* PB 16	*/
#define CFG_PB_ETH_POWERDOWN	0x00010000		/* PB 15	*/
#define CFG_PB_IDE_MOTOR	0x00020000		/* PB 14	*/

#define CFG_PC_ETH_RESET	((ushort)0x0010)	/* PC 11	*/
#define CFG_PC_IDE_RESET	((ushort)0x0020)	/* PC 10	*/

#define	CFG_HZ		1000		/* decrementer freq: 1 ms ticks	*/

#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */
/*-----------------------------------------------------------------------
 * Internal Memory Mapped Register
 */
#define CFG_IMMR		0xFFF00000 /* was: 0xFF000000 */

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CFG_INIT_RAM_ADDR	CFG_IMMR

#if defined (CONFIG_IVML24_16M)
# define	CFG_INIT_RAM_END	0x2F00	/* End of used area in DPRAM	*/
#elif defined (CONFIG_IVML24_32M)
# define	CFG_INIT_RAM_END	0x3000	/* End of used area in DPRAM	*/
#elif defined (CONFIG_IVML24_64M)
# define	CFG_INIT_RAM_END	0x3000	/* End of used area in DPRAM	*/
#endif

#define	CFG_GBL_DATA_SIZE	64  /* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define	CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_FLASH_BASE		0xFF000000
#ifdef	DEBUG
#define	CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#else
#define	CFG_MONITOR_LEN		(192 << 10)	/* Reserve 192 kB for Monitor	*/
#endif
#define CFG_MONITOR_BASE	CFG_FLASH_BASE
#define	CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux	*/
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	256	/* max number of sectors on one chip	*/

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define	CFG_ENV_IS_IN_FLASH	1
#define	CFG_ENV_OFFSET		0x7A000	/*   Offset   of Environment Sector	*/
#define	CFG_ENV_SIZE		0x2000	/* Total Size of Environment Sector	*/
/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_CACHELINE_SIZE	16	/* For all MPC8xx CPUs			*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT	4	/* log base 2 of the above value	*/
#endif

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control				11-9
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Software & Bus Monitor Timer max, Bus Monitor enable, SW Watchdog freeze
 */
#if defined(CONFIG_WATCHDOG)

# if defined (CONFIG_IVML24_16M)
#  define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
 			 SYPCR_SWE  | SYPCR_SWRI| SYPCR_SWP)
# elif defined (CONFIG_IVML24_32M)
#  define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWE  | SYPCR_SWP)
# elif defined (CONFIG_IVML24_64M)
#  define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWE  | SYPCR_SWP)
# endif

#else
#define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | SYPCR_SWP)
#endif

/*-----------------------------------------------------------------------
 * SIUMCR - SIU Module Configuration				11-6
 *-----------------------------------------------------------------------
 * PCMCIA config., multi-function pin tri-state
 */
/* EARB, DBGC and DBPC are initialised by the HCW */
/* => 0x000000C0 */
#define CFG_SIUMCR	(SIUMCR_BSC | SIUMCR_GB5E)

/*-----------------------------------------------------------------------
 * TBSCR - Time Base Status and Control				11-26
 *-----------------------------------------------------------------------
 * Clear Reference Interrupt Status, Timebase freezing enabled
 */
#define CFG_TBSCR	(TBSCR_REFA | TBSCR_REFB | TBSCR_TBF)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control		11-31
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Interrupt Timer freezing enabled
 */
#define CFG_PISCR	(PISCR_PS | PISCR_PITF)

/*-----------------------------------------------------------------------
 * PLPRCR - PLL, Low-Power, and Reset Control Register		15-30
 *-----------------------------------------------------------------------
 * Reset PLL lock status sticky bit, timer expired status bit and timer
 * interrupt status bit, set PLL multiplication factor !
 */
/* 0x00B0C0C0 */
#define CFG_PLPRCR							\
		(	(11 << PLPRCR_MF_SHIFT) |			\
			PLPRCR_SPLSS | PLPRCR_TEXPS | /*PLPRCR_TMIST|*/	\
			/*PLPRCR_CSRC|*/ PLPRCR_LPM_NORMAL |		\
			PLPRCR_CSR   | PLPRCR_LOLRE /*|PLPRCR_FIOPD*/	\
		)

/*-----------------------------------------------------------------------
 * SCCR - System Clock and reset Control Register		15-27
 *-----------------------------------------------------------------------
 * Set clock output, timebase and RTC source and divider,
 * power management and some other internal clocks
 */
#define SCCR_MASK	SCCR_EBDF11
/* 0x01800014 */
#define CFG_SCCR	(SCCR_COM01	| /*SCCR_TBS|*/		\
			 SCCR_RTDIV	|   SCCR_RTSEL	  |	\
			 /*SCCR_CRQEN|*/  /*SCCR_PRQEN|*/ 	\
			 SCCR_EBDF00	|   SCCR_DFSYNC00 |	\
			 SCCR_DFBRG00	|   SCCR_DFNL000  |	\
			 SCCR_DFNH000	|   SCCR_DFLCD101 |	\
			 SCCR_DFALCD00)

/*-----------------------------------------------------------------------
 * RTCSC - Real-Time Clock Status and Control Register		11-27
 *-----------------------------------------------------------------------
 */
/* 0x00C3 */
#define CFG_RTCSC	(RTCSC_SEC | RTCSC_ALR | RTCSC_RTF| RTCSC_RTE)


/*-----------------------------------------------------------------------
 * RCCR - RISC Controller Configuration Register		19-4
 *-----------------------------------------------------------------------
 */
/* TIMEP=2 */
#define CFG_RCCR 0x0200

/*-----------------------------------------------------------------------
 * RMDS - RISC Microcode Development Support Control Register
 *-----------------------------------------------------------------------
 */
#define CFG_RMDS 0

/*-----------------------------------------------------------------------
 *
 * Interrupt Levels
 *-----------------------------------------------------------------------
 */
#define CFG_CPM_INTERRUPT	13	/* SIU_LEVEL6	*/

/*-----------------------------------------------------------------------
 * PCMCIA stuff
 *-----------------------------------------------------------------------
 *
 */
#define CFG_PCMCIA_MEM_ADDR	(0xE0000000)
#define CFG_PCMCIA_MEM_SIZE	( 64 << 20 )
#define CFG_PCMCIA_DMA_ADDR	(0xE4000000)
#define CFG_PCMCIA_DMA_SIZE	( 64 << 20 )
#define CFG_PCMCIA_ATTRB_ADDR	(0xE8000000)
#define CFG_PCMCIA_ATTRB_SIZE	( 64 << 20 )
#define CFG_PCMCIA_IO_ADDR	(0xEC000000)
#define CFG_PCMCIA_IO_SIZE	( 64 << 20 )

/*-----------------------------------------------------------------------
 * IDE/ATA stuff
 *-----------------------------------------------------------------------
 */
#define CONFIG_IDE_8xx_DIRECT	1	/* PCMCIA interface required	*/
#define CONFIG_IDE_RESET	1	/* reset for ide supported	*/

#define CFG_IDE_MAXBUS		1	/* The IVML24 has only 1 IDE bus*/
#define CFG_IDE_MAXDEVICE	1	/*    ... and only 1 IDE device	*/

#define CFG_ATA_BASE_ADDR	0xFE100000
#define CFG_ATA_IDE0_OFFSET	0x0000
#undef	CFG_ATA_IDE1_OFFSET		/* only one IDE bus available	*/

#define CFG_ATA_DATA_OFFSET	0x0000	/* Offset for data I/O			*/
#define CFG_ATA_REG_OFFSET	0x0080	/* Offset for normal register accesses	*/
#define CFG_ATA_ALT_OFFSET	0x0100	/* Offset for alternate registers	*/

/*-----------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------
 *
 */
/*#define	CFG_DER	0x2002000F*/
#define CFG_DER	0

/*
 * Init Memory Controller:
 *
 * BR0 and OR0 (FLASH)
 */

#define FLASH_BASE0_PRELIM	0xFF000000	/* FLASH bank #0	*/

/* used to re-map FLASH both when starting from SRAM or FLASH:
 * restrict access enough to keep SRAM working (if any)
 * but not too much to meddle with FLASH accesses
 */
/* EPROMs are 512kb */
#define CFG_REMAP_OR_AM		0xFFF80000	/* OR addr mask */
#define CFG_PRELIM_OR_AM	0xFFF80000	/* OR addr mask */

/* FLASH timing: ACS = 11, TRLX = 0, CSNT = 1, SCY = 5, EHTR = 1	*/
#define CFG_OR_TIMING_FLASH	(OR_SCY_5_CLK | OR_EHTR)

#define CFG_OR0_REMAP	( CFG_REMAP_OR_AM | OR_ACS_DIV4 | OR_BI | \
				CFG_OR_TIMING_FLASH)
#define CFG_OR0_PRELIM	(CFG_PRELIM_OR_AM | OR_ACS_DIV4 | OR_BI | \
				CFG_OR_TIMING_FLASH)
/* 16 bit, bank valid */
#define CFG_BR0_PRELIM	((FLASH_BASE0_PRELIM & BR_BA_MSK) | BR_PS_16 | BR_V )

/*
 * BR1/OR1 - ELIC SACCO bank  @ 0xFE000000
 *
 * AM=0xFFFF8 ATM=0 CSNT/SAM=1 ACS/G5LA/G5LS=3 BIH=1 SCY=2 SETA=0 TRLX=1 EHTR=1
 */
#define ELIC_SACCO_BASE		0xFE000000
#define ELIC_SACCO_OR_AM	0xFFFF8000
#define ELIC_SACCO_TIMING	(OR_SCY_2_CLK | OR_TRLX | OR_EHTR)

#define CFG_OR1	(ELIC_SACCO_OR_AM | OR_CSNT_SAM | OR_ACS_DIV2 | OR_BI | \
			ELIC_SACCO_TIMING)
#define CFG_BR1	((ELIC_SACCO_BASE & BR_BA_MSK) | BR_PS_8 | BR_V )

/*
 * BR2/OR2 - ELIC EPIC bank   @ 0xFE008000
 *
 * AM=0xFFFF8 ATM=0 CSNT/SAM=1 ACS/G5LA/G5LS=3 BIH=1 SCY=2 SETA=0 TRLX=1 EHTR=1
 */
#define ELIC_EPIC_BASE		0xFE008000
#define ELIC_EPIC_OR_AM		0xFFFF8000
#define ELIC_EPIC_TIMING	(OR_SCY_2_CLK | OR_TRLX | OR_EHTR)

#define CFG_OR2 (ELIC_EPIC_OR_AM | OR_CSNT_SAM | OR_ACS_DIV2 | OR_BI | \
			ELIC_EPIC_TIMING)
#define CFG_BR2	((ELIC_EPIC_BASE & BR_BA_MSK) | BR_PS_8 | BR_V )

/*
 * BR3/OR3: SDRAM
 *
 * Multiplexed addresses, GPL5 output to GPL5_A (don't care)
 */
#define SDRAM_BASE3_PRELIM	0x00000000	/* SDRAM bank */
#define SDRAM_PRELIM_OR_AM	0xF8000000	/* map max. 128 MB */
#define SDRAM_TIMING		OR_SCY_0_CLK	/* SDRAM-Timing */

#define SDRAM_MAX_SIZE		0x04000000	/* max 64 MB SDRAM */

#define CFG_OR3_PRELIM	(SDRAM_PRELIM_OR_AM | OR_CSNT_SAM | OR_G5LS | SDRAM_TIMING )
#define CFG_BR3_PRELIM	((SDRAM_BASE3_PRELIM & BR_BA_MSK) | BR_MS_UPMB | BR_V )

/*
 * BR4/OR4 - HDLC Address
 *
 *  AM=0xFFFF8 ATM=0 CSNT/SAM=0 ACS/G5LA/G5LS=0 BIH=1 SCY=1 SETA=0 TRLX=0 EHTR=0
 */
#define HDLC_ADDR_BASE		0xFE108000	/* HDLC Address area */
#define HDLC_ADDR_OR_AM		0xFFFF8000
#define HDLC_ADDR_TIMING	OR_SCY_1_CLK

#define CFG_OR4	(HDLC_ADDR_OR_AM | OR_BI | HDLC_ADDR_TIMING)
#define CFG_BR4	((HDLC_ADDR_BASE & BR_BA_MSK) | BR_PS_8 | BR_WP | BR_V )

/*
 * BR5/OR5: SHARC ADSP-2165L
 *
 * AM=0xFFC00 ATM=0 CSNT/SAM=0 ACS/G5LA/G5LS=3 BIH=1 SCY=0 SETA=0 TRLX=0 EHTR=0
 */
#define SHARC_BASE		0xFE400000
#define SHARC_OR_AM		0xFFC00000
#define SHARC_TIMING		OR_SCY_0_CLK

#define CFG_OR5	(SHARC_OR_AM | OR_ACS_DIV2 | OR_BI | SHARC_TIMING )
#define CFG_BR5	((SHARC_BASE & BR_BA_MSK) | BR_PS_32 | BR_MS_UPMA | BR_V )

/*
 * Memory Periodic Timer Prescaler
 */

/* periodic timer for refresh */
#define CFG_MAMR_PTB	204

/* refresh rate 15.6 us (= 64 ms / 4K = 62.4 / quad bursts) for <= 128 MBit	*/
#define CFG_MPTPR_2BK_4K	MPTPR_PTP_DIV16		/* setting for 2 banks	*/
#define CFG_MPTPR_1BK_4K	MPTPR_PTP_DIV32		/* setting for 1 bank	*/

/* refresh rate 7.8 us (= 64 ms / 8K = 31.2 / quad bursts) for 256 MBit		*/
#define CFG_MPTPR_2BK_8K	MPTPR_PTP_DIV8		/* setting for 2 banks	*/

#if defined (CONFIG_IVML24_16M)
# define CFG_MPTPR_1BK_8K	MPTPR_PTP_DIV16		/* setting for 1 bank	*/
#elif defined (CONFIG_IVML24_32M)
# define CFG_MPTPR_1BK_8K	MPTPR_PTP_DIV16		/* setting for 1 bank	*/
#elif defined (CONFIG_IVML24_64M)
# define CFG_MPTPR_1BK_8K	MPTPR_PTP_DIV8		/* setting for 1 bank	*/
#endif


/*
 * MBMR settings for SDRAM
 */

#if defined (CONFIG_IVML24_16M)
 /* 8 column SDRAM */
# define CFG_MBMR_8COL	((CFG_MAMR_PTB << MAMR_PTB_SHIFT)  | \
 			 MAMR_AMB_TYPE_0 | MAMR_DSB_1_CYCL | MAMR_G0CLB_A11 |	\
 			 MAMR_RLFB_1X	 | MAMR_WLFB_1X	   | MAMR_TLFB_4X)
#elif defined (CONFIG_IVML24_32M)
/* 128 MBit SDRAM */
# define CFG_MBMR_8COL	((CFG_MAMR_PTB << MAMR_PTB_SHIFT)  | \
			 MAMR_AMB_TYPE_1 | MAMR_DSB_1_CYCL | MAMR_G0CLB_A10 |	\
			 MAMR_RLFB_1X	 | MAMR_WLFB_1X	   | MAMR_TLFB_4X)
#elif defined (CONFIG_IVML24_64M)
/* 128 MBit SDRAM */
# define CFG_MBMR_8COL	((CFG_MAMR_PTB << MAMR_PTB_SHIFT)  | \
			 MAMR_AMB_TYPE_1 | MAMR_DSB_1_CYCL | MAMR_G0CLB_A10 |	\
			 MAMR_RLFB_1X	 | MAMR_WLFB_1X	   | MAMR_TLFB_4X)
#endif

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define	BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH	*/
#define BOOTFLAG_WARM	0x02		/* Software reboot			*/

#endif	/* __CONFIG_H */
