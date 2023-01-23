/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * $Log:	vm_user.h,v $
 * Revision 2.3  91/05/14  17:51:44  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  18:00:43  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:35:10  mrt]
 * 
 * Revision 2.1  89/08/03  16:46:17  rwd
 * Created.
 * 
 * Revision 2.5  89/04/18  21:31:31  mwyoung
 * 	Reset history.
 * 
 */
/*
 *	File:	vm/vm_user.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1986
 *
 *	Declarations of user-visible virtual address space
 *	management functionality.
 */

#ifndef	_VM_VM_USER_H_
#define _VM_VM_USER_H_

#include <mach/kern_return.h>

extern kern_return_t	vm_allocate();
extern kern_return_t	vm_deallocate();
extern kern_return_t	vm_inherit();
extern kern_return_t	vm_protect();
extern kern_return_t	vm_statistics();
extern kern_return_t	vm_read();
extern kern_return_t	vm_write();
extern kern_return_t	vm_copy();
extern kern_return_t	vm_map();

#endif	_VM_VM_USER_H_
