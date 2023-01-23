/* 
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
 * $Log:	pio.c,v $
 * Revision 2.2  92/08/03  17:46:35  jfriedl
 * 	Brought from Mach 2.5.
 * 	[92/07/24            jfriedl]
 * 
 */

#include <kern/assert.h>
#include <luna88k/board.h>
#include <luna88k/pio.h>
#include <luna88k/asm_macro.h>	/* for interrupt disable macros */

/*
 *	set bit-pattern to pio0-portC
 */
unsigned int
pio0portC(bit,onoff)
register int bit ;
register int onoff;
{
    register unsigned char val;
    psr_type psr;
    
    /* make sure the input is OK.  Bit must be [0..7] */
    assert(bit >= 0 && bit <= 7);

    if (onoff)
	    val = (bit << 1) | 0x01;
    else
	    val = (bit << 1);
    
    psr = disable_interrupts_return_psr();

    PIO0->cntl = val;
    val = PIO0->portc;
    
    set_psr(psr);
    return val;
}

/*
 *     write bit-pattern to pio1-portC
 */
unsigned int
pio1portC(bit,onoff)
register int bit ;
register int onoff ;
{
	register unsigned char val;
	psr_type psr;

	assert(bit >= 0 && bit <= 7);

	if (onoff)
		val = (bit << 1) | 0x01;
	else
		val = (bit << 1);

	psr = disable_interrupts_return_psr();

	PIO1->cntl = val;
	val = PIO1->portc;

	set_psr(psr);
	return val;
}

void pio1_setmode(input)
int input;
{
    static int current_mode = -1;
    psr_type psr;
    
    if (current_mode == input)
	return;

    psr = disable_interrupts_return_psr();
    if (input) {
	/* change to input mode */
	PIO1->cntl = PIO1_MODEI;
	pio1portC(POWER_ON,1); 
    } else {
	/* change to output mode */
	PIO1->cntl = PIO1_MODEO;
	pio1portC(POWER_ON,1); 
    }
    current_mode = input;
    set_psr(psr);
}
