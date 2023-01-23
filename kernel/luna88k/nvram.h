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
 * $Log:	nvram.h,v $
 * Revision 2.3  93/01/26  18:03:32  danner
 * 	Added #ifndef file wrapper.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:18:51  danner
 * 	Initial Checkin
 * 	[91/06/26  12:29:22  danner]
 * 
 * Revision 2.2  91/04/05  14:04:01  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */
#ifndef __LUNA88K_NVRAM_H__
#define __LUNA88K_NVRAM_H__

/*
 * Non Volatile RAM
 */
#define	NVRAMSZ		2040	/* Size of NVRAM. (Total 2040 bytes) */
#define NVSYMSZ		16
#define NVVALSZ		16
#define NVSYSSZ		42
#define NVUSRSZ		512

/* 88/11/17 KINYA */
#define	IOCNVGET	_IO(N, 1)
#define	IOCNVSET	_IO(N, 2)
#define	IOCNVENGET	_IO(N, 3)

/* rtc register table */

struct rtc {
    unsigned char r_cntl;          /* control registert */
    unsigned char pad0[3];
    unsigned char r_s;             /* second     */
    unsigned char pad1[3];
    unsigned char r_m;             /* minute     */
    unsigned char pad2[3];
    unsigned char r_h;             /* hour       */
    unsigned char pad3[3];
    unsigned char r_dwk;           /* day of the week   */
    unsigned char pad4[3];
    unsigned char r_dmnth;         /* day of the month  */
    unsigned char pad5[3];
    unsigned char r_mnth;          /* month no keta     */  
    unsigned char pad6[3];
    unsigned char r_y;             /* year       */
    unsigned char pad7[3];
};

/*
 * Battery back-up memory space.
 */
struct nvram {
    char nv_testwrite[4];	/* for battery check */
    char nv_hdr[4];		/* header name */
    long nv_machtype;		/* machine-type ID */
    long nv_machno;		/* machine number */
    char nv_calclock[12];	/* RTC initialize */
    char nv_checksum[4];		/* check sum for "nv_system" */
    struct nv_system {
	    char nv_symbol[NVSYMSZ];
	    char nv_value[NVVALSZ];
    } nv_system[NVSYSSZ];	/* system define */
    char nv_reserve[152];	/* reserved */
    char nv_user[NVUSRSZ];	/* user avail area */
};

struct nvramio {
    char		en_adr[12];
    unsigned short	machtype;
    unsigned int	machno;
};

#endif /* __LUNA88K_NVRAM_H__ */
