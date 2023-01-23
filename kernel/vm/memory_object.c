/* 
 * Mach Operating System
 * Copyright (c) 1992,1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * $Log:	memory_object.c,v $
 * Revision 2.31  93/01/14  18:00:42  danner
 * 	Removed unneeded cast in argument to thread_wakeup/assert_wait.
 * 	[92/12/30            dbg]
 * 
 * 	Unlock object around call to vm_map_copy_invoke_cont in
 * 	memory_object_data_supply.
 * 	[92/09/22            dbg]
 * 	64bit cleanup.
 * 	[92/12/01            af]
 * 
 * 	Unlock object around call to vm_map_copy_invoke_cont in
 * 	memory_object_data_supply.
 * 	[92/09/22            dbg]
 * 
 * Revision 2.30  92/08/03  17:59:55  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.29  92/05/21  17:25:17  jfriedl
 * 	Added stuff to quiet gcc warnings.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.28  92/03/10  16:29:58  jsb
 * 	Add MEMORY_OBJECT_COPY_TEMPORARY case to
 * 	memory_object_set_attributes_common.
 * 	[92/03/06  16:57:38  jsb]
 * 
 * 	NORMA_VM: don't define cover and backwards compatibility routines.
 * 	[92/03/06  16:54:53  jsb]
 * 
 * 	[David L. Black 92/02/22  17:05:17  dlb@osf.org]
 * 	Implement no change of page lock functionality in
 * 	memory_object_lock_request.
 * 
 * Revision 2.27  92/02/23  19:50:36  elf
 * 	Bug fix to avoid duplicate vm_map_copy_t deallocate in
 * 	error case of memory_object_data_supply.
 * 	[92/02/19  17:38:17  dlb]
 * 
 * Revision 2.25.2.2  92/02/18  19:20:01  jeffreyh
 * 	Cleaned up incorrect comment
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.25.2.1  92/01/21  21:55:10  jsb
 * 	Created memory_object_set_attributes_common which provides
 * 	functionality common to memory_object_set_attributes,
 * 	memory_object_ready, and memory_object_change_attributes.
 * 	Fixed memory_object_change_attributes to set use_old_pageout
 * 	false instead of true.
 * 	[92/01/21  18:40:20  jsb]
 * 
 * Revision 2.26  92/01/23  15:21:33  rpd
 * 	Fixed memory_object_change_attributes.
 * 	Created memory_object_set_attributes_common.
 * 	[92/01/18            rpd]
 * 
 * Revision 2.25  91/10/09  16:19:18  af
 * 	Fixed assertion in memory_object_data_supply.
 * 	[91/10/06            rpd]
 * 
 * 	Added vm_page_deactivate_hint.
 * 	[91/09/29            rpd]
 * 
 * Revision 2.24  91/08/28  11:17:50  jsb
 * 	Continuation bug fix.
 * 	[91/08/05  17:42:57  dlb]
 * 
 * 	Add vm_map_copy continuation support to memory_object_data_supply.
 * 	[91/07/30  14:13:59  dlb]
 * 
 * 	Turn on page lists by default: gut and remove body of memory_object_
 * 	data_provided -- it's now a wrapper for memory_object_data_supply.
 * 	Precious page support:
 * 	    Extensive modifications to to memory_object_lock_{request,page}
 * 	    Implement old version wrapper xxx_memory_object_lock_request
 * 	    Maintain use_old_pageout field in vm objects.
 * 	    Add memory_object_ready, memory_object_change_attributes.
 * 	[91/07/03  14:11:35  dlb]
 * 
 * Revision 2.23  91/08/03  18:19:51  jsb
 * 	For now, use memory_object_data_supply iff NORMA_IPC.
 * 	[91/07/04  13:14:50  jsb]
 * 
 * Revision 2.22  91/07/31  18:20:56  dbg
 * 	Removed explicit data_dealloc argument to
 * 	memory_object_data_supply.  MiG now handles a user-specified
 * 	dealloc flag.
 * 	[91/07/29            dbg]
 * 
 * Revision 2.21  91/07/01  09:17:25  jsb
 * 	Fixed remaining MACH_PORT references.
 * 
 * Revision 2.20  91/07/01  08:31:54  jsb
 * 	Changed mach_port_t to ipc_port_t in memory_object_data_supply.
 * 
 * Revision 2.19  91/07/01  08:26:53  jsb
 * 	21-Jun-91 David L. Black (dlb) at Open Software Foundation
 * 	Add memory_object_data_supply.
 * 	[91/06/29  16:36:43  jsb]
 * 
 * Revision 2.18  91/06/25  11:06:45  rpd
 * 	Fixed includes to avoid norma files unless they are really needed.
 * 	[91/06/25            rpd]
 * 
 * Revision 2.17  91/06/25  10:33:04  rpd
 * 	Changed memory_object_t to ipc_port_t where appropriate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.16  91/06/17  15:48:55  jsb
 * 	NORMA_VM: include xmm_server_rename.h, for interposition.
 * 	[91/06/17  11:09:52  jsb]
 * 
 * Revision 2.15  91/05/18  14:39:33  rpd
 * 	Fixed memory_object_lock_page to handle fictitious pages.
 * 	[91/04/06            rpd]
 * 	Changed memory_object_data_provided, etc,
 * 	to allow for fictitious pages.
 * 	[91/03/29            rpd]
 * 	Added vm/memory_object.h.
 * 	[91/03/22            rpd]
 * 
 * Revision 2.14  91/05/14  17:47:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.13  91/03/16  15:04:30  rpd
 * 	Removed the old version of memory_object_data_provided.
 * 	[91/03/11            rpd]
 * 	Fixed memory_object_data_provided to return success
 * 	iff it consumes the copy object.
 * 	[91/02/10            rpd]
 * 
 * Revision 2.12  91/02/05  17:57:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:30:46  mrt]
 * 
 * Revision 2.11  91/01/08  16:44:11  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.10  90/10/25  14:49:30  rwd
 * 	Clean and not flush pages to lock request get moved to the
 * 	inactive queue.
 * 	[90/10/24            rwd]
 * 
 * Revision 2.9  90/08/27  22:15:49  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/08/06  15:08:16  rwd
 * 	Fix locking problems in memory_object_lock_request.
 * 	[90/07/12            rwd]
 * 	Fix memory_object_lock_request to only send contiguous pages as
 * 	one message.  If dirty pages were seperated by absent pages,
 * 	then the wrong thing was done.
 * 	[90/07/11            rwd]
 * 
 * Revision 2.7  90/06/19  23:01:38  rpd
 * 	Bring old single_page version of memory_object_data_provided up
 * 	to date.
 * 	[90/06/05            dbg]
 * 
 * 	Correct object locking in memory_object_lock_request.
 * 	[90/06/05            dbg]
 * 
 * Revision 2.6  90/06/02  15:10:14  rpd
 * 	Changed memory_object_lock_request/memory_object_lock_completed calls
 * 	to allow both send and send-once right reply-to ports.
 * 	[90/05/31            rpd]
 * 
 * 	Added memory_manager_default_port.
 * 	[90/04/29            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  23:11:14  rpd]
 * 
 * Revision 2.5  90/05/29  18:38:29  rwd
 * 	New memory_object_lock_request from dbg.
 * 	[90/05/18  13:04:36  rwd]
 * 
 * 	Picked up rfr MACH_PAGEMAP changes.
 * 	[90/04/12  13:45:43  rwd]
 * 
 * Revision 2.4  90/05/03  15:58:23  dbg
 * 	Pass should_flush to vm_pageout_page: don't flush page if not
 * 	requested.
 * 	[90/03/28            dbg]
 * 
 * Revision 2.3  90/02/22  20:05:10  dbg
 * 	Pick up changes from mainline:
 * 
 * 		Fix placeholder page handling in memory_object_data_provided.
 * 		Old code was calling zalloc while holding a lock.
 * 		[89/12/13  19:58:28  dlb]
 * 
 * 		Don't clear busy flags on any pages in memory_object_lock_page
 * 		(from memory_object_lock_request)!!  Implemented by changing
 * 		PAGE_WAKEUP to not clear busy flag and using PAGE_WAKEUP_DONE
 * 		when it must be cleared.  See vm/vm_page.h.  With dbg's help.
 * 		[89/12/13            dlb]
 * 
 * 		Don't activate fictitious pages after freeing them in
 * 		memory_object_data_{unavailable,error}.  Add missing lock and
 * 		unlock of page queues when activating pages in same routines.
 * 		[89/12/11            dlb]
 * 		Retry lookup after using CLEAN_DIRTY_PAGES in
 * 		memory_object_lock_request().  Also delete old version of
 * 		memory_object_data_provided().  From mwyoung.
 * 		[89/11/17            dlb]
 * 
 * 		Save all page-cleaning operations until it becomes necessary
 * 		to block in memory_object_lock_request().
 * 		[89/09/30  18:07:16  mwyoung]
 * 
 * 		Split out a per-page routine for lock requests.
 * 		[89/08/20  19:47:42  mwyoung]
 * 
 * 		Verify that the user memory used in
 * 		memory_object_data_provided() is valid, even if it won't
 * 		be used to fill a page request.
 * 		[89/08/01  14:58:21  mwyoung]
 * 
 * 		Make memory_object_data_provided atomic, interruptible,
 * 		and serializable when handling multiple pages.  Add
 * 		optimization for single-page operations.
 * 		[89/05/12  16:06:13  mwyoung]
 * 
 * 		Simplify lock/clean/flush sequences memory_object_lock_request.
 * 		Correct error in call to pmap_page_protect() there.
 * 		Make error/absent pages eligible for pageout.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:10  dbg
 * 	Pass keep_wired argument to vm_move.  Disabled
 * 	host_set_memory_object_default.
 * 	[89/07/14            dbg]
 * 
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Clean up fast_pager_data option.  Remove pager_data_provided_inline.
 *
 * Revision 2.18  89/04/23  13:25:30  gm0w
 * 	Fixed typo to pmap_page_protect in memory_object_lock_request().
 * 	[89/04/23            gm0w]
 * 
 * Revision 2.17  89/04/22  15:35:09  gm0w
 * 	Commented out check/uprintf if memory_object_data_unavailable
 * 	was called on a permanent object.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.16  89/04/18  21:24:24  mwyoung
 * 	Recent history:
 * 	 	Add vm_set_default_memory_manager(),
 * 		 memory_object_get_attributes().
 * 		Whenever asked to clean a page, use pmap_is_modified, even
 * 		 if not flushing the data.
 * 		Handle fictitious pages when accepting data (or error or
 * 		 unavailable).
 * 		Avoid waiting in memory_object_data_error().
 * 
 * 	Previous history has been integrated into the documentation below.
 * 	[89/04/18            mwyoung]
 * 
 */
/*
 *	File:	vm/memory_object.c
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface control functions.
 */

/*
 *	Interface dependencies:
 */

#include <mach/std_types.h>	/* For pointer_t */
#include <mach/mach_types.h>

#include <mach/kern_return.h>
#include <vm/vm_object.h>
#include <mach/memory_object.h>
#include <mach/memory_object_user.h>
#include <mach/memory_object_default.h>
#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/message.h>

/*
 *	Implementation dependencies:
 */
#include <vm/memory_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <vm/pmap.h>		/* For copy_to_phys, pmap_clear_modify */
#include <kern/thread.h>		/* For current_thread() */
#include <kern/host.h>
#include <vm/vm_kern.h>		/* For kernel_map, vm_move */
#include <vm/vm_map.h>		/* For vm_map_pageable */
#include <ipc/ipc_port.h>

#include <norma_vm.h>
#include <norma_ipc.h>
#if	NORMA_VM
#include <norma/xmm_server_rename.h>
#endif	NORMA_VM
#include <mach_pagemap.h>
#if	MACH_PAGEMAP
#include <vm/vm_external.h>	
#endif	MACH_PAGEMAP

typedef	int		memory_object_lock_result_t; /* moved from below */


ipc_port_t	memory_manager_default = IP_NULL;
decl_simple_lock_data(,memory_manager_default_lock)

/*
 *	Important note:
 *		All of these routines gain a reference to the
 *		object (first argument) as part of the automatic
 *		argument conversion. Explicit deallocation is necessary.
 */

#if	!NORMA_VM
/*
 *	If successful, destroys the map copy object.
 */
kern_return_t memory_object_data_provided(object, offset, data, data_cnt,
					  lock_value)
	vm_object_t	object;
	vm_offset_t	offset;
	pointer_t	data;
	unsigned int	data_cnt;
	vm_prot_t	lock_value;
{
        return memory_object_data_supply(object, offset, (vm_map_copy_t) data,
					 data_cnt, lock_value, FALSE, IP_NULL,
					 0);
}
#endif	!NORMA_VM


kern_return_t memory_object_data_supply(object, offset, data_copy, data_cnt,
	lock_value, precious, reply_to, reply_to_type)
	register
        vm_object_t		object;
	register
	vm_offset_t		offset;
	vm_map_copy_t		data_copy;
	unsigned int		data_cnt;
	vm_prot_t		lock_value;
	boolean_t		precious;
	ipc_port_t		reply_to;
	mach_msg_type_name_t	reply_to_type;
{
	kern_return_t	result = KERN_SUCCESS;
	vm_offset_t	error_offset = 0;
	register
	vm_page_t	m;
	register
	vm_page_t	data_m;
	vm_size_t	original_length;
	vm_offset_t	original_offset;
	vm_page_t	*page_list;
	boolean_t	was_absent;
	vm_map_copy_t	orig_copy = data_copy;

	/*
	 *	Look for bogus arguments
	 */

	if (object == VM_OBJECT_NULL) {
		return(KERN_INVALID_ARGUMENT);
	}

	if (lock_value & ~VM_PROT_ALL) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	if ((data_cnt % PAGE_SIZE) != 0) {
	    vm_object_deallocate(object);
	    return(KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Adjust the offset from the memory object to the offset
	 *	within the vm_object.
	 */

	original_length = data_cnt;
	original_offset = offset;

	assert(data_copy->type == VM_MAP_COPY_PAGE_LIST);
	page_list = &data_copy->cpy_page_list[0];

	vm_object_lock(object);
	vm_object_paging_begin(object);
	offset -= object->paging_offset;

	/*
	 *	Loop over copy stealing pages for pagein.
	 */

	for (; data_cnt > 0 ; data_cnt -= PAGE_SIZE, offset += PAGE_SIZE) {

		assert(data_copy->cpy_npages > 0);
		data_m = *page_list;

		if (data_m == VM_PAGE_NULL || data_m->tabled ||
		    data_m->error || data_m->absent || data_m->fictitious) {

			panic("Data_supply: bad page");
		      }

		/*
		 *	Look up target page and check its state.
		 */

retry_lookup:
		m = vm_page_lookup(object,offset);
		if (m == VM_PAGE_NULL) {
		    was_absent = FALSE;
		}
		else {
		    if (m->absent && m->busy) {

			/*
			 *	Page was requested.  Free the busy
			 *	page waiting for it.  Insertion
			 *	of new page happens below.
			 */

			VM_PAGE_FREE(m);
			was_absent = TRUE;
		    }
		    else {

			/*
			 *	Have to wait for page that is busy and
			 *	not absent.  This is probably going to
			 *	be an error, but go back and check.
			 */
			if (m->busy) {
				PAGE_ASSERT_WAIT(m, FALSE);
				vm_object_unlock(object);
				thread_block((void (*)()) 0);
				vm_object_lock(object);
				goto retry_lookup;
			}

			/*
			 *	Page already present; error.
			 *	This is an error if data is precious.
			 */
			result = KERN_MEMORY_PRESENT;
			error_offset = offset + object->paging_offset;

			break;
		    }
		}

		/*
		 *	Ok to pagein page.  Target object now has no page
		 *	at offset.  Set the page parameters, then drop
		 *	in new page and set up pageout state.  Object is
		 *	still locked here.
		 *
		 *	Must clear busy bit in page before inserting it.
		 *	Ok to skip wakeup logic because nobody else
		 *	can possibly know about this page.
		 */

		data_m->busy = FALSE;
		data_m->dirty = FALSE;
		pmap_clear_modify(data_m->phys_addr);

		data_m->page_lock = lock_value;
		data_m->unlock_request = VM_PROT_NONE;
		data_m->precious = precious;

		vm_page_lock_queues();
		vm_page_insert(data_m, object, offset);

		if (was_absent)
			vm_page_activate(data_m);
		else
			vm_page_deactivate(data_m);

		vm_page_unlock_queues();

		/*
		 *	Null out this page list entry, and advance to next
		 *	page.
		 */

		*page_list++ = VM_PAGE_NULL;

		if (--(data_copy->cpy_npages) == 0 &&
		    vm_map_copy_has_cont(data_copy)) {
			vm_map_copy_t	new_copy;

			vm_object_unlock(object);

			vm_map_copy_invoke_cont(data_copy, &new_copy, &result);

			if (result == KERN_SUCCESS) {

			    /*
			     *	Consume on success requires that
			     *	we keep the original vm_map_copy
			     *	around in case something fails.
			     *	Free the old copy if it's not the original
			     */
			    if (data_copy != orig_copy) {
				vm_map_copy_discard(data_copy);
			    }

			    if ((data_copy = new_copy) != VM_MAP_COPY_NULL)
				page_list = &data_copy->cpy_page_list[0];

			    vm_object_lock(object);
			}
			else {
			    vm_object_lock(object);
			    error_offset = offset + object->paging_offset +
						PAGE_SIZE;
			    break;
			}
		}
	}

	/*
	 *	Send reply if one was requested.
	 */
	vm_object_paging_end(object);
	vm_object_unlock(object);

	if (vm_map_copy_has_cont(data_copy))
		vm_map_copy_abort_cont(data_copy);

	if (IP_VALID(reply_to)) {
		memory_object_supply_completed(
				reply_to, reply_to_type,
				object->pager_request,
				original_offset,
				original_length,
				result,
				error_offset);
	}

	vm_object_deallocate(object);

	/*
	 *	Consume on success:  The final data copy must be
	 *	be discarded if it is not the original.  The original
	 *	gets discarded only if this routine succeeds.
	 */
	if (data_copy != orig_copy)
		vm_map_copy_discard(data_copy);
	if (result == KERN_SUCCESS)
		vm_map_copy_discard(orig_copy);


	return(result);
}

kern_return_t memory_object_data_error(object, offset, size, error_value)
	vm_object_t	object;
	vm_offset_t	offset;
	vm_size_t	size;
	kern_return_t	error_value;
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size))
		return(KERN_INVALID_ARGUMENT);

#ifdef	lint
	/* Error value is ignored at this time */
	error_value++;
#endif

	vm_object_lock(object);
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		m = vm_page_lookup(object, offset);
		if ((m != VM_PAGE_NULL) && m->busy && m->absent) {
			m->error = TRUE;
			m->absent = FALSE;
			vm_object_absent_release(object);

			PAGE_WAKEUP_DONE(m);

			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();
		}

		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	 }
	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

kern_return_t memory_object_data_unavailable(object, offset, size)
	vm_object_t	object;
	vm_offset_t	offset;
	vm_size_t	size;
{
#if	MACH_PAGEMAP
	vm_external_t	existence_info = VM_EXTERNAL_NULL;
#endif	MACH_PAGEMAP

	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size))
		return(KERN_INVALID_ARGUMENT);

#if	MACH_PAGEMAP
	if ((offset == 0) && (size > VM_EXTERNAL_LARGE_SIZE) && 
	    (object->existence_info == VM_EXTERNAL_NULL)) {
		existence_info = vm_external_create(VM_EXTERNAL_SMALL_SIZE);
	}
#endif	MACH_PAGEMAP

	vm_object_lock(object);
#if	MACH_PAGEMAP
 	if (existence_info != VM_EXTERNAL_NULL) {
		object->existence_info = existence_info;
	}
	if ((offset == 0) && (size > VM_EXTERNAL_LARGE_SIZE)) {
		vm_object_unlock(object);
		vm_object_deallocate(object);
		return(KERN_SUCCESS);
	}
#endif	MACH_PAGEMAP
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		/*
		 *	We're looking for pages that are both busy and
		 *	absent (waiting to be filled), converting them
		 *	to just absent.
		 *
		 *	Pages that are just busy can be ignored entirely.
		 */

		m = vm_page_lookup(object, offset);
		if ((m != VM_PAGE_NULL) && m->busy && m->absent) {
			PAGE_WAKEUP_DONE(m);

			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();
		}
		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	}

	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_object_lock_page
 *
 *	Description:
 *		Perform the appropriate lock operations on the
 *		given page.  See the description of
 *		"memory_object_lock_request" for the meanings
 *		of the arguments.
 *
 *		Returns an indication that the operation
 *		completed, blocked, or that the page must
 *		be cleaned.
 */

#define	MEMORY_OBJECT_LOCK_RESULT_DONE		0
#define	MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK	1
#define	MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN	2
#define	MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN	3

memory_object_lock_result_t memory_object_lock_page(m, should_return,
				should_flush, prot)
	vm_page_t		m;
	memory_object_return_t	should_return;
	boolean_t		should_flush;
	vm_prot_t		prot;
{
	/*
	 *	Don't worry about pages for which the kernel
	 *	does not have any data.
	 */

	if (m->absent)
		return(MEMORY_OBJECT_LOCK_RESULT_DONE);

	/*
	 *	If we cannot change access to the page,
	 *	either because a mapping is in progress
	 *	(busy page) or because a mapping has been
	 *	wired, then give up.
	 */

	if (m->busy)
		return(MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK);

	assert(!m->fictitious);

	if (m->wire_count != 0) {
		/*
		 *	If no change would take place
		 *	anyway, return successfully.
		 *
		 *	No change means:
		 *		Not flushing AND
		 *		No change to page lock [2 checks]  AND
		 *		Don't need to send page to manager
		 *
		 *	Don't need to send page to manager means:
		 *		No clean or return request OR (
		 *		    Page is not dirty [2 checks] AND (
		 *		        Page is not precious OR
		 *			No request to return precious pages ))
		 *		      
		 *	Now isn't that straightforward and obvious ?? ;-)
		 *
		 * XXX	This doesn't handle sending a copy of a wired
		 * XXX	page to the pager, but that will require some
		 * XXX	significant surgery.
		 */

		if (!should_flush &&
		    ((m->page_lock == prot) || (prot == VM_PROT_NO_CHANGE)) &&
		    ((should_return == MEMORY_OBJECT_RETURN_NONE) ||
		     (!m->dirty && !pmap_is_modified(m->phys_addr) &&
		      (!m->precious ||
		       should_return != MEMORY_OBJECT_RETURN_ALL)))) {
			/*
			 *	Restart page unlock requests,
			 *	even though no change took place.
			 *	[Memory managers may be expecting
			 *	to see new requests.]
			 */
			m->unlock_request = VM_PROT_NONE;
			PAGE_WAKEUP(m);

			return(MEMORY_OBJECT_LOCK_RESULT_DONE);
		}

		return(MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK);
	}

	/*
	 *	If the page is to be flushed, allow
	 *	that to be done as part of the protection.
	 */

	if (should_flush)
		prot = VM_PROT_ALL;

	/*
	 *	Set the page lock.
	 *
	 *	If we are decreasing permission, do it now;
	 *	let the fault handler take care of increases
	 *	(pmap_page_protect may not increase protection).
	 */

	if (prot != VM_PROT_NO_CHANGE) {
		if ((m->page_lock ^ prot) & prot) {
			pmap_page_protect(m->phys_addr, VM_PROT_ALL & ~prot);
		}
		m->page_lock = prot;

		/*
		 *	Restart any past unlock requests, even if no
		 *	change resulted.  If the manager explicitly
		 *	requested no protection change, then it is assumed
		 *	to be remembering past requests.
		 */

		m->unlock_request = VM_PROT_NONE;
		PAGE_WAKEUP(m);
	}

	/*
	 *	Handle cleaning.
	 */

	if (should_return != MEMORY_OBJECT_RETURN_NONE) {
		/*
		 *	Check whether the page is dirty.  If
		 *	write permission has not been removed,
		 *	this may have unpredictable results.
		 */

		if (!m->dirty)
			m->dirty = pmap_is_modified(m->phys_addr);

		if (m->dirty || (m->precious &&
				 should_return == MEMORY_OBJECT_RETURN_ALL)) {
			/*
			 *	If we weren't planning
			 *	to flush the page anyway,
			 *	we may need to remove the
			 *	page from the pageout
			 *	system and from physical
			 *	maps now.
			 */

			vm_page_lock_queues();
			VM_PAGE_QUEUES_REMOVE(m);
			vm_page_unlock_queues();

			if (!should_flush)
				pmap_page_protect(m->phys_addr,
						VM_PROT_NONE);

			/*
			 *	Cleaning a page will cause
			 *	it to be flushed.
			 */

			if (m->dirty)
				return(MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN);
			else
				return(MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN);
		}
	}

	/*
	 *	Handle flushing
	 */

	if (should_flush) {
		VM_PAGE_FREE(m);
	} else {
		extern boolean_t vm_page_deactivate_hint;

		/*
		 *	XXX Make clean but not flush a paging hint,
		 *	and deactivate the pages.  This is a hack
		 *	because it overloads flush/clean with
		 *	implementation-dependent meaning.  This only
		 *	happens to pages that are already clean.
		 */

		if (vm_page_deactivate_hint &&
		    (should_return != MEMORY_OBJECT_RETURN_NONE)) {
			vm_page_lock_queues();
			vm_page_deactivate(m);
			vm_page_unlock_queues();
		}
	}

	return(MEMORY_OBJECT_LOCK_RESULT_DONE);
}

/*
 *	Routine:	memory_object_lock_request [user interface]
 *
 *	Description:
 *		Control use of the data associated with the given
 *		memory object.  For each page in the given range,
 *		perform the following operations, in order:
 *			1)  restrict access to the page (disallow
 *			    forms specified by "prot");
 *			2)  return data to the manager (if "should_return"
 *			    is RETURN_DIRTY and the page is dirty, or
 * 			    "should_return" is RETURN_ALL and the page
 *			    is either dirty or precious); and,
 *			3)  flush the cached copy (if "should_flush"
 *			    is asserted).
 *		The set of pages is defined by a starting offset
 *		("offset") and size ("size").  Only pages with the
 *		same page alignment as the starting offset are
 *		considered.
 *
 *		A single acknowledgement is sent (to the "reply_to"
 *		port) when these actions are complete.  If successful,
 *		the naked send right for reply_to is consumed.
 */

kern_return_t
memory_object_lock_request(object, offset, size,
			should_return, should_flush, prot,
			reply_to, reply_to_type)
	register vm_object_t	object;
	register vm_offset_t	offset;
	register vm_size_t	size;
	memory_object_return_t	should_return;
	boolean_t		should_flush;
	vm_prot_t		prot;
	ipc_port_t		reply_to;
	mach_msg_type_name_t	reply_to_type;
{
	register vm_page_t	m;
	vm_offset_t		original_offset = offset;
	vm_size_t		original_size = size;
	vm_offset_t		paging_offset = 0;
	vm_object_t		new_object = VM_OBJECT_NULL;
	vm_offset_t		new_offset = 0;
	vm_offset_t		last_offset = offset;
	int			page_lock_result;
	int			pageout_action = 0; /* '=0' to quiet lint */

#define	DATA_WRITE_MAX	32
	vm_page_t		holding_pages[DATA_WRITE_MAX];

	/*
	 *	Check for bogus arguments.
	 */
	if (object == VM_OBJECT_NULL ||
		((prot & ~VM_PROT_ALL) != 0 && prot != VM_PROT_NO_CHANGE))
	    return (KERN_INVALID_ARGUMENT);

	size = round_page(size);

	/*
	 *	Lock the object, and acquire a paging reference to
	 *	prevent the memory_object and control ports from
	 *	being destroyed.
	 */

	vm_object_lock(object);
	vm_object_paging_begin(object);
	offset -= object->paging_offset;

	/*
	 *	To avoid blocking while scanning for pages, save
	 *	dirty pages to be cleaned all at once.
	 *
	 *	XXXO A similar strategy could be used to limit the
	 *	number of times that a scan must be restarted for
	 *	other reasons.  Those pages that would require blocking
	 *	could be temporarily collected in another list, or
	 *	their offsets could be recorded in a small array.
	 */

	/*
	 * XXX	NOTE: May want to consider converting this to a page list
	 * XXX	vm_map_copy interface.  Need to understand object
	 * XXX	coalescing implications before doing so.
	 */

#define	PAGEOUT_PAGES							\
MACRO_BEGIN								\
	vm_map_copy_t		copy;					\
	register int		i;					\
	register vm_page_t	hp;					\
									\
	vm_object_unlock(object);					\
									\
	(void) vm_map_copyin_object(new_object, 0, new_offset, &copy);	\
									\
	if (object->use_old_pageout) {					\
	    assert(pageout_action == MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN);  \
		(void) memory_object_data_write(			\
			object->pager,					\
			object->pager_request,				\
			paging_offset,					\
			(pointer_t) copy,				\
			new_offset);					\
	}								\
	else {								\
		(void) memory_object_data_return(			\
			object->pager,					\
			object->pager_request,				\
			paging_offset,					\
			(pointer_t) copy,				\
			new_offset,					\
	     (pageout_action == MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN),	\
			!should_flush);					\
	}								\
									\
	vm_object_lock(object);						\
									\
	for (i = 0; i < atop(new_offset); i++) {			\
	    hp = holding_pages[i];					\
	    if (hp != VM_PAGE_NULL)					\
		VM_PAGE_FREE(hp);					\
	}								\
									\
	new_object = VM_OBJECT_NULL;					\
MACRO_END

	for (;
	     size != 0;
	     size -= PAGE_SIZE, offset += PAGE_SIZE)
	{
	    /*
	     *	Limit the number of pages to be cleaned at once.
	     */
	    if (new_object != VM_OBJECT_NULL &&
		    new_offset >= PAGE_SIZE * DATA_WRITE_MAX)
	    {
		PAGEOUT_PAGES;
	    }

	    while ((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL) {
		switch ((page_lock_result = memory_object_lock_page(m,
					should_return,
					should_flush,
					prot)))
		{
		    case MEMORY_OBJECT_LOCK_RESULT_DONE:
			/*
			 *	End of a cluster of dirty pages.
			 */
			if (new_object != VM_OBJECT_NULL) {
			    PAGEOUT_PAGES;
			    continue;
			}
			break;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK:
			/*
			 *	Since it is necessary to block,
			 *	clean any dirty pages now.
			 */
			if (new_object != VM_OBJECT_NULL) {
			    PAGEOUT_PAGES;
			    continue;
			}

			PAGE_ASSERT_WAIT(m, FALSE);
			vm_object_unlock(object);
			thread_block((void (*)()) 0);
			vm_object_lock(object);
			continue;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN:
		    case MEMORY_OBJECT_LOCK_RESULT_MUST_RETURN:
			/*
			 * The clean and return cases are similar.
			 *
			 * Mark the page busy since we unlock the
			 * object below.
			 */
			m->busy = TRUE;

			/*
			 * if this would form a discontiguous block,
			 * clean the old pages and start anew.
			 *
			 * NOTE: The first time through here, new_object
			 * is null, hiding the fact that pageout_action
			 * is not initialized.
			 */
			if (new_object != VM_OBJECT_NULL &&
			    (last_offset != offset ||
			     pageout_action != page_lock_result)) {
			        PAGEOUT_PAGES;
			}

			vm_object_unlock(object);

			/*
			 *	If we have not already allocated an object
			 *	for a range of pages to be written, do so
			 *	now.
			 */
			if (new_object == VM_OBJECT_NULL) {
			    new_object = vm_object_allocate(original_size);
			    new_offset = 0;
			    paging_offset = m->offset +
					object->paging_offset;
			    pageout_action = page_lock_result;
			}

			/*
			 *	Move or copy the dirty page into the
			 *	new object.
			 */
			m = vm_pageout_setup(m,
					m->offset + object->paging_offset,
					new_object,
					new_offset,
					should_flush);

			/*
			 *	Save the holding page if there is one.
			 */
			holding_pages[atop(new_offset)] = m;
			new_offset += PAGE_SIZE;
			last_offset = offset + PAGE_SIZE;

			vm_object_lock(object);
			break;
		}
		break;
	    }
	}

	/*
	 *	We have completed the scan for applicable pages.
	 *	Clean any pages that have been saved.
	 */
	if (new_object != VM_OBJECT_NULL) {
	    PAGEOUT_PAGES;
	}

	if (IP_VALID(reply_to)) {
		vm_object_unlock(object);

		/* consumes our naked send-once/send right for reply_to */
		(void) memory_object_lock_completed(reply_to, reply_to_type,
			object->pager_request, original_offset, original_size);

		vm_object_lock(object);
	}

	vm_object_paging_end(object);
	vm_object_unlock(object);
	vm_object_deallocate(object);

	return (KERN_SUCCESS);
}

#if	!NORMA_VM
/*
 *	Old version of memory_object_lock_request.  
 */
kern_return_t
xxx_memory_object_lock_request(object, offset, size,
			should_clean, should_flush, prot,
			reply_to, reply_to_type)
	register vm_object_t	object;
	register vm_offset_t	offset;
	register vm_size_t	size;
	boolean_t		should_clean;
	boolean_t		should_flush;
	vm_prot_t		prot;
	ipc_port_t		reply_to;
	mach_msg_type_name_t	reply_to_type;
{
	register int		should_return;

	if (should_clean)
		should_return = MEMORY_OBJECT_RETURN_DIRTY;
	else
		should_return = MEMORY_OBJECT_RETURN_NONE;

	return(memory_object_lock_request(object,offset,size,
		      should_return, should_flush, prot,
		      reply_to, reply_to_type));
}
#endif	!NORMA_VM
	  
kern_return_t
memory_object_set_attributes_common(object, object_ready, may_cache,
				    copy_strategy, use_old_pageout)
	vm_object_t	object;
	boolean_t	object_ready;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
	boolean_t use_old_pageout;
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	/*
	 *	Verify the attributes of importance
	 */

	switch(copy_strategy) {
		case MEMORY_OBJECT_COPY_NONE:
		case MEMORY_OBJECT_COPY_CALL:
		case MEMORY_OBJECT_COPY_DELAY:
		case MEMORY_OBJECT_COPY_TEMPORARY:
			break;
		default:
			vm_object_deallocate(object);
			return(KERN_INVALID_ARGUMENT);
	}

	if (object_ready)
		object_ready = TRUE;
	if (may_cache)
		may_cache = TRUE;

	vm_object_lock(object);

	/*
	 *	Wake up anyone waiting for the ready attribute
	 *	to become asserted.
	 */

	if (object_ready && !object->pager_ready) {
		object->use_old_pageout = use_old_pageout;
		vm_object_wakeup(object, VM_OBJECT_EVENT_PAGER_READY);
	}

	/*
	 *	Copy the attributes
	 */

	object->can_persist = may_cache;
	object->pager_ready = object_ready;
	if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		object->temporary = TRUE;
	} else {
		object->copy_strategy = copy_strategy;
	}

	vm_object_unlock(object);

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

#if	!NORMA_VM

/*
 * XXX	rpd claims that reply_to could be obviated in favor of a client
 * XXX	stub that made change_attributes an RPC.  Need investigation.
 */

kern_return_t	memory_object_change_attributes(object, may_cache,
			copy_strategy, reply_to, reply_to_type)
	vm_object_t	object;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
	ipc_port_t		reply_to;
	mach_msg_type_name_t	reply_to_type;
{
	kern_return_t	result;

	/*
	 *	Do the work and throw away our object reference.  It
	 *	is important that the object reference be deallocated
	 *	BEFORE sending the reply.  The whole point of the reply
	 *	is that it shows up after the terminate message that
	 *	may be generated by setting the object uncacheable.
	 *
	 * XXX	may_cache may become a tri-valued variable to handle
	 * XXX	uncache if not in use.
	 */
	result = memory_object_set_attributes_common(object, TRUE,
						     may_cache, copy_strategy,
						     FALSE);

	if (IP_VALID(reply_to)) {

		/* consumes our naked send-once/send right for reply_to */
		(void) memory_object_change_completed(reply_to, reply_to_type,
			may_cache, copy_strategy);

	}

	return(result);
}

kern_return_t
memory_object_set_attributes(object, object_ready, may_cache, copy_strategy)
	vm_object_t	object;
	boolean_t	object_ready;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
{
	return memory_object_set_attributes_common(object, object_ready,
						   may_cache, copy_strategy,
						   TRUE);
}

kern_return_t	memory_object_ready(object, may_cache, copy_strategy)
	vm_object_t	object;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
{
	return memory_object_set_attributes_common(object, TRUE,
						   may_cache, copy_strategy,
						   FALSE);
}
#endif	!NORMA_VM

kern_return_t	memory_object_get_attributes(object, object_ready,
						may_cache, copy_strategy)
	vm_object_t	object;
	boolean_t	*object_ready;
	boolean_t	*may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	vm_object_lock(object);
	*may_cache = object->can_persist;
	*object_ready = object->pager_ready;
	*copy_strategy = object->copy_strategy;
	vm_object_unlock(object);

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

/*
 *	If successful, consumes the supplied naked send right.
 */
kern_return_t	vm_set_default_memory_manager(host, default_manager)
	host_t		host;
	ipc_port_t	*default_manager;
{
	ipc_port_t current_manager;
	ipc_port_t new_manager;
	ipc_port_t returned_manager;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	new_manager = *default_manager;
	simple_lock(&memory_manager_default_lock);
	current_manager = memory_manager_default;

	if (new_manager == IP_NULL) {
		/*
		 *	Retrieve the current value.
		 */

		returned_manager = ipc_port_copy_send(current_manager);
	} else {
		/*
		 *	Retrieve the current value,
		 *	and replace it with the supplied value.
		 *	We consume the supplied naked send right.
		 */

		returned_manager = current_manager;
		memory_manager_default = new_manager;

		/*
		 *	In case anyone's been waiting for a memory
		 *	manager to be established, wake them up.
		 */

		thread_wakeup((event_t) &memory_manager_default);
	}

	simple_unlock(&memory_manager_default_lock);

	*default_manager = returned_manager;
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_manager_default_reference
 *	Purpose:
 *		Returns a naked send right for the default
 *		memory manager.  The returned right is always
 *		valid (not IP_NULL or IP_DEAD).
 */

ipc_port_t	memory_manager_default_reference()
{
	ipc_port_t current_manager;

	simple_lock(&memory_manager_default_lock);

	while (current_manager = ipc_port_copy_send(memory_manager_default),
	       !IP_VALID(current_manager)) {
		thread_sleep((event_t) &memory_manager_default,
			     simple_lock_addr(memory_manager_default_lock),
			     FALSE);
		simple_lock(&memory_manager_default_lock);
	}

	simple_unlock(&memory_manager_default_lock);

	return current_manager;
}

/*
 *	Routine:	memory_manager_default_port
 *	Purpose:
 *		Returns true if the receiver for the port
 *		is the default memory manager.
 *
 *		This is a hack to let ds_read_done
 *		know when it should keep memory wired.
 */

boolean_t	memory_manager_default_port(port)
	ipc_port_t port;
{
	ipc_port_t current;
	boolean_t result;

	simple_lock(&memory_manager_default_lock);
	current = memory_manager_default;
	if (IP_VALID(current)) {
		/*
		 *	There is no point in bothering to lock
		 *	both ports, which would be painful to do.
		 *	If the receive rights are moving around,
		 *	we might be inaccurate.
		 */

		result = port->ip_receiver == current->ip_receiver;
	} else
		result = FALSE;
	simple_unlock(&memory_manager_default_lock);

	return result;
}

void		memory_manager_default_init()
{
	memory_manager_default = IP_NULL;
	simple_lock_init(&memory_manager_default_lock);
}
