/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * $Log:	bzero.c,v $
 * Revision 2.2  92/08/03  18:02:55  jfriedl
 * 	brought from luna88k versions [danner]
 * 	Created
 * 	[91/07/20            danner]
 * 
 * Revision 2.4  92/04/01  10:56:30  rpd
 * 	Fixed most serious typo.
 * 	[92/03/15            danner]
 * 
 * Revision 2.3  92/02/19  14:18:17  elf
 * 	Fixed subtract length twice bug.
 * 	[91/09/20            danner]
 * 	Changes.
 * 	[91/09/19  13:51:51  danner]
 * 
 * Revision 2.2  91/08/24  12:31:23  af
 * 	Created
 * 	[91/07/20            danner]
 * 
 */
/*
 *
 * Author : Daniel Stodolsky
 * Created: August 1991
 *
 * Slightly optimized bzero
 *
 */

void bzero(ptr,size)
register char *ptr; 
register unsigned int size;
{
  register int *iptr;
  /* get word aligned */

  while( (((unsigned int) ptr) & 3) && size)
    {
      *ptr++ = '\0';
      size--;
    }

  iptr = (int *) ptr;  
  while(size)
    {
      register int quanta;

      quanta = size > 64 ? 64  : size;
      size -= quanta;
      quanta >>= 2; /* get to words */
      switch (quanta)
	{
	case 16: 	  iptr[15] = 0;
	case 15:	  iptr[14] = 0;
	case 14:	  iptr[13] = 0;
	case 13:	  iptr[12] = 0;
	case 12:	  iptr[11] = 0;
	case 11:	  iptr[10] = 0;
	case 10:	  iptr[9] = 0;
	case 9:		  iptr[8] = 0;
	case 8:		  iptr[7] = 0;
	case 7:		  iptr[6] = 0;
	case 6:		  iptr[5] = 0;
	case 5:		  iptr[4] = 0;
	case 4:		  iptr[3] = 0;
	case 3:		  iptr[2] = 0;
	case 2:		  iptr[1] = 0;
	case 1:		  iptr[0] = 0;
	  break;
	}
      iptr += quanta;
    }

  return;
}
