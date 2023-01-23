/* 
 * Mach Operating System
 * Copyright (c) 1992,1991 Carnegie Mellon University
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
 * $Log:	mig_deallocate.c,v $
 * Revision 2.3  93/01/14  18:03:34  danner
 * 	Converted file to ANSI C.
 * 	[92/12/11            pds]
 * 
 * Revision 2.2  92/01/03  20:35:46  dbg
 * 	Created.
 * 	[91/09/21            dbg]
 * 
 */

/*
 * Memory deallocation routine for MiG interfaces.
 */
#include <mach.h>
#include <mach/mig_support.h>

void
mig_deallocate(vm_address_t addr, vm_size_t size)
{
	(void) vm_deallocate(mach_task_self(), addr, size);
}
