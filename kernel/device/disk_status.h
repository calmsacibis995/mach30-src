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
 * $Log:	disk_status.h,v $
 * Revision 2.7  93/03/09  10:54:16  danner
 * 	Horrible alpha-specific fix.  See apologies in the code.
 * 	[93/02/26            af]
 * 
 * Revision 2.5.1.1  92/12/10  17:33:17  af
 * 	Changed longs into ints, which is what they are anyways.
 * 	When we'll have a disk 1 terabyte big we'll see about it.
 * 	[92/08/02            af]
 * 
 * Revision 2.5  92/05/21  17:09:16  jfriedl
 * 	Appended 'U' to long constant.
 * 	[92/05/21  15:40:18  jfriedl]
 * 
 * Revision 2.4  92/04/03  12:08:31  rpd
 * 	We add one more partition so that we can point to the "whole"
 * 	disk.  It is at slot MAXPARTITIONS.
 * 	[92/04/01            rvb]
 * 
 * Revision 2.3  91/05/14  15:43:50  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/13  06:02:21  af
 * 	Created, starting from Reno's disklabel.h (copyright included).
 * 	[91/05/03            af]
 * 
 */
/*
 * Copyright (c) 1987, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)disklabel.h	7.10 (Berkeley) 6/27/88
 */

#ifndef	_DISK_STATUS_H_
#define	_DISK_STATUS_H_

/*
 * Each disk has a label which includes information about the hardware
 * disk geometry, filesystem partitions, and drive specific information.
 * The label is in block 0 or 1, possibly offset from the beginning
 * to leave room for a bootstrap, etc.
 */

#define LABELSECTOR	0			/* sector containing label */
#define LABELOFFSET	64			/* offset of label in sector */
#define DISKMAGIC	((unsigned int) 0x82564557U)	/* The disk magic number */
#ifndef MAXPARTITIONS
#define	MAXPARTITIONS	8
#endif


#ifndef LOCORE
struct disklabel {
	unsigned int	d_magic;		/* the magic number */
	short	d_type;			/* drive type */
	short	d_subtype;		/* controller/d_type specific */
	char	d_typename[16];		/* type name, e.g. "eagle" */
	/* 
	 * d_packname contains the pack identifier and is returned when
	 * the disklabel is read off the disk or in-core copy.
	 * d_boot0 and d_boot1 are the (optional) names of the
	 * primary (block 0) and secondary (block 1-15) bootstraps
	 * as found in /usr/mdec.  These are returned when using
	 * getdiskbyname(3) to retrieve the values from /etc/disktab.
	 */
#if defined(KERNEL) || defined(STANDALONE)
	char	d_packname[16];			/* pack identifier */ 
#else
	union {
		char	un_d_packname[16];	/* pack identifier */ 
		struct {
			char *un_d_boot0;	/* primary bootstrap name */
			char *un_d_boot1;	/* secondary bootstrap name */
		} un_b; 
	} d_un; 
#define d_packname	d_un.un_d_packname
#define d_boot0		d_un.un_b.un_d_boot0
#define d_boot1		d_un.un_b.un_d_boot1
#endif	/* ! KERNEL or STANDALONE */
			/* disk geometry: */
	unsigned int	d_secsize;		/* # of bytes per sector */
	unsigned int	d_nsectors;		/* # of data sectors per track */
	unsigned int	d_ntracks;		/* # of tracks per cylinder */
	unsigned int	d_ncylinders;		/* # of data cylinders per unit */
	unsigned int	d_secpercyl;		/* # of data sectors per cylinder */
	unsigned int	d_secperunit;		/* # of data sectors per unit */
	/*
	 * Spares (bad sector replacements) below
	 * are not counted in d_nsectors or d_secpercyl.
	 * Spare sectors are assumed to be physical sectors
	 * which occupy space at the end of each track and/or cylinder.
	 */
	unsigned short	d_sparespertrack;	/* # of spare sectors per track */
	unsigned short	d_sparespercyl;		/* # of spare sectors per cylinder */
	/*
	 * Alternate cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	unsigned int	d_acylinders;		/* # of alt. cylinders per unit */

			/* hardware characteristics: */
	/*
	 * d_interleave, d_trackskew and d_cylskew describe perturbations
	 * in the media format used to compensate for a slow controller.
	 * Interleave is physical sector interleave, set up by the formatter
	 * or controller when formatting.  When interleaving is in use,
	 * logically adjacent sectors are not physically contiguous,
	 * but instead are separated by some number of sectors.
	 * It is specified as the ratio of physical sectors traversed
	 * per logical sector.  Thus an interleave of 1:1 implies contiguous
	 * layout, while 2:1 implies that logical sector 0 is separated
	 * by one sector from logical sector 1.
	 * d_trackskew is the offset of sector 0 on track N
	 * relative to sector 0 on track N-1 on the same cylinder.
	 * Finally, d_cylskew is the offset of sector 0 on cylinder N
	 * relative to sector 0 on cylinder N-1.
	 */
	unsigned short	d_rpm;			/* rotational speed */
	unsigned short	d_interleave;		/* hardware sector interleave */
	unsigned short	d_trackskew;		/* sector 0 skew, per track */
	unsigned short	d_cylskew;		/* sector 0 skew, per cylinder */
	unsigned int	d_headswitch;		/* head switch time, usec */
	unsigned int	d_trkseek;		/* track-to-track seek, usec */
	unsigned int	d_flags;		/* generic flags */
#define NDDATA 5
	unsigned int	d_drivedata[NDDATA];	/* drive-type specific information */
#define NSPARE 5
	unsigned int	d_spare[NSPARE];	/* reserved for future use */
	unsigned int	d_magic2;		/* the magic number (again) */
	unsigned short	d_checksum;		/* xor of data incl. partitions */

			/* filesystem and partition information: */
	unsigned short	d_npartitions;		/* number of partitions in following */
	unsigned int	d_bbsize;		/* size of boot area at sn0, bytes */
	unsigned int	d_sbsize;		/* max size of fs superblock, bytes */
	struct	partition {		/* the partition table */
		unsigned int	p_size;		/* number of sectors in partition */
		unsigned int	p_offset;	/* starting sector */
		unsigned int	p_fsize;	/* filesystem basic fragment size */
		unsigned char	p_fstype;	/* filesystem type, see below */
		unsigned char	p_frag;		/* filesystem fragments per block */
		unsigned short	p_cpg;		/* filesystem cylinders per group */
	} d_partitions[MAXPARTITIONS+1];	/* actually may be more */

#if	defined(alpha) && defined(KERNEL)
	/*
	 * Disgusting hack. If this structure contains a pointer,
	 * as it does for non-kernel, then the compiler rounds
	 * the size to make it pointer-sized properly (arrays of..).
	 * But if I define the pointer for the kernel then instances
	 * of this structure better be aligned otherwise picking
	 * up a short might be done by too-smart compilers (GCC) with
	 * a load-long instruction expecting the short to be aligned.
	 * I bet the OSF folks stomped into this too, since they use
	 * the same disgusting hack below.. [whatelse can I do ??]
	 */
	int	bugfix;
#endif
};
#else LOCORE
	/*
	 * offsets for asm boot files.
	 */
	.set	d_secsize,40
	.set	d_nsectors,44
	.set	d_ntracks,48
	.set	d_ncylinders,52
	.set	d_secpercyl,56
	.set	d_secperunit,60
	.set	d_end_,276		/* size of disk label */
#endif LOCORE

/* d_type values: */
#define	DTYPE_SMD		1		/* SMD, XSMD; VAX hp/up */
#define	DTYPE_MSCP		2		/* MSCP */
#define	DTYPE_DEC		3		/* other DEC (rk, rl) */
#define	DTYPE_SCSI		4		/* SCSI */
#define	DTYPE_ESDI		5		/* ESDI interface */
#define	DTYPE_ST506		6		/* ST506 etc. */
#define	DTYPE_FLOPPY		10		/* floppy */

#ifdef DKTYPENAMES
static char *dktypenames[] = {
	"unknown",
	"SMD",
	"MSCP",
	"old DEC",
	"SCSI",
	"ESDI",
	"type 6",
	"type 7",
	"type 8",
	"type 9",
	"floppy",
	0
};
#define DKMAXTYPES	(sizeof(dktypenames) / sizeof(dktypenames[0]) - 1)
#endif

/*
 * Filesystem type and version.
 * Used to interpret other filesystem-specific
 * per-partition information.
 */
#define	FS_UNUSED	0		/* unused */
#define	FS_SWAP		1		/* swap */
#define	FS_V6		2		/* Sixth Edition */
#define	FS_V7		3		/* Seventh Edition */
#define	FS_SYSV		4		/* System V */
#define	FS_V71K		5		/* V7 with 1K blocks (4.1, 2.9) */
#define	FS_V8		6		/* Eighth Edition, 4K blocks */
#define	FS_BSDFFS	7		/* 4.2BSD fast file system */

#ifdef	DKTYPENAMES
static char *fstypenames[] = {
	"unused",
	"swap",
	"Version 6",
	"Version 7",
	"System V",
	"4.1BSD",
	"Eighth Edition",
	"4.2BSD",
	0
};
#define FSMAXTYPES	(sizeof(fstypenames) / sizeof(fstypenames[0]) - 1)
#endif

/*
 * flags shared by various drives:
 */
#define		D_REMOVABLE	0x01		/* removable media */
#define		D_ECC		0x02		/* supports ECC */
#define		D_BADSECT	0x04		/* supports bad sector forw. */
#define		D_RAMDISK	0x08		/* disk emulator */
#define		D_CHAIN		0x10		/* can do back-back transfers */

/*
 * Drive data for SMD.
 */
#define	d_smdflags	d_drivedata[0]
#define		D_SSE		0x1		/* supports skip sectoring */
#define	d_mindist	d_drivedata[1]
#define	d_maxdist	d_drivedata[2]
#define	d_sdist		d_drivedata[3]

/*
 * Drive data for ST506.
 */
#define d_precompcyl	d_drivedata[0]
#define d_gap3		d_drivedata[1]		/* used only when formatting */

/*
 * IBM controller info (d_precompcyl used, too)
 */
#define	d_step		d_drivedata[2]

#ifndef LOCORE
/*
 * Structure used to perform a format
 * or other raw operation, returning data
 * and/or register values.
 * Register identification and format
 * are device- and driver-dependent.
 */
struct format_op {
	char	*df_buf;
	int	df_count;		/* value-result */
	recnum_t	df_startblk;
	int	df_reg[8];		/* result */
};

/*
 * Disk-specific ioctls.
 */
		/* get and set disklabel; DIOCGPART used internally */
#define DIOCGDINFO	_IOR('d', 101, struct disklabel)/* get */
#define DIOCSDINFO	_IOW('d', 102, struct disklabel)/* set */
#define DIOCWDINFO	_IOW('d', 103, struct disklabel)/* set, update disk */

/* do format operation, read or write */
#define DIOCRFORMAT	_IOWR('d', 105, struct format_op)
#define DIOCWFORMAT	_IOWR('d', 106, struct format_op)

#define DIOCSSTEP	_IOW('d', 107, int)	/* set step rate */
#define DIOCSRETRIES	_IOW('d', 108, int)	/* set # of retries */
#define DIOCWLABEL	_IOW('d', 109, int)	/* write en/disable label */

#define DIOCSBAD	_IOW('d', 110, struct dkbad)	/* set kernel dkbad */

#endif LOCORE

#endif	/* _DISK_STATUS_H_ */
