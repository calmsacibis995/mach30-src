/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 * $Log:	vm_tuning.h,v $
 * Revision 2.2  92/08/03  17:49:50  jfriedl
 * 	Created m88k copy from luna [danner].
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:20:20  danner
 * 	3.0 Checkin.
 * 	[91/07/09  10:48:44  danner]
 * 
 */

/*
 *	File:	vm_tuning.h
 *
 *	tuning parameters
 */

#define	VM_PAGE_INACTIVE_TARGET(free)	(5 + (free) / 50)
