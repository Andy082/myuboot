#
# (C) Copyright 2000, 2001
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= libcommon.a

AOBJS	=

COBJS	= main.o altera.o bedbug.o \
	  cmd_autoscript.o cmd_bedbug.o cmd_boot.o \
	  cmd_bootm.o cmd_cache.o cmd_console.o cmd_date.o \
	  cmd_dcr.o cmd_diag.o cmd_doc.o cmd_dtt.o \
	  cmd_eeprom.o cmd_elf.o cmd_fdc.o cmd_flash.o \
	  cmd_fpga.o cmd_i2c.o cmd_ide.o cmd_immap.o \
	  cmd_jffs2.o cmd_mem.o cmd_mii.o cmd_misc.o \
	  cmd_net.o cmd_nvedit.o env_common.o \
	  env_flash.o env_eeprom.o env_nvram.o env_nowhere.o \
	  cmd_pci.o cmd_pcmcia.o \
	  cmd_reginfo.o cmd_scsi.o cmd_vfd.o cmd_usb.o \
	  command.o console.o devices.o dlmalloc.o \
	  docecc.o environment.o flash.o fpga.o \
	  hush.o kgdb.o lists.o miiphybb.o miiphyutil.o \
	  s_record.o soft_i2c.o soft_spi.o cmd_spi.o spartan2.o \
	  usb.o usb_kbd.o usb_storage.o \
	  virtex2.o xilinx.o

OBJS	= $(AOBJS) $(COBJS)

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB):	.depend $(OBJS)
	$(AR) crv $@ $(OBJS)

environment.o: environment.c ../tools/envcrc
	$(CC) $(AFLAGS) -Wa,--no-warn \
		-DENV_CRC=$(shell ../tools/envcrc) \
		-c -o $@ environment.c

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################
