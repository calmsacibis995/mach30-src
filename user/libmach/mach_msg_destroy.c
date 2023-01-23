/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log:	mach_msg_destroy.c,v $
 * Revision 2.6  93/01/14  18:03:18  danner
 * 	64bit cleanup.
 * 	[92/12/01            af]
 * 
 * Revision 2.5  92/01/23  15:22:25  rpd
 * 	Updated mach_msg_destroy to make it useful in cleaning up
 * 	messages which couldn't be sent.
 * 	[92/01/20            rpd]
 * 
 * Revision 2.4  91/05/14  17:53:15  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/14  14:17:43  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:44:15  mrt]
 * 
 * Revision 2.2  90/08/06  17:24:22  rpd
 * 	Created.
 * 
 */

#include <mach/port.h>
#include <mach/message.h>
#include <mach_init.h>

static void mach_msg_destroy_port();
static void mach_msg_destroy_memory();

/*
 *	Routine:	mach_msg_destroy
 *	Purpose:
 *		mach_msg_destroy is useful in two contexts.
 *
 *		First, it can deallocate all port rights and
 *		out-of-line memory in a received message.
 *		When a server receives a request it doesn't want,
 *		it needs this functionality.
 *
 *		Second, it can mimic the side-effects of a msg-send
 *		operation.  The effect is as if the message were sent
 *		and then destroyed inside the kernel.  When a server
 *		can't send a reply (because the client died),
 *		it needs this functionality.
 */

#define ptr_align(x)	\
	( ( ((vm_offset_t)(x)) + (sizeof(vm_offset_t)-1) ) & ~(sizeof(vm_offset_t)-1) )

void
mach_msg_destroy(msg)
    mach_msg_header_t *msg;
{
    mach_msg_bits_t mbits = msg->msgh_bits;

    /*
     *	The msgh_local_port field doesn't hold a port right.
     *	The receive operation consumes the destination port right.
     */

    mach_msg_destroy_port(msg->msgh_remote_port, MACH_MSGH_BITS_REMOTE(mbits));

    if (mbits & MACH_MSGH_BITS_COMPLEX) {
	vm_offset_t saddr;
	vm_offset_t eaddr;

	saddr = (vm_offset_t) (msg + 1);
	eaddr = (vm_offset_t) msg + msg->msgh_size;

	while (saddr < eaddr) {
	    mach_msg_type_long_t *type;
	    mach_msg_type_name_t name;
	    mach_msg_type_size_t size;
	    mach_msg_type_number_t number;
	    boolean_t is_inline;
	    vm_size_t length;
	    vm_offset_t addr;

	    type = (mach_msg_type_long_t *) saddr;
	    is_inline = type->msgtl_header.msgt_inline;
	    if (type->msgtl_header.msgt_longform) {
		    name = type->msgtl_name;
		    size = type->msgtl_size;
		    number = type->msgtl_number;
		    saddr += sizeof(mach_msg_type_long_t);
	    } else {
		    name = type->msgtl_header.msgt_name;
		    size = type->msgtl_header.msgt_size;
		    number = type->msgtl_header.msgt_number;
		    saddr += sizeof(mach_msg_type_t);
	    }

	    /* padding (ptrs and ports) ? */
	    if ((sizeof(natural_t) > sizeof(mach_msg_type_t)) &&
		((size >> 3) == sizeof(natural_t)))
		     saddr = ptr_align(saddr);

	    /* calculate length of data in bytes, rounding up to ints */
	    length = ((((number * size) + 7) >> 3) + 3) &~ 3;

	    addr = is_inline ? saddr : * (vm_offset_t *) saddr;

	    if (MACH_MSG_TYPE_PORT_ANY(name)) {
		mach_port_t *ports = (mach_port_t *) addr;
		mach_msg_type_number_t i;

		for (i = 0; i < number; i++)
		    mach_msg_destroy_port(*ports++, name);
	    }

	    if (is_inline) {
		/* inline data sizes round up to int boundaries */
		saddr += length;
	    } else {
		if (type->msgtl_header.msgt_deallocate)
		    mach_msg_destroy_memory(addr, length);
		saddr += sizeof(vm_offset_t);
	    }
	}
    }
}

static void
mach_msg_destroy_port(port, type)
    mach_port_t port;
    mach_msg_type_name_t type;
{
    if (MACH_PORT_VALID(port)) switch (type) {
      case MACH_MSG_TYPE_MOVE_SEND:
      case MACH_MSG_TYPE_MOVE_SEND_ONCE:
	/* destroy the send/send-once right */
	(void) mach_port_deallocate(mach_task_self(), port);
	break;

      case MACH_MSG_TYPE_MOVE_RECEIVE:
	/* destroy the receive right */
	(void) mach_port_mod_refs(mach_task_self(), port,
				  MACH_PORT_RIGHT_RECEIVE, -1);
	break;

      case MACH_MSG_TYPE_MAKE_SEND:
	/* create a send right and then destroy it */
	(void) mach_port_insert_right(mach_task_self(), port,
				      port, MACH_MSG_TYPE_MAKE_SEND);
	(void) mach_port_deallocate(mach_task_self(), port);
	break;

      case MACH_MSG_TYPE_MAKE_SEND_ONCE:
	/* create a send-once right and then destroy it */
	(void) mach_port_extract_right(mach_task_self(), port,
				       MACH_MSG_TYPE_MAKE_SEND_ONCE,
				       &port, &type);
	(void) mach_port_deallocate(mach_task_self(), port);
	break;
    }
}

static void
mach_msg_destroy_memory(addr, size)
    vm_offset_t addr;
    vm_size_t size;
{
    if (size != 0)
	(void) vm_deallocate(mach_task_self(), addr, size);
}
