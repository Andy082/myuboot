/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002 (440 port)
 * Scott McNutt, Artesyn Communication Producs, smcnutt@artsyncp.com
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
#include <watchdog.h>
#include <command.h>
#include <cmd_boot.h>
#include <asm/processor.h>
#include <ppc4xx.h>
#include <ppc_asm.tmpl>
#include <commproc.h>
#include "vecnum.h"

/****************************************************************************/

unsigned decrementer_count;		/* count value for 1e6/HZ microseconds */

/****************************************************************************/

/*
 * CPM interrupt vector functions.
 */
struct	irq_action {
	interrupt_handler_t *handler;
	void *arg;
	int count;
};

static struct irq_action irq_vecs[32];

#if defined(CONFIG_440)
static struct irq_action irq_vecs1[32]; /* For UIC1 */

void uic1_interrupt( void * parms); /* UIC1 handler */
#endif

/****************************************************************************/

static __inline__ unsigned long get_msr(void)
{
	unsigned long msr;

	asm volatile("mfmsr %0" : "=r" (msr) :);
	return msr;
}

static __inline__ void set_msr(unsigned long msr)
{
	asm volatile("mtmsr %0" : : "r" (msr));
}

#if defined(CONFIG_440)

/* SPRN changed in 440 */
static __inline__ void set_evpr(unsigned long val)
{
	asm volatile("mtspr 0x03f,%0" : : "r" (val));
}

#else /* !defined(CONFIG_440) */

static __inline__ unsigned long get_dec(void)
{
	unsigned long val;

	asm volatile("mfdec %0" : "=r" (val) :);
	return val;
}


static __inline__ void set_dec(unsigned long val)
{
	asm volatile("mtdec %0" : : "r" (val));
}


static __inline__ void set_pit(unsigned long val)
{
	asm volatile("mtpit %0" : : "r" (val));
}


static __inline__ void set_tcr(unsigned long val)
{
	asm volatile("mttcr %0" : : "r" (val));
}


static __inline__ void set_evpr(unsigned long val)
{
	asm volatile("mtevpr %0" : : "r" (val));
}
#endif /* defined(CONFIG_440 */


void enable_interrupts (void)
{
	set_msr (get_msr() | MSR_EE);
}

/* returns flag if MSR_EE was set before */
int disable_interrupts (void)
{
	ulong msr = get_msr();
	set_msr (msr & ~MSR_EE);
	return ((msr & MSR_EE) != 0);
}

/****************************************************************************/

int interrupt_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	int vec;
	unsigned long val;

	/*
	 * Mark all irqs as free
	 */
	for (vec=0; vec<32; vec++) {
		irq_vecs[vec].handler = NULL;
		irq_vecs[vec].arg = NULL;
		irq_vecs[vec].count = 0;
#if defined(CONFIG_440)
		irq_vecs1[vec].handler = NULL;
		irq_vecs1[vec].arg = NULL;
		irq_vecs1[vec].count = 0;
#endif
	}

#ifdef CONFIG_4xx
	/*
	 * Init PIT
	 */
#if defined(CONFIG_440)
	val = mfspr( tcr );
	val &= (~0x04400000);		/* clear DIS & ARE */
	mtspr( tcr, val );
	mtspr( dec, 0 );		/* Prevent exception after TSR clear*/
	mtspr( decar, 0 );		/* clear reload */
	mtspr( tsr, 0x08000000 );	/* clear DEC status */
	val = gd->bd->bi_intfreq/100;	/* 10 msec */
	mtspr( decar, val );		/* Set auto-reload value */
	mtspr( dec, val );		/* Set inital val */
#else
	set_pit(gd->bd->bi_intfreq / 1000);
#endif
#endif  /* CONFIG_4xx */

#ifdef CONFIG_ADCIOP
	/*
	 * Init PIT
	 */
	set_pit(66000);
#endif

	/*
	 * Enable PIT
	 */
	val = mfspr(tcr);
	val |= 0x04400000;
	mtspr(tcr, val);

	/*
	 * Set EVPR to 0
	 */
	set_evpr(0x00000000);

#if defined(CONFIG_440)
	/* Install the UIC1 handlers */
	irq_install_handler(VECNUM_UIC1NC, uic1_interrupt, 0);
	irq_install_handler(VECNUM_UIC1C, uic1_interrupt, 0);
#endif
	/*
	 * Enable external interrupts (including PIT)
	 */
	set_msr (get_msr() | MSR_EE);

	return (0);
}

/****************************************************************************/

/*
 * Handle external interrupts
 */
void external_interrupt(struct pt_regs *regs)
{
	ulong uic_msr;
	ulong msr_shift;
	int vec;

	/*
	 * Read masked interrupt status register to determine interrupt source
	 */
	uic_msr = mfdcr(uicmsr);
	msr_shift = uic_msr;
	vec = 0;

	while (msr_shift != 0) {
		if (msr_shift & 0x80000000) {
			/*
			 * Increment irq counter (for debug purpose only)
			 */
			irq_vecs[vec].count++;

			if (irq_vecs[vec].handler != NULL) {
				/* call isr */
				(*irq_vecs[vec].handler)(irq_vecs[vec].arg);
			} else {
				mtdcr(uicer, mfdcr(uicer) & ~(0x80000000 >> vec));
				printf ("Masking bogus interrupt vector 0x%x\n", vec);
			}

			/*
			 * After servicing the interrupt, we have to remove the status indicator.
			 */
			mtdcr(uicsr, (0x80000000 >> vec));
		}

		/*
		 * Shift msr to next position and increment vector
		 */
		msr_shift <<= 1;
		vec++;
	}
}

#if defined(CONFIG_440)
/* Handler for UIC1 interrupt */
void uic1_interrupt( void * parms)
{
	ulong uic1_msr;
	ulong msr_shift;
	int vec;

	/*
	 * Read masked interrupt status register to determine interrupt source
	 */
	uic1_msr = mfdcr(uic1msr);
	msr_shift = uic1_msr;
	vec = 0;

	while (msr_shift != 0) {
		if (msr_shift & 0x80000000) {
			/*
			 * Increment irq counter (for debug purpose only)
			 */
			irq_vecs1[vec].count++;

			if (irq_vecs1[vec].handler != NULL) {
				/* call isr */
				(*irq_vecs1[vec].handler)(irq_vecs1[vec].arg);
			} else {
				mtdcr(uic1er, mfdcr(uic1er) & ~(0x80000000 >> vec));
				printf ("Masking bogus interrupt vector (uic1) 0x%x\n", vec);
			}

			/*
			 * After servicing the interrupt, we have to remove the status indicator.
			 */
			mtdcr(uic1sr, (0x80000000 >> vec));
		}

		/*
		 * Shift msr to next position and increment vector
		 */
		msr_shift <<= 1;
		vec++;
	}
}
#endif /* defined(CONFIG_440) */

/****************************************************************************/

/*
 * Install and free a interrupt handler.
 */

void
irq_install_handler(int vec, interrupt_handler_t *handler, void *arg)
{
	struct irq_action *irqa = irq_vecs;
	int   i = vec;

#if defined(CONFIG_440)
	if (vec > 31) {
		i = vec - 32;
		irqa = irq_vecs1;
	}
#endif

	if (irqa[i].handler != NULL) {
		printf ("Interrupt vector %d: handler 0x%x replacing 0x%x\n",
			vec, (uint)handler, (uint)irqa[i].handler);
	}
	irqa[i].handler = handler;
	irqa[i].arg     = arg;

#if defined(CONFIG_440)
	if( vec > 31 )
		mtdcr(uic1er, mfdcr(uic1er) | (0x80000000 >> i));
	else
#endif
		mtdcr(uicer, mfdcr(uicer) | (0x80000000 >> i));
#if 0
	printf ("Install interrupt for vector %d ==> %p\n", vec, handler);
#endif
}

void
irq_free_handler(int vec)
{
	struct irq_action *irqa = irq_vecs;
	int   i = vec;

#if defined(CONFIG_440)
	if (vec > 31) {
		irqa = irq_vecs1;
		i = vec - 32;
	}
#endif

#if 0
	printf ("Free interrupt for vector %d ==> %p\n",
		vec, irq_vecs[vec].handler);
#endif

#if defined(CONFIG_440)
	if (vec > 31)
		mtdcr(uic1er, mfdcr(uic1er) & ~(0x80000000 >> i));
	else
#endif
		mtdcr(uicer, mfdcr(uicer) & ~(0x80000000 >> i));

	irqa[i].handler = NULL;
	irqa[i].arg     = NULL;
}

/****************************************************************************/


volatile ulong timestamp = 0;

/*
 * timer_interrupt - gets called when the decrementer overflows,
 * with interrupts disabled.
 * Trivial implementation - no need to be really accurate.
 */
void timer_interrupt(struct pt_regs *regs)
{
#if 0
	printf ("*** Timer Interrupt *** ");
#endif
	timestamp++;

#if defined(CONFIG_WATCHDOG)
	if ((timestamp % 1000) == 0)
		reset_4xx_watchdog();
#endif /* CONFIG_WATCHDOG */
}

/****************************************************************************/

void reset_timer (void)
{
	timestamp = 0;
}

ulong get_timer (ulong base)
{
	return (timestamp - base);
}

void set_timer (ulong t)
{
	timestamp = t;
}

/****************************************************************************/


#if (CONFIG_COMMANDS & CFG_CMD_IRQ)

/*******************************************************************************
 *
 * irqinfo - print information about PCI devices
 *
 */
int
do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int vec;

	printf ("\nInterrupt-Information:\n");
#if defined(CONFIG_440)
	printf ("\nUIC 0\n");
#endif
	printf ("Nr  Routine   Arg       Count\n");

	for (vec=0; vec<32; vec++) {
		if (irq_vecs[vec].handler != NULL) {
			printf ("%02d  %08lx  %08lx  %d\n",
				vec,
				(ulong)irq_vecs[vec].handler,
				(ulong)irq_vecs[vec].arg,
				irq_vecs[vec].count);
		}
	}

#if defined(CONFIG_440)
	printf ("\nUIC 1\n");
	printf ("Nr  Routine   Arg       Count\n");

	for (vec=0; vec<32; vec++)
	{
		if (irq_vecs1[vec].handler != NULL)
			printf ("%02d  %08lx  %08lx  %d\n",
				vec+31, (ulong)irq_vecs1[vec].handler,
				(ulong)irq_vecs1[vec].arg, irq_vecs1[vec].count);
	}
	printf("\n");
#endif
	return 0;
}


#endif  /* CONFIG_COMMANDS & CFG_CMD_IRQ */
