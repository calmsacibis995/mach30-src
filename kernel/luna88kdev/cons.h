/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	cons.h,v $
 * Revision 2.3  93/03/09  17:58:56  danner
 * 	cleanup
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:20:45  danner
 * 	Initial Checkin
 * 	[91/06/26  11:42:30  danner]
 * 
 * Revision 2.2  91/04/05  14:07:07  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

#define CONSOUT  1
#define SPLX()	spltty()

#define OFF 0
#define ON 1

#if 0
struct conssw {
    int (*c_open)();
    int (*c_close)();
    int (*c_read)();
    int (*c_write)();
    int (*c_ioctl)();
    short dev;
    char kbopen;
    char slpf;
    short mode;
    short previousdev;
    short previousmode;
};
#else
extern int keyboardopen;
#endif

extern struct tty cons_tty[];
extern struct tty *curtty;
extern int utopen(),utclose(), utread(), utwrite(), ttwrite(), utioctl();
extern int cons_cnt;

/* For console window definition */
#define SETMODE		1
#define RESETMODE 	2
#define TIOCNORMAL 	0
#define TIOCFORCE 	1
#define TIOCRESET 	0
#define TIOCPREVIOUS 	2
