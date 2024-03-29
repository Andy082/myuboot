/*
 * (C) Copyright 2000
 * Rob Taylor. Flying Pig Systems. robt@flyingpig.com.
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

#include <common.h>
#include <asm/processor.h>
#include <mpc824x.h>

#ifndef CFG_BANK0_ROW
#define CFG_BANK0_ROW 0
#endif
#ifndef CFG_BANK1_ROW
#define CFG_BANK1_ROW 0
#endif
#ifndef CFG_BANK2_ROW
#define CFG_BANK2_ROW 0
#endif
#ifndef CFG_BANK3_ROW
#define CFG_BANK3_ROW 0
#endif
#ifndef CFG_BANK4_ROW
#define CFG_BANK4_ROW 0
#endif
#ifndef CFG_BANK5_ROW
#define CFG_BANK5_ROW 0
#endif
#ifndef CFG_BANK6_ROW
#define CFG_BANK6_ROW 0
#endif
#ifndef CFG_BANK7_ROW
#define CFG_BANK7_ROW 0
#endif
#ifndef CFG_DBUS_SIZE2
#define CFG_DBUS_SIZE2 0
#endif

/*
 * Breath some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 */
void
cpu_init_f (void)
{
/* MOUSSE board is initialized in asm */
#if !defined(CONFIG_MOUSSE) && !defined(CONFIG_BMW)
    register unsigned long val;
    CONFIG_WRITE_HALFWORD(PCICR, 0x06); /* Bus Master, respond to PCI memory space acesses*/
/*    CONFIG_WRITE_HALFWORD(PCISR, 0xffff); */ /*reset PCISR*/

#if defined(CONFIG_MUSENKI) || defined(CONFIG_PN62)
/* Why is this here, you ask?  Try, just try setting 0x8000
 * in PCIACR with CONFIG_WRITE_HALFWORD()
 * this one was a stumper, and we are annoyed
 */

#define M_CONFIG_WRITE_HALFWORD( addr, data ) \
    __asm__ __volatile__(     \
      "                       \
      stw  %2,0(%0)\n         \
      sync\n                  \
      sth  %3,2(%1)\n         \
      sync\n                  \
      "                       \
      : /* no output */       \
      : "r" (CONFIG_ADDR), "r" (CONFIG_DATA),                 \
        "r" (PCISWAP(addr & ~3)), "r" (PCISWAP(data << 16))   \
      );

    M_CONFIG_WRITE_HALFWORD(PCIACR, 0x8000);
#endif

    CONFIG_WRITE_BYTE(PCLSR, 0x8);	/* set PCI cache line size */

    /*
     * Note that although this bit is cleared after a hard reset, it
     * must be explicitly set and then cleared by software during
     * initialization in order to guarantee correct operation of the
     * DLL and the SDRAM_CLK[0:3] signals (if they are used).
     */
    CONFIG_READ_BYTE (AMBOR, val);
    CONFIG_WRITE_BYTE(AMBOR, val & 0xDF);
    CONFIG_WRITE_BYTE(AMBOR, val | 0x20);
    CONFIG_WRITE_BYTE(AMBOR, val & 0xDF);

    CONFIG_READ_WORD(PICR1, val);
#if defined(CONFIG_MPC8240)
    CONFIG_WRITE_WORD( PICR1,
       (val & (PICR1_ADDRESS_MAP | PICR1_RCS0)) |
	       PIRC1_MSK | PICR1_PROC_TYPE_603E |
	       PICR1_FLASH_WR_EN | PICR1_MCP_EN |
	       PICR1_CF_DPARK | PICR1_EN_PCS |
	       PICR1_CF_APARK );
#elif defined(CONFIG_MPC8245)
    CONFIG_WRITE_WORD( PICR1,
       (val & (PICR1_RCS0)) |
	       PICR1_PROC_TYPE_603E |
	       PICR1_FLASH_WR_EN | PICR1_MCP_EN |
	       PICR1_CF_DPARK | PICR1_NO_BUSW_CK |
	       PICR1_DEC| PICR1_CF_APARK | 0x10);	/* 8245 UM says bit 4 must be set */ 
#else
#error Specific type of MPC824x must be defined (i.e. CONFIG_MPC8240)
#endif

    CONFIG_READ_WORD(PICR2, val);
    val= val & ~ (PICR2_CF_SNOOP_WS_MASK | PICR2_CF_APHASE_WS_MASK); /*mask off waitstate bits*/
#ifndef CONFIG_PN62
    val |= PICR2_CF_SNOOP_WS_1WS | PICR2_CF_APHASE_WS_1WS; /*1 wait state*/
#endif
    CONFIG_WRITE_WORD(PICR2, val);

    CONFIG_WRITE_WORD(EUMBBAR, CFG_EUMB_ADDR);
#ifndef CFG_RAMBOOT
    CONFIG_WRITE_WORD(MCCR1, (CFG_ROMNAL << MCCR1_ROMNAL_SHIFT) |
				 (CFG_BANK0_ROW) |
				 (CFG_BANK1_ROW << MCCR1_BANK1ROW_SHIFT) |
				 (CFG_BANK2_ROW << MCCR1_BANK2ROW_SHIFT) |
				 (CFG_BANK3_ROW << MCCR1_BANK3ROW_SHIFT) |
				 (CFG_BANK4_ROW << MCCR1_BANK4ROW_SHIFT) |
				 (CFG_BANK5_ROW << MCCR1_BANK5ROW_SHIFT) |
				 (CFG_BANK6_ROW << MCCR1_BANK6ROW_SHIFT) |
				 (CFG_BANK7_ROW << MCCR1_BANK7ROW_SHIFT) |
			     (CFG_ROMFAL << MCCR1_ROMFAL_SHIFT));
#endif

#if defined(CFG_ASRISE) && defined(CFG_ASFALL)
    CONFIG_WRITE_WORD(MCCR2, CFG_REFINT << MCCR2_REFINT_SHIFT |
    			     CFG_ASRISE << MCCR2_ASRISE_SHIFT |
    			     CFG_ASFALL << MCCR2_ASFALL_SHIFT);
#else
    CONFIG_WRITE_WORD(MCCR2, CFG_REFINT << MCCR2_REFINT_SHIFT);
#endif

#if defined(CONFIG_MPC8240)
    CONFIG_WRITE_WORD(MCCR3,
	(((CFG_BSTOPRE & 0x003c) >> 2) << MCCR3_BSTOPRE2TO5_SHIFT) |
	(CFG_REFREC << MCCR3_REFREC_SHIFT) |
	(CFG_RDLAT  << MCCR3_RDLAT_SHIFT));
#elif defined(CONFIG_MPC8245)
    CONFIG_WRITE_WORD(MCCR3,
	(((CFG_BSTOPRE & 0x003c) >> 2) << MCCR3_BSTOPRE2TO5_SHIFT) |
	(CFG_REFREC << MCCR3_REFREC_SHIFT));
#else
#error Specific type of MPC824x must be defined (i.e. CONFIG_MPC8240)
#endif

/* this is gross.  We think these should all be the same, and various boards
 *  should define CFG_ACTORW to 0 if they don't want to set it, or even, if
 *  its not set, we define it to zero in this file
 */
#if defined(CONFIG_CU824) || defined(CONFIG_PN62)
	CONFIG_WRITE_WORD(MCCR4,
	(CFG_PRETOACT << MCCR4_PRETOACT_SHIFT) |
	(CFG_ACTTOPRE << MCCR4_ACTTOPRE_SHIFT) |
	MCCR4_BIT21 |
	(CFG_REGISTERD_TYPE_BUFFER ? MCCR4_REGISTERED: 0) |
	((CFG_BSTOPRE & 0x0003) <<MCCR4_BSTOPRE0TO1_SHIFT ) |
	(((CFG_SDMODE_CAS_LAT <<4) | (CFG_SDMODE_WRAP <<3) |
		  CFG_SDMODE_BURSTLEN) << MCCR4_SDMODE_SHIFT) |
	(CFG_ACTORW << MCCR4_ACTTORW_SHIFT) |
	(((CFG_BSTOPRE & 0x03c0) >> 6) << MCCR4_BSTOPRE6TO9_SHIFT));
#elif defined(CONFIG_MPC8240)
	CONFIG_WRITE_WORD(MCCR4,
	(CFG_PRETOACT << MCCR4_PRETOACT_SHIFT) |
	(CFG_ACTTOPRE << MCCR4_ACTTOPRE_SHIFT) |
	MCCR4_BIT21 |
	(CFG_REGISTERD_TYPE_BUFFER ? MCCR4_REGISTERED: 0) |
	((CFG_BSTOPRE & 0x0003) <<MCCR4_BSTOPRE0TO1_SHIFT ) |
	(((CFG_SDMODE_CAS_LAT <<4) | (CFG_SDMODE_WRAP <<3) |
		  (CFG_SDMODE_BURSTLEN)) <<MCCR4_SDMODE_SHIFT) |
	(((CFG_BSTOPRE & 0x03c0) >> 6) <<MCCR4_BSTOPRE6TO9_SHIFT ));
#elif defined(CONFIG_MPC8245)
    CONFIG_READ_WORD(MCCR1, val);
    val &= MCCR1_DBUS_SIZE0;    /* test for 64-bit mem bus */

    CONFIG_WRITE_WORD(MCCR4,
	(CFG_PRETOACT << MCCR4_PRETOACT_SHIFT) |
	(CFG_ACTTOPRE << MCCR4_ACTTOPRE_SHIFT) |
        (CFG_EXTROM ? MCCR4_EXTROM : 0) |
        (CFG_REGDIMM ? MCCR4_REGDIMM : 0) |
	(CFG_REGISTERD_TYPE_BUFFER ? MCCR4_REGISTERED: 0) |
	((CFG_BSTOPRE & 0x0003) <<MCCR4_BSTOPRE0TO1_SHIFT ) |
	(CFG_DBUS_SIZE2 << MCCR4_DBUS_SIZE2_SHIFT) |
	(((CFG_SDMODE_CAS_LAT <<4) | (CFG_SDMODE_WRAP <<3) |
              (val ? 2 : 3)) << MCCR4_SDMODE_SHIFT)  |
	(CFG_ACTORW << MCCR4_ACTTORW_SHIFT) |
	(((CFG_BSTOPRE & 0x03c0) >> 6) <<MCCR4_BSTOPRE6TO9_SHIFT ));
#else
#error Specific type of MPC824x must be defined (i.e. CONFIG_MPC8240)
#endif

    CONFIG_WRITE_WORD(MSAR1,
	( (CFG_BANK0_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) |
	(((CFG_BANK1_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 8) |
	(((CFG_BANK2_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 16) |
	(((CFG_BANK3_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(EMSAR1,
	( (CFG_BANK0_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) |
	(((CFG_BANK1_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 8) |
	(((CFG_BANK2_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 16) |
	(((CFG_BANK3_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(MSAR2,
	( (CFG_BANK4_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) |
	(((CFG_BANK5_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 8) |
	(((CFG_BANK6_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 16) |
	(((CFG_BANK7_START & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(EMSAR2,
	( (CFG_BANK4_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) |
	(((CFG_BANK5_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 8) |
	(((CFG_BANK6_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 16) |
	(((CFG_BANK7_START & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(MEAR1,
	( (CFG_BANK0_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) |
	(((CFG_BANK1_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 8) |
	(((CFG_BANK2_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 16) |
	(((CFG_BANK3_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(EMEAR1,
	( (CFG_BANK0_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) |
	(((CFG_BANK1_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 8) |
	(((CFG_BANK2_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 16) |
	(((CFG_BANK3_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(MEAR2,
	( (CFG_BANK4_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) |
	(((CFG_BANK5_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 8) |
	(((CFG_BANK6_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 16) |
	(((CFG_BANK7_END & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT) << 24));
    CONFIG_WRITE_WORD(EMEAR2,
	( (CFG_BANK4_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) |
	(((CFG_BANK5_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 8) |
	(((CFG_BANK6_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 16) |
	(((CFG_BANK7_END & MICR_EADDR_MASK) >> MICR_EADDR_SHIFT) << 24));

    CONFIG_WRITE_BYTE(ODCR, CFG_ODCR);
#ifdef CFG_DLL_MAX_DELAY
	CONFIG_WRITE_BYTE(MIOCR1, CFG_DLL_MAX_DELAY);	/* needed to make DLL lock */
#endif
#if defined(CFG_DLL_EXTEND) && defined(CFG_PCI_HOLD_DEL)
	CONFIG_WRITE_BYTE(PMCR2, CFG_DLL_EXTEND | CFG_PCI_HOLD_DEL); 
#endif
#if defined(MIOCR2) && defined(CFG_SDRAM_DSCD)
	CONFIG_WRITE_BYTE(MIOCR2, CFG_SDRAM_DSCD);	/* change memory input */
#endif /* setup & hold time */

    CONFIG_WRITE_BYTE(MBER,
	 CFG_BANK0_ENABLE |
	(CFG_BANK1_ENABLE << 1) |
	(CFG_BANK2_ENABLE << 2) |
	(CFG_BANK3_ENABLE << 3) |
	(CFG_BANK4_ENABLE << 4) |
	(CFG_BANK5_ENABLE << 5) |
	(CFG_BANK6_ENABLE << 6) |
	(CFG_BANK7_ENABLE << 7));

#ifdef CFG_PGMAX
    CONFIG_WRITE_BYTE(MPMR, CFG_PGMAX);
#endif

    /* ! Wait 200us before initialize other registers */
    /*FIXME: write a decent udelay wait */
    __asm__ __volatile__(
      " mtctr	%0 \n \
       0: bdnz	 0b\n"
      :
      : "r" (0x10000));

   CONFIG_READ_WORD(MCCR1, val);
   CONFIG_WRITE_WORD(MCCR1, val | MCCR1_MEMGO); /* set memory access going */
   __asm__ __volatile__("eieio");

#endif /* !CONFIG_MOUSSE && !CONFIG_BMW */
}


#ifdef CONFIG_MOUSSE
#ifdef INCLUDE_MPC107_REPORT
struct MPC107_s{
    unsigned int iobase;
    char desc[120];
} MPC107Regs[] ={
    {BMC_BASE+0x0,  "MPC107 Vendor/Device ID"},
    {BMC_BASE+0x4,  "MPC107 PCI Command/Status Register"},
    {BMC_BASE+0x8,  "MPC107 Revision"},
    {BMC_BASE+0xC,  "MPC107 Cache Line Size"},
    {BMC_BASE+0x10, "MPC107 LMBAR"},
    {BMC_BASE+0x14, "MPC824x PCSR"},
    {BMC_BASE+0xA8, "MPC824x PICR1"},
    {BMC_BASE+0xAC, "MPC824x PICR2"},
    {BMC_BASE+0x46, "MPC824x PACR"},
    {BMC_BASE+0x310, "MPC824x ITWR"},
    {BMC_BASE+0x300, "MPC824x OMBAR"},
    {BMC_BASE+0x308, "MPC824x OTWR"},
    {BMC_BASE+0x14, "MPC107 Peripheral Control and Status Register"},
    {BMC_BASE+0x78, "MPC107 EUMBAR"},
    {BMC_BASE+0xC0, "MPC107 Processor Bus Error Status"},
    {BMC_BASE+0xC4, "MPC107 PCI Bus Error Status"},
    {BMC_BASE+0xC8, "MPC107 Processor/PCI Error Address"},
    {BMC_BASE+0xE0, "MPC107 AMBOR Register"},
    {BMC_BASE+0xF0, "MPC107 MCCR1 Register"},
    {BMC_BASE+0xF4, "MPC107 MCCR2 Register"},
    {BMC_BASE+0xF8, "MPC107 MCCR3 Register"},
    {BMC_BASE+0xFC, "MPC107 MCCR4 Register"}
};
#define N_MPC107_Regs	(sizeof(MPC107Regs)/sizeof(MPC107Regs[0]))
#endif /* INCLUDE_MPC107_REPORT */
#endif /* CONFIG_MOUSSE */

/*
 * initialize higher level parts of CPU like time base and timers
 */
int cpu_init_r (void)
{
#ifdef CONFIG_MOUSSE
#ifdef INCLUDE_MPC107_REPORT
	unsigned int tmp = 0, i;
#endif
	/*
	 * Initialize the EUMBBAR (Embedded Util Mem Block Base Addr Reg).
	 * This is necessary before the EPIC, DMA ctlr, I2C ctlr, etc. can
	 * be accessed.
	 */

#ifdef CONFIG_MPC8240			/* only on MPC8240 */
	mpc824x_mpc107_setreg (EUMBBAR, EUMBBAR_VAL);
	/* MOT/SPS: Issue #10002, PCI (FD Alias enable) */
	mpc824x_mpc107_setreg (AMBOR, 0x000000C0);
#endif


#ifdef INCLUDE_MPC107_REPORT
	/* Check MPC824x PCI Device and Vendor ID */
	while ((tmp = mpc824x_mpc107_getreg (BMC_BASE)) != 0x31057) {
		printf ("	MPC107: offset=0x%x, val = 0x%x\n",
			BMC_BASE,
			tmp);
	}

	for (i = 0; i < N_MPC107_Regs; i++) {
		printf ("	0x%x/%s = 0x%x\n",
			MPC107Regs[i].iobase,
			MPC107Regs[i].desc,
			mpc824x_mpc107_getreg (MPC107Regs[i].iobase));
	}

	printf ("IBAT0L = 0x%08X\n", mfspr (IBAT0L));
	printf ("IBAT0U = 0x%08X\n", mfspr (IBAT0U));
	printf ("IBAT1L = 0x%08X\n", mfspr (IBAT1L));
	printf ("IBAT1U = 0x%08X\n", mfspr (IBAT1U));
	printf ("IBAT2L = 0x%08X\n", mfspr (IBAT2L));
	printf ("IBAT2U = 0x%08X\n", mfspr (IBAT2U));
	printf ("IBAT3L = 0x%08X\n", mfspr (IBAT3L));
	printf ("IBAT3U = 0x%08X\n", mfspr (IBAT3U));
	printf ("DBAT0L = 0x%08X\n", mfspr (DBAT0L));
	printf ("DBAT0U = 0x%08X\n", mfspr (DBAT0U));
	printf ("DBAT1L = 0x%08X\n", mfspr (DBAT1L));
	printf ("DBAT1U = 0x%08X\n", mfspr (DBAT1U));
	printf ("DBAT2L = 0x%08X\n", mfspr (DBAT2L));
	printf ("DBAT2U = 0x%08X\n", mfspr (DBAT2U));
	printf ("DBAT3L = 0x%08X\n", mfspr (DBAT3L));
	printf ("DBAT3U = 0x%08X\n", mfspr (DBAT3U));
#endif /* INCLUDE_MPC107_REPORT */
#endif /* CONFIG_MOUSSE */
	return (0);
}
