/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * Copyright (c) 1991 IBM Corporation 
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation,
 * and that the name IBM not be used in advertising or publicity 
 * pertaining to distribution of the software without specific, written
 * prior permission.
 * 
 * CARNEGIE MELLON AND IBM ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND IBM DISCLAIM ANY LIABILITY OF ANY KIND FOR
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
 * $Log:	seg.h,v $
 * Revision 2.8  93/02/04  07:57:09  danner
 * 	Merge in PS2 support.
 * 	[92/02/22            dbg@ibm]
 * 
 * Revision 2.7  92/01/03  20:08:46  dbg
 * 	Add USER_LDT and USER_FPREGS.  Add defines for selector bits.
 * 	[91/08/20            dbg]
 * 
 * Revision 2.6  91/05/14  16:15:59  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/08  12:41:48  dbg
 * 	Added USER_TSS for TSS that holds IO permission bitmap.
 * 	Removed space for descriptors 8..38.
 * 	Added real_descriptor, real_gate, sel_idx.
 * 	[91/04/26  14:38:07  dbg]
 * 
 * Revision 2.4  91/02/05  17:14:28  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:37:46  mrt]
 * 
 * Revision 2.3  90/08/27  21:58:13  dbg
 * 	Created, to replace old file with same name.
 * 	[90/07/25            dbg]
 * 
 */

#ifndef	_I386_SEG_H_
#define	_I386_SEG_H_

#include <platforms.h>

/*
 * i386 segmentation.
 */

/*
 * Real segment descriptor.
 */
struct real_descriptor {
	unsigned int	limit_low:16,	/* limit 0..15 */
			base_low:16,	/* base  0..15 */
			base_med:8,	/* base  16..23 */
			access:8,	/* access byte */
			limit_high:4,	/* limit 16..19 */
			granularity:4,	/* granularity */
			base_high:8;	/* base 24..31 */
};

struct real_gate {
	unsigned int	offset_low:16,	/* offset 0..15 */
			selector:16,
			word_count:8,
			access:8,
			offset_high:16;	/* offset 16..31 */
};

/*
 * We build descriptors and gates in a 'fake' format to let the
 * fields be contiguous.  We shuffle them into the real format
 * at runtime.
 */
struct fake_descriptor {
	unsigned int	offset:32;		/* offset */
	unsigned int	lim_or_seg:20;		/* limit */
						/* or segment, for gate */
	unsigned int	size_or_wdct:4;		/* size/granularity */
						/* word count, for gate */
	unsigned int	access:8;		/* access */
};

#define	SZ_32		0x4			/* 32-bit segment */
#define	SZ_G		0x8			/* 4K limit field */

#define	ACC_A		0x01			/* accessed */
#define	ACC_TYPE	0x1e			/* type field: */

#define	ACC_TYPE_SYSTEM	0x00			/* system descriptors: */

#define	ACC_LDT		0x02			    /* LDT */
#define	ACC_CALL_GATE_16 0x04			    /* 16-bit call gate */
#define	ACC_TASK_GATE	0x05			    /* task gate */
#define	ACC_TSS		0x09			    /* task segment */
#define	ACC_CALL_GATE	0x0c			    /* call gate */
#define	ACC_INTR_GATE	0x0e			    /* interrupt gate */
#define	ACC_TRAP_GATE	0x0f			    /* trap gate */

#define	ACC_TSS_BUSY	0x02			    /* task busy */

#define	ACC_TYPE_USER	0x10			/* user descriptors */

#define	ACC_DATA	0x10			    /* data */
#define	ACC_DATA_W	0x12			    /* data, writable */
#define	ACC_DATA_E	0x14			    /* data, expand-down */
#define	ACC_DATA_EW	0x16			    /* data, expand-down,
							     writable */
#define	ACC_CODE	0x18			    /* code */
#define	ACC_CODE_R	0x1a			    /* code, readable */
#define	ACC_CODE_C	0x1c			    /* code, conforming */
#define	ACC_CODE_CR	0x1e			    /* code, conforming,
						       readable */
#define	ACC_PL		0x60			/* access rights: */
#define	ACC_PL_K	0x00			/* kernel access only */
#define	ACC_PL_U	0x60			/* user access */
#define	ACC_P		0x80			/* segment present */

/*
 * Components of a selector
 */
#define	SEL_LDT		0x04			/* local selector */
#define	SEL_PL		0x03			/* privilege level: */
#define	SEL_PL_K	0x00			    /* kernel selector */
#define	SEL_PL_U	0x03			    /* user selector */

/*
 * Convert selector to descriptor table index.
 */
#define	sel_idx(sel)	((sel)>>3)

/*
 * User descriptors for MACH - 32-bit flat address space
 */
#define	USER_SCALL	0x07		/* system call gate */
#define	USER_CS		0x17		/* user code segment */
#define	USER_DS		0x1f		/* user data segment */

#define	LDTSZ		4

/*
 * Kernel descriptors for MACH - 32-bit flat address space.
 */
#define	KERNEL_CS	0x08		/* kernel code */
#define	KERNEL_DS	0x10		/* kernel data */
#define	KERNEL_LDT	0x18		/* master LDT */
#define	KERNEL_TSS	0x20		/* master TSS (uniprocessor) */
#define	USER_LDT	0x28		/* place for per-thread LDT */
#define	USER_TSS	0x30		/* place for per-thread TSS
					   that holds IO bitmap */
#define	FPE_CS		0x38		/* floating-point emulator code */
#define	USER_FPREGS	0x40		/* user-mode access to saved
					   floating-point registers */

#ifdef PS2
#define ABIOS_INT_RET   0x48            /* 16 bit return selector for ABIOS */
#define ABIOS_TH_RET    0x50            /* 16 bit return selector for ABIOS */
#define	ABIOS_INT_SS	0x58		/* ABIOS interrupt stack selector */
#define	ABIOS_TH_SS	0x60		/* ABIOS current stack selector */
#define	ABIOS_FIRST_AVAIL_SEL \
			0x68		/* first selector for ABIOS
					   to allocate */
#define GDTSZ           0x300           /* size of gdt table */
#else   /* PS2 */
#define	GDTSZ		9
#endif  /* PS2 */

/*
 * Interrupt table is always 256 entries long.
 */
#define	IDTSZ		256

/*
 * Make_gdt_desc converts a segment base address, limit, access1, and access2
 * fields into a gdt descriptor entry.
 */

#define make_gdt_desc(sel, base, limit, acc1, acc2) \
    { \
	struct real_descriptor *g; \
	g = (struct real_descriptor *) (((char *)gdt) + sel); \
	g->limit_low = limit & 0xffff; \
	g->base_low  = base & 0xffff; \
	g->base_med  = (base >> 16) & 0xff; \
	g->access    = acc1 | ACC_P; \
	g->limit_high= limit >> 16; \
	g->granularity = acc2; \
	g->base_high = base >> 24; \
    }

#endif	/* _I386_SEG_H_ */
