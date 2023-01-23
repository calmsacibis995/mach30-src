/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
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
 * $Log:	softint.h,v $
 * Revision 2.3  93/01/26  18:04:19  danner
 * 	Added #ifndef file wrapper, prototypes.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:19:45  danner
 * 	Initial Checkin
 * 	[91/06/26  12:32:06  danner]
 * 
 * Revision 2.2  91/04/05  14:05:18  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

#ifndef __LUNA88K_SOFTINT_H__
#define __LUNA88K_SOFTINT_H__

#include <mach/m88k/vm_types.h> /* vm_offset_t */

/*
 * Definitions needed for pending AST's
 */
#define NSOFTCALLS	50

typedef int (*func_t)();

struct softcall {
	func_t	sc_func;		/* function to call */
	vm_offset_t	sc_arg;			/* arg to pass to func */
	struct softcall *sc_next;	/* next in list */
} softcalls[NSOFTCALLS];


/*
 * functions defined in "luna88k/softint.c"
 */
extern void softcall_init(void);
extern void softcall(func_t func, vm_offset_t arg, int cpu);
extern void softint(void);

#endif /* __LUNA88K_SOFTINT_H__ */
