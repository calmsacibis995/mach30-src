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
 * $Log:	movstrHI15x.s,v $
 * Revision 2.2  92/08/03  18:03:10  jfriedl
 * 	brought from gcc source [danner]
 * 
 * Revision 2.1.1.1  92/07/17  21:39:56  jfriedl
 * 	Converted to C style comments.
 * 	[92/05/16            danner]
 * 
 *
 */

	.file	 "movstrHI15x.s"
/* The following was calculated using awk. */
	.text
	.align	16
	.global	___movstrHI15x15
___movstrHI15x15:
	ld.h	r4,r3,2
	st.h	r5,r2,0
	.global	___movstrHI15x13
___movstrHI15x13:
	ld.h	r5,r3,4
	st.h	r4,r2,2
	.global	___movstrHI15x11
___movstrHI15x11:
	ld.h	r4,r3,6
	st.h	r5,r2,4
	.global	___movstrHI15x9
___movstrHI15x9:
	ld.h	r5,r3,8
	st.h	r4,r2,6
	.global	___movstrHI15x7
___movstrHI15x7:
	ld.h	r4,r3,10
	st.h	r5,r2,8
	.global	___movstrHI15x5
___movstrHI15x5:
	ld.h	r5,r3,12
	st.h	r4,r2,10
	ld.b	r4,r3,14
	st.h	r5,r2,12
	jmp.n	r1
	st.b	r4,r2,14

