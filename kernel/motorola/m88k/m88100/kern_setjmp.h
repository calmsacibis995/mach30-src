/* 
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 * $Log:	kern_setjmp.h,v $
 * Revision 2.3  93/01/26  18:01:21  danner
 * 	tidy.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:52:03  jfriedl
 * 	created [danner]
 * 
 */

#ifndef _M88100_SETJMP_H_
#define _M88100_SETJMP_H_

/* 
 * This needs to be kept in sync with the kernel setjmp/longjmp.  The
 * 88100 reference indicates that registers r2 - r9 are for called
 * procedure parameters and r10 - r13 are scratch registers, so they
 * don't/shouldn't be saved.  All other gpr's should be. This leaves 19
 * registers (r0, the constant zero register doesn't need to be saved) to
 * be stored. In the kernel, we don't worry about fpcr62/fpcr63.
 */

typedef struct jmp_buf 
{
  int  jmp_buf[19];    
} jmp_buf_t;

#endif  _M88100_SETJMP_H_
