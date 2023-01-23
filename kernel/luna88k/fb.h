/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1992 OMRON Corporation
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
 * $Log:	fb.h,v $
 * Revision 2.3  93/01/26  18:04:36  danner
 * 	Added #ifndef wrapper
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  93/01/14  17:38:35  danner
 * 	Created.
 * 	[92/12/07            jfriedl]
 * 
 */
#ifndef __LUNA88K_FB_H__
#define __LUNA88K_FB_H__

/* minor device numbers for driver */
#define FBMAP		0		/* Frame memory	*/
#define ACMAP		1		/* CRTC-2	*/
#define PCEX		2		/* PC9800 board */
#define MASKROM		3		/* MASK ROM 	*/
#define PALLET		4		/* PALLET reg	*/
#define ALLMAP		5		/* All map */

#define FB_SET_CONS_ON		0x123410
#define FB_WRITE_LOGO		0x123411
#define FB_SET_CONS_OFF		0x123420
#define FB_SET_RFCNT		0x123430
#define FB_SET_BMSEL		0x123440
#define FB_GET_PLANES		0x123450
#define FB_MAP_VIA_BATC		0x123460

#endif /* __LUNA88K_FB_H__ */
