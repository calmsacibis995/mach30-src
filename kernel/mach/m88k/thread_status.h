/* 
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 * $Log:	thread_status.h,v $
 * Revision 2.2  92/08/03  17:51:29  jfriedl
 * 	Created from luna88k version.
 * 	[92/07/24            jfriedl]
 * 
 *
 */
/*
 *	This file contains the structure definitions for the thread
 *	state as applied to the M88100 processor.
 */


#ifndef	_MACHINE_THREAD_STATE_
#define	_MACHINE_THREAD_STATE_

#include <motorola/m88k/m88100/thread_status.h>

/*
 *	luna88k_thread_state	basic machine state
 *				
 *      luna88k_exc_state       exception state
 *
 */

#define LUNA88K_THREAD_STATE	M88100_THREAD_STATE
#define LUNA88K_EXC_STATE       M88100_EXC_STATE

#define luna88k_thread_state m88100_thread_state
#define luna88k_exc_state    m88100_exc_state

#define	LUNA88K_THREAD_STATE_COUNT M88100_THREAD_STATE_COUNT
#define	LUNA88K_EXC_STATE_COUNT M88100_EXC_STATE_COUNT

#endif _MACHINE_THREAD_STATE_
