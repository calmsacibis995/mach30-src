/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 * $Log:	thread_status.h,v $
 * Revision 2.4  93/01/26  18:00:43  danner
 * 	Added #ifndef wrapper, int->unsigned
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.3  92/05/04  11:28:13  danner
 * 	Added missing M88100_EXC_STATE_COUNT (from jfriedl).
 * 	[92/04/30            danner]
 * 
 * Revision 2.2  92/02/18  18:03:55  elf
 * 	created.
 * 	[92/01/20            danner]
 * 
 */

#ifndef	_M88100_MACHINE_THREAD_STATE_
#define	_M88100_MACHINE_THREAD_STATE_
/*
 *	m88100_thread_state	basic machine state
 *				
 *      m88100_exc_state       exception state
 *
 */

#define M88100_THREAD_STATE	(1)
#define M88100_EXC_STATE        (2)

struct m88100_thread_state {
    unsigned r[32];
    unsigned fpsr; /* IEEE fp control registers */
    unsigned fpcr; /* IEEE fp control registers */
    unsigned psr;  /* processor status register */
    unsigned sxip; /* pc at time of trouble */
    unsigned snip; /* pc to return to */
    unsigned sfip; /* instruction after snip */
};

struct m88100_exc_state {
    unsigned ssbr;  /* scoreboard registers */
    unsigned dmt0;  /* data unit pipeline control */
    unsigned dmd0;
    unsigned dma0;
    unsigned dmt1;
    unsigned dmd1;
    unsigned dma1;
    unsigned dmt2;
    unsigned dmd2;
    unsigned dma2;
    unsigned fpecr; /* fp exception registers */
    unsigned fphs1;
    unsigned fpls1;
    unsigned fphs2;
    unsigned fpls2;
    unsigned fppt;
    unsigned fprh;
    unsigned fprl;
    unsigned fpit;
    unsigned vector;	/* exception number */
} ;

#define	M88100_THREAD_STATE_COUNT (sizeof(struct m88100_thread_state)/sizeof(int))
#define	M88100_EXC_STATE_COUNT	(sizeof(struct m88100_exc_state)/sizeof(int))

#endif _M88100_MACHINE_THREAD_STATE_
