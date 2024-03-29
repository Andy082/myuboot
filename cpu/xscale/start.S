/*
 *  armboot - Startup Code for XScale
 *
 *  Copyright (C) 1998	Dan Malek <dmalek@jlc.net>
 *  Copyright (C) 1999	Magnus Damm <kieraypc01.p.y.kie.era.ericsson.se>
 *  Copyright (C) 2000	Wolfgang Denk <wd@denx.de>
 *  Copyright (c) 2001	Alex Z�pke <azu@sysgo.de>
 *  Copyright (c) 2002	Kyle Harris <kharris@nexus-tech.net>
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



#include <config.h>
#include <version.h>

.globl _start
_start:	b       reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef


/*
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * - relocate armboot to ram
 * - setup stack
 * - jump to second stage
 */

/*
 * CFG_MEM_END is in the board dependent config-file (configs/config_BOARD.h)
 */
_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * Note: _armboot_end_data and _armboot_end are defined
 * by the (board-dependent) linker script.
 * _armboot_end_data is the first usable FLASH address after armboot
 */
.globl _armboot_end_data
_armboot_end_data:
	.word armboot_end_data
.globl _armboot_end
_armboot_end:
	.word armboot_end

/*
 * _armboot_real_end is the first usable RAM address behind armboot
 * and the various stacks
 */
.globl _armboot_real_end
_armboot_real_end:
	.word 0x0badc0de

/*
 * We relocate uboot to this address (end of RAM - 128 KiB)
 */
.globl _uboot_reloc
_uboot_reloc:
	.word CFG_DRAM_BASE + CFG_DRAM_SIZE - CFG_MONITOR_LEN

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif


/****************************************************************************/
/*                                                                          */
/* the actual reset code                                                    */
/*                                                                          */
/****************************************************************************/

reset:
	mrs	r0,cpsr			/* set the cpu to SVC32 mode        */
	bic	r0,r0,#0x1f		/* (superviser mode, M=10011)       */
	orr	r0,r0,#0x13
	msr	cpsr,r0

	bl	cpu_init_crit		/* we do sys-critical inits         */

relocate:				/* relocate ppcboot to RAM          */
	adr	r0, _start		/* r0 <- current position of code */
	ldr	r2, _armboot_start
	ldr	r3, _armboot_end
	sub	r2, r3, r2		/* r2 <- size of armboot */
/*	ldr	r1, _uboot_reloc	/ * r1 <- destination address        */
	ldr	r1, _TEXT_BASE
	add	r2, r0, r2		/* r2 <- source end address */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */ 
	ble	copy_loop

	/* Set up the stack                                                 */
	ldr	r0, _uboot_reloc	/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CFG_MALLOC_LEN	/* malloc area                      */
					/* FIXME: bdinfo should be here     */
	sub	sp, r0, #12		/* leave 3 words for abort-stack */

	ldr	pc, _start_armboot

_start_armboot:	.word start_armboot


/****************************************************************************/
/*                                                                          */
/* CPU_init_critical registers                                              */
/*                                                                          */
/* - setup important registers                                              */
/* - setup memory timing                                                    */
/*                                                                          */
/****************************************************************************/

	/* Interrupt-Controller base address                                */
IC_BASE:	   .word	   0x40d00000
#define ICMR	0x04

/* Reset-Controller */
RST_BASE:	.word   0x40f00030
#define RCSR	0x00


	/* Clock Manager Registers                                          */
CC_BASE:		.word	0x41300000
#define CCCR    0x00
cpuspeed:	.word   CFG_CPUSPEED

	/* RS: ???                                                          */ 
	.macro CPWAIT
    mrc  p15,0,r0,c2,c0,0
	mov  r0,r0
	sub  pc,pc,#4
	.endm


cpu_init_crit:

	/* mask all IRQs                                                    */
	ldr	r0, IC_BASE
	mov	r1, #0x00
	str	r1, [r0, #ICMR]

	/* set clock speed */
	ldr	r0, CC_BASE
	ldr	r1, cpuspeed
	str	r1, [r0, #CCCR]

	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a memsetup.S in your board directory.
	 */
	mov	ip,	lr
	bl	memsetup
	mov	lr,	ip

	/* Memory interfaces are working. Disable MMU and enable I-cache.   */

	ldr	r0, =0x2001		/* enable access to all coproc.     */
	mcr	p15, 0, r0, c15, c1, 0
    CPWAIT

	mcr	p15, 0, r0, c7, c10, 4	/* drain the write & fill buffers   */
    CPWAIT

	mcr	p15, 0, r0, c7, c7, 0	/* flush Icache, Dcache and BTB     */
    CPWAIT

	mcr	p15, 0, r0, c8, c7, 0	/* flush instuction and data TLBs   */
    CPWAIT

	/* Enable the Icache                                                */
/*
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #0x1800
	mcr	p15, 0, r0, c1, c0, 0
    CPWAIT
*/
	mov	pc, lr


/****************************************************************************/
/*                                                                          */
/* Interrupt handling                                                       */
/*                                                                          */
/****************************************************************************/

/* IRQ stack frame                                                          */

#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13

	/* use bad_save_user_regs for abort/prefetch/undef/swi ...          */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			/* Calling r0-r12           */
	add     r8, sp, #S_PC

	ldr	r2, _armboot_end
	add	r2, r2, #CONFIG_STACKSIZE
	sub	r2, r2, #8
	ldmia	r2, {r2 - r4}                   /* get pc, cpsr, old_r0     */
	add	r0, sp, #S_FRAME_SIZE		/* restore sp_SVC           */

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r4}                   /* save sp_SVC, lr_SVC, pc, cpsr, old_r */
	mov	r0, sp
	.endm


	/* use irq_save_user_regs / irq_restore_user_regs for                */
	/* IRQ/FIQ handling                                                  */

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			/* Calling r0-r12            */
	add     r8, sp, #S_PC
	stmdb   r8, {sp, lr}^                   /* Calling SP, LR            */
	str     lr, [r8, #0]                    /* Save calling PC           */
	mrs     r6, spsr
	str     r6, [r8, #4]                    /* Save CPSR                 */
	str     r0, [r8, #8]                    /* Save OLD_R0               */
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_end		@ setup our mode stack
	add	r13, r13, #CONFIG_STACKSIZE	@ resides at top of normal stack
	sub	r13, r13, #8

	str	lr, [r13]			@ save caller lr / spsr
	mrs	lr, spsr
	str     lr, [r13, #4]

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	msr	spsr_c, r13
	mov	lr, pc
	movs	pc, lr
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm


/****************************************************************************/
/*                                                                          */
/* exception handlers                                                       */
/*                                                                          */
/****************************************************************************/

	.align  5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl 	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	bl 	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl 	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl 	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl 	do_not_used

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl 	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	irq_save_user_regs		/* someone ought to write a more    */
	bl 	do_fiq			/* effiction fiq_save_user_regs     */ 
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl 	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl 	do_fiq

#endif

/*
 * FIXME How do we reset??? Watchdog timeout??
 */
	.align	5
.globl reset_cpu
reset_cpu:
	/*
	ldr	r0, RST_BASE
	mov	r1, #0x0			@ set bit 3-0 ...
	str	r1, [r0, #RCSR]			@ ... to clear in RCSR
	mov	r1, #0x1
	str	r1, [r0, #RCSR]			@ and perform reset
	*/
	b	reset_cpu			@ silly, but repeat endlessly

