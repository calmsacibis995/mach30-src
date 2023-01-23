/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log:	kd_mouse.c,v $
 * Revision 2.16  93/08/10  15:57:52  mrt
 * 	Make protocol 5 (-- minor [40-47] ) be Microsoft mouse
 * 	but 7 bits.  If this works universally, change protocol 1.
 * 	[93/08/03            rvb]
 * 	Support Logitech TrackMan mouse (V [or M+]) protocol as
 * 	protocol 4 -- minor [32-39]
 * 
 * Revision 2.15  93/05/11  13:58:56  rvb
 * 	Typo
 * 
 * Revision 2.14  93/05/10  21:19:45  rvb
 * 	Lint.
 * 	[93/05/08  11:22:15  af]
 * 
 * Revision 2.13  93/05/10  17:43:39  rvb
 * 	Allow for mouse speed to be patchable
 * 
 * Revision 2.12  93/01/24  13:18:05  danner
 * 	Added 3 button ps2 mouse from Jim Reese.
 * 	[93/01/19  13:15:08  rvb]
 * 
 * Revision 2.11  93/01/14  17:31:28  danner
 * 	Proper spl typing.
 * 	[92/11/30            af]
 * 
 * Revision 2.10  92/07/09  22:54:39  rvb
 * 	Fix microsoft 2 button mouse emulation to ignore two buttons down
 * 	simultaneously and let X11R5 handle it.
 * 	[92/07/06            rvb]
 * 
 * Revision 2.9  92/02/19  16:30:17  elf
 * 	On ps2_mouse_close, wait a bit and then drain console.  For
 * 	T2200 and sometime the IBM axion bus extender, data will show
 * 	up after a while (a 0) w/o interrupt and your kbd would hang.
 * 	[92/01/30            rvb]
 * 
 * 	Allow for only "keyboard mouse".  Does not use serial line.
 * 	[92/01/25            rvb]
 * 
 * Revision 2.8  91/10/07  17:26:02  af
 * 	Add support for ibm ps2 "keyboard" mouse.  Try to be general
 * 	about it to support other "keyboard mice", but we don't have
 * 	any.
 * 	[91/09/25            rvb]
 * 	Add support for MicroSoft 2 button Mouse from 2.5
 * 	[91/09/04            rvb]
 * 
 * Revision 2.7  91/08/24  11:58:51  af
 * 	New MI autoconf.
 * 	[91/08/02  02:49:38  af]
 * 
 * Revision 2.6  91/05/14  16:27:26  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:19:17  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:45:43  mrt]
 * 
 * Revision 2.4  91/01/08  17:33:25  rpd
 * 	Trim $ Header
 * 	[90/11/27  11:44:31  rvb]
 * 
 * Revision 2.3  90/11/26  14:50:33  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.5.1.5) & XMK35 (r2.3)
 * 	[90/11/15            rvb]
 * 
 * Revision 1.5.1.4  90/07/10  11:45:04  rvb
 * 	Allow mouse minor to select the serial line the mouse is on.
 * 	[90/06/15            rvb]
 * 
 * Revision 2.2  90/05/21  13:27:25  dbg
 * 	Convert for pure kernel.
 * 	[90/04/24            dbg]
 * 
 * Revision 1.5.1.3  90/02/28  15:50:25  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 1.5.1.2  90/01/08  13:30:55  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.5.1.1  89/10/22  11:34:32  rvb
 * 	Received from Intel October 5, 1989.
 * 	[89/10/13            rvb]
 * 
 * Revision 1.5  89/07/17  10:41:24  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.4.1.1  89/04/27  12:21:17  kupfer
 * Merge X79 with our latest and greatest.
 * 
 * Revision 1.2.1.1  89/04/27  11:54:40  kupfer
 * X79 from CMU.
 * 
 * Revision 1.4  89/03/09  20:06:46  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:42:38  gm0w
 * 	Changes for cleanup.
 * 
 */
 
/* **********************************************************************
 File:         kd_mouse.c
 Description:  mouse driver as part of keyboard/display driver

 $ Header: $

 Copyright Ing. C. Olivetti & C. S.p.A. 1989.
 All rights reserved.
********************************************************************** */
/*
  Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
Cupertino, California.

		All Rights Reserved

  Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Olivetti
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

  OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
 * Hacked up support for serial mouse connected to COM1, using Mouse
 * Systems 5-byte protocol at 1200 baud.  This should work for
 * Mouse Systems, SummaMouse, and Logitek C7 mice.
 *
 * The interface provided by /dev/mouse is a series of events as
 * described in i386at/kd.h.
 */

#include <mach/boolean.h>
#include <sys/types.h>
#ifdef	MACH_KERNEL
#include <device/errno.h>
#include <device/io_req.h>
#else	MACH_KERNEL
#include <sys/file.h>
#include <sys/errno.h>
#include <kern/thread.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#endif	MACH_KERNEL
#include <i386/ipl.h>
#include <chips/busses.h>
#include <i386at/kd.h>
#include <i386at/kd_queue.h>
#include <i386at/i8250.h>

static int (*oldvect)();		/* old interrupt vector */
static int oldunit;
static spl_t oldspl;
extern	struct	bus_device *cominfo[];

kd_event_queue mouse_queue;		/* queue of mouse events */
boolean_t mouse_in_use = FALSE;
#ifdef	MACH_KERNEL
queue_head_t	mouse_read_queue = { &mouse_read_queue, &mouse_read_queue };
#else	MACH_KERNEL
struct proc *mouse_sel = 0;		/* selecting process, if any */
short mousepgrp = 0;		/* process group leader when dev is open */
#endif	MACH_KERNEL

#ifdef	MACH_KERNEL
#else	MACH_KERNEL
int mouseflag = 0;
#define MOUSE_COLL	1		/* select collision */
#define MOUSE_ASYNC	2		/* user wants asynch notification */
#define MOUSE_NBIO	4		/* user wants non-blocking I/O */
#endif	MACH_KERNEL

/*
 * The state of the 3 buttons is encoded in the low-order 3 bits (both
 * here and in other variables in the driver).
 */
u_char lastbuttons;		/* previous state of mouse buttons */
#define MOUSE_UP	1
#define MOUSE_DOWN	0
#define MOUSE_ALL_UP	0x7

int mouseintr();
void mouse_enqueue();
int mouse_baud = BCNT1200;

/*
 * init_mouse_hw - initialize the serial port.
 */
init_mouse_hw(unit, mode)
{
	caddr_t base_addr  = (caddr_t)cominfo[unit]->address;

	outb(base_addr + RIE, 0);
	outb(base_addr + RLC, LCDLAB);
	outb(base_addr + RDLSB, mouse_baud & 0xff);
	outb(base_addr + RDMSB, (mouse_baud >> 8) & 0xff);
	outb(base_addr + RLC, mode);
	outb(base_addr + RMC, MCDTR | MCRTS | MCOUT2);
	outb(base_addr + RIE, IERD | IELS);
}


/*
 * mouseopen - Verify that the request is read-only, initialize,
 * and remember process group leader.
 */
/*
 * Low 3 bits of minor are the com port #.
 * The high 5 bits of minor are the mouse type
 */
#define	MOUSE_SYSTEM_MOUSE	0
#define MICROSOFT_MOUSE		1
#define IBM_MOUSE		2
#define NO_MOUSE		3
#define LOGITECH_TRACKMAN	4
#define	MICROSOFT_MOUSE7	5
static int mouse_type;
static int mousebufsize;
int track_man[10];

/*ARGSUSED*/
mouseopen(dev, flags)
	dev_t dev;
	int flags;
{
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	if (flags & FWRITE)
		return(ENODEV);
#endif	MACH_KERNEL
	if (mouse_in_use)
		return(EBUSY);
	mouse_in_use = TRUE;		/* locking? */
	kdq_reset(&mouse_queue);
	lastbuttons = MOUSE_ALL_UP;
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	mousepgrp = u.u_procp->p_pgrp;
#endif	MACH_KERNEL

	switch (mouse_type = ((minor(dev) & 0xf8) >> 3)) {
	case MICROSOFT_MOUSE7:
		mousebufsize = 3;
		serial_mouse_open(dev);
		init_mouse_hw(dev&7, LC7);
	case MICROSOFT_MOUSE:
		mousebufsize = 3;
		serial_mouse_open(dev);
		init_mouse_hw(dev&7, LC8);
		break;
	case MOUSE_SYSTEM_MOUSE:
		mousebufsize = 5;
		serial_mouse_open(dev);
		init_mouse_hw(dev&7, LC8);
		break;
	case LOGITECH_TRACKMAN:
		mousebufsize = 3;
		serial_mouse_open(dev);
		init_mouse_hw(dev&7, LC7);
		track_man[0] = comgetc(dev&7);
		track_man[1] = comgetc(dev&7);
		if (track_man[0] != 0x4d && 
		    track_man[1] != 0x33) {
			printf("LOGITECH_TRACKMAN: NOT M3");
		}
		break;
	case IBM_MOUSE:
		mousebufsize = 3;
		kd_mouse_open(dev, 12);
		ibm_ps2_mouse_open(dev);
		break;
	case NO_MOUSE:
		break;
	}
	return(0);
}

serial_mouse_open(dev)
{
	int unit = minor(dev) & 0x7;
	int mouse_pic = cominfo[unit]->sysdep1;

	spl_t s = splhi();		/* disable interrupts */

	oldvect = ivect[mouse_pic];
	ivect[mouse_pic] = mouseintr;

	oldunit = iunit[mouse_pic];
	iunit[mouse_pic] = unit;

				/* XXX other arrays to init? */
	splx(s);		/* XXX - should come after init? */
}

int mouse_packets = 0;
kd_mouse_open(dev, mouse_pic)
{
	spl_t s = splhi();	/* disable interrupts */
	extern int kdintr();

	oldvect = ivect[mouse_pic];
	ivect[mouse_pic] = kdintr;
	oldspl = intpri[mouse_pic];
	intpri[mouse_pic] = SPL6;
	form_pic_mask();
	splx(s);
}

/*
 * mouseclose - Disable interrupts on the serial port, reset driver flags, 
 * and restore the serial port interrupt vector.
 */
mouseclose(dev, flags)
{
	switch (mouse_type) {
	case MICROSOFT_MOUSE:
	case MICROSOFT_MOUSE7:
	case MOUSE_SYSTEM_MOUSE:
	case LOGITECH_TRACKMAN:
		serial_mouse_close(dev);
		break;
	case IBM_MOUSE:
		ibm_ps2_mouse_close(dev);
		kd_mouse_close(dev, 12);
		{int i = 20000; for (;i--;); }
		kd_mouse_drain();
		break;
	case NO_MOUSE:
		break;
	}

	kdq_reset(&mouse_queue);		/* paranoia */
	mouse_in_use = FALSE;
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	mousepgrp = 0;
	mouseflag = 0;
	mouse_sel = 0;
#endif	MACH_KERNEL
}

/*ARGSUSED*/
serial_mouse_close(dev, flags)
	dev_t dev;
	int flags;
{
	spl_t o_pri = splhi();		/* mutex with open() */
	int unit = minor(dev) & 0x7;
	int mouse_pic = cominfo[unit]->sysdep1;
	caddr_t base_addr  = (caddr_t)cominfo[unit]->address;

	assert(ivect[mouse_pic] == mouseintr);
	outb(base_addr + RIE, 0);	/* disable serial port */
	outb(base_addr + RMC, 0);	/* no rts */
	ivect[mouse_pic] = oldvect;
	iunit[mouse_pic] = oldunit;

	(void)splx(o_pri);
}

kd_mouse_close(dev, mouse_pic)
{
	spl_t s = splhi();

	ivect[mouse_pic] = oldvect;
	intpri[mouse_pic] = oldspl;
	form_pic_mask();
	splx(s);
}

#ifdef	MACH_KERNEL
#else	MACH_KERNEL
/*
 * mouseioctl - handling for asynch & non-blocking I/O.
 */

/*ARGSUSED*/
mouseioctl(dev, cmd, data, flag)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
{
	int s = SPLKD();
	int err = 0;

	switch (cmd) {
	case FIONBIO:
		if (*(int *)data)
			mouseflag |= MOUSE_NBIO;
		else
			mouseflag &= ~MOUSE_NBIO;
		break;
	case FIOASYNC:
		if (*(int *)data)
			mouseflag |= MOUSE_ASYNC;
		else
			mouseflag &= ~MOUSE_ASYNC;
		break;
	default:
		err = ENOTTY;
		break;
	}

	splx(s);
	return(err);
}


/*
 * mouseselect - check for pending events, etc.
 */

/*ARGSUSED*/
mouseselect(dev, rw)
{
	int s = SPLKD();

	if (!kdq_empty(&mouse_queue)) {
		splx(s);
		return(1);
	}

	if (mouse_sel)
		mouseflag |= MOUSE_COLL;
	else
		mouse_sel = (struct proc *)current_thread();
					/* eeeyuck */
	
	splx(s);
	return(0);
}
#endif	MACH_KERNEL

/*
 * mouseread - dequeue and return any queued events.
 */
#ifdef	MACH_KERNEL
boolean_t	mouse_read_done();	/* forward */

mouseread(dev, ior)
	dev_t	dev;
	register io_req_t	ior;
{
	register int	err, count;
	register spl_t	s;

	err = device_read_alloc(ior, (vm_size_t)ior->io_count);
	if (err != KERN_SUCCESS)
	    return (err);

	s = SPLKD();
	if (kdq_empty(&mouse_queue)) {
	    if (ior->io_mode & D_NOWAIT) {
		splx(s);
		return (D_WOULD_BLOCK);
	    }
	    ior->io_done = mouse_read_done;
	    enqueue_tail(&mouse_read_queue, (queue_entry_t)ior);
	    splx(s);
	    return (D_IO_QUEUED);
	}
	count = 0;
	while (!kdq_empty(&mouse_queue) && count < ior->io_count) {
	    register kd_event *ev;

	    ev = kdq_get(&mouse_queue);
	    *(kd_event *)(&ior->io_data[count]) = *ev;
	    count += sizeof(kd_event);
	}
	splx(s);
	ior->io_residual = ior->io_count - count;
	return (D_SUCCESS);
}

boolean_t mouse_read_done(ior)
	register io_req_t	ior;
{
	register int	count;
	register spl_t	s;

	s = SPLKD();
	if (kdq_empty(&mouse_queue)) {
	    ior->io_done = mouse_read_done;
	    enqueue_tail(&mouse_read_queue, (queue_entry_t)ior);
	    splx(s);
	    return (FALSE);
	}

	count = 0;
	while (!kdq_empty(&mouse_queue) && count < ior->io_count) {
	    register kd_event *ev;

	    ev = kdq_get(&mouse_queue);
	    *(kd_event *)(&ior->io_data[count]) = *ev;
	    count += sizeof(kd_event);
	}
	splx(s);

	ior->io_residual = ior->io_count - count;
	ds_read_done(ior);

	return (TRUE);
}

#else	MACH_KERNEL
/*ARGSUSED*/
mouseread(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	int s = SPLKD();
	int err = 0;
	kd_event *ev;
	int i;
	char *cp;

	if (kdq_empty(&mouse_queue))
		if (mouseflag & MOUSE_NBIO) {
			err = EWOULDBLOCK;
			goto done;
		} else 
			while (kdq_empty(&mouse_queue)) {
				splx(s);
				sleep((caddr_t)&mouse_queue, TTIPRI);
				s = SPLKD();
			}

	while (!kdq_empty(&mouse_queue) && uio->uio_resid >= sizeof(kd_event)) {
		ev = kdq_get(&mouse_queue);
		for (cp = (char *)ev, i = 0; i < sizeof(kd_event);
		     ++i, ++cp) {
			err = ureadc(*cp, uio);
			if (err)
				goto done;
		}
	}

done:
	splx(s);
	return(err);
}
#endif	MACH_KERNEL


/*
 * mouseintr - Get a byte and pass it up for handling.  Called at SPLKD.
 */
mouseintr(unit)
{
	caddr_t base_addr  = (caddr_t)cominfo[unit]->address;
	unsigned char id, ls;

	/* get reason for interrupt and line status */
	id = inb(base_addr + RID);
	ls = inb(base_addr + RLS);

	/* handle status changes */
	if (id == IDLS) {
		if (ls & LSDR) {
			inb(base_addr + RDAT);	/* flush bad character */
		}
		return;			/* ignore status change */
	}

	if (id & IDRD) {
		mouse_handle_byte((u_char)(inb(base_addr + RDAT) & 0xff));
	}
}


/*
 * handle_byte - Accumulate bytes until we have an entire packet.
 * If the mouse has moved or any of the buttons have changed state (up
 * or down), enqueue the corresponding events.
 * Called at SPLKD.
 * XXX - magic numbers.
 */
int show_mouse_byte = 0;
/*
   X down; middle down; middle up; X up		50 0 0; 50 0 0 22; 50 0 0 02; 40 0 0 
   X down; middle down; X up; middle up		50 0 0; 50 0 0 22; 40 0 0 22; 40 0 0 2
 *
 * The trick here is that all the while the middle button is down you get 4 byte
 * packets with the last byte 0x22.  When the middle button goes up you get a
 * last packet with 0x02.
 */
int lastgitech = 0x40;		/* figure whether the first 3 bytes imply */
 				/* its time to expect a fourth */
int fourthgitech = 0;		/* look for the 4th byte; we must process it */
int middlegitech = 0;		/* what should the middle button be */

#define MOUSEBUFSIZE	5		/* num bytes def'd by protocol */
static u_char mousebuf[MOUSEBUFSIZE];	/* 5-byte packet from mouse */
static short mbindex = 0;		/* index into mousebuf */

mouse_handle_byte(ch)
	u_char ch;
{
	if (show_mouse_byte) {
		printf("%x(%c) ", ch, ch);
	}
	if (mbindex == 0) {
		switch (mouse_type) {
		case MICROSOFT_MOUSE7:
			if ((ch & 0x40) != 0x40)
				return;
			break;
		case MICROSOFT_MOUSE:
			if ((ch & 0xc0) != 0xc0)
				return;
			break;
		case MOUSE_SYSTEM_MOUSE:
			if ((ch & 0xf8) != 0x80)
				return;
			break;
		case LOGITECH_TRACKMAN:
			if (fourthgitech == 1) {
				fourthgitech = 0;
				if (ch & 0xf0)
					middlegitech = 0x4;
				else
					middlegitech = 0x0;
				mouse_packet_microsoft_mouse(mousebuf);
				return;
			} else if ((ch & 0xc0) != 0x40)
				return;
			break;
		case IBM_MOUSE:
			break;
		}
	}

	mousebuf[mbindex++] = ch;
	if (mbindex < mousebufsize)
		return;
	
	/* got a packet */
	mbindex = 0;

	switch (mouse_type) {
	case MICROSOFT_MOUSE7:
	case MICROSOFT_MOUSE:
		mouse_packet_microsoft_mouse(mousebuf);
		break;
	case MOUSE_SYSTEM_MOUSE:
		mouse_packet_mouse_system_mouse(mousebuf);
		break;
	case LOGITECH_TRACKMAN:
		if ( mousebuf[1] || mousebuf[2] ||
		     mousebuf[0] != lastgitech) {
		     	mouse_packet_microsoft_mouse(mousebuf);
			lastgitech = mousebuf[0] & 0xf0;
		} else {
			fourthgitech = 1;
		}
		break;
	case IBM_MOUSE:
		mouse_packet_ibm_ps2_mouse(mousebuf);
		break;
	}
}

mouse_packet_mouse_system_mouse(mousebuf)
u_char mousebuf[MOUSEBUFSIZE];
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = mousebuf[0] & 0x7;	/* get current state of buttons */
	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = (char)mousebuf[1] + (char)mousebuf[3];
	moved.mm_deltaY = (char)mousebuf[2] + (char)mousebuf[4];

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_RIGHT, buttons & 1);
		if (buttonchanges & 2)
			mouse_button(MOUSE_MIDDLE, (buttons & 2) >> 1);
		if (buttonchanges & 4)
			mouse_button(MOUSE_LEFT, (buttons & 4) >> 2);
	}
}

/* same as above for microsoft mouse */
/*
 * 3 byte microsoft format used
 *
 * 7  6  5  4  3  2  1  0
 * 1  1  L  R  Y7 Y6 X7 X6
 * 1  0  X5 X4 X3 X3 X1 X0
 * 1  0  Y5 Y4 Y3 Y2 Y1 Y0
 *
 */
mouse_packet_microsoft_mouse(mousebuf)
u_char mousebuf[MOUSEBUFSIZE];
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = ((mousebuf[0] & 0x30) >> 4);
	buttons |= middlegitech;
			/* get current state of buttons */
#ifdef	gross_hack
	if (buttons == 0x03)	/* both buttons down */
		buttons = 0x04;
#endif	/* gross_hack */
	buttons = (~buttons) & 0x07;	/* convert to not pressed */

	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = ((mousebuf[0] & 0x03) << 6) | (mousebuf[1] & 0x3F);
	moved.mm_deltaY = ((mousebuf[0] & 0x0c) << 4) | (mousebuf[2] & 0x3F);
	if (moved.mm_deltaX & 0x80)	/* negative, in fact */
		moved.mm_deltaX = moved.mm_deltaX - 0x100;
	if (moved.mm_deltaY & 0x80)	/* negative, in fact */
		moved.mm_deltaY = moved.mm_deltaY - 0x100;
	/* and finally the Y orientation is different for the microsoft mouse */
	moved.mm_deltaY = -moved.mm_deltaY;

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_RIGHT, (buttons & 1) ?
						MOUSE_UP : MOUSE_DOWN);
		if (buttonchanges & 2)
			mouse_button(MOUSE_LEFT, (buttons & 2) ?
						MOUSE_UP : MOUSE_DOWN);
		if (buttonchanges & 4)
			mouse_button(MOUSE_MIDDLE, (buttons & 4) ?
						MOUSE_UP : MOUSE_DOWN);
	}
}

ibm_ps2_mouse_open(dev)
{
	char buf[4] = {0,0,0,0};

	lastbuttons = 0;
	mbindex = 0;

	kd_sendcmd(0xa8);	/* enable mouse in kbd */

	kd_cmdreg_write(0x47);	/* allow mouse interrupts */
				/* magic number for ibm? */

	if (kd_mouse_write(0xff) != 0xfa)	/* reset mouse */
		return;

	kd_mouse_read(2, buf);

	kd_mouse_write(0xea);	/* stream mode */
	kd_mouse_write(0xf4);	/* enable */

}

ibm_ps2_mouse_close(dev)
{
	char buf[4] = {0,0,0,0};

	if (kd_mouse_write(0xff) == 0xfa)	/* reset mouse */
		kd_mouse_read(2, buf);

	kd_sendcmd(0xa7);	/* disable mouse in kbd */
	kd_cmdreg_write(0x65);	/* disallow mouse interrupts */
				/* magic number for ibm? */
}

/*
 * 3 byte ibm ps2 format used
 *
 * 7  6  5  4  3  2  1  0
 * YO XO YS XS 1  M  R  L
 * X7 X6 X5 X4 X3 X3 X1 X0
 * Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
 *
 */
mouse_packet_ibm_ps2_mouse(mousebuf)
u_char mousebuf[MOUSEBUFSIZE];
{
	u_char buttons, buttonchanges;
	struct mouse_motion moved;

	buttons = mousebuf[0] & 0x7;	/* get current state of buttons */
	buttonchanges = buttons ^ lastbuttons;
	moved.mm_deltaX = ((mousebuf[0]&0x10) ? 0xffffff00 : 0 ) | (u_char)mousebuf[1];
	moved.mm_deltaY = ((mousebuf[0]&0x20) ? 0xffffff00 : 0 ) | (u_char)mousebuf[2];
	if (mouse_packets) {
		printf("(%x:%x:%x)", mousebuf[0], mousebuf[1], mousebuf[2]);
		return;
	}

	if (moved.mm_deltaX != 0 || moved.mm_deltaY != 0)
		mouse_moved(moved);

	if (buttonchanges != 0) {
		lastbuttons = buttons;
		if (buttonchanges & 1)
			mouse_button(MOUSE_LEFT,   !(buttons & 1));
		if (buttonchanges & 2)
			mouse_button(MOUSE_RIGHT,  !((buttons & 2) >> 1));
		if (buttonchanges & 4)
			mouse_button(MOUSE_MIDDLE, !((buttons & 4) >> 2));
	}
}

/*
 * Enqueue a mouse-motion event.  Called at SPLKD.
 */
mouse_moved(where)
	struct mouse_motion where;
{
	kd_event ev;

	ev.type = MOUSE_MOTION;
	ev.time = time;
	ev.value.mmotion = where;
	mouse_enqueue(&ev);
}


/*
 * Enqueue an event for mouse button press or release.  Called at SPLKD.
 */
mouse_button(which, direction)
	kev_type which;
	u_char direction;
{
	kd_event ev;

	ev.type = which;
	ev.time = time;
	ev.value.up = (direction == MOUSE_UP) ? TRUE : FALSE;
	mouse_enqueue(&ev);
}


/*
 * mouse_enqueue - enqueue an event and wake up selecting processes, if
 * any.  Called at SPLKD.
 */

void
mouse_enqueue(ev)
	kd_event *ev;
{
	if (kdq_full(&mouse_queue))
		printf("mouse: queue full\n");
	else
		kdq_put(&mouse_queue, ev);

#ifdef	MACH_KERNEL
	{
	    register io_req_t	ior;
	    while ((ior = (io_req_t)dequeue_head(&mouse_read_queue)) != 0)
		iodone(ior);
	}
#else	MACH_KERNEL
	if (mouse_sel) {
		selwakeup(mouse_sel, mouseflag & MOUSE_COLL);
		mouse_sel = 0;
		mouseflag &= ~MOUSE_COLL;
	}
	if (mouseflag & MOUSE_ASYNC)
		gsignal(mousepgrp, SIGIO);
	wakeup((caddr_t)&mouse_queue);
#endif	MACH_KERNEL
}
