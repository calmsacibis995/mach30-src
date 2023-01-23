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

/* $RCSfile: locore.c,v $ --  asm boot routines
 *
 **********************************************************************
 * This file created by Omron Corporation.
 *
 * HISTORY
 * $Log:	locore.s,v $
 * Revision 2.4  93/02/04  07:49:29  danner
 * 	luna88k/asm.h -> m88k/asm.h
 * 	[93/02/02            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:04:30  danner
 * 	Replaced ;comments with C-style comments for better CPPification.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:42:11  jfriedl
 * 	Moved and renamed from locore/locore.c
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.5.2.2  92/05/27  14:38:31  danner
 * 	Corrections to revision 2.5
 * 
 * Revision 2.5.2.1  92/05/23  18:29:29  danner
 * 	Added kernelstart label.
 * 	[92/05/23            danner]
 * 
 * Revision 2.7  92/05/22  09:21:29  jfriedl
 * 	took out 'cpu=1' at start of slave.
 * 
 * Revision 2.6  92/05/21  17:19:17  jfriedl
 * 	Added raw_putstr and raw_putchar.
 * 	[92/05/21  16:08:15  jfriedl]
 * 
 * Revision 2.5  92/02/19  14:03:41  elf
 * 	br should have been br.n. Pointed out by Dan Malek of HRI.
 * 	[92/02/10            danner]
 * 
 * 	Removed release_other_cpus. Moved call_rom_abort. Removed
 * 	      thread_bootstrap_return.
 * 	[92/01/30            danner]
 * 
 * 	Updated includes.
 * 	[92/01/30            danner]
 * 	Removed esym calculation.
 * 	[92/01/19            danner]
 * 
 * 	Removed esym definition, as it is obsoleted by the
 * 	 move_bootstrap.
 * 	[92/01/19            danner]
 * 	The interrupt_stack for the master processor was not getting set;
 * 	 it needs to be set to _intstack once the master knows its cpu
 * 	 number.
 * 
 * 	Fixed problem with master processor interrupt stack.
 * 	[91/11/04            danner]
 * 	Changed thread_bootstrap to return to
 * 	 return_from_exception_handler instead of check_ast. 
 * 	      No longer sets need_ast[cpu#] = 1.
 * 	[91/09/05            danner]
 * 
 * Revision 2.4  91/08/28  20:22:12  jsb
 * 	As a hack to make things compile, added null definition
 * 	of thread_bootstrap_return.
 * 
 * Revision 2.3  91/08/24  12:10:50  af
 * 	New slave stack initialization.
 * 	[91/08/15            rvb]
 * 	Removed the unused scASM* and ocsum functions.
 * 	[91/08/06            danner]
 * 	Made save context switch to interupt stack.
 * 	[91/07/15            danner]
 * 
 * Revision 2.2  91/04/05  13:55:36  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 *****************************************************************RCS**/

#ifndef ASSEMBLER /* predefined by ascpp, at least */
#define ASSEMBLER
#endif

#include "luna88k/locore.h"
#include "motorola/m88k/m88100/m88100.h"
#include "motorola/m88k/trap.h"
#include "m88k/asm.h"
#include "sys/version.h"	/* for INCLUDE_VERSION define */
#include "luna88k/board.h"	/* lots of stuff (OBIO_PIO*, SYSV_BASE, etc)*/
#include "luna88k/pio.h"		/* lots of stuff */
#include "mach/machine/vm_param.h"      /* INTSTACK_SIZE */

/***********************************************************************/

/*
 *  Arrange for the include file version number to appear directly in
 *  the namelist.
 */
global	_INCLUDE_VERSION
def	_INCLUDE_VERSION, INCLUDE_VERSION

/*
 * The memory looks like:
 *   0x00000 - 0x01000	 : exception vectors (512 vectors x 2 instructions)
 *   0x10000 - 0x20000	 : ROM work area (I don't know how much actually used).
 *   0x20000 == start    : branch to start_text
 *   0x20004 - start_text: some locks and temporary stack space.
 *	The locks:
 *  		master_processor_chosen
 *   		inter_processor_lock
 *	are described below.
 *
 *   All slave cpus now come up using allocated interrupt stacks.
 *   The masters comes up an uses intstack, a pre allocated interrupt stack.
 *
 ***********************************************************************/
	text

LABEL(_kernelstart)
LABEL(start)
	/* hardware (ROM) jumps here (this address, not this label) on reset*/
	br _start_text

    _LABEL(initialized_cpu_lock)
        /* XMEM spin lock -- to count CPUs */
	word 0

    LABEL(_initialized_cpus)
        /* CPU counter to initialize */
	word 0

    _LABEL(master_processor_chosen)
	/* The first processor that XMEMs this becomes the master */
	word 0

    LABEL(_inter_processor_lock)
	/* XMEM spin lock -- controls access to master_processor_chosen */
	word 0

    _LABEL(vector_list)		  /* references memory BELOW this line */
	#include "luna88k/exception_vectors.h"
	word	END_OF_VECTOR_LIST

    _LABEL(_msgsw)
	word 0 /* Bits here turn on/off debugging somewhere. */


/**************************************************************************/
LABEL(_start_text)	/* This is the *real* start upon poweron or reset */
	/*
	 * CPU Initialization
	 *
	 * Every CPU starts from here..
	 * (well, from 'start' above, which just jumps here).
	 *
	 * I use r11 and r22 here 'cause they're easy to not
	 * get mixed up -- r10, for example, looks too similar
	 * to r0 when not being careful....
	 *
	 * Ensure that the PSR is as we like:
	 *	supervisor mode
	 *	big-endian byte ordering
	 *	concurrent operation allowed
	 *	carry bit clear (I don't think we really care about this)
	 *	FPU enabled
	 *	misaligned access raises an exception
	 *	interrupts disabled
	 *	shadow registers frozen
	 *
	 * The manual says not to disable interrupts and freeze shadowing
	 * at the same time because interupts are not actually disabled
	 * until after the next instruction. Well, if an interrupt
	 * occurs now, we're in deep   anyway, so I'm going to do
	 * the two together.
	 *
	 * Upon a reset (or poweron, I guess), the PSR indicates:
	 *   supervisor mode
	 *   interrupts, shadowing, FPU, missaligned exception: all disabled
	 *
	 * We'll just construct our own turning on what we want.
	 *
	 *	jfriedl@omron.co.jp
	 */
	stcr	r0, SSBR	/* clear this for later */
	stcr	r0, SR0		/* clear "current thread" */
	stcr	r0, SR1		/* clear the CPU Flags */
	stcr	r0, SR2		/* not strictly necessary, but looks clean */
	stcr	r0, SR3		/* not strictly necessary, but looks clean */

	set	r11, r0,  1<PSR_SUPERVISOR_MODE_BIT>
	set	r11, r11, 1<PSR_INTERRUPT_DISABLE_BIT>
	stcr	r11, PSR
	/* shadowing, FPU, misalgined access exception: all enabled now.*/

	/* set the vector base */
	#if SYSV_BASE == 0	/* SYSV is "System Vector", not "AT&T SysV"! */
	   stcr	r0, VBR
	#else
	   or.u	r11,  r0, hi16(SYSV_BASE)
    	   or	r11, r11, lo16(SYSV_BASE)
	   stcr	r11, VBR
	#endif

	/*
	 * First time to count how many CPU to attached
	 */
	or.u	r11, r0,  hi16(initialized_cpu_lock)
	or	r11, r11, lo16(initialized_cpu_lock)
    _LABEL(check_init_lock)
	FLUSH_PIPELINE
	or	r22, r0,  1
	xmem    r22, r11, r0	  /* If r22 gets 0, we have the lock.. */
	bcnd	eq0, r22, have_init_lock /* ..but if not, we must wait */

    _LABEL(wait_for_init_lock)
	/* just watch the lock until it looks clear */
	ld	r22, r11, r0
	bcnd	eq0, r22, check_init_lock
	br	wait_for_init_lock		/* looks clear -- try to grab */

    _LABEL(have_init_lock)
	FLUSH_PIPELINE
	or.u	r11, r0,  hi16(_initialized_cpus)
        ld	r22, r11, lo16(_initialized_cpus)
	add	r23, r22, 1
	st	r23, r11, lo16(_initialized_cpus)

	or.u	r11, r0,  hi16(initialized_cpu_lock)
        st	r0,  r11, lo16(initialized_cpu_lock)

	/*
 	 * Now we vie with any other processors to see who's the master.
 	 * We first try to obtain a lock to see who's allowed
	 * to check/set the master lock.
	 */
	or.u	r11, r0,  hi16(_inter_processor_lock)
	or	r11, r11, lo16(_inter_processor_lock)
    _LABEL(check_ip_lock)
	FLUSH_PIPELINE
	or	r22, r0,  1
	xmem    r22, r11, r0	       /* If r22 gets 0, we have the lock.. */
	bcnd	eq0, r22, have_ip_lock /* ..but if not, we must wait */
    _LABEL(wait_for_ip_lock)
	/* just watch the lock until it looks clear */
	ld	r22, r11, r0
	bcnd	ne0, r22, wait_for_ip_lock
        /* since we can be here with caches off, add a few nops to 
	   keep the bus from getting overloaded */
        or      r2, r0, lo16(1000)
    _LABEL(ip_loop)
        subu    r2, r2, 1
        bcnd	eq0, r2, ip_loop
	br	check_ip_lock		/* looks clear -- try to grab */

    _LABEL(have_ip_lock)

	/* now try to grab the master_processor_chosen prize */
	FLUSH_PIPELINE
	or.u	r11, r0,  hi16(master_processor_chosen)
	or	r11, r11, lo16(master_processor_chosen)
	or	r22, r0,  1
	xmem	r22, r11, r0

	/*
	 * If r22 is not clear we're a slave,
	 * otherwise we're first and the master.
	 *
	 * Note that we haven't released the interprocessor lock....
	 * We'll do that when we're ready for another CPU to go.
	 * (if we're the master, we'll do that in master_start below.
	 *  if we're a slave, we'll do it in slave_start below).
	 */
	bcnd	ne0, r22, slave_start
	/* fall through to master start if that's appropriate */

/************************************************************************/

#if defined(RAW_PRINTF) && RAW_PRINTF
	bsr  replace_mayput_with_rawputchar
#endif

_LABEL(master_start)

        /* pick up the master stack */
	or.u	r31, r0,  hi16(_intstack_end)
	or	r31, r31, lo16(_intstack_end)
	clr	r31, r31, 3<0>	/* round down to 8-byte boundary */

	/*
	 * Want to make the call:
	 * 	vector_init(VBR, vector_list)
	 */
	or.u	r3, r0,  hi16(vector_list)
	or	r3, r3, lo16(vector_list)
	bsr.n	_vector_init
	ldcr	r2, VBR		
	
       /* PIO stuff */
        or      r10, r0, PIO0_MODED		/* initialize pio 0 */
        or.u    r11, r0, hi16(OBIO_PIO0)
        st.b    r10, r11, lo16(OBIO_PIO0)

	/* read dispswitch setting */
	ld.bu	r10, r11, lo16(OBIO_PIO0A)	/* dipsw-1 (from portA) */
	mak	r10, r10, 0<8>			/* shift left 8 bit */
	ld.bu	r12, r11, lo16(OBIO_PIO0B)	/* dipsw-2 (from portB) */
	or	r10, r10, r12

	or.u	r11,r0,hi16(_dipswitch)
	st.h	r10,r11,lo16(_dipswitch)

	or	r10,  r0, PIO1_MODEO		/* initialize pio1 */
	or.u	r11,  r0, hi16(OBIO_PIO1)
	st.b	r10, r11, lo16(OBIO_PIO1)
	or	r10,  r0, POWER_ON*2+1		/* port c bit 1 on */
	st.b	r10, r11, lo16(OBIO_PIO1)

	or.u	r10,  r0, hi16(SCSI_ADDR+SCSI_INTS)	/* clear scsi int */
	ld.b	r11, r10, lo16(SCSI_ADDR+SCSI_INTS)
	st.b	r11, r10, lo16(SCSI_ADDR+SCSI_INTS)

        /* write 0x41000000 to escape rom */
        or.u    r2,  r0,  hi16(0x41000000)
        st      r0,  r2,  lo16(0x41000000)
	/* make the call: luna_init() */
	bsr.n	_luna_init
	subu	r31, r31, 40
	addu	r31, r31, 40

        /* we now know our cpu number, so we
	   can set interrupt_stack[cpu_number()] = _intstack
        */
 	ldcr	r10, SR1
	mak     r10, r10, FLAG_CPU_FIELD_WIDTH<0>    /* r10 <-- CPU# */
	/* figure interrupt_stack[cpu_number()] */
	or.u	r11,  r0, hi16(_interrupt_stack)  
	or	r11, r11, lo16(_interrupt_stack)
        or.u	r12, r0,  hi16(_intstack)
        or	r12, r12,  lo16(_intstack)
	st	r12, r11 [r10]

	/* make the call: setup_main() */
        /* when we leave we haven't released the _inter_processor_lock 
	   so all the slaves are stuck spinning. They will be relased
	   when we call start_other_cpus in kern/startup.c */
	bsr.n	_setup_main   /* returns initial thread in r2 */
	subu	r31, r31, 40

       /* never return */
       
/*****************************************************************************/
/* I don't know why this stuff is here -- why not in some C file? -- jfriedl */

	data
LABEL(_intstack)        zero    INTSTACK_SIZE /* initial interrupt stack */
LABEL(_intstack_end)                          /* top of stack */
LABEL(_slavestack)      zero    INTSTACK_SIZE /* initial find my cpu number stack */   
LABEL(_slavestack_end)
LABEL(_dipswitch)	word	0
LABEL(_boothowto)	word	0

	text

/***********************************************************************/
/*
 *	slave CPU starts here
 */
_LABEL(slave_start)
        /* while holding the inter_processor_lock, the slave cpu
	   can find use the slavestack to call slave_pre_main and
	   determine its cpu number. After that, however, it should
	   switch over to the interrupt stack associated with its
           cpu */

	/* r31 <-- slavestack */
	or.u	r31,  r0, hi16(_slavestack_end)
        or      r31, r31, lo16(_slavestack_end)
	clr	r31, r31, 3<0>	/* round down to 8-byte boundary */

        bsr.n    _slave_pre_main /* set cpu number */
        subu    r31, r31, 48    /* allocate frame */

	bsr	_get_slave_stack
	addu	r31, r2, INTSTACK_SIZE

        /*      SR1 now contains our cpu number. We can now release
		the inter_processor_lock, as we are done with the 
		slavestack.  We also have an interrupt stack  */

	or.u	r10,  r0, hi16(_inter_processor_lock)
	st	 r0, r10, lo16(_inter_processor_lock)	

	br.n	_slave_main     /* does not return */
	subu	r31, r31, 40    /* allocate frame */


/*
 *	thread_bootstrap:
 *
 *	Bootstrap a new thread using the thread state that has been
 *	placed on the stack.  Our fp has been set up for us, we only need
 *	to fix up a few things in the saved frame, then get into
 *	usermode.
 */
ENTRY(thread_bootstrap)
        /* 
	 * Here r31 should point to the place on our stack which
	 * contains a pointer to our exception frame.
	 */	
#if MACH_KDB
        ENTRY_ASM
#endif
	br      return_from_exception_handler

/*
 *	save_context
 */
ENTRY(save_context)
	subu	r31,r31,40		/* allocate stack for r1 and args */
	st	r1,r31,36		/* save return address */
	bsr	_spl			/* get the current interrupt mask */
	ld	r1,r31,36		/* recover return address */
	addu	r31,r31,40		/* put stack pointer back */
	ldcr	r10,SR0			/* r10 <- current_thread() */
	ld	r10,r10,THREAD_PCB	/* r10 <- pcb */
#if (PCB_KERNEL!=0)
     	addu	r10, r10, PCB_KERNEL    /* point to kernel save region */
#endif
	st	r1,r10,0	/* do setjmp */ /* save return address */
	st	r14,r10,4
	st	r15,r10,2*4
	st	r16,r10,3*4
	st	r17,r10,4*4
	st	r18,r10,5*4
	st	r19,r10,6*4
	st	r20,r10,7*4
	st	r21,r10,8*4
	st	r22,r10,9*4
	st	r23,r10,10*4
	st	r24,r10,11*4
	st	r25,r10,12*4
        /* In principle, registers 26-29 are never manipulated in the
           kernel. Maybe we can skip saving them? */
	st	r26,r10,13*4
	st	r27,r10,14*4
	st	r28,r10,15*4
	st	r29,r10,16*4
	st	r30,r10,17*4		/* save frame pointer */
	st	r31,r10,18*4		/* save stack pointer */
	st	r2,r10,19*4		/* save interrupt mask */
       /* we need to switch to the interrupt stack here -
	  extract our cpu number */
 	ldcr	r10, SR1
	mak     r10, r10, FLAG_CPU_FIELD_WIDTH<0>    /* r10 <-- CPU# */
	/* figure interrupt_stack[cpu_number()] */
	or.u	r11,  r0, hi16(_interrupt_stack)  
	or	r11, r11, lo16(_interrupt_stack)
	ld	r31, r11 [r10]
        addu    r31, r31, INTSTACK_SIZE             /* end of stack */
	clr	r31, r31, 3<0>	/* round down to 8-byte boundary */
	jmp.n	r1
	or	r2,r0,r0

/* ------------------------------------------------------------------------ */
/*
 * unsigned measure_pause(volatile int *flag)
 *
 * Count cycles executed until *flag becomes nonzero.
 * Return the number of cycles counted.
 */
ENTRY(measure_pause)
	/* R2 is pointer to flag */
	def GRANULAIRTY, 10000

	or	r3, r0, 1	/* r3 is my counter, this is the first */

    measure_pause_outer_loop:
	or	r4, r0, GRANULAIRTY

    measure_pause_inner_loop:
	/*
	 * Execute a tight loop of a known number of cycles.
	 * This assumes, of course, that the instruction cache is on.
	 * This loop takes two cycles per iteration.
	 */
	bcnd.n	ne0, r4, measure_pause_inner_loop
	subu	r4, r4, 1


	/*
	 * Now add the number of cycles done above (plus the overhead
	 * of the outer loop) to the total count.
	 * Also, check the *flag and exit the outer loop if it's non-zero.
	 *
	 * The overhead is really unknown because it's not known how
	 * the memory system will tread the access to *flag, so we just
	 * take a guess.
	 */
	ld	r4, r2, r0			/* get the flag */
	addu	r3, r3, (GRANULAIRTY * 2 + 10)	/* account for the cost */
	bcnd	eq0, r4, measure_pause_outer_loop /* continue or exit the loop*/

	jmp.n	r1
	or	r2, r3, r0	/* pass count back */

/*
 * void delay_in_microseconds(int count)
 *
 * The processor loops (busy waits) for the given number of microseconds:
 * Thus, delay_in_microseconds(1000000) will delay for one second.
 *
 * REGISTER USAGE:
 *  IN  r1   - return address
 *  IN  r2   - (signed int) number of microseconds
 *      r3   - (float) number of microseconds
 *      r4/5 - (double) number of cycles per microsecond
 *      r6   - (float) number of cycles to delay
 *	r7   - (signed) number of cycles to delay
 */
ENTRY(delay_in_microseconds)
ENTRY(delay)
	flt.ss	 r3, r2	    /* convert microseconds from signed int to float */
	or.u	 r4, r0, hi16(_cycles_per_microsecond)
	ld.d	 r4, r4, lo16(_cycles_per_microsecond)
	fmul.ssd r6, r3, r4 /* convert microseconds to cycles */
	int.ss   r7, r6	    /* convert cycles from float to signed int */
	subu	 r7, r7, 25 /* subtract for overhead of above instruction */

	/* now loop for the given number of cycles */
    pause_loop:
	bcnd.n	gt0, r7, pause_loop
	subu	r7, r7, 2	/* two cycles per iteration */

	jmp	r1 /* return */

/*
 * void switch_to_shutdown_context(thread_t thread,
 *				   void (*routine)(processor_t),
 *				   processor_t processor)
 *
 * saves the kernel context of the thread,
 * switches to the interrupt stack,
 * continues the thread (with thread_dispatch),
 * then runs routine on the interrupt stack.
 *
 */
  
ENTRY(switch_to_shutdown_context)
/* call save_context to save the thread state */
     	subu 	r31, r31, 40
	or      r25, r3, r0    /* save arguments */
	or	r24, r4, r0     
     	bsr.n 	_save_context
	st	r1, r31, 36
     	addu 	r31, r31, 40
	ldcr	r10, SR0			/* r10 <- current_thread() */
	st      r31, r10, THREAD_KERNEL_STACK   /* save stack pointer */      
	st	r0,  r10, THREAD_SWAP_FUNC      /* null continuation */
        ldcr	r11, SR1
	mak	r11, r11, FLAG_CPU_FIELD_WIDTH<0> /* r1 = cpu # */
	or	r12, r12, lo16(_interrupt_stack)
	ld	r31, r12 [r11]
        addu    r31, r31, INTSTACK_SIZE             /* end of stack */
	clr	r31, r31, 3<0>	/* round down to 8-byte boundary */
        /* save the thread; switched to the interrupt stack; now call thread
	   dispatch to get rid of this thread */
	or	r2, r10, r0      
	bsr.n	_thread_dispatch
     	subu 	r31, r31, 40
  	/* call the continuation routine */
  	jsr.n	r25
	or	r2, r24, r0
        /* panic if here */
        or.u	r2, r0, hi16(1f)
	bsr.n	_panic
	or	r2, r2, lo16(1f)
1:
  	string "switch_to_shutdown_context"
