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
 * $Log:	csw.s,v $
 * Revision 2.9  93/08/02  21:45:02  mrt
 * 	Fixed comment in cproc_switch.
 * 	Bug reported by Ian_Service of stratus.com
 * 	[93/06/23            mrt]
 * 
 * Revision 2.8  93/05/10  17:51:35  rvb
 * 	Use C Comment
 * 	[93/05/04  18:13:59  rvb]
 * 
 * Revision 2.7  91/07/31  18:36:32  dbg
 * 	Fix for ANSI C preprocessor.
 * 	[91/07/30  17:35:16  dbg]
 * 
 * Revision 2.6  91/05/14  17:56:56  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/08  13:35:49  dbg
 * 	Unlock lock with a locked instruction (xchg).
 * 	[91/03/20            dbg]
 * 
 * Revision 2.4  91/02/14  14:20:02  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/13  12:15:27  mrt]
 * 
 * Revision 2.3  91/01/08  16:46:20  rpd
 * 	Don't use Times - horta doesn't like it for some reason.
 * 	[91/01/06            rpd]
 * 
 * Revision 2.2  90/05/03  15:54:37  dbg
 * 	Created.
 * 	[90/02/05            dbg]
 * 
 */
#include <i386/asm.h>

/*
 * Suspend the current thread and resume the next one.
 *
 * void cproc_switch(int *cur, int *next, int *lock)
 */
ENTRY(cproc_switch)
	pushl	%ebp		/* save ebp */
	movl	%esp,%ebp	/* set frame pointer to get arguments */
	pushl	%ebx		/* save ebx */
	pushl	%esi		/*      esi */
	pushl	%edi		/*      edi */
	movl	B_ARG0,%eax	/* get cur */
	movl	%esp,(%eax)	/* save current esp */
	movl	B_ARG2,%edx	/* get address of lock before switching */
				/* stacks */
	movl	B_ARG1,%eax	/* get next */
	movl	(%eax),%esp	/* get new stack pointer */
	xorl	%eax,%eax	/* unlock */
	xchgl	%eax,(%edx)	/* the lock - now old thread can run */
	
	popl	%edi		/* restore di */
	popl	%esi		/*	  si */
	popl	%ebx		/*	  bx */
	popl	%ebp		/*     and bp (don`t use "leave" - bp */
				/*	      still points to old stack) */
	ret

/*
 * Create a new stack frame for a 'waiting' thread,
 * save current thread's frame, and switch to waiting thread.
 *
 * void cproc_start_wait(int *cur,
 *			 cproc_t child,
 *			 int stackp,
 *			 int *lock)
 */
ENTRY(cproc_start_wait)
	pushl	%ebp		/* save ebp */
	movl	%esp,%ebp	/* set frame pointer */
	pushl	%ebx		/* save ebx */
	pushl	%esi		/*      esi */
	pushl	%edi		/*      edi */
	movl	B_ARG0,%eax	/* get cur */
	movl	%esp,(%eax)	/* save current esp */
	movl	B_ARG1,%eax	/* get child thread */
	movl	B_ARG3,%edx	/* point to lock before switching stack */
	movl	B_ARG2,%esp	/* get new stack */
	pushl	%eax		/* push child thread as argument */
	movl	$0,%ebp		/* (clear frame pointer) */
	xorl	%eax,%eax	/* unlock */
	xchgl	%eax,(%edx)	/* the lock - now old thread can run */
	call	_cproc_waiting	/* call cproc_waiting */
	/*NOTREACHED*/

/*
 * Set up a thread's stack so that when cproc_switch switches to
 * it, it will start up as if it called
 * cproc_body(child)
 *
 * void cproc_prepare(cproc_t child, int *context, int stack)
 */
ENTRY(cproc_prepare)
	pushl	%ebp		/* save ebp */
	movl	%esp,%ebp	/* set frame pointer */
	movl	B_ARG2,%edx	/* get child`s stack */
	subl	$28,%edx
				/* make room for context: */
				/* 0   saved edi () */
				/* 4   saved esi () */
				/* 8   saved ebx () */
				/* 12  saved ebp () */
				/* 16  return PC from cproc_switch */
				/* 20  return PC from cthread_body */
				/* 24  argument to cthread_body */
	movl	$0,12(%edx)	/* clear frame pointer */
	movl	$_cthread_body,16(%edx)
				/* resume at cthread_body */
	movl	$0,20(%edx)	/* fake return address from cthread_body */
	movl	B_ARG0,%ecx	/* get child thread pointer */
	movl	%ecx,24(%edx)	/* set as argument to cthread_body */
	movl	B_ARG1,%ecx	/* get pointer to context */
	movl	%edx,(%ecx)	/* save context */
	leave
	ret

