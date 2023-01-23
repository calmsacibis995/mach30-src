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
 * $Log:	xpr.h,v $
 * Revision 2.4  92/05/21  17:17:17  jfriedl
 * 	Added 'extern void xpr()', etc.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.3  91/05/14  16:50:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  17:31:18  mrt
 * 	MACH_KERNEL: removed conditionals.
 * 	[88/12/19            dbg]
 * 
 * Revision 2.1  89/08/03  15:57:39  rwd
 * Created.
 * 
 * Revision 2.5  88/12/19  02:51:59  mwyoung
 * 	Added VM system tags.
 * 	[88/11/22            mwyoung]
 * 
 * Revision 2.4  88/08/24  02:55:54  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:29:56  mwyoung]
 * 
 *
 *  9-Apr-88  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Added flags for TCP and MACH_NP debugging.
 *
 *  6-Jan-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Make the event structure smaller to make it easier to read from
 *	kernel debuggers.
 *
 * 16-Mar-87  Mike Accetta (mja) at Carnegie-Mellon University
 *	MACH:  made XPR_DEBUG definition conditional on MACH
 *	since the routines invoked under it won't link without MACH.
 *	[ V5.1(F7) ]
 */
/*
 * Include file for xpr circular buffer silent tracing.  
 *
 */
/*
 * If the kernel flag XPRDEBUG is set, the XPR macro is enabled.  The 
 * macro should be invoked something like the following:
 *	XPR(XPR_SYSCALLS, ("syscall: %d, 0x%x\n", syscallno, arg1);
 * which will expand into the following code:
 *	if (xprflags & XPR_SYSCALLS)
 *		xpr("syscall: %d, 0x%x\n", syscallno, arg1);
 * Xpr will log the pointer to the printf string and up to 6 arguements,
 * along with a timestamp and cpuinfo (for multi-processor systems), into
 * a circular buffer.  The actual printf processing is delayed until after
 * the buffer has been collected.  It is assumed that the text/data segments
 * of the kernel can easily be reconstructed in a post-processor which
 * performs the printf processing.
 *
 * If the XPRDEBUG compilation switch is not set, the XPR macro expands 
 * to nothing.
 */

#ifndef	_KERN_XPR_H_
#define _KERN_XPR_H_

#ifdef	KERNEL
#include <xpr_debug.h>
#else	KERNEL
#include <sys/features.h>
#endif	KERNEL

#include <machine/xpr.h>

#if	XPR_DEBUG

#define XPR(flags,xprargs) if(xprflags&flags) xpr xprargs

extern int xprflags;
/*
 * flags for message types.
 */
#define XPR_SYSCALLS	0x00000001
#define XPR_TRAPS	0x00000002
#define XPR_SCHED	0x00000004
#define XPR_NPTCP	0x00000008
#define XPR_NP		0x00000010
#define XPR_TCP		0x00000020

#define	XPR_VM_OBJECT		(1 << 8)
#define	XPR_VM_OBJECT_CACHE	(1 << 9)
#define	XPR_VM_PAGE		(1 << 10)
#define	XPR_VM_PAGEOUT		(1 << 11)
#define	XPR_MEMORY_OBJECT	(1 << 12)
#define	XPR_VM_FAULT		(1 << 13)
#define	XPR_INODE_PAGER		(1 << 14)
#define	XPR_INODE_PAGER_DATA	(1 << 15)

#else	XPR_DEBUG
#define XPR(flags,xprargs)
#endif	XPR_DEBUG

struct xprbuf {
	char 	*msg;
	int	arg1,arg2,arg3,arg4,arg5;
	int	timestamp;
	int	cpuinfo;
};

#ifndef WANT_PROTOTYPES
extern void xpr();
#endif
extern void xpr_dump();
extern void xprinit();
extern void xprbootstrap();

#endif _KERN_XPR_H_
