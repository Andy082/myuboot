/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
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

#ifndef _VIDEO_AD7176_H_
#define _VIDEO_AD7176_H_

#define VIDEO_ENCODER_NAME	"Analog Devices AD7176"

#define VIDEO_ENCODER_I2C_RATE	100000	/* Max rate is 100Khz	*/
#define VIDEO_ENCODER_CB_Y_CR_Y		/* Use CB Y CR Y format...	*/

#define VIDEO_MODE_YUYV		/* The only mode supported by this encoder	*/
#undef  VIDEO_MODE_RGB
#define VIDEO_MODE_BPP		16

#ifdef  VIDEO_MODE_PAL
#define VIDEO_ACTIVE_COLS	720
#define VIDEO_ACTIVE_ROWS	576
#define VIDEO_VISIBLE_COLS	640
#define VIDEO_VISIBLE_ROWS	480
#endif

#ifdef 	VIDEO_MODE_NTSC
#define VIDEO_ACTIVE_COLS	720
#define VIDEO_ACTIVE_ROWS	525
#define VIDEO_VISIBLE_COLS	640
#define VIDEO_VISIBLE_ROWS	400
#endif

static unsigned char video_encoder_data[] = {
#ifdef VIDEO_MODE_NTSC
    				        0x04, /* Mode Register 0	*/
#ifdef VIDEO_DEBUG_COLORBARS
					0x82,
#else
	                                0x02, /* Mode Register 1	*/
#endif
                                        0x16, /* Subcarrier Freq 0	*/
                                        0x7c, /* Subcarrier Freq 1	*/
                            		0xf0, /* Subcarrier Freq 2	*/
                                        0x21, /* Subcarrier Freq 3	*/
                                        0x00, /* Subcarrier phase	*/
                                        0x02, /* Timing Register 0	*/
                                	0x00, /* Extended Captioning 0	*/
                                        0x00, /* Extended Captioning 1	*/
                                        0x00, /* Closed Captioning 0	*/
                                	0x00, /* Closed Captioning 1	*/
                                        0x00, /* Timing Register 1	*/
                                        0x08, /* Mode Register 2	*/
                                	0x00, /* Pedestal Register 0	*/
                                        0x00, /* Pedestal Register 1	*/
                                        0x00, /* Pedestal Register 2	*/
                                        0x00, /* Pedestal Register 3	*/
                                        0x00  /* Mode Register 3	*/

#endif	/* VIDEO_MODE_NTSC */

#ifdef VIDEO_MODE_PAL
    				        0x05, /* Mode Register 0	*/
#ifdef VIDEO_DEBUG_COLORBARS
					0x82,
#else
	                                0x02, /* Mode Register 1 (2)	*/
#endif
                                        0xcb, /* Subcarrier Freq 0	*/
                                        0x8a, /* Subcarrier Freq 1	*/
                                        0x09, /* Subcarrier Freq 2	*/
                                        0x2a, /* Subcarrier Freq 3	*/
                                        0x00, /* Subcarrier phase	*/
                                        0x0a, /* Timing Register 0 (a)	*/
                                        0x00, /* Extended Captioning 0	*/
                                        0x00, /* Extended Captioning 1	*/
                                        0x00, /* Closed Captioning 0	*/
                                        0x00, /* Closed Captioning 1	*/
                                        0x00, /* Timing Register 1	*/
                                        0x08, /* Mode Register 2 (8)	*/
                                        0x00, /* Pedestal Register 0	*/
                                        0x00, /* Pedestal Register 1	*/
                                        0x00, /* Pedestal Register 2	*/
                                        0x00, /* Pedestal Register 3	*/
                                        0x00  /* Mode Register 3	*/
#endif	/* VIDEO_MODE_PAL */
} ;

#endif	/* _VIDEO_AD7176_H_ */
