/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log:	ast_check.c,v $
 * Revision 2.3  91/05/14  16:03:21  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:30:25  dbg
 * 	Copied for i386.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.2  90/08/27  22:13:09  dbg
 * 	Created.
 * 	[90/07/18            dbg]
 * 
 */
#include <cpus.h>

#if	NCPUS > 1

/*
 * Handle signalling ASTs on other processors.
 *
 * Initial i386 implementation does nothing.
 */

#include <kern/processor.h>

/*
 * Initialize for remote invocation of ast_check.
 */
init_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif lint
}

/*
 * Cause remote invocation of ast_check.  Caller is at splsched().
 */
cause_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif lint
}

#endif	/* NCPUS > 1 */
