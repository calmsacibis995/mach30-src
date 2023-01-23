/* 
 * Mach Operating System
 * Copyright (c) 1993,1992 Carnegie Mellon University
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
 * $Log:	pc_sample.h,v $
 * Revision 2.3  93/08/03  12:31:37  mrt
 * 	Defined some flavors for sampling.
 * 	[93/07/30  10:26:03  bershad]
 * 
 * Revision 2.2  93/01/24  13:20:55  danner
 * 	Added sample_control_t.
 * 	[93/01/15            rvb]
 * 
 *	File:	mach/pc_sample.h
 *	Author:	Brian Bershad
 *	Date:	January 1992
 *
 *	Mach external interface definitions -- file two
 *
 */

#ifndef	_MACH_PC_SAMPLE_H_
#define _MACH_PC_SAMPLE_H_

#include <mach/machine/vm_types.h>



typedef unsigned sampled_pc_flavor_t;


#define SAMPLED_PC_PERIODIC			0x1	/* default */


#define SAMPLED_PC_VM_ZFILL_FAULTS		0x10
#define SAMPLED_PC_VM_REACTIVATION_FAULTS	0x20
#define SAMPLED_PC_VM_PAGEIN_FAULTS		0x40
#define SAMPLED_PC_VM_COW_FAULTS		0x80
#define SAMPLED_PC_VM_FAULTS_ANY		0x100
#define SAMPLED_PC_VM_FAULTS		\
			(SAMPLED_PC_VM_ZFILL_FAULTS | \
			 SAMPLED_PC_VM_REACTIVATION_FAULTS |\
			 SAMPLED_PC_VM_PAGEIN_FAULTS |\
			 SAMPLED_PC_VM_COW_FAULTS )




/*
 *	Control structure.
 */



typedef struct sample_control {
    vm_offset_t		buffer;
    unsigned int	seqno;
    sampled_pc_flavor_t sampletypes;
} sample_control_t;


/*
 *	Definitions for the PC sampling interface.
 */

typedef struct sampled_pc {
    unsigned int	id;
    vm_offset_t		pc;
    sampled_pc_flavor_t sampletype;
} sampled_pc_t;

typedef sampled_pc_t *sampled_pc_array_t;
typedef unsigned sampled_pc_seqno_t;


#ifdef KERNEL
extern void take_pc_sample();
#endif KERNEL


#endif	_MACH_PC_SAMPLE_H_
