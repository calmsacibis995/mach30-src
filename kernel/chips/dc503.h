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
 * $Log:	dc503.h,v $
 * Revision 2.7  91/08/24  11:51:41  af
 * 	Moved padding issues down in impl file.
 * 	Struct pcc hack is gone.
 * 	[91/08/02  02:28:28  af]
 * 
 * Revision 2.6  91/06/19  11:47:24  rvb
 * 	File moved here from mips/PMAX since it tries to be generic;
 * 	it is used on the PMAX and the Vax3100.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.5  91/05/14  17:20:42  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:40:19  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:44  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:31  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:44  af
 * 	Fixed padding, works now.
 * 	[90/12/03  23:13:30  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:26  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	[90/09/03            af]
 */
/*
 *	File: dc503.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Defines for the DEC DC503 Programmable Cursor
 */

typedef struct {
	volatile unsigned short	pcc_cmdr;	/* all regs are wo */
	volatile unsigned short	pcc_xpos;
	volatile unsigned short	pcc_ypos;
	volatile unsigned short	pcc_xmin1;
	volatile unsigned short	pcc_xmax1;
	volatile unsigned short	pcc_ymin1;
	volatile unsigned short	pcc_ymax1;
	volatile unsigned short	pcc_xmin2;
	volatile unsigned short	pcc_xmax2;
	volatile unsigned short	pcc_ymin2;
	volatile unsigned short	pcc_ymax2;
	volatile unsigned short	pcc_memory;
} dc503_regmap_t;

/*
 *	Command register bits
 */

#define	DC503_CMD_TEST		0x8000	/* cursor test flip-flop */
#define	DC503_CMD_HSHI		0x4000	/* Hor sync polarity */
#define	DC503_CMD_VBHI		0x2000	/* Ver blank polarity */
#define	DC503_CMD_LODSA		0x1000	/* load sprite array */
#define	DC503_CMD_FORG2		0x0800	/* force detector2 to one */
#define	DC503_CMD_ENRG2		0x0400	/* enable detector2 */
#define	DC503_CMD_FORG1		0x0200	/* force detector1 to one */
#define	DC503_CMD_ENRG1		0x0100	/* enable detector1 */
#define	DC503_CMD_XHWID		0x0080	/* hair cursor (double) width */
#define	DC503_CMD_XHCL1		0x0040	/* hair clip region */
#define	DC503_CMD_XHCLP		0x0020	/* clip hair inside region */
#define	DC503_CMD_XHAIR		0x0010	/* enable hair cursor */
#define	DC503_CMD_FOPB		0x0008	/* force curs plane B to one */
#define	DC503_CMD_ENPB		0x0004	/* enable curs plane B */
#define	DC503_CMD_FOPA		0x0002	/* force curs plane A to one */
#define	DC503_CMD_ENPA		0x0001	/* enable curs plane A */

