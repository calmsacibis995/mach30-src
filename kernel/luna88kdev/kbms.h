/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON AND OMRON ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND OMRON DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	kbms.h,v $
 * Revision 2.4  93/03/09  17:59:43  danner
 * 	cleanup
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:39:34  danner
 * 	Dropped in Omron changes (added _IO('K') defines)
 * 	[92/12/08            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:20:59  danner
 * 	Initial Checkin
 * 	[91/06/26  11:42:36  danner]
 * 
 * Revision 2.2  91/04/05  14:08:40  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/*
 *  Keyboard and Mouse information
 */
#include <sys/ioctl.h>

/*
 * minor device numbers.
 */
#define TERM	2	/* ttya */
#define KBD	1
#define CONSOLE	0

/* keyboard encoding options */
#define	ENCODED		0
#define	SEMIENCODED	1
#define	KB_RAW		2


/*********************************************************************************************/
#define	MOUSE	0


#define	IOCTYPE		0xff00	

#define	KBBUZZER	_IO  ('K', 1)
#define	KBSETM		_IO  ('K', 2)
#define	KBRSTM		_IO  ('K', 3)
#define	KBREPTIM	_IOW ('K', 4, struct kbrepeat)
#define	KBFLSH		_IO  ('K', 5)
#define	KBSETKTM	_IOW ('K', 6, struct keyescmode)
#define	KBEGETT		_IOWR('K', 7, struct kbesget)
#define	KBEGETTA	_IOW ('K', 8, struct kbesgeta)
#define	KBESETT		_IOW ('K', 9, struct kbesget)
#define	KBESETTA	_IOW ('K', 10, struct kbesgeta)
#define	KBGETESC	_IOW ('K', 11, struct escentry)
#define	KBSETESC	_IOW ('K', 12, struct escentry)
#define	KBSGETT		_IOWR('K', 13, struct kbssget)
#define	KBSGETTA	_IOW ('K', 14, struct kbssgeta)
#define	KBSSETT		_IOW ('K', 15, struct kbesget)
#define	KBSSETTA	_IOW ('K', 16, struct kbssgeta)
#define	KBRSTENABLE	_IO  ('K', 17)
#define KBCLICK         _IO  ('K', 18)
#define KBBELL          _IO  ('K', 19)
#define KBCHMAP		_IOW ('K', 20, char *)
#define KBGETMAP	_IOW ('K', 21, char *)
#define KBSETKME	_IOW ('K', 22, struct km_ent)
#define KBGETKME	_IOW ('K', 23, struct km_ent)
#define KBSETTBL	_IOW ('K', 24, short *[8])
#define KBEXCHBS	_IO  ('K', 25)
#define	RSTDISABLE		0
#define	RSTENABLE		1
#define	DISABLE			0
#define	ENABLE			1
#define K_TIME40	0x00    /* buzzer time  40msec */
#define K_TIME150	0x08    /* buzzer time 150msec */
#define K_TIME400	0x10    /* buzzer time 400msec */
#define K_TIME700	0x18    /* buzzer time 700msec */
#define K_HZ6000	0x00    /* buzzer 6000Hz */
#define K_HZ3000	0x01    /* buzzer 3000Hz */
#define K_HZ1500	0x02    /* buzzer 1500Hz */
#define K_HZ1000	0x03    /* buzzer 1000Hz */
#define K_HZ600		0x04    /* buzzer  600Hz */
#define K_HZ300		0x05    /* buzzer  300Hz */
#define K_HZ150		0x06    /* buzzer  150Hz */
#define K_HZ100		0x07    /* buzzer  100Hz */

struct km_ent {
    short ent;
    char  value;
};

struct kbentry {
    char	flags;
    unsigned char	code;
};

struct kbesget	{
    short	tblno;
    short	keycode;
    struct kbentry	kbentry;
};

struct kbesgeta {
    short	tblno;
    struct kbentry	*kbtblp;
};	

struct kbssget	{
    short	keycode;
    char	kbsrept;
};

struct kbssgeta {
    char	*kbsreptp;
};	

struct keyescmode {
    short	keytype;
    unsigned short	escmode;
};

struct escentry {
    unsigned short	keycode;
    unsigned short	escmode;
    char	*esctext;
};

struct kbrepeat {
    unsigned short	repstart;
    unsigned short	repinterval;
};

#define	MSDGET		_IOWR('M', 1, struct msdget)
#define	MSDGETA		_IOWR('M', 2, struct msdgeta)
#define	MSSMPLCLK	_IO('M', 3)
#define	MSSETMICK	_IO('M', 4)
#define	MSSETLIMT	_IOW('M', 5, struct mssetlimt)
#define	MSSETCRNT	_IOW('M', 6, struct mssetcrnt)
#define	MSSETMODE	_IO('M', 7)
#define	MSSETCURS	_IO('M', 8)
#define	MSCURSCLK	_IO('M', 9)
#define	MSGETCRFNT	_IOWR('M', 10, struct mssgetcrfnt)
#define	MSSETCRFNT	_IOWR('M', 11, struct mssgetcrfnt)
#define	MSDFLUSH	_IO('M', 12)
#define FIONBIO		_IOW('f',126, int)  /* in ioctl.h in 2.5 */
#define TBLSPCL		_IO('K', 31)
#define TBLCTRL		_IO('K', 32)
#define TBLKNSF		_IO('K', 33)
#define TBLKANA		_IO('K', 34)
#define TBLCPSF		_IO('K', 35)
#define TBLCAPS		_IO('K', 36)
#define TBLSIFT		_IO('K', 37)
#define TBLNOML		_IO('K', 38)
#define TBLESCP		_IO('K', 39)
#define KEYMAP		_IO('K', 40)
#define TBLMODE		_IO('K', 41)
#define T_KEYMAP	_IO('K', 42)
#define FLGMODE		_IO('K', 43)

/*
 * THE FOLLOWING FOR UX ONLY... should get out of here for 2.5...
 */

#define	COORDEVNT	0	/* this now only for UX.. must fix */
#define	BUTONEVNT	1	/* this now only for UX.. must fix */

struct msdata	{
    unsigned short	time;
    short	type;
    union {
	struct {
	    short	X_coord;
	    short	Y_coord;
	    short	X_speed;
	    short	Y_speed;
	} coordinate;
	struct {
	    unsigned char  L_button;
	    unsigned char  M_button;
	    unsigned char  R_button;
	} button;
    } event;
};

struct msdget	{
    short	mode;
    short	retval;
    struct msdata	msdata;
};

struct msdgeta	{
    short	mode;
    short	retval;
    short	count;
    struct msdata	*msdatap;
};

struct mssetlimt	{
    short	mode;
    short	X_scoord;
    short	Y_scoord;
    short	X_ecoord;
    short	Y_ecoord;
};

struct mssetcrnt	{
    short	X_coord;
    short	Y_coord;
};

struct mssgetcrfnt	{
    short	font;
    char	*fontp;
};
