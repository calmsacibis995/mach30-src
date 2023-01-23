/*
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 *
 * HISTORY
 * $Log:	thread.h,v $
 * Revision 2.2  92/08/03  17:49:33  jfriedl
 * 	Created m88k copy from luna [danner].
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.5  92/02/19  13:57:52  elf
 * 	Made all fields unsigned - got bitten in the debugger.
 * 	[91/08/27            danner]
 * 
 * Revision 2.4  91/08/24  12:16:36  af
 * 	Added scratch1, pad fields.
 * 	[91/08/07            danner]
 * 	Reorganized luna88k_saved_state structure to match up nicely with
 * 	 the thread_status structure. Removed the pid field and moved the
 * 	 fpsr and fpcr registers to be right after the general user
 * 	 registers. Moved the vector field to follow the fpit field. Moved
 * 	      the ssbr register to follow the sfip.
 * 	[91/08/07            danner]
 * 
 * Revision 2.3.3.1  91/08/19  13:47:47  danner
 * 	Added scratch1, pad fields.
 * 	[91/08/07            danner]
 * 	Reorganized luna88k_saved_state structure to match up nicely with
 * 	 the thread_status structure. Removed the pid field and moved the
 * 	 fpsr and fpcr registers to be right after the general user
 * 	 registers. Moved the vector field to follow the fpit field. Moved
 * 	      the ssbr register to follow the sfip.
 * 	[91/08/07            danner]
 * 
 * Revision 2.3  91/07/11  11:01:03  danner
 * 	Copyright Fixes
 * 
 * Revision 2.2  91/07/09  23:19:55  danner
 * 	Keep_Stacks currently defined.
 * 	[91/07/08            danner]
 * 	Added include of mach/kern_return.h
 * 	[91/05/06            danner]
 * 
 *
 */
#ifndef	_LUNA_THREAD
#define	_LUNA_THREAD

#include <motorola/m88k/m88100/pcb.h>

#define	CURRENT_THREAD
#define current_thread()					\
	({							\
		thread_t __foo_;				\
		asm("ldcr %0, cr17" : "=r" (__foo_));		\
		__foo_;						\
	})

#endif	_LUNA_THREAD
