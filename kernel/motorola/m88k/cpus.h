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
 *
 * HISTORY
 * $Log:	cpus.h,v $
 * Revision 2.3  93/01/26  18:01:30  danner
 * 	int -> unsigned
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.2  92/02/18  18:03:18  elf
 * 	Created.
 * 	[92/01/22  19:04:05  danner]
 * 
 */
/* 
  Versions Idents for 88k family chips 
 */

#ifndef _M88K_CPUS_
#define _M88K_CPUS_

#include <sys/types.h>

/*
 * cpu Processor Identification Register (PID).
 */
#ifndef ASSEMBLER
union cpupid {
    unsigned cpupid;
    struct {
	unsigned
	    /*empty*/:16,
	    arc:8,
	    version:7,
	    master:1;
    } m88100;
    struct {
	unsigned
	    id:8,
	    type:3,
	    version:5,
	    /*empty*/:16;
    } m88200;
};
#endif ASSEMBLER

#define	M88100		0
#define M88200		5
#define M88204		6

#endif  _M88K_CPUS_
