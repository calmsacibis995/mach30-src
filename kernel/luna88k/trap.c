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
 * $Log:	trap.c,v $
 * Revision 2.14  93/03/09  17:58:38  danner
 * 	expanded debug prints.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.13  93/02/05  07:51:06  danner
 * 	Fixed some naughty debugging code.
 * 	[93/02/04            jfriedl]
 * 
 * Revision 2.12  93/01/26  18:03:36  danner
 * 	ANSIfied, cleaned up includes.
 * 	[93/01/23            jfriedl]
 * 
 * 	Added interrupted_pc()
 * 	[93/01/21            jfriedl]
 * 
 * Revision 2.11  93/01/14  17:39:06  danner
 * 	Some reorginization. Better error messages for debugging.
 * 	Added error_fault().
 * 	[92/12/07            jfriedl]
 * 
 * 	Cleanup to quiet more gcc warnings.
 * 	[92/08/12            jfriedl]
 * 
 * Revision 2.10  92/08/03  17:45:01  jfriedl
 * 	Added watchpoint support.
 * 	[92/08/02            jfriedl]
 * 
 * 	Adjusted references from luna88/locore.
 * 	Very Major cleanup. Condensed history.
 * 
 * 	Condensed 3.0 History:
 * 	  Add continuations for user page faults.	[92/05/04  danner]
 * 	  Was treating user breakpoint instructions like entry breakpoints,
 * 	    causing user space single stepping to act like noops.
 * 	  Removed syscall, mach_syscall and other 2.5 cruft. [92/03/03 danner]
 * 	  Added code for ddb traps from userspace. [92/02/18 danner]
 * 	  Removed T_ASTFLT case from trap, as ast_taken() is called
 * 	   directly from assembly in 3.0 instead. [91/11/03 danner]
 * 	  Support for user level ddb traps. [91/09/19 13:51:09  danner]
 * 	  Support for ddb instead of jkdb.  [91/03/09 danner]
 * 
 */

#include <luna88k/machdep.h>
#include <luna88k/exvec.h>		/* for EVN_* defines */
#include <mach/exception.h>		/* for EXC_* defines */
#include <kern/xpr.h>			/* XPR */
#include <motorola/m88k/m88100/m88100.h>/* DMT_VALID, etc. */
#include <motorola/m88k/m88100/psl.h>	/* FIP_E, etc. */
#include <vm/vm_kern.h>			/* kernel_map */
#include <vm/vm_fault.h>		/* vm_fault */
#ifdef MACH_KDB
 #include <machine/db_machdep.h>	  /* db_* stuff */
 #include <ddb/db_output.h>		  /* db_printf */
 #include <ddb/db_task_thread.h>	  /* db_lookup_task */
 #include <ddb/db_watch.h>		  /* db_find_watchpoint */
#endif

/* in ddb/db_watch.c */
extern struct db_watchpoint *db_watchpoint_list;
extern boolean_t db_watchpoints_inserted;

int stop_on_user_memory_error = 0;

#define TRAPTRACE
#if defined(TRAPTRACE)
unsigned traptrace = 0;
#endif

#if MACH_KDB
#define DEBUG_MSG db_printf
#else
#define DEBUG_MSG printf
#endif

#ifdef JEFF_DEBUG
# undef  DEBUG_MSG
# define DEBUG_MSG raw_printf
#endif

#define FROM_USERMODE(PSR)   (((struct psr*)&(PSR))->psr_mode == 0)
#define FROM_SYSTEMMODE(PSR) (((struct psr*)&(PSR))->psr_mode != 0)

/*
 * For debugging messages....
 * Return KERNEL, PAGER, SERVER, or USER as appropriate to the given task.
 * Assumes that the first taks is the microkernel, second is the pager,
 * third is the server, and others are "regular" user tasks.
 */
char *who_is_task(task_t t)
{
    #define KERNEL_TASK 0
    #define PAGER_TASK 1
    #define SERVER_TASK 2
    static char *message[] = {"KERNEL","PAGER","SERVER"};
    static task_t task[]   = { 0,       0,      0};
    static int valid[]     = { 0,       0,      0};

    if (valid[KERNEL_TASK] && t == task[KERNEL_TASK])
	return message[KERNEL_TASK];
    else if (valid[PAGER_TASK] && t == task[PAGER_TASK])
	return message[PAGER_TASK];
    else if (valid[SERVER_TASK] && t == task[SERVER_TASK])
	return message[SERVER_TASK];
    else if (valid[0] && valid[1] && valid[2])
	return "USER";
    else
    {
	int queuenum = db_lookup_task(t);
	if (queuenum < 0 || queuenum > 2)
	    return "USER";
	else
	{
	    task[queuenum] = t;
	    valid[queuenum] = 1;
	    return message[queuenum];
	}
    }
}

/*
 *	Mach trap handler.  Called from locore when a processor trap
 *	occurs.
 */
static void user_page_fault_continue(kern_return_t kr)
{
    struct m88100_saved_state *frame = &current_thread()->pcb->user_state;

    if (kr == KERN_SUCCESS) /* all went well */
    {
#if MACH_KDB
	if (db_watchpoint_list && db_watchpoints_inserted &&
	    (frame->dmt0 & DMT_VALID) &&
	    db_find_watchpoint(current_thread()->task->map,
		    (vm_offset_t)frame->dma0, frame))
	{
            int s = db_splhigh();
	    DEBUG_MSG("[WATCHPOINT hit: writing %x to %x from %x, frame=0x%x]\n",
		    frame->dmd0, frame->dma0, frame->sxip, frame);
            db_enable_interrupt(); /* turn interrupts on */
	    ddb_break_trap(T_KDB_WATCH,(db_regs_t*)frame);
            db_disable_interrupt(); /* shut them back off */
            db_splx(s);
	}
#endif


	/* call data access emulation to handle the fault */
	data_access_emulation((unsigned *)&(current_thread()->pcb->user_state));
	frame->dmt0 = 0;
	if (frame->vector == EVN_INS) /* instruction fault */
	{
	    /* back up SXIP, SNIP clearing the the Error bit */
	    frame->sfip = frame->snip & ~FIP_E;
	    frame->snip = frame->sxip & ~NIP_E;
	}
	thread_exception_return();
    }

    {
	task_t task = current_task();
	char *who = who_is_task(task);
	DEBUG_MSG("[%s FAULT cpu=%d th=%x t=%x R=%x pipe:t=%x/d=%x/a=%x]\n",
	    who, cpu_number(), current_thread(), task,
	    frame, frame->dmt0, frame->dmd0, frame->dma0);
#if MACH_KDB
        if (who[0] != 'U' || stop_on_user_memory_error)
	    gimmeabreak();
#endif
    }

    /*
     * since we're going to send the BAD ACCESS signal,
     * clear DMT0 so that later we don't do anything in
     * data_access_emulation...
     *      jfriedl@omron   90-Mar-12
     */
    frame->dmt0 &= ~DMT_VALID; /* un-validate */

    exception(EXC_BAD_ACCESS, kr, 0);
}

/*ARGSUSED*/
#if MACH_KDB
static void real_trap(unsigned type, struct m88100_saved_state *frame)
#else
static void trap(unsigned type, struct m88100_saved_state *frame)
#endif
{
    unsigned mycpu = cpu_number();

    if (FROM_SYSTEMMODE(frame->epsr))
    {
	/*
	 * Trap in system mode.  Only page-faults are valid,
	 * and then only in special circumstances.
	 */
	if (type == T_INSTFLT || type == T_DATAFLT)
	{
            unsigned fault_addr;
	    unsigned protection;
	    vm_offset_t recover;
	    vm_map_t map;
	    kern_return_t result;

	    /*
	     * Address of the fault is in DMA0 for data faults,
	     * or SXIP for instruction faults.
	     */
	    if (type == T_INSTFLT)
	    {
		fault_addr = frame->sxip & XIP_ADDR;
		protection = VM_PROT_READ;
	    }
	    else
	    {
		fault_addr = frame->dma0;
		if (frame->dmt0 & (DMT_WRITE|DMT_LOCKBAR))
		    protection = VM_PROT_READ|VM_PROT_WRITE;
		else
		    protection = VM_PROT_READ;
	    }

	    /*
	     * If an instruction fault, must be a kernel address.
	     * Also a kernel address if there's no current thread.
	     * Also a kernel address if the faulting data address
	     * was in supervisor mode.
	     */
	    if (type == T_INSTFLT || frame->dmt0 & DMT_DAS ||
		current_thread() == THREAD_NULL)
		    map = kernel_map;
	    else
		    map = current_thread()->task->map;

	    XPR(XPR_VM_FAULT, ("vm_fault(%x,%x) frame=%x pc=%x th=%x\n",
		  map, fault_addr, frame, frame->sxip, current_thread()));


	    result = vm_fault(map, trunc_page((vm_offset_t)fault_addr),
		protection, FALSE, FALSE, 0);

	    if (result == KERN_SUCCESS)
	    {
#if     MACH_KDB
		/* Look for kernel watchpoints */
		if (db_watchpoint_list && db_watchpoints_inserted &&
		    (protection == (VM_PROT_READ|VM_PROT_WRITE)) &&
		    db_find_watchpoint(map, fault_addr, frame))
			ddb_break_trap(T_KDB_WATCH,(db_regs_t*)frame);
#endif  MACH_KDB
		return;
	    }


	    if (current_thread() != THREAD_NULL)
		recover = current_thread()->recover;
	    else
		recover = 0;

	    if (recover == 0)
	    {
		extern unsigned ALLOW_FAULT_START,
				ALLOW_FAULT_END,
				FAULT_ERROR;
	        unsigned *pc = (unsigned *)(frame->sxip & XIP_ADDR);
		if (pc > &ALLOW_FAULT_START && pc < &ALLOW_FAULT_END)
		    recover =  (vm_offset_t)&FAULT_ERROR;
	    }
	    if (recover != 0) {
		frame->snip = ((unsigned)recover    ) | FIP_V;
		frame->sfip = ((unsigned)recover + 4) | FIP_V;
		frame->sxip = 0;
		frame->dmt0 = 0;
		return;
	    }

	    if (type == T_DATAFLT && (frame->dmt0 & DMT_DAS) == 0)
		goto user_fault;
	}
	/*
	 * BAD NEWS --- unknown kernel fault, or couldn't handle above.
         */
	DEBUG_MSG("[BAD KERNEL FAULT cpu=%d type=%d vec=%d%s frame=0x%08x]\n",
		mycpu, type, frame->vector,
		    frame->vector == EVN_DATA     ? " (data access)" :
		    frame->vector == ENV_MISALIGN ? " (misalign)" :
		    "", frame);

	if (frame->vector == EVN_DATA) /* data access exception */
	{
	    DEBUG_MSG("(CPU %d) kernel bus error @0x%08x accessing 0x%08x\n",
		mycpu, frame->sxip, frame->dma0);
	}
	else if (frame->vector == ENV_MISALIGN) /* misaligned access exception*/
	{
	    DEBUG_MSG("(CPU %d) kernel misalgined "
		"access exception @ 0x%08x\n", mycpu, frame->sxip);
	}
	else
	{
	    DEBUG_MSG("(CPU %d) trap type %d, vector=%d,",
		mycpu, type, frame->vector);
	    DEBUG_MSG(" sxip=%x, dma0=%x, dmt0=%x\n",
	        frame->sxip, frame->dma0, frame->dmt0);
	}

#if MACH_KDB
	gimmeabreak();
#endif

	DEBUG_MSG("\n --- CPU %d HALTED --\n", mycpu);
	spl7();
	enable_interrupt();
	while(1)
	    ;
    }
    else
    {
        int exc_type, exc_code, exc_subcode = 0;
        user_fault:
	/*
	 *	Trap from user mode.
	 */

	switch (type)
	{
	  case T_PRIVINFLT:
	    exc_type = EXC_BAD_INSTRUCTION;
	    exc_code = EXC_M88K_PRIV;
	    break;

	  case T_INSTFLT:
	  case T_DATAFLT:
	  {
	    unsigned fault_addr;
	    unsigned protection;

	    if (type == T_INSTFLT)
	    {
		fault_addr = frame->sxip & XIP_ADDR;
		protection = VM_PROT_READ;
	    }
	    else
	    {
		fault_addr = frame->dma0;
		if (frame->dmt0 & (DMT_WRITE|DMT_LOCKBAR))
		    protection = VM_PROT_READ|VM_PROT_WRITE;
		else
		    protection = VM_PROT_READ;
	    }

	    vm_fault(current_thread()->task->map,
		trunc_page((vm_offset_t)fault_addr),
		protection, FALSE, FALSE, user_page_fault_continue);
	    /* NOTREACHED */
#if MACH_KDB
	    DEBUG_MSG("[ ERROR AT FILE %s LINE %d!! ]\n", __FILE__, __LINE__);
#endif
	    exc_type = exc_code = 0; /* this to shut up gcc warnings */
	    break;
	  }

	  case T_MISALGNFLT:
	    exc_type = EXC_BAD_ACCESS;
	    exc_code = EXC_M88K_MISALIGN;
	    break;

	  case T_ILLFLT:
	    exc_type = EXC_BAD_INSTRUCTION;
	    exc_code = EXC_M88K_UNIMP;
	    break;

	  case T_BNDFLT:
	    exc_type = EXC_SOFTWARE;
	    exc_code = EXC_M88K_BOUND;
	    break;

	  case T_ZERODIV:
	    exc_type = EXC_ARITHMETIC;
	    exc_code = EXC_M88K_DIVIDE;
	    break;

	  case T_OVFFLT:
	    exc_type = EXC_ARITHMETIC;
	    exc_code = EXC_M88K_OVERFLOW;
	    break;

	  case T_FPEPFLT:
	    exc_type = EXC_ARITHMETIC;
	    exc_code = EXC_M88K_FLOAT_P;
	    break;

	  case T_FPEIFLT:
	    exc_type = EXC_ARITHMETIC;
	    exc_code = EXC_M88K_FLOAT_I;
	    break;

	  case T_SIGTRAP:
	    exc_type = EXC_BREAKPOINT;
	    exc_code = frame->vector;
	    break;

	  case T_STEPBPT:
	    /*
	     * This trap is used by the kernel to support single-step
	     * debugging (although any user could generate this trap
	     * which should probably be handled differently). When a
	     * process is continued by a debugger with the PT_STEP
	     * function of ptrace (single step), the kernel inserts
	     * one or two breakpoints in the user process so that only
	     * one instruction (or two in the case of a delayed branch)
	     * is executed.  When this breakpoint is hit, we get the
	     * T_STEPBPT trap.
	     */
	    exc_type = EXC_BREAKPOINT;
	    exc_code = frame->vector;

	    frame->sfip = frame->snip;    /* set up next FIP */
	    frame->snip = frame->sxip;    /* set up next NIP */
	    break;

	  case T_USERBPT:
	    /*
	     * This trap is meant to be used by debuggers to implement
	     * breakpoint debugging.  When we get this trap, we just
	     * return a signal which gets caught by the debugger.
	     */
	    exc_type = EXC_BREAKPOINT;
	    exc_code = frame->vector;

	    frame->sfip = frame->snip;    /* set up the next FIP */
	    frame->snip = frame->sxip;    /* set up the next NIP */
	    break;

	  case T_SIGSYS:
	    exc_type = EXC_SOFTWARE;
	    exc_code = EXC_M88K_RESTRAP;
	    exc_subcode = frame->vector;
	    break;

	  case T_KDB_BREAK:
	  case T_KDB_ENTRY:
	  default:
	    exc_type = EXC_SOFTWARE;
	    exc_code = EXC_M88K_UNKNOWN;
	    exc_subcode = type;
	    DEBUG_MSG("[BAD USER TRAP cpu %d type=%d, vector=%d, sxip=%x, frame=%x]\n",
		mycpu, type, frame->vector, frame->sxip, frame);
#if MACH_KDB
	    gimmeabreak();
#endif
	    break;
	}

	if (exc_type == EXC_BAD_INSTRUCTION ||
	    exc_type == EXC_BAD_ACCESS ||
	    exc_type == EXC_SOFTWARE)
	{
	    char *who = who_is_task(current_task());
	    DEBUG_MSG("[cpu %d exception(%d,%d,%d) to %s thread=%x frame=%x]\n",
		mycpu, exc_type, exc_code, exc_subcode, who,
		current_thread(), frame);

	    if (exc_type == EXC_BAD_ACCESS && exc_code == EXC_M88K_MISALIGN)
		DEBUG_MSG("[misalligned at 0x%x (r1=0x%x)\n",
		    frame->sxip, frame->r[1]);
#if MACH_KDB
	    if (who[0] != 'U'/*USER*/ || stop_on_user_memory_error)
	        gimmeabreak();
#endif
	}

	exception(exc_type, exc_code, exc_subcode);
    }
}

void error_fault(struct m88100_saved_state *frame)
{
    int mycpu = cpu_number();
    int thread = (unsigned)current_thread();
    DEBUG_MSG("\n[cpu %d thread %d ERROR FAULT (Bad News[tm]) frame 0x%08x]\n",
	mycpu, thread, frame);
#if MACH_KDB
    gimmeabreak();
    DEBUG_MSG("[you really can't restart after an error fault.]\n");
    gimmeabreak();
#endif
}


#if MACH_KDB
/*
 * typically entered with interrupts turned off
 */
void trap(unsigned type, struct m88100_saved_state *frame)
{

#ifdef JEFF_DEBUG
    /* look for stack overflow */
    #define STACK_MARGIN 1024
    extern unsigned active_stacks[4];
    int i;
    for (i = 0; i < 4; i++) {
	static x = 0;
	if (frame > active_stacks[i] && frame < active_stacks[i]+KERNEL_STACK_SIZE) {
	    if (x == 0 && i != cpu_number()) {
		raw_printf("oops, i is %d, cpu_number is %d\n", i, cpu_number());
		x = 1;
	    }
	    if (frame < active_stacks[i] + STACK_MARGIN) {
		raw_printf("[kernel stack redline hit! cpu=%d,frame=%x,thread=%x,task=%x]\n",
			cpu_number(), frame, current_thread(), current_task());
		gimmeabreak();
	    }
	}
    }
#endif

    if (FROM_SYSTEMMODE(frame->epsr))
    {
	switch (type)
	{
	  case T_KDB_BREAK:
	  {
	    int s = db_splhigh();
	    db_enable_interrupt(); /* turn interrupts on */
	    ddb_break_trap(T_KDB_BREAK,(db_regs_t*)frame);
	    db_disable_interrupt(); /* shut them back off */
	    db_splx(s);
	    return;
	  }
	  case T_KDB_ENTRY:
	  {
	    int s = db_splhigh();
	    db_enable_interrupt(); /* turn interrupts on */
	    ddb_entry_trap(T_KDB_ENTRY,(db_regs_t*)frame);
	    db_disable_interrupt(); /* shut them back off */
	    db_splx(s);
	    return;
	  }

	  case T_ILLFLT:
	  {
	    int s = db_splhigh();
	    db_enable_interrupt(); /* turn interrupts on */
#ifdef JEFF_DEBUG
	raw_printf("unimplemented opcode, frame = %x.\n", frame);
	gimmeabreak();
	if (0)
#endif
	    ddb_error_trap(type == T_ILLFLT ? "unimplemented opcode" :
		"error fault", (db_regs_t*)frame);
	    db_disable_interrupt(); /* shut them back off */
	    db_splx(s);
	    return;
	  }
	  case T_INSTFLT:
	  case T_DATAFLT:
	    /* watch point support code should go here */
	    break;
	}
    }
    else
    {
	switch (type)
	{
	  case T_KDB_ENTRY:
	  {
	    int s = db_splhigh();
	    db_enable_interrupt(); /* turn interrupts on */
	    ddb_entry_trap(T_KDB_ENTRY,(db_regs_t*)frame);
	    db_disable_interrupt(); /* shut them back off */
	    db_splx(s);
	    return;
	  }
	  case T_KDB_BREAK:
	  {
	    int s = db_splhigh();
	    db_enable_interrupt(); /* turn interrupts on */
	    ddb_break_trap(T_KDB_BREAK,(db_regs_t*)frame);
	    db_disable_interrupt(); /* shut them back off */
	    db_splx(s);
	    return;
	  }
	}
    }
#if defined(TRAPTRACE)
    if (traptrace != 0 &&
	(traptrace == 0xffffffffU ||
	 traptrace == (unsigned)current_thread() ||
	 traptrace == (unsigned)current_task()))
    {
	DEBUG_MSG("[TRAP TRACE cpu=%d thread=%x type=%d%s frame=%x "
		"(xip=%x, r1=%x, r31=%x)]\n", cpu_number(), current_thread(),
		type, type == T_INSTFLT ? "(I-fault)" :
			(type == T_DATAFLT ? "(D-fault)" : ""),
		frame, frame->sxip, frame->r[1], frame->r[31]);

        asm("traptracepoint: global traptracepoint");
    }
#endif
    real_trap(type, frame);
}
#endif /* MACH_KDB */

#include "mach_pcsample.h"
#if     MACH_PCSAMPLE > 0
/*
 * return saved state for interrupted user thread
 */
unsigned interrupted_pc(thread_t t)
{
    struct m88100_saved_state *frame = &t->pcb->user_state;
    unsigned sxip = frame->sxip;
    unsigned PC = sxip & ~3; /* clear lower bits which are flags... */
    return PC;
}
#endif  /* MACH_PCSAMPLE > 0*/
