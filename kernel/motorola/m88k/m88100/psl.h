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
 * $Log:	psl.h,v $
 * Revision 2.6  93/01/26  18:01:15  danner
 * 	Added #ifndef file wrapper.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.5  93/01/14  17:53:26  danner
 * 	u_int -> unsigned
 * 	[92/12/02            jfriedl]
 * 
 * Revision 2.4  92/08/03  17:52:34  jfriedl
 * 	changed ifndef to depend on ASSEMBLER. [danner]
 * 
 * Revision 2.3  92/05/21  17:23:01  jfriedl
 * 	Appended 'U' to constants that would otherwise be signed.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.2  92/02/18  18:03:52  elf
 * 	Moved from luna88k
 * 	[92/01/20            danner]
 * 
 */

#ifndef __MOTOROLA_M88K_M88100_PSL_H__
#define __MOTOROLA_M88K_M88100_PSL_H__
/* 
 * 88100 control registers
 */

/*
 * processor identification register (PID)
 */
#define PID_ARN		0x0000FF00U	/* architectural revision number */
#define PID_VN		0x000000FEU	/* version number */
#define PID_MC		0x00000001U	/* master/checker */

/*
 * processor status register
 */
#define PSR_MODE	0x80000000U	/* supervisor/user mode */
#define PSR_BO		0x40000000U	/* byte-ordering 0:big 1:little */
#define PSR_SER		0x20000000U	/* serial mode */
#define PSR_C		0x10000000U	/* carry */
#define PSR_SFD		0x000003F0U	/* SFU disable */
#define PSR_SFD1	0x00000008U	/* SFU1 (FPU) disable */
#define PSR_MXM		0x00000004U	/* misaligned access enable */
#define PSR_IND		0x00000002U	/* interrupt disable */
#define PSR_SFRZ	0x00000001U	/* shadow freeze */

/*
 *	This is used in ext_int() and hard_clock().
 */
#define PSR_IPL		0x00001000	/* for basepri */
#define PSR_IPL_LOG	12		/* = log2(PSR_IPL) */

#define PSR_MODE_LOG	31		/* = log2(PSR_MODE) */
#define PSR_BO_LOG	30		/* = log2(PSR_BO) */
#define PSR_SER_LOG	29		/* = log2(PSR_SER) */
#define PSR_SFD1_LOG	3		/* = log2(PSR_SFD1) */
#define PSR_MXM_LOG	2		/* = log2(PSR_MXM) */
#define PSR_IND_LOG	1		/* = log2(PSR_IND) */
#define PSR_SFRZ_LOG	0		/* = log2(PSR_SFRZ) */

#define PSR_SUPERVISOR	(PSR_MODE | PSR_SFD)
#define PSR_USER	(PSR_SFD)
#define PSR_SET_BY_USER	(PSR_BO | PSR_SER | PSR_C | PSR_MXM)

#ifndef	ASSEMBLER
struct psr {
    unsigned
	psr_mode: 1,
	psr_bo  : 1,
	psr_ser : 1,
	psr_c   : 1,
	        :18,
	psr_sfd : 6,
	psr_sfd1: 1,
	psr_mxm : 1,
	psr_ind : 1,
	psr_sfrz: 1;
};
#endif 

#define FIP_V		0x00000002U	/* valid */
#define FIP_E		0x00000001U	/* exception */
#define FIP_ADDR	0xFFFFFFFCU	/* address mask */
#define NIP_V		0x00000002U	/* valid */
#define NIP_E		0x00000001U	/* exception */
#define NIP_ADDR	0xFFFFFFFCU	/* address mask */
#define XIP_V		0x00000002U	/* valid */
#define XIP_E		0x00000001U	/* exception */
#define XIP_ADDR	0xFFFFFFFCU	/* address mask */

#endif /* __MOTOROLA_M88K_M88100_PSL_H__ */
