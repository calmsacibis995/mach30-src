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
 * $Log:	movstrSI64n.s,v $
 * Revision 2.2  92/08/03  18:03:43  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:41:01  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */

	.file	 "movstrSI64n.s"
/* The following was calculated using awk. */
	.text
	.align	16
loopSI64:
	addu	r3,r3,64
	addu	r2,r2,64
	subu	r6,r6,1
	.global	___movstrSI64n68
___movstrSI64n68:
	ld	r5,r3,4
	st	r4,r2,0
	.global	___movstrSI64n64
___movstrSI64n64:
	ld	r4,r3,8
	st	r5,r2,4
	.global	___movstrSI64n60
___movstrSI64n60:
	ld	r5,r3,12
	st	r4,r2,8
	.global	___movstrSI64n56
___movstrSI64n56:
	ld	r4,r3,16
	st	r5,r2,12
	.global	___movstrSI64n52
___movstrSI64n52:
	ld	r5,r3,20
	st	r4,r2,16
	.global	___movstrSI64n48
___movstrSI64n48:
	ld	r4,r3,24
	st	r5,r2,20
	.global	___movstrSI64n44
___movstrSI64n44:
	ld	r5,r3,28
	st	r4,r2,24
	.global	___movstrSI64n40
___movstrSI64n40:
	ld	r4,r3,32
	st	r5,r2,28
	.global	___movstrSI64n36
___movstrSI64n36:
	ld	r5,r3,36
	st	r4,r2,32
	.global	___movstrSI64n32
___movstrSI64n32:
	ld	r4,r3,40
	st	r5,r2,36
	.global	___movstrSI64n28
___movstrSI64n28:
	ld	r5,r3,44
	st	r4,r2,40
	.global	___movstrSI64n24
___movstrSI64n24:
	ld	r4,r3,48
	st	r5,r2,44
	.global	___movstrSI64n20
___movstrSI64n20:
	ld	r5,r3,52
	st	r4,r2,48
	.global	___movstrSI64n16
___movstrSI64n16:
	ld	r4,r3,56
	st	r5,r2,52
	.global	___movstrSI64n12
___movstrSI64n12:
	ld	r5,r3,60
	st	r4,r2,56
	.global	___movstrSI64n8
___movstrSI64n8:
	ld	r4,r3,64
	bcnd.n	gt0,r6,loopSI64
	st	r5,r2,60
	jmp.n	r1
	st	r4,r2,64
