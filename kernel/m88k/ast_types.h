/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	ast_types.h,v $
 * Revision 2.2  92/08/03  17:47:07  jfriedl
 * 	Created from luna.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:16:31  danner
 * 	3.0 Initial Checkin
 * 	[91/07/09  10:26:02  danner]
 * 
 * Revision 2.2  91/04/05  14:01:02  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

#ifndef	_LUNA_AST_TYPES_H_
#define	_LUNA_AST_TYPES_H_

/*
 *	Data type for remote ast_check() invocation support.  Currently
 *	not implemented.  Do this first to avoid include problems.
 */
typedef	int	ast_check_t;

#endif	_LUNA_AST_TYPES_H_
