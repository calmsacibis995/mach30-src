/* 
 * Mach Operating System
 * Copyright (c) 1992, 1991  Carnegie Mellon University
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
 * $Log:	exec.h,v $
 * Revision 2.2  92/08/03  17:20:44  jfriedl
 * 	created from luna88k version  [danner]
 * 	cleaned up ROUND_SEG() [sugahara@omron.co.jp]
 * 
 * Revision 2.4  92/03/05  23:11:35  rpd
 * 	Moved from kernel to bootstrap.
 * 	Removed sun definitions, added luna definitions.
 * 	[92/03/01            rpd]
 * 
 * Revision 2.2  91/08/24  12:09:30  af
 * 	Taken from i386/exec.h.
 * 	[91/08/14            rvb]
 * 
 *
 */
/*
 * exec stucture in an a.out file derived from FSF's
 * a.out.gnu.h file.
 */

#ifndef	_M88K_EXEC_H_
#define	_M88K_EXEC_H_

/*
 * Header prepended to each a.out file.
 */
struct exec
{
  unsigned char a_dynamic:1;
  unsigned char a_toolversion:7;
  unsigned char a_machtype;	/* machine type */
  unsigned short a_magic;	/* magic number */
  unsigned long a_text;		/* length of text, in bytes */
  unsigned long a_data;		/* length of data, in bytes */
  unsigned long a_bss;		/* length of uninitialized data area, in bytes */
  unsigned long a_syms;		/* length of symbol table data in file, in bytes */
  unsigned long a_entry;	/* start address */
  unsigned long a_trsize;	/* length of relocation info for text, in bytes */
  unsigned long a_drsize;	/* length of relocation info for data, in bytes */
};

/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413

#define M_68010		0x0000		/* 68010 image */
#define M_68020		0x0001		/* 68020 image */
#define M_68881		0x0002		/* 68881 image */
#define M_88000		0x0004		/* 88000 image */

#define SEGMENT_ALIGN   0x20000         /* align data seg to this boundry */
					/* for 0413 images */

#define ROUND_SEG(x) (((x)+SEGMENT_ALIGN-1) & ~(SEGMENT_ALIGN-1))

#endif	_M88K_EXEC_H_
