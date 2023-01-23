I B D-funkt
#if 0
/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	keymap.h,v $
 * Revision 2.3  93/03/09  17:59:46  danner
 * 	unused
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:46:09  jfriedl
 * 	renamed from .t file
 * 	[92/07/24            jfriedl]
 * 
 */

/******************/
/*                */
/*  SEMI-ENCODED  */
/*                */
/******************/


#if 0
#define NOMAL  (char)0x00
#define CNTRL  (char)0x0a
#define KANA (char)0x0b
#define SHIFTL (char)0x0c
#define SHIFTR (char)0x0d
#define CAPS (char)0x0e
#define SPECL (char)0x0f

#define DELKEY	0x15
#define BSKEY	0x11

static char t_keymap[CODTBSZ] = {
    /* -- 00 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,CNTRL,KANA,SHIFTL,SHIFTR,CAPS,SPECL,

    /* -- 10 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,

    /* -- 20 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,

    /* -- 30 -- */ 
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,

    /* -- 40 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,

    /* -- 50 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,

    /* -- 60 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    /* -- 70 -- */
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
    NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,NOMAL,
};

#endif

#endif/*filewrapper */
