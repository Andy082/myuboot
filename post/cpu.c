/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * CPU test
 *   
 * This test checks the arithmetic logic unit (ALU) of CPU.
 * It tests independently various groups of instructions using
 * run-time modification of the code to reduce the memory footprint.
 * For more details refer to post/cpu/ *.c files.
 */

#ifdef CONFIG_POST

#include <watchdog.h>
#include <post.h>

#if CONFIG_POST & CFG_POST_CPU

extern int cpu_post_test_cmp (void);
extern int cpu_post_test_cmpi (void);
extern int cpu_post_test_two (void);
extern int cpu_post_test_twox (void);
extern int cpu_post_test_three (void);
extern int cpu_post_test_threex (void);
extern int cpu_post_test_threei (void);
extern int cpu_post_test_andi (void);
extern int cpu_post_test_srawi (void);
extern int cpu_post_test_rlwnm (void);
extern int cpu_post_test_rlwinm (void);
extern int cpu_post_test_rlwimi (void);
extern int cpu_post_test_store (void);
extern int cpu_post_test_load (void);
extern int cpu_post_test_cr (void);
extern int cpu_post_test_b (void);
extern int cpu_post_test_multi (void);
extern int cpu_post_test_string (void);
extern int cpu_post_test_complex (void);

ulong cpu_post_makecr (long v)
{
	ulong cr = 0;

	if (v < 0)
		cr |= 0x80000000;
	if (v > 0)
		cr |= 0x40000000;
	if (v == 0)
		cr |= 0x20000000;

	return cr;
}

int cpu_post_test (int flags)
{
	int ic = icache_status ();
	int ret = 0;

	if (ic)
		icache_disable ();

	if (ret == 0)
		ret = cpu_post_test_cmp ();
	if (ret == 0)
		ret = cpu_post_test_cmpi ();
	if (ret == 0)
		ret = cpu_post_test_two ();
	if (ret == 0)
		ret = cpu_post_test_twox ();
	if (ret == 0)
		ret = cpu_post_test_three ();
	if (ret == 0)
		ret = cpu_post_test_threex ();
	if (ret == 0)
		ret = cpu_post_test_threei ();
	WATCHDOG_RESET();
	if (ret == 0)
		ret = cpu_post_test_andi ();
	if (ret == 0)
		ret = cpu_post_test_srawi ();
	if (ret == 0)
		ret = cpu_post_test_rlwnm ();
	if (ret == 0)
		ret = cpu_post_test_rlwinm ();
	if (ret == 0)
		ret = cpu_post_test_rlwimi ();
	if (ret == 0)
		ret = cpu_post_test_store ();
	if (ret == 0)
		ret = cpu_post_test_load ();
	WATCHDOG_RESET();
	if (ret == 0)
		ret = cpu_post_test_cr ();
	if (ret == 0)
		ret = cpu_post_test_b ();
	if (ret == 0)
		ret = cpu_post_test_multi ();
	if (ret == 0)
		ret = cpu_post_test_string ();
	if (ret == 0)
		ret = cpu_post_test_complex ();

	if (ic)
		icache_enable ();

	return ret;
}

#endif /* CONFIG_POST & CFG_POST_CPU */
#endif /* CONFIG_POST */
