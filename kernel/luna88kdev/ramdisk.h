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
 * $Log:	ramdisk.h,v $
 * Revision 2.2  91/07/09  23:21:13  danner
 * 	Initial Checkin
 * 	[91/06/26  11:45:55  danner]
 * 
 * Revision 2.2  91/04/05  14:09:54  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/* define the RAM dsik symbol */

#define	MINI_ROOT_START		(4*1024*1024)	/* default byte address */
#define	MINI_ROOT_SIZE		(4)		/* default M bytes */
#define	RAMROOTDEV		0x0400		/* ramdisk device no. */

#define	MEGA	(1024*1024)

#ifdef	KERNEL
extern	int ra_miniroot_exist;		/* mini-root flag on RAM disk */
extern	int ra_miniroot_start;		/* mini-root flag on RAM disk */
extern	int ra_miniroot_size;		/* mini-root flag on RAM disk */
extern	int ra_ramdisk_size;		/* RAM DISK size set at luna_init */
extern	int ra_ramdisk_start;		/* RAM DISK start address set at luna_init */
#endif	KERNEL
