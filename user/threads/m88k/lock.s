/*

  Mach Operating System
  Copyright (c) 1992,1991 Carnegie Mellon University
  All Rights Reserved.
  
  Permission to use, copy, modify and distribute this software and its
  documentation is hereby granted, provided that both the copyright
  notice and this permission notice appear in all copies of the
  software, derivative works or modified versions, and any portions
  thereof, and that both notices appear in supporting documentation.
  
  CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
  CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
  ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
  
  Carnegie Mellon requests users of this software to return to
  
   Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
   School of Computer Science
   Carnegie Mellon University
   Pittsburgh PA 15213-3890
  
  any improvements or extensions that they make and grant Carnegie Mellon 
  the rights to redistribute these changes.
 

 */
/*
 * HISTORY
 * $Log:	lock.s,v $
 * Revision 2.2  92/08/03  18:04:14  jfriedl
 * 	brought from luna88k versions [danner]
 * 
 */


/*
  m88k/lock.s
  Spin implementation for m88K.
 
*/
	text

/*
 	int
 	spin_try_lock(m)
 		spin_t m 	(= int m for our purposes)
*/

	align	4
	global	_spin_try_lock
_spin_try_lock:
	ld	r3,r2,0
	bcnd	ne0,r3,1f
	or	r3,r0,1
	xmem	r3,r2,0
	bcnd	ne0,r3,1f
	jmp.n	r1
	or	r2,r0,1		/* yes */
1:
	jmp.n	r1
	or	r2,r0,0		/* no */

/*
 	void
 	spin_unlock(m)
 		spin_t m 	(= int m for our purposes)
*/
	align	4
	global	_spin_unlock
_spin_unlock:
	jmp.n	r1
	st	r0,r2,0

