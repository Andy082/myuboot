/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * 
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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

#ifndef _PPCBOOT_ARM_H_
#define _PPCBOOT_ARM_H_	1

/* for the following variables, see start.S */
extern ulong _armboot_start;	/* code start */
extern ulong _armboot_end_data;	/* code + data end */ 
extern ulong _armboot_end;	/* BSS end */ 
extern ulong IRQ_STACK_START;	/* top of IRQ stack */
extern ulong FIQ_STACK_START;	/* top of FIQ stack */
extern ulong _armboot_real_end;	/* first usable RAM address */

/* cpu/.../cpu.c */
int	cpu_init(void);
int	cleanup_before_linux(void);

/* board/.../... */
int	board_init(void);
int	dram_init (void);

/* ------------------------------------------------------------ */
/* Here is a list of some prototypes which are incompatible to	*/
/* the PPCBoot implementation					*/
/* To be fixed!							*/
/* ------------------------------------------------------------ */
/* common/cmd_nvedit.c */
void	setenv		(char *, char *);

/* cpu/.../interrupt.c */
void	reset_timer_masked	(void);
ulong	get_timer_masked	(void);
void	udelay_masked		(unsigned long usec);

#if 0 /*------------------------------------------------------------*/
/*
 * Don't define some stuff private to armboot target code
 */
#ifndef HOST_TOOLS

/* board/.../env.c */
int 	board_env_getchar(bd_t * bd, int index, uchar *c);
int 	board_env_save	(bd_t * bd, env_t *data, int size);
int 	board_env_copy 	(bd_t * bd, env_t *data, int size);
uchar *	board_env_getaddr(bd_t * bd, int index);

#endif /* HOST_TOOLS */
#endif /* 0 ------------------------------------------------------------*/


#endif	/* _PPCBOOT_ARM_H_ */
