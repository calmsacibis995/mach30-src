/* 
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 * $Log:	endian.c,v $
 * Revision 2.4  93/01/26  18:01:41  danner
 * 	ANSIfied.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.3  92/05/04  11:27:52  danner
 * 	Add ntohl, ntohs, htons to support norma ether.
 * 	[92/05/03            danner]
 * 
 * Revision 2.2  92/02/18  18:03:20  elf
 * 	Created.
 * 	[92/01/20            danner]
 * 
 */

/*
 * Random things that depend on endianness
 * Normally, we run big endian.
 *
 * May be defined in machine/machparam.h for speed.
 */
unsigned long htonl(unsigned long l)
{
  return l;
}

unsigned long ntohl(unsigned long l)
{
  return l;
}

unsigned short htons(unsigned short l)
{
  return l;
}

unsigned short ntohs(unsigned short l)
{
  return l;
}
