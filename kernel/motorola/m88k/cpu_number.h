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
 * $Log:	cpu_number.h,v $
 * Revision 2.3  93/01/26  18:01:37  danner
 * 	Converted macro to static inline.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.2  92/02/18  18:03:15  elf
 * 	Created.
 * 	[92/01/21            danner]
 * 
 */

#ifndef	_M88K_CPU_NUMBER_
#define _M88K_CPU_NUMBER_
 
#ifdef	KERNEL
#include <cpus.h>
#include <platforms.h>

#if NCPUS == 1
 #define cpu_number()	     (0)
 #define set_cpu_number(cpu) /* nothing */
#else
 static inline unsigned cpu_number(void)
 {
	unsigned cpu;
	asm("ldcr %0, cr18  \n"
	    "clr  %0, %0, 0<4>" : "=r" (cpu));
	return cpu;
 }
#endif

#endif /* KERNEL */
#endif /* _M88K_CPU_NUMBER_ */
