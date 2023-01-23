/* 
 * Mach Operating System
 * Copyright (c) 1993,1992, 1991 Carnegie Mellon University
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
 * $Log:	cthreads.h,v $
 * Revision 2.3  93/01/14  18:05:25  danner
 * 	asm -> __asm__
 * 	[93/01/10            danner]
 * 
 * Revision 2.2  92/08/03  18:04:10  jfriedl
 * 	brought from luna88k versions [danner]
 * 	Redid cthread_sp() for gcc2 [jfriedl]
 * 
 * Revision 2.1.2.1  92/07/18  00:50:16  jfriedl
 * 
 * Revision 2.3  92/05/04  11:28:55  danner
 * 	Added inline cthread_sp()
 * 	[92/04/29            danner]
 * 
 * Revision 2.2  90/11/05  14:38:00  rpd
 * 	Created.
 * 	[90/11/01            rwd]
 * 
 */

#ifndef _MACHINE_CTHREADS_H_
#define _MACHINE_CTHREADS_H_

typedef volatile int spin_lock_t;
#define SPIN_LOCK_INITIALIZER 0
#define spin_lock_init(s) (*(s)=0)
#define spin_lock_locked(s) (*(s) != 0)

#ifdef __GNUC__
#define cthread_sp() ({register int r31 __asm__("r31"); r31;})
#endif /* __GNU__ */

#endif _MACHINE_CTHREADS_H_
