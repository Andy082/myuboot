/*
 * (C) Copyright 2000, 2001
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
#ifndef _PART_H
#define _PART_H


typedef struct block_dev_desc {
	int           if_type;    /* type of the interface */
	int           dev;        /* device number */
	unsigned char part_type;  /* partition type */
	unsigned char target;			/* target SCSI ID */
	unsigned char lun;				/* target LUN */
	unsigned char type;				/* device type */
	unsigned long lba;        /* number of blocks */
	unsigned long blksz;			/* block size */
	unsigned char vendor[40]; /* IDE model, SCSI Vendor */
	unsigned char product[20];/* IDE Serial no, SCSI product */
	unsigned char revision[8];/* firmware revision */
	unsigned char removable;	/* removable device */
	unsigned long (*block_read)(int dev,unsigned long start,unsigned long blkcnt, unsigned long *buffer);
}block_dev_desc_t;
/* Interface types: */
#define IF_TYPE_UNKNOWN	0
#define IF_TYPE_IDE			1
#define IF_TYPE_SCSI		2
#define IF_TYPE_ATAPI		3
#define IF_TYPE_USB			4
#define IF_TYPE_DOC			5
/* Part types */
#define	PART_TYPE_UNKNOWN	0x00
#define PART_TYPE_MAC		0x01
#define PART_TYPE_DOS		0x02
#define PART_TYPE_ISO		0x03
/* device types */
#define DEV_TYPE_UNKNOWN 0xff /* not connected */
#define DEV_TYPE_HARDDISK 0x00 /* harddisk */
#define DEV_TYPE_TAPE 		0x01 /* Tape */
#define DEV_TYPE_CDROM 		0x05 /* CD-ROM */
#define DEV_TYPE_OPDISK 	0x07 /* optical disk */

typedef	struct disk_partition {
	ulong	start;		/* # of first block in partition	*/
	ulong	size;		/* number of blocks in partition	*/
	ulong	blksz;		/* block size in bytes			*/
	uchar	name[32];	/* partition name			*/
	uchar	type[32];	/* string type description		*/
} disk_partition_t;

/* disk/part.c */
int get_partition_info (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part (block_dev_desc_t *dev_desc);
void  init_part (block_dev_desc_t *dev_desc);
void dev_print(block_dev_desc_t *dev_desc);


#ifdef CONFIG_MAC_PARTITION
/* disk/part_mac.c */
int get_partition_info_mac (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_mac (block_dev_desc_t *dev_desc);
int   test_part_mac (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_DOS_PARTITION
/* disk/part_dos.c */
int get_partition_info_dos (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_dos (block_dev_desc_t *dev_desc);
int   test_part_dos (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_ISO_PARTITION
/* disk/part_iso.c */
int get_partition_info_iso (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_iso (block_dev_desc_t *dev_desc);
int   test_part_iso (block_dev_desc_t *dev_desc);
#endif

#endif /* _PART_H */


