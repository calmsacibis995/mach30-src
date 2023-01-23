/* 
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 * Exception handler entry routines. Many ideas from both omron sources
 * and encore sources.
 */
/*
 * HISTORY
 * $Log:	lotrap.s,v $
 * Revision 2.3  93/01/26  18:00:57  danner
 * 	Tidy.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/02/18  18:03:36  elf
 * 	Created. 
 * 	[92/02/01            danner]
 * 
 */

#define ASSEMBLER /* this is required for some of the include files */
#include <machine/locore/locore.h>	
#include <motorola/m88k/m88100/m88k.h>
#include <machine/trap.h>          	/* for T_ defines */
#include <assym.s>   	  		/* for PCB_KSP, etc */
#include <mach/machine/vm_param.h>	/* for KERNEL_STACK_SIZE */
#include <kern/syscall_emulation.h>
#include <mach/kern_return.h>

/*
 * Control register usage;
 *
 * SR0 (cr17) contains various flags, including the cpu number
 * SR1 (cr18) contains the thread pointer
 * SR2 (cr19) and SR3 are generally free.
 *
 * Upon entry to an exception handler, the exception handler stores
 * r1 into SR2 and then calls exception_frame, which creates an exception
 * frame and stores all nonfp shadow registers.
 * The odd ball is the data_access_exception handler, which does some
 * funny things to support badaddr.
 *	
 * Upon return from exception_frame r1 is free, r30 points to the exception
 * frame and r31 points to the kernel stack. Also *r31 = r30
 * Most routines will now call fp_drain to drain the floating point pipe and
 * reenable the fpu.
 * Then save_regs is called to save all the user registers. It also handles
 * any data access faults that have happened.
 * Traps however, need not do this.
 */

#define PREP(NAME,num) 	\
LABEL(NAME)		NEWLINE \
	bsr.n	exception_frame NEWLINE \
	stcr	r1, SR2 	NEWLINE \
	or	r1, r0, num	NEWLINE \
	st	r1, r30, REG_OFF(EF_VECTOR)	

#define EXCEPTION_RETURN \
	bsr.n	return_from_exception
	ld	r30, r31, 0
	
	
PREP(interrupt_handler,1)
	bsr	_fp_drain
	bsr	_save_regs
	CALL(_pre_ext_int, 1, r30)
	EXCEPTION_RETURN

PREP(instruction_access_handler)
	bsr	_fp_drain
	bsr	_save_regs
	CALL(_pre_ext_int, 1, r30)
/* back up the pc's to restart the instruction	
	ld	r30, r31, 0
	; Now, to retry the instruction
	; Copy the SNIP to the SFIP, clearing the E bit.
	; Copy the SXIP to the SNIP, clearing the E bit.
	ld	r1, r30, REG_OFF(EF_SNIP)
	ld	r2, r30, REG_OFF(EF_SXIP)
	clr	r1, r1, 1<RTE_ERROR_BIT>
	clr	r2, r2, 1<RTE_ERROR_BIT>
	st	r1, r30, REG_OFF(EF_SFIP)
	st	r2, r30, REG_OFF(EF_SNIP)
	EXCEPTION_RETURN

/*
 * If the IGNORE_DATA_ACCESS bit is set in the flags,
 * we jump to ignore_data_access and just skip this
 * exception
 */

LABEL(data_acesss_handler)
	stcr	r1, SR2      ; save r1
	ldcr	r1, FLAGS    ; get the flags
	bb1.n   FLAG_IGNORE_DATA_EXCEPTION, r1, ignore_data_exception
        ldcr	r1, SR2      ; restore r1
PREP(data_access_handler2)
	bsr	_fp_drain
	bsr	_save_regs
	EXCEPTION_RETURN
	
PREP(misaligned_exception,4)
	ldcr	r1,sxip			; get instruction address
	ldcr	r2,epsr
	bb1.n	31,r2,1f		; were we in kernel mode ?
	clr	r1,r1,2<0>		; clear valid and error bits

	ld.usr	r1,r1,r0		; get offending instruction
	tb1	0, r0, 0		; wait for pipe.
	br	2f
1:
	ld	r1,r1,0			; get instruction from kernel space
2:
	bb0.n	31,r1,3f		; immediate addressing mode ?
	extu	r2,r1,5<21>		; extract destination register number

	rot	r1,r1,<16>		; not immediate -- shift opcode bits
3:
	bb1	29,r1,5f		; st ? (st doesn't set sb bits)

	extu	r1,r1,3<26>		; xmem or ld
	xor	r1,r1,4			; ld.d sets 2 sb bits
	bcnd.n	ne0,r1,4f		; not ld.d ?
	or	r2,r2,0x20		; clear 1 bit only

	xor	r2,r2,0x60		; ld.d -- clear 2 bits
4:
	ldcr	r1,ssbr
	clr	r1,r1,r2		; clear ssbr bits
	stcr	r1,ssbr

5:
	bsr	_fp_drain		; drain FP pipeline
	bsr	_save_regs
        CALL(_pre_trap,T_MISALGNFTL, r30)				
	EXCEPTION_RETURN

PREP(unimplemented_handler,5)
	bsr	_fp_drain
	bsr	_save_regs
	CALL(_pre_trap,T_ILLFLT, r30)
	EXCEPTION_RETURN


LABEL(ignore_data_exception)
	/* ignore the data access exception, and just return to
	   badaddr__return_nonzero. We are allowed to trash
	   r2-r9. */
	or.u	r2, r0, hi16(badaddr__return_nonzero + 2)
	or  	r2, r2, lo16(badaddr__return_nonzero + 2)
	stcr	r2, SNIP	; Make it the next instruction to execute
	addu	r2, r2, 4
	stcr	r2, SFIP	; and the next one after that, too.
	stcr	r0, SSBR	; make the scoreboard happy.
	stcr	r0, DMT0	; just to make sure
	RTE
