/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON AND OMRON ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND OMRON DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/* 
 *
 **********************************************************************
 * This file created by Omron Corporation.
 *
 * HISTORY
 * $Log:	rawprint_asm_routines.s,v $
 * Revision 2.3  93/01/26  18:04:54  danner
 * 	Replaced ;comments with C-style comments for better CPPification.
 * 	Some gcc cleanup.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:44:38  jfriedl
 * 	Created.
 * 	[92/07/24            jfriedl]
 * 
 *****************************************************************RCS**/

#ifndef ASSEMBLER /* predefined by ascpp, at least */
#define ASSEMBLER
#endif

#include <luna88k/locore.h>

    text
    align 4
#if !defined(RAW_PRINTF) || !RAW_PRINTF
    /* If not defined, just make them no-ops in case someone else calls them */
    LABEL(_raw_putstr)
    LABEL(_raw_putchar)
	jmp r1

#else

    #define RAW_INTERCHAR_PAUSE 0x1000 /* range 0x0000 .. 0xffff */
    raw_put_lock: word 0

/*
 * Very low-level raw output to the serial line *ONLY*.
 * (nothing goes to the bitmapped console).
 *
 * Only registers used are r1, r2, r3, and r4 (no stack used).
 * This routine is very safe and robust and can be called at the most
 * delecate of times.  About the only thing one must watch out for is
 * if the cache state between processors is inconsistant, which happens
 * at times during the boot.
 *
 * void raw_putstr(char *string)
 *	Output the null-terminated string to the serial line.
 *	The print lock is held througout, so concurrent calls won't have
 *	their output intermixed.
 *
 * void raw_putchar(char c);
 *	Output the character to the serial line.
 *
 * _raw_putstr:
 *   ENTER:
 *	r1 = return address
 *	r2 = string pointer
 *   EXIT:
 *	r2, r3, r4 trashed.
 *	R1 low two bits cleared (doesn't matter if it's a return address)
 *	All other registers unchanged.
 *
 * _raw_putchar:
 *   ENTER:
 *	r1 = return address
 *	r2 = data byte
 *   EXIT:
 *	r3, r4 trashed.
 *	R1 low two bits cleared (doesn't matter if it's a return address)
 *	All other regesters unmodified..
 *
 */
    raw_putstr_retval: word 0

LABEL(_raw_putstr)
	/* access the entire string to make sure it is all resident. */
	or	r3, r2, r0
     1: ld.bu	r4, r3, 0
	bcnd.n	ne0, r4, 1b
	addu	r3, r3, 1
	
	FLUSH_PIPELINE
	set	r1, r1, 1<0> /* use the low bit of R1 to mark 'putstr'. */
	/* fall-through */

LABEL(_raw_putchar)
	/* disable interrupts, save original PSR in r4 */
	ldcr	r4, PSR
	set	r3, r4, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r3, PSR
	FLUSH_PIPELINE

	/* try to grab lock */
	or.u	r3, r0, hi16(raw_put_lock)
	or	r3, r3, lo16(raw_put_lock)
	xmem	r3, r3, r0	/* if returns 0, we have the lock.*/
	FLUSH_PIPELINE
	bcnd	eq0, r3, have_raw_put_lock

	/* put PSR back (maybe interrupts go back on) */
	stcr	r4, PSR
	FLUSH_PIPELINE

	/* pause for a while before checking */
     1:	or	r3, r0, RAW_INTERCHAR_PAUSE
     2:	bcnd.n	ne0, r3, 2b
        subu	r3, r3, 1

	/* take a peek */
	or.u	r3, r0, hi16(raw_put_lock)
	ld	r3, r3, lo16(raw_put_lock)
	bcnd	eq0, r3, _raw_putchar /* might be able to get it.. try */
	br	1b

   have_raw_put_lock:
	/* split now depending upon putchar and putstr */
	bb1	0, r1, do_raw_putstr		/* check mark */

	/* access the serial line control port */
       uart_check:
	or.u	r3, r0, hi16(0x51000004)
	ld.b	r3, r3, lo16(0x51000004)
    	bb0	2, r3, uart_check

	/* now do *(unsigned char *)0x51000000 = r2 to send char */
	or.u	r3, r0, hi16(0x51000000)
	st.b	r2, r3, lo16(0x51000000)

	/* pause a bit before we shove the character. */
       	or	r3, r0, RAW_INTERCHAR_PAUSE
     3: bcnd.n	ne0, r3, 3b
	subu	r3, r3, 1

   done_with_put:
        /* all done. Release the lock, restore PSR, and return. */
	or.u	r3, r0, hi16(raw_put_lock)
	st	r0, r3, lo16(raw_put_lock)
	FLUSH_PIPELINE
	stcr	r4, PSR
	jmp	r1

   do_raw_putstr:
	clr	r1, r1, 1<0> /* remove mark. */
	/* stuff return value away so we can use R1 */
	or.u	r3, r0, hi16(raw_putstr_retval)
	st	r1, r3, lo16(raw_putstr_retval)

     4: ld.bu	r1, r2, r0  /* get byte to print. */
	bcnd	eq0, r1, 6f /* done if zero */

     uart_check2:
	/* pause a bit before we shove the character. */
       	or	r3, r0, RAW_INTERCHAR_PAUSE
     5: bcnd.n	ne0, r3, 5b
	subu	r3, r3, 1

	/* access the serial line control port */
	or.u	r3, r0, hi16(0x51000004)
	ld.bu	r3, r3, lo16(0x51000004)
    	bb0	2, r3, uart_check2

	/* now do *(unsigned char *)0x51000000 = r1 */
	or.u	r3, r0, hi16(0x51000000)
	st.b	r1, r3, lo16(0x51000000)


	/* now increment r2 (string pointer) */
	addu	r2, r2, 1
	br	4b	/* continue with loop */
	
     6: or.u	r3, r0, hi16(raw_putstr_retval) /* reclaim R1, return. */
	ld	r1, r3, lo16(raw_putstr_retval)
	br done_with_put


LABEL(replace_mayput_with_rawputchar)
	/*
	 * Want all uses of mayput() to use our lock.
	 * Therefore, we install a wrapper routine that the system
	 * will think is mayput. It will get the lock, call the real
	 * mayput, and then release the lock and return.
	 *
	 * That wrapper routine is pre_mayput(), and we install
	 * it into the ROM vector table here.
	 *
	 * Note that address 0x1110 is where the system expects to
	 * find the address of the mayput routine.
	 */
	ld	r3, r0, 0x1110	/* get address of original mayput */
	or.u	r2, r0, hi16(real_mayput_addr)
	st	r3, r2, lo16(real_mayput_addr) /* stuff away */
	or.u	r3, r0, hi16(pre_mayput)
	or	r3, r3, lo16(pre_mayput)
	st	r3, r0, 0x1110  /* stuff ; replace with ours. */
	jmp	r1

    align 4
    real_mayput_addr: word 0
    real_mayput_return: word 0
    orig_mayput_psr: word 0
  /*
   * The routine that is called instead of mayput....
   */
    pre_mayput:
	/*
	 * on entry,
	 *	r1 is return address
	 *	r2 is byte to output
	 * we can trash r3-r9 as we like.
	 */
	/* disable interrupts, save original PSR in r4 */
	ldcr	r4, PSR
	set	r3, r4, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r3, PSR
	FLUSH_PIPELINE

	/* try to grab lock */
	or.u	r3, r0, hi16(raw_put_lock)
	or	r3, r3, lo16(raw_put_lock)
	xmem	r3, r3, r0	/* if returns 0, we have the lock. */
	FLUSH_PIPELINE
	bcnd	eq0, r3, mayput__have_raw_put_lock

	/* put PSR back (maybe interrupts go back on) */
	stcr	r4, PSR
	FLUSH_PIPELINE

	/* pause for a while before checking */
     1:	or	r3, r0, RAW_INTERCHAR_PAUSE
     2:	bcnd.n	ne0, r3, 2b
        subu	r3, r3, 1

	/* take a peek */
	or.u	r3, r0, hi16(raw_put_lock)
	ld	r3, r3, lo16(raw_put_lock)
	bcnd	eq0, r3, _raw_putchar /* might be able to get it.. try */
	br	1b

    mayput__have_raw_put_lock:
	/* save real return address. */
	or.u r3, r0, hi16(real_mayput_return)
	st   r1, r3, lo16(real_mayput_return)
	/* save PSR */
	or.u r3, r0, hi16(orig_mayput_psr)
	st   r4, r3, lo16(orig_mayput_psr)
	/* jump to real mayput */
	or.u r3, r0, hi16(real_mayput_addr)
	ld   r3, r3, lo16(real_mayput_addr)
	jsr r3

	/* get back original return address */
	or.u r3, r0, hi16(real_mayput_return)
	ld   r1, r3, lo16(real_mayput_return)
	/* get original psr */
	or.u r3, r0, hi16(orig_mayput_psr)
	ld   r4, r3, lo16(orig_mayput_psr)

	/*  all done. Release the lock, restore PSR, and return. */
	or.u    r3, r0, hi16(raw_put_lock)
	st      r0, r3, lo16(raw_put_lock)
	FLUSH_PIPELINE
	stcr    r4, PSR
	jmp     r1



/*
 * user_raw_putstr
 *
 * Prints the string in r2 (r2 is an address in the USER address space)
 * to the uart.
 *	IN:  R2 (user address)
 *	OUT: R2=0 (no error)
 *	Modified:
 *		R2
 *		SR2
 *		SR3
 *
 * Be careful... Little error checking is done....
 */ 
LABEL(user_raw_putstr)
        stcr      r3, SR2       #save a scratch register
        stcr      r4, SR3       #save a scratch register

 	/* try to grab the global UART lock. */

   user__try_to_grab_lock:
        or.u    r3, r0, hi16(raw_put_lock)
        or      r3, r3, lo16(raw_put_lock)
        xmem    r3, r3, r0      /* if returns 0, we have the lock. */
        bcnd    eq0, r3, user__have_raw_put_lock
 
        /*  pause for a while before checking */
     1: or      r3, r0, RAW_INTERCHAR_PAUSE
     2: bcnd.n  ne0, r3, 2b
        subu    r3, r3, 1
 
        /* take a peek */
        or.u    r3, r0, hi16(raw_put_lock)
        ld      r3, r3, lo16(raw_put_lock)
        bcnd    eq0, r3, user__try_to_grab_lock
        br      1b

   user__have_raw_put_lock:
	/* Have the lock... output a character. */
	
     4: ld.bu.usr r4, r2, r0  /* get byte to print.*/
	bcnd	  eq0, r4, 6f /* done if zero */

     uart_check3:
	/* pause a bit before we shove the character. */
       	or	r3, r0, RAW_INTERCHAR_PAUSE
     5: bcnd.n	ne0, r3, 5b
	subu	r3, r3, 1

	/* access the serial line control port */
	or.u	r3, r0, hi16(0x51000004)
	ld.bu	r3, r3, lo16(0x51000004)
    	bb0	2, r3, uart_check3

	/* now do *(unsigned char *)0x51000000 = r4 */
	or.u	r3, r0, hi16(0x51000000)
	st.b	r4, r3, lo16(0x51000000)

	/* now increment r2 (string pointer) */
	addu	r2, r2, 1
	br  	4b	/* continue with loop */
	
     6: 
        /* all done. Release the lock, restore temps, and return. */
        or.u    r3, r0, hi16(raw_put_lock)
        st      r0, r3, lo16(raw_put_lock)
	ldcr	r3, SR2	/* know we are from user mode... ok to use */
	ldcr	r4, SR3	/* SR2 and SR3 as we like.... */
	or	r2, r0, r0	## signal "no-error" return.
	rte
#endif /* endif RAW_PRINTF */
