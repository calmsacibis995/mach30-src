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
 * $Log:	asic.h,v $
 * Revision 2.2  92/03/02  18:33:32  rpd
 * 	Created, from the DEC specs:
 * 	"IO Controller ASIC Functional Specifications"
 * 	Workstation Systems Engineering, Palo Alto, CA. Feb 1, 1991.
 * 	[92/01/17            af]
 * 
 */
/*
 *	File: asic.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	1/92
 *
 *	Definitions specific to the IO Controller ASIC, used
 *	on 3min and MAXine motherboards.
 */

#ifndef	MIPS_ASIC_H
#define	MIPS_ASIC_H 1

/*
 * Slot definitions
 */

#define	ASIC_SLOT_0_START	0x000000
#define	ASIC_SLOT_1_START	0x040000
#define	ASIC_SLOT_2_START	0x080000
#define	ASIC_SLOT_3_START	0x0c0000
#define	ASIC_SLOT_4_START	0x100000
#define	ASIC_SLOT_5_START	0x140000
#define	ASIC_SLOT_6_START	0x180000
#define	ASIC_SLOT_7_START	0x1c0000
#define	ASIC_SLOT_8_START	0x200000
#define	ASIC_SLOT_9_START	0x240000
#define	ASIC_SLOT_10_START	0x280000
#define	ASIC_SLOT_11_START	0x2c0000
#define	ASIC_SLOT_12_START	0x300000
#define	ASIC_SLOT_13_START	0x340000
#define	ASIC_SLOT_14_START	0x380000
#define	ASIC_SLOT_15_START	0x3c0000
#define	ASIC_SLOTS_END		0x3fffff

/*
 *  Register offsets (slot 1)
 */

#define	ASIC_SCSI_DMAPTR	ASIC_SLOT_1_START+0x000
#define	ASIC_SCSI_NEXTPTR	ASIC_SLOT_1_START+0x010
#define	ASIC_LANCE_DMAPTR	ASIC_SLOT_1_START+0x020
#define	ASIC_SCC_T1_DMAPTR	ASIC_SLOT_1_START+0x030
#define	ASIC_SCC_R1_DMAPTR	ASIC_SLOT_1_START+0x040
#define	ASIC_SCC_T2_DMAPTR	ASIC_SLOT_1_START+0x050
#define	ASIC_SCC_R2_DMAPTR	ASIC_SLOT_1_START+0x060
#define	ASIC_FLOPPY_DMAPTR	ASIC_SLOT_1_START+0x070
#define	ASIC_ISDN_X_DMAPTR	ASIC_SLOT_1_START+0x080
#define	ASIC_ISDN_X_NEXTPTR	ASIC_SLOT_1_START+0x090
#define	ASIC_ISDN_R_DMAPTR	ASIC_SLOT_1_START+0x0a0
#define	ASIC_ISDN_R_NEXTPTR	ASIC_SLOT_1_START+0x0b0
#define	ASIC_BUFF0		ASIC_SLOT_1_START+0x0c0
#define	ASIC_BUFF1		ASIC_SLOT_1_START+0x0d0
#define	ASIC_BUFF2		ASIC_SLOT_1_START+0x0e0
#define	ASIC_BUFF3		ASIC_SLOT_1_START+0x0f0
#define	ASIC_CSR		ASIC_SLOT_1_START+0x100
#define	ASIC_INTR		ASIC_SLOT_1_START+0x110
#define	ASIC_IMSK		ASIC_SLOT_1_START+0x120
#define	ASIC_CURADDR		ASIC_SLOT_1_START+0x130
#define	ASIC_ISDN_X_DATA	ASIC_SLOT_1_START+0x140
#define	ASIC_ISDN_R_DATA	ASIC_SLOT_1_START+0x150
#define	ASIC_LANCE_DECODE	ASIC_SLOT_1_START+0x160
#define	ASIC_SCSI_DECODE	ASIC_SLOT_1_START+0x170
#define	ASIC_SCC0_DECODE	ASIC_SLOT_1_START+0x180
#define	ASIC_SCC1_DECODE	ASIC_SLOT_1_START+0x190
#define	ASIC_FLOPPY_DECODE	ASIC_SLOT_1_START+0x1a0
#define	ASIC_SCSI_SCR		ASIC_SLOT_1_START+0x1b0
#define	ASIC_SCSI_SDR0		ASIC_SLOT_1_START+0x1c0
#define	ASIC_SCSI_SDR1		ASIC_SLOT_1_START+0x1d0

/* system Status and Control register (SSR) */

#define ASIC_CSR_DMAEN_T1	0x80000000	/* rw */
#define ASIC_CSR_DMAEN_R1	0x40000000	/* rw */
#define ASIC_CSR_DMAEN_T2	0x20000000	/* rw */
#define ASIC_CSR_DMAEN_R2	0x10000000	/* rw */
#define ASIC_CSR_xxx		0x0f800000	/* reserved */
#define ASIC_CSR_FLOPPY_DIR	0x00400000	/* rw */
#define ASIC_CSR_DMAEN_FLOPPY	0x00200000	/* rw */
#define ASIC_CSR_DMAEN_ISDN_T	0x00100000	/* rw */
#define ASIC_CSR_DMAEN_ISDN_R	0x00080000	/* rw */
#define ASIC_CSR_SCSI_DIR	0x00040000	/* rw */
#define ASIC_CSR_DMAEN_SCSI	0x00020000	/* rw */
#define ASIC_CSR_DMAEN_LANCE	0x00010000	/* rw */
/* low 16 bits are rw gp outputs */

/* system Interrupt register (and interrupt mask register) */

#define	ASIC_INTR_T1_PAGE_END	0x80000000	/* rz */
#define	ASIC_INTR_T1_READ_E	0x40000000	/* rz */
#define	ASIC_INTR_R1_HALF_PAGE	0x20000000	/* rz */
#define	ASIC_INTR_R1_DMA_OVRUN	0x10000000	/* rz */
#define	ASIC_INTR_T2_PAGE_END	0x08000000	/* rz */
#define	ASIC_INTR_T2_READ_E	0x04000000	/* rz */
#define	ASIC_INTR_R2_HALF_PAGE	0x02000000	/* rz */
#define	ASIC_INTR_R2_DMA_OVRUN	0x01000000	/* rz */
#define	ASIC_INTR_FLOPPY_DMA_E	0x00800000	/* rz */
#define	ASIC_INTR_ISDN_PTR_LOAD	0x00400000	/* rz */
#define	ASIC_INTR_ISDN_OVRUN	0x00200000	/* rz */
#define	ASIC_INTR_ISDN_READ_E	0x00100000	/* rz */
#define	ASIC_INTR_SCSI_PTR_LOAD	0x00080000	/* rz */
#define	ASIC_INTR_SCSI_OVRUN	0x00040000	/* rz */
#define	ASIC_INTR_SCSI_READ_E	0x00020000	/* rz */
#define	ASIC_INTR_LANCE_READ_E	0x00010000	/* rz */
/* low 16 are gp interrupts */

/* DMA pointer registers (SCSI, Comm, ...) */

#define	ASIC_DMAPTR_MASK	0xffffffe0
#define	ASIC_DMAPTR_SHIFT	5
#	define	ASIC_DMAPTR_SET(reg,val)	\
			(reg) = (((val)<<ASIC_DMAPTR_SHIFT)&ASIC_DMAPTR_MASK)
#	define	ASIC_DMAPTR_GET(reg,val)	\
			(val) = (((reg)&ASIC_DMAPTR_MASK)>>ASIC_DMAPTR_SHIFT)

/* For the LANCE DMA pointer register initialization the above suffices */

/* More SCSI DMA registers */

#define	ASIC_SCR_STATUS		0x00000004
#define	ASIC_SCR_WORD		0x00000003

/* Various Decode registers */

#define	ASIC_DECODE_HW_ADDRESS	0x000003f0
#define	ASIC_DECODE_CHIP_SELECT	0x0000000f

#endif	MIPS_ASIC_H
