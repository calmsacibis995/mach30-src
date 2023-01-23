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
 * $Log:	movstrSI96x.s,v $
 * Revision 2.2  92/08/03  18:03:47  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:41:09  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */
	.file	 "movstrSI96x.s"
/* The following was calculated using awk. */
	.text
	.align	16
	.global	___movstrSI96x96
___movstrSI96x96:
	ld	r4,r3,4
	st	r5,r2,0
	.global	___movstrSI96x92
___movstrSI96x92:
	ld	r5,r3,8
	st	r4,r2,4
	.global	___movstrSI96x88
___movstrSI96x88:
	ld	r4,r3,12
	st	r5,r2,8
	.global	___movstrSI96x84
___movstrSI96x84:
	ld	r5,r3,16
	st	r4,r2,12
	.global	___movstrSI96x80
___movstrSI96x80:
	ld	r4,r3,20
	st	r5,r2,16
	.global	___movstrSI96x76
___movstrSI96x76:
	ld	r5,r3,24
	st	r4,r2,20
	.global	___movstrSI96x72
___movstrSI96x72:
	ld	r4,r3,28
	st	r5,r2,24
	.global	___movstrSI96x68
___movstrSI96x68:
	ld	r5,r3,32
	st	r4,r2,28
	.global	___movstrSI96x64
___movstrSI96x64:
	ld	r4,r3,36
	st	r5,r2,32
	.global	___movstrSI96x60
___movstrSI96x60:
	ld	r5,r3,40
	st	r4,r2,36
	.global	___movstrSI96x56
___movstrSI96x56:
	ld	r4,r3,44
	st	r5,r2,40
	.global	___movstrSI96x52
___movstrSI96x52:
	ld	r5,r3,48
	st	r4,r2,44
	.global	___movstrSI96x48
___movstrSI96x48:
	ld	r4,r3,52
	st	r5,r2,48
	.global	___movstrSI96x44
___movstrSI96x44:
	ld	r5,r3,56
	st	r4,r2,52
	.global	___movstrSI96x40
___movstrSI96x40:
	ld	r4,r3,60
	st	r5,r2,56
	.global	___movstrSI96x36
___movstrSI96x36:
	ld	r5,r3,64
	st	r4,r2,60
	.global	___movstrSI96x32
___movstrSI96x32:
	ld	r4,r3,68
	st	r5,r2,64
	.global	___movstrSI96x28
___movstrSI96x28:
	ld	r5,r3,72
	st	r4,r2,68
	.global	___movstrSI96x24
___movstrSI96x24:
	ld	r4,r3,76
	st	r5,r2,72
	.global	___movstrSI96x20
___movstrSI96x20:
	ld	r5,r3,80
	st	r4,r2,76
	.global	___movstrSI96x16
___movstrSI96x16:
	ld	r4,r3,84
	st	r5,r2,80
	.global	___movstrSI96x12
___movstrSI96x12:
	ld	r5,r3,88
	st	r4,r2,84
	.global	___movstrSI96x8
___movstrSI96x8:
	ld	r4,r3,92
	st	r5,r2,88
	jmp.n	r1
	st	r4,r2,92

