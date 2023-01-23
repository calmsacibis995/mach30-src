I B defunkt
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
 * $Log:	deftbl.h,v $
 * Revision 2.3  93/03/09  17:59:02  danner
 * 	unused
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:45:37  jfriedl
 * 	renamed from .t file
 * 	[92/07/24            jfriedl]
 * 
 */

/*            */
/*  KEY TYPE  */
/*            */

#define	  TNORMAL   0x0000     /* normal key   */
#define   TCURSOL   0x0000     /* cursol key   */
#define   TKEYPAD   0x0001     /* keypad key   */
#define   THYOKYA   0x0000     /* hyoji sentaku key A  */
#define   THYOKYB   0x0000     /* hyoji sentaku key B  */
#define   TSPEKEY   0x0000     /* tokushu key  */
#define   TBRKKEY   0x0040     /* break key    */
#define   TRSTKEY   0x0080     /* reset key    */

/*                    */
/*  KEY CONTROL FLAG  */
/*                    */

#define   OK        0x8000     /* not invalid  */
#define   NG        0x0000     /* invalid key  */
#define   ESCLINK   0x4000     /* escape seaquence link  */
#define   NOTESCL   0x0000     /* not link     */
#define   REPEATB   0x2000     /* repeat ok    */
#define   NOTREPT   0x0000     /* not repeat   */

/*           */
/*  KEY MODE */
/*           */

#define   VT52       1         /* VT52 MODE    */
#define   ASET       2         /* ANSI KEY MODE SET */
#define   ARST       3         /* ANSI KEY MODE RESET */
#define   VT52KNM    4         /* VT52 Keypad Numeric Mode */
#define   VT52KAM    5         /* VT52 Keypad Application Mode */
#define   ANSIKNM    6         /* ANSI Keypad Numeric Mode */
#define   ANSIKAM    7         /* ANSI Keypad Application Mode */

#define BUFMAX  3              /* input code buf size    */

#define ESCSIZE 36              /* escape table data size */
#define ESCTSIZ 256             /* escape table size      */
#define ESCBYTE ESCTSIZ*ESCSIZE /* escape table bytesu    */

#define CODTBSZ 128            /* code table size        */
#define MODTBSZ 16             /* mode table size        */

#define REPCNT  30             /* repeat delay 16.6*30 ms */
#define RPINTVL 3              /* repeat interval timer 16.6*3 ms */

#define ENDCODE 0              /* escape table end code  */

#define ON      1              /* flag on                */
#define OFF     0              /* flag off               */

#endif
