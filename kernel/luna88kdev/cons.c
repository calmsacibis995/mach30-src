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
 * $Log:	cons.c,v $
 * Revision 2.11  93/05/17  10:27:51  rvb
 * 	Type casts, etc to quiet gcc 2.3.3 warnings
 * 
 * Revision 2.10  93/03/09  17:58:52  danner
 * 	cleanup
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.9  93/02/05  08:07:37  danner
 * 	commented-out kanji support (for now)
 * 	[93/02/04            jfriedl]
 * 
 * Revision 2.8  93/01/26  18:07:30  danner
 * 	ANSIfied.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.7  93/01/14  17:39:16  danner
 * 	Removed kanji support (to be replaced with MI support at a later date)
 * 	[92/12/10            jfriedl]
 * 
 * 	Removed unused 2.5 support.
 * 	Some Omron changes.
 * 	[92/12/08            jfriedl]
 * 
 * Revision 2.6  92/08/03  17:45:32  jfriedl
 * 	Added flexability to support /dev/ttya as the console.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.4.2.1  92/05/27  14:45:52  danner
 * 	updated headers.
 * 	[92/05/17            danner]
 * 
 * Revision 2.5  92/05/21  17:21:46  jfriedl
 * 	Cleanup to quiet gcc warnings.
 * 	Made CHAR and DEV_T args to be UNSIGNED (ANSI/std problems).
 * 	Added prototypes.
 * 	[92/05/18            jfriedl]
 * 
 * Revision 2.4  92/05/04  11:27:32  danner
 * 	Added SPLX protection in cnputc. Possibly gratuitous.
 * 	[92/05/03            danner]
 * 
 * Revision 2.3  92/02/19  14:06:23  elf
 * 	Added hack to allow a keyboard character to cause a debugger
 * 	 entry. Set the integer ddb_trap_char to the ascii code of the
 * 	 character. This character will never make it past the debugger.
 * 	[91/09/17            danner]
 * 
 * Revision 2.2  91/07/09  23:20:42  danner
 * 	Initial Checkin
 * 	[91/06/26  11:42:24  danner]
 * 
 * Revision 2.2  91/04/05  14:07:01  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */


/*
 * LUNA-88K console driver
 *	For output character, console driver uses ROM mayput routine.
 *	And this driver supports console switch function.
 */
#include <luna88k/machdep.h>
#include <sys/types.h>		/* minor, etc. */
#include <luna88k/softint.h>
#include <luna88kdev/kbms.h>
#include <luna88kdev/cons.h>
#include <luna88k/rom1ep.h>	/* mayput, etc. */
#include <device/conf.h>
#include <device/tty.h>
#include <device/io_req.h>
#include <device/buf.h>		/* for struct uio */

#ifdef MACH_KDB
  #include <machine/db_machdep.h>
#endif

#if 0
volatile struct conssw conssw = {
    utopen, utclose, utread, utwrite, 0, CONSOLE, 0, 0, 0, 0, 0 };
   #define keyboardopen conssw.kbopen
#else
int keyboardopen = 0;
#endif


struct  tty     cons_tty[3];
int     	cons_cnt = 3;

struct tty *curtty  = &cons_tty[0/*CONSOLE*/];       /* current tty struct */

extern void ttrstrt();

/*ARGSUSED*/
int cnopen(unsigned dev, int flag, io_req_t ior)
{
    if (minor(dev) == KBD) {
	if (keyboardopen)            /* dual open */
	    return(D_NO_SUCH_DEVICE);
	else {
	    int s = SPLX();
	    keyboardopen = ON;
	    curtty = &cons_tty[KBD];
	    splx(s);
	}
    }
    return utopen(dev,flag,ior);
}

/*ARGSUSED*/
int cnclose(unsigned dev)
{
    int	s = spltty();
    struct tty *tp;
    int ret;

    tp = &cons_tty[minor(dev)];
    if (minor(dev) == KBD) {
	keyboardopen = OFF;
	curtty = &cons_tty[CONSOLE];
	keyboard_setencoded();
    }

    simple_lock(&tp->t_lock);
    ret = utclose(dev);
    ttyclose(&cons_tty[minor(dev)]);
    simple_unlock(&tp->t_lock);
    splx(s);
    return ret;
}

int cnwrite(unsigned dev, struct uio *uio)
{
  return utwrite(dev, uio);
}

int cnread(unsigned dev, struct uio *uio)
{
    struct tty *tp;

    tp = &cons_tty[minor(dev)];
    tp->t_state |= TS_CARR_ON;
    return utread(dev,uio);
}

io_return_t cngetstat(
	unsigned	dev,
	int		flavor,
	int *		data,		/* pointer to OUT array */
	unsigned int	*count)		/* OUT */
{
  return tty_get_status(&cons_tty[minor(dev)], flavor, data, count);
}

io_return_t cnsetstat(
	unsigned	dev,
	int		flavor,
	int *		data,		/* pointer to OUT array */
	unsigned int	count)
{
  return tty_set_status(&cons_tty[minor(dev)], flavor, data, count);
}


static void cnxint(struct tty *tp, unsigned c);/*forward*/

static void cnxint_cons(int c)
{
    cnxint(&cons_tty[CONSOLE], c);
}

static void cnxint_term(int c)
{
    cnxint(&cons_tty[TERM], c);
}

static void cnxint_kbd(int c)
{
    cnxint(&cons_tty[KBD], c);
}

void cnstart(struct tty *tp);
static void cnxint(struct tty *tp, unsigned c)
{
    register s;
    
    s = spl6();
    if (c != 0 && mayput(c) == -1) {
	if (tp == &cons_tty[CONSOLE])
	    softcall((func_t)cnxint_cons, (vm_offset_t)c, cpu_number());
	else if (tp == &cons_tty[TERM])
	    softcall((func_t)cnxint_term, (vm_offset_t)c, cpu_number());
	else
	    softcall((func_t)cnxint_kbd, (vm_offset_t)c, cpu_number());
    }
    else {
	tp->t_state &= ~TS_FLUSH;
	if (tp->t_state & TS_BUSY) {
	    tp->t_state &= ~TS_BUSY;
	    if (tp->t_line)
		(*linesw[tp->t_line].l_start)(tp);
	    else
		cnstart(tp);
	}
    }
    splx(s);
}

void cnstart(struct tty *tp)
{
    register int s,c;
    
    s = spl6();
    if (tp->t_state & (TS_TIMEOUT|TS_BUSY|TS_TTSTOP)) 
	goto out;
    if (tp->t_outq.c_cc <= TTLOWAT(tp))
      tt_write_wakeup(tp);

    if (tp->t_outq.c_cc == 0)
	goto out;
    c = getc(&tp->t_outq);
    if ((tp->t_flags&(RAW|LITOUT|PASS8)) || c <= 0177) {
	if (tp == &cons_tty[CONSOLE])
	    softcall((func_t)cnxint_cons, (vm_offset_t)(c&0xff), cpu_number());
	else if (tp == &cons_tty[TERM])
	    softcall((func_t)cnxint_term, (vm_offset_t)(c&0xff), cpu_number());
	else
	    softcall((func_t)cnxint_kbd, (vm_offset_t)(c&0xff), cpu_number());
    }
    else {
	timeout((int(*)())ttrstrt, (vm_offset_t)tp, (c&0xff));
	tp->t_state |= TS_TIMEOUT;
	goto out;
    }
    tp->t_state |= TS_BUSY;
 out:
    splx(s);
}


#undef cnputc /* want to make sure we've got a public copy */
void cnputc(int c)
{
    if (c)
    {
	unsigned int s = SPLX();
	while (mayput(c & 0xff) == -1)
	    /*loop*/;
	if (c == '\n')
	    cnputc('\r');
	splx(s);
    }
}

#if MACH_KDB
int ddb_trap_char = -2;
#endif

#undef cngetc /* want to make sure we've got a public copy */
/*
* Get a character from console.
*/
int cngetc(void)
{
    register int s,c;
    
    s = SPLX();
    c = mayget();
#if MACH_KDB
    if ( c == ddb_trap_char)
      {
        c = -1;
        gimmeabreak();
      }
#endif
    splx(s);
    return c;
}

int cnportdeath(unsigned dev, mach_port_t port)
{
    return (tty_portdeath(&cons_tty[minor(dev)], (ipc_port_t)port));
}

/*ARGSUSED*/
int cnmmap(unsigned dev, off_t off, int prot)
{
  return -1;
}

#ifdef KANJI_STUFF
/* moved from tty.c in 2.5 */

ushort g_sjis[SJISGAS];

void j_init(struct tty *tp)
{
    static gset = 0;

    if (!gset) {		/* sift JIS gaiji address table set */
	int i;
	unsigned char c1, c2;

	for (i = 0, c1 = SJISG1S, c2 = SJISG2S; i < SJISGAS; i++) {
	    g_sjis[i] = (c1 << 8) + c2;
	    c2++;
	    if (c2 == 0x7f)
		    c2++;
	    if (c2 == SJISG2E + 1) {
		    c2 = SJISG2S;
		    c1++;
	    }

	    if (c1 > SJISG1E)
		    break;
	}
    gset = 1;
    }

#ifdef KANJI_STUFF
    tp->t_jt.j_level = 0;
    tp->t_jt.j_flg = 0;
    tp->t_jt.j_ecode = 0;
    tp->t_jt.j_jstl = 0;
    tp->t_jt.j_jendl = 0;
    tp->t_jt.j_gdspl = 0;
    tp->t_jt.j_gcsa[0] = 0;
    tp->t_jt.j_gcsa[1] = 0;
    tp->t_jt.j_firstcode = 0;
    tp->t_jt.outputp = 0;
    tp->t_jt.firstbyte = 0;
    tp->t_jt.kesccount = 0;
    tp->t_jt.inputconv = 0;
    tp->t_jt.eschit = 0;
    tp->t_jt.kesctime = 0;
    tp->t_jt.inputaddr[0] = 0;
    tp->t_jt.inputaddr[1] = 0;
    tp->t_jt.kana = 0;
    for (j = 0; j < EMAX; j++) {
	    tp->t_jt.j_jst[j] = 0;
	    tp->t_jt.j_jend[j] = 0;
	    tp->t_jt.j_gdsp[j] = 0;
    }
#endif
}
#endif /*KANJI_STUFF*/
