/* 
 * Mach Operating System
 * Copyright (c) 1993,1992,1991 Carnegie Mellon University
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
 * $Log:	thread.c,v $
 * Revision 2.5  93/05/14  15:10:27  rvb
 * 	#include string.h->strings.h; kill stdlib.h
 * 	[93/05/14            rvb]
 * 
 * Revision 2.4  93/02/02  21:55:11  mrt
 * 	Changed include of mach/mach.h to mach.h
 * 	[93/02/02            mrt]
 * 
 * Revision 2.3  93/01/14  18:05:28  danner
 * 	ansification updates.
 * 	[93/01/10            danner]
 * 
 * Revision 2.2  92/08/03  18:04:19  jfriedl
 * 	brought from luna88k versions [danner]
 * 	Created by rwd
 * 	[91/08/19  13:49:41  danner]
 * 
 * Revision 2.5  92/05/04  11:28:58  danner
 * 	Updated Copyright.
 * 	Set valid bit in sfip.
 * 	[92/04/12            danner]
 * 
 * Revision 2.4  92/04/01  10:56:42  rpd
 * 	Moved broken  cthread_sp to csw.s where it can be written
 * 	 correctly in godfearing assembler.
 * 	[92/03/20            danner]
 * 
 * Revision 2.3  92/02/19  14:15:05  elf
 * 	Updated to reflect thread structure.
 * 	[92/02/18  16:41:00  danner]
 * 	Updated setregs to reflect new thread structure.
 * 	[91/09/19  16:58:25  danner]
 * 
 * 	Updated for new thread structure.
 * 	[91/08/25  16:39:33  danner]
 * 
 * Revision 2.2  91/08/24  12:35:41  af
 * 	Created.
 * 	[91/07/19  18:45:28  danner]
 * 
 */
/*
 * luna/thread.c
 *
 * Cproc startup for M88K CTHREAD implementation.
 */

#include <cthreads.h>
#include "cthread_internals.h"
#include <mach.h>
#include <strings.h>

/*
 * Set up the initial state of a MACH thread
 * so that it will invoke cthread_body(child)
 * when it is resumed.
 */

/*
 * Set up the initial state of a MACH thread
 * so that it will invoke cthread_body(child)
 * when it is resumed.
 */
void
cproc_setup(register cproc_t child, thread_t thread, void (*routine)(cproc_t))
{
	register int top = child->stack_base + child->stack_size;
	struct luna88k_thread_state state;
	register struct luna88k_thread_state *ts = &state;
	kern_return_t r;

	/*
	 * Set up M88000 call frame and registers.
	 * See M88100 Manual, Object Compatibility Standard/88open,
	 * GNU C compiler documentation, GreenHills C compiler documentation,
	 * etc.
	 */
	memset(ts, 0, sizeof(struct luna88k_thread_state));
	/*
	 * Set pc(fetch instruction pointer) to location of procedure entry.
	 * Inner cast needed since too many C compilers choke on the type void (*)().
	 */
	ts->sxip = 0;
	ts->snip = 0;
	ts->sfip = ((int) routine) | 2 /* valid bit */;
	ts->r[2] = (int) child;	/* argument to function */
	ts->fpcr = 0x1f;	/* matched with locore.s, signal is default */
	ts->r[30] = 0; /* frame pointer, points 0 */
	ts->r[31] = top - 2*sizeof(int); /* stack pointer, points arg area */

	MACH_CALL(thread_set_state(thread, \
				   LUNA88K_THREAD_STATE, \
				   (thread_state_t) &state, \
				   LUNA88K_THREAD_STATE_COUNT),
		  r);
}
