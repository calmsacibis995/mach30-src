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
 * $Log:	exception.h,v $
 * Revision 2.2  92/08/03  17:51:15  jfriedl
 * 	added EXC_M88K_UNKNOWN
 * 	[92/07/30            jfriedl]
 * 
 * 	Created from luna88k version.
 * 	[92/07/24            jfriedl]
 * 
 */

#ifndef	_MACHINE_EXCEPTION_
#define _MACHINE_EXCEPTION_

/*
 *	Bad Access
 */
#define EXC_M88K_SEGV_CODE		1
#define EXC_M88K_SEGV_DATA		2
#define EXC_M88K_MISALIGN		3
/* should not conflict with kernel return code */

/*
 *	Bad Instruction
 */
#define EXC_M88K_UNIMP			1
#define EXC_M88K_PRIV			2


/*
 *	Arithmetic
 */
#define EXC_M88K_DIVIDE			1
#define EXC_M88K_OVERFLOW		2
#define EXC_M88K_FLOAT_P		3
#define EXC_M88K_FLOAT_I		4

/*
 *	EXC_ARITHMETIC 
 */

#define EXC_M88K_FPE_INTOVF		1
#define EXC_M88K_FPE_INTDIV		2
#define EXC_M88K_FPE_BNDCHK		3
#define EXC_M88K_FLTINEX		4
#define EXC_M88K_FLTOVF			5
#define EXC_M88K_FLTUND			6
#define EXC_M88K_FLTDIV			7
#define EXC_M88K_FLTOPERR		8
#define EXC_M88K_FPE_PRIVVIO		9
#define EXC_M88K_FPE_UNIMPL		10
#define EXC_M88K_FLTNAN			11

/*
 *	EXC_BREAKPOINT
 */

#define EXC_M88K_BPT			1
#define EXC_M88K_TRACE			2

/*
 *	Software
 */
#define EXC_M88K_BOUND			1
#define EXC_M88K_RESTRAP		2
#define EXC_M88K_UNKNOWN		3

#endif	_MACHINE_EXCEPTION_
