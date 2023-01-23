/* 
 * Mach Operating System
 * Copyright (c) 1993,1991,1990,1989 Carnegie Mellon University
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
 * $Log:	fdreg.h,v $
 * Revision 2.6  93/08/02  21:44:01  mrt
 * 	Put the free copyright on this file, again.
 * 	[93/07/22            mrt]
 * 
 * Revision 2.5  93/01/24  13:15:42  danner
 * 	Make unit_info.addr a u_short to make inb and friends happy.
 * 	[93/01/18            rvb]
 * 
 * Revision 2.4  91/10/07  17:25:31  af
 * 	More mg32 changes
 * 	[91/10/07            rvb]
 * 	New chips/busses.[ch] nomenclature.
 * 	[91/09/09  17:11:55  rvb]
 * 
 * 	Synched again with 2.5.
 * 	[91/09/04            rvb]
 * 
 * Revision 2.3  91/05/14  16:23:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/14  14:42:36  mrt
 * 	New, Improved, compatible with new fd.c
 * 	[91/01/28  15:33:58  rvb]
 * 
 * Revision 1.6.1.5  90/11/27  13:44:40  rvb
 * 	Synched 2.5 & 3.0 at I386q (r1.6.1.5) & XMK35 (r2.4)
 * 	[90/11/15            rvb]
 * 
 * Revision 1.6.1.4  90/09/18  08:38:29  rvb
 * 	Flush setparms from here.
 * 	[90/09/08            rvb]
 * 
 * Revision 2.3  90/08/27  22:01:00  dbg
 * 	Flush ushort.
 * 	[90/07/17            dbg]
 * 
 * Revision 1.6.1.3  90/07/27  11:26:42  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/07/27            rvb]
 * 
 * Revision 2.2  90/05/03  15:45:27  dbg
 * 	Change for pure kernel.
 * 	[90/04/19            dbg]
 * 
 * Revision 1.6.1.2  90/01/08  13:30:22  rvb
 * 	Add Intel copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.6.1.1  89/10/22  11:34:43  rvb
 * 	Received from Intel October 5, 1989.
 * 	[89/10/13            rvb]
 * 
 * Revision 1.6  89/09/25  12:27:00  rvb
 * 	Flush B_VERFIY and B_FORMAT
 * 	[89/09/23            rvb]
 * 
 * Revision 1.5  89/09/20  17:29:17  rvb
 * 	It bothers me that we are changing a constant here so that
 * 	25 Mhz machines will loop for a full 1msec.  There has to be
 * 	a way to set cpuspeed once we find out the kind of machine and
 * 	clock speed.
 * 	[89/09/20            rvb]
 * 
 * Revision 1.4  89/03/09  20:07:10  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:40:12  gm0w
 * 	Changes for cleanup.
 * 
 * 
 */
 
/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*	Copyright (c) 1987, 1988 TOSHIBA Corp.		*/
/*		All Rights Reserved			*/

#ident  "@(#)m765.h	1.13 - 88/02/17"

/*******************************************************************
 *
 *	 Toshiba Floppy Driver for UNIX System V R3
 *
 *					June 21, 1988 
 *
 *	Intended Drive Units:
 *		Worldwide - Model No. ND-356  3.5" unformatted 2MB/1MB
 *		UNIX Media Type Name: 2HD512/2DD512/2D512/1D512.
 *
 *		In Japan Only - Model No. ND-355  3.5" unformatted 1.6MB/1MB
 *		UNIX Media Type Name: 2HC1024/2HC512/2HC256/2DD512/2D512/1D512.
 *
 *		Worldwide - Model No. ND-04DT-A  5.25" unformatted 500 KB
 *		UNIX Media Type Name: 2D512/1D512.
 *
 *		In Japan Only - Model No. ND-08DE  5.25" unformatted 1.6MB/1MB
 *		UNIX Media Type Name: 2HC1024/2HC512/2HC256/2DD512/2D512/1D512.
 *
 *		Use with other devices may require modification.
 *
 *	Notes:
 *		For further detail regarding drive units contact 
 *		Toshiba America,Inc. Disk Products Division,
 *		Irvine, CA (714) 583-3000.
 *
 *******************************************************************/

/*
 *	fdcmd.c_rbmtr
 *
 *      	|--+--+--+--+--+--+--+--|
 *      	|  |  |  |  |  |  |  |  |
 *      	|--+--+--+--+--+--+--+--|
 *		       ^  ^        ^  ^
 *		       |  |        |  |--- unit0 motor on flag
 *		       |  |        |------ unit1 motor on flag
 *		       |  |--------------- unit0 recalibrate flag
 *		       |------------------ unit1 recalibrate flag
 */
#define MTRMASK		0x003		/* mask motor_flag for get status */
#define MTRRST		0x0fc		/* reset motor_flag data */
#define RBSHIFT		0x004		/* shift count for recalibrate data */
#define RBRST		0x0cf		/* reset recalibrate data */

/*
 *	fdcmd.c_intr
 *
 *      		    	  |--+--+--+--+--+--+--+--|
 *      	  	    	  |  |  |  |  |  |  |  |  |
 *	        	    	  |--+--+--+--+--+--+--+--|
 *		 	            ^  ^  ^  ^  ^  ^  ^  ^
 *                      reserved  --+  |  |  |  |  |  |  +--- read/write flag
 *                      reserved  -----+  |  |  |  |  +------ seek flag
 * 	                reserved  --------+  |  |  +------ seek flag for retry
 * recalibrate/seek flag(for open) ----------+  +--------- recalibrate flag
 */
#define RWFLAG		0x001
#define SKFLAG		0x002
#define SKEFLAG		0x004
#define RBFLAG		0x008
#define WUPFLAG		0x010
#define CMDRST		0x000	

/* 
 *	fddrtab.dr_type 
 *
 *        +---+---+---+---+---+---+---+---+
 *        |   |   |   |   |   |   |   |   |
 *        +---+---+---+---+---+---+---+---+
 *          ^   ^   ^   ^   ^
 *          |   |   |   |   |----------- rapid seek flag
 *          |---|   |   |                  0: normal seek
 *            |     |   |                  1: rapid seek
 *            |     |   |--------------- detect format
 *            |     |                      0: no detect
 *            |     |                      1: format type OK
 *            |     |------------------- 40 or 80 cylinder(for 2hc/2dd drive)
 *            |                            0: 80 cylinder
 *            |                            1: 40 cylinder
 *            |------------------------- transfer rate(for read/write/format)
 *                                         00: 500kbps  10: 250kbps
 *                                         01: 300kbps  11: reserved
 */
#define RPSEEK		0x00		/* rapid seek */
#define RAPID		0x08		/* rapid seek flag */
#define OKTYPE		0x10		/* media change flag */
#define DOUBLE		0x20		/* double/single step change */
#define NMSEEK		0x80		/* normal seek */
#define RATEMASK	0xc0		/* transfer parameter mask data */

/*
 *	device number
 *
 *	 15             10  9  8  7                    0
 *	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *	| 0  0  0  0  0  0  0  1|     | 0| 0| 0| 0|    	|
 *	+-----------+-----+-----+--+--+--+--+-----------+
 *	  ^                    ^  ^  ^              ^  ^ 
 *	  |____________________|  |__|              |__|
 *		    |		   |                 |
 *		    |		   |                 |- media type
 *	       major number	   |			0: 3.50"  720 KB
 *				   |- unit number	1: 3.50" 1.44 Meg
 *							2: 5.25"  360 KB
 *							3: 5.25" 1.20 Meg
 */
#define UNIT(dev)	((dev & 0xc0)>>6)	/* get unit number */
#define MEDIATYPE(dev)	(dev & 0x03)		/* get media type */
/*****************************************************************************
 
		wait time / timeout count

 *****************************************************************************/
#define STSCHKCNT	0x2800		/* For check status */
#define ITOUT		HZ*5		/* interrupt timeout count */
#define TOUT		HZ/4		/* media type check timeout count */
#define MTRSTOP		HZ*2		/* motor off time */
#define SEEKWAIT	HZ/100*3	/* head_lock time */

/******************************************************************************
 
		define for FDC

 ******************************************************************************/
/* FDC register */
#define CTRLREG(ADDR)	(ADDR)		/* controle register */
#define STSREG(ADDR)	((ADDR)+2)	/* status register */
#define DATAREG(ADDR)	((ADDR)+3)	/* data register */
#define VFOREG(ADDR)	((ADDR)+5)	/* vfo register */

/* CTRLREG flags */
#define FDC_RST		0x04
#define MTR_ON		0x04
#define DMAREQ		0x08
#define RDY		0x40
#define BSY		0x80

/* status for command_out */
#define FD_OSTS		0x00		/* For output check */
#define FD_ISTS		0x40		/* For input check */
#define DTOCPU		0x40
#define DATAOK		0x80

/* Command for FDC */
#define SPCCMD		0x03		/* Specify command */
#define RBCMD		0x07		/* Recalibrate command */
#define SISCMD		0x08		/* Sense interrupt status command */
#define SEEKCMD		0x0f		/* seek command */
#define RDM		0xe6		/* FDC READ command */
#define RDMV		0x42e6		/* VERIFY READ command */
#define WTM		0xc5		/* FDC WRITE command */
#define FMTM		0x4d		/* FDC FORMAT command */
#define FMTDATA		0x5e		/* format data */

/* check value */
#define OPENBIT		0x80		/* VFO check define */
#define BYTEMASK	0xff

/* FDC error code define */
#define ERROR		0xff
#define EBBHARD		128
#define EBBSOFT		129
#define ST0AT		0x40
#define ST0IC		0x80
#define ST0OK		0xc0
#define ADDRERR		0x01
#define WTPRT		0x02
#define NOREC		0x03
#define OVERRUN		0x04
#define CRCERR		0x05
#define FDCERR		0x06
#define TIMEOUT		0x08
#define DOORERR		0x09

/******************************************************************************
 
		define for DMA

 *****************************************************************************/
/* DMA register */
#define DMACMD1		0x08		/* DMA #1 command register 	*/
#define DMAMSK1		0x0f		/* DMA #1 all mask register	*/
#define DMABPFF		0x0c
#define DMAMODE		0x0b
#define DMAADDR		0x04
#define DMAPAGE		0x81
#define DMACNT		0x05
#define DMAMSK		0x0a

/* dma set data */
#define DMARD		0x46		/* DMA read mode		*/
#define DMAWT		0x4a		/* DMA	write mode		*/
#define DMAVRF		0x42		/* DMA verify mode		*/

#define DMADATA0	0x00		/* DMA #2 all mask data 	*/
#define DMADATA1	0x0b		/* DMA #1 all mask data 	*/
#define CHANNEL2	0x02

#define SRTHUT          0xdf
#define HLTND           0x02
#define DTL             0xff

/******************************************************************************
 
		etc. define

 *****************************************************************************/
#define SPL		spl5	/* Same as in i386at/autoconf.c */
#define MAXUNIT		4	/* Max unit number */
#define BLKSIZE		512	/* block size	   */

/* fdcmd.c_stsflag	*/
#define MTRFLAG	0x01
#define MTROFF	0x02
#define INTROUT	0x04

/* fdcmd.c_devflag (media check flag . etc.) */
#define FDMCHK		0x01
#define FDWAIT		0x02
#define STRCHK		0x04
#define STRWAIT		0x08

/* fdcmd.c_dcount */
#define FDCCNT	9	/* Command table for read/write/format (FDC) */
#define RWCNT	9	/* Read/Write command count */
#define FMTCNT	6	/* format command count */

struct	fdcmd {
	int	c_rbmtr;		/* moter & rcalibrate flag */
	int	c_intr;			/* intr flag */
	int	c_stsflag;		/* moter flag */
	int	c_mtrid;		/* motor off queue id */
	int	c_timeid;		/* interrupt timeout id */
	int	c_devflag;		/* device status   */
	int	c_dcount;		/* Read/Write/Format data count */
	int	c_rwdata[FDCCNT];	/* Read/Write/Format cmd (FDC) */
	int	c_saddr;		/* cmd seek address */
};

/* fdmbuf.b_rberr/fdmbuf.b_seekerr/fdmbuf.b_rwerr */
#define MEDIARD		0x01
#define MEDIASEEK	0x01
#define SRETRY		0x03
#define MRETRY		0x30
#define LRETRY		0x300
#define SRMASK		0x0f
#define MRMASK		0xf0
#define RMRMASK		0xff0
#define LRMASK		0xf00
#define MINC		0x10
#define LINC		0x100

struct	ctrl_info {
	struct	unit_info *b_unitf;	/* first buffer for this dev 	   */
	struct	unit_info *b_uip;	/* like b_unit			   */
	struct	unit_info *b_wup;	/* unit to wake up when WUPFLAG    */
	short	b_rberr;		/* rb error count (for recovery)   */
	short	b_seekerr;		/* seek error count (for recovery) */
	short	b_rwerr;		/* r/w error count (for recovery)  */
	short	b_status;		/* error status			   */
	struct	buf	*b_buf;		/* set bp address 		   */
	caddr_t b_xferaddr;		/* trasfer address		   */
	unsigned int b_xfercount;	/* total transfer count		   */
	unsigned int b_xferdma;		/* dma transfer count		   */
	daddr_t	b_sector;		/* read/write sector 		   */
	struct	fdcmd	b_cmd;		/* set command table address       */
};

#define FMTID	4
struct fmttbl {
	unsigned char cyl;
	unsigned char head;
	unsigned char sector;
	unsigned char s_type;
};

struct	fddrtab {
	u_short	dr_ncyl;	/* cylinder count		 */
	u_short	dr_spc;		/* actual sectors/cylinder	 */
	daddr_t p_nsec;		/* disk length (sector count)	 */
	char	dr_nsec;	/* sector per track		 */
	char	dr_type;	/* media type			 */
	char	dr_rwgpl;	/* Read / Write Gap length	 */
	char	dr_fgpl;	/* Format Gap length		 */
};

struct	unit_info {
	struct unit_info *b_unitf;	/* next slave 		      */
	struct buf 	 *av_forw;	/* head of I/O queue (b_forw) */
	int		 b_seekaddr;	/* cylinder address           */
	u_short		 addr;
	struct fddrtab	 d_drtab;	/* floppy disk parameter      */
	struct bus_device *dev;
	struct fdcmd	 *b_cmd;	/* set command table address  */
	char		 wakeme;	/* set if someone wants to be woken */
};

#define HZ		100		/* 100 ticks/second of the clock */
#define NBPSCTR		512		/* Bytes per LOGICAL disk sector */
					/* These should be added to 
					   "sys/param.h". */
#define PAGESIZ		4096
#ifdef	MACH_KERNEL
#define	PZERO		25
#define	PRIBIO		20

#define	B_VERIFY	IO_SPARE_START
#define	B_FORMAT	(IO_SPARE_START << 1)

#define	b_pfcent	io_mode

#endif	MACH_KERNEL
