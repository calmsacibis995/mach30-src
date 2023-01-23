/* 
 *  Mach Operating System
 *  Copyright (c) 1991 Carnegie Mellon University
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
 */
/*
 * 
 * HISTORY
 * $Log:	_setjmp.s,v $
 * Revision 2.2  92/08/03  18:02:46  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 * Revision 2.3  92/02/19  14:20:03  elf
 *
 *  	Comment format changed.
 *  	[92/02/18  16:39:09  danner]
 *  	An unbalanced comment was eating most of _setjmp.
 *  	[91/11/03            danner]
 *  
 * Revision 2.2  91/08/24  12:31:03  af
 *  	Added _longjmp, optimized setjmp.
 *  	[91/07/20            danner]
 *  	Renamed from _setjmp.h. Completed
 *  	[91/07/11            danner]
 *  
 * Revision 2.1.3.1  91/08/19  13:49:16  danner
 *  	Added _longjmp, optimized setjmp.
 *  	[91/07/20            danner]
 *  
 *  	Renamed from _setjmp.h. Completed
 *  	[91/07/11            danner]
 *  
 * Revision 2.2  91/07/09  23:23:58  danner
 *  	Created. Incomplete
 *  	[91/07/08  23:01:08  danner]
 *  
 */
	text
/*
       setjmp needs to save r1, r14-r31 
       We assume the buffer is only word aligned.

       r2 contains the address of the jumpbuf.
       We save r1, r14-r31, a total of 19 registers.
*/

	align 4
__setjmp:	global __setjmp
_setjmp:        global _setjmp  
        extu    r3, r2, 3<0>     /* get the low three bits */
        bcnd.n	ne0, r3, 1f      /* only word aligned */
	st	r1,  r2, 0
	st	r14, r2, 4       /* now back to double word alignment */
	st.d	r15, r2, 8
	st.d	r17, r2, 16
	st.d	r19, r2, 24
	st.d	r21, r2, 32
	st.d	r23, r2, 40
	st.d	r25, r2, 48
	st.d	r27, r2, 56
	st.d	r29, r2, 64
	st	r31, r2, 72
  	jmp.n	r1              /* hop back */
  	or	r2, r0, r0
1:
	st.d	r14, r2, 4      /* double word aligned  */
	st.d	r16, r2, 12
	st.d	r18, r2, 20
	st.d	r20, r2, 28
	st.d	r22, r2, 36
	st.d	r24, r2, 44
	st.d	r26, r2, 52
	st.d	r28, r2, 60
	st.d	r30, r2, 68
  	jmp.n	r1              /* hop back */
  	or	r2, r0, r0

	align 4
__longjmp:	global __longjmp
_longjmp:       global _longjmp  
/*
 *	Object:
 *		longjmp				EXPORTED function
 *
 *		Perform a non-local goto
 *
 *	Arguments:
 *		r2				jmp_buf *
 *		r3				int
 *
 *	Restores all registers that are callee-saved from the
 *      given longjmp buffer, as set in the above routines. Returns
 *      the second argument (r3).
 *
 */
        extu    r4, r2, 3<0>     /* get the low three bits */
        bcnd.n	ne0, r4, 1f      /* only word aligned */
	ld	r1,  r2, 0	 /* restore return address */
	ld	r14, r2, 4       /* now back to proper alignment */
	ld.d	r15, r2, 8
	ld.d	r17, r2, 16
	ld.d	r19, r2, 24
	ld.d	r21, r2, 32
	ld.d	r23, r2, 40
	ld.d	r25, r2, 48
	ld.d	r27, r2, 56
	ld.d	r29, r2, 64
	ld	r31, r2, 72
	jmp.n	r1
	or	r2, r0, r3      /* pick up correct return value */
1:
	ld.d	r14, r2, 4      
	ld.d	r16, r2, 12
	ld.d	r18, r2, 20
	ld.d	r20, r2, 28
	ld.d	r22, r2, 36
	ld.d	r24, r2, 44
	ld.d	r26, r2, 52
	ld.d	r28, r2, 60
	ld.d	r30, r2, 68
  	jmp.n	r1              
  	or	r2, r0, r3
