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
 * $Log:	panic.c,v $
 * Revision 2.3  93/05/10  17:51:05  rvb
 * 	Put a delay loop before actually panicing. Otherwise
 * 	the panic message can get lost or scrambled. Currently
 * 	#ifdef PC532 but could be generic.
 * 	Ian Dall <DALL@hfrd.dsto.gov.au>	4/28/93
 * 	[93/05/10  13:18:58  rvb]
 * 
 * Revision 2.2  92/02/20  15:58:04  elf
 * 	Created from bootstrap's version.
 * 	[92/02/11            rpd]
 * 
 */

#include <mach/port.h>
#include <varargs.h>

static mach_port_t master_host_port;

panic_init(port)
	mach_port_t port;
{
	master_host_port = port;
}

/*VARARGS1*/
panic(s, va_alist)
	char *s;
	va_dcl
{
	va_list listp;

	printf("panic: ");
	va_start(listp);
	vprintf(s, listp);
	va_end(listp);
	printf("\n");

#ifdef PC532
	{ int l; for (l=0;l < 1000000;l++) ; }
#endif PC532
#define RB_DEBUGGER	0x1000	/* enter debugger NOW */
	(void) host_reboot(master_host_port, RB_DEBUGGER);
}
