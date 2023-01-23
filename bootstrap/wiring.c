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
 * $Log:	wiring.c,v $
 * Revision 2.3  92/01/14  16:43:25  rpd
 * 	Fixed wire_all_memory to deallocate ports from vm_region.
 * 	[92/01/14            rpd]
 * 
 * Revision 2.2  92/01/03  19:58:37  dbg
 * 	Don't wire memory unless for current task.
 * 	[91/10/17            dbg]
 * 
 * 	Created.
 * 	[91/04/29            dbg]
 * 
 */

/*
 * Package to wire current task's memory.
 */
#include <mach.h>
#include <mach_init.h>
#include <mach/machine/vm_param.h>

mach_port_t	this_task;		/* our task */
mach_port_t	priv_host_port = MACH_PORT_NULL;
					/* the privileged host port */

void
wire_setup(host_priv)
	mach_port_t	host_priv;
{
	priv_host_port = host_priv;
	this_task = mach_task_self();
}

void
wire_memory(start, size, prot)
	vm_address_t	start;
	vm_size_t	size;
	vm_prot_t	prot;
{
	kern_return_t	kr;

	if (priv_host_port == MACH_PORT_NULL)
	    return;

	kr = vm_wire(priv_host_port,
		     this_task,
		     start, size, prot);
	if (kr != KERN_SUCCESS)
	    panic("mem_wire: %d", kr);
}

void
wire_thread()
{
	kern_return_t	kr;

	if (priv_host_port == MACH_PORT_NULL)
	    return;

	kr = thread_wire(priv_host_port,
			 mach_thread_self(),
			 TRUE);
	if (kr != KERN_SUCCESS)
	    panic("wire_thread: %d", kr);
}

void
wire_all_memory()
{
	register kern_return_t kr;
	vm_offset_t	address;
	vm_size_t	size;
	vm_prot_t	protection;
	vm_prot_t	max_protection;
	vm_inherit_t	inheritance;
	boolean_t	is_shared;
	memory_object_name_t object;
	vm_offset_t	offset;

	if (priv_host_port == MACH_PORT_NULL)
	    return;

	/* iterate thru all regions, wiring */
	address = 0;
	while (
	    (kr = vm_region(this_task, &address,
	    		&size,
			&protection,
			&max_protection,
			&inheritance,
			&is_shared,
			&object,
			&offset))
		== KERN_SUCCESS)
	{
	    if (MACH_PORT_VALID(object))
		(void) mach_port_deallocate(this_task, object);
	    wire_memory(address, size, protection);
	    address += size;
	}
}

/*
 * Alias for vm_allocate to return wired memory.
 */
kern_return_t
vm_allocate(task, address, size, anywhere)
	task_t		task;
	vm_address_t	*address;
	vm_size_t	size;
	boolean_t	anywhere;
{
	kern_return_t	kr;

	if (anywhere)
	    *address = VM_MIN_ADDRESS;
	kr = vm_map(task,
		address, size, (vm_offset_t) 0, anywhere,
		MEMORY_OBJECT_NULL, (vm_offset_t)0, FALSE,
		VM_PROT_DEFAULT, VM_PROT_ALL, VM_INHERIT_DEFAULT);
	if (kr != KERN_SUCCESS)
	    return kr;

	if (task == this_task)
	    (void) vm_wire(priv_host_port, task, *address, size,
			VM_PROT_DEFAULT);
	return KERN_SUCCESS;
}
