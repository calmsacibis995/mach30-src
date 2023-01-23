/*
   Mach Operating System
   Copyright (c) 1992, 1991 Carnegie Mellon University
   All Rights Reserved.
   
   Permission to use, copy, modify and distribute this software and its
   documentation is hereby granted, provided that both the copyright
   notice and this permission notice appear in all copies of the
   software, derivative works or modified versions, and any portions
   thereof, and that both notices appear in supporting documentation.
   
   CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
   CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
   ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
   
   Carnegie Mellon requests users of this software to return to
   
    Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
    School of Computer Science
    Carnegie Mellon University
    Pittsburgh PA 15213-3890
   
   any improvements or extensions that they make and grant Carnegie Mellon 
   the rights to redistribute these changes.
*/

/*
 * HISTORY
 * $Log:	csw.s,v $
 * Revision 2.2  92/08/03  18:04:01  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 *
 */ 
 
/*

 m88k/csw.s

 Context switch and cproc startup for m88K COROUTINE implementation.

*/

	text

/*
 
  Suspend the current thread and resume the next one.
 
 	void
 	cproc_switch(cur, next, lock)
 		int *cur;
 		int *next; 
 
*/
	align	4
	global	_cproc_switch
_cproc_switch:
	subu	r31,r31,0x40	/* 4 bytes * 16, for r30, r1, r14-25, */
				/* fcr62, and fcr63 */
	st	r30,r31,0x00	/* old frame pointer */
	st	r1, r31,0x04	/* return address */
	st	r14,r31,0x08
	st	r15,r31,0x0c
	st	r16,r31,0x10
	st	r17,r31,0x14
	st	r18,r31,0x18
	st	r19,r31,0x1c
	st	r20,r31,0x20
	st	r21,r31,0x24
	st	r22,r31,0x28
	st	r23,r31,0x2c
	st	r24,r31,0x30
	st	r25,r31,0x34
	fldcr	r11,fcr62
	st	r11,r31,0x38
	fldcr	r11,fcr63
	st	r11,r31,0x3c
	st	r31,r2,0	/* save current fp */

	st	r0, r4,0	/* clear lock */

	ld	r31 ,r3,0	/* restore next fp */
	ld	r30,r31,0x00	/* frame pointer */
	ld	r1, r31,0x04	/* return address */
	ld	r14,r31,0x08
	ld	r15,r31,0x0c
	ld	r16,r31,0x10
	ld	r17,r31,0x14
	ld	r18,r31,0x18
	ld	r19,r31,0x1c
	ld	r20,r31,0x20
	ld	r21,r31,0x24
	ld	r22,r31,0x28
	ld	r23,r31,0x2c
	ld	r24,r31,0x30
	ld	r25,r31,0x34
	ld	r11,r31,0x38
	fstcr	r11,fcr62
	ld	r11,r31,0x3c
	fstcr	r11,fcr63
	jmp.n	r1		/*  return to next thread */
	addu	r31,r31,0x40	/* restore sp */

/*
 	void
 	cproc_start_wait(parent_context, child, stackp, lock)
 		int *parent_context;
 		cproc_t child;
 		int stackp;
*/

	align	4
	global	_cproc_start_wait
_cproc_start_wait:
	subu	r31,r31,0x40	/* 4 bytes * 16, for r30, r1, r14-25, */
				/* fcr62, and fcr63 */
	st	r30,r31,0x00	/* old frame pointer */
	st	r1, r31,0x04	/* return address */
	st	r14,r31,0x08
	st	r15,r31,0x0c
	st	r16,r31,0x10
	st	r17,r31,0x14
	st	r18,r31,0x18
	st	r19,r31,0x1c
	st	r20,r31,0x20
	st	r21,r31,0x24
	st	r22,r31,0x28
	st	r23,r31,0x2c
	st	r24,r31,0x30
	st	r25,r31,0x34
	fldcr	r11,fcr62
	st	r11,r31,0x38
	fldcr	r11,fcr63
	st	r11,r31,0x3c
	st	r31,r2 ,0	/* save current fp */

	st	r0 ,r5 ,0	/* clear lock */

	subu	r31,r4 ,4	/* align */
	or	r30,r0 ,0	/*  zero fp */

	br.n	_cproc_waiting
	or	r2 ,r0 ,r3	/* set arg */
/*
	 
	  Control never returns from cthread_body().
	 
*/

/*
 	void
 	cproc_prepare(child, child_context, stack)
 		int *child_context;
		int *stack;
*/
	global	_cproc_prepare
_cproc_prepare:
	subu	r4 ,r4 ,0x10	/* make room for (child) */
	st	r2 ,r4 ,0	/* save */
	subu	r4 ,r4 ,0x40	/* make cproc_switch frame */
	st	r0 ,r4, 0	/* frame */
	st	r0 ,r4 ,0x38	/* fake fcr62 */
	st	r0 ,r4 ,0x3c	/* fake fcr63 */
	or.u	r10,r0 ,hi16(cproc_set_args) /* return address */
	or	r10,r10,lo16(cproc_set_args)
	st	r10,r4,0x4	/* return address */
	st	r4 ,r3, 0	/* save fp */
	jmp	r1

cproc_set_args:	
	ld	r2 ,r31,0	/* cthread_body(child) */
	br.n	_cthread_body
	addu	r31,r31,0x8	/* fix stack */

/*
  get current stack pointer 
*/
	global	_cthread_sp
_cthread_sp:	
	jmp.n 	r1
	or	r2, r0, r31


