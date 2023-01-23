/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON AND OMRON ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND OMRON DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	pio.h,v $
 * Revision 2.5  93/03/09  17:58:19  danner
 * 	Fixed diptest.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.4  93/01/26  18:03:42  danner
 * 	Added #ifndef file wrapper
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.3  92/08/03  18:14:16  jfriedl
 * 	Cleaned up (but still ways to go).
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.2  91/07/09  23:18:59  danner
 * 	Initial Checkin
 * 	[91/06/26  12:30:36  danner]
 * 
 * Revision 2.2  91/04/05  14:04:16  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

#ifndef __LUNA88K_PIO_H__
#define __LUNA88K_PIO_H__
/*
 * Parallel IO interface.
 *
 *   Channel #0 Port A -- DIP switch #1 (read)
 *   Channel #0 Port B -- DIP switch #2 (read)
 *   Channel #0 Port C -- Host Interupt Control (read/write)
 *   Channel #1 Port A -- LCD data (read/write)
 *   Channel #1 Port B -- interrupts the IO processor.
 *   Channel #1 Port C -- interrupt status, powerdown, LCD control.
 */

/*
 * The mode byte is:
 *  bits 0-2: "Group B"
 *  bit 0   : port C (lower nibble) - 0=output, 1=input
 *  bit 1   : port B                - 0=output, 1=input
 *  bit 2   : mode select (0=basic I/O, 1=strobed I/O)
 *
 *  bits 3-6: "Group A"
 *  bit 3   : port C (upper nibble) - 0=output, 1=input
 *  bit 4   : port A                - 0=output, 1=input
 *  bit 5,6 : mode select (0,0=basic I/O, 1,0=strobed I/O, 1,x=bidirectional)
 *
 *  bit 7   : valid (1=write new mode, 0=ignore me)
 */
#define	PIO0_MODE	0xb6   /* pio0-port mode set word */
#define	PIO0_MODED	0xb6   /* pio0-port mode set word (for DIPSW) */
#define	PIO1_MODEO	0x84   /* pio1-port mode set word (output) */
#define	PIO1_MODEI	0x94   /* pio1-port mode set word (input) */

/* pio0-portc */
 
#define	INT5_REQ	0x8	/* INT5 intrpt request 1:request 0:nothing */
#define	XP_RESET	0x7	/* XP reset 1:reset 0:run */
#define	PCEN_PARITY	0x6	/* PCEN* parity 1:disable 0:enable */
#define	INT5_PERMIT	0x4	/* INT5 intrpt permit 1:permit 0:inhibit */
#define	INT1_PERMIT	0x2	/* INT1 intrpt permit 1:permit 0:inhibit */
#define	INT1_REQ	0x1	/* INT1 intrpt request 1:request 0:no */

/* pio1-portc */

#define	LCD_E		0x7	/* LCD E-bit 1:active */
#define	LCD_RS		0x6	/* LCD RS-bit 1:data 0:instruction */
#define	LCD_RW		0x5	/* LCD R/W-bit 1:read 0:write */
#define	POWER_ON	0x4	/* POWERDOWN bit 1:POWERON 0:POWEROFF */
#define XP_NOREQ	0x1	/* CPU to XP intrpt request 1:no 0:request */

#define	CPU_XP		0xff	/* CPU to XP intrpt reuest */
#define PIO_DUMMY	2	/* dummy data for reading portc */
#define PIO_ON		1	/* pio control flag set */

#ifndef	LOCORE

/*
 * Functions in luna88kdev/pio.c
 */
extern unsigned int pio0portC(/*bit, flag*/);
extern unsigned int pio1portC(/*bit, flag*/);
extern void pio1portA_modechange(/*input=1 or output=0*/);

/*
 * pio-port struct. What the mapped device looks like as memory.
 */
struct pio {
    unsigned char porta;
    unsigned char pad1[3];
    unsigned char portb;
    unsigned char pad2[3];
    unsigned char portc;
    unsigned char pad3[3];
    unsigned char cntl;
    unsigned char pad4[3];
};

/*
 * Actual address for the 'PIO' ports.
 */
#define	PIO0	((volatile struct pio *)OBIO_PIO0A)
#define	PIO1	((volatile struct pio *)OBIO_PIO1A)

/*
 * DIP-SWITCH save area.
 * Bit is on if switch is UP (note: backwards from intutative).
 */
union dipswitch {
    struct {
	unsigned char dip_sw1; /* read from pio0 portA */
	unsigned char dip_sw2; /* read from pio0 portB */
    } dipswc;
    unsigned short dipsws;
};
extern union dipswitch dipswitch;

/*
 * diptest(NUM)
 * Return true if the front panel dip switch NUM (0=leftmost, 7=rightmost
 * of left switch) is on (down).
 */
#define diptest(num) (!(dipswitch.dipsws & (0x8000>>(num))))

/*
 * Access routines.
 */
#define pio1portA(data) 	(PIO1->porta = (data))
#define pio1portB(data) 	(PIO1->portb = (data))
#define dipread() 		(~dipswitch.dipsws)

#define	isINT1()	(PIO0->portc & INT1_REQ) /* INT1 intrpt request check */
#define	INT1mask()	pio0portC(INT1_PERMIT,0) /* INT1 intrpt mask */
#define	INT1unmask()	pio0portC(INT1_PERMIT,1) /* INT1 intrpt unmask */
#define isINT5()	(PIO0->portc & INT5_REQ)
						 /* INT5 intrpt request check */
#define	INT5mask()	pio0portC(INT5_PERMIT,0) /* INT5 intrpt mask */
#define	INT5unmask()	pio0portC(INT5_PERMIT,1) /* INT5 intrpt unmask */
#define	PCENenable()	pio0portC(PCEN_PARITY,0) /* PCEN PARITY enable */
#define	PCENdisable()	pio0portC(PCEN_PARITY,1) /* PCEN PARITY disable */
#define	XPreset()	pio0portC(XP_RESET,1)	/* XP reset */
#define	XPrun()		pio0portC(XP_RESET,0)	/* XP run */

#define	isXPINTclear()	pio1portC_read()\
			&XP_NOREQ		/* XP reset CPUtoXP intrpt */
#define	POWERDOWN()	pio1portC(POWER_ON,0)	/* POWERDOWN */

#define	DIPSW1()	(PIO0->porta)		/* dipsw1 read */
#define	DIPSW2()	(PIO0->portb)		/* dipsw2 read */
/* INT1 intrpt clear */
#define	INT1clear()	(dipswitch.dipswc.dip_sw2 = DIPSW2())
/* INT5 intrpt clear */
#define	INT5clear()	(dipswitch.dipswc.dip_sw1 = DIPSW1())

#define	XPINT()		pio1portB(CPU_XP)	/* send intrpt to XP */

#endif /* !LOCORE */
#endif /* __LUNA88K_PIO_H__ */
