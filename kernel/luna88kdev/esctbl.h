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
 * $Log:	esctbl.h,v $
 * Revision 2.3  93/03/09  17:59:29  danner
 * 	unused
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:45:52  jfriedl
 * 	renamed from .t file
 * 	[92/07/24            jfriedl]
 * 
 */

/*              */
/*  MODE TABLE  */
/*              */

static short t_mode[MODTBSZ] = 
{
    ARST,      /* cursor key   */
    ANSIKNM,   /* keypad key   */
};

/*                */
/*  ESCAPE TABLE  */
/*                */

#define MAX_EXPANSION_LEN 4
#define ESC 0x1b

static struct {
    unsigned char keycode;	/* raw keyboard code */
    unsigned char mode;
    unsigned char expansion[MAX_EXPANSION_LEN];
} encoded_expansions[] = {
    /* F1  */	{ 0x72, ANSIKNM, { ESC, 'O', 'P' }},  /* ESCOP */
    /* F2  */	{ 0x73, ANSIKNM, { ESC, 'O', 'Q' }},  /* ESCOQ */
    /* F3  */	{ 0x74, ANSIKNM, { ESC, 'O', 'R' }},  /* ESCOR */
    /* F4  */	{ 0x75, ANSIKNM, { ESC, 'O', 'S' }},  /* ESCOS */
    /* F5  */	{ 0x76, ANSIKNM, { ESC, 'O', 'T' }},  /* ESCOT */
    /* F6  */ 	{ 0x77, ANSIKNM, { ESC, 'O', 'U' }},  /* ESCOU */
    /* F7  */	{ 0x78, ANSIKNM, { ESC, 'O', 'V' }},  /* ESCOV */
    /* F8  */	{ 0x79, ANSIKNM, { ESC, 'O', 'W' }},  /* ESCOW */
    /* F9  */	{ 0x7a, ANSIKNM, { ESC, 'O', 'X' }},  /* ESCOX */
    /* F10 */	{ 0x7b, ANSIKNM, { ESC, 'O', 'Y' }},  /* ESCOY */
    /* F11 */	{ 0x18, ANSIKNM, { ESC, 'O', 'a' }},  /* ESCOa */
    /* F12 */	{ 0x19, ANSIKNM, { ESC, 'O', 'b' }},  /* ESCOb */
    /* F13 */	{ 0x1a, ANSIKNM, { ESC, 'O', 'c' }},  /* ESCOc */
    /* F14 */	{ ESC,  ANSIKNM, { ESC, 'O', 'd' }},  /* ESCOd */
    /* up */	{ 0x1c, ARST,	 { ESC, '[', 'A' }},  /* ESC[A */
    /* left */	{ 0x1d, ARST,	 { ESC, '[', 'D' }},  /* ESC[D */
    /* right */	{ 0x1e, ARST,	 { ESC, '[', 'C' }},  /* ESC[C */
    /* down */	{ 0x1f, ARST,	 { ESC, '[', 'B' }},  /* ESC[B */
    /* + key */	{ 0x61, ANSIKNM, '+' }, 
    /* - key */	{ 0x62, ANSIKNM, '-' }, 
    /* 7 key */ { 0x63, ANSIKNM, '7' }, 
    /* 8 key */ { 0x64, ANSIKNM, '8' }, 
    /* 9 key */ { 0x65, ANSIKNM, '9' }, 
    /* 4 key */ { 0x66, ANSIKNM, '4' }, 
    /* 5 key */ { 0x67, ANSIKNM, '5' }, 
    /* 6 key */ { 0x68, ANSIKNM, '6' }, 
    /* 1 key */ { 0x69, ANSIKNM, '1' }, 
    /* 2 key */ { 0x6a, ANSIKNM, '2' }, 
    /* 3 key */ { 0x6b, ANSIKNM, '3' }, 
    /* 0 key */ { 0x6c, ANSIKNM, '0' }, 
    /* . key */ { 0x6d, ANSIKNM, '.' }, 
    /* * key */	{ 0x7c, ANSIKNM, '*' }, 
    /* / key */	{ 0x7d, ANSIKNM, '/' }, 
    /* = key */	{ 0x7e, ANSIKNM, '=' }, 
    /* , key */	{ 0x7f, ANSIKNM, ',' }, 
};

#define EXPANSIONS (sizeof(encoded_expansions)/sizeof(encoded_expansions[0]))
#endif
