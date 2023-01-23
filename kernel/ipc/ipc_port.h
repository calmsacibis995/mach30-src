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
 * $Log:	ipc_port.h,v $
 * Revision 2.14  92/04/06  01:16:28  rpd
 * 	Fixed ip_msgcount overflow problem by changing it
 * 	and ip_qlimit to be mach_port_msgcount_t.
 * 	Added ipc_port_lock_mqueue.
 * 	[92/04/06            rpd]
 * 
 * Revision 2.13  92/03/10  16:26:18  jsb
 * 	Merged in norma branch changes as of NORMA_MK7:
 * 	Removed token-related fields. Added fields for receive-right migration
 * 	and xmm support. Added ip_nsproxy* macros for no-senders support.
 * 	[92/03/09  13:14:35  jsb]
 * 
 * Revision 2.12  91/12/14  14:28:26  jsb
 * 	NORMA_IPC: replaced dummy port struct fields with real names.
 * 
 * Revision 2.11  91/11/14  16:56:20  rpd
 * 	Added ipc_fields.h hack, with fields in struct ipc_port to match.
 *	Added IP_NORMA_IS_PROXY macro.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.10  91/10/09  16:10:01  af
 * 	Added (unconditional) ipc_port_print declaration.
 * 	[91/09/02            rpd]
 * 
 * Revision 2.9  91/08/28  11:13:50  jsb
 * 	Added ip_seqno and ipc_port_set_seqno.
 * 	[91/08/09            rpd]
 * 	Renamed clport (now ip_norma) fields in struct ipc_port.
 * 	[91/08/14  19:31:55  jsb]
 * 
 * Revision 2.8  91/08/03  18:18:37  jsb
 * 	Fixed include. Added clport fields directly to struct ipc_port.
 * 	[91/07/17  14:06:25  jsb]
 * 
 * Revision 2.7  91/06/17  15:46:26  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:44:06  jsb]
 * 
 * Revision 2.6  91/05/14  16:35:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/05  17:23:10  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:50:04  mrt]
 * 
 * Revision 2.4  90/11/05  14:29:39  rpd
 * 	Added ipc_port_reference, ipc_port_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.3  90/09/28  16:55:18  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:03:58  jsb]
 * 
 * Revision 2.2  90/06/02  14:51:13  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:01:25  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_port.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for ports.
 */

#ifndef	_IPC_IPC_PORT_H_
#define _IPC_IPC_PORT_H_

#include <mach_ipc_compat.h>
#include <norma_ipc.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <kern/lock.h>
#include <kern/macro_help.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_thread.h>

/*
 *  A receive right (port) can be in four states:
 *	1) dead (not active, ip_timestamp has death time)
 *	2) in a space (ip_receiver_name != 0, ip_receiver points
 *	to the space but doesn't hold a ref for it)
 *	3) in transit (ip_receiver_name == 0, ip_destination points
 *	to the destination port and holds a ref for it)
 *	4) in limbo (ip_receiver_name == 0, ip_destination == IP_NULL)
 *
 *  If the port is active, and ip_receiver points to some space,
 *  then ip_receiver_name != 0, and that space holds receive rights.
 *  If the port is not active, then ip_timestamp contains a timestamp
 *  taken when the port was destroyed.
 */

typedef unsigned int ipc_port_timestamp_t;

typedef struct ipc_port {
	struct ipc_object ip_object;

	union {
		struct ipc_space *receiver;
		struct ipc_port *destination;
		ipc_port_timestamp_t timestamp;
	} data;
	mach_port_t ip_receiver_name;

	ipc_kobject_t ip_kobject;

	mach_port_mscount_t ip_mscount;
	mach_port_rights_t ip_srights;
	mach_port_rights_t ip_sorights;

	struct ipc_port *ip_nsrequest;
	struct ipc_port *ip_pdrequest;
	struct ipc_port_request *ip_dnrequests;

	struct ipc_pset *ip_pset;
	mach_port_seqno_t ip_seqno;		/* locked by message queue */
	mach_port_msgcount_t ip_msgcount;
	mach_port_msgcount_t ip_qlimit;
	struct ipc_mqueue ip_messages;
	struct ipc_thread_queue ip_blocked;

#if	NORMA_IPC
	unsigned long ip_norma_uid;
	unsigned long ip_norma_dest_node;
	long ip_norma_stransit;
	long ip_norma_sotransit;
	long ip_norma_xmm_object_refs;
	boolean_t ip_norma_is_proxy;
	boolean_t ip_norma_is_special;
	struct ipc_port *ip_norma_atrium;
	struct ipc_port *ip_norma_queue_next;
	struct ipc_port *ip_norma_xmm_object;
	struct ipc_port *ip_norma_next;
	long ip_norma_spare1;
	long ip_norma_spare2;
	long ip_norma_spare3;
	long ip_norma_spare4;
#endif	NORMA_IPC
} *ipc_port_t;

#define	ip_references		ip_object.io_references
#define	ip_bits			ip_object.io_bits
#define	ip_receiver		data.receiver
#define	ip_destination		data.destination
#define	ip_timestamp		data.timestamp

#define	IP_NULL			((ipc_port_t) IO_NULL)
#define	IP_DEAD			((ipc_port_t) IO_DEAD)

#define	IP_VALID(port)		IO_VALID(&(port)->ip_object)

#define	ip_active(port)		io_active(&(port)->ip_object)
#define	ip_lock_init(port)	io_lock_init(&(port)->ip_object)
#define	ip_lock(port)		io_lock(&(port)->ip_object)
#define	ip_lock_try(port)	io_lock_try(&(port)->ip_object)
#define	ip_unlock(port)		io_unlock(&(port)->ip_object)
#define	ip_check_unlock(port)	io_check_unlock(&(port)->ip_object)
#define	ip_reference(port)	io_reference(&(port)->ip_object)
#define	ip_release(port)	io_release(&(port)->ip_object)

#define	ip_alloc()		((ipc_port_t) io_alloc(IOT_PORT))
#define	ip_free(port)		io_free(IOT_PORT, &(port)->ip_object)

#define	ip_kotype(port)		io_kotype(&(port)->ip_object)

typedef ipc_table_index_t ipc_port_request_index_t;

typedef struct ipc_port_request {
	union {
		struct ipc_port *port;
		ipc_port_request_index_t index;
	} notify;

	union {
		mach_port_t name;
		struct ipc_table_size *size;
	} name;
} *ipc_port_request_t;

#define	ipr_next		notify.index
#define	ipr_size		name.size

#define	ipr_soright		notify.port
#define	ipr_name		name.name

#define	IPR_NULL		((ipc_port_request_t) 0)

#if	MACH_IPC_COMPAT
/*
 *	For backwards compatibility, the ip_pdrequest field can hold a
 *	send right instead of a send-once right.  This is indicated by
 *	the low bit of the pointer.  This works because the zone package
 *	guarantees that the two low bits of port pointers are zero.
 */

#define	ip_pdsendp(soright)	((unsigned int)(soright) & 1)
#define ip_pdsend(soright)	((ipc_port_t)((unsigned int)(soright) &~ 1))
#define	ip_pdsendm(sright)	((ipc_port_t)((unsigned int)(sright) | 1))

/*
 *	For backwards compatibility, the ipr_soright field can hold
 *	a space pointer.  This is indicated by the low bit of the pointer.
 *	This works because the zone package guarantees that the two low
 *	bits of port and space pointers are zero.
 */

#define	ipr_spacep(soright)	((unsigned int)(soright) & 1)
#define ipr_space(soright)	((ipc_space_t)((unsigned int)(soright) &~ 1))
#define	ipr_spacem(space)	((ipc_port_t)((unsigned int)(space) | 1))
#endif	MACH_IPC_COMPAT

/*
 *	Taking the ipc_port_multiple lock grants the privilege
 *	to lock multiple ports at once.  No ports must locked
 *	when it is taken.
 */

decl_simple_lock_data(extern, ipc_port_multiple_lock_data)

#define	ipc_port_multiple_lock_init()					\
		simple_lock_init(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_lock()					\
		simple_lock(&ipc_port_multiple_lock_data)

#define	ipc_port_multiple_unlock()					\
		simple_unlock(&ipc_port_multiple_lock_data)

/*
 *	The port timestamp facility provides timestamps
 *	for port destruction.  It is used to serialize
 *	mach_port_names with port death.
 */

decl_simple_lock_data(extern, ipc_port_timestamp_lock_data)
extern ipc_port_timestamp_t ipc_port_timestamp_data;

#define	ipc_port_timestamp_lock_init()					\
		simple_lock_init(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_lock()					\
		simple_lock(&ipc_port_timestamp_lock_data)

#define	ipc_port_timestamp_unlock()					\
		simple_unlock(&ipc_port_timestamp_lock_data)

extern ipc_port_timestamp_t
ipc_port_timestamp();

/*
 *	Compares two timestamps, and returns TRUE if one
 *	happened before two.  Note that this formulation
 *	works when the timestamp wraps around at 2^32,
 *	as long as one and two aren't too far apart.
 */

#define	IP_TIMESTAMP_ORDER(one, two)	((int) ((one) - (two)) < 0)

#define	ipc_port_translate_receive(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_RECEIVE,		\
				     (ipc_object_t *) (portp))

#define	ipc_port_translate_send(space, name, portp)			\
		ipc_object_translate((space), (name),			\
				     MACH_PORT_RIGHT_SEND,		\
				     (ipc_object_t *) (portp))

extern kern_return_t
ipc_port_dnrequest(/* ipc_port_t, mach_port_t, ipc_port_t,
		      ipc_port_request_index_t * */);

extern kern_return_t
ipc_port_dngrow(/* ipc_port_t */);

extern ipc_port_t
ipc_port_dncancel(/* ipc_port_t, mach_port_t, ipc_port_request_index_t */);

#define	ipc_port_dnrename(port, index, oname, nname)			\
MACRO_BEGIN								\
	ipc_port_request_t ipr, table;					\
									\
	assert(ip_active(port));					\
									\
	table = port->ip_dnrequests;					\
	assert(table != IPR_NULL);					\
									\
	ipr = &table[index];						\
	assert(ipr->ipr_name == oname);					\
									\
	ipr->ipr_name = nname;						\
MACRO_END

extern void
ipc_port_pdrequest(/* ipc_port_t, ipc_port_t, ipc_port_t * */);

extern void
ipc_port_nsrequest(/* ipc_port_t, mach_port_mscount_t,
		      ipc_port_t, ipc_port_t * */);

extern void
ipc_port_set_qlimit(/* ipc_port_t, mach_port_msgcount_t */);

#define	ipc_port_set_mscount(port, mscount)				\
MACRO_BEGIN								\
	assert(ip_active(port));					\
									\
	(port)->ip_mscount = (mscount);					\
MACRO_END

extern struct ipc_mqueue *
ipc_port_lock_mqueue(/* ipc_port_t */);

extern void
ipc_port_set_seqno(/* ipc_port_t, mach_port_seqno_t */);

extern void
ipc_port_clear_receiver(/* ipc_port_t */);

extern void
ipc_port_init(/* ipc_port_t, ipc_space_t, mach_port_t */);

extern kern_return_t
ipc_port_alloc(/* ipc_space_t, mach_port_t *, ipc_port_t * */);

extern kern_return_t
ipc_port_alloc_name(/* ipc_space_t, mach_port_t, ipc_port_t * */);

extern void
ipc_port_destroy(/* ipc_port_t */);

extern boolean_t
ipc_port_check_circularity(/* ipc_port_t, ipc_port_t */);

extern ipc_port_t
ipc_port_lookup_notify(/* ipc_space_t, mach_port_t */);

extern ipc_port_t
ipc_port_make_send(/* ipc_port_t */);

extern ipc_port_t
ipc_port_copy_send(/* ipc_port_t */);

extern mach_port_t
ipc_port_copyout_send(/* ipc_port_t, ipc_space_t */);

extern void
ipc_port_release_send(/* ipc_port_t */);

extern ipc_port_t
ipc_port_make_sonce(/* ipc_port_t */);

extern void
ipc_port_release_sonce(/* ipc_port_t */);

extern void
ipc_port_release_receive(/* ipc_port_t */);

extern ipc_port_t
ipc_port_alloc_special(/* ipc_space_t */);

extern void
ipc_port_dealloc_special(/* ipc_port_t */);

#define	ipc_port_alloc_kernel()		\
		ipc_port_alloc_special(ipc_space_kernel)
#define	ipc_port_dealloc_kernel(port)	\
		ipc_port_dealloc_special((port), ipc_space_kernel)

#define	ipc_port_alloc_reply()		\
		ipc_port_alloc_special(ipc_space_reply)
#define	ipc_port_dealloc_reply(port)	\
		ipc_port_dealloc_special((port), ipc_space_reply)

#define	ipc_port_reference(port)	\
		ipc_object_reference(&(port)->ip_object)

#define	ipc_port_release(port)		\
		ipc_object_release(&(port)->ip_object)

#if	MACH_IPC_COMPAT

extern kern_return_t
ipc_port_alloc_compat(/* ipc_space_t, mach_port_t *, ipc_port_t * */);

extern mach_port_t
ipc_port_copyout_send_compat(/* ipc_port_t, ipc_space_t */);

extern mach_port_t
ipc_port_copyout_receiver(/* ipc_port_t, ipc_space_t */);

#endif	MACH_IPC_COMPAT

extern void
ipc_port_print(/* ipc_port_t */);

#if	NORMA_IPC

#define	IP_NORMA_IS_PROXY(port)	((port)->ip_norma_is_proxy)

/*
 *	A proxy never has a real nsrequest, but is always has a fake
 *	nsrequest so that the norma ipc system is notified when there
 *	are no send rights for a proxy. A fake nsrequest is indicated by
 *	the low bit of the pointer.  This works because the zone package
 *	guarantees that the two low bits of port pointers are zero.
 */

#define	ip_nsproxyp(nsrequest)	((unsigned int)(nsrequest) & 1)
#define ip_nsproxy(nsrequest)	((ipc_port_t)((unsigned int)(nsrequest) &~ 1))
#define	ip_nsproxym(proxy)	((ipc_port_t)((unsigned int)(proxy) | 1))

#endif	NORMA_IPC

#endif	_IPC_IPC_PORT_H_
