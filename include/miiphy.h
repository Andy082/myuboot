/*----------------------------------------------------------------------------+
|
|       This source code has been made available to you by IBM on an AS-IS
|       basis.  Anyone receiving this source is licensed under IBM
|       copyrights to use it in any way he or she deems fit, including
|       copying it, modifying it, compiling it, and redistributing it either
|       with or without modifications.  No license under IBM patents or
|       patent applications is to be implied by the copyright license.
|
|       Any user of this software should understand that IBM cannot provide
|       technical support for this software and will not be responsible for
|       any consequences resulting from the use of this software.
|
|       Any person who transfers this source code or any derivative work
|       must include the IBM copyright notice, this paragraph, and the
|       preceding two paragraphs in the transferred software.
|
|       COPYRIGHT   I B M   CORPORATION 1999
|       LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
|
|  File Name:   miiphy.h
|
|  Function:    Include file defining PHY registers.
|
|  Author:      Mark Wisner
|
|  Change Activity-
|
|  Date        Description of Change                                       BY
|  ---------   ---------------------                                       ---
|  04-May-99   Created                                                     MKW
|  07-Jul-99   Added full duplex support                                   MKW
|  08-Sep-01   Tweaks                                                      gvb
|
+----------------------------------------------------------------------------*/
#ifndef _miiphy_h_
#define _miiphy_h_


int  miiphy_read(unsigned char addr, unsigned char reg, unsigned short * value);
int  miiphy_write(unsigned char addr, unsigned char reg, unsigned short value);
int  miiphy_info(unsigned char addr, unsigned int  *oui, unsigned char *model,
                 unsigned char *rev);
int  miiphy_reset(unsigned char addr);
int  miiphy_speed(unsigned char addr);
int  miiphy_duplex(unsigned char addr);


/* phy seed setup */
#define AUTO     99
#define _100BASET 100
#define _10BASET  10
#define HALF   22
#define FULL   44

/* phy register offsets */
#define PHY_BMCR 		0x00
#define PHY_BMSR		0x01
#define PHY_PHYIDR1 	0x02
#define PHY_PHYIDR2	0x03
#define PHY_ANAR		0x04
#define PHY_ANLPAR	0x05
#define PHY_ANER		0x06
#define PHY_ANNPTR	0x07
#define PHY_PHYSTS	0x10
#define PHY_MIPSCR	0x11
#define PHY_MIPGSR	0x12
#define PHY_DCR		0x13
#define PHY_FCSCR		0x14
#define PHY_RECR		0x15
#define PHY_PCSR		0x16
#define PHY_LBR		0x17
#define PHY_10BTSCR	0x18
#define PHY_PHYCTRL	0x19

/* PHY BMCR */
#define PHY_BMCR_RESET  	0x8000
#define PHY_BMCR_LOOP  	 	0x4000
#define PHY_BMCR_100MB		0x2000
#define PHY_BMCR_AUTON		0x1000
#define PHY_BMCR_POWD		0x0800
#define PHY_BMCR_ISO		0x0400
#define PHY_BMCR_RST_NEG	0x0200
#define PHY_BMCR_DPLX		0x0100
#define PHY_BMCR_COL_TST	0x0080

/* phy BMSR */
#define PHY_BMSR_100T4		0x8000
#define PHY_BMSR_100TXF		0x4000
#define PHY_BMSR_100TXH		0x2000
#define PHY_BMSR_10TF		0x1000
#define PHY_BMSR_10TH		0x0800
#define PHY_BMSR_PRE_SUP	0x0040
#define PHY_BMSR_AUTN_COMP	0x0020
#define PHY_BMSR_RF		0x0010
#define PHY_BMSR_AUTN_ABLE	0x0008
#define PHY_BMSR_LS		0x0004
#define PHY_BMSR_JD		0x0002
#define PHY_BMSR_EXT		0x0001

/*phy ANLPAR */
#define PHY_ANLPAR_NP		0x8000
#define PHY_ANLPAR_ACK		0x4000
#define PHY_ANLPAR_RF         0x2000
#define PHY_ANLPAR_T4         0x0200
#define PHY_ANLPAR_TXFD       0x0100
#define PHY_ANLPAR_TX         0x0080
#define PHY_ANLPAR_10FD		0x0040
#define PHY_ANLPAR_10         0x0020
#define PHY_ANLPAR_100        0x0380        /* we can run at 100 */
#endif
