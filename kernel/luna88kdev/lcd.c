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
 * $Log:	lcd.c,v $
 * Revision 2.4  93/03/09  17:59:49  danner
 * 	Used LCD_GOTO() in init message.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:07:52  danner
 * 	Minor mods to sync with ANSIfication of other MD files
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:46:24  jfriedl
 * 	Brought from Nagaoka. Completely redid for 3.0.
 * 	[92/07/24            jfriedl]
 * 
 */

/*
 * LCD driver
 */
#include <luna88k/machdep.h>	 /* standard stuff               */
#include <device/device_types.h> /* for return codes             */
#include <device/buf.h>          /* for struct uio               */
#include <device/io_req.h>	 /* for io_req_t type            */
#include <luna88k/pio.h>	 /* parallel-io definitions -- PIO1, etc. */
#include <luna88k/lcd.h>	 /* LCD control codes.           */
#include <machine/mach_param.h>  /* for interrupt disable macros */

/*
 * The boot message is global so that they may be patched if desired.
 */
                                    /*"1234567890123456"*/
unsigned char lcd_boot_message1[16] = " Omron Luna/88k ";
unsigned char lcd_boot_message2[16] = "  CMU Mach 3.0  ";

static int lcd_device_exists;  /* set by lcd_slave() if the LCD exists */

/*
 * Communication to the LCD device is a bit odd.
 * To write a character...
 *   Check to make sure that the busy flag is not set:
 *      The parallel-io (PIO) channel must be set to read (via PIO1->cntl).
 *      Set the desired status to portc. They are selected from the sets
 *        of flags below.  Note that if POWER is not set to portc soon after
 *        the cntl port is set, the machine's power will turn off (!).
 *	  To read the busy flag, use status (POWER+READ_BUSYFLAG+ENABLE)
 *      Wait until porta's high bit is off.
 *	Change from ENABLE to DISABLE. (don't know why... manual says).
 *   Now write character:
 *      The parallel-io (PIO) channel must be set to write (via PIO1->cntl).
 *      Set the desired status to portc (use POWER+WRITE_DATA+ENABLE to
 *         write a character to the display, or POWER+WRITE_CMD+ENABLE to
 *	   send a control code).
 *	Change from ENABLE to DISABLE.
 */

#define ENABLE             0x80
#define DISABLE            0x00

#define READ_DATA          (0x20 | 0x40)
#define READ_BUSYFLAG      (0x20 | 0x00)

#define WRITE_DATA         (0x00 | 0x40)
#define WRITE_CMD          (0x00 | 0x00)

#define POWER              0x10
#define POWER_OFF	   0x00

#ifndef __GNUC__
#  define __inline__    /*nothing*/
#endif

/*
 * Send a single character to the LCD display.
 * Only used in one place, so inlined.
 */
static __inline__ void write_lcd_data(data)
unsigned char data;
{
    m88k_psr_type psr;

    /*
     * First, check busy flag and wait for it to clear.
     */
    psr = disable_interrupts_return_psr();
    PIO1->cntl = PIO1_MODEI; /* set parallel-IO channel #1 to input mode. */
    PIO1->portc = POWER + READ_BUSYFLAG + ENABLE;  /* want to read busy flag */
    set_psr(psr);

    while (PIO1->porta & 0x80) /* wait until high bit is off */ 
	    ;
    psr = disable_interrupts_return_psr();
    PIO1->portc = POWER + READ_BUSYFLAG + DISABLE;

    /*
     * Now send the character to the device.
     */
    PIO1->cntl = PIO1_MODEO; /* set parallel-IO channel #1 to output mode. */
    PIO1->portc = POWER + WRITE_DATA + ENABLE;
    PIO1->porta = (unsigned char)data;
    PIO1->portc = POWER + WRITE_DATA + DISABLE;
    set_psr(psr);
}

/*
 * Send a single control character to the LCD device.
 * Except for the portc flags, this function is identical
 * to write_lcd_data() above.
 * Only used in one place, so inlined.
 */
static __inline__ void write_lcd_cmd(cmd)
unsigned cmd;
{
    m88k_psr_type psr;

    psr = disable_interrupts_return_psr();
    PIO1->cntl = PIO1_MODEI;
    PIO1->portc = POWER + READ_BUSYFLAG + ENABLE;
    set_psr(psr);

    while (PIO1->porta & 0x80)
	    ;
    psr = disable_interrupts_return_psr();
    PIO1->portc = POWER + READ_BUSYFLAG + DISABLE;

    PIO1->cntl = PIO1_MODEO;
    PIO1->portc = POWER + WRITE_CMD + ENABLE;
    PIO1->porta = (unsigned char)cmd;
    PIO1->portc = POWER + WRITE_CMD + DISABLE;
    set_psr(psr);
}

/*
 * Read a single character from the LCD display.
 * Only used in one place, so inlined.
 */
static __inline__ unsigned read_lcd_data()
{
    register unsigned char data;
    m88k_psr_type psr;

    psr = disable_interrupts_return_psr();
    PIO1->cntl = PIO1_MODEI;
    PIO1->portc = POWER + READ_BUSYFLAG + ENABLE;
    set_psr(psr);

    while (PIO1->porta & 0x80)
	    ;
    psr = disable_interrupts_return_psr();
    PIO1->portc = POWER + READ_BUSYFLAG + DISABLE;

    PIO1->portc = POWER + READ_DATA + ENABLE;
    data = PIO1->porta;
    PIO1->portc = POWER + READ_DATA + DISABLE;
    set_psr(psr);
    return data;
}

/*
 * Given a hunk of data, send it to the LCD.
 *
 * If the special character LCD_CTRL is found, the _next_ character
 * after is send as a control character to the LCD device.
 * Otherwise, characters are send as data to the LCD display.
 */
static void output_lcd_data(data, len)
unsigned char *data;
int len;
{
    while (len-- > 0)
    {
        unsigned char c;

	if (c = *data++, c != LCD_CTRL)
	    write_lcd_data(c);
	else if (len-- > 0)
	    write_lcd_cmd(*data++);
    }
}

/*
 * "Open" the LCD device (nothing is actually done).
 * Can only be opened for write.
 */
int lcd_open(dev, flag, ior)
unsigned dev;
int flag;
io_req_t ior;
{
    if (lcd_device_exists) 
	return flag == D_WRITE ? D_SUCCESS : D_INVALID_OPERATION;
    else 
	return D_DEVICE_DOWN;
}

/*
 * Write to the LCD device.
 */
int lcd_write(dev, uio)
int dev;
struct uio *uio;
{
    if (!lcd_device_exists) 
	return D_DEVICE_DOWN;
    output_lcd_data((unsigned char*)uio->io_data, uio->io_count);
    return D_SUCCESS;
}

/*
 * "Close" the lcd device. Nothing is actually done.
 */
int lcd_close(dev)
int dev;
{
    return lcd_device_exists ? D_SUCCESS : D_DEVICE_DOWN;
}

/*
 * Initialize the LCD device upon boot.
 */
void lcd_slave()
{
    static unsigned char lcd_init_string[] = {  
	LCD_CTRL, LCD_INIT_CMD,   /* initialize */
	LCD_CTRL, LCD_ENTRY_CMD,
	LCD_CTRL, LCD_ON_CMD,     /* turn it on */
	LCD_CTRL, LCD_CLS_CMD,    /* clear screen */
	LCD_CTRL, LCD_HOME_CMD,   /* home cursor */
    };

    output_lcd_data(lcd_init_string, sizeof(lcd_init_string));

    /*
     * If the device exists, reading a character at this point should
     * yield a blank space (the screen has been cleared).  If the device
     * isn't there, some other value (such as the last character written
     * to the parallel-io channel -- the HOME command) will be there.
     */
    lcd_device_exists = (read_lcd_data() == ' ');

    /* Put up an initial message */
    if (lcd_device_exists)
    {
        static unsigned char goto_line1[2] = { LCD_CTRL, LCD_GOTO(0,0) };
        static unsigned char goto_line2[2] = { LCD_CTRL, LCD_GOTO(1,0) };
	output_lcd_data(goto_line1,        sizeof(goto_line1));
	output_lcd_data(lcd_boot_message1, sizeof(lcd_boot_message1));
	output_lcd_data(goto_line2,        sizeof(goto_line2));
	output_lcd_data(lcd_boot_message2, sizeof(lcd_boot_message2));
    }
}
