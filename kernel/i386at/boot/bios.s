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
 * HISTORY
 * $Log:	bios.s,v $
 * Revision 2.3  93/05/10  17:46:55  rvb
 * 	Use C Comment
 * 	[93/05/04  17:29:17  rvb]
 * 
 * Revision 2.2  92/04/04  11:34:26  rpd
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[92/04/03            rvb]
 * 	From 2.5 version
 * 	[92/03/30            mg32]
 * 
 * Revision 2.2  91/04/02  14:35:21  mbj
 * 	Add Intel copyright
 * 	[90/02/09            rvb]
 * 
 */


/*
  Copyright 1988, 1989, 1990, 1991, 1992 
   by Intel Corporation, Santa Clara, California.

                All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

	.file	"bios.s"

#include "machine/asm.h"
	.text
/*
 * biosread(dev, cyl, head, sec)
 *	Read one sector from disk into the internal buffer "intbuf" which
 *	is the first 512 bytes of the boot loader.
 * BIOS call "INT 0x13 Function 0x2" to read sectors from disk into memory
 *	Call with	%ah = 0x2
 *			%al = number of sectors
 *			%ch = cylinder
 *			%cl = sector
 *			%dh = head
 *			%dl = drive (0x80 for hard disk, 0x0 for floppy disk)
 *			%es:%bx = segment:offset of buffer
 *	Return:		
 *			%al = 0x0 on success; err code on failure
 */

ENTRY(biosread)
	push	%ebp
	mov	%esp, %ebp

	push	%ebx
	push	%ecx
	push	%edx
	push	%es

	movb	0x10(%ebp), %dh
	movw	0x0c(%ebp), %cx
	xchgb	%ch, %cl	/* cylinder; the highest 2 bits of cyl is in %cl */
	rorb	$2, %cl
	movb	0x14(%ebp), %al
	orb	%al, %cl
	incb	%cl		/* sector; sec starts from 1, not 0 */
	movb	0x8(%ebp), %dl	/* device */
	xor	%ebx, %ebx	/* offset -- 0 */
				/* prot_to_real will set %es to BOOTSEG */

	call	EXT(prot_to_real)	/* enter real mode */
	movb	$0x2, %ah	/* subfunction */
	movb	$0x1, %al	/* number of sectors -- one */

	sti
	int	$0x13
	cli

	mov	%eax, %ebx	/* save return value */

	data16
	call	EXT(real_to_prot) /* back to protected mode */

	xor	%eax, %eax
	movb	%bh, %al	/* return value in %ax */

	pop	%es
	pop	%edx
	pop	%ecx
	pop	%ebx
	pop	%ebp

	ret

/*
 * putc(ch)
 * BIOS call "INT 10H Function 0Eh" to write character to console
 *	Call with	%ah = 0x0e
 *			%al = character
 *			%bh = page
 *			%bl = foreground color ( graphics modes)
 */


ENTRY(putc)
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	push	%ecx

	movb	0x8(%ebp), %cl

	call	EXT(prot_to_real)

	data16
	mov	$0x1, %ebx	/* %bh=0, %bl=1 (blue) */
	movb	$0xe, %ah
	movb	%cl, %al
	sti
	int	$0x10		/* display a byte */
	cli

	data16
	call	EXT(real_to_prot)

	pop	%ecx
	pop	%ebx
	pop	%ebp
	ret

/*
 * getc()
 * BIOS call "INT 16H Function 00H" to read character from keyboard
 *	Call with	%ah = 0x0
 *	Return:		%ah = keyboard scan code
 *			%al = ASCII character
 */

ENTRY(getc)
	push	%ebp
	mov	%esp, %ebp
	push	%ebx		/* save %ebx */

	call	EXT(prot_to_real)

	movb	$0x0, %ah
	sti
	int	$0x16
	cli

	movb	%al, %bl	/* real_to_prot uses %eax */

	data16
	call	EXT(real_to_prot)

	xor	%eax, %eax
	movb	%bl, %al

	pop	%ebx
	pop	%ebp
	ret

/*
 * ischar()
 *       if there is a character pending, return it; otherwise return 0
 * BIOS call "INT 16H Function 01H" to check whether a character is pending
 *	Call with	%ah = 0x1
 *	Return:
 *		If key waiting to be input:
 *			%ah = keyboard scan code
 *			%al = ASCII character
 *			Zero flag = clear
 *		else
 *			Zero flag = set
 */

ENTRY(ischar)
	push	%ebp
	mov	%esp, %ebp
	push	%ebx

	call	EXT(prot_to_real)		/* enter real mode */

	xor	%ebx, %ebx
	movb	$0x1, %ah
	sti
	int	$0x16
	cli
	data16
	jz	nochar
	movb	%al, %bl

nochar:
	data16
	call	EXT(real_to_prot)

	xor	%eax, %eax
	movb	%bl, %al

	pop	%ebx
	pop	%ebp
	ret

/*
 * get_diskinfo():  return a word that represents the
 *	max number of sectors and  heads and drives for this device
 */

ENTRY(get_diskinfo)
	push	%ebp
	mov	%esp, %ebp
	push	%es
	push	%ebx
	push	%ecx
	push	%edx

	movb	0x8(%ebp), %dl		/* diskinfo(drive #) */
	call	EXT(prot_to_real)	/* enter real mode */

	movb	$0x8, %ah		/* ask for disk info */

	sti
	int	$0x13
	cli

	data16
	call	EXT(real_to_prot)	/* back to protected mode */

	xor	%eax, %eax

/*	form a longword representing all this gunk */
	movb	%dh, %ah		/* # heads */
	andb	$0x3f, %cl		/* mask of cylinder gunk */
	movb	%cl, %al		/* # sectors */

	pop	%edx
	pop	%ecx
	pop	%ebx
	pop	%es
	pop	%ebp
	ret

/*
 *
 * memsize(i) :  return the memory size in KB. i == 0 for conventional memory,
 *		i == 1 for extended memory
 *	BIOS call "INT 12H" to get conventional memory size
 *	BIOS call "INT 15H, AH=88H" to get extended memory size
 *		Both have the return value in AX.
 */

ENTRY(memsize)
	push	%ebp
	mov	%esp, %ebp
	push	%ebx

	mov	8(%ebp), %ebx

	call	EXT(prot_to_real)		/* enter real mode */

	cmpb	$0x1, %bl
	data16
	je	xext
	
	sti
	int	$0x12
	cli
	data16
	jmp	xdone

xext:	movb	$0x88, %ah
	sti
	int	$0x15
	cli

xdone:
	mov	%eax, %ebx

	data16
	call	EXT(real_to_prot)

	mov	%ebx, %eax
	pop	%ebx
	pop	%ebp
	ret
