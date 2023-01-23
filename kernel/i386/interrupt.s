/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * Copyright (c) 1991 IBM Corporation 
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation,
 * and that the name IBM not be used in advertising or publicity 
 * pertaining to distribution of the software without specific, written
 * prior permission.
 * 
 * CARNEGIE MELLON AND IBM ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND IBM DISCLAIM ANY LIABILITY OF ANY KIND FOR
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
 * $Log:	interrupt.s,v $
 * Revision 2.16  93/02/04  07:56:24  danner
 * 		Convert asm comment "/" over to "/ *" "* /"
 * 	[93/01/28            rvb]
 * 
 * 	Integrate PS2 code from IBM.
 * 	[93/01/18            prithvi]
 * 
 * Revision 2.15  92/04/06  01:15:53  rpd
 * 	Converted from #-style to /-style comments, for ANSI preprocessors.
 * 	[92/04/05            rpd]
 * 
 * Revision 2.14  91/10/07  17:24:48  af
 * 	From mg32: testing for spurious interrupts is bogus.
 * 	[91/09/23            rvb]
 * 
 * Revision 2.13  91/08/28  21:31:06  jsb
 * 	Check for out-of-range interrupts.
 * 	[91/08/20            dbg]
 * 
 * Revision 2.12  91/07/31  17:37:31  dbg
 * 	Support separate interrupt stack.  Interrupt handler may now be
 * 	called from different places.
 * 	[91/07/30  16:52:19  dbg]
 * 
 * Revision 2.11  91/06/19  11:55:12  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:44:55  rvb]
 * 
 * Revision 2.10  91/05/14  16:09:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.9  91/05/08  12:38:18  dbg
 * 	Put parentheses around substituted immediate expressions, so
 * 	that they will pass through the GNU preprocessor.
 * 
 * 	Use platforms.h.  Call version of set_spl that leaves interrupts
 * 	disabled (IF clear) until iret.
 * 	[91/04/26  14:35:53  dbg]
 * 
 * Revision 2.8  91/02/05  17:12:22  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:58  mrt]
 * 
 * Revision 2.7  91/01/08  17:32:06  rpd
 * 	Need special interrupt_return
 * 	[90/12/21  14:36:12  rvb]
 * 
 * Revision 2.6  90/12/20  16:35:58  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07  15:43:38  jeffreyh]
 * 
 * Revision 2.5  90/12/04  14:46:08  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:16:47  jsb]
 * 
 * Revision 2.4  90/11/26  14:48:33  rvb
 * 	Change Prime copyright as per Peter J. Weyman authorization.
 * 	[90/11/19            rvb]
 * 
 * Revision 2.3  90/09/23  17:45:14  jsb
 * 	Added support for iPSC2.
 * 	[90/09/21  16:40:09  jsb]
 * 
 * Revision 2.2  90/05/03  15:27:54  dbg
 * 	Stole from Prime.
 * 	Pass new parameters to clock_interrupt (no longer called
 * 	hardclock).  Set curr_ipl correctly around call to clock_interrupt.
 * 	Moved softclock logic to splx.
 * 	Added kdb_kintr to find registers for kdb.
 * 	[90/02/14            dbg]
 * 
 */

/*
Copyright (c) 1988,1989 Prime Computer, Inc.  Natick, MA 01760
All Rights Reserved.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and
without fee is hereby granted, provided that the above
copyright notice appears in all copies and that both the
copyright notice and this permission notice appear in
supporting documentation, and that the name of Prime
Computer, Inc. not be used in advertising or publicity
pertaining to distribution of the software without
specific, written prior permission.

THIS SOFTWARE IS PROVIDED "AS IS", AND PRIME COMPUTER,
INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT SHALL PRIME COMPUTER, INC.  BE LIABLE FOR ANY
SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR
OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <platforms.h>

#include <i386/asm.h>
#include <i386/ipl.h>
#include <i386/pic.h>
#include <assym.s>

/*
 *	Called from locore.s.  The register save area is on top
 *	of the stack.  %eax contains the interrupt number.
 *	Only %ecx and %edx have been saved.
 */
ENTRY(interrupt)

	movl	%eax,%ecx		/* stash interrupt vector number */
	subl	$0x40,%ecx		/* interrupt vectors we use */
					/* start at 0x40, not 0 */
	jl	int_range_err		/* and are between 0x40 */
	cmpl	$15,%ecx		/* and 0x4f */
	jg	int_range_err

#ifndef PS2
/*
 *	Now we must acknowledge the interrupt and issue an EOI command to
 *	the pics.  We send a NON-SPECIFIC EOI, as we assume that the pic
 *	automatically interrupts us with only the highest priority interrupt.
 */

	movl	_master_icw,%edx	/*  2	EOI for master. */
	movw	_PICM_OCW2,%ax		/*  2 */
	outb	%al,%dx			/*  4 */

	movw	_PICS_OCW2,%ax		/*  2	EOI for slave. */
	addw	$(SIZE_PIC),%dx		/*  2 */
	outb	%al,%dx			/*  4 */
#endif PS2

/*
 *	Now we must change the interrupt priority level, with interrupts
 *	turned off.  First we get the interrupt number and get
 *	the interrupt level associated with it, then we call set_spl().
 */

/*
 *	check for spurious interrupt
 */

#ifndef	AT386
	/* 
	  The code below seems to do nothing useful for ISA PC's
	  except to make them unable to take interrupts from a second
	  disk controller that would use interrupt level 15
	 */
	movl	_master_icw, %edx
#if	defined(AT386) || defined(PS2)
	cmpl	$2, %ecx		/* ATs slave the second pic on IRQ2 */
#else	/* AT386 || PS2 */
	cmpl	$7, %ecx		/* iPSCs slave the second pic on IRQ7 */
#endif	/* AT386 || PS2 */
	je	int_check
	cmpl	$15, %ecx		/* IRQ15 */
	jne	int_ok
	addw	$(SIZE_PIC),%dx		/*  2 */
int_check:
	inb	%dx,%al			/* read ISR */
	testb	$0x80, %al		/* return if IS7 is off */
	jz	interrupt_return

int_ok:
#endif	AT386
	movzbl	_intpri(%ecx), %eax	/*  4	intpri[int#]   */
	call	_set_spl		/* interrupts are enabled */

/*
 *	Interrupts are now enabled.  Call the relevant interrupt
 *	handler as per the ivect[] array set up in pic_init.
 */

	pushl	%eax			/*  2	save old IPL */
#ifdef PS2
	pushl	%ecx			/*  2	save intr # */
#endif PS2
	pushl	_iunit(,%ecx,4)		/*  2	push unit# as int handler arg */
	call	*_ivect(,%ecx,4)	/*  4	*ivect[int#]() */
	addl	$4,%esp			/* remove interrupt number from stack */
#ifdef PS2
	popl	%ecx			/*  2	save intr # */
#endif PS2
	cli				/*  3	disable interrupts */

#ifdef PS2
/*
 *      Now we must acknowledge the interrupt and issue an EOI command to 
 *      the pics, we send a SPECIFIC EOI.
 */
        mov     %ecx,%eax
        cmp     $7,%ecx
        jle     do_master
        movl    _slaves_icw,%edx            /*  2  EOI for slave. */
        andb    $7,%al
        orb     $0x60,%al
        outb    %al,%dx
        movb    $2,%al
do_master:
        movl    _master_icw,%edx            /*  2  EOI for master. */
        orb     $0x60,%al
        outb    %al,%dx
#endif PS2

/*
 *	5. Having dealt with the interrupt now we must return to the previous
 *	interrupt priority level.  This is done with interrupts turned off.
 */

	popl	%eax			/* get old IPL from stack */
	cmpl	_curr_ipl, %eax		/* if different from current IPL, */
	je	no_splx
	call	_set_spl_noi		/* reset IPL to old value */
					/* leaving IF off. */
no_splx:

/*
 *	Return to caller.
 */

interrupt_return:
	ret

/*
 * Interrupt number out of range.
 */
int_range_err:
	addl	$0x40,%ecx		/* restore original interrupt number */
	pushl	%ecx			/* push number */
	pushl	$int_range_message	/* push message */
	call	_panic			/* panic */
	addl	$8,%esp			/* pop stack */
	ret				/* return to caller */

int_range_message:
	.ascii	"Bad interrupt number %#x"
	.byte	0

