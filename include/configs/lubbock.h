/*
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Configuation settings for the LUBBOCK board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * If we are developing, we might want to start armboot from ram
 * so we MUST NOT initialize critical regs like mem-timing ...
 */
#define CONFIG_INIT_CRITICAL            /* undef for developing */

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_PXA250           1       /* This is an PXA250 CPU    */
#define CONFIG_LUBBOCK          1       /* on an LUBBOCK Board      */

#undef CONFIG_USE_IRQ                   /* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CONFIG_MALLOC_SIZE      (CFG_ENV_SIZE + 128*1024)

/*
 * Hardware drivers
 */

/*
 * select serial console configuration
 */
#define CONFIG_FFUART          1       /* we use FFUART on LUBBOCK */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE         115200

#define CONFIG_COMMANDS         (CONFIG_CMD_DFL & ~CFG_CMD_NET)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY        3
#define CONFIG_BOOTARGS         "root=ramfs devfs=mount console=ttySA0,9600"
#define CONFIG_ETHADDR          08:00:3e:26:0a:5b
#define CONFIG_NETMASK          255.255.0.0
#define CONFIG_IPADDR           192.168.0.21
#define CONFIG_SERVERIP         192.168.0.250
#define CONFIG_BOOTCOMMAND      "FIXME"

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE    230400          /* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX   2               /* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP                            /* undef to save memory         */
#define CFG_PROMPT              "=> "   /* Monitor Command Prompt       */
#define CFG_CBSIZE              256             /* Console I/O Buffer Size      */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS             16              /* max number of command args   */
#define CFG_BARGSIZE            CFG_CBSIZE      /* Boot Argument Buffer Size    */

#define CFG_MEMTEST_START       0xa0400000      /* memtest works on     */
#define CFG_MEMTEST_END         0xa0800000      /* 4 ... 8 MB in DRAM   */

#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR           0xa8000000      /* default load address */

#define CFG_HZ                  3686400         /* incrementer freq: 3.6864 MHz */
#define CFG_CPUSPEED            0x141           /* set core clock to 200/200/100 MHz */

                                                /* valid baudrates */
#define CFG_BAUDRATE_TABLE      { 9600, 19200, 38400, 57600, 115200 }

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    4          /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1            0xa0000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       0x04000000 /* 64 MB */
#define PHYS_SDRAM_2            0xa4000000 /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE       0x00000000 /* 0 MB */
#define PHYS_SDRAM_3            0xa8000000 /* SDRAM Bank #3 */
#define PHYS_SDRAM_3_SIZE       0x00000000 /* 0 MB */
#define PHYS_SDRAM_4            0xac000000 /* SDRAM Bank #4 */
#define PHYS_SDRAM_4_SIZE       0x00000000 /* 0 MB */

#define PHYS_FLASH_1            0x00000000 /* Flash Bank #1 */
#define PHYS_FLASH_2            0x04000000 /* Flash Bank #1 */
#define PHYS_FLASH_SIZE         0x02000000 /* 32 MB */

#define CFG_DRAM_BASE           0xa0000000
#define CFG_DRAM_SIZE           0x04000000

#define CFG_FLASH_BASE          PHYS_FLASH_1

#define FPGA_REGS_BASE_PHYSICAL 0x08000000

/*
 * GPIO settings
 */
#define CFG_GPSR0_VAL       0x00008000
#define CFG_GPSR1_VAL       0x00FC0382
#define CFG_GPSR2_VAL       0x0001FFFF
#define CFG_GPCR0_VAL       0x00000000
#define CFG_GPCR1_VAL       0x00000000
#define CFG_GPCR2_VAL       0x00000000
#define CFG_GPDR0_VAL       0x0060A800
#define CFG_GPDR1_VAL       0x00FF0382
#define CFG_GPDR2_VAL       0x0001C000
#define CFG_GAFR0_L_VAL     0x98400000
#define CFG_GAFR0_U_VAL     0x00002950
#define CFG_GAFR1_L_VAL     0x000A9558
#define CFG_GAFR1_U_VAL     0x0005AAAA
#define CFG_GAFR2_L_VAL     0xA0000000
#define CFG_GAFR2_U_VAL     0x00000002

#define CFG_PSSR_VAL        0x20

/*
 * Memory settings
 */
#define CFG_MSC0_VAL        0x23F223F2
#define CFG_MSC1_VAL        0x3FF1A441
#define CFG_MSC2_VAL        0x7FF17FF1
#define CFG_MDCNFG_VAL      0x00001AC9
#define CFG_MDREFR_VAL      0x000BC018
#define CFG_MDREFR_VAL_100  0x00018018
#define CFG_MDMRS_VAL       0x00000000

/*
 * PCMCIA and CF Interfaces
 */
#define CFG_MECR_VAL        0x00000000
#define CFG_MCMEM0_VAL      0x00010504
#define CFG_MCMEM1_VAL      0x00010504
#define CFG_MCATT0_VAL      0x00010504
#define CFG_MCATT1_VAL      0x00010504
#define CFG_MCIO0_VAL       0x00004715
#define CFG_MCIO1_VAL       0x00004715

#define _LED        0x08000010
#define LED_BLANK  (0x08000040)

/*
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     2       /* max number of memory banks           */
#define CFG_MAX_FLASH_SECT      128  /* max number of sectors on one chip    */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT    (2*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (2*CFG_HZ) /* Timeout for Flash Write */

/* FIXME */
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR            (PHYS_FLASH_1 + 0x1C000)        /* Addr of Environment Sector   */
#define CFG_ENV_SIZE            0x4000  /* Total Size of Environment Sector     */


/*
 * FPGA Offsets
 */
#define WHOAMI_OFFSET           0x00
#define HEXLED_OFFSET           0x10
#define BLANKLED_OFFSET         0x40
#define DISCRETELED_OFFSET      0x40
#define CNFG_SWITCHES_OFFSET    0x50
#define USER_SWITCHES_OFFSET    0x60
#define MISC_WR_OFFSET          0x80
#define MISC_RD_OFFSET          0x90
#define INT_MASK_OFFSET         0xC0
#define INT_CLEAR_OFFSET        0xD0
#define GP_OFFSET               0x100

#endif  /* __CONFIG_H */
