/* 
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 * $Log:	bcopy.c,v $
 * Revision 2.2  92/08/03  18:02:51  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 * Revision 2.5  92/05/04  11:28:46  danner
 * 	Fixed nocpy() for cases when not 4-byte aligned and len < 4.
 * 	[92/04/19  16:33:44  jfriedl]
 * 
 * Revision 2.4  92/04/01  10:56:25  rpd
 * 	Corrected, optimized nocpy.
 * 	[92/03/21            danner]
 * 
 * Revision 2.3  92/02/18  18:04:07  elf
 * 	Removed misconcieved dependence of memcpy.
 * 	[92/01/17            danner]
 * 
 * Revision 2.2  91/07/09  23:24:03  danner
 * 	Additions.
 * 	[91/07/08  23:01:44  danner]
 * 
 *
 */

static void nocpy(to, from, len)
register char *from, *to;
register unsigned int len;
{
  /* FROM and TO are known to be nonoverlapping. */

  switch(len) {
    case 4: to[3] = from[3];
    case 3: to[2] = from[2];
    case 2: to[1] = from[1];
    case 1: to[0] = from[0];
	    return;
  }
  
  if ((((int) from) & 0x3) == (((int) to) & 0x3) )
    {
      int *ito, *ifrom;
      unsigned short int ll = 4 - ((int) from & 0x3);
      int  il;

      /* on the same alignment boundry */
      switch (ll)
	{
	case 4:
	  ll = 0;
	  break;
	case 3:
	  to[2] = from[2];
	case 2:
	  to[1] = from[1];
	case 1:
	  to[0] = from[0];
	default:
	  break;
	}
      
      len -= ll;
      il = len>>2;
      len &= 0x3;
      ifrom = (int *) (from+ll);
      ito = (int *) (to+ll);
      while (il--)
	*ito++ = *ifrom++;
      from = (char *) ifrom;
      to = (char *) ito;

      /* final bytes */
      switch (len)
	{
	case 3:
	  to[2] = from[2];
	case 2:
	  to[1] = from[1];
	case 1:
	  to[0] = from[0];
	default:
	  break;
	}
    }
  else
    { /* misaligned with respect to each other. hopeless */
      while (len--)
	*to++ = *from++;
    }
  return;
}

void bcopy(from,to,len)
register char *from, *to;
register unsigned int len;
{
  /* LOGIC - if to+len < from and to < from then there is no overlap, 
             so use nocpy. 
             Similarly if  from+len < to and from < to, there is no overlap.
   
	     If from < to and from+len >= to, then we need to copy tailwise.
             If to < from and to+len > from,  then we need to copy headwise.

             Assuming  pointers can be coerced to unsigned ints. 
  */

  if (to==from || len==0) 
       return;

  if (((unsigned int) to) < ((unsigned int) from))
    {
      if (len < from-to)
         nocpy(to,from,len); /* Memcpy reverses arguments */
      else  /* to < from, to+len >= from  -> headwise copy */
         { 
           register int i;
           for (i=0; i<len; i++)
             to[i] = from[i];
         }
    }
  else /* to > from */
   {
      if (len < to-from)
         nocpy(to,from,len); /* Memcpy reverses arguments */
      else /* from < to, from+len >= to  -> tailwise copy */
        {
          register int i;
	  for (i=len-1; i>=0; i--)
            to[i] = from[i];
        }
   }
   return;
}
