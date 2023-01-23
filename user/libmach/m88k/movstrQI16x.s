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
 * $Log:	movstrQI16x.s,v $
 * Revision 2.2  92/08/03  18:03:25  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:40:21  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */

	.file	 "movstrQI16x.s"
/* The following was calculated using awk. */
	.text
	.align	16
	.global	___movstrQI16x16
___movstrQI16x16:
	ld.b	r4,r3,1
	st.b	r5,r2,0
	.global	___movstrQI16x15
___movstrQI16x15:
	ld.b	r5,r3,2
	st.b	r4,r2,1
	.global	___movstrQI16x14
___movstrQI16x14:
	ld.b	r4,r3,3
	st.b	r5,r2,2
	.global	___movstrQI16x13
___movstrQI16x13:
	ld.b	r5,r3,4
	st.b	r4,r2,3
	.global	___movstrQI16x12
___movstrQI16x12:
	ld.b	r4,r3,5
	st.b	r5,r2,4
	.global	___movstrQI16x11
___movstrQI16x11:
	ld.b	r5,r3,6
	st.b	r4,r2,5
	.global	___movstrQI16x10
___movstrQI16x10:
	ld.b	r4,r3,7
	st.b	r5,r2,6
	.global	___movstrQI16x9
___movstrQI16x9:
	ld.b	r5,r3,8
	st.b	r4,r2,7
	.global	___movstrQI16x8
___movstrQI16x8:
	ld.b	r4,r3,9
	st.b	r5,r2,8
	.global	___movstrQI16x7
___movstrQI16x7:
	ld.b	r5,r3,10
	st.b	r4,r2,9
	.global	___movstrQI16x6
___movstrQI16x6:
	ld.b	r4,r3,11
	st.b	r5,r2,10
	.global	___movstrQI16x5
___movstrQI16x5:
	ld.b	r5,r3,12
	st.b	r4,r2,11
	.global	___movstrQI16x4
___movstrQI16x4:
	ld.b	r4,r3,13
	st.b	r5,r2,12
	.global	___movstrQI16x3
___movstrQI16x3:
	ld.b	r5,r3,14
	st.b	r4,r2,13
	.global	___movstrQI16x2
___movstrQI16x2:
	ld.b	r4,r3,15
	st.b	r5,r2,14
	jmp.n	r1
	st.b	r4,r2,15

