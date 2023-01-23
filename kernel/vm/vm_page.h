/* 
 * Mach Operating System
 * Copyright (c) 1993-1988 Carnegie Mellon University
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
 * $Log:	vm_page.h,v $
 * Revision 2.13  93/01/14  18:02:02  danner
 * 	Added ANSI prototypes.
 * 	[92/12/30            dbg]
 * 	64bit cleanup.
 * 	[92/12/01            af]
 * 
 * Revision 2.12  92/01/14  16:48:28  rpd
 * 	Added vm_page_info declaration.
 * 	[92/01/14            rpd]
 * 
 * Revision 2.11  91/07/01  08:28:05  jsb
 * 	Change unused was_absent field to precious field.
 * 
 * 	Remove extraneous include of kern/thread.h.
 * 	[91/06/21            dlb]
 * 
 * Revision 2.10  91/05/18  14:41:37  rpd
 * 	Removed vm_page_fictitious_zone.
 * 	[91/05/16            rpd]
 * 
 * 	Added vm_page_fictitious_addr.
 * 	Removed VM_PAGE_TO_PHYS.
 * 	[91/04/10            rpd]
 * 
 * 	Changed vm_page_free_wanted to unsigned int.
 * 	[91/04/05            rpd]
 * 	Added vm_page_grab_fictitious, etc.
 * 	[91/03/29            rpd]
 * 	Removed VM_PAGE_DEBUG and associated variables,
 * 	leaving a dummy VM_PAGE_CHECK definition.
 * 	Added vm_page_attach.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.9  91/05/14  17:50:46  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/03/16  15:06:38  rpd
 * 	Added continuation argument to VM_PAGE_WAIT, vm_page_wait.
 * 	[91/02/05            rpd]
 * 
 * Revision 2.7  91/02/05  17:59:48  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:06  mrt]
 * 
 * Revision 2.6  91/01/08  16:45:48  rpd
 * 	Changed to singly-linked VP bucket chains.
 * 	[91/01/03            rpd]
 * 	Changed vm_wait/VM_WAIT to vm_page_wait/VM_PAGE_WAIT.
 * 	[90/11/13            rpd]
 * 
 * Revision 2.5  90/10/25  14:50:43  rwd
 * 	Removed the max_mapping field of pages.
 * 	[90/10/22            rpd]
 * 
 * Revision 2.4  90/10/12  13:06:41  rpd
 * 	Added software reference bit to the page structure.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.3  90/02/22  20:06:43  dbg
 * 		Rename PAGE_WAKEUP to PAGE_WAKEUP_DONE to reflect the fact that
 * 		it clears the busy flag.  Add new PAGE_WAKEUP that only does the
 * 		wakeup, and leaves the busy flag alone.
 * 		[89/12/13            dlb]
 * 
 * Revision 2.2  90/01/11  11:48:21  dbg
 * 	Add changes from mainline:
 * 
 * 		Add vm_page_module_init() declaration.
 * 		Change vm_page_zero_fill() to return void.
 * 		[89/08/07            mwyoung]
 * 		Added was_absent, overwriting fields.
 * 		[89/05/15            mwyoung]
 * 		Separate "private" from "fictitious" page attributes.
 * 		[89/04/22            mwyoung]
 * 		Move dirty bit under the object lock (from the page
 * 		queues lock).
 * 		[89/04/22            mwyoung]
 * 	Changes for MACH_KERNEL:
 * 	. Removed non-XP declarations.
 * 	[89/04/28            dbg]
 * 
 * Revision 2.1  89/08/03  16:45:49  rwd
 * Created.
 * 
 * Revision 2.10  89/04/18  21:26:59  mwyoung
 * 	Recent history:
 * 		Add vm_page_fictitious_zone; remove clean, owner fields.
 * 	History condensation: no relevant history.
 * 	[89/04/18            mwyoung]
 * 
 * Revision 2.4  88/12/19  03:00:55  mwyoung
 */
/*
 *	File:	vm/vm_page.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Resident memory system definitions.
 */

#ifndef	_VM_VM_PAGE_H_
#define _VM_VM_PAGE_H_

#include <mach_vm_debug.h>

#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/vm_param.h>
#include <vm/vm_object.h>
#include <kern/queue.h>
#include <kern/lock.h>
#include <kern/zalloc.h>

#include <kern/macro_help.h>
#include <kern/sched_prim.h>	/* definitions of wait/wakeup */

#if	MACH_VM_DEBUG
#include <mach_debug/hash_info.h>
#endif

/*
 *	Management of resident (logical) pages.
 *
 *	A small structure is kept for each resident
 *	page, indexed by page number.  Each structure
 *	is an element of several lists:
 *
 *		A hash table bucket used to quickly
 *		perform object/offset lookups
 *
 *		A list of all pages for a given object,
 *		so they can be quickly deactivated at
 *		time of deallocation.
 *
 *		An ordered list of pages due for pageout.
 *
 *	In addition, the structure contains the object
 *	and offset to which this page belongs (for pageout),
 *	and sundry status bits.
 *
 *	Fields in this structure are locked either by the lock on the
 *	object that the page belongs to (O) or by the lock on the page
 *	queues (P).  [Some fields require that both locks be held to
 *	change that field; holding either lock is sufficient to read.]
 */

struct vm_page {
	queue_chain_t	pageq;		/* queue info for FIFO
					 * queue or free list (P) */
	queue_chain_t	listq;		/* all pages in same object (O) */
	struct vm_page	*next;		/* VP bucket link (O) */

	vm_object_t	object;		/* which object am I in (O,P) */
	vm_offset_t	offset;		/* offset into that object (O,P) */

	unsigned int	wire_count:16,	/* how many wired down maps use me?
					   (O&P) */
	/* boolean_t */	inactive:1,	/* page is in inactive list (P) */
			active:1,	/* page is in active list (P) */
			laundry:1,	/* page is being cleaned now (P)*/
			free:1,		/* page is on free list (P) */
			reference:1,	/* page has been used (P) */
			:0;		/* (force to 'long' boundary) */
#ifdef	ns32000
	int		pad;		/* extra space for ns32000 bit ops */
#endif	/* ns32000 */

	unsigned int
	/* boolean_t */	busy:1,		/* page is in transit (O) */
			wanted:1,	/* someone is waiting for page (O) */
			tabled:1,	/* page is in VP table (O) */
			fictitious:1,	/* Physical page doesn't exist (O) */
			private:1,	/* Page should not be returned to
					 *  the free list (O) */
			absent:1,	/* Data has been requested, but is
					 *  not yet available (O) */
			error:1,	/* Data manager was unable to provide
					 *  data due to error (O) */
			dirty:1,	/* Page must be cleaned (O) */
			precious:1,	/* Page is precious; data must be
					 *  returned even if clean (O) */
			overwriting:1,	/* Request to unlock has been made
					 * without having data. (O)
					 * [See vm_object_overwrite] */
			:0;

	vm_offset_t	phys_addr;	/* Physical address of page, passed
					 *  to pmap_enter (read-only) */
	vm_prot_t	page_lock;	/* Uses prohibited by data manager (O) */
	vm_prot_t	unlock_request;	/* Outstanding unlock request (O) */
};

typedef struct vm_page	*vm_page_t;

#define VM_PAGE_NULL		((vm_page_t) 0)

/*
 *	For debugging, this macro can be defined to perform
 *	some useful check on a page structure.
 */

#define VM_PAGE_CHECK(mem)

/*
 *	Each pageable resident page falls into one of three lists:
 *
 *	free	
 *		Available for allocation now.
 *	inactive
 *		Not referenced in any map, but still has an
 *		object/offset-page mapping, and may be dirty.
 *		This is the list of pages that should be
 *		paged out next.
 *	active
 *		A list of pages which have been placed in
 *		at least one physical map.  This list is
 *		ordered, in LRU-like fashion.
 */

extern
vm_page_t	vm_page_queue_free;	/* memory free queue */
extern
vm_page_t	vm_page_queue_fictitious;	/* fictitious free queue */
extern
queue_head_t	vm_page_queue_active;	/* active memory queue */
extern
queue_head_t	vm_page_queue_inactive;	/* inactive memory queue */

extern
vm_offset_t	first_phys_addr;	/* physical address for first_page */
extern
vm_offset_t	last_phys_addr;		/* physical address for last_page */

extern
int	vm_page_free_count;	/* How many pages are free? */
extern
int	vm_page_fictitious_count;/* How many fictitious pages are free? */
extern
int	vm_page_active_count;	/* How many pages are active? */
extern
int	vm_page_inactive_count;	/* How many pages are inactive? */
extern
int	vm_page_wire_count;	/* How many pages are wired? */
extern
int	vm_page_free_target;	/* How many do we want free? */
extern
int	vm_page_free_min;	/* When to wakeup pageout */
extern
int	vm_page_inactive_target;/* How many do we want inactive? */
extern
int	vm_page_free_reserved;	/* How many pages reserved to do pageout */
extern
int	vm_page_laundry_count;	/* How many pages being laundered? */

decl_simple_lock_data(extern,vm_page_queue_lock)/* lock on active and inactive
						   page queues */
decl_simple_lock_data(extern,vm_page_queue_free_lock)
						/* lock on free page queue */

extern unsigned int	vm_page_free_wanted;
				/* how many threads are waiting for memory */

extern vm_offset_t	vm_page_fictitious_addr;
				/* (fake) phys_addr of fictitious pages */

extern void		vm_page_bootstrap(
	vm_offset_t	*startp,
	vm_offset_t	*endp);
extern void		vm_page_module_init(void);

extern void		vm_page_create(
	vm_offset_t	start,
	vm_offset_t	end);
extern vm_page_t	vm_page_lookup(
	vm_object_t	object,
	vm_offset_t	offset);
extern vm_page_t	vm_page_grab_fictitious(void);
extern void		vm_page_release_fictitious(vm_page_t);
extern boolean_t	vm_page_convert(vm_page_t);
extern void		vm_page_more_fictitious(void);
extern vm_page_t	vm_page_grab(void);
extern void		vm_page_release(vm_page_t);
extern void		vm_page_wait(void (*)(void));
extern vm_page_t	vm_page_alloc(
	vm_object_t	object,
	vm_offset_t	offset);
extern void		vm_page_init(
	vm_page_t	mem,
	vm_offset_t	phys_addr);
extern void		vm_page_free(vm_page_t);
extern void		vm_page_activate(vm_page_t);
extern void		vm_page_deactivate(vm_page_t);
extern void		vm_page_rename(
	vm_page_t	mem,
	vm_object_t	new_object,
	vm_offset_t	new_offset);
extern void		vm_page_insert(
	vm_page_t	mem,
	vm_object_t	object,
	vm_offset_t	offset);
extern void		vm_page_remove(
	vm_page_t	mem);

extern void		vm_page_zero_fill(vm_page_t);
extern void		vm_page_copy(vm_page_t src_m, vm_page_t dest_m);

extern void		vm_page_wire(vm_page_t);
extern void		vm_page_unwire(vm_page_t);

extern void		vm_set_page_size(void);

#if	MACH_VM_DEBUG
extern unsigned int	vm_page_info(
	hash_info_bucket_t	*info,
	unsigned int		count);
#endif

/*
 *	Functions implemented as macros
 */

#define PAGE_ASSERT_WAIT(m, interruptible)			\
		MACRO_BEGIN					\
		(m)->wanted = TRUE;				\
		assert_wait((event_t) (m), (interruptible));	\
		MACRO_END

#define PAGE_WAKEUP_DONE(m)					\
		MACRO_BEGIN					\
		(m)->busy = FALSE;				\
		if ((m)->wanted) {				\
			(m)->wanted = FALSE;			\
			thread_wakeup(((event_t) m));		\
		}						\
		MACRO_END

#define PAGE_WAKEUP(m)						\
		MACRO_BEGIN					\
		if ((m)->wanted) {				\
			(m)->wanted = FALSE;			\
			thread_wakeup((event_t) (m));		\
		}						\
		MACRO_END

#define VM_PAGE_FREE(p) 			\
		MACRO_BEGIN			\
		vm_page_lock_queues();		\
		vm_page_free(p);		\
		vm_page_unlock_queues();	\
		MACRO_END

/*
 *	Macro to be used in place of pmap_enter()
 */

#define PMAP_ENTER(pmap, virtual_address, page, protection, wired) \
		MACRO_BEGIN					\
		pmap_enter(					\
			(pmap),					\
			(virtual_address),			\
			(page)->phys_addr,			\
			(protection) & ~(page)->page_lock,	\
			(wired)					\
		 );						\
		MACRO_END

#define	VM_PAGE_WAIT(continuation)	vm_page_wait(continuation)

#define vm_page_lock_queues()	simple_lock(&vm_page_queue_lock)
#define vm_page_unlock_queues()	simple_unlock(&vm_page_queue_lock)

#define VM_PAGE_QUEUES_REMOVE(mem)				\
	MACRO_BEGIN						\
	if (mem->active) {					\
		queue_remove(&vm_page_queue_active,		\
			mem, vm_page_t, pageq);			\
		mem->active = FALSE;				\
		vm_page_active_count--;				\
	}							\
								\
	if (mem->inactive) {					\
		queue_remove(&vm_page_queue_inactive,		\
			mem, vm_page_t, pageq);			\
		mem->inactive = FALSE;				\
		vm_page_inactive_count--;			\
	}							\
	MACRO_END

#endif	/* _VM_VM_PAGE_H_ */
