/*
 * (C) Copyright 2001
 * Erik Theisen,  Wave 7 Optics, etheisen@mindspring.com.
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

/*
 * Watchdog functions and macros.
 */
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#if defined(CONFIG_HW_WATCHDOG) && defined(CONFIG_WATCHDOG)
#  error "Configuration error: CONFIG_HW_WATCHDOG and CONFIG_WATCHDOG can't be used together."
#endif

/*
 * Hardware watchdog
 */
#ifdef CONFIG_HW_WATCHDOG
	#if defined(__ASSEMBLY__)
		#define WATCHDOG_RESET bl hw_watchdog_reset
	#else
		extern void hw_watchdog_reset(void);

		#define WATCHDOG_RESET hw_watchdog_reset
	#endif /* __ASSEMBLY__ */
#else
	/*
	 * Maybe a software watchdog?
	 */
	#if defined(CONFIG_WATCHDOG)
		#if defined(__ASSEMBLY__)
			#define WATCHDOG_RESET bl watchdog_reset
		#else
			extern void watchdog_reset(void);

			#define WATCHDOG_RESET watchdog_reset
		#endif
	#else
		/*
		 * No hardware or software watchdog.
		 */
		#if defined(__ASSEMBLY__)
			#define WATCHDOG_RESET /*XXX DO_NOT_DEL_THIS_COMMENT*/
		#else
			#define WATCHDOG_RESET() {}
		#endif /* __ASSEMBLY__ */
	#endif /* CONFIG_WATCHDOG && !__ASSEMBLY__ */
#endif /* CONFIG_HW_WATCHDOG */

/*
 * Prototypes from $(CPU)/cpu.c.
 */

/* MPC 8xx */
#if (defined(CONFIG_8xx) || defined(CONFIG_MPC860)) && !defined(__ASSEMBLY__)
	void reset_8xx_watchdog(volatile immap_t *immr);
#endif

/* IBM 4xx */
#if defined(CONFIG_4xx) && !defined(__ASSEMBLY__)
	void reset_4xx_watchdog(void);
#endif

/* MPC 8260 */
#if defined(CONFIG_MPC8260) && !defined(__ASSEMBLY__)
#if defined(CONFIG_WATCHDOG)
extern __inline__ void
reset_8260_watchdog(volatile immap_t *immr)
{
	immr->im_siu_conf.sc_swsr = 0x556c;
	immr->im_siu_conf.sc_swsr = 0xaa39;
}
#endif /* !__ASSEMBLY__ && CONFIG_WATCHDOG */
#endif /* CONFIG_MPC8260 && !__ASSEMBLY__ */

#endif /* _WATCHDOG_H_ */

