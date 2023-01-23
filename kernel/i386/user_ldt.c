/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	user_ldt.c,v $
 * Revision 2.3  92/02/23  19:45:12  elf
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/23            danner]
 * 
 * Revision 2.2  92/01/03  20:10:02  dbg
 * 	Created.
 * 	[91/08/20            dbg]
 * 
 */

/*
 * User LDT management.
 * Each thread in a task may have its own LDT.
 */

#include <kern/kalloc.h>
#include <kern/thread.h>

#include <vm/vm_kern.h>

#include <i386/seg.h>
#include <i386/thread.h>
#include <i386/user_ldt.h>

char	acc_type[8][3] = {
    /*	code	stack	data */
    {	0,	0,	1	},	/* data */
    {	0,	1,	1	},	/* data, writable */
    {	0,	0,	1	},	/* data, expand-down */
    {	0,	1,	1	},	/* data, writable, expand-down */
    {	1,	0,	0	},	/* code */
    {	1,	0,	1	},	/* code, readable */
    {	1,	0,	0	},	/* code, conforming */
    {	1,	0,	1	},	/* code, readable, conforming */
};

extern struct fake_descriptor ldt[];	/* for system call gate */

boolean_t selector_check(thread, sel, type)
	thread_t	thread;
	int		sel;
	int		type;	/* code, stack, data */
{
	struct user_ldt	*ldt;
	int	access;

	ldt = thread->pcb->ims.ldt;
	if (ldt == 0) {
	    switch (type) {
		case S_CODE:
		    return sel == USER_CS;
		case S_STACK:
		    return sel == USER_DS;
		case S_DATA:
		    return sel == 0 ||
			   sel == USER_CS ||
			   sel == USER_DS;
	    }
	}

	if (type != S_DATA && sel == 0)
	    return FALSE;
	if ((sel & (SEL_LDT|SEL_PL)) != (SEL_LDT|SEL_PL_U)
	  || sel > ldt->desc.limit_low)
		return FALSE;

	access = ldt->ldt[sel_idx(sel)].access;
	
	if ((access & (ACC_P|ACC_PL|ACC_TYPE_USER))
		!= (ACC_P|ACC_PL_U|ACC_TYPE_USER))
	    return FALSE;
		/* present, pl == pl.user, not system */

	return acc_type[(access & 0xe)>>1][type];
}

/*
 * Add the descriptors to the LDT, starting with
 * the descriptor for 'first_selector'.
 */
kern_return_t
i386_set_ldt(thread, first_selector, desc_list, count, desc_list_inline)
	thread_t	thread;
	int		first_selector;
	struct real_descriptor *desc_list;
	unsigned int	count;
	boolean_t	desc_list_inline;
{
	user_ldt_t	new_ldt, old_ldt, temp;
	struct real_descriptor *dp;
	int		i;
	pcb_t		pcb;
	vm_size_t	ldt_size_needed;
	int		first_desc = sel_idx(first_selector);
	vm_map_copy_t	old_copy_object;

	if (thread == THREAD_NULL)
	    return KERN_INVALID_ARGUMENT;
	if (first_desc < 0 || first_desc > 8191)
	    return KERN_INVALID_ARGUMENT;
	if (first_desc + count >= 8192)
	    return KERN_INVALID_ARGUMENT;

	/*
	 * If desc_list is not inline, it is in copyin form.
	 * We must copy it out to the kernel map, and wire
	 * it down (we touch it while the PCB is locked).
	 *
	 * We make a copy of the copyin object, and clear
	 * out the old one, so that returning KERN_INVALID_ARGUMENT
	 * will not try to deallocate the data twice.
	 */
	if (!desc_list_inline) {
	    kern_return_t	kr;
	    vm_offset_t		dst_addr;

	    old_copy_object = (vm_map_copy_t) desc_list;

	    kr = vm_map_copyout(ipc_kernel_map, &dst_addr,
				vm_map_copy_copy(old_copy_object));
	    if (kr != KERN_SUCCESS)
		return kr;

	    (void) vm_map_pageable(ipc_kernel_map,
			dst_addr,
			dst_addr + count * sizeof(struct real_descriptor),
			VM_PROT_READ|VM_PROT_WRITE);
	    desc_list = (struct real_descriptor *)dst_addr;
	}

	for (i = 0, dp = desc_list;
	     i < count;
	     i++, dp++)
	{
	    switch (dp->access & ~ACC_A) {
		case 0:
		case ACC_P:
		    /* valid empty descriptor */
		    break;
		case ACC_P | ACC_CALL_GATE:
		    /* Mach kernel call */
		    *dp = *(struct real_descriptor *)
				&ldt[sel_idx(USER_SCALL)];
		    break;
		case ACC_P | ACC_PL_U | ACC_DATA:
		case ACC_P | ACC_PL_U | ACC_DATA_W:
		case ACC_P | ACC_PL_U | ACC_DATA_E:
		case ACC_P | ACC_PL_U | ACC_DATA_EW:
		case ACC_P | ACC_PL_U | ACC_CODE:
		case ACC_P | ACC_PL_U | ACC_CODE_R:
		case ACC_P | ACC_PL_U | ACC_CODE_C:
		case ACC_P | ACC_PL_U | ACC_CODE_CR:
		case ACC_P | ACC_PL_U | ACC_CALL_GATE_16:
		case ACC_P | ACC_PL_U | ACC_CALL_GATE:
		    break;
		default:
		    return KERN_INVALID_ARGUMENT;
	    }
	}
	ldt_size_needed = sizeof(struct real_descriptor)
			* (first_desc + count - 1);

	pcb = thread->pcb;
	new_ldt = 0;
    Retry:
	simple_lock(&pcb->lock);
	old_ldt = pcb->ims.ldt;
	if (old_ldt == 0 ||
	    old_ldt->desc.limit_low + 1 < ldt_size_needed)
	{
	    /*
	     * No old LDT, or not big enough
	     */
	    if (new_ldt == 0) {
		simple_unlock(&pcb->lock);

		new_ldt = (user_ldt_t) kalloc(ldt_size_needed
					      + sizeof(struct real_descriptor));
		new_ldt->desc.limit_low   = ldt_size_needed - 1;
		new_ldt->desc.limit_high  = 0;
		new_ldt->desc.base_low    = ((vm_offset_t)new_ldt) & 0xffff;
		new_ldt->desc.base_med    = (((vm_offset_t)new_ldt) >> 16)
						 & 0xff;
		new_ldt->desc.base_high   = ((vm_offset_t)new_ldt) >> 24;
		new_ldt->desc.access      = ACC_P | ACC_LDT;
		new_ldt->desc.granularity = 0;

		goto Retry;
	    }

	    /*
	     * Have new LDT.  Copy descriptors from old to new.
	     */
	    if (old_ldt)
		bcopy((char *)&old_ldt->ldt[0],
		      (char *)&new_ldt->ldt[0],
		      old_ldt->desc.limit_low + 1);

	    temp = old_ldt;
	    old_ldt = new_ldt;	/* use new LDT from now on */
	    new_ldt = temp;	/* discard old LDT */

	    pcb->ims.ldt = old_ldt;	/* new LDT for thread */
	}

	/*
	 * Install new descriptors.
	 */
	bcopy((char *)desc_list,
	      (char *)&old_ldt->ldt[first_desc],
	      count * sizeof(struct real_descriptor));

	simple_unlock(&pcb->lock);

	if (old_ldt)
	    kfree((char *)old_ldt,
		  old_ldt->desc.limit_low + 1
		+ sizeof(struct real_descriptor));

	/*
	 * Free the descriptor list, if it was
	 * out-of-line.  Also discard the original
	 * copy object for it.
	 */
	if (!desc_list_inline) {
	    (void) kmem_free(ipc_kernel_map,
			(vm_offset_t) desc_list,
			count * sizeof(struct real_descriptor));
	    vm_map_copy_discard(old_copy_object);
	}

	return KERN_SUCCESS;
}

kern_return_t
i386_get_ldt(thread, first_selector, selector_count, desc_list, count)
	thread_t	thread;
	int		first_selector;
	int		selector_count;		/* number wanted */
	struct real_descriptor **desc_list;	/* in/out */
	unsigned int	*count;			/* in/out */
{
	struct user_ldt *user_ldt;
	pcb_t		pcb;
	int		first_desc = sel_idx(first_selector);
	unsigned int	ldt_count;
	vm_size_t	ldt_size;
	vm_size_t	size, size_needed;
	vm_offset_t	addr;

	if (thread == THREAD_NULL)
	    return KERN_INVALID_ARGUMENT;
	if (first_desc < 0 || first_desc > 8191)
	    return KERN_INVALID_ARGUMENT;
	if (first_desc + selector_count >= 8192)
	    return KERN_INVALID_ARGUMENT;

	addr = 0;
	size = 0;

	for (;;) {
	    simple_lock(&pcb->lock);
	    user_ldt = pcb->ims.ldt;
	    if (user_ldt == 0) {
		simple_unlock(&pcb->lock);
		if (addr)
		    kmem_free(ipc_kernel_map, addr, size);
		*count = 0;
		return KERN_SUCCESS;
	    }

	    /*
	     * Find how many descriptors we should return.
	     */
	    ldt_count = (user_ldt->desc.limit_low + 1) /
			sizeof (struct real_descriptor);
	    ldt_count -= first_desc;
	    if (ldt_count > selector_count)
		ldt_count = selector_count;

	    ldt_size = ldt_count * sizeof(struct real_descriptor);

	    /*
	     * Do we have the memory we need?
	     */
	    if (ldt_count <= *count)
		break;		/* fits in-line */

	    size_needed = round_page(ldt_size);
	    if (size_needed <= size)
		break;

	    /*
	     * Unlock the pcb and allocate more memory
	     */
	    simple_unlock(&pcb->lock);

	    if (size != 0)
		kmem_free(ipc_kernel_map, addr, size);

	    size = size_needed;

	    if (kmem_alloc(ipc_kernel_map, &addr, size)
			!= KERN_SUCCESS)
		return KERN_RESOURCE_SHORTAGE;
	}

	/*
	 * copy out the descriptors
	 */
	bcopy((char *)&user_ldt[first_desc],
	      (char *)*desc_list,
	      ldt_size);
	*count = ldt_count;
	simple_unlock(&pcb->lock);

	if (addr) {
	    vm_size_t		size_used, size_left;
	    vm_map_copy_t	memory;

	    /*
	     * Free any unused memory beyond the end of the last page used
	     */
	    size_used = round_page(ldt_size);
	    if (size_used != size)
		kmem_free(ipc_kernel_map,
			addr + size_used, size - size_used);

	    /*
	     * Zero the remainder of the page being returned.
	     */
	    size_left = size_used - ldt_size;
	    if (size_left > 0)
		bzero((char *)addr + ldt_size, size_left);

	    /*
	     * Make memory into copyin form - this unwires it.
	     */
	    (void) vm_map_copyin(ipc_kernel_map, addr, size_used, TRUE, &memory);
	    *desc_list = (struct real_descriptor *)memory;
	}

	return KERN_SUCCESS;
}

void
user_ldt_free(user_ldt)
	user_ldt_t	user_ldt;
{
	kfree((char *)user_ldt,
		user_ldt->desc.limit_low + 1
		+ sizeof(struct real_descriptor));
}
