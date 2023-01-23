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
 * $Log:	kdsoft.h,v $
 * Revision 2.5  91/05/14  16:28:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:19:42  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:46:13  mrt]
 * 
 * Revision 2.3  90/11/26  14:50:41  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.4.1.4) & XMK35 (r2.3)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.2  90/05/03  15:45:20  dbg
 * 	First checkin.
 * 
 * Revision 1.4.1.3  90/02/28  15:50:45  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 1.4.1.2  90/01/08  13:30:35  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.4.1.1  89/10/22  11:34:39  rvb
 * 	Revision 1.5  89/09/27  16:02:59  kupfer
 * 	Add support for resetting display before reboot.
 * 
 * Revision 1.4  89/07/17  10:42:18  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.1.1.1  89/04/27  12:36:07  kupfer
 * X79 from CMU.
 * 
 * Revision 1.3  89/02/26  12:42:58  gm0w
 * 	Changes for cleanup.
 * 
 */
 
/* **********************************************************************
 File:         kdsoft.h
 Description:  Software structures for keyboard/display driver, shared with
	drivers for specific graphics cards.

 $ Header: $

 Copyright Ing. C. Olivetti & C. S.p.A. 1988, 1989.
 All rights reserved.
********************************************************************** */

/*
  Copyright 1988, 1989 by Olivetti Advanced Technology Center, Inc.,
Cupertino, California.

		All Rights Reserved

  Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Olivetti
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

  OLIVETTI DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL OLIVETTI BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
 * This driver handles two types of graphics cards.  The first type
 * (e.g., EGA, CGA), treats the screen as a page of characters and
 * has a hardware cursor.  The second type (e.g., the Blit) treats the
 * screen as a bitmap.  A hardware cursor may be present, but it is
 * ignored in favor of a software cursor.
 *
 *
 * Most of the driver uses the following abstraction for the display:
 *
 * The cursor position is simply an index into a (logical) linear char
 * array that wraps around at the end of each line.  Each character
 * takes up ONE_SPACE bytes.  Values in [0..ONE_PAGE) are positions in
 * the displayed page.  Values < 0 and >= ONE_PAGE are off the page
 * and require some scrolling to put the cursor back on the page.
 *
 * The kd_dxxx routines handle the conversion from this abstraction to
 * what the hardware requires.
 *
 * (*kd_dput)(pos, ch, chattr)
 *	csrpos_t pos;
 *	char ch, chattr;
 *  Displays a character at "pos", where "ch" = the character to
 *  be displayed and "chattr" is its attribute byte.
 *
 * (*kd_dmvup)(from, to, count)
 *	csrpos_t from, to;
 *	int count;
 *  Does a (relatively) fast block transfer of characters upward.
 *  "count" is the number of character positions (not bytes) to move.
 *  "from" is the character position to start moving from (at the start
 *  of the block to be moved).  "to" is the character position to start
 *  moving to.
 *
 * (*kd_dmvdown)(from, to, count)
 *	csrpos_t from, to;
 *	int count;
 *  "count" is the number of character positions (not bytes) to move.
 *  "from" is the character position to start moving from (at the end
 *  of the block to be moved).  "to" is the character position to
 *  start moving to.
 *
 * (*kd_dclear)(to, count, chattr)
 *	csrpos_t, to;
 *	int count;
 *	char chattr;
 *  Erases "count" character positions, starting with "to".
 *
 * (*kd_dsetcursor)(pos)
 *  Sets kd_curpos and moves the displayed cursor to track it.  "pos"
 *  should be in the range [0..ONE_PAGE).
 *  
 * (*kd_dreset)()
 *  In some cases, the boot program expects the display to be in a
 *  particular state, and doing a soft reset (i.e.,
 *  software-controlled reboot) doesn't put it into that state.  For
 *  these cases, the machine-specific driver should provide a "reset"
 *  procedure, which will be called just before the kd code causes the
 *  system to reboot.
 */

extern void bmpput(), bmpmvup(), bmpmvdown(), bmpclear(), bmpsetcursor();

extern void	(*kd_dput)();		/* put attributed char */
extern void	(*kd_dmvup)();		/* block move up */
extern void	(*kd_dmvdown)();	/* block move down */
extern void	(*kd_dclear)();		/* block clear */
extern void	(*kd_dsetcursor)();
				/* set cursor position on displayed page */
extern void	(*kd_dreset)();		/* prepare for reboot */


/*
 * Globals used for both character-based controllers and bitmap-based
 * controllers.
 */
typedef	short	csrpos_t;	/* cursor position, ONE_SPACE bytes per char */

extern u_char 	*vid_start;	/* VM start of video RAM or frame buffer */
extern csrpos_t kd_curpos;		/* should be set only by kd_setpos */
extern short	kd_lines;		/* num lines in tty display */
extern short	kd_cols;
extern char	kd_attr;		/* current character attribute */


/*
 * Globals used only for bitmap-based controllers.
 * XXX - probably needs reworking for color.
 */

/*
 * The following font layout is assumed:
 *
 *  The top scan line of all the characters comes first.  Then the
 *  second scan line, then the third, etc.
 *
 *     ------ ... ---------|-----N--------|-------------- ... -----------
 *     ------ ... ---------|-----N--------|-------------- ... -----------
 *		.
 *		.
 *		.
 *     ------ ... ---------|-----N--------|-------------- ... -----------
 *
 * In the picture, each line is a scan line from the font.  Each scan
 * line is stored in memory immediately after the previous one.  The
 * bits between the vertical lines are the bits for a single character
 * (e.g., the letter "N").
 * There are "char_height" scan lines.  Each character is "char_width"
 * bits wide.  We make the simplifying assumption that characters are
 * on byte boundaries.  (We also assume that a byte is 8 bits.)
 */

extern u_char	*font_start;		/* starting addr of font */

extern short	fb_width;		/* bits in frame buffer scan line */
extern short	fb_height;		/* scan lines in frame buffer*/
extern short	char_width;		/* bit width of 1 char */
extern short	char_height;		/* bit height of 1 char */
extern short	chars_in_font;
extern short	cursor_height;		/* bit height of cursor */
			/* char_height + cursor_height = line_height */

extern u_char	char_black;		/* 8 black (off) bits */
extern u_char	char_white;		/* 8 white (on) bits */


/*
 * The tty emulation does not usually require the entire frame buffer.
 * (xstart, ystart) is the bit address for the upper left corner of the 
 * tty "screen".
 */

extern short	xstart, ystart;


/*
 * Accelerators for bitmap displays.
 */

extern short	char_byte_width;	/* char_width/8 */
extern short	fb_byte_width;		/* fb_width/8 */
extern short	font_byte_width;	/* num bytes in 1 scan line of font */
