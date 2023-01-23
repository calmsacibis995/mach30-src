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
 * $Log:	rom1ep.h,v $
 * Revision 2.3  93/01/26  18:04:09  danner
 * 	Added #ifndef file wrapper.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:19:25  danner
 * 	Initial Checkin
 * 	[91/06/26  12:31:29  danner]
 * 
 * Revision 2.2  91/04/05  14:04:45  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */
#ifndef __LUNA88K_ROM1EP_H__
#define __LUNA88K_ROM1EP_H__

/*
 * Function table for kernel in ROM
 */
struct rom1ep {
							/*     offset */
    int			EPromversion;			/*  (0) 0x00 */
    int 		*EPMemorySize;			/*  (1) 0x04 */
    int 		(*EPstartmon)();		/*  (2) 0x08 */
    int 		(*EPmayget)();			/*  (3) 0x0c */
    int 		(*EPmayput)();			/*  (4) 0x10 */
    int			*EPgrtype;			/*  (5) 0x14 */
    int 		(*EPdisp)();			/*  (6) 0x18 */
    int 		(*EPundisp)();			/*  (7) 0x1c */
    unsigned char	(*EPgetxargs)[];		/*  (8) 0x20 */
    char		EPsoft_intr_rom[4];		/*  (9) 0x24 */

    long		reserve22;			/* (10) 0x28 */
    long		reserve21;			/* (11) 0x2c */
    long		reserve20;			/* (12) 0x30 */
    long		reserve19;			/* (13) 0x34 */
    long		reserve18;			/* (14) 0x38 */
    long		reserve17;			/* (15) 0x3c */
    long		reserve16;			/* (16) 0x40 */
    long		reserve15;			/* (17) 0x44 */
    long		reserve14;			/* (18) 0x48 */
    long		reserve13;			/* (19) 0x4c */
    long		reserve12;			/* (20) 0x50 */
    long		reserve11;			/* (21) 0x54 */
    long		reserve10;			/* (22) 0x58 */
    long		reserve9;			/* (23) 0x5c */
    long		reserve8;			/* (24) 0x60 */
    long		reserve7;			/* (25) 0x64 */
    long		reserve6;			/* (26) 0x68 */
    long		reserve5;			/* (27) 0x6c */
    long		reserve4;			/* (28) 0x70 */
    long		reserve3;			/* (29) 0x74 */
    long		reserve2;			/* (30) 0x78 */
    long		reserve1;			/* (31) 0x7c */
};


#define	RVPtr		((struct rom1ep *)0x1100)

#define mayput(x)	(*RVPtr->EPmayput)(x)	/* Rom 1 char out */
#define mayget()	(*RVPtr->EPmayget)()	/* Rom 1 char get */
#define	GETXARGS	(RVPtr->EPgetxargs)

#define CONSON()	(*RVPtr->EPdisp)()	/* console on */
#define CONSOFF()	(*RVPtr->EPundisp)()	/* console off */

#endif /* __LUNA88K_ROM1EP_H__ */
