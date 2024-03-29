/*
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#define CONFIG_HHP_CRADLE       1       /* on an Cradle Board       */

#undef CONFIG_USE_IRQ                   /* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CONFIG_MALLOC_SIZE      (CFG_ENV_SIZE + 128*1024)

/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_SMC91111
#define CONFIG_SMC91111_BASE 0x10000300
#define CONFIG_SMC91111_EXT_PHY
#define CONFIG_SMC_USE_32_BIT

/*
 * select serial console configuration
 */
#define CONFIG_FFUART          1       /* we use FFUART on LUBBOCK */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE         115200

#define CONFIG_COMMANDS         (CONFIG_CMD_DFL)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY        3
#define CONFIG_BOOTARGS         "root=/dev/mtdblock2 console=ttyS0,115200"
#define CONFIG_ETHADDR          08:00:3e:26:0a:5b
#define CONFIG_NETMASK          255.255.0.0
#define CONFIG_IPADDR           192.168.0.21
#define CONFIG_SERVERIP         192.168.0.250
#define CONFIG_BOOTCOMMAND      "bootm 40000"
#define CONFIG_CMDLINE_TAG

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

#define CFG_LOAD_ADDR           0xa2000000      /* default load address */

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
#define PHYS_SDRAM_1_SIZE       0x01000000 /* 64 MB */
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

/*
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     1     /* max number of memory banks           */
#define CFG_MAX_FLASH_SECT      32    /* max number of sectors on one chip    */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT    (2*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (2*CFG_HZ) /* Timeout for Flash Write */

#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR				0x00020000	/* absolute address for now   */
#define CFG_ENV_SIZE				0x20000	   /* 8K ouch, this may later be */

/******************************************************************************
 *
 * CPU specific defines
 *
 ******************************************************************************/

/*
 * GPIO settings
 *
 * GPIO pin assignments
 * GPIO     Name        Dir Out AF
 * 0        NC
 * 1        NC
 * 2        SIRQ1       I
 * 3        SIRQ2       I
 * 4        SIRQ3       I
 * 5        DMAACK1     O   0
 * 6        DMAACK2     O   0
 * 7        DMAACK3     O   0
 * 8        TC1         O   0
 * 9        TC2         O   0
 * 10       TC3         O   0
 * 11       nDMAEN      O   1
 * 12       AENCTRL     O   0
 * 13       PLDTC       O   0
 * 14       ETHIRQ      I
 * 15       NC
 * 16       NC
 * 17       NC
 * 18       RDY         I
 * 19       DMASIO      I
 * 20       ETHIRQ      NC
 * 21       NC
 * 22       PGMEN       O   1    FIXME for debug only enable flash
 * 23       NC
 * 24       NC
 * 25       NC
 * 26       NC
 * 27       NC
 * 28       NC
 * 29       NC
 * 30       NC
 * 31       NC
 * 32       NC
 * 33       NC
 * 34       FFRXD       I       01
 * 35       FFCTS       I       01
 * 36       FFDCD       I       01
 * 37       FFDSR       I       01
 * 38       FFRI        I       01
 * 39       FFTXD       O   1   10
 * 40       FFDTR       O   0   10
 * 41       FFRTS       O   0   10
 * 42       RS232FOFF   O   0   00
 * 43       NC
 * 44       NC
 * 45       IRSL0       O   0
 * 46       IRRX0       I       01
 * 47       IRTX0       O   0   10
 * 48       NC
 * 49       nIOWE       O   0
 * 50       NC
 * 51       NC
 * 52       NC
 * 53       NC
 * 54       NC
 * 55       NC
 * 56       NC
 * 57       NC
 * 58       DKDIRQ      I
 * 59       NC
 * 60       NC
 * 61       NC
 * 62       NC
 * 63       NC
 * 64       COMLED      O   0
 * 65       COMLED      O   0
 * 66       COMLED      O   0
 * 67       COMLED      O   0
 * 68       COMLED      O   0
 * 69       COMLED      O   0
 * 70       COMLED      O   0
 * 71       COMLED      O   0
 * 72       NC
 * 73       NC
 * 74       NC
 * 75       NC
 * 76       NC
 * 77       NC
 * 78       CSIO        O   1
 * 79       NC
 * 80       CSETH       O   1
 *
 * NOTE: All NC's are defined to be outputs
 *
 */
/* Pin direction control */
/* NOTE GPIO 0, 61, 62 are set for inputs due to CPLD SPAREs */
#define CFG_GPDR0_VAL       0xfff3bf02
#define CFG_GPDR1_VAL       0xfbffbf83
#define CFG_GPDR2_VAL       0x0001ffff
/* Set and Clear registers */
#define CFG_GPSR0_VAL       0x00400800
#define CFG_GPSR1_VAL       0x00000480
#define CFG_GPSR2_VAL       0x00014000
#define CFG_GPCR0_VAL       0x00000000
#define CFG_GPCR1_VAL       0x00000000
#define CFG_GPCR2_VAL       0x00000000
/* Edge detect registers (these are set by the kernel) */
#define CFG_GRER0_VAL       0x00000000
#define CFG_GRER1_VAL       0x00000000
#define CFG_GRER2_VAL       0x00000000
#define CFG_GFER0_VAL       0x00000000
#define CFG_GFER1_VAL       0x00000000
#define CFG_GFER2_VAL       0x00000000
/* Alternate function registers */
#define CFG_GAFR0_L_VAL     0x00000000
#define CFG_GAFR0_U_VAL     0x00000010
#define CFG_GAFR1_L_VAL     0x900a9550
#define CFG_GAFR1_U_VAL     0x00000008
#define CFG_GAFR2_L_VAL     0x20000000
#define CFG_GAFR2_U_VAL     0x00000002

/*
 * Clocks, power control and interrupts
 */
#define CFG_PSSR_VAL        0x00000020
#define CFG_CCCR_VAL        0x00000141  /* 100 MHz memory, 200 MHz CPU  */
#define CFG_CKEN_VAL        0x00000060  /* FFUART and STUART enabled    */
#define CFG_ICMR_VAL        0x00000000  /* No interrupts enabled        */

/* FIXME
 *
 * RTC settings
 * Watchdog
 *
 */

/*
 * Memory settings
 *
 * FIXME Can ethernet be burst read and/or write?? This is set for lubbock
 *       Verify timings on all
 */
#define CFG_MSC0_VAL        0x000023FA  /* flash bank    (cs0)   */
/*#define CFG_MSC1_VAL        0x00003549  / * SuperIO bank  (cs2)   */
#define CFG_MSC1_VAL        0x0000354c  /* SuperIO bank  (cs2)   */
#define CFG_MSC2_VAL        0x00001224  /* Ethernet bank (cs4)   */
#ifdef REDBOOT_WAY
#define CFG_MDCNFG_VAL      0x00001aa1  /* FIXME can DTC be 01?     */
#define CFG_MDMRS_VAL       0x00000000
#define CFG_MDREFR_VAL      0x00018018
#else
#define CFG_MDCNFG_VAL      0x00001aa1  /* FIXME can DTC be 01?     */
#define CFG_MDMRS_VAL       0x00000000
#define CFG_MDREFR_VAL      0x00403018  /* Initial setting, individual bits set in memsetup.S */
#endif

/*
 * PCMCIA and CF Interfaces (NOT USED, these values from lubbock init)
 */
#define CFG_MECR_VAL          0x00000000
#define CFG_MCMEM0_VAL        0x00010504
#define CFG_MCMEM1_VAL        0x00010504
#define CFG_MCATT0_VAL        0x00010504
#define CFG_MCATT1_VAL        0x00010504
#define CFG_MCIO0_VAL         0x00004715
#define CFG_MCIO1_VAL         0x00004715

/* Board specific defines */

/* LED defines */
#define YELLOW    0x03
#define RED       0x02
#define GREEN     0x01
#define OFF       0x00
#define LED_IRDA0 0
#define LED_IRDA1 2
#define LED_IRDA2 4
#define LED_IRDA3 6
#define CRADLE_LED_SET_REG GPSR2
#define CRADLE_LED_CLR_REG GPCR2

/* SuperIO defines */
#define CRADLE_SIO_INDEX      0x2e
#define CRADLE_SIO_DATA       0x2f

/* IO defines */
#define CRADLE_CPLD_PHYS      0x08000000
#define CRADLE_SIO1_PHYS      0x08100000
#define CRADLE_SIO2_PHYS      0x08200000
#define CRADLE_SIO3_PHYS      0x08300000
#define CRADLE_ETH_PHYS       0x10000000

#ifndef __ASSEMBLY__

/* global prototypes */
void led_code(int code, int color);

#endif

#endif  /* __CONFIG_H */
