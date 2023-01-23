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
 * $Log:	setroot.c,v $
 * Revision 2.9  93/02/09  15:44:18  mrt
 * 	Removed unused get_root_device
 * 
 * Revision 2.8  92/08/03  17:44:47  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.7  92/05/21  17:21:02  jfriedl
 * 	Added void decl.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.6  92/04/01  11:00:25  rpd
 * 	Switched back to sd6c.
 * 	[92/03/16  17:21:07  danner]
 * 
 * Revision 2.5  92/03/03  15:38:38  rpd
 * 	sd6c -> sd6a
 * 	[92/03/02            danner]
 * 
 * Revision 2.4  91/08/24  12:15:51  af
 * 	Updated to coincide slightly more with reality.
 * 	[91/07/13            danner]
 * 
 * Revision 2.3.3.1  91/08/19  13:47:42  danner
 * 	Updated to coincide slightly more with reality.
 * 	[91/07/13            danner]
 * 
 * Revision 2.3  91/07/11  11:01:00  danner
 * 	Copyright Fixes
 * 
 * Revision 2.2  91/07/09  23:19:39  danner
 * 	Lifted from i386at directory.
 * 	[91/05/09            danner]
 * 
 */


/*
 * Get root device.  Temporarily hard-coded.
 */

char *root_name = "sd6c";
