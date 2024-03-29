/*------------------------------------------------------------------------------+
 *
 *      This source code has been made available to you by IBM on an AS-IS
 *      basis.  Anyone receiving this source is licensed under IBM
 *      copyrights to use it in any way he or she deems fit, including
 *      copying it, modifying it, compiling it, and redistributing it either
 *      with or without modifications.  No license under IBM patents or
 *      patent applications is to be implied by the copyright license.
 *
 *      Any user of this software should understand that IBM cannot provide
 *      technical support for this software and will not be responsible for
 *      any consequences resulting from the use of this software.
 *
 *      Any person who transfers this source code or any derivative work
 *      must include the IBM copyright notice, this paragraph, and the
 *      preceding two paragraphs in the transferred software.
 *
 *      COPYRIGHT   I B M   CORPORATION 1995
 *      LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *-------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Function:     ext_bus_cntlr_init
 * Description:  Initializes the External Bus Controller for the external
 *		peripherals. IMPORTANT: For pass1 this code must run from
 *		cache since you can not reliably change a peripheral banks
 *		timing register (pbxap) while running code from that bank.
 *		For ex., since we are running from ROM on bank 0, we can NOT
 *		execute the code that modifies bank 0 timings from ROM, so
 *		we run it from cache.
 *	Bank 0 - Flash or Multi Purpose Socket
 *	Bank 1 - Multi Purpose Socket or Flash (set in C-Code)
 *	Bank 2 - UART 1 (set in C-Code)
 *	Bank 3 - UART 2 (set in C-Code)
 *	Bank 4 - not used
 *	Bank 5 - not used
 *	Bank 6 - not used
 *	Bank 7 - PLD Register
 *-----------------------------------------------------------------------------*/
#include <ppc4xx.h>

#define _LINUX_CONFIG_H 1	/* avoid reading Linux autoconf.h file	*/

#include <configs/MIP405.h>
#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>
#include "mip405.h"


 	.globl	ext_bus_cntlr_init
ext_bus_cntlr_init:
  mflr    r4                      /* save link register */
  bl      ..getAddr
..getAddr:
  mflr    r3                      /* get address of ..getAddr */
  mtlr    r4                      /* restore link register */
  addi    r4,0,14                 /* set ctr to 14; used to prefetch */
  mtctr   r4                      /* 14 cache lines to fit this function */
                                   /* in cache (gives us 8x14=112 instrctns) */
..ebcloop:
  icbt    r0,r3                   /* prefetch cache line for addr in r3 */
  addi    r3,r3,32								/* move to next cache line */
  bdnz    ..ebcloop               /* continue for 14 cache lines */

   /*-------------------------------------------------------------------
    * Delay to ensure all accesses to ROM are complete before changing
    * bank 0 timings.
    *------------------------------------------------------------------- */
	addis	r3,0,0x0
  ori     r3,r3,0xA000
  mtctr   r3
..spinlp:
  bdnz    ..spinlp                /* spin loop */

	/*-----------------------------------------------------------------------
	 * decide boot up mode
	 *----------------------------------------------------------------------- */
	addi		r4,0,pb0cr
	mtdcr		ebccfga,r4
	mfdcr		r4,ebccfgd

	andi.		r0, r4, 0x2000			/* mask out irrelevant bits */
	beq			0f						/* jump if 8 bit bus width */

	/* setup 16 bit things (Flash Boot)
   *-----------------------------------------------------------------------
   * Memory Bank 0 (16 Bit Flash) initialization
   *---------------------------------------------------------------------- */

	addi    r4,0,pb0ap
	mtdcr   ebccfga,r4
//	addis   r4,0,0xFF8F
//	ori     r4,r4,0xFE80
//	addis   r4,0,0x9B01
//	ori     r4,r4,0x5480
	addis   r4,0,(FLASH_AP_B)@h
	ori     r4,r4,(FLASH_AP_B)@l
	mtdcr   ebccfgd,r4

	addi    r4,0,pb0cr
	mtdcr   ebccfga,r4
	/* BS=0x010(4MB),BU=0x3(R/W), */
//	addis   r4,0,((FLASH_BASE0_PRELIM & 0xFFF00000) | 0x00050000)@h
//	ori     r4,r4,0xA000          /* BW=0x01(16 bits) */
	addis   r4,0,(FLASH_CR_B)@h
	ori     r4,r4,(FLASH_CR_B)@l
	mtdcr   ebccfgd,r4
	b				1f

0:

  	/* 8Bit boot mode: */
	/*-----------------------------------------------------------------------
   	* Memory Bank 0 Multi Purpose Socket initialization
   	*----------------------------------------------------------------------- */
	/* 0x7F8FFE80 slowest boot */
	addi    r4,0,pb0ap
	mtdcr   ebccfga,r4
#if 0
	addis   r4,0,0x9B01
	ori     r4,r4,0x5480
#else
	addis   r4,0,(MPS_AP_B)@h
	ori     r4,r4,(MPS_AP_B)@l
#endif
	mtdcr   ebccfgd,r4

	addi    r4,0,pb0cr
 	mtdcr   ebccfga,r4
	/* BS=0x010(4MB),BU=0x3(R/W), */
//	addis   r4,0,((FLASH_BASE0_PRELIM & 0xFFF00000) | 0x00050000)@h
//	ori     r4,r4,0x8000          /* BW=0x0( 8 bits) */

	addis   r4,0,(MPS_CR_B)@h
	ori     r4,r4,(MPS_CR_B)@l

	mtdcr   ebccfgd,r4


1:
  /*-----------------------------------------------------------------------
   * Memory Bank 2-3-4-5-6 (not used) initialization
   *-----------------------------------------------------------------------*/
  addi    r4,0,pb1cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

  addi    r4,0,pb2cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

  addi    r4,0,pb3cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

  addi    r4,0,pb4cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

  addi    r4,0,pb5cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

	addi    r4,0,pb6cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4

	addi    r4,0,pb7cr
  mtdcr   ebccfga,r4
  addis   r4,0,0x0000
  ori     r4,r4,0x0000
  mtdcr   ebccfgd,r4
	nop				/* pass2 DCR errata #8 */
  blr

/*-----------------------------------------------------------------------------
 * Function:     sdram_init
 * Description:  Configures the internal SRAM memory. and setup the
 *               Stackpointer in it.
 *----------------------------------------------------------------------------- */
        .globl  sdram_init

sdram_init:


  blr

