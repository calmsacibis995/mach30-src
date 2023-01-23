/* 
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
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
 * $Log:	lcd.h,v $
 * Revision 2.4  93/03/09  17:57:33  danner
 * 	Added LCD_GOTO.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:04:13  danner
 * 	Renamed #ifndef wrapper.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:42:04  jfriedl
 * 	created.
 * 	[92/07/24            jfriedl]
 * 
 */

/*
 * LCD driver
 */
#ifndef __LUNA88K_LCD_H__
#define __LUNA88K_LCD_H__

/*
 * When writing to the LCD device, LCD_CTRL indicates that the
 * next byte is a command character code.
 */
#define LCD_CTRL  0x01

/*
 * The following are LCD device command codes.
 */
#define LCD_CLS_CMD     0x01    /* clear LCD display */
#define LCD_HOME_CMD    0x02    /* cursor to upper-left of display */
#define LCD_ENTRY_CMD   0x06
#define LCD_ON_CMD      0x0c	/* Turn display on */
#define LCD_INIT_CMD    0x38
/*
 * Y is either 0 or 1, X is in the range of 0..15.
 * LCD_GOTO compiles into a single byte that should be sent after LCD_CTRL. 
 * LCD_GOTO(0,0) is the same as LCD_HOME_CMD.
 * LCD_GOTO(1,0) goes to the start of the 2nd line.
 */
#define LCD_GOTO(Y, X)   (((Y)&1 ? 0xc0 : 0x80) | (X)&0x0f)

#endif /* __LUNA88K_LCD_H__ */
