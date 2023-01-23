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
 * $Log:	exvec.h,v $
 * Revision 2.3  93/01/26  18:00:47  danner
 * 	Removed cruft.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/02/18  18:03:33  elf
 * 	[92/01/22  19:01:26  danner]
 * 
 */
/*
 * Architecturally and vendor independent exception vector numbers
 */

#ifndef _M88100_EXVEC_
#define _M88100_EXVEC_

/*
 * exception vector number
 */
#define EVN_RESET	0	/* reset exception */
#define EVN_INT		1	/* intrrupt exception */
#define EVN_INS		2	/* instruction access exception */
#define EVN_DATA	3	/* data access exception */
#define ENV_MISALIGN	4	/* misaligned access exception */
#define EVN_UNIMP	5	/* unimplemented opcode exception */
#define EVN_PRIV	6	/* privilege violation exception */
#define EVN_BOUND	7	/* bounds check violation exception */
#define EVN_DIVIDE	8	/* illegal integer divide exception */
#define EVN_OVERFLOW	9	/* integer overflow exception */
#define EVN_ERROR	10	/* error exception */
#define	EVN_BREAK	64	/* break point (luna specific) */
#define	EVN_TRACE	65	/* trace (luna specific) */
#define	EVN_KDB		66	/* enter to KDB (luna specific) */
#define	EVN_UNKNOWN	67	/* undefined exception (luna specific) */
#define EVN_FLOAT_P	114	/* floating-point precise exception */
#define EVN_FLOAT_I	115	/* floating-point imprecise exception */
#define EVN_USER	128	/* user defined exception */
#define EVN_MAX		512	/* maximum exception number <= 512 */

#endif  _M88100_EXVEC_
