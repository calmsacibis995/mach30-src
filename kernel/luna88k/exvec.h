/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 * Revision 2.2  92/02/18  18:01:24  elf
 * 	Created.
 * 	[92/01/22            danner]
 * 
 */
/*
 * Exception vectors
 */


#ifndef _LUNA_EXVEC_
#define _LUNA_EXVEC_

#include <motorola/m88k/m88100/exvec.h>

#define EVN_ROM_BREAK	68	/* ROM break point */
#define EVN_ROM_TRACE	69	/* ROM trace */
#define EVN_ROM		70	/* ROM enter at initialization */
#define EVN_ROM_ABORT	71      /* ROM enter on ABORT interrupt */
#define EVN_SYSCALL	128	/* BSD system call */
#define EVN_MACH	129	/* MACH system call */


#endif  _LUNA_EXVEC_
