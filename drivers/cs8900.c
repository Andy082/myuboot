/*
 * Cirrus Logic CS8900A Ethernet
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 1999 Ben Williamson <benw@pobox.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is loaded into SRAM in bootstrap mode, where it waits
 * for commands on UART1 to read and write memory, jump to code etc.
 * A design goal for this program is to be entirely independent of the
 * target board.  Anything with a CL-PS7111 or EP7211 should be able to run
 * this code in bootstrap mode.  All the board specifics can be handled on
 * the host.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <command.h>
#include "cs8900.h"
#include <net.h>

#ifdef CONFIG_DRIVER_CS8900

#if (CONFIG_COMMANDS & CFG_CMD_NET)


/* packet page register access functions */

#ifdef CS8900_BUS32
/* we don't need 16 bit initialisation on 32 bit bus */
#define get_reg_init_bus(x) get_reg((x))
#else
static unsigned short get_reg_init_bus(int regno)
{
    /* force 16 bit busmode */
    volatile unsigned char c;
    c = CS8900_BUS16_0;
    c = CS8900_BUS16_1;
    c = CS8900_BUS16_0;
    c = CS8900_BUS16_1;
    c = CS8900_BUS16_0;

    CS8900_PPTR = regno;
    return (unsigned short) CS8900_PDATA;
}
#endif

static unsigned short
get_reg(int regno)
{
    CS8900_PPTR = regno;
    return (unsigned short) CS8900_PDATA;
}


static void put_reg(int regno, unsigned short val)
{
    CS8900_PPTR = regno;
    CS8900_PDATA = val;
}

static void eth_reset(void)
{
    int tmo;
    unsigned short us;

    /* reset NIC */
    put_reg(PP_SelfCTL, get_reg(PP_SelfCTL) | PP_SelfCTL_Reset );

    /* wait for 200ms */
    udelay(200000);
    /* Wait until the chip is reset */

    tmo = get_timer(0) + 1 * CFG_HZ;
    while ((((us = get_reg_init_bus(PP_SelfSTAT)) & PP_SelfSTAT_InitD) == 0)
	   && tmo < get_timer(0))
      /*NOP*/;
}

void cs8900_get_enetaddr (uchar *addr)
{
    int i;
    /* verify chip id */
    if (get_reg_init_bus(PP_ChipID) != 0x630e)
	return;
    eth_reset();    
    if ((get_reg(PP_SelfST) & (PP_SelfSTAT_EEPROM | PP_SelfSTAT_EEPROM_OK)) == 
	      (PP_SelfSTAT_EEPROM|PP_SelfSTAT_EEPROM_OK)) {
	/* Load the MAC from EEPROM */      
	for (i=0; i<6/2; i++) {
	    unsigned int Addr;
	    Addr = get_reg(PP_IA+i*2);
	    addr[i*2] = Addr & 0xFF;
	    addr[i*2+1] = Addr >> 8;
	}
    }		
}

void eth_halt( void )
{
    /* disable transmitter/receiver mode */
    put_reg(PP_LineCTL, 0);

    /* "shutdown" to show ChipID or kernel wouldn't find he cs8900 ... */
    get_reg_init_bus(PP_ChipID);
}

int eth_init( bd_t *bd )
{
    
    /* verify chip id */
    if (get_reg_init_bus(PP_ChipID) != 0x630e)
    {
	printf( "CS8900 Ethernet chip not found?!\n" );
	return 0;
    }

    eth_reset();
    /* set the ethernet address */
    put_reg(PP_IA + 0, bd->bi_enetaddr[0] | (bd->bi_enetaddr[1] << 8));
    put_reg(PP_IA + 2, bd->bi_enetaddr[2] | (bd->bi_enetaddr[3] << 8));
    put_reg(PP_IA + 4, bd->bi_enetaddr[4] | (bd->bi_enetaddr[5] << 8));
	     
    /* receive only error free packets addressed to this card */
    put_reg(PP_RxCTL, PP_RxCTL_IA | PP_RxCTL_Broadcast | PP_RxCTL_RxOK);

    /* do not generate any interrupts on receive operations */
    put_reg(PP_RxCFG, 0);

    /* do not generate any interrupts on transmit operations */
    put_reg(PP_TxCFG, 0);

    /* do not generate any interrupts on buffer operations */
    put_reg(PP_BufCFG, 0);

    /* enable transmitter/receiver mode */
    put_reg(PP_LineCTL, PP_LineCTL_Rx | PP_LineCTL_Tx);

    return 0;
}

/* Get a data block via Ethernet */
extern int eth_rx(void)
{
    int i;
    unsigned short rxlen;
    unsigned short *addr;
    unsigned short status;

    status = get_reg(PP_RER);

    if ((status & PP_RER_RxOK) == 0)
      return 0;

    status = CS8900_RTDATA;       /* stat */
    rxlen = CS8900_RTDATA;        /* len */

    if(rxlen > PKTSIZE_ALIGN + PKTALIGN)
      printf("packet too big!\n");

    for(addr = (unsigned short *)NetRxPackets[0], i = rxlen >> 1; i > 0; i-- )
      *addr++ = CS8900_RTDATA;
    if(rxlen & 1)
      *addr++ = CS8900_RTDATA;

    /* Pass the packet up to the protocol layers. */
    NetReceive(NetRxPackets[0], rxlen);

    return rxlen;
}

/* Send a data block via Ethernet. */
extern int eth_send(volatile void *packet, int length)
{
    volatile unsigned short *addr;
    int tmo;
    unsigned short s;

retry:
    /* initiate a transmit sequence */
    CS8900_TxCMD = PP_TxCmd_TxStart_Full;
    CS8900_TxLEN = length;
    
    /* Test to see if the chip has allocated memory for the packet */
    if ((get_reg(PP_BusSTAT) & PP_BusSTAT_TxRDY) == 0) {    
	/* Oops... this should not happen! */
	printf("cs: unable to send packet; retrying...\n");
	for (tmo = get_timer(0) + 5 * CFG_HZ; get_timer(0) < tmo; )
	  /*NOP*/;
	eth_reset();
	goto retry; 
    }

    /* Write the contents of the packet */
    /* assume even number of bytes */
    for(addr = packet; length > 0; length -= 2 )
      CS8900_RTDATA = *addr++;

    /* wait for transfer to succeed */
    tmo = get_timer(0) + 5 * CFG_HZ;
    while((s = get_reg(PP_TER) & ~0x1F) == 0) 
    {
	if (get_timer(0) >= tmo)
	  break;
    }
     
      /* nothing */ ;
    if ((s & (PP_TER_CRS | PP_TER_TxOK)) != PP_TER_TxOK) {
	printf("\ntransmission error %#x\n", s);
    }

    return 0;
}

#endif /* COMMANDS & CFG_NET */

#endif /* CONFIG_DRIVER_CS8900 */
