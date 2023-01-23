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
 * $Log:	machspl.h,v $
 * Revision 2.2  93/05/15  20:58:39  mrt
 * 	Used to be machparam.h
 * 	[93/05/15            mrt]
 * 
 * Revision 2.4  93/01/26  18:05:15  danner
 * 	Added defines to shadow endian.c, include of luna88k/machdep.h
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:39:58  danner
 * 	Added spl_t. Inclusion protections.
 * 	[92/12/18  18:07:36  danner]
 * 
 * Revision 2.2  92/08/03  17:48:15  jfriedl
 * 	created [danner]
 * 
 */

#ifndef	_MACHINE_MACHSPL_H_
#define	_MACHINE_MACHSPL_H_

typedef unsigned int spl_t;

/*
 * Fast versions of what's in "motorola/m88k/endian.c"
 */
#define htonl(L) (L)
#define ntohl(L) (L)
#define htons(L) (L)
#define ntohs(L) (L)

#ifdef luna88k
  #include <luna88k/machdep.h>
#endif

#endif	_MACHINE_MACHSPL_H_
