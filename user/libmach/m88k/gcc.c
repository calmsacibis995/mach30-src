/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 *
 * HISTORY
 * $Log:	gcc.c,v $
 * Revision 2.3  93/02/01  09:56:23  danner
 * 	Removed memcpy.
 * 	[93/01/20            danner]
 * 
 * Revision 2.2  92/08/03  18:15:15  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 * Revision 2.3  92/04/01  10:56:36  rpd
 * 	Added memcpy
 * 	[92/03/16  17:23:41  danner]
 * 
 * Revision 2.2  92/02/18  18:04:17  elf
 * 	Created.
 * 	[92/01/17            danner]
 * 
 *
 */
/*
 * Support functions for the compiler 
 */

int memcmp(a,b,len)
register char *a, *b;
register unsigned int len;
{
  while (len--)
    {
      char c = *a++;
      char d = *b++;
      if (c < d)
	return -1;
      else
	if (c > d)
	  return 1;
    }
  return 0;
}


