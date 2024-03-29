/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <command.h>
#include <cmd_boot.h>
#include <image.h>
#include <zlib.h>
#include <asm/byteorder.h>

#include <asm/setup.h>
#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)
#define tag_next(t)     ((struct tag *)((u32 *)(t) + (t)->hdr.size))

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_VFD)
static void setup_start_tag(bd_t *bd);
static void setup_memory_tags(bd_t *bd);
static void setup_commandline_tag(bd_t *bd, char *commandline);
#if 0
static void setup_ramdisk_tag(bd_t *bd);
#endif
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end);
static void setup_end_tag(bd_t *bd);
#if defined (CONFIG_VFD)
static void setup_videolfb_tag(gd_t *gd);
#endif


static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

extern image_header_t header;           /* from cmd_bootm.c */


void do_bootm_linux(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		ulong addr, ulong *len_ptr, int   verify)
{
	DECLARE_GLOBAL_DATA_PTR;

    ulong len = 0, checksum;
    ulong initrd_start, initrd_end;
    ulong data;
    void (*theKernel)(int zero, int arch);
    image_header_t *hdr = &header;
    bd_t *bd = gd->bd;
#ifdef CONFIG_CMDLINE_TAG
    char *commandline = getenv("bootargs");
#endif

    theKernel = (void (*)(int, int))ntohl(hdr->ih_ep);
   
    /*
     * Check if there is an initrd image
     */
    if (argc >= 3) {
	addr = simple_strtoul(argv[2], NULL, 16);
	
	printf ("## Loading Ramdisk Image at %08lx ...\n", addr);
	
	/* Copy header so we can blank CRC field for re-calculation */
	memcpy (&header, (char *)addr, sizeof(image_header_t));
	
	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
	    printf ("Bad Magic Number\n");
	    do_reset (cmdtp, flag, argc, argv);
	}
	
	data = (ulong)&header;
	len  = sizeof(image_header_t);
	
	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	
	if (crc32 (0, (char *)data, len) != checksum) {
	    printf ("Bad Header Checksum\n");
	    do_reset (cmdtp, flag, argc, argv);
	}
	
	print_image_hdr (hdr);
	
	data = addr + sizeof(image_header_t);
	len  = ntohl(hdr->ih_size);
	
	if (verify) {
	    ulong csum = 0;

	    printf ("   Verifying Checksum ... ");
	    csum = crc32 (0, (char *)data, len);
	    if (csum != ntohl(hdr->ih_dcrc)) {
		printf ("Bad Data CRC\n");
		do_reset (cmdtp, flag, argc, argv);
	    }
	    printf ("OK\n");
	}
	
	if ((hdr->ih_os   != IH_OS_LINUX)	||
	    (hdr->ih_arch != IH_CPU_ARM)	||
	    (hdr->ih_type != IH_TYPE_RAMDISK)	) {
	    printf ("No Linux ARM Ramdisk Image\n");
	    do_reset (cmdtp, flag, argc, argv);
	}
	
	/*
	 * Now check if we have a multifile image
	 */
    } else if ((hdr->ih_type==IH_TYPE_MULTI) && (len_ptr[1])) {
	ulong tail    = ntohl(len_ptr[0]) % 4;
	int i;
	
	/* skip kernel length and terminator */
	data = (ulong)(&len_ptr[2]);
	/* skip any additional image length fields */
	for (i=1; len_ptr[i]; ++i)
	  data += 4;
	/* add kernel length, and align */
	data += ntohl(len_ptr[0]);
	if (tail) {
	    data += 4 - tail;
	}
	
	len   = ntohl(len_ptr[1]);
	
    } else {
	/*
	 * no initrd image
	 */
	data = 0;
    }
    
#ifdef	DEBUG
    if (!data) {
	printf ("No initrd\n");
    }
#endif
    
    if (data) {
	initrd_start = data;
	initrd_end   = initrd_start + len;
    } else {
	initrd_start = 0;
	initrd_end = 0;
    }
    
#ifdef DEBUG
    printf ("## Transferring control to Linux (at address %08lx) ...\n",
	    (ulong)theKernel);
#endif

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_VFD)
    setup_start_tag(bd);
#ifdef CONFIG_SETUP_MEMORY_TAGS
    setup_memory_tags(bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
    setup_commandline_tag(bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
    setup_initrd_tag(bd, initrd_start, initrd_end);
#endif
#if 0
    setup_ramdisk_tag(bd);
#endif
#if defined (CONFIG_VFD)
    setup_videolfb_tag(gd);
#endif
    setup_end_tag(bd);
#endif

    /* we assume that the kernel is in place */
    printf("\nStarting kernel ...\n\n");

    cleanup_before_linux();

    theKernel(0, bd->bi_arch_number);
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_VFD)
static void setup_start_tag(bd_t *bd)
{
    params = (struct tag *)bd->bi_boot_params;
    
    params->hdr.tag = ATAG_CORE;
    params->hdr.size = tag_size(tag_core);
    
    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0;
    
    params = tag_next(params);
}


static void setup_memory_tags(bd_t *bd)
{
    int i;
    
    for(i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size(tag_mem32);
	
	params->u.mem.start = bd->bi_dram[i].start;
	params->u.mem.size = bd->bi_dram[i].size;
	    
	params = tag_next(params);
    }
}


static void setup_commandline_tag(bd_t *bd, char *commandline)
{
    char *p;
    
    /* eat leading white space */
    for(p = commandline; *p == ' '; p++)
      ;
    
    /* skip non-existent command lines so the kernel will still
     * use its default command line.
     */
    if(*p == '\0')
      return;
    
    params->hdr.tag = ATAG_CMDLINE;
    params->hdr.size = (sizeof(struct tag_header) + strlen(p) + 1 + 4) >> 2;
    
    strcpy(params->u.cmdline.cmdline, p);
    
    params = tag_next(params);
}


#ifndef ATAG_INITRD2
#define ATAG_INITRD2    0x54420005
#endif
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
    /* an ATAG_INITRD node tells the kernel where the compressed
     * ramdisk can be found. ATAG_RDIMG is a better name, actually.
     */
    params->hdr.tag = ATAG_INITRD2;
    params->hdr.size = tag_size(tag_initrd);
    
    params->u.initrd.start = initrd_start;
    params->u.initrd.size = initrd_end - initrd_start;
    
    params = tag_next(params);
}


#if 0
static void setup_ramdisk_tag(bd_t *bd)
{
    /* an ATAG_RAMDISK node tells the kernel how large the
     * decompressed ramdisk will become.
     */
    params->hdr.tag = ATAG_RAMDISK;
    params->hdr.size = tag_size(tag_ramdisk);
    
    params->u.ramdisk.start = 0;
    /*params->u.ramdisk.size = RAMDISK_SIZE; */
    params->u.ramdisk.flags = 1;	/* automatically load ramdisk */
 
    params = tag_next(params);
}
#endif /* 0 */

#if defined (CONFIG_VFD)
static void setup_videolfb_tag(gd_t *gd)
{
    /* An ATAG_VIDEOLFB node tells the kernel where and how large
     * the framebuffer for video was allocated (among other things).
     * Note that a _physical_ address is passed !
     *
     * We only use it to pass the address and size, the other entries
     * in the tag_videolfb are not of interest.
     */
    params->hdr.tag = ATAG_VIDEOLFB;
    params->hdr.size = tag_size(tag_videolfb);

    params->u.videolfb.lfb_base = (u32)gd->fb_base;
    /* 7168 = 256*4*56/8 - actually 2 pages (8192 bytes) are allocated */
    params->u.videolfb.lfb_size = 7168;
 
    params = tag_next(params);
}
#endif

static void setup_end_tag(bd_t *bd)
{
    params->hdr.tag = ATAG_NONE;
    params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
