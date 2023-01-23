/* 
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */

/*
 * HISTORY
 * $Log:	conf.c,v $
 * Revision 2.12  93/05/17  16:36:28  rvb
 * 	Type casts, etc to quiet gcc 2.3.3 warnings
 * 	[93/05/17            rvb]
 * 
 * Revision 2.10  93/03/09  10:55:49  danner
 * 	Added CD-ROM Audio.
 * 	[93/03/06  14:36:30  af]
 * 
 * Revision 2.9  93/02/05  07:50:23  danner
 * 	Include for vm_offset_t, new mouse routines.
 * 	[93/02/04            jfriedl]
 * 
 * Revision 2.8  93/01/14  17:37:49  danner
 * 	support for frame buffer, mouse, and DIP [from Omron]
 * 	Added mapped ethernet, made it default 'am' device.
 * 	[92/10/19            jfriedl]
 * 
 * Revision 2.7  92/08/03  17:41:03  jfriedl
 * 	Added support for /dev/ttya and /dev/kbd.
 * 	Added LCD support (from Nagaoka). Cleaned up.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.6.1.1  92/07/18  00:22:38  jfriedl
 * 
 * Revision 2.6  92/05/04  11:26:22  danner
 * 	Added ethernet.
 * 	[92/04/21            danner]
 * 
 * Revision 2.5  92/01/03  20:40:29  dbg
 * 	Added devinfo routine to scsi to accomodate MI change
 * 	that screwed up extra large writes.
 * 	[91/12/26  11:10:01  af]
 * 
 * Revision 2.4  91/08/28  11:14:59  jsb
 * 	Added entries for new dev_info field.
 * 	[91/08/27  17:58:24  jsb]
 * 
 * Revision 2.3  91/07/11  11:00:41  danner
 * 	Copyright Fixes
 * 
 * Revision 2.2  91/07/09  23:16:51  danner
 * 	Initial 3.0 Checkin
 * 	[91/06/26  11:58:45  danner]
 * 
 * Revision 2.2  90/12/04  14:49:59  jsb
 * 	First checkin.
 * 	[90/12/03  21:47:04  jsb]
 * 
 */

#include <mach/m88k/vm_types.h>         /* vm_offset_t */
#include <device/conf.h>

/* Console */
#define   cnname "console"
extern int cngetstat(), cnsetstat(), cnportdeath();
extern int cnopen(), cnclose(), cnread(), cnwrite();
extern vm_offset_t cnmmap();

/* Mapped time */
#define timename "time"
extern int timeopen(),timeclose();
extern vm_offset_t timemmap();

/* Scsi driver */
#define rz_name "sd"
extern int rz_get_status(), rz_set_status(), rz_devinfo();
extern int rz_open(), rz_close(), rz_read(), rz_write();

/* SCSI CD-ROM-DA */
#define cdname		"cd_audio"	/* CD-ROM DA */
int	cd_open(), cd_close(), cd_read(), cd_write();

/* ethernet (named am for 2.5 compat) */
#define se_name "se"
extern int se_setinput(), se_restart(), se_set_status();
extern int se_open(), se_output(), se_get_status();

#define SE_name "am"
extern int SE_open(), SE_close(), SE_portdeath();
extern int SE_get_status(), SE_set_status();
extern vm_offset_t SE_mmap();

/* Frame Buffer */
int     fb_open(), fb_close(), fb_get_status(), fb_set_status();
vm_offset_t fb_mmap();
#define fb_name "fb"

/* Dip Switches */
int     dip_open(), dip_close(), dip_get_status();
#define dip_name "dip"

/* Mouse */
int     mouse_open(), mouse_close(), mouse_read(), mouse_get_status(),
	mouse_set_status();
#define ms_name "ms"

/* LCD driver */
#define lcd_name "lcd"
extern int lcd_open(), lcd_close(), lcd_write();

/*
 * List of devices - console must be at slot 0
 * nulldev() means not there, but no error to try.
 * nodev() means not there, and an error is returned.
 */
struct dev_ops dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev,
	  dev_info */


	{ cnname,	cnopen,		cnclose,	cnread,
	  cnwrite,	cngetstat,	cnsetstat,	cnmmap,
	  nodev,	nulldev,	cnportdeath,	0,
	  nodev },

	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev,	0,
	  nodev },

	{ rz_name,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nomap,
	  nodev,	nulldev,	nulldev,	8,
	  rz_devinfo },

	{ cdname,	cd_open,	cd_close,	cd_read,
	  cd_write,	nodev,		nodev,		nomap,
	  nodev,	nulldev,	nulldev,	8,
	  nodev },

	{ se_name,	se_open,	nulldev,	nodev,
	  se_output,	se_get_status,	se_set_status,	nomap,
	  se_setinput,	se_restart,	nulldev,	0,
	  nodev },

	{ SE_name,      SE_open,        SE_close,       nulldev,
	  nulldev,      SE_get_status,  SE_set_status,  SE_mmap,
	  nodev,        nulldev,        SE_portdeath,   0,
	  nodev },

	{ lcd_name,     lcd_open,       lcd_close,      nodev,
	  lcd_write,    nodev,          nodev,          nomap,
	  nodev,        nulldev,        nulldev,        0,
	  nodev },

	{ fb_name,      fb_open,        fb_close,       nulldev,
	  nulldev,      fb_get_status,  fb_set_status,  fb_mmap,
	  nulldev,      nulldev,        nulldev,        0,
	  nodev },

	{ dip_name,     dip_open,       dip_close,      nulldev,
	  nulldev,      dip_get_status, nulldev,        nomap,
	  nulldev,      nulldev,        nulldev,        0,
	  nodev },

	{ ms_name,     mouse_open,      mouse_close,     mouse_read,
	  nulldev,     mouse_get_status,mouse_set_status,nomap,
	  nulldev,     nulldev,         nulldev,         0,
	  nodev },

	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev,
	  dev_info */
};

int dev_name_count = sizeof(dev_name_list)/sizeof(dev_name_list[0]);

/*
 * Indirect list.
 */
struct dev_indirect dev_indirect_list[] = {
	{ "console",	&dev_name_list[0],	0 },
	{ "kbd",        &dev_name_list[0],      1 },
	{ "ttya",       &dev_name_list[0],      2 },
};

int dev_indirect_count = sizeof(dev_indirect_list) /
			sizeof(dev_indirect_list[0]);
