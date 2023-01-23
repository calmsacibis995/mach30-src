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
 * HISTORY
 * $Log:	pcb.c,v $
 * Revision 2.7  93/01/26  18:01:06  danner
 * 	ANSIfied, cleaned up includes.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.6  92/08/03  17:52:26  jfriedl
 * 	Support for new pmap module [danner]
 * 
 * Revision 2.5  92/05/21  17:22:51  jfriedl
 * 	Appended 'U' to constants that would otherwise be signed.
 * 	Fixed bug in setting count for flavor == M88100_EXC_STATE.
 * 	Removed unused variable 'stack' from stack_handoff().
 * 		Added prototypes.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.4  92/05/04  11:28:07  danner
 * 	Support for gcc 2.x's moptimize-arg-area switch.
 * 	[92/05/02            danner]
 * 
 * 	Add updating of active_stacks, current_thread and active_threads
 * 	 to switch_context. Add additional arguments to Switch_context
 * 	 call to simplify its implementation.
 * 
 * 	Add asserts to stack_handoff and switch_context.
 * 	[92/04/30            danner]
 * 	Added support for m88100_exc_state flavor in thread_getstatus
 * 	 (from jfriedl).
 * 	[92/04/30            danner]
 * 	Updated thread_getstatus to return all three program counters.
 * 	Thread_setstatus can now set both the snip and sfip.
 * 	[92/04/12            danner]
 * 
 * Revision 2.3  92/04/01  10:56:21  rpd
 * 	Implemented thread_set_syscall_return.
 * 	[92/03/24            danner]
 * 	Added pcb_terminate
 * 	[92/03/03            danner]
 * 
 * 	Made pcb zone collectable, added alignment assert.
 * 	[92/03/03            danner]
 * 
 * Revision 2.2  92/02/18  18:03:46  elf
 * 	Added include of mach_kdb.h
 * 	[92/02/06            danner]
 * 
 * 	Added calls to actually set up the cmmu in switch_context and
 * 	 stack handoff.
 * 	[92/02/06            danner]
 * 
 * 	Added stack_attach,stack_detach, stack_handoff, switch_context.
 * 	[92/02/01            danner]
 * 	Removed thread_start (MI under continuation model).
 * 	[92/01/30            danner]
 * 	Added set_user_regs, user_stack_low to support out of kernel
 * 	 bootstrap task.
 * 	[92/01/20            danner]
 * 
 */

/* Pcb related functions */

#include <luna88k/machdep.h>		/* lots of stuff */
#include <kern/assert.h>	        /* assert */
#include <kern/zalloc.h>	        /* zone_t */
#include <mach/m88k/boolean.h>          /* boolean.h */
#include <kern/mach_param.h>	        /* THREAD_MAX */
#include <kern/thread.h>		/* thread_t */
#include <motorola/m88k/m88100/pcb.h>	/* pcb */
#include <motorola/m88k/m88100/psl.h>	/* PSR_USER */

static zone_t pcb_zone;

/* 
 * Initialize the zone for the PCB package
 */
void pcb_module_init(void)
{
    /*
     * There are various variables that Omron wants set differently from
     * compile-time values set in the machine independent files...
     * we'll change those here.
     */
    extern boolean_t zone_ignore_overflow;
    zone_ignore_overflow = TRUE;

    /*
     * allocate initialize the zone - make it 8+ so we can align properly
     */
    pcb_zone = zinit(sizeof(struct pcb)+8,
    THREAD_MAX  * (8+sizeof(struct pcb)),
    THREAD_CHUNK  * (8+sizeof(struct pcb)),
    FALSE, "m88100 pcb state");
}


/*
 * Return prefered address of user stack.
 * Always returns low address.  If stack grows up,
 * the stack grows away from this address;
 * if stack grows down, the stack grows towards this
 * address.
 */

/*
  Initialize the a PCB for the given thread. 
  The kernel stack pointer is already live in thread->kernel_stack.
*/

#define PCB_MAGIC ((unsigned int) 0xf0f0f0f1U)

void pcb_init(thread_t thread)
{
    register struct pcb	*pcb;
    register struct m88100_saved_state	*saved_state;

    unsigned int	*p;

    p = (unsigned int *) zalloc(pcb_zone);
    if (p == (unsigned int *) 0) 
	panic("pcb_init");
    *p = PCB_MAGIC;
    if ((((int) p) & 0x7)==0)
    {
	/* has proper alignment */
	pcb = (struct pcb *)(p + 2);
	p[1] = 0;
    }
    else
	pcb = (struct pcb *)(p + 1);

    assert ((((int) pcb) & 0x7) == 0);
    thread->pcb = pcb;

    bzero((vm_offset_t)pcb, sizeof(struct pcb));

    /*
     *	Set up thread to start at user bootstrap.  The frame
     *	pointer is setup to point to the frame that corresponds
     *	to the user's state (thread_bootstrap will perform a
     *	ret instruction to simulate returning from a trap).
     *
     */
    pcb->kernel_state.pcb_pc = (unsigned) thread_bootstrap;
    saved_state = (struct m88100_saved_state *) (&(pcb->user_state));
    /*
     * We need to record on the stack a pointer to the exception
     * frame. Since we are returning to a naked stack, this should
     * just be a doubleword below the top of the stack.
     */
    pcb->kernel_state.pcb_sp = (unsigned int) thread->kernel_stack
				+ KERNEL_STACK_SIZE - 8;
    *((int *) (pcb->kernel_state.pcb_sp)) = (int)saved_state;
    pcb->kernel_state.pcb_mask = spl_mask(0); 

    /*
     *	Initialize the PCB versions of the mapping information
     */

    /*
     *	Guarantee that the bootstrapped thread will be in user
     *	mode (this psl assignment above executes the bootstrap
     *	code in kernel mode).  Note, this is the only user register
     *	that we set.  All others are assumed to be random unless
     *	the user sets them.
     */
    saved_state->epsr = PSR_USER;
    saved_state->mode = 0;
    saved_state->mask = spl_mask(0);
    saved_state->dmt0 = 0;
    /* hopefully,  sfip and snip are set elsewhere [setregs()?] */
}

/*
 * Return a pcb 
 */
void pcb_terminate(thread_t thread)
{
    unsigned int *p = (unsigned int *) thread->pcb;
    p--;
    if (*p != PCB_MAGIC)
	p--;
    assert (*p == PCB_MAGIC);
    zfree(pcb_zone,(vm_offset_t)p);
}

/*
 *	Set up the context for the very first thread to execute
 *	(which is passed in as the parameter).
 */
void initial_context(thread_t thread)
{
    active_threads[cpu_number()] = thread;
    pmap_activate(vm_map_pmap(thread->task->map), thread, cpu_number());
}


/*
 *	thread_setstatus:
 *
 *	Set the status of the specified thread.
 */
kern_return_t thread_setstatus(
    thread_t	   thread,
    int		   flavor,
    thread_state_t tstate,
    unsigned int   count)
{
    struct m88100_thread_state *state;
    struct m88100_saved_state *saved_state;

    /* flavor '0' is compatibility code for old interface. */
    if (flavor != 0) {
	if (flavor != M88100_THREAD_STATE || count < M88100_THREAD_STATE_COUNT)
	    return(KERN_INVALID_ARGUMENT);	/* invalid size structure */
    }

    state = (struct m88100_thread_state *) tstate;
    saved_state = USER_REGS(thread);

    bcopy((char *)(&state->r[0]),
	  (char *)(&saved_state->r[0]),
	  32 * sizeof(int));

    /* ignore sxip */
    saved_state->snip = state->snip;
    saved_state->sfip = state->sfip;
    saved_state->epsr = (saved_state->epsr & ~ PSR_SET_BY_USER) 
			| (state->psr & PSR_SET_BY_USER);
    return(KERN_SUCCESS);
}

/*
 *	thread_getstatus:
 *
 *	Get the status of the specified thread.
 */
kern_return_t thread_getstatus(
    thread_t	   thread,
    int		   flavor,
    thread_state_t tstate,	/* pointer to OUT array */
    unsigned int   *count)	/* IN/OUT */
{
    /* flavor '0' is compatibility code for old interface. */

    if (flavor == 0 || flavor == M88100_THREAD_STATE)
    {
	struct m88100_thread_state *state;
	struct m88100_saved_state  *saved_state;

	if (flavor && *count < M88100_THREAD_STATE_COUNT)
	    return(KERN_INVALID_ARGUMENT);

	state = (struct m88100_thread_state *) tstate;
	saved_state = USER_REGS(thread);

	bcopy((char *)(&saved_state->r[0]),
	      (char *)(&state->r[0]), 32 * sizeof(int));
	state->sxip = saved_state->sxip;
	state->snip = saved_state->snip;
	state->sfip = saved_state->sfip; 
	state->psr  = saved_state->epsr;
	state->fpsr = saved_state->fpsr;
	state->fpcr = saved_state->fpcr;

	*count = M88100_THREAD_STATE_COUNT;
	return(KERN_SUCCESS);
    }
    else if (flavor == M88100_EXC_STATE)
    {
	register struct m88100_exc_state	*state;
	register struct m88100_saved_state	*saved_state;

	if (*count < M88100_EXC_STATE_COUNT)
	    return(KERN_INVALID_ARGUMENT);

	state = (struct m88100_exc_state *)tstate;
	saved_state = USER_REGS(thread);

	state->ssbr   = saved_state->ssbr;
	state->dmt0   = saved_state->dmt0;
	state->dmd0   = saved_state->dmd0;
	state->dma0   = saved_state->dma0;
	state->dmt1   = saved_state->dmt1;
	state->dmd1   = saved_state->dmd1;
	state->dma1   = saved_state->dma1;
	state->dmt2   = saved_state->dmt2;
	state->dmd2   = saved_state->dmd2;
	state->dma2   = saved_state->dma2;
	state->fpecr  = saved_state->fpecr;
	state->fphs1  = saved_state->fphs1;
	state->fpls1  = saved_state->fpls1;
	state->fphs2  = saved_state->fphs2;
	state->fpls2  = saved_state->fpls2;
	state->fppt   = saved_state->fppt;
	state->fprh   = saved_state->fprh;
	state->fprl   = saved_state->fprl;
	state->fpit   = saved_state->fpit;
	state->vector = saved_state->vector;

	*count = M88100_EXC_STATE_COUNT;
	return(KERN_SUCCESS);
    }

    return(KERN_INVALID_ARGUMENT);
}

/*
 * Alter the threads state so that a thread_exception_return
 * will return the specified code.  
 */
void thread_set_syscall_return(thread_t t, kern_return_t r)
{
    t->pcb->user_state.r[2] = r;
}

/*
 * Support for bootstrap task.
 */

vm_offset_t user_stack_low(vm_size_t stack_size)
{
    return (VM_MAX_ADDRESS - stack_size);
}

/*
 * Allocate argument area and set registers for first user thread.
 */
vm_offset_t set_user_regs(
    vm_offset_t	stack_base,	/* low address */
    vm_offset_t	stack_size,
    int		*entry,
    vm_size_t	arg_size)
{
    vm_offset_t	arg_addr;
    register struct m88100_saved_state *saved_state;

    /* align on 4 word boundry */
    arg_size = (arg_size + 4*sizeof(int) - 1) & ~((4*sizeof(int))-1);
    arg_addr = stack_base + stack_size - arg_size;

    saved_state = USER_REGS(current_thread());
    bzero((char*)saved_state,sizeof(struct m88100_saved_state));

    /* saved_state->snip = 0; */
    saved_state->sfip = (entry[0] | FIP_V) & ~ FIP_E;
    saved_state->epsr =  0x3f0; 
    saved_state->r[31] = arg_addr; /* stack pointer */

    return (arg_addr);
}


/*
 * Stack_attach(Thread, Stack, Continuation)
 * setup the kernel stack and (exception frame) for
 * (kernel mode) thread Thread so when switch_context(<null>,thread)
 * is called, Thread is resumed at function Continuation running
 * on kernel stack Stack with the current thread as the argument.
 *
 * Here we (1) setup the stack (compute the stack pointer, store
 * the pointers to the user continuation), (2) set up the return 
 * address. The establishment of the return value is done in
 * switch_context
 * 
 */
void stack_attach(
    thread_t thread,
    vm_offset_t kernel_stack,
    void (*continuation)())
{
    register pcb_t pcb = thread->pcb;
    register vm_offset_t sp = kernel_stack+KERNEL_STACK_SIZE-8;

    thread->kernel_stack = kernel_stack;  /* record stack assignment */
    pcb->kernel_state.pcb_pc = (unsigned int) continuation; 
    /* establish return address */
    /* record exception link */
    *(unsigned int **) sp = (unsigned int *) &(thread->pcb->user_state); 

    /* record twice for debugging */
    #if MACH_KDB
      *((unsigned int **)sp+1) = (unsigned int *) &(thread->pcb->user_state); 
    #endif

    /*
     * Decrement the stack pointer by 32 bytes, leaving space for a argument
     * save area. Needed for the moptimize-arg-area option of gcc 2.x
     */
    pcb->kernel_state.pcb_sp = sp - 32; /* establish stack pointer */
    return;
}
  
/*
 * Resume the new thread on its preserved kernel stack.
 * If continuation is nonnull, save the current thread (=old)
 * thread register state. In here we handle everything except
 * updating the register state. That dirty work (setjmp/longjmp pair,
 * essentially is handled in assembler (in continuation.s).
 */
thread_t switch_context(thread_t old, void (*continuation)(), thread_t new)
{

    task_t old_task, new_task;

    if ((old_task = old->task) == (new_task = new->task)) 
	PMAP_CONTEXT(vm_map_pmap(new_task->map), new);
    else 
    {
	PMAP_DEACTIVATE_USER(vm_map_pmap(old_task->map),old, cpu_number());
	PMAP_ACTIVATE_USER(vm_map_pmap(new_task->map), new, cpu_number());
    }

    /* 
     * Record the current thread (old) is blocking with
     * continuation (continuation).
     */
    old->swap_func = continuation;

    /* 
     * Disable interrupts before calling Switch_context.
     * It will return with interrupts enabled.
     * Set active_threads, current_thread, active_stacks here.
     * To reduce the amount of computation that has to be done
     * Switch_context, will also pass &(old->pcb->kernel) and
     * &(new->pcb->kernel) as arguments. 
     *
     * Interrupts are disabled because the stack and
     * current thread are inconsistent. 
     */

    disable_interrupts_return_psr();

    /* lots of paranoia */
    assert(old->kernel_stack == current_stack());
    assert(current_thread() == old);
    assert(new->kernel_stack ==
	(new->pcb->kernel_state.pcb_sp & ~(KERNEL_STACK_SIZE-1)));

    set_current_thread(new);
    active_threads[cpu_number()] = new;
    active_stacks[cpu_number()] = new->kernel_stack;
    return Switch_context(old, continuation, new, 
			 &(old->pcb->kernel_state), &(new->pcb->kernel_state));
}

/*
 * Return the specified threads kernel stack
 * Set old->kernel_stack to null.
 */
vm_offset_t stack_detach(thread_t old)
{
    vm_offset_t stack = old->kernel_stack;
    old->kernel_stack = (vm_offset_t) 0;
    return stack;
}

void stack_handoff(thread_t old, thread_t new)
{
    task_t old_task, new_task;
    m88k_psr_type psr;

    if ((old_task = old->task) == (new_task = new->task)) 
	PMAP_CONTEXT(vm_map_pmap(new_task->map), new);
    else 
    {
	PMAP_DEACTIVATE_USER(vm_map_pmap(old_task->map),old, cpu_number());
	PMAP_ACTIVATE_USER(vm_map_pmap(new_task->map), new, cpu_number());
    }

    /* update current_thread, active_threads */
    psr = disable_interrupts_return_psr();
    set_current_thread(new);
    active_threads[cpu_number()] = new;
    new->kernel_stack = stack_detach(old);

    assert(new->kernel_stack == current_stack());

    set_psr(psr);
    return;
}
