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
 * HISTORY
 * $Log:	eh.s,v $
 * Revision 2.6  93/03/09  17:56:56  danner
 * 	Renamed user_ksp_valid to exception_handler_has_ksp and moved
 * 	to allow backtrace routines to work past data exceptions.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.5  93/02/04  07:49:20  danner
 * 	luna88k/asm.h -> m88k/asm.h
 * 	[93/02/02            jfriedl]
 * 
 * Revision 2.4  93/01/26  18:03:54  danner
 * 	Replaced ;comments with C-style comments for better CPPification.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:38:25  danner
 * 	Replaced rediculous version of error_handler with something
 * 	that might even work.
 * 	[92/12/07            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:41:39  jfriedl
 * 	_pre_trap --> trap (now handled in trap.c)
 * 	[92/07/30            jfriedl]
 * 
 * 	Moved and renmaed from locore/eh.c.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.8  92/05/21  17:19:04  jfriedl
 * 	Added _badwordaddr(addr) which is same as _badaddr(addr, 4).
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.7  92/05/04  11:26:49  danner
 * 	Remove check_frame calls, clean up error handler.
 * 	[92/05/03            danner]
 * 	Remove debug code.
 * 	[92/04/26            danner]
 * 
 * 	Add save of r14, r15 to handoff_native_syscall.
 * 	[92/04/17            danner]
 * 	Remove debugging cruft.
 * 	[92/04/12            danner]
 * 	Debugging version, for posterity.
 * 	[92/04/12            danner]
 * 	Changed the emulation return to set the SNIP and SFIP to be the
 * 	 first two instructions of the emulation routine. 
 * 	[92/04/08            danner]
 * 
 * Revision 2.6  92/04/01  10:54:47  rpd
 * 	Added save of shadow XIP in syscall_native under MACH_KDB
 * 	 conditionals to support cleaner stack traces.
 * 	[92/03/18            danner]
 * 
 * Revision 2.5  92/03/03  15:38:29  rpd
 * 	Fixes to the syscall_handler. It turns out none of the control
 * 	 registers except SR0-SR3 are read/write; the rest are
 * 	 read and clear. So using DMT0 as a scratch register is out.
 * 	[92/03/02            danner]
 * 
 * Revision 2.4  92/02/19  14:00:35  elf
 * 	Removed mach syscall handler. Removed user_{entry,trace,break}
 * 	 traps. Renumbered entry,trace, and break traps.
 * 	[92/02/13            danner]
 * 
 * 	Under mach_kdb conditionals dump the pointer to the exception
 * 	 twice so the debugger can recognize it.
 * 	[92/01/23            danner]
 * 	Updated to deal with variable emulation vector base (minimum)
 * 	 introduced in MK68.
 * 	[92/01/17            danner]
 * 	Modifed ast call from syscall native return to go to splhigh,
 * 	 turn interrrupts on, then call ast (instead of just disabling
 * 	 interrupts). 
 * 	[91/11/07            danner]
 * 
 * 	Updated to use the 3.0 ast technology, ast_taken instead of
 * 	 calling trap(T_ASTFLT)
 * 	[91/11/03            danner]
 * 	Corrected return from exception handler to work in the case of
 * 	 user mode return (preservation of the stack pointer).
 * 
 * 	     Corrected stack pickup from user mode.
 * 	[91/09/05            danner]
 * 
 * Revision 2.3  91/08/24  12:10:14  af
 * 	Bug fixes.
 * 	[91/07/11            danner]
 * 
 * Revision 2.2.3.1  91/08/19  13:46:40  danner
 * 	Bug fixes.
 * 	[91/07/11            danner]
 * 
 * Revision 2.2  91/07/09  23:17:40  danner
 * 	Reorganized to support stack handoff. All exceptions from user
 * 	 mode now store registers in the pcb in the thread structure.
 * 	 This has increased ugliness, esp. in the case when turning on
 * 	 the FPU causes an exception.
 * 
 * 	Removed stack crawling support because the exception frame is now
 * 	 not always on the stack. Sigh
 * 
 * 	     Still a bug in cleaning up DMT0. 
 * 	[91/07/04            danner]
 * 	Upgraded to last pre_trap and pre_ext_int technology
 * 	[91/05/13            danner]
 * 
 * Revision 2.2  91/04/05  14:03:05  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */
 
 /*
 **************************************************************RCS******
 *
 *  -------------------------------------------------------------------
 * |  In the following discussion, references are made to:             |
 * |          MC88100 - RISC MICROPROCESSOR USER'S MANUAL              |
 * |  (second edition). Reference in []s refer to section numbers.     |
 * |                                                                   |
 * |  This discussion assumes that you are at least vaguely familiar   |
 * |  with 88100 exception handling (chapter 6), the MACH kernel, and  |
 * |  that you have a brain (and use it while reading this).           |
 * |                                                                   |
 * |  I also assume (and hope) that you're not offended by             |
 * |  frequent misspellings.                                           |
 * |                                                                   |
 * |                       Jeffrey Friedl                              |
 * |			   jfriedl@rna.ncl.omron.co.jp		       |
 * |                       December, 1989                              |
 *  -------------------------------------------------------------------
 *
 * EXCEPTIONS, INTERRUPTS, and TRAPS
 * ---------------------------------
 * This is the machine exception handler.
 * In the MC88100, various "conditions" cause an exception, where
 * processing momentarily jumps here to "service" the exception,
 * and then continues where it left off.
 *
 * There are a number of different types of exceptions.
 * For example, exception #6 is the privilege violation exception which
 * is raised when the user tries to execute a supervisor-only instruction.
 *
 * Exception #1 is the interrupt exception, and is raised when an
 * outside device raises the INT line on the CPU.  This happens,
 * for example, when the clock signals that it is time for a context
 * switch, or perhaps the disk drive signaling that some operation
 * is complete.
 *
 * Traps are also exceptions.  Traps are ways for user programs to request
 * kernel operations.  For example, "tcnd eq0, r0, 128" will raise
 * exception 128, the system call exception.
 *
 *
 * SERVICING AN EXCEPTION
 * -----------------------
 * When an exception occurs, each control register is saved in its
 * respective shadow register and execution continues from the
 * appropriate exception handler.  The exception handler must
 *      - save the context from the time of the exception
 *      - service the exception
 *      - restore the context (registers, etc)
 *      - pick up from where the exception occurred.
 *
 * The context is saved on a stack.
 *
 * Servicing the exception is usually straightforward and in fact not dealt
 * with very much here.  Usually a C routine is called to handle it.
 * For example, when a privilege exception is raised, the routine that sends
 * an "illegal instruction" signal to the offending process is called.
 *
 * When the exception has been serviced, the context is restored from the
 * stack and execution resumes from where it left off.
 *
 * In more detail:
 *
 * Saving the exception-time context.
 * ---------------------------------
 *     In saving the exception-time context, we copy the shadow and general
 * purpose registers to memory.  Since one exception may occur while
 * servicing another, the memory used to save the exception-time context may
 * not be static (i.e. the same every time).  Thus, memory on a stack is set
 * aside for the exception frame (area where the exception-time context is
 * saved). The same stack is also used when C routines are called (to
 * service the exception).
 *
 * 	Each thread has a stack in kernel space (called the "kernel stack",
 * short for "thread's kernel stack) as well as the user space stack.  When
 * entering the kernel from user space, the kernel stack is unused.  On this
 * stack we save the exception state and (most likely call a C routine to)
 * service the exception.
 *
 * Before servicing an exception, several issues must be addressed.
 *
 * 1) When an interrupt is recognized by the hardware, the data pipeline is
 *    allowed to clear.  However, if one of these data accesses faults (bad
 *    reference, or a reference to a page which needs to be swapped in), that
 *    reference, as well as any others in the pipeline at the time (at most
 *    three total) are left there, to be taken care of by the exception
 *    handler [6.4.1].  This involves swapping in the proper page and
 *    manually doing the appropriate load or store.
 *
 *    The other (at most, two other) data accesses that might have been in
 *    the pipeline must also be manually completed (even though they may not
 *    be at fault [yes, that's a bad pun, thank you]).
 *
 * 2) If any of the (at most three) uncompleted data access in the pipeline
 *    are loads (from memory to a register), then the bit for the destination
 *    register is set in the SSBR.  Since the hardware will never complete
 *    that load (since we do it manually), the hardware will never clear that
 *    SSBR bit.  Thus, we must clear it manually.  If this isn't done, the
 *    system will hang waiting for a bit to clear that will never.
 *
 * 3) If the exception is the privilege violation exception, the bounds
 *    violation exception, or the misaligned access exception, the
 *    destination register bit in the SSBR may need to be cleared.
 *
 * 4) If the exception is one of the floating exceptions, then the
 *    destination register for that floating process won't be written,
 *    and the SSBR must be cleared explicitly.
 *
 * 5) The FPU must be enabled (as it is disabled by the exception processing
 *    hardware) and allowed to complete actions in progress. This is so
 *    so that it may be used in the servicing of any instruction.
 *    When the FPU is being restarted, operations attempting to complete
 *    may themselves fault (raising another exception).
 *
 * More on Restarting the FPU
 * --------------------------
 *	The manual [section 6.4.3.4] gives only minor mention to this
 * rather complex task.  Before the FPU is restarted all SSBR bits are
 * cleared for actions that the exception handler completes (as mentioned
 * above) so that the SSBR is clear unless there are FPU operations that
 * have not actually been completed (and hence not written to the registers).
 * Also, all control registers (at least all those that we care about) are
 * saved to the stack exception frame before the FPU is restarted (this
 * is important... the reason comes later).
 *
 * The FPU is restarted by doing an rte to a trap-not-taken (the rte
 * actually enables the fpu because we ensure that the EPSR has the
 * FPU-enable bit on; the trap-not-taken ensures anything in the FPU
 * completes by waiting until scoreboard register is clear).
 *
 * At the time the FPU is restarted (the rte to the trap-not-taken) the FPU
 * can write to ANY of the general registers.  Thus, we must make sure that
 * all general registers (r1..r31) are in their pre-exception state so that
 * when saved to the exception frame after the FPU is enabled, they properly
 * reflect any changes made by the FPU in being restarted.
 *
 * Because we can't save the pointer to the exception frame in a general
 * register during the FPU restart (it could get overwritten by the FPU!),
 * we save it in a control register, SR3, during the restart.
 *
 *
 * HOWEVER .....
 *
 * Because other uncompleted actions in the FPU may fault when the FPU is
 * restarted, a new exception may be raised during the restart. This may
 * happen recursively a number of times. Thus, during a restart, ANY register
 * whatsoever may be modified, including control registers.  Because of this
 * we must make sure that the exception handler preserves SR3 throughout
 * servicing an exception so that, if the exception had been raised during
 * an FPU restart, it is returned unmolested when control returns to the FPU
 * restart.
 *
 * Thus: if an exception not from kernel space, me MUST preserve SR3.
 * (if it from user space, no FPU-enable can be in progress and SR3 is
 *  unimportant).
 *
 * Now is a good time to recap SR0..SR3 usage:
 *   SR0 - current thread (zero when no threads are running during startup)
 *   SR1 - CPU flags (cpu number and some exception-handler flags)
 *   SR2 - generally free
 *   SR3 - free only if the exception is from user mode
 *
 * Once the FPU has been restarted, the general registers are saved to the
 * exception frame.  If the exception is not the interrupt exception,
 * interrupts are enabled and any faulted data accesses (see above) are
 * serviced.  In either case, the exception is then serviced (usually by
 * calling a C routine).  After servicing, any faulted data accesses are
 * serviced (if it had been the interrupt exception).  The context is then
 * restored and control returns to where the exception occurred.
 *
 */

#ifndef ASSEMBLER /* predefined by ascpp, at least */
#define ASSEMBLER /* this is required for some of the include files */
#endif

#include <assym.s>   	  		/* for PCB_KSP, etc */
#include <motorola/m88k/trap.h>        	/* for T_ defines */
#include <luna88k/locore.h>		/* lots of stuff */
#include <m88k/asm.h>
#include <mach/machine/vm_param.h>	/* for KERNEL_STACK_SIZE */
#include <kern/syscall_emulation.h>
#include <mach/kern_return.h>

/*
 * The exception frame as defined in "luna/m88k.h" (among other places) is
 * a bit outdated and needs to be changed. Until then, we'll define some
 * pseudo-fields there for our needs.
 *
 * EF_SR3    A place to save the exception-time SR3 from just after the
 * 	     time when an exception is raised until just after the FPU
 * 	     has been restarted.  This does not necessarly conflict with
 *	     the general registers (though it can if you're not careful)
 * 	     and so we can use a spot later used to save a general register.
 *
 * EF_FLAGS  This is just the old EF_MODE. "EF_MODE" isn't a very good name.
 */
#define EF_SR3			(EF_R0 + 5)
#define EF_FLAGS		EF_MODE

#define FLAG_FROM_KERNEL	8	/* this should be in locore.h */

	text
	align 4

/***************************************************************************
 ***************************************************************************
 **
 ** #define PREP(NAME, NUM, BIT, SSBR_STUFF, FLAG_CHECK)
 **
 ** This is the "exception processing preparaton" common to all exception
 ** processing.  It is used in the following manor:
 **
 ** 	LABEL(foo_handler)
 **           PREP("foo", 11, DEBUG_FOO_BIT, No_SSBR_Stuff, No_Precheck)
 **           CALL(_trap, T_FOO_FAULT, r31)
 **           DONE(DEBUG_FOO_BIT)
 **
 ** This defines the exception handler for the "foo" exception.
 ** The arguments ro PREP():
 **	NAME - 	String for debugging (more info later)
 **	NUM  - 	The exception number [see the manual, Table 6-1]
 **	BIT  - 	Bit to check in eh_debug for debugging (more info later)
 **	SSBR_STUFF -
 **		If the exception might leave some bits in the SSBR set,
 **		this should indicate how they are cleared.
 **	FLAG_PRECHECK -
 **		This is for the data access exception only. See it for
 **		more info.
 **
 **
 ** What's in between PREP() and DONE() (usually a CALL) is the actual
 ** servicing of the interrupt.  During this time, any register may
 ** be used freely as they've all been saved in the exception frame
 ** (which is pointed-to by r31).
 **/
#define PREP(NAME, NUM, BIT, SSBR_STUFF, FLAG_PRECHECK)		NEWLINE \
		xcr	FLAGS, FLAGS, SR1			NEWLINE \
		FLAG_PRECHECK					NEWLINE \
		 						NEWLINE \
		/* the bsr later clobbers r1, so save now */	NEWLINE \
		stcr	r1, SR2	/* r1 now free */		NEWLINE \
								NEWLINE \
		/* set or clear the FLAG_FROM_KERNEL bit */	NEWLINE \
		ldcr	r1, EPSR				NEWLINE \
		bb0.n	PSR_SUPERVISOR_MODE_BIT, r1, 1f		NEWLINE \
		clr	FLAGS, FLAGS, 1<FLAG_FROM_KERNEL>	NEWLINE \
		set	FLAGS, FLAGS, 1<FLAG_FROM_KERNEL>	NEWLINE \
								NEWLINE \
		/* get a stack (exception frame) */		NEWLINE \
	1:	bsr	setup_phase_one				NEWLINE \
								NEWLINE \
		/* TMP2 now free -- use to set EF_VECTOR */	NEWLINE \
		or	TMP2, r0, NUM				NEWLINE \
		st	TMP2, r31, REG_OFF(EF_VECTOR)		NEWLINE \
								NEWLINE \
		/* Clear any bits in the SSBR (held in TMP) */	NEWLINE \
		/* SSBR_STUFF may be empty, though.         */	NEWLINE \
		SSBR_STUFF					NEWLINE \
								NEWLINE \
		/* call setup_phase_two to restart the FPU  */  NEWLINE \
		/* and to save all general registers.	    */ 	NEWLINE \
		bsr	setup_phase_two				NEWLINE \
								NEWLINE \
		/* All general regs free -- do any debugging */	NEWLINE \
		PREP_DEBUG(BIT, NAME)

/* Some defines for use with PREP() */
#define No_SSBR_Stuff		/* empty */
#define Clear_SSBR_Dest		bsr clear_dest_ssbr_bit
#define No_Precheck		/* empty */
#define Data_Precheck \
	bb1.n   FLAG_IGNORE_DATA_EXCEPTION, FLAGS, ignore_data_exception

#if EH_DEBUG
	/*
	 * If we allow debugging, there is a variable "eh_debug"
	 * in which there is a bit for each exception.  If the bit
	 * is set for an exception, debugging information is printed
	 * about that exception whenever it occurs.
	 *
	 * The bits are defined in "locore.h"
	 */
	LABEL(_eh_debug) 	word 0x00000000

	/*
	 * additional pre-servicing preparation to be done when
	 * debugging... check eh_debug and make the call if
	 * need be.
	 */
	#define PREP_DEBUG(DebugNumber, Name)  \
		or.u	r2, r0, hi16(_eh_debug)		NEWLINE \
		ld  	r3, r2, lo16(_eh_debug)		NEWLINE \
		bb0	DebugNumber, r3, 4f		NEWLINE \
		/* call MY_info(ef,thread,flags,kind)*/ NEWLINE \
		or	r2, r31, r0			NEWLINE \
		ldcr	r3, SR0				NEWLINE \
		ldcr	r4, SR1				NEWLINE \
		or.u	r5, r0, hi16(2f)		NEWLINE \
		or	r5, r5, lo16(2f)		NEWLINE \
		bsr.n	_MY_info			NEWLINE \
		subu	r31, r31, 40			NEWLINE \
		br.n	4f				NEWLINE \
		addu	r31, r31, 40			NEWLINE \
	     2: string Name				NEWLINE \
		byte 0					NEWLINE \
		align 4					NEWLINE \
	     4:


	/*
	 * Post-servicing work to be done.
	 * When debugging, check "eh_debug" and call the
	 * debug routined if neeed be.
	 *
	 * Then, return from the interrupt handler.
	 */
	#define DONE(DebugNumber)  \
		or.u	r2, r0, hi16(_eh_debug)		NEWLINE \
		ld  	r3, r2, lo16(_eh_debug)		NEWLINE \
		bb0	DebugNumber, r3, 2f		NEWLINE \
		ldcr	r4, SR1				NEWLINE \
		CALL(_MY_info_done, r31, r4)		NEWLINE \
		2: br	return_from_exception_handler
#else
	/*
	 * If not debugging, then no debug-prep to do.
	 * Also, when you're done, you're done!	(no debug check).
	 */
	#define PREP_DEBUG(bit, name)
	#define DONE(num)		br return_from_exception_handler
#endif


/*#########################################################################*/
/*#### THE ACTUAL EXCEPTION HANDLER ENTRY POINTS ##########################*/
/*#########################################################################*/

/* unknown exception handler */
LABEL(unknown_handler)
	PREP("unknown", 0, DEBUG_UNKNOWN_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_UNKNOWNFLT, r30)
	DONE(DEBUG_UNKNOWN_BIT)

/* interrupt exception handler */
LABEL(interrupt_handler)
	PREP("interrupt", 1, DEBUG_INTERRUPT_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_pre_ext_int, 1, r30)
	DONE(DEBUG_INTERRUPT_BIT)

/* instruction access exception handler */
LABEL(instruction_access_handler)
	PREP("inst", 2, DEBUG_INSTRUCTION_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_INSTFLT, r30)
	/*
	 * Now, to retry the instruction.
	 * Copy the SNIP to the SFIP, clearing the E bit.
	 * Copy the SXIP to the SNIP, clearing the E bit.
	 */
	ld	r1, r30, REG_OFF(EF_SNIP)
	ld	r2, r30, REG_OFF(EF_SXIP)
	clr	r1, r1, 1<RTE_ERROR_BIT>
	clr	r2, r2, 1<RTE_ERROR_BIT>
	st	r1, r30, REG_OFF(EF_SFIP)
	st	r2, r30, REG_OFF(EF_SNIP)
	DONE(DEBUG_INSTRUCTION_BIT)

/*
 * data access exception handler --
 *  See badaddr() below for info about Data_Precheck.
 */
LABEL(data_exception_handler)
	PREP("data", 3, DEBUG_DATA_BIT, No_SSBR_Stuff, Data_Precheck)
	DONE(DEBUG_DATA_BIT)

/* misaligned access exception handler */
LABEL(misaligned_handler)
	PREP("misalign", 4, DEBUG_MISALIGN_BIT, Clear_SSBR_Dest, No_Precheck)
	CALL(_trap, T_MISALGNFLT, r30)
	DONE(DEBUG_MISALIGN_BIT)

/* unimplemented opcode exception handler */
LABEL(unimplemented_handler)
	PREP("unimp", 5, DEBUG_UNIMPLEMENTED_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_ILLFLT, r30)
	DONE(DEBUG_UNIMPLEMENTED_BIT)

/*
 * Some versions of the chip have * a bug whereby false privilege
 * violation exceptions are raised. If the valid bit in the SXIP is clear,
 * it is false.  If so, just return.  The code before PREP handles this....
 */
LABEL(privilege_handler)
	stcr    r1, SR2 	/* hold r1 for a moment */
	ldcr    r1, SXIP	/* look at the sxip... valid bit set? */
	bb1.n   RTE_VALID_BIT, r1, 1f /*skip over return if a valid exception*/
	ldcr    r1, SR2 	/* restore r1 */
	RTE
    1:  PREP("privilege", 6, DEBUG_PRIVILEGE_BIT, Clear_SSBR_Dest, No_Precheck)
	CALL(_trap, T_PRIVINFLT, r30)
	DONE(DEBUG_PRIVILEGE_BIT)

/*
 * I'm not sure what the trap(T_BNDFLT,...) does, but it doesn't send
 * a signal to the process...
 */
LABEL(bounds_handler)
	PREP("bounds", 7, DEBUG_BOUNDS_BIT, Clear_SSBR_Dest, No_Precheck)
	CALL(_trap, T_BNDFLT, r30)
	DONE(DEBUG_BOUNDS_BIT)

/* integer divide-by-zero exception handler */
LABEL(divide_handler)
	PREP("divide", 8, DEBUG_DIVIDE_BIT, Clear_SSBR_Dest, No_Precheck)
	CALL(_trap, T_ZERODIV, r30)
	DONE(DEBUG_DIVIDE_BIT)

/* integer overflow exception handelr */
LABEL(overflow_handler)
	PREP("overflow", 9, DEBUG_OVERFLOW_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_OVFFLT, r30)
	DONE(DEBUG_OVERFLOW_BIT)

/* Floating-point precise handler */
#define FPp_SSBR_STUFF bsr clear_FPp_ssbr_bit
LABEL(fp_precise_handler)
	PREP("FPU precise", 114, DEBUG_FPp_BIT, FPp_SSBR_STUFF, No_Precheck)
	CALL(_Xfp_precise, r0, r30) /* call fp_precise(??, exception_frame)*/
	DONE(DEBUG_FPp_BIT)

/* Floating-point imprecise handler */
#define FPi_SSBR_STUFF  bsr clear_FPi_ssbr_bit
LABEL(fp_imprecise_handler)
	PREP("FPU imprecise", 115, DEBUG_FPi_BIT, FPi_SSBR_STUFF, No_Precheck)
	CALL(_Xfp_imprecise, r0, r30) /*call fp_imprecise(??,exception_frame)*/
	DONE(DEBUG_FPi_BIT)



/* All standard system calls.  */
LABEL(syscall_handler)
        /* The approximate psuedo-code is:

	   t = current_thread->task->emul_dispatch;
	   if ( t != NULL) goto syscall_native;
	   r9 = r9 - t->disp_min;
	   if (r9 < 0) goto syscall_native;
	   if (t->disp_count < r9) goto syscall_native;
	   s = ((int *) t->disp_vector)[r9];
	   if (s==0) goto syscall_native;
	   ;; hit an emulated syscall
	   r9 = SNIP;
	   SNIP = s | valid_bit;
	   SFIP = 0   ;; noop
	   nop
	   rei

	   syscall_native:
	   	  r9 += t->disp_min.
                         .....

           We are going to handle emulated syscalls without saving
	   any registers. We can get two free registers by storing
	   there values into SR2, which is also free and SR3, which
	   we know is free because this is a trap from user space.

*/

/*
  Emulated or native syscall?

  Current status

  All registers have user values. (r9 syscall number)
  Shadow registers are user values.
  Nothing valid in pcb
  SR2 free
  SR3 free

  Shadowing Disabled
  Interrupts Disabled

  Immediate goal: determine wether it is a native or emulated
  syscall. We need some space to determine this, so stuff

  r2 -> SR2
  r3 -> SR3
*/

        stcr	r2, SR2
	stcr	r3, SR3


        /*
	  Since we expect the syscall to be emulated, we munge the
	  syscall number here in a reversible manner.
	  If it turns out to be native, we restore it.

	  The following checks are done to determine if the
	  syscall is emulated.

	  (1) null task emulation vector -> native
	  (2) syscall number < 0 ?  native
	  (3) check EML_MIN_SYSCALL+syscall number > vector length ? native
	  (4) vector[EML_MIN_SYSCALL+syscall number] == 0 ? native : emulated

	 Always use a positive value so we can use a 16 bit constant.
	 */

        ldcr    r3,  SR0                   /* get current thread */
        ld      r3,  r3, THREAD_TASK       /* get task pointer */
        ld	r3,  r3, TASK_EMUL         /* get emulation vector */
        bcnd	eq0, r3, syscall_native_r9_unmolested    /* if null, native */
	ld	r2,  r3, DISP_MIN          /* get min syscall */
	subu	r9,  r9, r2		   /* r9 = syscall - min_syscall */
       	bcnd.n	lt0, r9, syscall_native    /* if negative, native */
        ld	r2,  r3, DISP_COUNT        /* get count to r2 */
        /* compare against normalized syscall number */
  	/* is vector length less than specified element? */
        cmp     r2,  r2, r9
	bb1.n	ls,  r2, syscall_native    /* if r2 <= r9, native */
  	/* get array base; array is inline (part of struct) */
        addu    r3,  r3, DISP_VECTOR
        ld      r2,  r3[r9]                /* get element (word) */
        bcnd    eq0, r2, syscall_native    /* if zero, native */

/*
  An emulated syscall

  Current status:

  r1, r4-r31  have user values
  r2 has the address of the emulation routine (in user space)
  r3 has junk.
  Shadow registers have trap time values (except DMT0)
  Nothing valid in PCB.
  SR2 holds user r2
  SR3 holds user r3

  Interrupts Disabled
  Shadowing Disabled

  Immediate Goal: setup to go to user space.
 
  We have determined it is an emulated syscall.
  Put the SNIP (return address) into R9.
  Restore r2, r3 to user values
  Stuff SNIP with r2 (after setting valid bit).
  Stuff SFIP with SNIP+4 (indicating noop - following instruction).

  Jump back to user space
*/

        ldcr	r9, SNIP       /* get the snip, but don't massage it. */
        or	r2, r2, 2      /* set the valid bit */
        stcr    r2, SNIP       /* set the SNIP */
  	addu	r2, r2, 4      /* computer the following instruction */
  	stcr	r2, SFIP       /* set the SFIP */
        ldcr	r2, SR2        /* restore r2 */
	ldcr	r3, SR3        /* restore r3 */
/*
  Return to user space

  Current status:

  r1-r8, r10-r31 have user values.
  r9 has the trap time SNIP (return PC)
  SNIP is the emulation entry point
  SFIP indicates a nop
  Other shadow registers have trap time values.

  Interrupts Disabled
  Shadowing Disabled
 
  Immediate goal: execute an RTE.
  Trap time R9 was the syscall number.
  Since the emulation is vectored, the emulator can recover the syscall
  number. We pass the SNIP instead of SNIP & ~(3) for debugging purposes.

*/
  	RTE

LABEL(syscall_native)          /* not an emulated syscall */
/*
  We have a native syscall.

  Current status

  r1,r4-r31 have user values.
  r2, r3, saved in SR2, SR3 respectively
  r9 has had task->task_emulation.disp_min subtracted from it.
  Shadow registers have trap time values (except DMT0).
 
  Interrupts Disabled
  Shadowing Disabled

  Invariant: If we are here, thread->task->task_emul != 0

  Immediate goal: Restore r9

*/
	/* r9 has had disp_min substracted from it; add it back */
	ldcr	r3, SR0	 		/* thread pointer */
	ld	r3, r3, THREAD_TASK	/* get trap count number */
	ld	r3, r3, TASK_EMUL
  	ld	r3, r3, DISP_MIN
  	addu	r9, r9, r3     		/* restored r9 */


LABEL(syscall_native_r9_unmolested)
/*
  We have a native syscall.

  Current status

  r1,r4-r31 have user values.
  r2, r3, saved in SR2, SR3 respectively


  Interrupts Disabled
  Shadowing Disabled

  Immediate goal: determine if this is a legit native syscall.

*/
     /* get mach trap count limit */

     	or.u    r2, r0, hi16(_mach_trap_count)
        ld      r2, r2, lo16(_mach_trap_count)

	/* All Legit mach syscalls are negative; so negate r9
	   If it was positive, it will become negative.
	   In an unsigned comparison, this will make it very big. */
       	subu	r9, r0, r9
	/* do unsigned comparison of syscall number against limit */
	cmp	r2, r9, r2
        bb1.n	hs, r2, syscall_native_failure /* 'hs' = "higher or same" */

        ldcr	r3, SR3      /* restore r3 */

/*
  A legit native syscall.

  current status -

  Register usage -
   r1, r3-r31 have user values. (r9 has been negated)
   r2 free
   user r2 in SR2.
   Shadow reigsters have trap time values.
   Nothing valid in pcb.

   Shadowing Disabled
   Interrupts Disabled

   Immediate goal:   Save the shadow registers, r1-r3, r30 and r31.
   Enable shadowing and interrupts.
*/

	ldcr 	r2, SR0              /* get the thread pointer */
        ld	r2, r2, THREAD_PCB   /* get the pcb pointer */
#if (PCB_USER != 0)
	addu	r2, r2, PCB_USER     /* point to user storage */
#endif
        /* dump r30/r31 */
        st.d	r30, r2, GENREG_OFF(30)
        /* move r2 to r30 */
     	or	r30, r0, r2
	/* dump r1 */
  	st	r1,  r30, GENREG_OFF(1)
	/* dump user r2, r3 */
	ldcr	r2, SR2
  	st.d	r2,  r30, GENREG_OFF(2)
        /* dump shadow pc's, flags, epsr, and vector (128) */
        ldcr	r2, SFIP
	st	r2, r30, REG_OFF(EF_SFIP)
     	ldcr	r2, SNIP
	st	r2, r30, REG_OFF(EF_SNIP)
#if MACH_KDB
	/* avoid confusion in the debugger */
	ldcr	r2, SXIP
	st	r2, r30, REG_OFF(EF_SXIP)
#endif
	ldcr	r2, EPSR
	st	r2, r30, REG_OFF(EF_EPSR)
	or	r2, r0, 128
	st	r2, r30, REG_OFF(EF_VECTOR)
	ldcr	r2, SR1
	st	r2, r30, REG_OFF(EF_FLAGS)
	st	r0, r30, REG_OFF(EF_DMT0)

        /* pick up the kernel stack */
	ldcr	r2, SR0
        ld	r31, r2, THREAD_KERNEL_STACK
	addu	r31, r31, KERNEL_STACK_SIZE
/*
 * store the EF pointer in the usual place
 * for the sake of the debugger.
 */
        subu	r31, r31, 8
        st	r30, r31, 0
#if MACH_KDB
  	st	r30, r31, 4
#endif

	/* turn back on the FPU, and shadowing */
        ldcr	r1, PSR
	clr	r1, r1, 4<0> /* clear bits 0-3 (shadowing,fpu,intr,misalign) */
	stcr	r1, PSR
	FLUSH_PIPELINE

/*
  Argument count check

  Current status:

  r3-r8, r10-r29 have user values.
  r30 points to the pcb.
  r31 is the kernel stack pointer
  r9 is the (negated) syscall number.
  r1-3, r30-r31 stored in pcb.
  Shadow registers (SFIP/SNIP/SXIP,EPSR/VECTOR/FLAGS/DMT0) valid in pcb.

  Interrupts enabled.
  Shadowing enabled.

  Immediate Goal: determine if this syscall has more than
  seven arguments. If so, go to mach_syscall_interrupted.


  Load trap descriptor, and check the number of args.
	 	   Entries in the table are defined by:

	  		typedef struct {
	 			int		mach_trap_nargs;
	 			int		(*mach_trap_function)();
	 			int		mach_trap_stack;
	 			int		mach_trap_unused;
	  		} mach_trap_t;


*/
/* first place where a breakpoint can be inserted in the syscall path */
LABEL(syscall_breakable)
  	or.u	r2, r0, hi16(_mach_trap_table)
	or	r2, r2, lo16(_mach_trap_table)
        /* index by syscall number (r9*16) = r9 << 4  */
        mak	r9, r9, 0<4>      /* shift left 4 */
        ld	r1, r2, r9        /* r3 <- mach_trap_table[syscall] (nargs) */
	addu	r2, r2, r9        /* combine for later usage */
     	subu	r1, r1, 7	  /* nargs -= 7 */
       	bcnd	gt0, r1, syscall_native_interrupt  /* branch if nargs>7 */


/*
  Determine handoffedness of the syscall.

  Current status

  r2 is mach_trap_table[syscall]
  r9 holds the syscall number * 16
  r3-r8, r10-r29 have user values.
  r30 points to the pcb.
  r31 is the kernel stack pointer
  r1, r30-r31 stored in pcb.
  Shadow registers (SFIP/SNIP/SXIP,EPSR/VECTOR/FLAGS/DMT0) valid in pcb.

  Interrupts enabled.
  Shadowing enabled.

  Immediate goal:
  Load r1 with function pointer.
  If the syscall requires stack handoff, branch to the
  handoff_native_syscall code. Otherwise fall through to
  easy_native_syscall.
*/

  ld		r1, r2, 4    /* get function pointer */
  ld		r2, r2, 8
  bcnd.n 	ne0, r2, handoff_native_syscall
  ld		r2, r30, GENREG_OFF(2)      /* R2 back to user value. */

LABEL(easy_native_syscall)
/*

  A syscall without stack handoff.

  Current status -

  r1 holds pointer to syscall function.
  r2-r8, r10-r29 have user values.
  r9 is syscall*16.
  r1, r30-31 saved in pcb.
  Shadow registers saved in pcb.
  r30 points to the pcb
  r31 is the kernel stack pointer

  Interrupts enabled
  Shadowing Enabled

  Immediate goal: jump to syscall
*/
LABEL(easy_syscall_call)
        jsr.n	r1
	subu	r31, r31, 40
	addu	r31, r31, 40

       /*
	 Disable interrupts.
	 */
	ldcr	r30, PSR
	set	r30, r30, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r30, PSR
	FLUSH_PIPELINE
        /*
	  Restore all saved user state registers (except r31)
	  Then branch to common syscall return routine.
	  */

	ldcr	r30, SR0
	ld	r30, r30, THREAD_PCB
#if (PCB_USER>0)
	addu	r30, r30, PCB_USER
#endif
	/* restore r1, r30 */
	ld	r1,  r30, GENREG_OFF(1)
	br.n	syscall_native_return
	ld	r30, r30, GENREG_OFF(30)


/*
   Common return point for syscall routines.

   Current status:

   r1,r3-r30 are their user level values
   r2 is the return value.
   r31 is the kernel stack pointer.
   r1, r30, r31 valid in the at pcb.
   shadow registers valid in the pcb.
   SR2, SR3 are free

   Shadowing Enabled
   Interrupts Disabled

   Immediate Goal:   Check for ASTs.
*/
LABEL(syscall_native_return)
     1:
	stcr	r2, SR2          /* get a free register */
        ldcr	r2, SR1
	mak	r2, r2, FLAG_CPU_FIELD_WIDTH <2>	/* extract cpu number */
	/* if we need an ast, we never return */
	or.u	r2, r2, hi16(_need_ast)
	ld	r2, r2, lo16(_need_ast)                 /* r2 = need_ast */
     	bcnd	ne0, r2, 2f                             /* need an ast */

/*
  Past AST check.

  Current status

  r1, r3-r30 have their user values.
  r2 is stored in SR2
  r31 kernel stack pointer.
  SR3 is free.

  Interrupts disabled
  Shadowing Enabled.

  Immediate goal: disable shadowing, reload R31, shadow registers,
  and jump off to user mode.
*/

        ldcr	r2, PSR
       	set	r2, r2, 1<PSR_SHADOW_FREEZE_BIT>
	stcr	r2, PSR
       	FLUSH_PIPELINE

       	stcr	r3, SR3
	ldcr	r2, SR0        /* thread pointer */
	ld	r2, r2, THREAD_PCB
#if (PCB_USER>0)
	addu	r2, r2, PCB_USER
#endif
        ld	r3, r2, REG_OFF(EF_SFIP)
	stcr	r3, SFIP
       	ld	r3, r2, REG_OFF(EF_SNIP)
	stcr	r3, SNIP
	ld	r3, r2, REG_OFF(EF_EPSR)
	stcr	r3, EPSR
	ld	r3, r2, REG_OFF(EF_FLAGS)
  	ld	r31, r2, GENREG_OFF(31)
	/*
	 * restore the mode (cpu flags).
	 * This can't be done directly, because the flags include the
	 * CPU number.  We might now be on a different CPU from when we
	 * first entered the exception handler (due to having been blocked
	 * and then restarted on a different CPU).  Thus, we'll grab the
	 * old flags and put the current cpu number there.
	 */
	clr	r3, r3, FLAG_CPU_FIELD_WIDTH<0> /* clear WIDTH .. 0 bits */
	ldcr	r2, SR1
	clr	r2, r2, 0<FLAG_CPU_FIELD_WIDTH> /* clear 31 .. WIDTH bits */
       	or	r3, r3, r2

	stcr	r3, SR1
	/* now restore r2, r3, */
	ldcr	r2, SR2
	ldcr	r3, SR3

        /* clear the scoreboard register */
        stcr	r0, SSBR

      	/* return to user space */
	RTE

       2:

/*
   Need an ast

   Current status:

   r1, r30, r31 valid in pcb.
   Shadow registers valid in pcb.
   r1, r3-r30 have the user values.
   r31 is the kernel stack pointer.
   user r2 stored in SR2

   Interrupts Disabled
   Shadowing Enabled

   Goal: store all user registers, jump off to thread syscall return.
   Immediate goal: store all user registers in the pcb (except r1,r30, r31),
*/
	/* get pointer to user PCB */
       	ldcr	r2, SR0 /* thread pointer */
	ld	r2, r2, THREAD_PCB
#if (PCB_USER>0)
	addu	r2, r2, PCB_USER
#endif
        st	r3, r2, GENREG_OFF(3)
        st.d	r4, r2, GENREG_OFF(4)
        st.d	r6, r2, GENREG_OFF(6)
        st.d	r8, r2, GENREG_OFF(8)
        st.d	r10, r2, GENREG_OFF(10)
        st.d	r12, r2, GENREG_OFF(12)
        st.d	r14, r2, GENREG_OFF(14)
        st.d	r16, r2, GENREG_OFF(16)
        st.d	r18, r2, GENREG_OFF(18)
        st.d	r20, r2, GENREG_OFF(20)
        st.d	r22, r2, GENREG_OFF(22)
        st.d	r24, r2, GENREG_OFF(24)
        st.d	r26, r2, GENREG_OFF(26)
        st.d	r28, r2, GENREG_OFF(28)
	ldcr	r30, SR2
	st	r30, r2, GENREG_OFF(2)  /* store r2 */

/*
  jump to thread syscall return to handle the ast

  All user registers valid in the pcb.
  r31 is the kernel stack pointer
  r30 contains return value.

  Interrupts disabled
  Shadowing Disabled.

  Immediate goal: shove return code into r2,
  jump.

*/
	br.n	_thread_syscall_return
	or	r2, r0, r30


LABEL(handoff_native_syscall)
/*

  A syscall with stack handoff.

  Current status -

  r1 holds pointer to syscall function.
  r2-r8, r10-r29 have user values.
  r9 is syscall*16.
  r1, r30-31 saved in pcb.
  Shadow registers saved in pcb.
  r30 points to the pcb
  r31 is the kernel stack pointer

  Interrupts enabled
  Shadowing Enabled

  Immediate goal: save remaining user registers.
*/
        /* start saving registers
	   we need not save r2-r9 (argument registers)
	   or r10-r13 (leaf clobberable registers).
	*/
	st.d	r14, r30, GENREG_OFF(14)
     	st.d    r16, r30, GENREG_OFF(16)
        st.d    r18, r30, GENREG_OFF(18)
        st.d    r20, r30, GENREG_OFF(20)
        st.d    r22, r30, GENREG_OFF(22)
        st.d    r24, r30, GENREG_OFF(24)
        st.d    r26, r30, GENREG_OFF(26)
        st.d    r28, r30, GENREG_OFF(28)
     	st      r0,  r30, GENREG_OFF(0)

#ifdef JEFF_DEBUG
	/*
	 * save everything so that the exception frame is filled in case
 	 * we want to see from the debugger.
	 */
	st.d	 r2, r30, GENREG_OFF( 2)
	st.d	 r4, r30, GENREG_OFF( 4)
	st.d	 r6, r30, GENREG_OFF( 6)
	extu	 r9, r9, 0<4> /* r9 /= 16 */
	subu	 r9, r0, r9
	st.d	 r8, r30, GENREG_OFF( 8)
	st.d	r10, r30, GENREG_OFF(10)
	st.d	r12, r30, GENREG_OFF(12)
#endif

/*
  Current status:

  Shadow registers valid in the pcb.
  r0 - r31 valid in pcb. (r2-r13 random)
  r30 points to the pcb.
  r31 points to the kernel stack.
  r2-r8 are their trap time values.
  r1 points to the system call to make.

  Interrupts enabled
  Shadowing enabled

  Immediate goal: call the syscall function
  */
LABEL(handoff_syscall_call)
        jsr.n	r1
	subu	r31, r31, 40
	addu	r31, r31, 40

/*
  Major goal: Return to user space

  Current status:

  Shadow registers valid in the pcb.
  r0 - r31 valid in pcb.
  r31 points to the kernel stack.
  r2 is the return value from the syscall.

  Interrupts enabled
  Shadowing enabled

  Immediate goal:
  	jump off to thread_syscall_return,
	which will do the right thing.
*/
	br	_thread_syscall_return

LABEL(syscall_native_failure)
/*
  A legit native syscall.

  current status -

  Register usage -
   r1, r3-r31 have user values.
   r2 free
   Shadow registers have trap time values.
   Nothing valid in pcb.

   Shadowing Disabled
   Interrupts Disabled

   Immediate goal:   Set R2 to KERN_FAILURE, then back to user space.
*/
     	or.u	r2, r0, hi16(KERN_FAILURE)
     	or	r2, r2, lo16(KERN_FAILURE)
     	RTE

LABEL(syscall_native_interrupt)
/*
  A native syscall, but one that takes more than 7 arguments.

  Current status:

  r3-r8, r10-r29 have user values.
  r30 points to the pcb.
  r31 is the kernel stack pointer
  r1-3, r30-r31 stored in pcb.
  Shadow registers (SFIP/SNIP/SXIP,EPSR/VECTOR/FLAGS/DMT0) valid in pcb.

  Interrupts enabled.
  Shadowing enabled.

  Return MACH_SEND_INTERRUPTED.
  Need to disable interrupts, shadowings, restore shadow registers, r30, r31,
  and r1, r3.
*/
  	ld	r3, r30, REG_OFF(EF_SNIP)    /* load SNIP */
	ld	r1, r30, REG_OFF(EF_SFIP)    /* load SFIP */
     	/* disable shadowing, interrupts */
     	ldcr	r2, PSR
  	set	r2, r2, 1<PSR_INTERRUPT_DISABLE_BIT>
  	set	r2, r2, 1<PSR_SHADOW_FREEZE_BIT>
  	stcr	r2, PSR
  	FLUSH_PIPELINE
	stcr	r3, SNIP
  	stcr	r1, SFIP
  	/* restore shadow registers, r1, r3, r30, r31 */
  	ld	r2, r30, REG_OFF(EF_EPSR)
	ld	r1, r30, GENREG_OFF(1)	     /* restore 1 */
  	stcr	r2, EPSR
  	stcr	r0, SSBR                     /* clear score board */
	ld	r2, r30, REG_OFF(EF_FLAGS)
	ld	r3, r30, GENREG_OFF(3)       /* restore 3 */
	/* No way we could have switched cpus, so don't worry
	   about resetting the cpu number in the flags */
	stcr	r2, SR1                      /* restore FLAGS */
	ld.d	r30, r30, GENREG_OFF(30)     /* restore 30, r31 */
  	or.u	r2, r0, hi16(MACH_SEND_INTERRUPTED)
     	or	r2, r2, lo16(MACH_SEND_INTERRUPTED)
     	RTE

LABEL(_sigsys)
	PREP("sigsys", 0, DEBUG_SIGSYS_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_SIGSYS, r30)
	DONE(DEBUG_SIGSYS_BIT)

LABEL(_sigtrap)
	PREP("sigtrap", 0, DEBUG_SIGTRAP_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_SIGTRAP, r30)
	DONE(DEBUG_SIGTRAP_BIT)

LABEL(_stepbpt)
	PREP("sigtrap", 0, DEBUG_SIGTRAP_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_STEPBPT, r30)
	DONE(DEBUG_SIGTRAP_BIT)

LABEL(_userbpt)
	PREP("sigtrap", 0, DEBUG_SIGTRAP_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_USERBPT, r30)
	DONE(DEBUG_SIGTRAP_BIT)

#if MACH_KDB
   LABEL(break)
	PREP("break", 130, DEBUG_BREAK_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_KDB_BREAK, r30)
	DONE(DEBUG_BREAK_BIT)
   LABEL(trace)
	PREP("trace", 131, DEBUG_TRACE_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_KDB_TRACE, r30)
	DONE(DEBUG_TRACE_BIT)
   LABEL(entry) /* used to known as kdb */
	PREP("kdb", 132, DEBUG_KDB_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_KDB_ENTRY, r30)
	DONE(DEBUG_KDB_BIT)
#else /* else not MACH_KDB */
   LABEL(break)
	PREP("break", 130, DEBUG_BREAK_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_UNKNOWNFLT, r30)
	DONE(DEBUG_BREAK_BIT)
   LABEL(trace)
	PREP("trace", 131, DEBUG_TRACE_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_UNKNOWNFLT, r30)
	DONE(DEBUG_TRACE_BIT)
   LABEL(entry)
	PREP("unknown", 132, DEBUG_UNKNOWN_BIT, No_SSBR_Stuff, No_Precheck)
	CALL(_trap, T_UNKNOWNFLT, r30)
	DONE(DEBUG_KDB_BIT)
#endif	/* MACH_KDB */


/*--------------------------------------------------------------------------*/

/*
 * The error exception handler.
 * The error exception is raised when any other non-trap exception is raised
 * while shadowing is off. This is Bad News.
 *
 * The shadow registers are not valid in this case (shadowing was off, ne).
 * R1-R31 may be interesting though, so we'll save them.
 *
 * We'll not worry about trashing r26-29 here,
 * since they aren't generally used.
 */
LABEL(error_handler)
        /* pick up the slavestack */
	or	r26, r0, r31           /* save old stack */
        or.u	r31, r0,  hi16(_slavestack_end)
        or	r31, r31, lo16(_slavestack_end)

	/* zero the stack, so we'll know what we're lookin' at */
        or.u	r27, r0,  hi16(_slavestack)
        or	r27, r27, lo16(_slavestack)
   1:	cmp	r28, r27, r31
	bb1      ge, r28,  2f   /* branch if at the end of the stack */
	st	 r0,  r0, r27
	br.n     1b
	addu    r28, r27,   4   /* bump up */
   2:   /* stack has been cleared */

	/* ensure that stack is 8-byte aligned */
        clr     r31, r31, 3<0>  /* round down to 8-byte boundary */

	/* create exception frame on stack */
        subu    r31, r31, SIZEOF_EF             /* r31 now our E.F. */

	/* save old R31 and other R registers */
        st.d    r0 , r31, GENREG_OFF(0)
        st.d    r2 , r31, GENREG_OFF(2)
        st.d    r4 , r31, GENREG_OFF(4)
        st.d    r6 , r31, GENREG_OFF(6)
        st.d    r8 , r31, GENREG_OFF(8)
        st.d    r10, r31, GENREG_OFF(10)
        st.d    r12, r31, GENREG_OFF(12)
        st.d    r14, r31, GENREG_OFF(14)
        st.d    r16, r31, GENREG_OFF(16)
        st.d    r18, r31, GENREG_OFF(18)
        st.d    r20, r31, GENREG_OFF(20)
        st.d    r22, r31, GENREG_OFF(22)
        st.d    r24, r31, GENREG_OFF(24)
        st      r30, r31, GENREG_OFF(30)
	st	r26, r31, GENREG_OFF(31)

	/* save shadow registers (are OLD, though) */
	ldcr	r10, SXIP
	st	r10, r31, REG_OFF(EF_SXIP)
	ldcr	r10, SFIP
	st	r10, r31, REG_OFF(EF_SFIP)
	ldcr	r10, SNIP
	st	r10, r31, REG_OFF(EF_SNIP)
	ldcr	r10, SSBR
	st	r10, r31, REG_OFF(EF_SSBR)
	ldcr	r10, EPSR
	st	r10, r31, REG_OFF(EF_EPSR)

	ldcr	r10, DMT0
	st	r10, r31, REG_OFF(EF_DMT0)
	ldcr	r11, DMD0
	st	r11, r31, REG_OFF(EF_DMD0)
	ldcr	r12, DMA0
	st	r12, r31, REG_OFF(EF_DMA0)

	ldcr	r10, DMT1
	st	r10, r31, REG_OFF(EF_DMT1)
	ldcr	r11, DMD1
	st	r11, r31, REG_OFF(EF_DMD1)
	ldcr	r12, DMA1
	st	r12, r31, REG_OFF(EF_DMA1)

	ldcr	r10, DMT2
	st	r10, r31, REG_OFF(EF_DMT2)
	ldcr	r11, DMD2
	st	r11, r31, REG_OFF(EF_DMD2)
	ldcr	r12, DMA2
	st	r12, r31, REG_OFF(EF_DMA2)

	ldcr	r10, SR1
	st	r10, r31, REG_OFF(EF_MODE)

	/* shove sr2 into EF_FPLS1 */
	ldcr	r10, SR2
	st	r10, r31, REG_OFF(EF_FPLS1)

	/* shove sr3 into EF_FPHS2 */
	ldcr	r10, SR3
	st	r10, r31, REG_OFF(EF_FPHS2)

	/* error vector is zippo numero el'zeroooo */
	st	r0,  r31, REG_OFF(EF_VECTOR)

	stcr	r0,  SSBR  /* won't want shadow bits bothering us later */

	/*
	 * Cheep way to enable FPU and start shadowing again.
	 */
        ldcr    r10, PSR
        clr     r10, r10, 1<PSR_FPU_DISABLE_BIT>    /* enable the FPU */
        clr     r10, r10, 1<PSR_SHADOW_FREEZE_BIT>  /* also enable shadowing */
	
        stcr    r10, PSR  /* bang */
	FLUSH_PIPELINE

	/* put pointer to regs into r30... r31 will become a simple stack */
	or	r30, r31, r0

        subu    r31, r31, 0x10 /* make some breathing space */
        st      r30, r31, 0x0c /* store frame pointer on the st */
        st      r30, r31, 0x08 /* store again for the debugger to recognize */
	or.u    r20,  r0, hi16(0x87654321)
	or      r20, r20, lo16(0x87654321)
	st	r20, r31, 0x04
	st	r20, r31, 0x00

        CALL(_error_fault, r30, r30)

        /* TURN INTERUPTS back on */
        ldcr r1, PSR
   	clr  r1, r1, 1<PSR_INTERRUPT_DISABLE_BIT>
        stcr r1, PSR
        FLUSH_PIPELINE
LABEL(error_loop) bsr error_loop
	/* never returns*/

/*
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */


/*
 * This is part of baddadr (below).
 */
_LABEL(ignore_data_exception)
	/******************************************************\
	*  SR0: pointer to the current thread structure        *
	*  SR1: previous FLAGS reg			       *
	*  SR2: free                                           *
	*  SR3: must presere                                   *
	*  FLAGS:  CPU status flags                            *
	\******************************************************/
 	xcr	FLAGS, FLAGS, SR1	/* replace SR1, FLAGS */

	/*
	 * For more info, see badaddr() below.
	 *
	 * We just want to jump to "badaddr__return_nonzero" below.
	 *
	 * We don't worry about trashing R2 here because we're
	 * jumping back to the function badaddr() where we're allowd
	 * to blast r2..r9 as we see fit.
	 */

	/* the "+2" below is to set the VALID bit. */
	or.u	r2, r0, hi16(badaddr__return_nonzero + 2)
	or  	r2, r2, lo16(badaddr__return_nonzero + 2)
	stcr	r2, SNIP	/* Make it the next instruction to execute */
	addu	r2, r2, 4
	stcr	r2, SFIP	/* and the next one after that, too. */
	stcr	r0, SSBR	/* make the scoreboard happy. */

 	/* the following jumps to "badaddr__return_nonzero" in below */
	RTE

/*
 * extern boolean_t badaddr(unsigned addr, unsigned len)
 *
 * Returns true (non-zero) if the given LEN bytes starting at ADDR are
 * not all currently accessable by the kernel.
 *
 * If all LEN bytes starting at ADDR are accessable, zero is returned.
 *
 * Len may be be 1, 2, or 4.
 *
 * This is implementd by setting a special flag CPU before trying to access
 * the given address. If a data access exception is raised, the address
 * is inaccessable. The exception handler will notice the special CPU flag
 * and not try to swap the address in. Rather, it will return to
 * "badaddr__return_nonzero" in this routine so that we may return non-zero
 * to the calling routine.
 *
 * If no fault is raised, we continue to where we return zero to the calling
 * routine (after removing the special CPU flag).
 */

/*
 * Badwordaddr(addr) is same as badaddr(addr,4).
 */
LABEL(_badwordaddr)  /* boolean_t badwordaddr(void*) */
	/* It's a bad address if it's misaligned.  */
	bb1	0, r2, 1f
	bb1	1, r2, 1f
	/* Disable interrupts ... don't want a context switch while we're
	 * doing this! Also, save the old PSR in R8 to restore later. */
	ldcr	r8, PSR
	set	r4, r8, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r4, PSR
	ldcr	r5, SR1
	set	r6, r5, 1<FLAG_IGNORE_DATA_EXCEPTION>
	FLUSH_PIPELINE
	stcr	r6, SR1
	/* The next line will either fault or not. See comment in badaddr.  */
	ld	r3, r2, 0
	FLUSH_PIPELINE
	or	r2, r0, r0 /* indicate a zero (address not bad) return. */
	stcr	r5, SR1
	stcr	r8, PSR
	jmp  	r1
    1: /* misaligned bad return */
	or	r2, r0, 1
	jmp	r1

LABEL(_badaddr)
	/*
	 * Disable interrupts ... don't want a context switch while we're
	 * doing this! Also, save the old PSR in R8 to restore later.
	 */
	ldcr	r8, PSR
	set	r4, r8, 1<PSR_INTERRUPT_DISABLE_BIT>
	FLUSH_PIPELINE
	stcr	r4, PSR

	ldcr	r5, SR1
	set	r5, r5, 1<FLAG_IGNORE_DATA_EXCEPTION>
	/* resetting r5 to SR1 done in the delay slot below. */

	/*
	 * If it's a word we're doing, do that here. Otherwise,
	 * see if it's a halfword.....
	 */
	sub	r6, r3, 4
	bcnd.n	ne0, r6, badaddr__maybe_halfword
	stcr	r5, SR1
	FLUSH_PIPELINE

	/*
	 * It's a bad address if it's misaligned.
	 */
	bb1	0, r2, badaddr__return_nonzero
	bb1	1, r2, badaddr__return_nonzero
	/*
	 * The next line will either fault or not. If it faults, execution
	 * will go to:  data_access_handler (see above)
	 * and then to: ignore_data_exception (see above)
	 * and then to: badaddr__return_nonzero (see below)
	 * which will return to the calling function.
 	 *
	 * If there is no fault, execution just continues as normal.
	 */
	ld	r5, r2, 0
	FLUSH_PIPELINE
	br.n	badaddr__return
	or	r2, r0, r0	/* indicate a zero (address not bad) return.*/

    badaddr__maybe_halfword:
	/* More or less like the code for checking a word above */
	sub	r6, r3, 2
	bcnd	ne0, r6, badaddr__maybe_byte

	/* it's bad if it's misaligned */
	bb1	0, r2, badaddr__return_nonzero

	FLUSH_PIPELINE
	ld.h	r5, r2, 0
	FLUSH_PIPELINE
	br.n	badaddr__return
	or	r2, r0, r0

    badaddr__maybe_byte:
	/* More or less like the code for checking a word above */
	sub	r6, r3, 1
	bcnd	ne0, r6, badaddr__unknown_size
	FLUSH_PIPELINE
	ld.b	r5, r2, 0
	FLUSH_PIPELINE
	br.n	badaddr__return
	or	r2, r0, r0
    badaddr__unknown_size:
#ifndef NDEBUG
	data
	  1: string "bad length (%d) to badaddr() from 0x%x\n\000"
	text
	or.u	r2, r0, hi16(1b)
	or  	r2, r2, lo16(1b)
	or	r4, r0, r1
	bsr	_printf
	or.u	r2, r0, hi16(1b)
	or  	r2, r2, lo16(1b)
	bsr	_panic
	/*NOTREACHED*/
#endif

_LABEL(badaddr__return_nonzero)
	or	r2, r0, 1
	/* fall through to badaddr__return */

_LABEL(badaddr__return)
	ldcr	r4, SR1
	clr	r4, r4, 1<FLAG_IGNORE_DATA_EXCEPTION>
	stcr	r4, SR1

	/*
	 * Restore the PSR to what it was before.
	 * The only difference is that we might be enabling interrupts
	 * (which we turned off above).  If interrupts were already off,
	 * we do not want to turn them on now, so we just restore from
	 * where we saved it.
	 */
	FLUSH_PIPELINE
	stcr	r8, PSR
	jmp  	r1


/*
******************************************************************************
******************************************************************************
******************************************************************************
*/


LABEL(setup_phase_one)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread (if any, null if not)			*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: saved copy of exception-time r1				*
	* SR3: must be preserved .. may be the exception-time stack	*
	* r1: return address to calling exception handler		*
	* FLAGS: CPU status flags					*
	***************************************************		*
	* immediate goal:						*
	* 	Decide where we're going to put the exception frame.	*
	*	Might be at the end of R31, SR3, or the thread's	*
	*	pcb.	          					*
	\***************************************************************/

        /* Check if we are coming in from a FPU restart exception.
           If so, the pcb will be in SR3 */
	bb1.n	FLAG_ENABLING_FPU, FLAGS, use_SR3_pcb
	xcr	r1, r1, SR2
        /* are we coming in from user mode? If so, pick up thread pcb */
	bb0	FLAG_FROM_KERNEL, FLAGS, pickup_stack

        /* Interrupt in kernel mode, not FPU restart */
    _LABEL(already_on_kernel_stack)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread (if any, null if not)			*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: return address to the calling exception handler		*
	* SR3: must be preserved; may be important for other exceptions	*
	* FLAGS: CPU status flags					*
	***************************************************		*
	* immediate goal:						*
	*	We're already on the kernel stack, but not having	*
	*	needed to use SR3. We can just make room on the		*
	* 	stack (r31) for our exception frame.			*
	\***************************************************************/
	subu	r31, r31, SIZEOF_EF	      /* r31 now our E.F. */
	st	FLAGS, r31, REG_OFF(EF_FLAGS) /* save flags */
	st	r1, r31, GENREG_OFF(1)	      /* save prev. r1 (now r1 free)*/

	ldcr	r1, SR3				/* save previous SR3 */
	st	r1, r31, REG_OFF(EF_SR3)

	addu	r1, r31, SIZEOF_EF		/* save previous r31 */
	br.n	have_pcb
	st	r1, r31, GENREG_OFF(31)


    _LABEL(use_SR3_pcb)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread (if any, null if not)			*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: return address to the calling exception handler		*
	* SR3: must be preserved; exception-time stack pointer		*
	* FLAGS: CPU status flags					*
	***************************************************		*
	* immediate goal:						*
	*	An exception occured while enabling the FPU. Since r31	*
	*	is the user's r31 while enabling the FPU, we had put	*
	*	our pcb pointer into SR3, so make room from       	*
	*	there for our stack pointer.				*
	*       We need to check if SR3 is the old stack pointer or the *
        *       pointer off to the user pcb. If it pointing to the user *
        *       pcb, we need to pick up the kernel stack. Otherwise     *
        *       we need to allocate a frame upon it.                    *
        *       We look at the EPSR to see if it was from user mode     *
        *       Unfortunately, we have no registers free at the moment  *
        *       But we know register 0 in the pcb frame will always be  *
        *       zero, so we can use it as scratch storage.              *
        *                                                               *
        *                                                               *
	\***************************************************************/
	xcr 	r30, r30, SR3			/* r30 = old exception frame */
        st      r1,  r30, GENREG_OFF(0)         /* free up r1 */
        ld      r1,  r30, REG_OFF(EF_EPSR)      /* get back the epsr */
        bb0.n     PSR_SUPERVISOR_MODE_BIT, r1, 1f /* if user mode */
        ld      r1,  r30, GENREG_OFF(0)         /* restore r1 */
        /* we were in kernel mode - dump frame upon the stack */
	st      r0,  r30, GENREG_OFF(0)         /* repair old frame */
	subu	r30, r30, SIZEOF_EF		/* r30 now our E.F. */
	st	FLAGS, r30, REG_OFF(EF_FLAGS)	/* save flags */
	st	r1, r30, GENREG_OFF(1)	    /* save prev. r1 (now r1 free) */

	st	r31, r30, GENREG_OFF(31)	/* save previous r31 */
	or	r31, r0, r30			/* make r31 our pointer. */
	addu	r30, r30, SIZEOF_EF		/* r30 now has previous SR3 */
	st	r30, r31, REG_OFF(EF_SR3)	/* save previous SR3 */
	br.n	have_pcb
	xcr	r30, r30, SR3			/* restore r30 */
       1:
         /* we took an exception while restarting the FPU from user space.
            Consequently, we never picked up a stack. Do so now.
	    R1 is currently free (saved in the exception frame pointed at by
	    r30) */
	ldcr	r1, SR0     /* thread pointer */
	ld	r1, r1, THREAD_KERNEL_STACK
        addu	r1, r1, KERNEL_STACK_SIZE-SIZEOF_EF
	st	FLAGS, r1, REG_OFF(EF_FLAGS)    /* store flags */
	st	r31, r1,   GENREG_OFF(31)       /* store r31 - now free */
        st      r30, r1,   REG_OFF(EF_SR3)      /* store old SR3 (pcb) */
     	or	r31, r1, r0 	/* make r31 our exception frame pointer */
	ld	r1,  r30, GENREG_OFF(0)         /* restore old r1 */
        st	r0,  r30, GENREG_OFF(0)         /* repair that frame */
	st	r1,  r31, GENREG_OFF(1)	   /* store r1 in its proper place */
	br.n	have_pcb
	xcr	r30, r30, SR3			/* restore r30 */

    _LABEL(pickup_stack)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: return address to the calling exception handler		*
	* SR3: free							*
	* FLAGS: CPU status flags					*
	***************************************************		*
	* immediate goal:						*
	* 	Since we're servicing an exception from user mode, we	*
	*	know that SR3 is free.  We use it to free up a temp.	*
	*	register to be used in getting the thread's pcb         *
	\***************************************************************/
	stcr	r31, SR3		/* save previous r31 */

	/* switch to the thread's kernel stack. */
	ldcr	r31, SR0			/* get thread pointer */
	ld	r31, r31, THREAD_PCB		/* get the thread pcb pointer */
     	addu	r31, r31, PCB_USER		/* point to user save area */
	st	FLAGS, r31, REG_OFF(EF_FLAGS)	/* save flags */
	st	r1, r31, GENREG_OFF(1)		/* save prev. r1 (now r1 free)*/
	ldcr	r1, SR3				/* save previous r31 */
	st	r1, r31, GENREG_OFF(31)
	/*FALLTHROUGH */

    _LABEL(have_pcb)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: return address to the calling exception handler		*
	* SR3: free							*
	* r1:  free							*
	* FLAGS: CPU status flags					*
	* r31: our exception frame   					*
	*    Valid in the exception frame:				*
	*	Exception-time r1, r31, FLAGS.	 			*
	*	Exception SR3, if appropriate.				*
	***************************************************		*
	* immediate goal:						*
	* 	Save the shadow registers that need to be saved to	*
	*	the exception frame.					*
	\***************************************************************/
	stcr	TMP, SR3	/* free up TMP, TMP2, TMP3 */
	SAVE_TMP2
	SAVE_TMP3

	/* save some exception-time registers to the exception frame */
	ldcr	TMP, EPSR
	ldcr	TMP2, SFIP
	ldcr	TMP3, SNIP
	st	TMP, r31, REG_OFF(EF_EPSR)
	st	TMP2, r31, REG_OFF(EF_SFIP)
	st	TMP3, r31, REG_OFF(EF_SNIP)

	ldcr	TMP, SSBR
	ldcr	TMP2, SXIP
	ldcr	TMP3, DMT0
	st	TMP2, r31, REG_OFF(EF_SXIP)

#if 0
	/*
	 * The following is a kludge so that
	 * a core file will have a copy of
	 * DMT0 so that 'sim' can display it
	 * correctly.
	 * After a data fault has been noticed,
	 * the real EF_DTM0 is cleared, so I need
	 * to throw this somewhere.
	 * There's no special reason I chose this
	 * register (FPIT)... it's just one of many
	 * for which it causes no pain to do this.
	 */
	st TMP3, r31, REG_OFF(EF_FPIT)
#endif

	/*
	 * The above shadow registers are obligatory for any and all
	 * exceptions.  Now, if the data access pipeline is not clear,
	 * we must save the DMx shadow registers, as well as clear
	 * the appropriate SSBR bits for the destination registers of
	 * loads or xmems.
	 */
	bb0.n	DMT_VALID_BIT, TMP3, DMT_check_finished
	st	TMP3, r31, REG_OFF(EF_DMT0)

	ldcr	TMP2, DMT1
	ldcr	TMP3, DMT2
	st	TMP2, r31, REG_OFF(EF_DMT1)
	st	TMP3, r31, REG_OFF(EF_DMT2)

	ldcr	TMP2, DMA0
	ldcr	TMP3, DMA1
	st	TMP2, r31, REG_OFF(EF_DMA0)
	st	TMP3, r31, REG_OFF(EF_DMA1)

	ldcr	TMP2, DMA2
	ldcr	TMP3, DMD0
	st	TMP2, r31, REG_OFF(EF_DMA2)
	st	TMP3, r31, REG_OFF(EF_DMD0)

	ldcr	TMP2, DMD1
	ldcr	TMP3, DMD2
	st	TMP2, r31, REG_OFF(EF_DMD1)
	st	TMP3, r31, REG_OFF(EF_DMD2)

	/*
         *---------------------------------------------------------------
	 * need to clear "appropriate" bits in the SSBR before
	 * we restart the FPU
	 */


    _LABEL(check_DMT0)
	ldcr	TMP2, DMT0
	bb0.n	DMT_VALID_BIT, TMP2, DMT_check_finished
	stcr	r0, DMT0 /* so an exception at fpu_enable doesn't see our DMT0*/
	bb1	DMT_LOCK_BIT,  TMP2, do_DMT0
	bb1	DMT_WRITE_BIT, TMP2, check_DMT1
        _LABEL(do_DMT0)
	extu	TMP2, TMP2, DMT_DREG_WIDTH <DMT_DREG_OFFSET>
	set	TMP2, TMP2, 1<5>
	clr	TMP, TMP, TMP2

    _LABEL(check_DMT1)
	ldcr	TMP2, DMT1
	bb0	DMT_VALID_BIT, TMP2, check_DMT2
	bb1	DMT_LOCK_BIT,  TMP2, do_DMT1
	bb1	DMT_WRITE_BIT, TMP2, check_DMT2
        _LABEL(do_DMT1)
	extu	TMP2, TMP2, DMT_DREG_WIDTH <DMT_DREG_OFFSET>
	set	TMP2, TMP2, 1<5>
	clr	TMP, TMP, TMP2

    _LABEL(check_DMT2)
	ldcr	TMP2, DMT2
	bb0	DMT_VALID_BIT, TMP2, DMT_check_finished
	bb1	DMT_LOCK_BIT,  TMP2, do_DMT2_single
	bb1	DMT_WRITE_BIT, TMP2, DMT_check_finished
	bb1	DMT_DOUBLE_BIT,TMP2, do_DMT2_double
        _LABEL(do_DMT2_single)
	extu	TMP2, TMP2, DMT_DREG_WIDTH <DMT_DREG_OFFSET>
	br.n	1f
	set	TMP2, TMP2, 1<5>
	_LABEL(do_DMT2_double)
	extu	TMP2, TMP2, DMT_DREG_WIDTH <DMT_DREG_OFFSET>
	set	TMP2, TMP2, 1<6>
1:	clr	TMP, TMP, TMP2

    _LABEL(DMT_check_finished)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: return address to the calling exception handler		*
	* SR3: saved TMP						*
	* r1:  free							*
	* TMP: possibly revised SSBR					*
	* TMP2: free							*
	* TMP3: free							*
	* FLAGS: CPU status flags					*
	* r31: exception frame						*
	*    Valid in the exception frame:				*
	*	Exception-time r1, r31, FLAGS.	 			*
	*	Exception-time TMP2, TMP3.				*
	*	Exception-time espr, sfip, snip, sxip.			*
	*	Dmt0.							*
	*	Other data pipeline control registers, if appropriate.	*
	*	Exception SR3, if appropriate.				*
	\***************************************************************/
	ldcr	r1, SR2
	jmp	r1 /* return to allow the handler to clear more SSBR bits */

/************************************************************************/
/************************************************************************/

    _LABEL(clear_FPi_ssbr_bit)
	/*
	 * Clear floatingpont-imprecise ssbr bits.
	 * Also, save appropriate FPU control registers to the E.F.
	 *
	 *  r1:  return address to calling exception handler
         *  TMP : (possibly) revised ssbr
         *  TMP2 : free
         *  TMP3 : free
	 */
 	fldcr	TMP2, FPSR
	fldcr	TMP3, FPCR
	st	TMP2, r31, REG_OFF(EF_FPSR)
	st	TMP3, r31, REG_OFF(EF_FPCR)

 	fldcr	TMP2, FPECR
	fldcr	TMP3, FPRH
	st	TMP2, r31, REG_OFF(EF_FPECR)
	st	TMP3, r31, REG_OFF(EF_FPRH)

	fldcr	TMP2, FPIT
 	fldcr	TMP3, FPRL
	st	TMP2, r31, REG_OFF(EF_FPIT)
	st	TMP3, r31, REG_OFF(EF_FPRL)

	/*
	 * We only need clear the bit in the SSBR for the
	 * 2nd reg of a double result [see section 6.8.5]
	 */
	#define FPIT_SIZE_BIT	10
	bb0	FPIT_SIZE_BIT, TMP2, not_double_fpi
	extu	TMP2, TMP2, 5<0>  /* get the reg. */
	set	TMP2, TMP2, 1<6>  /* set width (width=2 will clear two bits) */
	clr	TMP, TMP, TMP2

       _LABEL(not_double_fpi)
	jmp	r1


/************************************************************************/
/************************************************************************/


    _LABEL(clear_FPp_ssbr_bit)
	/*
	 * Clear floating pont precise ssbr bits.
	 * Also, save appropriate FPU control registers to the E.F.
	 *
	 *  r1:  return address to calling exception handler
         *  TMP : (possibly) revised ssbr
         *  TMP2 : free
         *  TMP3 : free
	 */
 	fldcr	TMP2, FPSR
	fldcr	TMP3, FPCR
	st	TMP2, r31, REG_OFF(EF_FPSR)
	st	TMP3, r31, REG_OFF(EF_FPCR)

 	fldcr	TMP2, FPHS1
	fldcr	TMP3, FPHS2
	st	TMP2, r31, REG_OFF(EF_FPHS1)
	st	TMP3, r31, REG_OFF(EF_FPHS2)

 	fldcr	TMP2, FPLS1
	fldcr	TMP3, FPLS2
	st	TMP2, r31, REG_OFF(EF_FPLS1)
	st	TMP3, r31, REG_OFF(EF_FPLS2)

 	fldcr	TMP2, FPPT
	fldcr	TMP3, FPECR
	st	TMP2, r31, REG_OFF(EF_FPPT)
	st	TMP3, r31, REG_OFF(EF_FPECR)

	#define 	FPPT_SIZE_BIT	5
	bb1.n	FPPT_SIZE_BIT, TMP2, 1f
	extu	TMP3, TMP2, 5<0> /* get FP operation dest reg */
	br.n	2f
	set	TMP3, TMP3, 1<5> /* set size=1 -- clear one bit for "float" */
     1: set	TMP3, TMP3, 1<6> /* set size=2 -- clear two bit for "double" */
     2:
	clr	TMP, TMP, TMP3	/* clear bit(s) in ssbr. */
	jmp	r1


/************************************************************************/
/************************************************************************/


    _LABEL(clear_dest_ssbr_bit)
	/*
	 * There are various cases where an exception can leave the
	 * destination register's bit in the SB set.
	 * Examples:
	 *	misaligned or privilege exception on a LD or XMEM
	 *	DIV or DIVU by zero.
 	 *
 	 * I think that if the instruction is LD.D, then two bits must
	 * be cleared.
 	 *
	 * Even though there are a number of instructions/exception
	 * combinations that could fire this code up, it's only required
	 * to be run for the above cases.  However, I don't think it'll
	 * ever be a problem to run this in other cases (ST instructions,
	 * for example), so I don't bother checking.  If we had to check
	 * for every possible instruction, this code would be much larger.
 	 *
	 * The only checking, then, is to see if it's a LD.D or not.
	 *
	 * At the moment....
	 *  r1:  return address to calling exception handler
         *  TMP : (possibly) revised ssbr
         *  TMP2 : free
         *  TMP3 : free
	 */
	ldcr	TMP3, EPSR	/* going to check: user or system memory? */
	ldcr	TMP2, SXIP	/* get the instruction's address */
	bb1.n	PSR_SUPERVISOR_MODE_BIT, TMP3, 2f
	clr	TMP2, TMP2, 2<0> /* get rid of valid and error bits. */

    1:  /* user space load here */
#if ERRATA__XXX_USR
	NOP
	ld.usr	TMP2, TMP2, r0		/* get the instruction itself */
	NOP
	NOP
	NOP
	br	3f
#else
	br.n	3f
	ld.usr	TMP2, TMP2, r0		/* get the instruction itself */
#endif

    2:  /* system space load here */
	ld	TMP2, TMP2, r0		/* get the instruction itself */

    3:  /* now have the instruction..... */
	/*
	 * Now see if it's a double load
	 * There are three forms of double load [IMM16, scaled, unscaled],
	 * which can be checked by matching against two templates:
	 *			 -- 77776666555544443333222211110000 --
	 *	if (((instruction & 11111100000000000000000000000000) ==
	 *			    00010000000000000000000000000000) ||
	 *	    ((instruction & 11111100000000001111110011100000) ==
	 *			    11110100000000000001000000000000))
	 *	{
	 *		It's a load double, so
	 *		clear two SSBR bits.
	 *	}
	 *	else
	 *	{
	 *		It's not a load double.
	 *		Must be a load single, xmem, or st
	 *		Thus, clear one SSBR bit.
	 *	}
	 */
	/* check the first pattern for ld.d */
	extu	TMP3, TMP2, 16<16>	/* get the upper 16 bits */
	mask	TMP3, TMP3, 0xFC00	/* apply the mask */
	cmp	TMP3, TMP3, 0x1000  /* if this is equal, it's a load double */
	bb1	eq,   TMP3, misaligned_double

	/* still could be -- check the second pattern for ld.d */
	/* look at the upper 16 bits first */
	extu	TMP3, TMP2, 16<16>  /* get the upper 16 bits */
	mask	TMP3, TMP3, 0xFC00  /* apply the mask */
	cmp	TMP3, TMP3, 0xF400  /* if equal, it might be a load double */
	bb1	ne,   TMP3, misaligned_single /* not equal, so must be single */

	/* now look at the lower 16 bits */
	extu	TMP3, TMP2, 16<0>    /* get the lower 16 bits */
	mask	TMP3, TMP3, 0xFCE0   /* apply the mask */
	cmp	TMP3, TMP3, 0x1000   /* if this is equal, it's a load double */
	bb1	eq,   TMP3, misaligned_double

      _LABEL(misaligned_single)
	extu	TMP2, TMP2, 5<21>	/* get the destination register */
	br.n	1f
        set	TMP2, TMP2, 1<5>	/* set size=1 */

      _LABEL(misaligned_double)
	extu	TMP2, TMP2, 5<21>  /* get the destination register */
        set	TMP2, TMP2, 1<6>   /* set size=2 -- clear two bit for "ld.d" */

     1: jmp.n	r1
	clr	TMP, TMP, TMP2	/* clear bit(s) in ssbr. */

/************************************************************************/
/************************************************************************/



  LABEL(setup_phase_two)
	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: saved copy of exception-time register now holding FLAGS	*
	* SR2: free							*
	* SR3: saved TMP						*
	* r1:  return address to calling exception handler 		*
	* TMP: possibly revised SSBR					*
	* TMP2: free							*
	* TMP3: free							*
	* FLAGS: CPU status flags					*
	* r31: our exception frame					*
	*    Valid in the exception frame:				*
	*	Exception-time r1, r31, FLAGS.	 			*
	*	Exception-time TMP2, TMP3.				*
	*	Exception-time espr, sfip, snip, sxip.			*
	*	Exception number (EF_VECTOR).				*
	*	Dmt0							*
	*	Other data pipeline control registers, if appropriate.	*
	*	FPU control registers, if appropriate.			*
	*	Exception SR3, if appropriate.				*
	***************************************************		*
	* immediate goal:						*
	*	restore the system to the exception-time state (except	*
	* SR3 will be OUR stack pointer) so that we may resart the FPU.	*
	\***************************************************************/
	stcr	TMP, SSBR	/* done with SSBR, TMP now free */
	RESTORE_TMP2		/* done with extra temp regs */
	RESTORE_TMP3		/* done with extra temp regs */

	/* Get the current PSR and modify for the rte to enable the FPU */
	ldcr	TMP, PSR
	clr	TMP, TMP, 1<PSR_FPU_DISABLE_BIT>    /* enable the FPU */
        clr	TMP, TMP, 1<PSR_SHADOW_FREEZE_BIT>  /* also enable shadowing */
	stcr	TMP, EPSR

	/* the "+2" below is to set the VALID_BIT */
	or.u	TMP, r0, hi16(fpu_enable + 2)
	or	TMP, TMP, lo16(fpu_enable + 2)
	stcr	TMP, SNIP		/* jump to here fpu_enable */
	addu	TMP, TMP, 4
	stcr	TMP, SFIP		/* and then continue after that */

	set	FLAGS, FLAGS, 1<FLAG_ENABLING_FPU> /* note what we're doing.*/
	xcr	FLAGS, FLAGS, SR1
	st	r1, r31, REG_OFF(EF_RET) /* save the return address */
	ld	r1, r31, GENREG_OFF(1)	 /* get original r1 */

	xcr	TMP, r31, SR3	/* TMP now restored. R31 now saved in SR3 */
	ld	r31, r31, GENREG_OFF(31) /* get original r31 */

	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: CPU flags						*
	* SR2: free							*
	* SR3: pointer to our exception frame (our stack pointer)	*
	* r1 through r31: original exception-time values		*
	*								*
	*    Valid in the exception frame:				*
	*	Exception-time FLAGS.	 				*
	*	Exception-time espr, sfip, snip, sxip.			*
	*	Exception number (EF_VECTOR).				*
	*	Dmt0							*
	*	Other data pipeline control registers, if appropriate.	*
	*	FPU control registers, if appropriate.			*
	*	Exception SR3, if appropriate.				*
	*   Held temporarly in the exception frame:			*
	*	Return address to the calling excption handler.		*
	***************************************************		*
	* immediate goal:						*
	*	Do an RTE to restart the fpu and jump to "fpu_enable"	*
	*	Another exception (or exceptions) may be raised in	*
	*	this, which is why FLAG_ENABLING_FPU is set in SR1.	*
	\***************************************************************/
	RTE	/* jumps to "fpu_enable" on the next line to enable the FPU. */

  _LABEL(fpu_enable)
	FLUSH_PIPELINE
	xcr	TMP, TMP, SR3		 /* get E.F. pointer */
	st.d	r30, TMP, GENREG_OFF(30) /* save previous r30, r31 */
	or	r31, TMP, r0		 /* transfer E.F. pointer to r31 */
	ld	TMP, r31, REG_OFF(EF_SR3)/* get previous SR3; maybe important*/

	/* make sure that the FLAG_ENABLING_FPU bit is off */
	xcr	FLAGS, FLAGS, SR1
	clr	FLAGS, FLAGS, 1<FLAG_ENABLING_FPU>
	xcr	FLAGS, FLAGS, SR1

	xcr	TMP, TMP, SR3		 /* replace TMP, SR3 */

	/* now save all regs to the exception frame. */
	st.d	r0 , r31, GENREG_OFF(0)
	st.d	r2 , r31, GENREG_OFF(2)
	st.d	r4 , r31, GENREG_OFF(4)
	st.d	r6 , r31, GENREG_OFF(6)
	st.d	r8 , r31, GENREG_OFF(8)
	st.d	r10, r31, GENREG_OFF(10)
	st.d	r12, r31, GENREG_OFF(12)
	st.d	r14, r31, GENREG_OFF(14)
	st.d	r16, r31, GENREG_OFF(16)
	st.d	r18, r31, GENREG_OFF(18)
	st.d	r20, r31, GENREG_OFF(20)
	st.d	r22, r31, GENREG_OFF(22)
	st.d	r24, r31, GENREG_OFF(24)
	st.d	r26, r31, GENREG_OFF(26)
	st.d	r28, r31, GENREG_OFF(28)
#ifdef JEFF_DEBUG
	/* mark beginning of frame with notable value */
	or.u r20,  r0, hi16(0x12345678)
	or   r20, r20, lo16(0x12345678)
	st   r20, r31, GENREG_OFF(0)
#endif

	/***************** REGISTER STATUS BLOCK ***********************\
	* SR0: current thread 						*
	* SR1: free							*
	* SR2: free							*
	* SR3: previous exception-time SR3				*
	* r1: return address to the calling exception handler		*
	* r2 through r30: free						*
	* r31: our exception frame  					*
	*								*
	*    Valid in the exception frame:				*
	*	Exception-time r0 through r31.				*
	*	Exception-time FLAGS.	 				*
	*	Exception-time espr, sfip, snip, sxip.			*
	*	Exception number (EF_VECTOR).				*
	*	Dmt0							*
	*	Other data pipeline control registers, if appropriate.	*
	*	FPU control registers, if appropriate.			*
	*	Exception SR3, if appropriate.				*
	***************************************************		*
	* immediate goal:						*
        *	Pick up a stack if we came in from user mode. Put	*
        *       A copy of the exception frame pointer into r30          *
        *       bump the stack a doubleword and write the exception     *
        *       frame pointer.                                          *
	*	if not an interrupt exception,				*
	*	    Turn on interrupts and service any outstanding	*
	*	    data access exceptions.				*
	*	Return to calling exception handler to			*
	*	service the exception.					*
	\***************************************************************/

	/*
	 * If it's not the interrupt exception, enable interrupts and
	 * take care of any data access exceptions......
	 */

        ld	r2,  r31, REG_OFF(EF_EPSR)
	bb1.n	PSR_SUPERVISOR_MODE_BIT, r2, 1f  /* have a stack */
	or	r30, r0, r31              /* get a copy of the e.f. pointer */
	ldcr	r31, SR0                         /* get thread pointer */
	ld	r31, r31, THREAD_KERNEL_STACK    /* get ksp */
	addu	r31, r31, KERNEL_STACK_SIZE        /* point at proper end */
 /* This label is here for debugging */
 exception_handler_has_ksp:  global exception_handler_has_ksp
  1:    /*
           here - r30 holds a pointer to the exception frame.
           r31 is a pointer to the kernel stack.
        */
  	subu	r31, r31, 8  /* make some breathing space */
	st	r30, r31, 0  /* store frame pointer on the stack */
#if MACH_KDB
  	st	r30, r31, 4  /* store it again for the debugger to recognize */
#endif MACH_KDB

	ld	r2, r30, REG_OFF(EF_VECTOR)
        bcnd.n  eq0, r2, return_to_calling_exception_handler  /* is error */
        ld	r14, r30, REG_OFF(EF_RET)
	cmp	r3, r2, 1 /* interrupt is exception #1 ;Is an interrupt? */
	bb1.n	eq, r3, return_to_calling_exception_handler /* skip if so */

#if MACH_KDB
	cmp    	r3, r2, 64 /* KDB break exception */
        bb1.n   eq, r3, return_to_calling_exception_handler

        cmp     r3, r2, 66 /* KDB entry exception */
        bb1.n   eq, r3, return_to_calling_exception_handler
#endif

	ldcr    r2, PSR
 	clr	r2, r2, 1<PSR_INTERRUPT_DISABLE_BIT>	/* enable interrupts */
 	stcr	r2, PSR
#if     MACH_KDB
  	FLUSH_PIPELINE
#endif

	/* service any outstanding data pipeline stuff
	   - check dmt0 anything outstanding?*/

	ld	r3, r30, REG_OFF(EF_DMT0)
	bb0	DMT_VALID_BIT, r3, return_to_calling_exception_handler

/*
        r30 can be clobbered by calls. So stuff its value into a
        preserved register, say r15. R14 is in use (see return_to_... below).
 */
	or 	r15, r0, r30

	CALL(_trap, T_DATAFLT, r15)
	CALL(_data_access_emulation, r15, r0)

/*	restore it... */
	or	r30, r0, r15

	/* clear the dmt0 word in the E.F */
	st	r0, r30, REG_OFF(EF_DMT0)

    _LABEL(return_to_calling_exception_handler)
	jmp	r14 /* loaded above */



/*
 * ##########################################################################
 * ##########################################################################
 * ##########################################################################
 */

LABEL(return_from_exception_handler)
	/*
	 * Regs r1-r30 are free. R31 is pointing at the word
         * on the kernel stack where our pointer to the exception frame
         * it stored. Reload it now.
 	 *
	 * At this point, if EF_DMT0 is not zero, then
	 * this must have been an interrupt where the fault didn't
	 * get corrected above.  We'll do that now.
	 *
	 * We load it into r14 since it is preserved across function
	 * calls, and we may have to call some routines from within here.
	 *
	 * control is transfered here from obvious places in this file
         * and thread_bootstrap in luna88k/locore.c.
	 *
	 */
#define FPTR    r14
	ld	FPTR, r31, 0                 /* grab exception frame pointer */
	ld	r3, FPTR, REG_OFF(EF_DMT0)
	bb0	DMT_VALID_BIT, r3, check_ast /*[Oh well, nothing to do here] */

#if 1
	    /*
	    * This might happen for non-interrupts  If the user sets DMT0
	    * in an exception handler.........
	    */
	    ld 	r2, FPTR, REG_OFF(EF_VECTOR)
	    cmp	r2, r2, 1 /* interrupt is exception #1 ; Is an interrupt? */
	    bb1	eq, r2, 1f
	    LABEL(oops)
	    or.u	r4, r0, hi16(2f)
	    or		r4, r4, lo16(2f)
#if MACH_KDB
	    CALL(_db_printf, r4, r0)
     	    ENTRY_ASM
#endif
            br 1f
		2:  string "OOPS: DMT0 not zero and not interrupt.\n\000"
		    align 4
       1:
#endif
	/*
	 * If it's the interrupt exception, enable interrupt.
	 * Take care of any data access exception...... 90/8/15 add by yama
	 */
	ld	r2, FPTR, REG_OFF(EF_VECTOR)
	cmp	r2, r2, 1   /* interrupt is exception #1 ; Is an interrupt? */
        bb1	ne, r2, 1f  /* If not so, skip */

        /* if EPSR has interrupts disabled, skip also */
	ld      r2, FPTR, REG_OFF(EF_EPSR)
        bb1	PSR_INTERRUPT_DISABLE_BIT, r2, 1f        /* skip if disabled */
	ldcr    r2, PSR
 	clr	r2, r2, 1<PSR_INTERRUPT_DISABLE_BIT>	 /* enable interrupts */
        FLUSH_PIPELINE
        stcr	r2, PSR
   1:
	/*
	 * if there happens to be a data fault that hasn't been serviced yet,
	 * go off and service that...
	 */
	CALL(_trap, T_DATAFLT, r30)
	CALL(_data_access_emulation, r30, r0)     /* really only 2 args */

	/* clear the dmt0 word in the E.F. */
	st	r0 , FPTR, REG_OFF(EF_DMT0)


LABEL(check_ast)

	ldcr	r1, PSR				     /* get current PSR */
	set	r1, r1, 1<PSR_INTERRUPT_DISABLE_BIT> /* set for disable intr. */
	stcr	r1, PSR				     /* install new PSR */
	FLUSH_PIPELINE

	/*
	 *
	 * This code (including a bit above) is more or less:
	 *
	 *    check_ast:
	 *
	 *	Disable interrupts
	 *	if (exception was from user mode && need_ast[cpu_number()])
	 *	{
	 *	    call: ast_taken()
	 *	    goto check_ast
	 *	}
 	 *
	 *
	 * need_ast[] is (currently: 10/89) defined in kern/ast.c as:
	 *	int need_ast[NCPUS]
 	 * and is modified by aston() and astoff() in the same file.
	 *
	 *
	 */

	ld	r2, FPTR, REG_OFF(EF_EPSR)   /* get pre-exception PSR */
	bb1	PSR_SUPERVISOR_MODE_BIT, r2, no_ast /*skip if in system mode */

	/* get and check need_ast[cpu#] */
	ldcr	r2, SR1
	mak	r2, r2, FLAG_CPU_FIELD_WIDTH <2>  /* r2 = cpu# */
	or.u	r2, r2, hi16(_need_ast)
	ld	r3, r2, lo16(_need_ast)		  /* r3 now need_ast[cpu#] */
	bcnd	eq0, r3, no_ast
	st  	r0, r2, lo16(_need_ast)		  /* unset it */


        bsr.n  	_ast_taken		/* enter with interrupts disabled */
        subu	r31, r31, 40		/* return with interrupts enabled */
        addu    r31, r31, 40
	br	check_ast		/* and check again..... */

_LABEL(no_ast)

  /* now ready to return....*/

    /*
     * Transfer the frame pointer to r31, since we no longer need a stack.
     * No page faults here, and interrupts are disabled.
     */

        or	r31, r0,  FPTR


/* restore r1 later */
	ld.d	r2 , r31, GENREG_OFF(2)
	ld.d	r4 , r31, GENREG_OFF(4)
	ld.d	r6 , r31, GENREG_OFF(6)
	ld.d	r8 , r31, GENREG_OFF(8)
	ld.d	r10, r31, GENREG_OFF(10)
	ld.d	r12, r31, GENREG_OFF(12)
	ld.d	r14, r31, GENREG_OFF(14)
	ld.d	r16, r31, GENREG_OFF(16)
	ld.d	r18, r31, GENREG_OFF(18)
	ld.d	r20, r31, GENREG_OFF(20)
	ld.d	r22, r31, GENREG_OFF(22)
	ld.d	r24, r31, GENREG_OFF(24)
	ld.d	r26, r31, GENREG_OFF(26)
	ld.d	r28, r31, GENREG_OFF(28)
	/* restore r1, r30, r31 later */


	/* disable shadowing (interrupts already disabled above) */
	ldcr	r1, PSR
	set	r1, r1, 1<PSR_SHADOW_FREEZE_BIT>
	FLUSH_PIPELINE
	stcr	r1, PSR

	/* reload the control regs*/

	/*
	 * Note: no need to restore the SXIP.
	 * When the "rte" causes execution to continue
	 * first with the instruction pointed to by the NIP
	 * and then the FIP.
	 *
	 * See MC88100 Risc Processor User's Manual, 2nd Edition,
	 * section 6.4.3.1.2-4
	 */
	ld	r30, r31, REG_OFF(EF_SNIP)
	ld	r1, r31, REG_OFF(EF_SFIP)
	stcr	r0, SSBR
	stcr	r30, SNIP
	stcr	r1, SFIP

	ld	r30, r31, REG_OFF(EF_EPSR)
	ld	r1,  r31, REG_OFF(EF_MODE)
	stcr	r30, EPSR

	/*
	 * restore the mode (cpu flags).
	 * This can't be done directly, because the flags include the
	 * CPU number.  We might now be on a different CPU from when we
	 * first entered the exception handler (due to having been blocked
	 * and then restarted on a different CPU).  Thus, we'll grab the
	 * old flags and put the current cpu number there.
	 */
	clr	r1, r1, FLAG_CPU_FIELD_WIDTH <0> /* clear bits 0..WIDTH */
	ldcr	r30, SR1
	clr	r30, r30, 0<FLAG_CPU_FIELD_WIDTH> /* clear bits WIDTH..31 */
	or	r30, r1, r30
	stcr	r30, SR1  /* restore old flags with (maybe new) CPU number */

	/* Now restore r1, r30, and r31 */
	ld	r1,  r31, GENREG_OFF(1)
	ld.d	r30, r31, GENREG_OFF(30)

    _LABEL(return_from_exception)
	RTE

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
