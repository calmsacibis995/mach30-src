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
 * 
 * HISTORY
 * $Log:	movstrSI47x.s,v $
 * Revision 2.2  92/08/03  18:03:38  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:40:50  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */

	.file	 "movstrSI47x.s"
/* The following was calculated using awk. */
	.text
	.align	16
	.global	___movstrSI47x47
___movstrSI47x47:
	ld	r4,r3,4
	st	r5,r2,0
	.global	___movstrSI47x43
___movstrSI47x43:
	ld	r5,r3,8
	st	r4,r2,4
	.global	___movstrSI47x39
___movstrSI47x39:
	ld	r4,r3,12
	st	r5,r2,8
	.global	___movstrSI47x35
___movstrSI47x35:
	ld	r5,r3,16
	st	r4,r2,12
	.global	___movstrSI47x31
___movstrSI47x31:
	ld	r4,r3,20
	st	r5,r2,16
	.global	___movstrSI47x27
___movstrSI47x27:
	ld	r5,r3,24
	st	r4,r2,20
	.global	___movstrSI47x23
___movstrSI47x23:
	ld	r4,r3,28
	st	r5,r2,24
	.global	___movstrSI47x19
___movstrSI47x19:
	ld	r5,r3,32
	st	r4,r2,28
	.global	___movstrSI47x15
___movstrSI47x15:
	ld	r4,r3,36
	st	r5,r2,32
	.global	___movstrSI47x11
___movstrSI47x11:
	ld	r5,r3,40
	st	r4,r2,36
	ld.h	r4,r3,44
	st	r5,r2,40
	ld.b	r5,r3,46
	st.h	r4,r2,44
	jmp.n	r1
	st.b	r5,r2,46

