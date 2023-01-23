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
 * $Log:	ipc_splay.h,v $
 * Revision 2.5  91/10/09  16:10:55  af
 * 	 Revision 2.4.2.1  91/09/16  10:16:03  rpd
 * 	 	Fixed typo in ipc_splay_tree_insert declaration.
 * 	 	[91/09/02            rpd]
 * 
 * Revision 2.4.2.1  91/09/16  10:16:03  rpd
 * 	Fixed typo in ipc_splay_tree_insert declaration.
 * 	[91/09/02            rpd]
 * 
 * Revision 2.4  91/05/14  16:37:18  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:24:09  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:51:53  mrt]
 * 
 * Revision 2.2  90/06/02  14:51:54  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:04:02  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_splay.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Declarations of primitive splay tree operations.
 */

#ifndef	_IPC_IPC_SPLAY_H_
#define _IPC_IPC_SPLAY_H_

#include <mach/port.h>
#include <kern/assert.h>
#include <kern/macro_help.h>
#include <ipc/ipc_entry.h>

typedef struct ipc_splay_tree {
	mach_port_t ist_name;		/* name used in last lookup */
	ipc_tree_entry_t ist_root;	/* root of middle tree */
	ipc_tree_entry_t ist_ltree;	/* root of left tree */
	ipc_tree_entry_t *ist_ltreep;	/* pointer into left tree */
	ipc_tree_entry_t ist_rtree;	/* root of right tree */
	ipc_tree_entry_t *ist_rtreep;	/* pointer into right tree */
} *ipc_splay_tree_t;

#define	ist_lock(splay)		/* no locking */
#define ist_unlock(splay)	/* no locking */

extern void
ipc_splay_tree_init(/* ipc_splay_tree_t splay */);

extern boolean_t
ipc_splay_tree_pick(/* ipc_splay_tree_t splay,
		       mach_port_t *namep, ipc_tree_entry_t *entryp */);

extern ipc_tree_entry_t
ipc_splay_tree_lookup(/* ipc_splay_tree_t splay, mach_port_t name */);

extern void
ipc_splay_tree_insert(/* ipc_splay_tree_t splay, mach_port_t name,
			 ipc_tree_entry_t entry */);

extern void
ipc_splay_tree_delete(/* ipc_splay_tree_t splay, mach_port_t name,
			 ipc_tree_entry_t entry */);

extern void
ipc_splay_tree_split(/* ipc_splay_tree_t splay, mach_port_t name,
			ipc_splay_tree_t small */);

extern void
ipc_splay_tree_join(/* ipc_splay_tree_t splay, ipc_splay_tree_t small */);

extern void
ipc_splay_tree_bounds(/* ipc_splay_tree_t splay, mach_port_t name,
			 mach_port_t *lowerp, mach_port_t *upperp */);

extern ipc_tree_entry_t
ipc_splay_traverse_start(/* ipc_splay_tree_t splay */);

extern ipc_tree_entry_t
ipc_splay_traverse_next(/* ipc_splay_tree_t splay, boolean_t delete */);

extern void
ipc_splay_traverse_finish(/* ipc_splay_tree_t splay */);

#endif	_IPC_IPC_SPLAY_H_
