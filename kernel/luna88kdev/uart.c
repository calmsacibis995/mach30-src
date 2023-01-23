/*
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 * $Log:	uart.c,v $
 * Revision 2.11  93/03/09  18:00:31  danner
 * 	cleanup.
 * 	[93/03/09  14:31:44  jfriedl]
 * 
 * Revision 2.10  93/02/05  08:07:52  danner
 * 	commented-out kanji support (for now)
 * 	[93/02/04            jfriedl]
 * 
 * Revision 2.9  93/01/26  18:07:47  danner
 * 	caddr_t -> vm_offset_t
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.8  93/01/14  17:39:46  danner
 * 	Removed kanji support.
 * 	[92/12/10            jfriedl]
 * 
 * 	mods to quiet gcc warnings.
 * 	Removed some unused (2.5) code.
 * 	[92/08/12            jfriedl]
 * 	Removed call to ttstart - not needed in pure kernel.
 * 	[92/11/18            dbg]
 * 
 * Revision 2.7  92/08/03  17:46:40  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.6  92/05/21  17:22:09  jfriedl
 * 	Cleanup to quiet gcc warnings.
 * 	Changed UNSIGNED CHAR and DEV_T args to UNSIGNED (ANSI/std problems)
 * 	Removed unused flag argument from utclose.
 * 	[92/05/18            jfriedl]
 * 	Appended 'U' to constants that would otherwise be signed.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.5  92/05/04  11:27:37  danner
 * 	Added initialization of t_stop field (from jfriedl). 
 * 	Updated copyright.
 * 	[92/04/30            danner]
 * 
 * Revision 2.4  92/04/01  10:56:06  rpd
 * 	Added a missing ttyinput call.
 * 	[92/03/16  17:22:02  danner]
 * 
 * Revision 2.3  92/02/19  14:07:21  elf
 * 	Includes fixup.
 * 	[92/01/20            danner]
 * 	Added ddb trap via the funky two over from the BS key (right next
 * 	 to the DEL) key.
 * 	[91/09/17            danner]
 * 
 * Revision 2.2  91/07/09  23:21:19  danner
 * 	Initial Checkin
 * 	[91/06/26  11:46:18  danner]
 * 
 * Revision 1.1  91/04/21  16:22:26  jfriedl
 * Initial revision
 * 
 * Revision 2.2  91/04/05  14:10:51  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/*
 * 	cpu-board serial i/o driver
 */
#include <luna88k/machdep.h>	/* standard_goodies */
#include <sys/types.h>
#include <kern/time_out.h>
#include <device/tty.h>
#include <device/io_req.h>
#include <device/errno.h>
#include <luna88kdev/kbms.h>
#include <luna88kdev/cons.h>
#include <luna88kdev/uart.h>


#define KBIDLESCAN HZ /* how often to scan for auto-repeats while idle */
#define KBREPTSCAN 1  /* how often to scan for auto-repeats when not idle */

#define RXRDY   0x01		/* Rx CHAR. Available (RR0) */
#define INTPEND 0x02		/* INT Pending (CH.A) (RR0) */
#define TXRDY   0x04		/* Tx Buffer Empty (RR0) */
#define CD	0x08		/* Data Carrier Detect (RR0) */
#define CTS	0x20		/* CTS (RR0) */
#define TXUDRUN	0x40		/* Tx Underrun/EOM (RR0) */
#define RCVBRK	0x80		/* Break/Abort (RR0) */

#define ALLSENT 0x01		/* All Sent (RR1) */
#define PE	0x10		/* Parity Error (RR1) */
#define RXOVRUN 0x20		/* Rx Overrun Error (RR1) */
#define FRMER	0x40		/* CRC/Framing Error (RR1) */

#define RSTINT  0x38		/* End of Interrupt (WR0) */
#define RSTESIN 0x10		/* Reset External/Status Interrupt (WR0) */
#define CHANRST 0x18		/* Channel Reset (WR0) */
#define RSTPEDI 0x28		/* Reset Tx Interrput/DMA Pending (WR0) */
#define ERRST	0x30		/* Error Reset (WR0) */
#define EOMRST	0xc0		/* Tx Underrun/EOM (WR0) */
#define ESIE    0x01		/* External Interrupt Enable (WR1) */
#define TXIE	0x02		/* Tx Interrupt/DMA Enable (WR1) */
#define VECMODE 0x04		/* Status Affects Vector (CH.B) (WR1) */
#define RXIE	0x10	/* Rx Interrupt - All Char or Specilal condition (WR1)*/
#define RXID	0xe7		/* Rx Int/DMA disable (WR1) */
#define RR2     2
#define INTMODE 0x03		/* INT/DMA Mode 0,1 (WR2A) */
#define CHMSK   0x04		/* Priority Select (WR2A) */
#define RXAVL   0x02		/* Receive Character Available (WR2B) */
#define SPECIAL 0x03		/* Special Receive Condition (WR2B) */
#define TXEMPT  0x00		/* Transmit Buffer Empty (WR2B) */
#define WR1DATA 0x1e
#define WR2DATA 0x14
#define WR4DATA 0x40
#define BITS0	0xc0
#define BITS5	0x00		/* Rx 5 Bits/Character (WR3) */
#define BITS7	0x40		/* Rx 7 Bits/Character (WR3) */
#define BITS6   0x80		/* Rx 6 Bits/Character (WR3) */
#define BITS8	0xc0		/* Rx 8 Bits/Character (WR3) */
#define AUTOEN  0x20		/* Auto Enables (WR3) */
#define RXEN    0x01		/* Rx Enable (WR3) */
#define ONESB	0x04		/* 1 Stop Bit/Character (WR4) */
#define TWOSB   0x0c		/* 2 Stop Bit/Character (WR4) */
#define PENABLE 0x01		/* Parity Enable (WR4) */
#define EVENPA	0x02		/* Parity Even/Odd (1=Even) (WR4) */
#define DTR     0x80		/* DTR (WR5) */
#define SENDBRK	0x10		/* Send Break (WR5) */
#define TXEN	0x08		/* Tx Enable (WR5) */
#define RTS	0x02		/* RTS (WR5) */

#define WR0     0
#define WR1	1
#define WR2	2
#define WR3	3
#define WR4	4
#define WR5	5
#define NWR     6

/* Hardware bits */
#define	BSD_DONE	0200
#define	BSD_NABLE	0100
#define	BSD_OVERRUN	040000
#define	BSD_FRERROR	020000
#define	BSD_PERROR	010000

/* OMRON extended flag (set in t_OMRON) */
#define WCLOSE          02
#define LCLOSE          04
#define HZ 60

struct uart_device  *uart_addr[] = {
    (struct uart_device *)(OBIO_SIO),
    (struct uart_device *)(OBIO_SIO+8)
};
#define UART_COUNT (sizeof(uart_addr)/sizeof(uart_addr[0]))
#define status	cmd		/* both written and read from */

/*
 * Apparently can handle only 2400, 4800, and 9600 baud
 */
static unsigned char uart_speeds[16] = {
    0,0,0,0,0,0,0,0,0,0,0,0xc0,0x80,0x40,0,0
};

static int uart_init = 0;
static char uart_brk[UART_COUNT];
static char lsiprm[UART_COUNT][NWR];


extern void ttrstrt();
extern void cnstart();

int kbms_init = 0;		

/* 2.5 compabaility stuff */
#define TOSTOP 0x00400000
#define DECCTQ 0x40000000
#define CSTOP  ('s' & 0x37)
#define CSTART ('q' & 0x37)

#define	IFLAGS (TOSTOP|PASS8|EVENP|ODDP|ECHO|XTABS|CRMOD)

static void utlsi(int line, unsigned wr, unsigned data, int onoff)
{
    char *prm;
    struct uart_device volatile *uartaddr;
    unsigned s;

    s = SPLX();
    prm = &lsiprm[line][wr];
    uartaddr = uart_addr[line];
    if (onoff==ON)
	*prm |= data;
    else
	*prm &= data;
    uartaddr->cmd = wr;
    uartaddr->cmd = *prm;
    uartaddr->cmd = ERRST;
    splx(s);
}

static int utmodem(unsigned dev, int flag)
{
    struct uart_device volatile *uartaddr;
    int line;

    line = ldevtop(dev);
    uartaddr = uart_addr[line];
    if (flag == OFF) 
	utlsi(line,WR5,~(DTR|RTS),OFF);
    else 
	utlsi(line,WR5,(DTR|RTS),ON); 
    return(uartaddr->status & CD);
}

static void kbreptscan(void)
{
    char repeatbuf[32];
    struct tty *tp;
    unsigned short c;
    unsigned char ctmp = 0;
    int count, i, j;
    
    tp = curtty;
    
    if (!(tp->t_state&(TS_ISOPEN|TS_WOPEN))) {
	timeout((int(*)())kbreptscan,0,KBIDLESCAN);
	return;
    }
    c = 0;
    count = kbrepeat(repeatbuf);
    if(count == 0 || tp->t_state & TS_TBLOCK) {
	timeout((int(*)())kbreptscan,0,KBREPTSCAN);
	return;
    }
    if((count > 0) && (tp->t_flags & DECCTQ)) {
	for(i=0;i<count;) {
	    c = repeatbuf[i] & 0177;
	    if ((ctmp == CSTART) || (ctmp == CSTOP)) {
		ctmp = c;
		count--;
		for(j=0;j<count;j++)
		    repeatbuf[j] = repeatbuf[j+1];
		continue;
	    } else 
		ctmp = c;
	    i++;
	}
    } else if(count < 0) 
	c = (BSD_FRERROR|BSD_PERROR|BSD_OVERRUN);
    
    if (tp->t_flags&DECCTQ) {
	if(tp->t_state & TS_TTSTOP ) {
	    if (((ctmp == CSTART) || (ctmp == CSTOP)) &&
		(count == 0)) {
		timeout((int(*)())kbreptscan,0,KBREPTSCAN);
		return;
	    }
	}
    }
    if (c & (BSD_FRERROR|BSD_PERROR|BSD_OVERRUN)) {
	timeout((int(*)())kbreptscan,0,KBREPTSCAN);
	return;
    }
    i = count;
    while (count) {
	c = repeatbuf[i-(count--)];
	ttyinput(c,tp);
    }
    timeout((int(*)())kbreptscan,0,KBREPTSCAN);
}

static void utparam(unsigned dev)
{
    struct tty *tp;
    char data;
    int line;
    unsigned s;
    char wr3,wr5, wr4 = 0;	
    static short kbrep_scan = 0;
    
    tp = &cons_tty[minor(dev)];
    line = ldevtop(dev);
    /* hang up line */
    if (tp->t_ispeed == 0) {
	utmodem(dev, OFF);
	return;
    }
    if ((minor(dev) != TERM)&&(kbrep_scan == 0)) {
	kbreptscan();
	kbrep_scan++;
    }
    if (minor(dev) != TERM)
	tp->t_ispeed = tp->t_ospeed = B9600;
    s = SPLX();
    lsiprm[line][WR4] = 0;
    lsiprm[line][WR3] = 0;
    lsiprm[line][WR5] = 0;
    if (!(data = uart_speeds[(unsigned char)tp->t_ispeed]))
	data = uart_speeds[B9600];
    if (tp->t_ispeed == B134) {			/* CS6 */
	wr3 = BITS6;
	wr5 = BITS6>>1;
    } else if (tp->t_flags & (RAW|LITOUT|PASS8)) {	/* CS8 */
	wr3 = BITS8;
	wr5 = BITS8>>1;
    } else {					/* CS7 */
	wr3 = BITS7;
	wr5 = BITS7>>1;
    }
    if (((tp->t_flags&ODDP)&&(tp->t_flags&EVENP)) ||
	(!(tp->t_flags&ODDP)&&!(tp->t_flags&EVENP))) {
	wr4 = 0;
    } else if (tp->t_flags&ODDP) {
	wr4 = PENABLE;
    } else {
	wr4 = (PENABLE|EVENPA);
    }
#ifdef KANJI_STUFF
    if (tp->t_ospeed == B110 || tp->t_omron & TS_TWOSB)
#else
    if (tp->t_ospeed == B110)
#endif
	wr4 |= TWOSB;
    else
	wr4 |= ONESB;
    wr4 |= data;
    wr5 |= RTS;
    utlsi(line,WR4, wr4, ON );
    utlsi(line,WR3,(RXEN|wr3),ON);
    utlsi(line,WR5,(TXEN|wr5),ON);
    lsiprm[line][WR1] = 0;
    utlsi(line,WR1,RXIE|TXIE,ON); 
    splx(s);
}

static int utstart(struct tty *tp)
{
    struct uart_device volatile *uartaddr;
    int c, x;

    uartaddr = uart_addr[ldevtop(TERM)];
    x = SPLX();
    if (tp->t_state & (TS_TIMEOUT|TS_BUSY|TS_TTSTOP))
	goto out;
    if (tp->t_outq.c_cc <= TTLOWAT(tp)) {
	    tt_write_wakeup(tp);
    }
    if (tp->t_outq.c_cc == 0) 
	goto out;
    tp->t_state |= TS_BUSY;
    c = getc(&tp->t_outq);
    if (tp->t_flags & (RAW|LITOUT|PASS8)) {
	uartaddr->data = c&0xff;
    } else if (c <= 0177) {
	uartaddr->data = c&0xff;
    } else {
	tp->t_state &= ~TS_BUSY;
	timeout((int(*)())ttrstrt, (vm_offset_t) tp, c&0xff);
	tp->t_state |= TS_TIMEOUT;
    }
 out:
    splx(x);
    return 1;
}

static void utstop(struct tty *tp, int flag)
{
    unsigned s;

    s = SPLX();
    if (tp->t_state & TS_BUSY) 
	if ((tp->t_state&TS_TTSTOP)==0) 
	    tp->t_state |= TS_FLUSH;
    splx(s);
}

int utopen(unsigned dev, int flag, io_req_t ior)
{
    struct tty *tp;
    unsigned s;

    if (minor(dev) > TERM)
	return D_NO_SUCH_DEVICE;
    tp = &cons_tty[minor(dev)];
    tp->t_stop = (int (*)())utstop;

    s = SPLX();
    if (minor(dev) != TERM) {
	tp->t_oproc = (int (*)())cnstart;
    } else {
	tp->t_oproc = utstart;
    }
    if ((tp->t_state&TS_ISOPEN) == 0) {
	ttychars(tp);
	tp->t_flags = IFLAGS;
	tp->t_ospeed = tp->t_ispeed = B9600;
#ifdef KANJI_STUFF
	j_init(tp);     		/* Japanese  tty.c j_init() */
#endif
	if (minor(dev) != TERM) {	/* BMcons      */
	    if (kbms_init == 0) {	
		utparam(dev);
		kbms_init = 1;
	    }			
	    kbsetinit(0);       /* keyboard initialize    */
	} else {
	    utparam(dev);
	}
    } 

    if(utmodem(dev, ON)) 
      ttymodem(tp, 1);

    splx(s);
    return char_open(dev, tp, flag, ior);
}

int utclose(unsigned dev)
{
    struct tty *tp;
    unsigned  s;
    extern kbreset;

    tp = &cons_tty[minor(dev)];
    if (tp->t_state&TS_HUPCLS && !(tp->t_state&TS_ISOPEN))
	utmodem(dev, OFF);
    s = SPLX();
    if (minor(dev) == KBD)
	kbreset = ENABLE;
    ttyclose(tp);
    splx(s);
    return 0;
}

int utread(unsigned dev, io_req_t ior)
{
    struct tty *tp;

    tp = &cons_tty[minor(dev)];
    return char_read(tp, ior);
}

int utwrite(unsigned dev, io_req_t ior)
{
    struct tty *tp;
    
    tp = &cons_tty[minor(dev)];
    return char_write(tp, ior);
}

static void utrint(int line)
{
    char encodebuf[32];
    struct tty *tp;
    struct uart_device volatile *uartaddr;
    unsigned char data;
    int count, i;
    unsigned short c;
    
    uartaddr = uart_addr[line];
    if (line == 0)
	tp = &cons_tty[TERM];
    else
    {
	if (line == KBD && !keyboardopen)
	    tp = &cons_tty[CONSOLE];
	else
	    tp = curtty;

	if (tp != &cons_tty[CONSOLE] && tp != &cons_tty[KBD])
	    panic("utrint");
    }

    data = uartaddr->data;

    if ((tp->t_state&(TS_ISOPEN|TS_WOPEN)) == 0)
	return;

    uartaddr->cmd = WR1;
    c = uartaddr->status;
    if (c & (PE|RXOVRUN))
	uartaddr->cmd = ERRST;

    if(uart_brk[line]) {
	uart_brk[line] = 0;
	c = (FRMER <<7);
	data = tp->t_breakc; /* XXX was 0 */
	ttyinput(data, tp);
    } else { /* not break charcter */
	if (line == 0) { /* TTYA  */
#ifdef MACH_KDB
	    if (data == 0x1c) { /* Ctrl-\ Ctrl-\ (???) */
		gimmeabreak();
		return;
	    }
#endif
	    if (tp->t_ispeed == B134)	/* CS6 */
		data &= 077;
	    else if (tp->t_flags&(RAW|LITOUT|PASS8))/*CS8*/
		;
	    else
		data &= 0177;		/* CS7 */
	     ttyinput(data, tp);
	} else { /* must be from keyboard/mouse */
	    if(mouse_receive(data))
		return;
	    if(!(tp->t_state&(TS_ISOPEN|TS_WOPEN)))
		return;
	    count = kbencode(data, encodebuf);
	    if(count == 0 || tp->t_state & TS_TBLOCK)
		return;
	    for (i = 0; i < count; i++) {
		data = encodebuf[i];

		  ttyinput(data,tp);
	    }
	} /* CONSOLE */
    } /* not break charcter */
}

static void utxint(int line)
{
    struct tty *tp;
    struct uart_device volatile *uartaddr;

    uartaddr = uart_addr[line];
    if (line == 0) 
	tp = &cons_tty[TERM];
    else 
	goto out;
    if((tp->t_state & (TS_ISOPEN|TS_WOPEN))==0) 
	goto out;

    if (tp->t_state & TS_FLUSH) 
	tp->t_state &= ~TS_FLUSH;
    if (tp->t_state & TS_BUSY) {
	tp->t_state &= ~TS_BUSY;
	utstart(tp);
	if (tp->t_state & TS_BUSY)
	    return;
    }
 out:
    uartaddr->cmd = RSTPEDI;
}

/*
 * Called upon a level 5 interrupt from luna88k/machdep.c
 */
void utintr(void)
{
    struct uart_device volatile *uartaddr;
    int line;


    if (!uart_init){
	uartaddr = uart_addr[0];
	uartaddr->cmd = RSTINT;
	return;
    }

    for (line=0 ; line < UART_COUNT; line++)
    {
        uartaddr = uart_addr[line];
	if (uartaddr->status & RXRDY && !(uartaddr->status & RCVBRK))
	    utrint(line);
	if (uartaddr->status & TXRDY)
	    utxint(line);
	if(uartaddr->status & RCVBRK)
	    uart_brk[line]++;
	uartaddr->cmd = RSTESIN;
    }
}

void utinit(void)
{
    int line;
    int wr;
    struct uart_device volatile *uartaddr = (struct uart_device *)&wr;
    unsigned s;
    extern clev_tty;

    delay_in_microseconds(3200);	/* 0.003200 seconds */
    (*(int *)uartaddr)++;
    s = spl7();
    if (uart_init == 0){
	for (line = 0; line < UART_COUNT; line++) {
	    uartaddr = uart_addr[line];	
	    for (wr = 0 ; wr < NWR ; wr++ )
		lsiprm[line][wr] = 0; 
	    uartaddr->cmd = CHANRST;              
	}
	uart_init = ON;
    }
    utlsi(0,WR2,WR2DATA,ON);
    utlsi(1,WR2,0 ,OFF);
    for (line = 0; line < UART_COUNT; line++) {
	utlsi(line,WR4,ONESB|WR4DATA ,ON);
	utlsi(line,WR3,BITS8|RXEN ,ON);
	utlsi(line,WR5,(BITS8>>1)|TXEN|RTS ,ON);
    }
    uartaddr = uart_addr[0];
    while ((uartaddr->status&INTPEND) == INTPEND)
	uartaddr->cmd = RSTINT;
    uartaddr->cmd = RSTPEDI;
    uartaddr = uart_addr[1];
    uartaddr->cmd = RSTPEDI;
    splx(s);
}

void msparam(void)
{
    int speed;

    lsiprm[ldevtop(KBD)][WR4] = 0;
    lsiprm[ldevtop(KBD)][WR3] = 0;
    lsiprm[ldevtop(KBD)][WR5] = 0;
    speed = uart_speeds[B9600];
    utlsi(ldevtop(KBD),WR4,(speed|ONESB|WR4DATA),ON);
    utlsi(ldevtop(KBD),WR3,(RXEN|BITS8),ON);
    utlsi(ldevtop(KBD),WR5,(TXEN|BITS8>>1),ON);
    lsiprm[ldevtop(KBD)][WR1] = 0;
    utlsi(ldevtop(KBD),WR1,RXIE|TXIE,ON);
}
