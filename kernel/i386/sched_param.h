/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	sched_param.h,v $
 * Revision 2.2  91/07/31  17:40:21  dbg
 * 	Created.
 * 	[91/07/30  16:56:40  dbg]
 * 
 */

/*
 *	Scheduler parameters.
 */

#ifndef	_I386_SCHED_PARAM_H_
#define	_I386_SCHED_PARAM_H_

/*
 *	Sequent requires a right shift of 18 bits to convert
 *	microseconds to priorities.
 */

#define	PRI_SHIFT	18

#endif	_I386_SCHED_PARAM_H_
