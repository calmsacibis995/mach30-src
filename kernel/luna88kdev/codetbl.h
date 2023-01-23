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
 * $Log:	codetbl.h,v $
 * Revision 2.3  93/03/09  17:58:43  danner
 * 	simplified and cleaned up.
 * 	Flushed VT52 support.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:45:15  jfriedl
 * 	renamed from .t file
 * 	[92/07/24            jfriedl]
 * 
 */

#define   TKEYPAD   0x0001     /* keypad key   */
#define   TBRKKEY   0x0040     /* break key    */
#define   TRSTKEY   0x0080     /* reset key    */

/*                    */
/*  KEY CONTROL FLAG  */
/*                    */

#define   OK        0x8000     /* not invalid  */
#define   NG        0x0000     /* invalid key  */
#define   ESCLINK   0x4000     /* escape seaquence link  */
#define   REPEATB   0x2000     /* repeat ok    */

/*           */
/*  KEY MODE */
/*           */

#define   ASET       2         /* ANSI KEY MODE SET */
#define   ARST       3         /* ANSI KEY MODE RESET */
#define   ANSIKNM    6         /* ANSI Keypad Numeric Mode */
#define   ANSIKAM    7         /* ANSI Keypad Application Mode */

#define CODTBSZ 128            /* code table size        */


/***********************/
/*                     */
/*  CODE HENKAN TABLE  */
/*                     */
/***********************/


/*************/
/*           */
/*  SPECIAL  */
/*           */
/*************/


static short t_specl[CODTBSZ] = 
{
    /* -- 00 -- */
    
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG+ESCLINK,/* TAB   */
    NG+ESCLINK,/* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */
    
    /* -- 10 -- */
    
    NG+0x1b,  /* ESC */
    NG+REPEATB+0x08,  /* BS  */
    NG+0x0d,  /* CR  */
    NG,
    NG+REPEATB+' ',   /* space */
    NG+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    NG+ESCLINK+REPEATB,        /* cursol up    */
    NG+ESCLINK+REPEATB,        /* cursol left  */
    NG+ESCLINK+REPEATB,        /* cursol right */
    NG+ESCLINK+REPEATB,        /* cursol down  */
    
    /* -- 20 -- */
    
    NG,
    NG,
    NG+REPEATB+'1',
    NG+REPEATB+'2',
    NG+REPEATB+'3',
    NG+REPEATB+'4',
    NG+REPEATB+'5',
    NG+REPEATB+'6',
    NG+REPEATB+'7',
    NG+REPEATB+'8',
    NG+REPEATB+'9',
    NG+REPEATB+'0',
    NG+REPEATB+'-',
    NG+REPEATB+'^',
    NG+REPEATB+0x5c,   /* \\ */
    NG,
    
    /* -- 30 -- */ 
    
    NG,
    NG,
    NG+REPEATB+'q',
    NG+REPEATB+'w',
    NG+REPEATB+'e',
    NG+REPEATB+'r',
    NG+REPEATB+'t',
    NG+REPEATB+'y',
    NG+REPEATB+'u',
    NG+REPEATB+'i',
    NG+REPEATB+'o',
    NG+REPEATB+'p',
    NG+REPEATB+'@',
    NG+REPEATB+'[',
    NG,
    NG,
    
    /* -- 40 -- */
    
    NG,
    NG,
    NG+REPEATB+'a',
    NG+REPEATB+'s',
    NG+REPEATB+'d',
    NG+REPEATB+'f',
    NG+REPEATB+'g',
    NG+REPEATB+'h',
    NG+REPEATB+'j',
    NG+REPEATB+'k',
    NG+REPEATB+'l',
    NG+REPEATB+';',
    NG+REPEATB+':',
    NG+REPEATB+']',
    NG,
    NG,
    
    /* -- 50 -- */
    
    NG,
    NG,
    NG+REPEATB+'z',
    NG+REPEATB+'x',
    NG+REPEATB+'c',
    NG+REPEATB+'v',
    NG+REPEATB+'b',
    NG+REPEATB+'n',
    NG+REPEATB+'m',
    NG+REPEATB+',',
    NG+REPEATB+'.',
    NG+REPEATB+'/',
    NG,
    NG,
    NG,
    NG,
    
    /* -- 60 -- */
    
    NG,
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    NG+0x0d,   /* CR       */
    NG,
    
    /* -- 70 -- */
    
    NG,
    NG,
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    NG+ESCLINK+REPEATB+TKEYPAD         /* PF , */
	
    };

/*************/
/*           */
/*  CONTROL  */
/*           */
/*************/

static short t_cntrl[CODTBSZ] = 
{
    /* -- 00 -- */

    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG+ESCLINK,        /* TAB   */
    NG+ESCLINK,        /* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */
    
    /* -- 10 -- */
    
    NG+0x1b,  /* ESC */
    NG+REPEATB+0x08,  /* BS  */
    NG+0x0d,  /* CR  */
    NG,
    OK+REPEATB+0x00,  /* NUL */
    NG+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    NG+ESCLINK+REPEATB,        /* cursol up    */
    NG+ESCLINK+REPEATB,        /* cursol left  */
    NG+ESCLINK+REPEATB,        /* cursol right */
    NG+ESCLINK+REPEATB,        /* cursol down  */
    
    /* -- 20 -- */
    
    NG,
    NG,
    NG+REPEATB+'1',
    NG+REPEATB+'2',
    NG+REPEATB+'3',
    NG+REPEATB+'4',
    NG+REPEATB+'5',
    NG+REPEATB+'6',
    NG+REPEATB+'7',
    NG+REPEATB+'8',
    NG+REPEATB+'9',
    NG+REPEATB+'0',
    NG+REPEATB+'-',
    OK+REPEATB+0x1e,   /* RS */
    OK+REPEATB+0x1c,   /* FS */
    NG,
    
    /* -- 30 -- */ 

    NG,
    NG,
    OK+REPEATB+0x11,   /* DC1 or XON */
    OK+REPEATB+0x17,   /* ETB        */
    OK+REPEATB+0x05,   /* ENQ        */
    OK+REPEATB+0x12,   /* DC2        */
    OK+REPEATB+0x14,   /* DC4        */
    OK+REPEATB+0x19,   /* EM         */
    OK+REPEATB+0x15,   /* NAK        */
    OK+REPEATB+0x09,   /* HT         */
    OK+REPEATB+0x0f,   /* SI         */
    OK+REPEATB+0x10,   /* DLE        */
    NG+REPEATB+'@',
    OK+REPEATB+0x1b,   /* ESC        */
    NG,
    NG,
    
    /* -- 40 -- */
    
    NG,
    NG,
    OK+REPEATB+0x01,   /* SOH         */
    OK+REPEATB+0x13,   /* DC3 or XOFF */
    OK+REPEATB+0x04,   /* EOT         */
    OK+REPEATB+0x06,   /* ACK         */
    OK+REPEATB+0x07,   /* BELL        */
    OK+REPEATB+0x08,   /* BS          */
    OK+REPEATB+0x0a,   /* LF          */
    OK+REPEATB+0x0b,   /* VT          */
    OK+REPEATB+0x0c,   /* FF          */
    NG+REPEATB+';',
    NG+REPEATB+':',
    OK+REPEATB+0x1d,   /* GS          */
    NG,
    NG,

    /* -- 50 -- */

    NG,
    NG,
    OK+REPEATB+0x1a,   /* SUB */
    OK+REPEATB+0x18,   /* CAN */
    OK+REPEATB+0x03,   /* ETX */
    OK+REPEATB+0x16,   /* SYN */
    OK+REPEATB+0x02,   /* STX */
    OK+REPEATB+0x0e,   /* SO  */
    OK+0x0d,   /* CR  */
    NG+REPEATB+',',
    NG+REPEATB+'.',
    OK+REPEATB+0x1f,   /* US  */
    NG,
    NG,
    NG,
    NG,
    
    /* -- 60 -- */
    
    NG,
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    NG+0x0d,   /* CR       */
    NG,
    
    /* -- 70 -- */
    
    NG,
    NG,
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    NG+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    NG+ESCLINK+REPEATB+TKEYPAD         /* PF , */
	      
    };

/**************************/
/*                        */
/*   CAPS LOCK  +  SHIFT  */
/*                        */
/**************************/

static short t_capsf[CODTBSZ] = 
{
    /* -- 00 -- */

    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    OK+REPEATB+0x09,   /* TAB   */
    NG+ESCLINK,        /* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */

    /* -- 10 -- */
    
    OK+0x1b,  /* ESC */
    OK+REPEATB+0x08,  /* BS  */
    OK+0x0d,  /* CR  */
    NG,
    OK+REPEATB+' ',   /* space */
    OK+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    OK+ESCLINK+REPEATB,        /* cursol up    */
    OK+ESCLINK+REPEATB,        /* cursol left  */
    OK+ESCLINK+REPEATB,        /* cursol right */
    OK+ESCLINK+REPEATB,        /* cursol down  */

    /* -- 20 -- */

    NG,
    NG,
    OK+REPEATB+'!', 
    OK+REPEATB+'"',
    OK+REPEATB+'#', 
    OK+REPEATB+'$',
    OK+REPEATB+'%', 
    OK+REPEATB+'&', 
    OK+REPEATB+'\'', 
    OK+REPEATB+'(', 
    OK+REPEATB+')', 
    NG+REPEATB,
    OK+REPEATB+'=',
    OK+REPEATB+0x7e,    /* ~           */
    OK+REPEATB+'|',             
    NG,

    /* -- 30 -- */ 

    NG,
    NG,
    OK+REPEATB+'q',
    OK+REPEATB+'w',
    OK+REPEATB+'e',
    OK+REPEATB+'r',
    OK+REPEATB+'t',
    OK+REPEATB+'y',
    OK+REPEATB+'u',
    OK+REPEATB+'i',
    OK+REPEATB+'o',
    OK+REPEATB+'p',
    OK+REPEATB+'`',
    OK+REPEATB+'{',
    NG,
    NG,

    /* -- 40 -- */

    NG,
    NG,
    OK+REPEATB+'a',
    OK+REPEATB+'s',
    OK+REPEATB+'d',
    OK+REPEATB+'f',
    OK+REPEATB+'g',
    OK+REPEATB+'h',
    OK+REPEATB+'j',
    OK+REPEATB+'k',
    OK+REPEATB+'l',
    OK+REPEATB+'+',
    OK+REPEATB+'*',
    OK+REPEATB+'}',
    NG,
    NG,

    /* -- 50 -- */

    NG,
    NG,
    OK+REPEATB+'z',
    OK+REPEATB+'x',
    OK+REPEATB+'c',
    OK+REPEATB+'v',
    OK+REPEATB+'b',
    OK+REPEATB+'n',
    OK+REPEATB+'m',
    OK+REPEATB+'<',
    OK+REPEATB+'>',
    OK+REPEATB+'?',
    OK+'_',
    NG,
    NG,
    NG,

    /* -- 60 -- */

    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    OK+0x0d,   /* CR       */
    NG,

    /* -- 70 -- */

    NG,
    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    OK+ESCLINK+REPEATB+TKEYPAD         /* PF , */
	
    };

/***************/
/*             */
/*  CAPS LOCK  */
/*             */
/***************/

static short t_cploc[CODTBSZ] = 
{
    /* -- 00 -- */
    
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    OK+REPEATB+0x09,   /* TAB   */
    NG+ESCLINK,        /* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */
    
    /* -- 10 -- */
    
    OK+0x1b,  /* ESC */
    OK+REPEATB+0x08,  /* BS  */
    OK+0x0d,  /* CR  */
    NG,
    OK+REPEATB+' ',   /* space */
    OK+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    OK+ESCLINK+REPEATB,        /* cursol up    */
    OK+ESCLINK+REPEATB,        /* cursol left  */
    OK+ESCLINK+REPEATB,        /* cursol right */
    OK+ESCLINK+REPEATB,        /* cursol down  */

    /* -- 20 -- */

    NG,
    NG,
    OK+REPEATB+'1',
    OK+REPEATB+'2', 
    OK+REPEATB+'3',
    OK+REPEATB+'4', 
    OK+REPEATB+'5',
    OK+REPEATB+'6', 
    OK+REPEATB+'7',
    OK+REPEATB+'8',
    OK+REPEATB+'9',
    OK+REPEATB+'0',
    OK+REPEATB+'-',
    OK+REPEATB+'^',
    OK+REPEATB+0x5c,   /* \\ */
    NG,

    /* -- 30 -- */ 

    NG,
    NG,
    OK+REPEATB+'Q',
    OK+REPEATB+'W',
    OK+REPEATB+'E',
    OK+REPEATB+'R',
    OK+REPEATB+'T',
    OK+REPEATB+'Y',
    OK+REPEATB+'U',
    OK+REPEATB+'I',
    OK+REPEATB+'O',
    OK+REPEATB+'P',
    OK+REPEATB+'@',
    OK+REPEATB+'[',
    NG,
    NG,

    /* -- 40 -- */

    NG,
    NG,
    OK+REPEATB+'A',
    OK+REPEATB+'S',
    OK+REPEATB+'D',
    OK+REPEATB+'F',
    OK+REPEATB+'G',
    OK+REPEATB+'H',
    OK+REPEATB+'J',
    OK+REPEATB+'K',
    OK+REPEATB+'L',
    OK+REPEATB+';',
    OK+REPEATB+':',
    OK+REPEATB+']',
    NG,
    NG,

    /* -- 50 -- */

    NG,
    NG,
    OK+REPEATB+'Z',
    OK+REPEATB+'X',
    OK+REPEATB+'C',
    OK+REPEATB+'V',
    OK+REPEATB+'B',
    OK+REPEATB+'N',
    OK+REPEATB+'M',
    OK+REPEATB+',',
    OK+REPEATB+'.',
    OK+REPEATB+'/',
    NG,
    NG,
    NG,
    NG,

    /* -- 60 -- */

    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    OK+0x0d,   /* CR       */
    NG,

    /* -- 70 -- */

    NG,
    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    OK+ESCLINK+REPEATB+TKEYPAD         /* PF , */

    };

/***********/
/*         */
/*  SHIFT  */
/*         */
/***********/

static short t_shift[CODTBSZ] = 
{
    /* -- 00 -- */

    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    OK+REPEATB+0x09,   /* TAB   */
    NG+ESCLINK,        /* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */
    
    /* -- 10 -- */

    OK+0x1b,  /* ESC */
    OK+REPEATB+0x08,  /* BS  */
    OK+0x0d,  /* CR  */
    NG,
    OK+REPEATB+' ',   /* space */
    OK+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    OK+ESCLINK+REPEATB,        /* cursol up    */
    OK+ESCLINK+REPEATB,        /* cursol left  */
    OK+ESCLINK+REPEATB,        /* cursol right */
    OK+ESCLINK+REPEATB,        /* cursol down  */

    /* -- 20 -- */
    
    NG,
    NG,
    OK+REPEATB+'!', 
    OK+REPEATB+'"',
    OK+REPEATB+'#', 
    OK+REPEATB+'$',
    OK+REPEATB+'%', 
    OK+REPEATB+'&', 
    OK+REPEATB+'\'', 
    OK+REPEATB+'(', 
    OK+REPEATB+')', 
    NG+REPEATB,
    OK+REPEATB+'=',
    OK+REPEATB+0x7e,    /* ~           */
    OK+REPEATB+'|',             
    NG,

    /* -- 30 -- */ 

    NG,
    NG,
    OK+REPEATB+'Q',
    OK+REPEATB+'W',
    OK+REPEATB+'E',
    OK+REPEATB+'R',
    OK+REPEATB+'T',
    OK+REPEATB+'Y',
    OK+REPEATB+'U',
    OK+REPEATB+'I',
    OK+REPEATB+'O',
    OK+REPEATB+'P',
    OK+REPEATB+'`',
    OK+REPEATB+'{',
    NG,
    NG,

    /* -- 40 -- */

    NG,
    NG,
    OK+REPEATB+'A',
    OK+REPEATB+'S',
    OK+REPEATB+'D',
    OK+REPEATB+'F',
    OK+REPEATB+'G',
    OK+REPEATB+'H',
    OK+REPEATB+'J',
    OK+REPEATB+'K',
    OK+REPEATB+'L',
    OK+REPEATB+'+',
    OK+REPEATB+'*',
    OK+REPEATB+'}',
    NG,
    NG,

    /* -- 50 -- */

    NG,
    NG,
    OK+REPEATB+'Z',
    OK+REPEATB+'X',
    OK+REPEATB+'C',
    OK+REPEATB+'V',
    OK+REPEATB+'B',
    OK+REPEATB+'N',
    OK+REPEATB+'M',
    OK+REPEATB+'<',
    OK+REPEATB+'>',
    OK+REPEATB+'?',
    OK+'_',
    NG,
    NG,
    NG,

    /* -- 60 -- */

    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    OK+0x0d,   /* CR       */
    NG,

    /* -- 70 -- */

    NG,
    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    OK+ESCLINK+REPEATB+TKEYPAD         /* PF , */

    };

/************/
/*          */
/*  NORMAL  */
/*          */
/************/

static short t_nomal[CODTBSZ] = 
{
    /* -- 00 -- */
    
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    NG,
    OK+REPEATB+0x09,   /* TAB   */
    NG+ESCLINK,        /* CTRL  */
    NG,        /* kana  */
    NG,        /* shift */
    NG,        /* shift */
    NG,        /* CAP    */
    NG,        /* special */
    
    /* -- 10 -- */
    
    OK+0x1b,  /* ESC */
    OK+REPEATB+0x08,  /* BS  */
    OK+0x0d,  /* CR  */
    NG,
    OK+REPEATB+' ',   /* space */
    OK+REPEATB+0x7f,  /* DEL */
    OK+0xe0,  /* henkan */
    OK+0xe1,  /* kakutei */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF11 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF12 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF13 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF14 */
    OK+ESCLINK+REPEATB,        /* cursol up    */
    OK+ESCLINK+REPEATB,        /* cursol left  */
    OK+ESCLINK+REPEATB,        /* cursol right */
    OK+ESCLINK+REPEATB,        /* cursol down  */
    
    /* -- 20 -- */
    
    NG,
    NG,
    OK+REPEATB+'1',
    OK+REPEATB+'2',
    OK+REPEATB+'3',
    OK+REPEATB+'4',
    OK+REPEATB+'5',
    OK+REPEATB+'6',
    OK+REPEATB+'7',
    OK+REPEATB+'8',
    OK+REPEATB+'9',
    OK+REPEATB+'0',
    OK+REPEATB+'-',
    OK+REPEATB+'^',
    OK+REPEATB+0x5c,   /* \\ */
    NG,
    
    /* -- 30 -- */ 
    
    NG,
    NG,
    OK+REPEATB+'q',
    OK+REPEATB+'w',
    OK+REPEATB+'e',
    OK+REPEATB+'r',
    OK+REPEATB+'t',
    OK+REPEATB+'y',
    OK+REPEATB+'u',
    OK+REPEATB+'i',
    OK+REPEATB+'o',
    OK+REPEATB+'p',
    OK+REPEATB+'@',
    OK+REPEATB+'[',
    NG,
    NG,
    
    /* -- 40 -- */
    
    NG,
    NG,
    OK+REPEATB+'a',
    OK+REPEATB+'s',
    OK+REPEATB+'d',
    OK+REPEATB+'f',
    OK+REPEATB+'g',
    OK+REPEATB+'h',
    OK+REPEATB+'j',
    OK+REPEATB+'k',
    OK+REPEATB+'l',
    OK+REPEATB+';',
    OK+REPEATB+':',
    OK+REPEATB+']',
    NG,
    NG,
    
    /* -- 50 -- */
    
    NG,
    NG,
    OK+REPEATB+'z',
    OK+REPEATB+'x',
    OK+REPEATB+'c',
    OK+REPEATB+'v',
    OK+REPEATB+'b',
    OK+REPEATB+'n',
    OK+REPEATB+'m',
    OK+REPEATB+',',
    OK+REPEATB+'.',
    OK+REPEATB+'/',
    NG,
    NG,
    NG,
    NG,
    
    /* -- 60 -- */
    
    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD + */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD - */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD 0 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* KEYPAD . */
    OK+0x0d,   /* CR       */
    NG,
    
    /* -- 70 -- */
    
    NG,
    NG,
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF1 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF2 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF3 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF4 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF5 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF6 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF7 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF8 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF9 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF10 */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF * */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF / */
    OK+ESCLINK+REPEATB+TKEYPAD,        /* PF = */
    OK+ESCLINK+REPEATB+TKEYPAD         /* PF , */
};
