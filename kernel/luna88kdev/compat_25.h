/*
 * HISTORY
 * $Log:	compat_25.h,v $
 * Revision 2.4  93/03/09  17:58:49  danner
 * 	unused.
 * 	[93/03/09            jfriedl]
 * 
 */
I B D-funkt

#if 0
#define MIN(a,b)		((a) < (b) ? (a) : (b))

#define O_NDELAY		D_NOWAIT

#define	ESUCCESS                D_SUCCESS		
#define	EIO			D_IO_ERROR		
#define	ENXIO			D_NO_SUCH_DEVICE	
#define	EINVAL			D_INVALID_SIZE		
#define	EBUSY			D_ALREADY_OPEN		
/* #define	EINVAL			D_INVALID_OPERATION	*/
#define ENOMEM			D_NO_MEMORY		
#define EWOULDBLOCK		D_WOULD_BLOCK		
/* #define EIO			D_DEVICE_DOWN		 */
#define	EROFS			D_INVALID_OPERATION /* READ_ONLY */
#define EACCES			D_INVALID_OPERATION
#define EBADF			D_INVALID_OPERATION

#define S_IFMT  0170000         /* type of file */
#define         S_IFDIR 0040000 /* directory */
#define         S_IFCHR 0020000 /* character special */
#define         S_IFBLK 0060000 /* block special */
#define         S_IFREG 0100000 /* regular */
#define         S_IFLNK 0120000 /* symbolic link */
#define         S_IFSOCK 0140000/* socket */
#define S_ISUID 0004000         /* set user id on execution */
#define S_ISGID 0002000         /* set group id on execution */
#define S_ISVTX 0001000         /* save swapped text even after use */
#define S_IREAD 0000400         /* read permission, owner */
#define S_IWRITE 0000200        /* write permission, owner */
#define S_IEXEC 0000100         /* execute/search permission, owner */


#define FWRITE           00002

#define PZERO   25

/*
 * Each disk has a label which includes information about the hardware
 * disk geometry, filesystem partitions, and drive specific information.
 * The label is in block 0 or 1, possibly offset from the beginning
 * to leave room for a bootstrap, etc.
 */

#define LABELSECTOR	0			/* sector containing label */
#define LABELOFFSET	64			/* offset of label in sector */
#define DISKMAGIC	0x82564557U		/* The disk magic number */
#ifndef MAXPARTITIONS
#define	MAXPARTITIONS	8
#endif


struct disklabel {
	u_long	d_magic;		/* the magic number */
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
	char	d_packname[16];			/* pack identifier */ 
			/* disk geometry: */
	u_long	d_secsize;		/* # of bytes per sector */
	u_long	d_nsectors;		/* # of data sectors per track */
	u_long	d_ntracks;		/* # of tracks per cylinder */
	u_long	d_ncylinders;		/* # of data cylinders per unit */
	u_long	d_secpercyl;		/* # of data sectors per cylinder */
	u_long	d_secperunit;		/* # of data sectors per unit */
	/*
	 * Spares (bad sector replacements) below
	 * are not counted in d_nsectors or d_secpercyl.
	 * Spare sectors are assumed to be physical sectors
	 * which occupy space at the end of each track and/or cylinder.
	 */
	u_short	d_sparespertrack;	/* # of spare sectors per track */
	u_short	d_sparespercyl;		/* # of spare sectors per cylinder */
	/*
	 * Alternate cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	u_long	d_acylinders;		/* # of alt. cylinders per unit */

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
	u_short	d_rpm;			/* rotational speed */
	u_short	d_interleave;		/* hardware sector interleave */
	u_short	d_trackskew;		/* sector 0 skew, per track */
	u_short	d_cylskew;		/* sector 0 skew, per cylinder */
	u_long	d_headswitch;		/* head switch time, usec */
	u_long	d_trkseek;		/* track-to-track seek, usec */
	u_long	d_flags;		/* generic flags */
#define NDDATA 5
	u_long	d_drivedata[NDDATA];	/* drive-type specific information */
#define NSPARE 5
	u_long	d_spare[NSPARE];	/* reserved for future use */
	u_long	d_magic2;		/* the magic number (again) */
	u_short	d_checksum;		/* xor of data incl. partitions */

			/* filesystem and partition information: */
	u_short	d_npartitions;		/* number of partitions in following */
	u_long	d_bbsize;		/* size of boot area at sn0, bytes */
	u_long	d_sbsize;		/* max size of fs superblock, bytes */
	struct	partition {		/* the partition table */
		u_long	p_size;		/* number of sectors in partition */
		u_long	p_offset;	/* starting sector */
		u_long	p_fsize;	/* filesystem basic fragment size */
		u_char	p_fstype;	/* filesystem type, see below */
		u_char	p_frag;		/* filesystem fragments per block */
		u_short	p_cpg;		/* filesystem cylinders per group */
	} d_partitions[MAXPARTITIONS];	/* actually may be more */
};

/* d_type values: */
#define	DTYPE_SMD		1		/* SMD, XSMD; VAX hp/up */
#define	DTYPE_MSCP		2		/* MSCP */
#define	DTYPE_DEC		3		/* other DEC (rk, rl) */
#define	DTYPE_SCSI		4		/* SCSI */
#define	DTYPE_ESDI		5		/* ESDI interface */
#define	DTYPE_ST506		6		/* ST506 etc. */
#define	DTYPE_FLOPPY		10		/* floppy */

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
	daddr_t	df_startblk;
	int	df_reg[8];		/* result */
};

/*
 * Structure used internally to retrieve
 * information about a partition on a disk.
 */
struct partinfo {
	struct disklabel *disklab;
	struct partition *part;
};

/*
 * Disk-specific ioctls.
 */
		/* get and set disklabel; DIOCGPART used internally */
#if  defined(__STDC__) || defined(MACH_KERNEL)
#define DIOCGDINFO	_IOR('d', 101, struct disklabel)/* get */
#define DIOCSDINFO	_IOW('d', 102, struct disklabel)/* set */
#define DIOCWDINFO	_IOW('d', 103, struct disklabel)/* set, update disk */
#define DIOCGPART	_IOW('d', 104, struct partinfo)	/* get partition */
#else
#define DIOCGDINFO	_IOR(d, 101, struct disklabel)/* get */
#define DIOCSDINFO	_IOW(d, 102, struct disklabel)/* set */
#define DIOCWDINFO	_IOW(d, 103, struct disklabel)/* set, update disk */
#define DIOCGPART	_IOW(d, 104, struct partinfo)	/* get partition */
#endif

/* do format operation, read or write */
#if	defined(__STDC__) || defined(MACH_KERNEL)
#define DIOCRFORMAT	_IOWR('d', 105, struct format_op)
#define DIOCWFORMAT	_IOWR('d', 106, struct format_op)

#define DIOCSSTEP	_IOW('d', 107, int)	/* set step rate */
#define DIOCSRETRIES	_IOW('d', 108, int)	/* set # of retries */
#define DIOCWLABEL	_IOW('d', 109, int)	/* write en/disable label */

#define DIOCSBAD	_IOW('d', 110, struct dkbad)	/* set kernel dkbad */
#else
#define DIOCRFORMAT	_IOWR(d, 105, struct format_op)
#define DIOCWFORMAT	_IOWR(d, 106, struct format_op)

#define DIOCSSTEP	_IOW(d, 107, int)	/* set step rate */
#define DIOCSRETRIES	_IOW(d, 108, int)	/* set # of retries */
#define DIOCWLABEL	_IOW(d, 109, int)	/* write en/disable label */

#define DIOCSBAD	_IOW(d, 110, struct dkbad)	/* set kernel dkbad */
#endif

#endif LOCORE



#define DEV_BSIZE  512
#define DEV_BSHIFT 9


#define TOSTOP 		0x00400000
#define LTSTOP 		(TOSTOP>>16)

#define FNDELAY 0x4
#define TS_TWOSB	0x400000    /* Two Stop Bits */


#define CSTOP  ('s' & 0x37) 
#define CSTART ('q' & 0x37) 
#define DECCTQ 0x40000000

#define EFAULT 14
#endif /* file wrapper */
