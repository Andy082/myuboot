/*
 * (C) Copyright 2000-2002
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
#include <commproc.h>

#ifdef CFG_ALLOC_DPRAM

int dpram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* Reclaim the DP memory for our use. */
	gd->dp_alloc_base = CPM_DATAONLY_BASE;
	gd->dp_alloc_top  = CPM_DATAONLY_BASE + CPM_DATAONLY_SIZE;

	return (0);
}

/* Allocate some memory from the dual ported ram.  We may want to
 * enforce alignment restrictions, but right now everyone is a good
 * citizen.
 */
uint dpram_alloc (uint size)
{
	DECLARE_GLOBAL_DATA_PTR;
	uint addr = gd->dp_alloc_base;

	if ((gd->dp_alloc_base + size) >= gd->dp_alloc_top)
		return (CPM_DP_NOSPACE);

	gd->dp_alloc_base += size;

	return addr;
}

uint dpram_base (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	return gd->dp_alloc_base;
}

/* Allocate some memory from the dual ported ram.  We may want to
 * enforce alignment restrictions, but right now everyone is a good
 * citizen.
 */
uint dpram_alloc_align (uint size, uint align)
{
	DECLARE_GLOBAL_DATA_PTR;

	uint addr, mask = align - 1;

	addr = (gd->dp_alloc_base + mask) & ~mask;

	if ((addr + size) >= gd->dp_alloc_top)
		return (CPM_DP_NOSPACE);

	gd->dp_alloc_base = addr + size;

	return addr;
}

uint dpram_base_align (uint align)
{
	DECLARE_GLOBAL_DATA_PTR;

	uint mask = align - 1;

	return (gd->dp_alloc_base + mask) & ~mask;
}
#endif	/* CFG_ALLOC_DPRAM */

#ifdef CONFIG_POST

void post_word_store (ulong a)
{
	volatile void *save_addr =
		((immap_t *) CFG_IMMR)->im_cpm.cp_dpmem + CPM_POST_WORD_ADDR;

	*(volatile ulong *) save_addr = a;
}

ulong post_word_load (void)
{
	volatile void *save_addr =
		((immap_t *) CFG_IMMR)->im_cpm.cp_dpmem + CPM_POST_WORD_ADDR;

	return *(volatile ulong *) save_addr;
}

#endif	/* CONFIG_POST */
