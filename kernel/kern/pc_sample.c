/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log:	pc_sample.c,v $
 * Revision 2.4  93/08/03  12:31:18  mrt
 * 	[93/08/02  16:50:13  bershad]
 * 
 * 	Flavor support for PC sampling.
 * 	[93/07/30  10:23:02  bershad]
 * 
 * Revision 2.3  93/01/27  09:39:48  danner
 * 	take_pc_sample() is void, get_sampled_pcs is static return_t
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  93/01/24  13:19:46  danner
 * 	Lots of changes.
 * 	[93/01/12            rvb]
 * 	Created.
 * 	[92/05/10            cmaeda]
 * 
 */



#include <mach_pcsample.h>

#if 	MACH_PCSAMPLE > 0


#include <mach/mach_types.h>	/* vm_address_t */
#include <mach/std_types.h>	/* pointer_t */
#include <mach/pc_sample.h>
#include <kern/host.h>
#include <kern/thread.h>

#define MAX_PC_SAMPLES 512

typedef sampled_pc_t sampled_pcs[MAX_PC_SAMPLES];

int pc_sampling_enabled = 0;
decl_simple_lock_data(, pc_sampling_lock)	/* lock for enabling */

void take_pc_sample(t, cp, flavor)
    register thread_t t;
    register sample_control_t *cp;
    sampled_pc_flavor_t flavor;
{
    unsigned pc;
    struct sampled_pc *sample;
    
    pc = interrupted_pc(t);
    cp->seqno++;
    sample = &((sampled_pc_t *)cp->buffer)[cp->seqno % MAX_PC_SAMPLES];
    sample->id = (unsigned int)t;
    sample->pc = pc;
    sample->sampletype = flavor;
}

kern_return_t
thread_enable_pc_sampling(thread, tickp, flavors)
    thread_t thread;
    int *tickp;
    sampled_pc_flavor_t flavors;
{
    vm_offset_t buf;
    extern int tick;

    if (thread == THREAD_NULL)  {
	return KERN_INVALID_ARGUMENT;
    }
    if (!thread->pc_sample.buffer)  {
	if (!(int) (buf = (vm_offset_t) kalloc(sizeof (sampled_pcs)))) {
	    printf("thread_enable_pc_sampling: kalloc failed\n");
	    return KERN_INVALID_ARGUMENT;
	}
	thread->pc_sample.buffer = buf;
	thread->pc_sample.seqno = 0;
    }
    *tickp = tick;
    thread->pc_sample.sampletypes = flavors;
    return KERN_SUCCESS;
}

kern_return_t
task_enable_pc_sampling(task, tickp, flavors)
    task_t task;
    int *tickp;
    sampled_pc_flavor_t flavors;
{
    vm_offset_t buf;
    extern int tick;

    if (task == TASK_NULL)  {
	return KERN_INVALID_ARGUMENT;
    }
    if (!task->pc_sample.buffer)  {
	if (!(int) (buf = (vm_offset_t) kalloc(sizeof (sampled_pcs)))) {
	    printf("task_enable_pc_sampling: kalloc failed\n");
	    return KERN_INVALID_ARGUMENT;
	}
	task->pc_sample.buffer = buf;
	task->pc_sample.seqno = 0;
    }
    *tickp = tick;
    task->pc_sample.sampletypes = flavors;
    return KERN_SUCCESS;
}

kern_return_t
thread_disable_pc_sampling(thread, samplecntp)
    thread_t thread;
    int *samplecntp;
{
    vm_offset_t buf;

    if (thread == THREAD_NULL)  {
	return KERN_INVALID_ARGUMENT;
    }
    if (buf = thread->pc_sample.buffer)
	kfree(buf, sizeof (sampled_pcs));
    thread->pc_sample.buffer = (vm_offset_t) 0;
    thread->pc_sample.seqno = 0;
    
    return KERN_SUCCESS;
}

kern_return_t
task_disable_pc_sampling(task, samplecntp)
    task_t task;
    int *samplecntp;
{
    vm_offset_t buf;

    if (task == TASK_NULL)  {
	return KERN_INVALID_ARGUMENT;
    }
    if (buf = task->pc_sample.buffer)
	kfree(buf, sizeof (sampled_pcs));
    task->pc_sample.buffer = (vm_offset_t) 0;
    task->pc_sample.seqno = 0;

    return KERN_SUCCESS;
}

static kern_return_t
get_sampled_pcs(cp, seqnop, sampled_pcs_out, sampled_pcs_cntp)
	sample_control_t *cp;
	sampled_pc_seqno_t *seqnop;
	sampled_pc_array_t sampled_pcs_out;
	int *sampled_pcs_cntp;
{
    int nsamples;
    sampled_pc_seqno_t seqidx1, seqidx2;

    nsamples	= cp->seqno - *seqnop;
    seqidx1	= *seqnop % MAX_PC_SAMPLES;	   /* index of *seqnop */
    seqidx2	= cp->seqno % MAX_PC_SAMPLES;  /* index of cp->seqno */

    if  (nsamples > MAX_PC_SAMPLES) {
	nsamples = MAX_PC_SAMPLES;
	seqidx1  = (seqidx2 + 1) % MAX_PC_SAMPLES;
    }

    if (nsamples > 0)  {
	/*
	 * Carefully copy sampled_pcs into sampled_pcs_msgbuf IN ORDER.
	 */
	if (seqidx1 < seqidx2) {
	    /*
	     * Simple case: no wraparound.
	     * Copy from seqidx1 to seqidx2.
	     */
	    bcopy((sampled_pc_array_t)cp->buffer + seqidx1 + 1,
		  sampled_pcs_out,
		  nsamples * sizeof(sampled_pc_t));
	} else {
	    /* seqidx1 > seqidx2 -- Handle wraparound. */

	    bcopy((sampled_pc_array_t)cp->buffer + seqidx1 + 1,
		  sampled_pcs_out,
		  (MAX_PC_SAMPLES - seqidx1 - 1) * sizeof(sampled_pc_t));

	    bcopy((sampled_pc_array_t)cp->buffer,
		  sampled_pcs_out + (MAX_PC_SAMPLES - seqidx1 - 1),
		  (seqidx2 + 1) * sizeof(sampled_pc_t));
	}
    } else {
	/*  could either be zero because of overflow, or because
	 *  we are being lied to.  In either case, return nothing.
	 *  If overflow, only once in a blue moon. If being lied to,
	 *  then we have no obligation to return anything useful anyway.
	 */
	;
    }
	
    *sampled_pcs_cntp = nsamples;
    *seqnop = cp->seqno;
    return KERN_SUCCESS;
}

kern_return_t
thread_get_sampled_pcs(thread, seqnop, sampled_pcs_out, sampled_pcs_cntp)
	thread_t thread;
	sampled_pc_seqno_t *seqnop;
	sampled_pc_array_t sampled_pcs_out;
	int *sampled_pcs_cntp;
{
    if (thread == THREAD_NULL)
    	return KERN_INVALID_ARGUMENT;

    if ((int)thread->pc_sample.buffer == 0)
        return KERN_FAILURE;

    return get_sampled_pcs(&thread->pc_sample, seqnop, sampled_pcs_out, sampled_pcs_cntp);
}

kern_return_t
task_get_sampled_pcs(task, seqnop, sampled_pcs_out, sampled_pcs_cntp)
	task_t task;
	sampled_pc_seqno_t *seqnop;
	sampled_pc_array_t sampled_pcs_out;
	int *sampled_pcs_cntp;
{
    int nsamples;
    sampled_pc_seqno_t seqidx1, seqidx2;

    if (task == TASK_NULL)
    	return KERN_INVALID_ARGUMENT;

    if ((int)task->pc_sample.buffer == 0)
        return KERN_FAILURE;

    return get_sampled_pcs(&task->pc_sample, seqnop, sampled_pcs_out, sampled_pcs_cntp);
    
}


#endif /* MACH_PCSAMPLE > 0 */

