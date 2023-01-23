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
 * $Log:	machine_thread.c,v $
 * Revision 2.6  93/01/26  18:03:27  danner
 * 	ANSIfied.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.5  92/08/03  17:44:08  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.4  92/05/21  17:20:17  jfriedl
 * 	tried prototypes.
 * 	[92/05/18            jfriedl]
 * 
 * Revision 2.3  92/02/18  18:02:23  elf
 * 	Delete stack_{attach,detach}, thread_exception_return,
 * 	 thread_syscall_return.
 * 	[92/02/11            danner]
 * 
 * Revision 2.2  91/07/09  23:18:42  danner
 * 	Created
 * 	[91/05/10            danner]
 * 
 */

#include <mach_kdb.h>

#if MACH_KDB
void thread_kdb_return(void)
{
  /* drop into the debugger */
  gimmeabreak();
  return;
}
#endif
