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
 * $Log:	machine_routines.h,v $
 * Revision 2.3  92/01/03  20:08:11  dbg
 * 	Define MACHINE_SERVER_ROUTINE.
 * 	[91/11/25            dbg]
 * 
 * Revision 2.2  91/08/01  14:34:43  dbg
 * 	Created.
 * 	[91/08/01            dbg]
 * 
 */

#ifndef	_I386_MACHINE_ROUTINES_H_
#define	_I386_MACHINE_ROUTINES_H_

/*
 * The i386 has a set of machine-dependent interfaces.
 */
#define	MACHINE_SERVER		mach_i386_server
#define	MACHINE_SERVER_ROUTINE	mach_i386_server_routine

#endif

