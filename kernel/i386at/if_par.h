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
 * $Log:	if_par.h,v $
 * Revision 2.2  92/02/19  16:30:03  elf
 * 	Add lpr and par devices.  (taken from 2.5)
 * 	[92/02/13            rvb]
 * 
 * Revision 2.2  91/04/02  12:08:52  mbj
 * 		Changed protocol to utilitize LapLink cables.  Incompatible with
 * 	previous revisions.
 * 		[91/03/15            mg32]
 * 	Created 08/05/90.
 * 		Parallel port network driver.
 * 	[90/08/14            mg32]
 * 
 */
 
/* 
 *	Parallel port network driver v1.0
 *	All rights reserved.
 */ 
#define OUTPUT(addr) (addr + 0)
#define INPUT(addr)  (addr + 1)
#define INTR(addr)   (addr + 2)

#define SPLNET		spl6
#define PARMTU		8192
#define MAXSPIN		10000
