/*
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
/* locore_asm_routines.c 
 *
 **********************************************************************
 * This file created by Omron Corporation, 1990.
 *
 * HISTORY
 * $Log:	locore_asm_routines.s,v $
 * Revision 2.6  93/03/09  17:57:37  danner
 * 	Created raw_xpr stuff.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.5  93/02/04  07:49:38  danner
 * 	luna88k/asm.h -> m88k/asm.h
 * 	[93/02/02            jfriedl]
 * 
 * Revision 2.4  93/01/26  18:04:41  danner
 * 	Replaced ;comments with C-style comments for better CPPification.
 * 	Cleaned up includes. Removed unused byte_* fcns, other old cruft.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:38:40  danner
 * 	various cleanups.
 * 	[92/12/10            jfriedl]
 * 
 * Revision 2.2  92/08/03  18:14:39  jfriedl
 * 	Moved and renamed from locore/locore_asm_routines.c
 * 	[92/07/24            jfriedl]
 * 
 *
 * Revision 2.6.2.1  92/05/27  14:39:19  danner
 * 	Fixed simple_lock to be cleanly tracable when compiled with
 * 	SIMPLE_LOCK_DEBUG on.
 * 
 * Revision 2.7  92/05/21  17:19:30  jfriedl
 * 	cleanup.
 * 	[92/05/21  16:09:11  jfriedl]
 * 
 * Revision 2.6  92/05/04  11:27:04  danner
 * 	Increased timeout on simple lock debugging.
 * 	[92/04/18            danner]
 * 
 * Revision 2.5  92/04/01  10:55:07  rpd
 * 	Cleaned up log.
 * 	[92/03/21  12:02:38  danner]
 * 
 * 	Added simple lock debugging code
 * 	[92/03/16  17:20:02  danner]
 * 
 * Revision 2.4  92/02/18  18:01:53  elf
 * 	Fixed bugs in enable/disable interrupt routines.
 * 	[92/02/01            danner]
 * 
 * 	moved call_rom_abort here.
 * 	[92/01/30            danner]
 * 
 * 	Updated includes.
 * 	[92/01/30            danner]
 * 	setjmp -> _setjmp
 * 	longjmp -> _longjmp.
 * 	[92/01/19            danner]
 * 
 * 	Added simple_lock_pause. Added lockpause counters to go with it.
 * 	 Made lockinit, lockuse, and lockpause reside in the data
 * 	 segment. Changed there definitions to depend on NCPUS, instead
 * 	 of implicitly assuming 4.
 * 	[92/01/19            danner]
 * 
 * 	Add memcpy definition for gcc 2.
 * 	[92/01/19            danner]
 * 
 * Revision 2.3  91/08/24  12:11:34  af
 * 	Removed ffs, suword, fuword, careful_bcopy
 * 	[91/08/06            danner]
 * 	Added read processor identification register function.
 * 	[91/07/20  15:36:01  danner]
 * 
 * Revision 2.2.3.1  91/08/19  13:47:00  danner
 * 	Removed ffs, suword, fuword, careful_bcopy
 * 	[91/08/06            danner]
 * 	Added read processor identification register function.
 * 	[91/07/20  15:36:01  danner]
 * 
 * Revision 2.2  91/07/09  23:18:02  danner
 * 	Copyinmsg and copyoutmsg aliased to copyin and copyout, resp.
 * 	[91/05/08            danner]
 * 
 * 	Removed include of sys/errno.h and copyinstr, copyoutstr,
 * 	     addupc, copystr.
 * 	[91/05/05            danner]
 * 
 * 	Removed include sys/syscall.h
 * 	[91/05/05            danner]
 * 	Flush jkdb.
 * 	[91/04/01            rvb]
 * 	We will now use gcc routines for copy/zero.
 * 	[91/04/01            rvb]
 * 	Add are_interrupts_disabled for the debugger.
 * 	[91/03/12            danner]
 * 
 * 	add simple_lock_held for debugger
 * 	[91/03/12            danner]
 * 
 * 	fixed a bcnd statement that use ne as the test instead of ne0
 * 	[91/03/07            danner]
 * 
 * 	Replaced \\ with NEWLINE for ENTRY macro
 * 	[91/03/07            danner]
 * 
 * Revision 2.2  91/04/05  13:55:51  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 * Revision 2.1  91/03/06  15:03:26  rvb
 * my changes to omron for X134
 * 
 * Revision 2.2  90/09/14  02:55:58  jfriedl
 *  Omron Luna/88k MACH Release 1.00 [release name version_1_00]
 * 
 * Revision 2.0  90/08/28  13:05:47  jfriedl
 * about to redo exception handler
 * 
 * Revision 1.7  90/08/20  13:41:12  jfriedl
 * using Don's new addupc again
 * 
 * Revision 1.6  90/08/16  09:24:45  jfriedl
 * brought back to Nagaoka from K.A.B.A. -- checkin
 * 
 * Revision 1.5  90/08/09  17:15:24  jfriedl
 * for some reason, simple_lock doesn't work when the temp variable
 * is r3 instead of r10.  I'll have to check out why later....
 * 
 * Revision 1.3  90/08/09  14:11:14  jfriedl
 * cleaned up the simple_lock stuff (with and without debugging).
 * Also prettied up some of the code...
 * 
 *    Revision 1.15  90/06/11  09:28:55  don
 *    deleted some DON_DEBUG code in instruction access exception
 *    
 *    Revision 1.14  90/05/07  17:03:06  jfriedl
 *    finally added a real, working bcopy().
 *    About 5% of the calls to the kernel bcopy have DEST > SRC,
 *    and for these cases it's not optimized at all (for these
 *    cases, the length is usually 1000+ and the addresses are
 *    aligned to an 8-byte boundary -- yet I only byte-copy right
 *    now).  When it's more worth it, someone can optimize.....
 *    
 *    Revision 1.13  90/05/07  09:30:15  yama
 *    bcopy had a bug. So I made c version bcopy (in machdep.c).
 *    
 *    Revision 1.10  90/04/27  19:06:54  jfriedl
 *    bcopy sometimes copies in the wrong order --- put cheap fix.
 *    Must do properly later.....
 *    
 *    Revision 1.9  90/04/25  17:10:47  jfriedl
 *    got rid of old, errored bzero -- new bzero is there.
 *    
 *    Revision 1.8  90/04/25  17:06:27  jfriedl
 *    Don Harper fixed an ffs bug (was scanning the word in the wrong
 *    order)
 *    
 *    Revision 1.3  90/04/19  10:44:09  jfriedl
 *    Found out today from Motorola about the new bug which explains
 *    the problems we've been having.  As a result, I added the
 *    ERRATA__XXX_USR stuff to enable the kluge (which involves
 *    wrapping any xxx.usr instruction with NOPs).
 *    
 *    Revision 1.2  90/04/18  15:06:11  jfriedl
 *    after debugging new processor. Now has kluges for new processors.
 *
 **************************************************************RCS*****/

#ifndef ASSEMBLER
# define ASSEMBLER
#endif

#include <m88k/asm.h>
#include <luna88k/locore.h>
#include <luna88k/exvec.h>
#include <motorola/m88k/m88100/exvec.h>
#include <motorola/m88k/trap.h>


#undef ENTRY 	/* don't want anything to do with a G?PROF ENTRY() */
#ifdef __STDC__
#  define ENTRY(name)	align 4 NEWLINE _ ## name: global _ ## name
#else
#  define ENTRY(name)	align 4 NEWLINE _/**/name: global _/**/name
#endif


/*****************************************************************************
 * DO_LOAD_ADDRESS
 *
 * 	unsigned int do_load_word(address, supervisor_mode, little_endian)
 *	vm_offset_t address;		\\ in r2
 *	boolean_t supervisor_mode;	\\ in r3
 *	boolean_t little_endian;	\\ in r3
 *
 * Return the word at ADDRESS (from user space if SUPERVISOR_MODE is zero,
 * supervisor space if non-zero).
 *
 * If LITTLE_ENDIAN is true, do the access as a little_endian access.
 */

ENTRY(do_load_word)	/* do_load_word(address, supervisor, endian) */
	bcnd	eq0,r4,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r3,2f
#if  ERRATA__XXX_USR
	NOP
	ld.usr	r2,r2,r0
	NOP
	NOP
	NOP
#else
	ld.usr	r2,r2,r0
#endif
	br	3f
2:	ld	r2,r2,r0
3:	bcnd	eq0,r4,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_load_half)	/* do_load_half(address, supervisor, endian) */
	bcnd	eq0,r4,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r3,2f
#if  ERRATA__XXX_USR
	NOP
	ld.h.usr	r2,r2,r0
	NOP
	NOP
	NOP
#else
	ld.h.usr	r2,r2,r0
#endif
	br	3f
2:	ld.h	r2,r2,r0
3:	bcnd	eq0,r4,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_load_byte)	/* do_load_byte(address, supervisor, endian) */
	bcnd	eq0,r4,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r3,2f
#if  ERRATA__XXX_USR
	NOP
	ld.b.usr	r2,r2,r0
	NOP
	NOP
	NOP
#else
	ld.b.usr	r2,r2,r0
#endif
	br	3f
2:	ld.b	r2,r2,r0
3:	bcnd	eq0,r4,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_store_word)	/* do_store_word(address, data, supervisor, endian) */
	bcnd	eq0,r5,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r4,2f
#if  ERRATA__XXX_USR
	NOP
	st.usr	r3,r2,r0
	NOP
	NOP
	NOP
#else
	st.usr	r3,r2,r0
#endif
	br	3f
2:	st	r3,r2,r0
3:	bcnd	eq0,r5,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_store_half)	/* do_store_half(address, data, supervisor, endian) */
	bcnd	eq0,r5,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r4,2f
#if  ERRATA__XXX_USR
	NOP
	st.h.usr	r3,r2,r0
	NOP
	NOP
	NOP
#else
	st.h.usr	r3,r2,r0
#endif
	br	3f
2:	st.h	r3,r2,r0
3:	bcnd	eq0,r5,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_store_byte)	/* do_store_byte(address, data, supervisor, endian) */
	bcnd	eq0,r5,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r4,2f
#if  ERRATA__XXX_USR
	NOP
	st.b.usr	r3,r2,r0
	NOP
	NOP
	NOP
#else
	st.b.usr	r3,r2,r0
#endif
	br	3f
2:	st.b	r3,r2,r0
3:	bcnd	eq0,r5,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_xmem_word)	/* do_xmem_word(address, data, supervisor, endian) */
	bcnd	eq0,r5,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r4,2f
#if  ERRATA__XXX_USR
	NOP
	xmem.usr	r3,r2,r0
	NOP
	NOP
	NOP
#else
	xmem.usr	r3,r2,r0
#endif
	br	3f
2:	xmem	r3,r2,r0
3:	or	r2,r3,r0
	bcnd	eq0,r5,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

ENTRY(do_xmem_byte)	/* do_xmem_byte(address, data, supervisor, endian) */
	bcnd	eq0,r5,1f
	ldcr	r10,PSR		/* emulate little endian */
	set	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
1:	bcnd	ne0,r4,2f
#if  ERRATA__XXX_USR
	NOP
	xmem.bu.usr	r3,r2,r0
	NOP
	NOP
	NOP
#else
	xmem.bu.usr	r3,r2,r0
#endif
	br	3f
2:	xmem.bu	r3,r2,r0
3:	or	r2,r3,r0
	bcnd	eq0,r5,4f
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_BIG_ENDIAN_MODE>
	stcr	r10,PSR
4:	jmp	r1

/*************************************************************************
 *************************************************************************
 **
 ** void set_cpu_number(unsigned number);
 **
 ** Sets the kernel cpu number for this cpu to the given value.
 **
 ** Input:
 **	r1	return address
 **	r2 	the number (should be 0, 1, 2, or 3).
 **
 ** Other registers used:
 **	r3	temp
 **	r4	original PSR
 **	r5	temporary new PSR
 **
 ** Output:
 **	none
 **/
ENTRY(set_cpu_number)
	/* make sure the CPU number is valid */
	clr	r3, r2, FLAG_CPU_FIELD_WIDTH<0>
	bcnd	ne0, r3, 1f	/* bad cpu number */

	/* going to change a control register -- disable interrupts */
	ldcr	r4, PSR
	set	r5, r4, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r5, PSR
	tcnd	ne0,r0,EVN_ERROR /* make sure interrupts are really disabled */

	/* put in the cpu number */
	ldcr	r3, SR1				/* get the flags */
	clr	r3, r3, FLAG_CPU_FIELD_WIDTH<0>	/* clean the slate */
	or	r3, r3, r2			/* add the cpu number */
	stcr	r3, SR1				/* put back */
	
	/* put back the PSR to what it was before and return */
	stcr	r4, PSR
	jmp	r1

     1: /* bad cpu number*/
	or.u	r2, r0, hi16(1f)
	bsr.n	_panic
	or	r2, r2, lo16(1f)
	1: string "bad CPU number\0"
	align   4
	/* will not return */

#if 0
/*************************************************************************
 *************************************************************************
 **
 ** 	int cpu_number(void)
 **
 **	Returns the number of the current cpu. ifdefd out because everyone
 **     should be using the macro form in luna88k/cpu.h
 **/
#undef cpu_number
ENTRY(cpu_number)
    ldcr   r2, SR1
    jmp.n  r1
    clr	   r2, r2, FLAG_CPU_FIELD_WIDTH	/* clears all but the CPU num */
#endif

       data

/*
 * reserve NCPUS words for lockinit and lockeach;
 * counters for simple_lock_init calls and lock acquisition calls.
 */
LABEL(lockinit)
       zero 4*NCPUS

LABEL(lockuse)
       zero 4*NCPUS

LABEL(lockpause)
       zero 4*NCPUS

       text

/*************************************************************************/
/******************  SIMPLE LOCK OPERATIONS  *****************************/
/*************************************************************************/

/*************************************************************
 *************************************************************
 **
 **  	void simple_lock_init(int *lock_data)
 **	{
 **		*lock_data = 0;
 **	}
 **
 **  	void simple_unlock(simple_lock_t *)
 **	{
 **		*lock_data = 0;
 **	}
 **/
#undef simple_unlock
ENTRY(simple_lock_init)
	st      r0, r2, 0               /* init */
        ldcr    r2, SR1                               /* extract cpu number*/
        clr     r2, r2, 0<FLAG_CPU_FIELD_WIDTH>

        mask     r2, r2, 3               /* play it safe */
        or.u    r3, r0, hi16(lockinit)
        or      r3, r3, lo16(lockinit)
        ld      r4, r3[r2]
        addu    r4, r4, 1
        jmp.n   r1
        st      r4, r3[r2]
        
ENTRY(simple_unlock)
	jmp.n	r1
	st	r0, r2, 0

#if MACH_KDB
/* version of simple_unlock for the debugger - should be identical to
   simple_unlock, but should never have breakpoints inserted on it */
ENTRY(db_simple_unlock)
	jmp.n	r1
	st	r0, r2, 0
#endif       

/**
 **     Simple_lock
 **
 **	Returns when the lock is taken. It also increments lockuse[cpu]
 **/
  ENTRY(simple_lock)
      	/* do r3 = test_and_set(r2, 1) */
	or	r3, r0, 1
	xmem	r3, r2, r0
	bcnd	ne0, r3, 1f
#if 0
        ldcr    r5, SR1                               /* extract cpu number */
        clr     r5, r5, 0<FLAG_CPU_FIELD_WIDTH>
        mask    r5, r5, 3               /* play it safe */

        or.u    r3, r0, hi16(lockuse)
        or      r3, r3, lo16(lockuse)
        ld      r4, r3[r5]
        addu    r4, r4, 1
        st      r4, r3[r5]
#endif
        jmp     r1
	
    1:
	/* wait until the lock appears to be free */
        or.u	r4, r0, 0x0300
   2:
        subu	r4, r4, 1
        bcnd	eq0, r4, _simple_lock_too_long

    	ld	r3, r2, 0       /* check lock */
	bcnd	ne0, r3, 2b
	br	_simple_lock	/* looks free... check again with the xmem */

ENTRY(simple_lock_too_long)
#ifdef JEFF_DEBUG
	/* just want to break here.... */
	tb0 0, r0 , 0x84 /* gimmeabreak */
#else
	/* waited too long */
	subu r31, r31, 0x40
	st    r1, r31, 0x30
	st   r30, r31, 0x34
	st    r2, r31, 0x38
	or    r3, r0, r1
	bsr _report_lock_info
	ld    r2, r31, 0x38
	ld   r30, r31, 0x34
	ld    r1, r31, 0x30
	addu r31, r31, 0x40
	br   _simple_lock
#endif /* JEFF_DEBUG */


#if MACH_KDB
/*
 * Version of simple_lock for the kernel debugger; should never have
 * breakpoints set on it. Should be kept consistent with simple_lock.
 */
  ENTRY(db_simple_lock)
    	/* do r10 = test_and_set(r2, 1) */
	or	r10, r0, 1
	xmem	r10, r2, r0
	bcnd	ne0, r10, db_simple_lock_watch

        ldcr    r2, SR1                               /* extract cpu number*/
        clr     r2, r2, 0<FLAG_CPU_FIELD_WIDTH>
        mask     r2, r2, 3               /* play it safe*/

        or.u    r3, r0, hi16(lockuse)
        or      r3, r3, lo16(lockuse)
        ld      r4, r3[r2]
        addu    r4, r4, 1
        jmp.n   r1
        st      r4, r3[r2]
	
   db_simple_lock_watch:
	/* wait until the lock appears to be free */
    3:
	ld	r10, r2, 0
	bcnd	ne0, r10, 3b
	br	_db_simple_lock	/* looks free... check again with the xmem */
#endif /* MACH_KDB */

/*************************************************************
 *************************************************************
 **
 ** 	boolean_t simple_try_lock(simple_lock_t *);
 **
 **	Grab the lock if it's free.  Return zero if the lock was
 **	busy, non-zero if the lock has been taken.
 **/
  ENTRY(simple_lock_try)
	or	r3, r0, 1	/* r3 := test_and_set(r2, 1) */
	xmem	r3, r2, r0
	/*  If r3 is now zero, we hold the lock -- return non-zero.  */
	/* If r3 is now one,  we didn't get it -- return zero.	     */
	/* Thus, we want to "return(!r3)"			     */
	cmp	r4, r3, r0
	jmp.n	r1
	extu	r2, r4, 1<2>

#if MACH_KDB
/* version for the kernel debugger - keep consistent with above */
ENTRY(db_simple_lock_try)
	or	r3, r0, 1	/* r3 := test_and_set(r2, 1) */
	xmem	r3, r2, r0
	/*  If r3 is now zero, we hold the lock -- return non-zero.  */
	/* If r3 is now one,  we didn't get it -- return zero.	     */
	/* Thus, we want to "return(!r3)"			     */
	cmp	r4, r3, r0
	jmp.n	r1
	extu	r2, r4, 1<2>
#endif

#if MACH_KDB
/* version for the debugger */
  ENTRY(db_simple_lock_held)
        jmp.n  r1
        ld     r2, r2, 0
#endif

/*
 * void simple_lock_pause(void). 
 *
 * This routine is called when we find a simple lock held that we wish to
 * acquire, but cannot spin on because we are holding a lock that is in the
 * wrong order to it with respect to the locking hierarchy. Once we drop the
 * lock we are holding, however, we cannot assume the lock we were trying to 
 * acquire is not deallocated. Consequently, we drop the lock we are holding 
 * and wait for a while, then retry. This is the wait for a while routine. 
 *
 * We define a array of counters[cpus], lockpause
 * to tell use how many times this routine is called.
 * We currently wait 128 cycles per call.
 */

ENTRY(simple_lock_pause)
        ldcr    r2, SR1                               /* extract cpu number*/
        clr     r2, r2, FLAG_CPU_FIELD_WIDTH
        mask     r2, r2, 3               	      /* play it safe */
        or.u    r3, r0, hi16(lockpause)
        or      r3, r3, lo16(lockpause)
        ld      r4, r3[r2]
        or	r5, r0, 128 			      /* initialize counters*/
1:	subu	r5, r5, 1                             /* count down */
  	bcnd	ne0, r5, 1b
        addu    r4, r4, 1
	jmp.n	r1				      /* return*/
  	st	r4, r3[r2]

#if 0
/*************************************************************************
 *************************************************************************
 **
 **	void enable_interrupt(void)
 **
 **	Enables processor interrupts (for the executing cpu).
 **/
#undef enable_interrupt
ENTRY(enable_interrupt)
	ldcr	r2, PSR
	clr	r2, r2, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r2, PSR
        FLUSH_PIPELINE
	jmp	r1
#endif

#if 0
#if MACH_KDB
/* a version of enable_interrupt for the debugger; should never
   have breakpoints set it in. Keep it consistent with enable
   interrupt above */
ENTRY(db_enable_interrupt)
	ldcr	r2, PSR
	clr	r2, r2, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r2, PSR
        FLUSH_PIPELINE
	jmp	r1
#endif
#endif

#if 0
/*************************************************************************
 *************************************************************************
 **
 ** unsigned long disable_interrupt(void)
 **
 ** Disables processor interrupts (for the executing CPU) and returns
 ** the *previous* PSR.
 **
 **     if ((oldPSR & 0x02) == 0)
 **		interrupts_were_previously_on = 1;
 **/
#undef disable_interrupt
ENTRY(disable_interrupt)
	ldcr	r2, PSR
	set	r3, r2, 1<PSR_INTERRUPT_DISABLE_BIT>	 /* set disable bit*/
	stcr	r3, PSR
        FLUSH_PIPELINE
	jmp	r1
#endif


#if 0
/* a version of disable_interrupt for the kernel debugger. Should never
   have breakpoints set in it. Make sure it stays consistent with 
   disable_interrupt */

#if MACH_KDB
ENTRY(db_disable_interrupt)
	ldcr	r2, PSR
	set	r3, r2, 1<PSR_INTERRUPT_DISABLE_BIT>	 /* set disable bit*/
	stcr	r3, PSR
        FLUSH_PIPELINE
	jmp	r1
#endif
#endif

/* version for the debugger */

#if MACH_KDB

ENTRY(db_are_interrupts_disabled)
     ldcr	r2, PSR                   /* get the processor status word */
     set  	r3, r0, 1<PSR_INTERRUPT_DISABLE_BIT>   /* set mask */
     jmp.n 	r1                                     /* delayed return */
     and 	r2, r2, r3                            /* r2 = r3 & r2 */
#endif
    
/*************************************************************************
 *************************************************************************
 **
 **  void set_current_thread(thread_t thread);
 **
 **  Sets the "current thread" for the executing cpu to the given thread.
 **/
ENTRY(set_current_thread)
	jmp.n	r1
	stcr	r2,SR0


LABEL(_FAULT_ERROR)
	or	r2,r0,1		/* bad copy */
	jmp	r1

LABEL(_ALLOW_FAULT_START)

/*
 * Copy specified amount of data from user space into the kernel
 * copyin(from, to, len)
 *	r2 == from (user source address)
 *	r3 == to (kernel destination address)
 *	r4 == length
 * (r1=return addr)
 */

#define SRC  	r2
#define DEST  	r3
#define LEN  	r4

ENTRY(copyin)
ENTRY(copyinmsg)
	/* If it's a small length (less than 8), then do byte-by-byte */
	cmp		r9,   LEN,  8
	bb1		lt,   r9,   copyin_byte_only

	/* If they're not aligned similiarly, use byte only... */
	xor		r9,   SRC,  DEST
	mask		r8,   r9,   0x3
	bcnd		ne0,  r8,   copyin_byte_only

	/*
	 * At this point, we don't know if they're word aligned or not,
	 * but we know that what needs to be done to one to align
	 * it is what's needed for the other.
	 */
	bb1		0,    SRC,  copyin_left_align_to_halfword
copyin_left_aligned_to_halfword:
	bb1		1,    SRC,  copyin_left_align_to_word
copyin_left_aligned_to_word:
	bb1		0,    LEN,  copyin_right_align_to_halfword
copyin_right_aligned_to_halfword:
	bb1		1,    LEN,  copyin_right_align_to_word
copyin_right_aligned_to_word:

	/* At this point, both SRC and DEST are aligned to a word */
	/* boundry, and LEN is an even multiple of 4.             */
	bb1.n		2,    LEN,  copyin_right_align_to_doubleword
	or		r7,   r0,   4

copyin_right_aligned_to_doubleword:
#if  ERRATA__XXX_USR
	NOP
	ld.usr 		r5,   SRC,  r0
	NOP
	NOP
	NOP
	ld.usr    	r6,   SRC,  r7
	NOP
	NOP
	NOP
#else
	ld.usr 		r5,   SRC,  r0
	ld.usr    	r6,   SRC,  r7
#endif
	subu		LEN,  LEN,  8
	st		r5,   DEST, r0
	addu		SRC,  SRC,  8
	st		r6,   DEST, r7
	bcnd.n		ne0,  LEN,  copyin_right_aligned_to_doubleword
	addu		DEST, DEST, 8
   	or		r2, r0, r0	/* successful return */
	jmp		r1

        /***************************************************/

copyin_left_align_to_halfword:
#if  ERRATA__XXX_USR
	NOP
	ld.b.usr   	r5,   SRC,  r0
	NOP
	NOP
	NOP
#else
	ld.b.usr   	r5,   SRC,  r0
#endif
	subu		LEN,  LEN,  1
	st.b		r5,   DEST, r0
	addu		SRC,  SRC,  1
	br.n		copyin_left_aligned_to_halfword
	addu		DEST, DEST, 1

copyin_left_align_to_word:
#if  ERRATA__XXX_USR
	NOP
	ld.h.usr   	r5,   SRC,  r0
	NOP
	NOP
	NOP
#else
	ld.h.usr   	r5,   SRC,  r0
#endif
	subu      	LEN,  LEN,  2
	st.h		r5,   DEST, r0
	addu		SRC,  SRC,  2
	br.n		copyin_left_aligned_to_word
	addu		DEST, DEST, 2

copyin_right_align_to_halfword:
	subu		LEN,  LEN,  1
#if  ERRATA__XXX_USR
	NOP
	ld.b.usr   	r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.b.usr   	r5,   SRC,  LEN
#endif
	br.n		copyin_right_aligned_to_halfword
	st.b		r5,   DEST, LEN

copyin_right_align_to_word:
	subu		LEN,  LEN,  2
#if  ERRATA__XXX_USR
	NOP
	ld.h.usr   	r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.h.usr   	r5,   SRC,  LEN
#endif
	br.n		copyin_right_aligned_to_word
	st.h		r5,   DEST, LEN

copyin_right_align_to_doubleword:
	subu		LEN,  LEN,  4
#if  ERRATA__XXX_USR
	NOP
	ld.usr		r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.usr		r5,   SRC,  LEN
#endif
	bcnd.n		ne0,  LEN, copyin_right_aligned_to_doubleword
	st		r5,   DEST, LEN
   	or		r2, r0, r0	/* successful return */
	jmp		r1

copyin_byte_only:
	bcnd		eq0, LEN, 2f
   1:
	subu		LEN, LEN, 1
#if  ERRATA__XXX_USR
	NOP
	ld.b.usr   	r5, SRC, LEN
	NOP
	NOP
	NOP
#else
	ld.b.usr   	r5, SRC, LEN
#endif
	bcnd.n		ne0, LEN, 1b
	st.b		r5, DEST, LEN
   2:	or		r2, r0, r0	/* successful return */
	jmp		r1

#undef SRC
#undef DEST
#undef LEN
/*######################################################################*/
/*######################################################################*/

/*
 * Copy specified amount of data from kernel to the user space
 * Copyout(from, to, len)
 *	r2 == from (kernel source address)
 *	r3 == to (user destination address)
 *	r4 == length
 */

#define SRC  	r2
#define DEST  	r3
#define LEN  	r4

ENTRY(copyout)
ENTRY(copyoutmsg)
	/* If it's a small length (less than 8), then do byte-by-byte */
	cmp		r9,   LEN,  8
	bb1		lt,   r9,   copyout_byte_only

	/* If they're not aligned similiarly, use byte only... */
	xor		r9,   SRC,  DEST
	mask		r8,   r9,   0x3
	bcnd		ne0,  r8,   copyout_byte_only

	/*
	 * At this point, we don't know if they're word aligned or not,
	 * but we know that what needs to be done to one to align
	 * it is what's needed for the other.
	 */
	bb1		0,    SRC,  copyout_left_align_to_halfword
copyout_left_aligned_to_halfword:
	bb1		1,    SRC,  copyout_left_align_to_word
copyout_left_aligned_to_word:
	bb1		0,    LEN,  copyout_right_align_to_halfword
copyout_right_aligned_to_halfword:
	bb1		1,    LEN,  copyout_right_align_to_word
copyout_right_aligned_to_word:

	/*
	 * At this point, both SRC and DEST are aligned to a word
	 * boundry, and LEN is an even multiple of 4.
	 */
	bb1.n		2,    LEN,  copyout_right_align_to_doubleword
	or		r7,   r0,   4

copyout_right_aligned_to_doubleword:
	ld 		r5,   SRC,  r0
	ld    		r6,   SRC,  r7
	subu		LEN,  LEN,  8
#if  ERRATA__XXX_USR
	NOP
	st.usr		r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.usr		r5,   DEST, r0
#endif
	addu		SRC,  SRC,  8
#if  ERRATA__XXX_USR
	NOP
	st.usr		r6,   DEST, r7
	NOP
	NOP
	NOP
#else
	st.usr		r6,   DEST, r7
#endif
	bcnd.n		ne0,  LEN,  copyout_right_aligned_to_doubleword
	addu		DEST, DEST, 8
   	or		r2, r0, r0	/* successful return */
	jmp		r1

	/***************************************************/
copyout_left_align_to_halfword:
	ld.b   		r5,   SRC,  r0
	subu		LEN,  LEN,  1
#if  ERRATA__XXX_USR
	NOP
	st.b.usr	r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.b.usr	r5,   DEST, r0
#endif
	addu		SRC,  SRC,  1
	br.n		copyout_left_aligned_to_halfword
	addu		DEST, DEST, 1

copyout_left_align_to_word:
	ld.h   		r5,   SRC,  r0
	subu      	LEN,  LEN,  2
#if  ERRATA__XXX_USR
	NOP
	st.h.usr	r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.h.usr	r5,   DEST, r0
#endif
	addu		SRC,  SRC,  2
	br.n		copyout_left_aligned_to_word
	addu		DEST, DEST, 2

copyout_right_align_to_halfword:
	subu		LEN,  LEN,  1
	ld.b   		r5,   SRC,  LEN
#if  ERRATA__XXX_USR
	NOP
	st.b.usr	r5,   DEST, LEN
	NOP
	NOP
	NOP
	br		copyout_right_aligned_to_halfword
#else
	br.n		copyout_right_aligned_to_halfword
	st.b.usr	r5,   DEST, LEN
#endif

copyout_right_align_to_word:
	subu		LEN,  LEN,  2
	ld.h   		r5,   SRC,  LEN
#if  ERRATA__XXX_USR
	NOP
	st.h.usr	r5,   DEST, LEN
	NOP
	NOP
	NOP
	br		copyout_right_aligned_to_word
#else
	br.n		copyout_right_aligned_to_word
	st.h.usr	r5,   DEST, LEN
#endif

copyout_right_align_to_doubleword:
	subu		LEN,  LEN,  4
	ld		r5,   SRC,  LEN
#if  ERRATA__XXX_USR
	NOP
	st.usr		r5,   DEST, LEN
	NOP
	NOP
	NOP
	bcnd		ne0,  LEN, copyout_right_aligned_to_doubleword
#else
	bcnd.n		ne0,  LEN, copyout_right_aligned_to_doubleword
	st.usr		r5,   DEST, LEN
#endif
   	or		r2, r0, r0	/* successful return */
	jmp		r1

_LABEL(copyout_byte_only)
	bcnd		eq0, LEN, 2f
   1:
	subu		LEN, LEN, 1
	ld.b   		r5, SRC, LEN
#if  ERRATA__XXX_USR
	NOP
	st.b.usr	r5, DEST, LEN
	NOP
	NOP
	NOP
	bcnd		ne0, LEN, 1b
#   else
	bcnd.n		ne0, LEN, 1b
	st.b.usr	r5, DEST, LEN
#   endif

   2:	or		r2, r0, r0	/* successful return */
	jmp		r1

#undef SRC
#undef DEST
#undef LEN

/*######################################################################*/
LABEL(_ALLOW_FAULT_END)
word 0	/* to separate from routine below */
/*######################################################################*/

/*
 * Gcc 2 generates calls to memcpy for bcopies of unknown size. memcpy
 * can simply be implemented as ovbcopy but the src (r2, r3) and dst args need to
 * be switched. 
 */
/*
 * void memcpy(dest, source, count)
 *
 */
ENTRY(memcpy)
 	or 	r5, r0, r2    /* dst -> tmp */
	or	r2, r0, r3    /* src -> 1st arg */
	br.n	_ovbcopy      /* call ovbcopy */
       	or	r3, r0, r5    /* dst -> 2nd arg */
	
       
/*
 * void bcopy(source, destination, count)
 *
 * copy count bytes of data from source to destination
 * Don Harper (don@omron.co.jp), Omron Corporation.
 *
 */

ENTRY(bcopy)
ENTRY(ovbcopy)
	bcnd	le0,r4,bcopy_out /* nothing to do if count <= 0 */
/*
 *	check position of source and destination data
 */
	cmp 	r9,r2,r3	/* compare source address to destination */
	bb1	eq,r9,bcopy_out	/* nothing to do if addresses are equal */	
	bb1	lo,r9,bcopy_reverse /* copy in reverse if src < destination */
/*
 *	source address is greater than destination address, copy forward 
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,f_byte_copy /* copy bytes for small data length */ 
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(f_strat) /* forward strategy table address (high) */
	or	r12,r12,lo16(f_strat) /* forward strategy table address (low) */
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */


/*
 * Copy three bytes from src to destination then copy words
 */
_LABEL(f_3byte_word_copy)
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	addu	r2,r2,3		/* increment source pointer */
	addu	r3,r3,3		/* increment destination pointer */
	br.n	f_word_copy	/* copy full words */
	subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
_LABEL(f_1half_word_copy)
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	addu	r2,r2,2		/* increment source pointer */
	addu	r3,r3,2		/* increment destination pointer */
	br.n	f_word_copy	/* copy full words */
	subu	r4,r4,2		/* decrement remaining length */

/* 
 * Copy 1 byte from src to destination then copy words
 */
_LABEL(f_1byte_word_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */	
_LABEL(f_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */	
	bb1	lo,r10,f_byte_copy 	/* not enough left, copy bytes */
	ld	r6,r2,0		/* load first word */
	ld	r7,r2,4		/* load second word */
	ld	r8,r2,8		/* load third word */
	ld	r9,r2,12	/* load fourth word */
	st	r6,r3,0		/* store first word */
	st	r7,r3,4		/* store second word */
	st 	r8,r3,8		/* store third word */
	st 	r9,r3,12	/* store fourth word */
	addu	r2,r2,16	/* increment source pointer */
	addu	r3,r3,16	/* increment destination pointer */
	br.n	f_word_copy	/* branch to copy another block */
	subu	r4,r4,16	/* decrement remaining length */
	
_LABEL(f_1byte_half_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

_LABEL(f_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */	
	bb1	lo,r10,f_byte_copy /* not enough left, copy bytes */
	ld.hu	r6,r2,0		/* load first half-word */
	ld.hu	r7,r2,2		/* load second half-word */
	ld.hu	r8,r2,4		/* load third half-word */
	ld.hu	r9,r2,6		/* load fourth half-word */
	ld.hu	r10,r2,8	/* load fifth half-word */
	ld.hu	r11,r2,10	/* load sixth half-word */
	ld.hu	r12,r2,12	/* load seventh half-word */
	ld.hu	r13,r2,14	/* load eighth half-word */
	st.h	r6,r3,0		/* store first half-word */
	st.h	r7,r3,2		/* store second half-word */
	st.h 	r8,r3,4		/* store third half-word */
	st.h 	r9,r3,6		/* store fourth half-word */
	st.h	r10,r3,8	/* store fifth half-word */
	st.h	r11,r3,10	/* store sixth half-word */
	st.h 	r12,r3,12	/* store seventh half-word */
	st.h 	r13,r3,14	/* store eighth half-word */
	addu	r2,r2,16	/* increment source pointer */
	addu	r3,r3,16	/* increment destination pointer */
	br.n	f_half_copy	/* branch to copy another block */
	subu	r4,r4,16	/* decrement remaining length */
	
_LABEL(f_byte_copy)
	bcnd	eq0,r4,bcopy_out /* branch if nothing left to copy */ 
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	br.n	f_byte_copy	/* branch for next byte */ 
	subu	r4,r4,1		/* decrement remaining length */

/*
 *	source address is less than destination address, copy in reverse
 */
_LABEL(bcopy_reverse)
/*
 * start copy pointers at end of data 
 */
	addu	r2,r2,r4	/* start source at end of data */
	addu	r3,r3,r4	/* start destination at end of data */
/*
 * check for short data
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,r_byte_copy /* copy bytes for small data length */ 
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(r_strat) /* reverse strategy table address (high) */
	or	r12,r12,lo16(r_strat) /* reverse strategy table address (low) */
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */

/*
 * Copy three bytes from src to destination then copy words
 */
_LABEL(r_3byte_word_copy)
	subu	r2,r2,3		/* decrement source pointer */
	subu	r3,r3,3		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	br.n	r_word_copy	/* copy full words */
	subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
_LABEL(r_1half_word_copy)
	subu	r2,r2,2		/* decrement source pointer */
	subu	r3,r3,2		/* decrement destination pointer */
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	br.n	r_word_copy	/* copy full words */
	subu	r4,r4,2		/* decrement remaining length */

/* 
 * Copy 1 byte from src to destination then copy words
 */
_LABEL(r_1byte_word_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */	
_LABEL(r_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */	
	bb1	lo,r10,r_byte_copy 	/* not enough left, copy bytes */
	subu	r2,r2,16	/* decrement source pointer */
	subu	r3,r3,16	/* decrement destination pointer */
	ld	r6,r2,0		/* load first word */
	ld	r7,r2,4		/* load second word */
	ld	r8,r2,8		/* load third word */
	ld	r9,r2,12	/* load fourth word */
	st	r6,r3,0		/* store first word */
	st	r7,r3,4		/* store second word */
	st 	r8,r3,8		/* store third word */
	st 	r9,r3,12	/* store fourth word */
	br.n	r_word_copy	/* branch to copy another block */
	subu	r4,r4,16	/* decrement remaining length */
	
_LABEL(r_1byte_half_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

_LABEL(r_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */	
	bb1	lo,r10,r_byte_copy /* not enough left, copy bytes */
	subu	r2,r2,16	/* decrement source pointer */
	subu	r3,r3,16	/* decrement destination pointer */
	ld.hu	r6,r2,0		/* load first half-word */
	ld.hu	r7,r2,2		/* load second half-word */
	ld.hu	r8,r2,4		/* load third half-word */
	ld.hu	r9,r2,6		/* load fourth half-word */
	ld.hu	r10,r2,8	/* load fifth half-word */
	ld.hu	r11,r2,10	/* load sixth half-word */
	ld.hu	r12,r2,12	/* load seventh half-word */
	ld.hu	r13,r2,14	/* load eighth half-word */
	st.h	r6,r3,0		/* store first half-word */
	st.h	r7,r3,2		/* store second half-word */
	st.h 	r8,r3,4		/* store third half-word */
	st.h 	r9,r3,6		/* store fourth half-word */
	st.h	r10,r3,8	/* store fifth half-word */
	st.h	r11,r3,10	/* store sixth half-word */
	st.h 	r12,r3,12	/* store seventh half-word */
	st.h 	r13,r3,14	/* store eighth half-word */
	br.n	r_half_copy	/* branch to copy another block */
	subu	r4,r4,16	/* decrement remaining length */
	
_LABEL(r_byte_copy)
	bcnd	eq0,r4,bcopy_out /* branch if nothing left to copy */ 
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	br.n	r_byte_copy	/* branch for next byte */ 
	subu	r4,r4,1		/* decrement remaining length */

_LABEL(bcopy_out)
	jmp	r1		/* all done, return to caller */
	
	data
	align	4
_LABEL(f_strat)
	word	f_word_copy
	word	f_byte_copy
	word	f_half_copy
	word	f_byte_copy
	word	f_byte_copy
	word	f_3byte_word_copy
	word	f_byte_copy
	word	f_1byte_half_copy
	word	f_half_copy
	word	f_byte_copy
	word	f_1half_word_copy
	word	f_byte_copy
	word	f_byte_copy
	word	f_1byte_half_copy
	word	f_byte_copy
	word	f_1byte_word_copy

_LABEL(r_strat)
	word	r_word_copy
	word	r_byte_copy
	word	r_half_copy
	word	r_byte_copy
	word	r_byte_copy
	word	r_1byte_word_copy
	word	r_byte_copy
	word	r_1byte_half_copy
	word	r_half_copy
	word	r_byte_copy
	word	r_1half_word_copy
	word	r_byte_copy
	word	r_byte_copy
	word	r_1byte_half_copy
	word	r_byte_copy
	word	r_3byte_word_copy

	text

/*######################################################################*/
/*######################################################################*/

/*
 * April 1990, Omron Corporation
 * jfriedl@nff.ncl.omron.co.jp
 *
 * void bzero(destination, length)
 *
 * Clear (set to zero) LENGTH bytes of memory starting at DESTINATION.
 * Note that there is no return value.
 *
 * This is fast. Really fast. Especially for long lengths.
 */
#define R_dest			r2
#define R_len			r3

#define R_bytes			r4
#define R_mark_address		r5
#define R_addr			r6	/* R_addr && R_temp SHARE */
#define R_temp			r6	/* R_addr && R_temp SHARE */


ENTRY(blkclr)
ENTRY(bzero)
	/*
	 * If the destination is not word aligned, we'll word align
	 * it first to make things easier.
	 *
	 * We'll check to see first if bit #0 is set and then bit #1
	 * (of the destination address). If either are set, it's
	 * not word aligned.
	 */
	bb1	0, R_dest, not_initially_word_aligned
	bb1	1, R_dest, not_initially_word_aligned

   now_word_aligned:
	/*
	 * before we get into the main loop, grab the
	 * address of the label "mark" below.
	 */
	or.u	R_mark_address, r0, hi16(mark)
	or	R_mark_address, R_mark_address, lo16(mark)
	
   top_of_main_loop:
#	define MAX_AT_ONE_TIME 128
	/*
	 * Now we find out how many words we can zero-fill in a row.
	 * We do this by doing something like:
 	 *
	 *	bytes &= 0xfffffffc;
	 *	if (bytes > MAX_AT_ONE_TIME)
	 *		bytes = MAX_AT_ONE_TIME;
	 */

	/*
	 * Clear lower two bits of length to give us the number of bytes
	 * ALIGNED TO THE WORD LENGTH remaining to move.
	 */
	clr	R_bytes, R_len, 2<0>

	/* if we're done clearing WORDS, jump out */
	bcnd	eq0, R_bytes, done_doing_words

	/* if the number of bytes > MAX_AT_ONE_TIME, do only the max */
	cmp	R_temp, R_bytes, MAX_AT_ONE_TIME
	bb1	lt, R_temp, 1f

		/*
		 * Since we're doing the max, we know exactly where we're
	 	 * jumping (the first one in the list!), so we can jump
		 * right there.  However, we've still got to adjust
		 * the length, so we'll jump to where we ajust the length
	 	 * which just happens to fall through to the first store zero
		 * in the list.
		 *
	 	 * Note, however, that we're jumping to an instruction that
		 * would be in the delay slot for the jump in front of it,
		 * so if you change things here, WATCH OUT.
		 */
		br.n	do_max
		or	R_bytes, r0, MAX_AT_ONE_TIME
	
     1:

	/*
	 * Now we have the number of bytes to zero during this iteration,
	 * (which, as it happens, is the last iteration if we're here).
	 * We'll calculate the proper place to jump and then jump there,
	 * after adjusting the length.  NOTE that there is a label between
	 * the "jmp.n" and the "subu" below... the "subu" is NOT always
	 * executed in the delay slot of the "jmp.n".
 	 */
	subu	R_addr, R_mark_address, R_bytes

	/* and go there (after adjusting the length via ".n") */
	jmp.n	R_addr
do_max: subu	R_len, R_len, R_bytes	/* NOTE: this is in the delay slot! */

	st	r0, R_dest, 0x7c	/* 128 */
	st	r0, R_dest, 0x78	/* 124 */
	st	r0, R_dest, 0x74	/* 120 */
	st	r0, R_dest, 0x70	/* 116 */
	st	r0, R_dest, 0x6c	/* 112 */
	st	r0, R_dest, 0x68	/* 108 */
	st	r0, R_dest, 0x64	/* 104 */
	st	r0, R_dest, 0x60	/* 100 */
	st	r0, R_dest, 0x5c	/*  96 */
	st	r0, R_dest, 0x58	/*  92 */
	st	r0, R_dest, 0x54	/*  88 */
	st	r0, R_dest, 0x50	/*  84 */
	st	r0, R_dest, 0x4c	/*  80 */
	st	r0, R_dest, 0x48	/*  76 */
	st	r0, R_dest, 0x44	/*  72 */
	st	r0, R_dest, 0x40	/*  68 */
	st	r0, R_dest, 0x3c	/*  64 */
	st	r0, R_dest, 0x38	/*  60 */
	st	r0, R_dest, 0x34	/*  56 */
	st	r0, R_dest, 0x30	/*  52 */
	st	r0, R_dest, 0x2c	/*  44 */
	st	r0, R_dest, 0x28	/*  40 */
	st	r0, R_dest, 0x24	/*  36 */
	st	r0, R_dest, 0x20	/*  32 */
	st	r0, R_dest, 0x1c	/*  28 */
	st	r0, R_dest, 0x18	/*  24 */
	st	r0, R_dest, 0x14	/*  20 */
	st	r0, R_dest, 0x10	/*  16 */
	st	r0, R_dest, 0x0c	/*  12 */
	st	r0, R_dest, 0x08	/*   8 */
	st	r0, R_dest, 0x04	/*   4 */
	st	r0, R_dest, 0x00	/*   0 */

   mark:
	br.n	top_of_main_loop
	addu	R_dest, R_dest, R_bytes	/* bump up the dest address */



   done_doing_words:
	bcnd	ne0, R_len, finish_up_last_bytes
	jmp	r1		/* RETURN */

   finish_up_last_bytes:
	subu	R_len, R_len, 1
	bcnd.n	ne0, R_len, finish_up_last_bytes
	st.b	r0, R_dest, R_len

   leave:
	jmp	r1		/* RETURN */
	
   not_initially_word_aligned:
	/*
	 * Bzero to word-align the address (at least if the length allows it).
	 */
	bcnd	eq0, R_len, leave
	st.b	r0, R_dest, 0
	addu	R_dest, R_dest, 1
	mask	R_temp, R_dest, 0x3
	bcnd.n	eq0, R_temp, now_word_aligned
	subu	R_len, R_len, 1
	br	not_initially_word_aligned

#undef  R_dest
#undef  R_len
#undef  R_bytes
#undef  R_mark_address
#undef  R_addr
#undef  R_temp
#undef  MAX_AT_ONE_TIME

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/*
 * non-local goto
 */
ENTRY(_setjmp)
	st	r1,r2,0
	st	r14,r2,4
	st	r15,r2,2*4
	st	r16,r2,3*4
	st	r17,r2,4*4
	st	r18,r2,5*4
	st	r19,r2,6*4
	st	r20,r2,7*4
	st	r21,r2,8*4
	st	r22,r2,9*4
	st	r23,r2,10*4
	st	r24,r2,11*4
	st	r25,r2,12*4
	st	r26,r2,13*4
	st	r27,r2,14*4
	st	r28,r2,15*4
	st	r29,r2,16*4
	st	r30,r2,17*4
	st	r31,r2,18*4
	jmp.n	r1
	or	r2,r0,r0

ENTRY(_longjmp)
	ld	r1,r2,0
	ld	r14,r2,4
	ld	r15,r2,2*4
	ld	r16,r2,3*4
	ld	r17,r2,4*4
	ld	r18,r2,5*4
	ld	r19,r2,6*4
	ld	r20,r2,7*4
	ld	r21,r2,8*4
	ld	r22,r2,9*4
	ld	r23,r2,10*4
	ld	r24,r2,11*4
	ld	r25,r2,12*4
	ld	r26,r2,13*4
	ld	r27,r2,14*4
	ld	r28,r2,15*4
	ld	r29,r2,16*4
	ld	r30,r2,17*4
	ld	r31,r2,18*4
	jmp.n	r1
	or	r2,r3,r0

ENTRY(longjmp_int_enable)
	ld	r1,r2,0
	ld	r14,r2,4
	ld	r15,r2,2*4
	ld	r16,r2,3*4
	ld	r17,r2,4*4
	ld	r18,r2,5*4
	ld	r19,r2,6*4
	ld	r20,r2,7*4
	ld	r21,r2,8*4
	ld	r22,r2,9*4
	ld	r23,r2,10*4
	ld	r24,r2,11*4
	ld	r25,r2,12*4
	ld	r26,r2,13*4
	ld	r27,r2,14*4
	ld	r28,r2,15*4
	ld	r29,r2,16*4
	ld	r30,r2,17*4
	ld	r31,r2,18*4
	or	r2,r3,r0
	ldcr	r10,PSR
	clr	r10,r10,1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r10,PSR
	jmp	r1

#if MACH_KDB
ENTRY(db_flush_pipeline)
        FLUSH_PIPELINE
	jmp	r1
#endif	

ENTRY(read_processor_identification_register)
     	jmp.n	r1
	ldcr	r2, PID
	
/*
 *	call rom abort (called when non-maskable interrupt detected)
 */
ENTRY(call_rom_abort)
	tcnd	eq0, r0, EVN_ROM_ABORT	/* trap to ROM */
	jmp	r1			/* and return */

;------------------------------------------------------------------------

/* JEFF_DEBUG stuff */
	align 8
raw_xpr_stack_top:
	zero 0x40
raw_xpr_inital_stack_frame:
	zero 0x60
raw_xpr_stack_bottom:

user_raw_xpr: global user_raw_xpr
	ldcr	r5, cr17
	ld	r5, r5, THREAD_TASK
	/* get a stack ... can use r9 no problem */
	or.u	r9, r0, hi16(raw_xpr_inital_stack_frame)
	or	r9, r9, lo16(raw_xpr_inital_stack_frame)
	st	r31, r9, 0x38
	st	r1, r9, 0x3c
	or	r31, r9, r0
	bsr	__raw_xpr
	ld	r1, r31, 0x3c
	ld	r31, r31, 0x38
	rte

_raw_xpr: global _raw_xpr
	or.u	r5, r0, hi16(_kernel_task)
	ld	r5, r5, lo16(_kernel_task)
	br	__raw_xpr
