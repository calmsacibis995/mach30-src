/* 
 *  Mach Operating System
 *  Copyright (c) 1992 Carnegie Mellon University
 *  All Rights Reserved.
 *  
 *  Permission to use, copy, modify and distribute this software and its
 *  documentation is hereby granted, provided that both the copyright
 *  notice and this permission notice appear in all copies of the
 *  software, derivative works or modified versions, and any portions
 *  thereof, and that both notices appear in supporting documentation.
 *  
 *  CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 *  CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *  
 *  Carnegie Mellon requests users of this software to return to
 *  
 *   Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *   School of Computer Science
 *   Carnegie Mellon University
 *   Pittsburgh PA 15213-3890
 *  
 *  any improvements or extensions that they make and grant Carnegie Mellon 
 *  the rights to redistribute these changes.
 * 
 */
/*
 * 
 * HISTORY
 * $Log:	fork.s,v $
 * Revision 2.2  92/08/03  18:03:04  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 * Revision 2.1.1.1  92/05/27  15:11:52  danner
 * 	Changed to c-style comments.
 * 	[92/05/16            danner]
 * 
 * 
 */
	text
	align   8
	
_fork:	global	_fork
	or	r9, r0, 2
	tb0	0, r0, 0x80
	br	2f		 /* failed  */
	bcnd	eq0, r3, 1f      
	subu	r31, r31, 0x30	 /* stack frame alloc */
	st	r1,  r31, 0x20	 /* store return address */
	bsr	_mach_init	 /* the child */
	ld      r1,  r31, 0x20	 /* restore return address */
	addu	r31, r31, 0x30	 /* reset stack pointer */
	or	r2, r0, 0	 
1:  
	jmp	r1		 
2:      
	or.u	r3, r0, hi16(_errno) /* fork failed */
	st	r2, r3, hi16(_errno)
	jmp.n  r1
	subu	r2, r0, 1	     /* return -1 */

