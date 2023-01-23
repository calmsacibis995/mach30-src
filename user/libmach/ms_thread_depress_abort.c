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
 * $Log:	ms_thread_depress_abort.c,v $
 * Revision 2.2  92/02/19  16:07:40  elf
 * 	Created.
 * 	[92/01/20            rwd]
 * 
 */

#include <mach.h>
#include <mach/message.h>

kern_return_t thread_depress_abort(target_thread)
	thread_t target_thread;
{
	kern_return_t result;

	result = syscall_thread_depress_abort(target_thread);
	if (result == MACH_SEND_INTERRUPTED)
		result = mig_thread_depress_abort(target_thread);
	return(result);
}
