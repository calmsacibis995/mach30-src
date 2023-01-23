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
 * $Log:	softint.c,v $
 * Revision 2.7  93/01/26  18:04:04  danner
 * 	ANSIfied. Cleaned up includes.
 * 	[93/01/23            jfriedl]
 * 
 * Revision 2.6  93/01/14  17:38:58  danner
 * 	Gcc -Wall cleanup.
 * 	[92/12/07            jfriedl]
 * 
 * Revision 2.5  92/08/03  17:44:52  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.4  92/05/21  17:21:06  jfriedl
 * 	Added void type to fcns that needed. Some cleanup for gcc warnings.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.3  92/02/18  18:02:45  elf
 * 	luna88k/cpu.h -> luna88k/cpu_number.h
 * 	[92/01/21            danner]
 * 
 * Revision 2.2  91/07/09  23:19:42  danner
 * 	Initial Checkin
 * 	[91/06/26  12:32:00  danner]
 * 
 * Revision 2.3  91/05/07  11:12:54  rvb
 * 	Added include of luna88k/cpu.h to get cpu_number macro.
 * 	[91/04/12            danner]
 * 
 * Revision 2.2  91/04/05  14:05:12  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/* 
 * Handle software interrupts through 'softcall' mechanism
 */

#include <luna88k/machdep.h>
#include <luna88k/softint.h>

static simple_lock_data_t softcall_lock;
static struct softcall *softhead[MAX_CPUS];
static struct softcall *softtail[MAX_CPUS];
static struct softcall *softfree;

/*
 * Initialization, called from "luna88k/autoconf.c"
 */
void softcall_init(void)
{
    struct softcall *sc;

    for (sc = softcalls; sc < &softcalls[NSOFTCALLS]; sc++) {
	sc->sc_next = softfree;
	softfree = sc;
    }
    simple_lock_init(&softcall_lock);
}

/*
 * Call function func with argument arg 
 * at some later time at software interrupt priority
 */
void softcall(func_t func, vm_offset_t arg, int cpu)
{
	struct softcall *sc;
	int s;

	s = splhigh();
	simple_lock(&softcall_lock);
	/* coalesce identical softcalls */
	for (sc = softhead[cpu]; sc != 0; sc = sc->sc_next)
		if (sc->sc_func == func && sc->sc_arg == arg)
			goto out;
	if ((sc = softfree) == 0)
		panic("too many softcalls");
	softfree = sc->sc_next;
	sc->sc_func = func;
	sc->sc_arg = arg;
	sc->sc_next = 0;
	if (softhead[cpu]) {
		softtail[cpu]->sc_next = sc;
		softtail[cpu] = sc;
	} else {
		softhead[cpu] = softtail[cpu] = sc;
		softint_on(cpu);
	}
    out:
	simple_unlock(&softcall_lock);
	splx(s);
}

/*
 * Called to process software interrupts take one off queue, call it, repeat.
 * Note queue may change during call.
 * Called from "luna88k/machdep.c"
 */
void softint(void)
{
	func_t func = 0; /* '=0' to shut up lint */
	vm_offset_t arg = 0; /* '=0' to shut up lint */
	struct softcall *sc;
	int cpu = cpu_number();
	int s;

	for (;;) {
		s = splhigh();
		simple_lock(&softcall_lock);
		if (sc = softhead[cpu], sc != 0) {
			func = sc->sc_func;
			arg = sc->sc_arg;
			softhead[cpu] = sc->sc_next;
			sc->sc_next = softfree;
			softfree = sc;
		}
		simple_unlock(&softcall_lock);
		splx(s);
		if (sc == 0)
			return;
		(*func)(arg);
	}
}
