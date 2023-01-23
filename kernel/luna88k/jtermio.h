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
 * $Log:	jtermio.h,v $
 * Revision 2.4  93/01/26  18:03:08  danner
 * 	Added #ifndef wrapper.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.3  92/05/21  17:18:47  jfriedl
 * 	Cleaned up to quiet gcc warnings.
 * 	[92/05/21  16:03:45  jfriedl]
 * 
 * Revision 2.2  91/07/09  23:17:25  danner
 * 	Initial 3.0 Checkin
 * 	[91/06/26  11:59:47  danner]
 * 
 * Revision 2.2  91/04/05  14:02:35  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */
#ifndef __LUNA88K_JTERMIO_H__
#define __LUNA88K_JTERMIO_H__

/* @(#)jterm.h	*/
/* Kanji output control structure */
#ifndef	EMAX
#define	EMAX	6			/* kanji esc seq's max length */
#endif

struct jterm {
	short	j_level;		/* level of kanji function */
	short	j_flg;			/* kanji terminal flag */
	short	j_ecode;		/* external kanji code : */
					/* JIS = 0, SJIS = 1, UJIS = 2 */
	char	j_jst[EMAX];		/* kanji output start esc seq */
	short	j_jstl;			/* length of k_o_s seq */
	char	j_jend[EMAX];		/* kanji output end esq seq */
	short	j_jendl;		/* length of k_o_e seq */
	char	j_gcsa[2];		/* gaiji code start address */
	char	j_gdsp[EMAX];		/* gaiji output start esq seq */
	short	j_gdspl;		/* length of g_o_s seq */
/* following members used by driver */
	unsigned int j_firstcode;	/* kanji first code. added 88/8/24 */
	unsigned char kescinput[EMAX];
	unsigned char localbuf[2];
	unsigned char outputbuf[EMAX+2];
	short   kesccount;
	short   firstbyte;
	short   outputp;
	char    eschit;
	char    inputconv;
	char    kesctime;		/* timeout for JIS esc 89/12/6 */
	int     inputaddr[2];		/* timeout for JIS esc 89/12/6 */
	char    kana;			/* inputconvert kana JIS esc 89/12/6 */
};

/* For Kanji Output */
#define	ASCII	0		/* must send kout */
#define	KINPROC	0x01		/* kanji output in progress */
#define	GCPROC	0x02		/* gaiji output in progress */
#define	TBUFNE	0x04		/* tbuf not empty */
#define	JINTR	0x08		/* interrupt flag */

/* Kanji Codes */
#define	JIS1S	0x21		/* JIS code 1st byte start */
#define	JIS1E	0x7e		/* JIS code 1st byte end */
#define	JIS2S	JIS1S		/* JIS code 2nd byte start */
#define	JIS2E	JIS1E		/* JIS code 2nd byte end */
#define JISG1S	0x75		/* JIS gaiji code 1st byte start */
#define JISG1E	JIS1E		/* JIS gaiji code 1st byte end */
#define JISG2S	JIS2S		/* JIS gaiji code 2nd byte start */
#define JISG2E	JIS2E		/* JIS gaiji code 2nd byte end */

#define	SJISH1S	0x81		/* SJIS code (high) 1st byte start */
#define	SJISH1E	0x9f		/* SJIS code (high) 1st byte start */
#define	SJISL1S	0xe0		/* SJIS code (low) 1st byte end */
#define	SJISL1E	0xfc		/* SJIS code (low) 1st byte end */
#define	SJIS2S	0x40		/* SJIS code 2nd byte start */
#define	SJIS2E	0xfc		/* SJIS code 2nd byte end */
#define SJISG1S	0xeb		/* SJIS gaiji code 1st byte start */
#define SJISG1E	SJISL1E		/* SJIS gaiji code 1st byte end */
#define SJISG2S	SJIS2S		/* SJIS gaiji code 2nd byte start */
#define SJISG2E	SJIS2E		/* SJIS gaiji code 2nd byte end */

#define	_1_S	0xa1		/* Internal kanji code 1st byte start */
#define	_1_E	0xfe		/* Internal kanji code 1st byte end */
#define	_2_S	0xa1		/* Internal kanji code 2nd byte start */
#define	_2_E	0xfe		/* Internal kanji code 2nd byte end */
#define	G_1_S	0xf5		/* Gaiji code 1st byte start */
#define	G_1_E	0xfe		/* Gaiji code 1st byte end */
#define	G_2_S	0xa1		/* Gaiji code 2nd byte start */
#define	G_2_E	0xfe		/* Gaiji code 2nd byte end */
#define	H_K_1	0x8e		/* Han-kaku kana code 1st byte code */
#define	H_K_S	0xa0		/* Han-kaku kana code start byte */
#define	H_K_E	0xdf		/* Han-kaku kana code end byte */

#define	G_1_L	(G_1_E - G_1_S + 1)
#define	G_2_L	(G_2_E - G_2_S + 1)

#define	SJISGAS	3384		/* SJIS gaiji address table size */

/***** from termuio.h *********/

/* for Kanji output */
#ifndef	EMAX			/* the same simbol is defined in "lprio.h" */
#define	EMAX	6		/* kanji esc seq's max length */
#endif

struct jtermio {
	short	j_level;		/* level of kanji function */
	short	j_flg;			/* kanji terminal flag */
	short	j_ecode;		/* external kanji code : */
					/* JIS = 0, SJIS = 1, UJIS = 2 */
	char	j_jst[EMAX];		/* kanji output start esc seq */
	short	j_jstl;			/* length of k_o_s seq */
	char	j_jend[EMAX];		/* kanji output end esq seq */
	short	j_jendl;		/* length of k_o_e seq */
	char	j_gcsa[2];		/* gaiji code start address */
	char	j_gdsp[EMAX];		/* gaiji output start esq seq */
	short	j_gdspl;		/* length of g_o_s seq */
	unsigned int j_firstcode;	/* kanji first code. added 88/8/24 */
};

#ifndef	JIS		/* the same simbol is defined in "lprio.h" */
#define	JIS	0
#define	SJIS	1
#define	UJIS	2
#endif

#define	CONVTOEXT	0x01		/* convert to the external code */
#define	KANJIINPUT	0x02		/* input conversion enable */
#define	CONVTOINT	0x04		/* convert to the internal code */
#define	RAWKANJIIN	0x08		/* input conversion enable in raw mode */
#define	WNN_EXIST	0x80		/* wnn exist now */

#ifndef	JIOC		/* the same simbol is defined in "lprio.h" */
# define JIOC ('J'<<8)
#endif
#define	JTERMGET	_IOR(J, 1, struct jtermio)
#define	JTERMSET	_IOW(J, 2, struct jtermio)

#endif /* __LUNA88K_JTERMIO_H__ */
