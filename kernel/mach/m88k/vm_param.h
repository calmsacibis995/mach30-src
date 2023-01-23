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
 * $Log:	vm_param.h,v $
 * Revision 2.3  93/01/14  17:43:24  danner
 * 	Appended 'U' to constants with high bit set.
 * 	[92/12/02            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:51:33  jfriedl
 * 	Created from luna88k version.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.4  91/07/11  11:01:22  danner
 * 	Copyright Fixes
 * 
 * Revision 2.3  91/07/09  23:21:56  danner
 * 	Initial Checkin
 * 	[91/06/26  12:40:27  danner]
 * 
 *
 */
/*
 *	File:	vm_param.h
 *
 *	machine dependent virtual memory parameters.
 *	Most of the declarations are preceeded by M88K_ (or m88k_)
 *	which is OK because only M88K specific code will be using
 *	them.
 */


#ifndef	_MACHINE_VM_PARAM_
#define _MACHINE_VM_PARAM_

#define BYTE_SIZE	8	/* byte size in bits */

#define M88K_PGBYTES	(1<<12)	/* bytes per m88k page */
#define M88K_PGSHIFT	12	/* number of bits to shift for pages */

/*
 *	Convert bytes to pages and convert pages to bytes.
 *	No rounding is used.
 */

#define	m88k_btop(x)		(((unsigned)(x)) >> M88K_PGSHIFT)
#define	m88k_ptob(x)		(((unsigned)(x)) << M88K_PGSHIFT)

/*
 *	Round off or truncate to the nearest page.  These will work
 *	for either addresses or counts.  (i.e. 1 byte rounds to 1 page
 *	bytes.
 */

#define m88k_round_page(x)	((((unsigned)(x)) + M88K_PGBYTES - 1) & \
					~(M88K_PGBYTES-1))
#define m88k_trunc_page(x)	(((unsigned)(x)) & ~(M88K_PGBYTES-1))

#define	VM_MIN_ADDRESS	((vm_offset_t) 0)
#define	VM_MAX_ADDRESS	((vm_offset_t) 0xffc00000U)

#define	VM_MIN_USER_ADDRESS	((vm_offset_t) 0)
#define	VM_MAX_USER_ADDRESS	((vm_offset_t) 0xffc00000U)

/* on vme188, max = 0xf0000000 */

#define VM_MIN_KERNEL_ADDRESS	((vm_offset_t) 0)
#define VM_MAX_KERNEL_ADDRESS	((vm_offset_t) 0x1fffffff)

#define KERNEL_STACK_SIZE	(2*4096)	/* kernel stack size */
#define INTSTACK_SIZE		(2*4096)	/* interrupt stack size */

/*
 *	Conversion between MACHINE pages and VM pages
 */

#define trunc_m88k_to_vm(p)	(atop(trunc_page(m88k_ptob(p))))
#define round_m88k_to_vm(p)	(atop(round_page(m88k_ptob(p))))
#define vm_to_m88k(p)		(m88k_btop(ptoa(p)))

#if	1	/*Do we really need all this stuff*/
#if	1	/*Do we really need all this stuff*/
#if	1	/*Do we really need all this stuff*/
#define	M88K_SGPAGES	(1<<10)	/* pages per m88k segment */
#define	M88K_SGPGSHIFT	10	/* number of bits to shift for segment-page */
#define	M88K_ALSEGMS	(1<<10)	/* segments per m88k all space */
#define	M88K_ALSGSHIFT	10	/* number of bits to shift for all-segment */

#define	M88K_SGBYTES	(1<<22)	/* bytes per m88k segments */
#define	M88K_SGSHIFT	22	/* number of bits to shift for segment */
#define	M88K_ALPAGES	(1<<20)	/* pages per m88k all space */
#define	M88K_ALPGSHIFT	20	/* number of bits to shift for all-page */

/*
 *	Convert bytes to pages and convert pages to bytes.
 *	No rounding is used.
 */

#define	m88k_btopr(x)		(((unsigned)(x) + (M88K_PGBYTES - 1)) >> M88K_PGSHIFT)
#define	m88k_btosr(x)		(((unsigned)(x) + (M88K_SGBYTES - 1)) >> M88K_SGSHIFT)
#define	m88k_btos(x)		(((unsigned)(x)) >> M88K_SGSHIFT)
#define	m88k_stob(x)		(((unsigned)(x)) << M88K_SGSHIFT)
#define	m88k_ptosr(x)		(((unsigned)(x) + (M88K_SGPAGES - 1)) >> M88K_SGPGSHIFT)
#define	m88k_ptos(x)		(((unsigned)(x)) >> M88K_SGPGSHIFT)
#define	m88k_stop(x)		(((unsigned)(x)) << M88K_SGPGSHIFT)

/*
 *	Round off or truncate to the nearest page.  These will work
 *	for either addresses or counts.  (i.e. 1 byte rounds to 1 page
 *	bytes.
 */

#define m88k_round_segm(x)	((((unsigned)(x)) + M88K_SGBYTES - 1) & \
					~(M88K_SGBYTES-1))
#define m88k_next_segm(x)	((((unsigned)(x)) & ~(M88K_SGBYTES-1)) + \
					M88K_SGBYTES)
#define m88k_trunc_segm(x)	(((unsigned)(x)) & ~(M88K_SGBYTES-1))

#define m88k_round_seg(x)	((((unsigned)(x)) + M88K_SGBYTES - 1) & \
					~(M88K_SGBYTES-1))
#define m88k_trunc_seg(x)	(((unsigned)(x)) & ~(M88K_SGBYTES-1))

#define	VEQR_ADDR	0x20000000	/* kernel virtual eq phy mapping */
#endif	/*  Do we really need all this stuff */
#endif	/*  Do we really need all this stuf  */
#endif	/*  Do we really need all this stuff */

#endif	_MACHINE_VM_PARAM_
