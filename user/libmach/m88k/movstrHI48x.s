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
 * $Log:	movstrHI48x.s,v $
 * Revision 2.2  92/08/03  18:03:15  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:40:07  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */

	.file	 "movstrHI48x.s"
/*  The following was calculated using awk. */
	.text
	.align	16
	.global	___movstrHI48x48
___movstrHI48x48:
	ld.h	r4,r3,2
	st.h	r5,r2,0
	.global	___movstrHI48x46
___movstrHI48x46:
	ld.h	r5,r3,4
	st.h	r4,r2,2
	.global	___movstrHI48x44
___movstrHI48x44:
	ld.h	r4,r3,6
	st.h	r5,r2,4
	.global	___movstrHI48x42
___movstrHI48x42:
	ld.h	r5,r3,8
	st.h	r4,r2,6
	.global	___movstrHI48x40
___movstrHI48x40:
	ld.h	r4,r3,10
	st.h	r5,r2,8
	.global	___movstrHI48x38
___movstrHI48x38:
	ld.h	r5,r3,12
	st.h	r4,r2,10
	.global	___movstrHI48x36
___movstrHI48x36:
	ld.h	r4,r3,14
	st.h	r5,r2,12
	.global	___movstrHI48x34
___movstrHI48x34:
	ld.h	r5,r3,16
	st.h	r4,r2,14
	.global	___movstrHI48x32
___movstrHI48x32:
	ld.h	r4,r3,18
	st.h	r5,r2,16
	.global	___movstrHI48x30
___movstrHI48x30:
	ld.h	r5,r3,20
	st.h	r4,r2,18
	.global	___movstrHI48x28
___movstrHI48x28:
	ld.h	r4,r3,22
	st.h	r5,r2,20
	.global	___movstrHI48x26
___movstrHI48x26:
	ld.h	r5,r3,24
	st.h	r4,r2,22
	.global	___movstrHI48x24
___movstrHI48x24:
	ld.h	r4,r3,26
	st.h	r5,r2,24
	.global	___movstrHI48x22
___movstrHI48x22:
	ld.h	r5,r3,28
	st.h	r4,r2,26
	.global	___movstrHI48x20
___movstrHI48x20:
	ld.h	r4,r3,30
	st.h	r5,r2,28
	.global	___movstrHI48x18
___movstrHI48x18:
	ld.h	r5,r3,32
	st.h	r4,r2,30
	.global	___movstrHI48x16
___movstrHI48x16:
	ld.h	r4,r3,34
	st.h	r5,r2,32
	.global	___movstrHI48x14
___movstrHI48x14:
	ld.h	r5,r3,36
	st.h	r4,r2,34
	.global	___movstrHI48x12
___movstrHI48x12:
	ld.h	r4,r3,38
	st.h	r5,r2,36
	.global	___movstrHI48x10
___movstrHI48x10:
	ld.h	r5,r3,40
	st.h	r4,r2,38
	.global	___movstrHI48x8
___movstrHI48x8:
	ld.h	r4,r3,42
	st.h	r5,r2,40
	.global	___movstrHI48x6
___movstrHI48x6:
	ld.h	r5,r3,44
	st.h	r4,r2,42
	.global	___movstrHI48x4
___movstrHI48x4:
	ld.h	r4,r3,46
	st.h	r5,r2,44
	jmp.n	r1
	st.h	r4,r2,46

