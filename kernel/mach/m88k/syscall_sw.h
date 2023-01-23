/*
 * Mach Operating System
 * Copyright (c) 1992,1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	syscall_sw.h,v $
 * Revision 2.3  93/01/14  17:43:20  danner
 * 	deal with ansi preprocessing.
 * 	[92/12/17            danner]
 * 
 * Revision 2.2  92/08/03  17:51:24  jfriedl
 * 	Created from luna88k version.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.3  92/05/04  11:27:44  danner
 * 	Unixoid traps (htg_syscall, task_by_pid) need different syscall stubs 
 * 	than mach kernel traps....
 * 	[92/04/14  14:02:06  danner]
 * 
 * Revision 2.2  91/07/09  23:21:42  danner
 * 	Created
 * 	[91/07/08  22:39:56  danner]
 * 
 *
 */

/*
 * The unixoid traps need different preambles than the normal ones.
 * On syscall stubs that are unixoid UNIXOID trap will be defined.
 */

#ifndef	_MACHINE_SYSCALL_SW_
#define _MACHINE_SYSCALL_SW_

/*
 * This really shouldn't be in the kernel...
 */

#ifdef luna88k
#define GLOBAL global
#define ALIGN  align
#else
#define GLOBAL .global
#define ALIGN  .align
#endif

#ifdef __STDC__
#define _U(n) _ ## n
#else
#define _U(n) _/**/n
#endif

#ifdef UNIXOID_TRAP 
#define kernel_trap(trap_name, trap_number, number_args) \
	GLOBAL  cerror\\ \
        \\ \
	GLOBAL	_U(trap_name)\\ \
	ALIGN	4\\ \
_U(trap_name:) \
	or.u	r9,r0,hi16(trap_number)\\ \
	or	r9,r9,lo16(trap_number)\\ \
	tb0	0,r0,129\\ \
	br	cerror\\ \
	jmp	r1
#else
#define kernel_trap(trap_name, trap_number, number_args) \
	GLOBAL	_U(trap_name)\\ \
	ALIGN	4\\ \
_U(trap_name:) \
	or.u	r9,r0,hi16(trap_number)\\ \
	or	r9,r9,lo16(trap_number)\\ \
	tb0	0,r0,129\\ \
	jmp	r1
#endif

#endif	_MACHINE_SYSCALL_SW_


