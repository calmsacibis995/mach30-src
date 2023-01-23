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
 * $Log:	db_interface.c,v $
 * Revision 2.12  93/03/09  17:56:30  danner
 * 	Added "gimmeabreak" boot arg.
 * 	[93/03/09            jfriedl]
 * 
 * 	Added /t option to show_translation.
 * 	Added db_task_name to showalltasks/u.
 * 	[93/02/22            jfriedl]
 * 
 * 	addr_mod in luna_dmx_print was opposite.
 * 	[93/02/20            jfriedl]
 * 
 * Revision 2.11  93/01/26  18:02:37  danner
 * 	ANSIfied, reorginized. Cleaned up includes.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.10  93/01/14  17:38:02  danner
 * 	Removed max-width restrictions when raw_printf used.
 * 	Added quiet_db_read_bytes (currently used only in db_trace).
 * 	Some gcc -Wall cleanup..
 * 	Added /h (help) to "mach regs" command.
 * 	Added /h (help) and /f (force) to "mach frame" command.
 * 	Added luna_db_translate ("mach trans" command).
 * 	[92/12/07            jfriedl]
 * 
 * 	Renamed MD command "registers" to "regs".
 * 	Removed 2.5 support (was unused).
 * 	[92/08/10            jfriedl]
 * 
 *   PREVIOUS HISTORY FLUSHED.
 *	Most real work done by danner.
 *
 * 	Created
 * 	[91/01/08            danner]
 * 
 */

/*
 * luna interface to ddb debugger
 */
#include <luna88k/machdep.h>
#include <luna88k/rom1ep.h>		 /* CONSON, CONSOFF, RVPtr      */
#include <luna88k/locore.h>     	 /* USERMODE                    */
#include <luna88k/pio.h>		 /* POWERDOWN                   */
#include <machine/db_machdep.h>		 /* local ddb stuff             */
#include <machine/thread.h>		 /* current_thread()            */
#include <ddb/db_command.h>		 /* db_error(), db_command      */
#include <ddb/db_output.h>		 /* db_printf()                 */
#include <ddb/db_sym.h>			 /* db_find_xtrn_sym_and_offset */
#include <ddb/db_run.h>			 /* STEP_RUN, etc.              */
#include <kern/thread.h>		 /* active_threads[]		*/
#include <motorola/m88k/m88100/m882xx.h> /* CMMU stuff                  */

typedef db_regs_t *db_regs_p;

/* state pointers for the exception frame */
db_regs_t db_push;
db_regs_p db_cur_exc_frame[NCPUS] = {0,0,0,0};
db_regs_p db_master_frame = 0;

simple_lock_data_t ddb_entry_lock;
simple_lock_data_t ddb_softcall_lock;
simple_lock_data_t ddb_print_lock;

int ddb_master = -1;
int ddb_depth  = 0;
volatile int db_barrier[3]={0,0,0};
volatile int db_frozen[NCPUS] = {0,0,0,0};
int bwait=0;
int db_noisy = 0;
int db_console_grabbed;
int db_was_console_on;
int quiet_db_read_bytes = 0;
int db_wait = 8;

static int freeze_stack[10];
static int freeze_depth = 0;

extern int pmap_initialized;
extern unsigned int db_maxoff;
extern int rawmem_size;
extern simple_lock_data_t cmmu_cpu_lock; /* "motorola/m88k/m88100/cmmu.c" */


/************************/
/* PRINTING *************/
/************************/

static int luna_db_print_lock_acquire(void)
{
    int count = 0;
    int got;
    /*
     * Try to get the print lock, but give up after 5 megacycles -
     * about a second
     */

    while (!(got = db_simple_lock_try(&ddb_print_lock)) && count < 5*1024*1024)
    {
	while (db_simple_lock_held(&ddb_print_lock) && count < 5*1024*1024)
	    count++;
	count++;
    }

    return got;
}

static int luna_db_print_lock_release(int held)
{
    if (held)
	db_simple_unlock(&ddb_print_lock);
    return 0;
}

static void luna_db_str(char *str)
{
    int acq = luna_db_print_lock_acquire();

    db_printf(str);
    luna_db_print_lock_release(acq);
}

static void luna_db_str1(char *str, natural_t arg1)
{
    int acq = luna_db_print_lock_acquire();

    db_printf(str, arg1);
    luna_db_print_lock_release(acq);
}

static void luna_db_str2(char *str, natural_t arg1, natural_t arg2)
{
    int acq = luna_db_print_lock_acquire();

    db_printf(str, arg1, arg2);
    luna_db_print_lock_release(acq);
}

/************************/
/* LUNA_DB_REGISTERS ****/
/************************/

/*
 *
 * If you really feel like understanding the following procedure and
 * macros, see pages 6-22 to 6-30 (Section 6.7.3) of
 *
 * MC881000 RISC Microprocessor User's Manual Second Edition
 * (Motorola Order: MC88100UM/AD REV 1)
 *
 * and ERRATA-5 (6-23, 6-24, 6-24) of
 *
 * Errata to MC88100 User's Manual Second Edition MC88100UM/AD Rev 1
 * (Oct 2, 1990)
 * (Motorola Order: MC88100UMAD/AD)
 *
 */

/* macros for decoding dmt registers */

#define XMEM(x)  ((x) & (1<<12))
#define XMEM_MODE(x) ((((x)>>2 & 0xf) == 0xf) ? "" : ".bu")
#define MODE(x) ((x)>>2 & 0xf)
#define DOUB(x) ((x) & (1<<13))
#define SIGN(x) ((x) & (1<<6))
#define DAS(x) (((x) & (1<<14)) ? "" : ".usr")
#define REG(x) (((x)>>7) & 0x1f)
#define STORE(x) ((x) & 0x2)

/*
 * return 1 if the printing of the next stage should be surpressed
 */
static int luna_dmx_print(unsigned t, unsigned d, unsigned a, unsigned no)
{
    static unsigned addr_mod[16] = { 0, 3, 2, 2, 1, 0, 0, 0,
				     0, 0, 0, 0, 0, 0, 0, 0};
    static char *mode[16]  = { "?", ".b", ".b", ".h", ".b", "?", "?", "?",
			      ".b", ".h", "?" , "?" , "?" , "?", "?", ""};
    static unsigned mask[16] = { 0,           0xff,        0xff00,     0xffff,
				 0xff0000,    0,           0,          0,
				 0xff000000U, 0xffff0000U, 0,          0,
				 0,           0,           0,    0xffffffffU};
    static unsigned shift[16] = { 0,  0, 8, 0, 16, 0, 0, 0,
				 24, 16, 0, 0,  0, 0, 0, 0};
    int reg = REG(t);

    if (XMEM(t))
    {
	db_printf("xmem%s%s r%d(0x%x) <-> mem(0x%x),",
	    XMEM_MODE(t), DAS(t), reg,
	    (((t)>>2 & 0xf) == 0xf) ? d : (d & 0xff), a );
	return 1;
    }
    else
    {
	if (MODE(t) == 0xf)
	{
	    /* full or double word */
	    if (STORE(t))
		if (DOUB(t) && no == 2)
		    db_printf("st.d%s -> mem(0x%x) (** restart sxip **)",
			DAS(t), a);
		else
		    db_printf("st%s (0x%x) -> mem(0x%x)", DAS(t), d, a);
	    else /* load */
		if (DOUB(t) && no == 2)
		    db_printf("ld.d%s r%d <- mem(0x%x), r%d <- mem(0x%x)",
			DAS(t), reg, a, reg+1, a+4);
		else
		    db_printf("ld%s r%d <- mem(0x%x)",  DAS(t), reg, a);
	}
	else
	{
	    /* fractional word - check if load or store */
	    a += addr_mod[MODE(t)];
	    if (STORE(t))
		db_printf("st%s%s (0x%x) -> mem(0x%x)", mode[MODE(t)], DAS(t),
		(d & mask[MODE(t)]) >> shift[MODE(t)], a);
	    else
		db_printf("ld%s%s%s r%d <- mem(0x%x)",
		mode[MODE(t)], SIGN(t) ? "" : "u", DAS(t), reg, a);
	}
    }
    return 0;
}

static void luna_db_print_frame(
    db_expr_t addr,
    boolean_t have_addr,
    int count,
    char *modif)
{
    struct m88100_saved_state *s = (struct m88100_saved_state *)addr;
    char *name;
    db_expr_t offset;
    int surpress1 = 0, surpress2 = 0;
    int c, force = 0, help = 0;

    if (!have_addr) {
	db_printf("requires address of frame\n");
	help = 1;
    }

    while (modif && *modif) switch (c = *modif++, c) {
	case 'f': force = 1; break;
	case 'h': help = 1; break;
        default:
	    db_printf("unknown modifier [%c]\n", c);
	    help = 1;
	    break;
    }

    if (help) {
	db_printf("usage: mach frame/[f] ADDRESS\n");
	db_printf("  /f force printing of insane frames.\n");
	return;
    }

    if (badwordaddr((vm_offset_t)s) ||
	badwordaddr((vm_offset_t)(&((db_regs_t*)s)->mode))) {
	    db_printf("frame at 0x%08x is unreadable\n", s);
	    return;
    }

    if (!frame_is_sane(s))  /* see db_trace.c */
    {
	db_printf("frame seems insane (");

	if (force)
	    db_printf("forging ahead anyway...)\n");
	else {
	    db_printf("use /f to force)\n");
	    return;
	}
    }

#define R(i) s->r[i]
#define IPMASK(x) ((x) &  ~(3))
    db_printf("R00-05: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	R(0),R(1),R(2),R(3),R(4),R(5));
    db_printf("R06-11: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	R(6),R(7),R(8),R(9),R(10),R(11));
    db_printf("R12-17: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	R(12),R(13),R(14),R(15),R(16),R(17));
    db_printf("R18-23: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	R(18),R(19),R(20),R(21),R(22),R(23));
    db_printf("R24-29: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	R(24),R(25),R(26),R(27),R(28),R(29));
    db_printf("R30-31: 0x%08x  0x%08x\n",R(30),R(31));

    db_printf("sxip: 0x%08x ",s->sxip);
    db_find_xtrn_sym_and_offset((db_addr_t) IPMASK(s->sxip),&name,&offset);
    if (name!= 0 && (unsigned)offset <= db_maxoff)
	db_printf("%s+0x%08x",name,(unsigned)offset);
    db_printf("\n");
    if (s->snip != s->sxip+4)
    {
	db_printf("snip: 0x%08x ",s->snip);
	db_find_xtrn_sym_and_offset((db_addr_t) IPMASK(s->snip),&name,&offset);
	if (name!= 0 && (unsigned)offset <= db_maxoff)
	    db_printf("%s+0x%08x",name,(unsigned)offset);
	db_printf("\n");
    }
    if (s->sfip != s->snip+4)
    {
	db_printf("sfip: 0x%08x ",s->sfip);
	db_find_xtrn_sym_and_offset((db_addr_t) IPMASK(s->sfip),&name,&offset);
	if (name!= 0 && (unsigned)offset <= db_maxoff)
	    db_printf("%s+0x%08x",name,(unsigned)offset);
	db_printf("\n");
    }

    db_printf("vector: 0x%02x                    interrupt mask: 0x%08x\n",
	s->vector, s->mask >> 8);
    db_printf("epsr: 0x%08x                current thread: 0x%x\n",
	s->epsr, current_thread());

    /*
     * If the vector indicates trap, instead of an exception or
     * interrupt, skip the check of dmt and fp regs.
     *
     * Interrupt and exceptions are vectored at 0-10 and 114-127.
     */

    if (!(s->vector <= 10 || (114 <= s->vector && s->vector <= 127)))
    {
	db_printf("\n\n");
	return;
    }

    if (s->vector == /*data*/3 || s->dmt0 & 1)
    {
	db_printf("dmt,d,a0: 0x%08x  0x%08x  0x%08x ",s->dmt0,s->dmd0,s->dma0);
	db_find_xtrn_sym_and_offset((db_addr_t) s->dma0,&name,&offset);
	if (name!= 0 && (unsigned)offset <= db_maxoff)
	    db_printf("%s+0x%08x",name,(unsigned)offset);
	db_printf("\n          ");
	surpress1 = luna_dmx_print(s->dmt0|0x01, s->dmd0, s->dma0, 0);
	db_printf("\n");

	if ((s->dmt1 & 1) && (!surpress1))
	{
	    db_printf("dmt,d,a1: 0x%08x  0x%08x  0x%08x ",s->dmt1, s->dmd1,s->dma1);
	    db_find_xtrn_sym_and_offset((db_addr_t) s->dma1,&name,&offset);
	    if (name!= 0 && (unsigned)offset <= db_maxoff)
		db_printf("%s+0x%08x",name,(unsigned)offset);
	    db_printf("\n          ");
	    surpress2 = luna_dmx_print(s->dmt1, s->dmd1, s->dma1, 1);
	    db_printf("\n");

	    if ((s->dmt2 & 1) && (!surpress2))
	    {
		db_printf("dmt,d,a2: 0x%08x  0x%08x  0x%08x ",s->dmt2, s->dmd2,s->dma2);
		db_find_xtrn_sym_and_offset((db_addr_t) s->dma2,&name,&offset);
		if (name!= 0 && (unsigned)offset <= db_maxoff)
		    db_printf("%s+0x%08x",name,(unsigned)offset);
		db_printf("\n          ");
		(void) luna_dmx_print(s->dmt2, s->dmd2, s->dma2, 2);
		db_printf("\n");
	    }
	}
    }

    if (s->fpecr & 255) /* floating point error occured */
    {
	db_printf("fpecr: 0x%08x fpsr: 0x%08x fpcr: 0x%08x\n",
	    s->fpecr,s->fpsr,s->fpcr);
	db_printf("fcr1-4: 0x%08x  0x%08x  0x%08x  0x%08x\n",
	    s->fphs1, s->fpls1, s->fphs2, s->fpls2);
	db_printf("fcr5-8: 0x%08x  0x%08x  0x%08x  0x%08x\n",
	    s->fppt,  s->fprh,  s->fprl,  s->fpit);
    }
    db_printf("\n\n");
}

/*
 * If HAVE_ADDR is true,  ADDR is cpu number of frame to dump.
 * Otherwise use ddb master cpu.
 */
static void luna_db_registers(
    db_expr_t addr,
    boolean_t have_addr,
    int count,
    char *modif)
{
    int cpu = ddb_master;
    int all = 0;
    unsigned char c;

    while (modif && *modif) switch (c = *modif++, c) {
	case 'a':  all = 1; break;
	default:
	    db_printf("unknown modifier [%c]\n", c);
	    /* FALLTHROUGH*/
	case 'h':
	    db_printf("usage: mach regs/[a] [CPU#]\n");
	    db_printf("  If CPU# not given, ddb master cpu's regs shown\n");
	    db_printf("    /a - show all cpu's registers\n");
	    return;
    }

    if (all) {
	for (cpu = 0; cpu < NCPUS; cpu++)
	    if (machine_slot[cpu].is_cpu)
	        luna_db_print_frame((db_expr_t)db_cur_exc_frame[cpu], TRUE,0,0);
	return;
    }

    if (have_addr)
	if (addr >= 0 && addr < NCPUS && machine_slot[addr].is_cpu)
	    cpu = addr;
	else {
	    db_printf("bad cpu number %d passed to luna show reg\n", addr,cpu);
	    return;
	}

    luna_db_print_frame((db_expr_t)db_cur_exc_frame[cpu], TRUE,0,0);
}

/************************/
/* PAUSE ****************/
/************************/

/*
 * pause for 2*ticks many cycles
 */
static void luna_db_pause(unsigned volatile ticks)
{
    while (ticks)
	ticks -= 1;
    return;
}


/************************/
/* CONSOLE/PALETTE ******/
/************************/

/*
 * When bm_set_palette_colors() is used to set the foreground and
 * background colors, it can return the old values of those colors.
 * However, only for 8-plane boards. Therefore, we'll stuff some valid
 * values in here now so that we'll have something from which to restore
 * the palette for one and four plane boards.
 *
 * The default X colormap defaults to have white and black preallocated
 * to indicies #0 and #1, so we'll pre-load ddb_saved_palettte with
 * white and black.  The 8-plane board will overwrite these, but they'll
 * be here for when the 4-plane board needs to restore the palette....
 */
static unsigned char ddb_saved_palette[2][3] = {
    {/*WHITE*/0xff, 0xff, 0xff},
    {/*BLACK*/0x00, 0x00, 0x00}
};

/* get a hold of the console - via rom call (see rom1ep.h) */
static int luna_db_grab_console(void)
{
    int acq = luna_db_print_lock_acquire();
    int i = 0;

    bm_set_direct_framebuffer_access();
    if (i = CONSON(), !i)
    {
        bm_set_palette_colors(/*put previous values in */ddb_saved_palette,
		/* foreground R,G,B */ 9, 7, 15,	/* purple */
		/* background R G B */ 0, 0, 0); 	/* black  */
	bm_align_screen();
    }

    luna_db_print_lock_release(acq);
    db_was_console_on = i;
    return i;
}

static void luna_db_release_console(void)
{
    if (db_console_grabbed && !db_was_console_on)
    {
	CONSOFF();
        bm_set_palette_colors(/*put previous values in */0,
		ddb_saved_palette[0][0],
		ddb_saved_palette[0][1],
		ddb_saved_palette[0][2],
		ddb_saved_palette[1][0],
		ddb_saved_palette[1][1],
		ddb_saved_palette[1][2]);
	bm_align_screen();
    }
}

void db_printing(void)
{
    if (db_console_grabbed == FALSE)
    {
	db_console_grabbed = TRUE;
	luna_db_grab_console();
    }
}

static void luna_db_clear_screen()
{
    /* this clears the screen of garbage, then redraws the console text */
    CONSOFF();
    CONSON();
    bm_align_screen();
}


/************************/
/* FREEZE/THAW **********/
/************************/

static void luna_db_freeze_cpu(db_expr_t addr, boolean_t have_addr)
{
    if (!have_addr)
    {
	db_printf("freeze requires cpu number as argument\n");
	return;
    }

    if (addr >= 0 && addr < NCPUS && machine_slot[addr].is_cpu)
	if (db_frozen[addr])
	    db_printf("cpu %d already frozen\n",addr);
	else
	{
	    db_printf("freezing cpu %d\n",addr);
	    db_frozen[addr] = 1;
	}
    else
	db_printf("cpu number %d invalid\n",addr);
}

static void luna_db_thaw_cpu(db_expr_t addr, boolean_t have_addr)
{
    if (!have_addr)
    {
	db_printf("thaw requires cpu number as argument\n");
	return;
    }

    if (addr >= 0 && addr < NCPUS && machine_slot[addr].is_cpu)
    if (!db_frozen[addr])
	db_printf("cpu %d already thawed\n",addr);
    else
    {
	db_printf("thawing cpu %d\n",addr);
	db_frozen[addr] = 0;
    }
    else
	db_printf("cpu number %d invalid\n",addr);
}

static void luna_db_status_cpu(void)
{
    int i;

    for (i=0; i<NCPUS; i++)
	if (machine_slot[i].is_cpu)
	    db_printf("cpu %d: %s ",i,db_frozen[i] ? "frozen" : "thawed");
	else
	    db_printf("cpu %d: missing",i);
    db_printf("\n\n");
    return;
}

static void luna_db_hold_down(void)
{
    int cpu = cpu_number();
    int i;

    freeze_stack[freeze_depth] = 0;
    if (!(db_run_mode==STEP_NONE || db_run_mode == STEP_CONTINUE))
    {
	/* freeze the other cpus */
	for (i=0; i<NCPUS; i++)
	    if (i != cpu)
	    {
		freeze_stack[freeze_depth] |= db_frozen[i] << i;
		db_frozen[i] = 1;
	    }
	    else
	    {
		freeze_stack[freeze_depth] |= db_frozen[i] << i;
		db_frozen[i] = 0;
	    }
	freeze_depth++;
	NOISY(luna_db_str1("(Because in step mode is nonzero, freezing all cpus except %d)\n", cpu);)
    }
    else
    {
	/* store idempotently */
	for (i=0; i<NCPUS; i++)
	    freeze_stack[freeze_depth] |= db_frozen[i] << i;
	freeze_depth++;
    }
}

static void luna_db_release(void)
{
    int i;
    if (freeze_depth)
    {
	freeze_depth--;
	for (i=0; i<NCPUS; i++)
	    db_frozen[i] = (freeze_stack[freeze_depth] & 1<<i) ? 1:0;
    }
}

static void luna_db_trap(int type, int code, int soft)
{
    int cpu = cpu_number();

    if (db_spl() != 0x03ffff00 /* splhigh */)
	luna_db_str2("WARNING: (cpu %d) spl is not high in luna_db_trap (spl=%x)\n", cpu,db_spl());

    if (db_are_interrupts_disabled())
	luna_db_str1("WARNING: (cpu %d) entered debugger with interrupts disabled\n", cpu);

    db_simple_lock(&ddb_entry_lock);

    if (ddb_master == -1 && !soft)
    {
	ddb_master = cpu;
	ddb_depth = 1;
    }
    else
	if (ddb_master == cpu)

    ddb_depth++;
    db_simple_unlock(&ddb_entry_lock);

    NOISY2(luna_db_str1(cpu==ddb_master ?
	"(cpu %d) master\n" :  "(cpu %d) slave\n",cpu);)

    if (ddb_master == cpu) /* self is master */
    {
	extern void luna_reset_simple_keyboard_state(void);

	int oldbwait = bwait; /* record current barrier status */
	int i;
	int ncpus=0;
	int delay = 2;
	db_regs_t *oframe;

	db_console_grabbed = FALSE;

	for (ncpus=0, i=0; i<NCPUS; i++)
	    if (machine_slot[i].is_cpu)
		ncpus++;

	ncpus--;
	NOISY(luna_db_str1("%d slaves expected\n",ncpus);)
	bwait = 0;      /* set counter to zero */
	db_barrier[0] = 1; /* let the slaves through to register */

	i = 0;
	while(bwait != ncpus && i<db_wait)
	{
	    luna_db_pause(delay);
	    delay *= 2;
	    i++;
	}

	NOISY(luna_db_str2("%d slaves responded (count=%d) \n", bwait,i);)
	if (bwait != ncpus)
	    luna_db_str1("WARNING:{%d}  not all cpus responded\n",cpu);

	/* relock the first barrier */
	db_barrier[0] = 0;

	NOISY(luna_db_str1("cpu %d entering ddb as master\n",cpu);)
	oframe = db_master_frame;
	db_master_frame = db_cur_exc_frame[cpu];

	/* reset the frozen masks to what they should be */
	luna_db_release();
	db_task_trap(type,code,USERMODE(db_master_frame->epsr));
	/* If any of the single step flags are set,
	   we should freeze all the other cpus temporarily */
	luna_db_hold_down();

	db_master_frame = oframe;
	db_simple_lock(&ddb_entry_lock);
	NOISY2(luna_db_str("master leaving luna_db_trap\n");)
	if (--ddb_depth == 0)
	    ddb_master = -1;
	db_simple_unlock(&ddb_entry_lock);
	/* release the first barrier */

	/* lock the third, unlock the second let the slaves count clear */
	db_barrier[2] = 0; /* order is important */
	db_barrier[1] = 1;
	for (i=0, delay = 2; bwait && i<24; i++)
	{
	    luna_db_pause(delay);
	    delay *= 2;
	}

	if (bwait!=0)
	    luna_db_str1("barrier wait %d after long wait; cont. anyway\n",bwait);

	bwait = oldbwait;
	db_barrier[1] = 0; /* relock the second */
	db_barrier[2] = 1; /* unlock the third */

	luna_db_release_console();
	luna_reset_simple_keyboard_state(); /* luna88kdev/kbmain.c */
    }
    else
    {
	/* we are a slave. register our status */
	while(!db_barrier[0])
		;
	db_simple_lock(&ddb_entry_lock);
	bwait++;
	db_simple_unlock(&ddb_entry_lock);
	while(!db_barrier[1])
		;
	db_simple_lock(&ddb_entry_lock);
	bwait--;
	db_simple_unlock(&ddb_entry_lock);
	while(!db_barrier[2])
		;
    }
}

/*
 * when the below routine is entered interrupts should be on
 * but spl should be high
 */
static void luna_db_freeze(int trap_type)
{
    int cpu = cpu_number();

    while (db_frozen[cpu])
    {
	/* if we are frozen just chill here until db_barrier[0] opens,
	 then drop into the debugger as a slave. */
	NOISY2(luna_db_str1("(cpu %d) freezing\n",cpu);)
	while(!db_barrier[0])
	    ;
	luna_db_trap(trap_type,0,1);
    }
}

/************************/
/* DEBUGGER ENTRY *******/
/************************/

/*
 * send soft interrupt to every other cpu
 * besides self and frozen cpus.
 *
 * Not sure what spl status one should have when
 * this is invoked.
 */
static void luna_db_signal_cpus(int self)
{
    int i;

    for (i = 0; i < NCPUS; i++)
    {
	if (i != self && machine_slot[i].is_cpu && db_frozen[i]==0)
	{
	    /* not self, unfrozen */
	    RVPtr->EPsoft_intr_rom[i] = LUNA_DDB_SOFTCALL;
	    switch(i)
	    {
	      case 0:
		*((volatile int *) SOFT_INT0) = 0;
		break;
	      case 1:
		*((volatile int *) SOFT_INT1) = 0;
		break;
	      case 2:
		*((volatile int *) SOFT_INT2) = 0;
		break;
	      case 3:
		*((volatile int *) SOFT_INT3) = 0;
		break;
	    }
	}
    }
}

/* gimmeabreak - drop execute the ENTRY trap */
void gimmeabreak(void)
{
    asm (ENTRY_ASM); /* entry trap */
    /* ends up at ddb_entry_trap below */
}

/* fielded a non maskable interrupt - from ext_int (machdep.c) */
int ddb_nmi_trap(int level, db_regs_t *eframe)
{
    db_regs_t *oldframe;
    int cpu = cpu_number();

    NOISY(luna_db_str1("kernel: nmi interrupt cpu %d\n",cpu);)
    oldframe = db_cur_exc_frame[cpu];
    db_cur_exc_frame[cpu] = eframe;
    luna_db_trap(T_KDB_ENTRY,0,0);
    luna_db_freeze(T_KDB_ENTRY);
    db_cur_exc_frame[cpu] = oldframe;

    return 0;
}

/*
 * when the below routine is entered interrupts should be on
 * but spl should be high
 */

/* softcall entry - always a slave */
int ddb_softcall_trap(int level, db_regs_t *eframe)
{
    db_regs_t *oldframe;
    int cpu = cpu_number();

    if (!db_simple_lock_held(&ddb_softcall_lock))
    {
	/* the softcall lock is not held; this means
	   this  call is spurious; just return */
	NOISY2(luna_db_str1("kernel: spurious softcall on cpu %d\n",cpu);)
    }
    else
    {
	NOISY(luna_db_str1("kernel: softcall interrupt cpu %d\n",cpu);)
	oldframe = db_cur_exc_frame[cpu];
	db_cur_exc_frame[cpu] = eframe;
	luna_db_trap(T_KDB_ENTRY,0,1);
	luna_db_freeze(T_KDB_ENTRY);
	db_cur_exc_frame[cpu] = oldframe;
    }
    return 0;
}

/*
 * When the below routine is entered interrupts should be on
 * but spl should be high
 *
 * The following routine is for breakpoint and watchpoint entry.
 * When a processor hits this, it needs to do a software interrupt
 * (softcall) to halt the other processors. But since multiple threads
 * could have hit breakpoints, protect the softcall by a lock. So if
 * a processor hits here and is unable to grab the lock, that means another
 * processor has hit a breakpoint/watchpoint and already emitted the
 * softcall (or is in the process of doing so).
 *
 * In the case that the softcall lock is already held, the processor
 * should drop into the kernel as slave; when it recieves the softcall
 * interrupt latter, it will notice that the softcall lock is not held
 * and just ignore the interrupt and continue. (See above).
 *
 */

/* breakpoint/watchpoint entry */
int ddb_break_trap(int type, db_regs_t *eframe)
{
    db_regs_t *oldframe;
    int cpu = cpu_number();

    NOISY(luna_db_str1("kernel: break/watch/ cpu %d\n",cpu);)
    oldframe = db_cur_exc_frame[cpu];
    db_cur_exc_frame[cpu] = eframe;
    if (db_simple_lock_try(&ddb_softcall_lock))
    {
	/* we got the lock - signal other cpus */
#if 0
	luna_db_str2("cpu %d breakpoint trap, frame = 0x%x\n", cpu,(int)eframe);
#endif
	luna_db_signal_cpus(cpu); /* halt other cpus */
	luna_db_trap(type,0,0);
	db_simple_unlock(&ddb_softcall_lock);
    }
    else
	luna_db_trap(type,0,1); /* enter as slave */

    luna_db_freeze(type);
    db_cur_exc_frame[cpu] = oldframe;
    if (type == T_KDB_BREAK)
    {
	/* back up an instruction and retry the instruction at the
	   breakpoint address */
	eframe->sfip = eframe->snip;
	eframe->snip = eframe->sxip;
    }

    return 0;
}

/* enter at splhigh */
int ddb_entry_trap(int level, db_regs_t *eframe)
{
    db_regs_t *oldframe;
    int cpu = cpu_number();

    NOISY(luna_db_str1("kernel: entry trap cpu %d\n",cpu);)
    oldframe = db_cur_exc_frame[cpu];
    db_cur_exc_frame[cpu] = eframe;
    if (db_simple_lock_try(&ddb_softcall_lock))
    {
	/* we got the lock - signal other cpus */
	luna_db_signal_cpus(cpu); /* halt other cpus */
	luna_db_trap(T_KDB_ENTRY,0,0);
	db_simple_unlock(&ddb_softcall_lock);
    }
    else
	luna_db_trap(T_KDB_ENTRY,0,1); /* enter as slave */

    luna_db_freeze(T_KDB_ENTRY);
    db_cur_exc_frame[cpu] = oldframe;
    /* unlike break traps, we need not restart the instruction */

    return 0;
}

/*
 * when the below routine is entered interrupts should be on
 * but spl should be high
 */
/* error trap - unreturnable */
void ddb_error_trap(char *error, db_regs_t *eframe)
{
    db_regs_t *oldframe;
    int cpu = cpu_number();

    luna_db_str2("KERNEL:  terminal error [%s] (cpu %d)\n",(int)error,cpu);
    luna_db_str ("KERNEL:  Exiting debugger will cause abort to rom\n");
    luna_db_str1("at 0x%x ", eframe->sxip & ~3);
    luna_db_str2("dmt0 0x%x dma0 0x%x", eframe->dmt0, eframe->dma0);
    luna_db_pause(1000000);
    oldframe = db_cur_exc_frame[cpu];
    db_cur_exc_frame[cpu] = eframe;
    if (db_simple_lock_try(&ddb_softcall_lock))
    {
	luna_db_signal_cpus(cpu); /* halt other cpus */
	luna_db_trap(T_KDB_BREAK,0,0);
	luna_db_freeze(T_KDB_BREAK);
	while(1)
	    call_rom_abort(2, (unsigned *)eframe);
    }
    else
    {
	luna_db_str1("(cpu %d) with ERROR entering debugger as slave;\n",cpu);
	luna_db_trap(T_KDB_BREAK,0,1);
	luna_db_freeze(T_KDB_BREAK);
	while(1)
	    call_rom_abort(2, (unsigned *)eframe);
    }
}

/************************/
/* VIRTUAL -> PHYSICAL **/
/************************/

/*
 * Translate from virtual address va in task task
 * to physical address pa. Return 1 if succesfull
 * translation, 0 if not
 */
static vm_offset_t db_task_va_to_phys(
    vm_offset_t va,
    task_t task,
    vm_offset_t *pa)
{
    pmap_t pm;
    vm_offset_t p;

    if (task == TASK_NULL)
    {
	if (badaddr(va, 1))
	    return 0;
	if (!pmap_initialized)
	    *pa = va;
	else
	{
	    p = pmap_extract_unlocked(kernel_pmap,va);
	    *pa = p ? p : va;
	}
	return 1;
    }

    /* check map */
    if (badwordaddr((vm_offset_t)task->map))
	return 0;

    /* check pmap sanity */
    if (badwordaddr((vm_offset_t)task->map->pmap))
	return 0;

    pm = task->map->pmap;

    if (pm == kernel_pmap)
    {
	if (badaddr(va, 1))
	    return 0;
	p = pmap_extract_unlocked(pm,va);
	*pa = p ? p : va;
	return 1;
    }

    p = pmap_extract_unlocked(pm,va);
    if (p == 0 && va != 0) /* pmap_extract_unlocked failed */
	return 0;
    *pa = p;
    return 1;
}

/************************/
/* LUNA_PC **************/
/************************/

int luna_pc(struct m88100_saved_state *regs)
{
    if (regs->sxip & 2) /* is valid */
	return regs->sxip & ~3;
    if (regs->snip & 2)
	return regs->snip & ~3;
    if (regs->sfip & 2)
	return regs->sfip & ~3;
    /* we are in trouble - none of the program counters are valid */
    return 0;
}

/************************/
/* XXXXXXXX *************/
/************************/

/*
 * read some bytes for the debugger
 */
void db_read_bytes(char *addr, unsigned size, char *data, task_t task)
{
    char *paddr;

    while(size--)
    {
	if (!db_task_va_to_phys((vm_offset_t)addr,task,(vm_offset_t*)&paddr))
	    db_error(quiet_db_read_bytes ? 0 : "  <<translation failure>>  ");

	if (paddr < (char*)rawmem_size && pmap_initialized)
	    paddr += VEQR_ADDR; /* set pointer into physical mapping region */ 

	if (!badaddr((vm_offset_t)paddr, 1))
	    *data++ = *paddr;
	else
	    db_error(quiet_db_read_bytes ? 0 :
		"Invalid virtual address or page not resident");
	addr++;
    }
}

/*
 * write some bytes for the debugger
 */
void db_write_bytes(char *addr, unsigned size, char *data, task_t task)
{
    char *paddr;

    while(size--)
    {
	if (!db_task_va_to_phys((vm_offset_t)addr,task,(vm_offset_t*)&paddr))
	    db_error("Invalid virtual address or page not resident");

      if (paddr < (char*)rawmem_size && pmap_initialized)
	paddr += VEQR_ADDR; /* set pointer into physical mapping region */ 

	if (!badaddr((vm_offset_t)paddr, 1))
	    *paddr = *data++;
	else
	    db_error("Invalid virtual address or page not resident");
	addr++;
    }
}

/* to print a character to the console */
void db_putc(unsigned c)
{
    while (mayput(c & 0xff) == -1)
	;
    if (c == '\n')
	db_putc('\r');
}

/* to peek at the console; returns -1 if no character is there */
int db_getc(void)
{
    return mayget();
}

/* machine dependent debugger commands */
static void luna_db_rom_abort(void)
{
    call_rom_abort(2, (unsigned *)db_cur_exc_frame[cpu_number()]);
}

static volatile void luna_db_sysreset(void)
{
    db_printf("<sysreset>\n\n");
    *(volatile int *)RESET_CPU_ALL = 0;
    for(;;)
	;
}

static volatile void luna_db_poweroff(void)
{
    POWERDOWN();
    for(;;)
	;
}

/* display where all the cpus are stopped at */
static void luna_db_where(void)
{
    int i;
    struct m88100_saved_state *s;
    char *name;
    int *offset;

    for (i=0; i<NCPUS; i++)
	if (machine_slot[i].is_cpu && (s = db_cur_exc_frame[i]))
	{
	    int l = luna_pc(s); /* clear low bits */

	    db_find_xtrn_task_sym_and_offset((db_addr_t) l,&name,
	        (db_addr_t*)&offset,
		USERMODE(s->epsr) ? active_threads[i]->task : kernel_task);
	    if (name && (unsigned)offset <= db_maxoff)
		db_printf("cpu %d: stopped at 0x%x  (%s+0x%x)\n",
			i, l, name, offset);
	    else
		db_printf("cpu %d: stopped at 0x%x\n", i, l);
	}
}

/*
 * Walk back a stack, looking for exception frames.
 * These frames are recognized by the routine frame_is_sane. Frames
 * only start with zero, so we only call frame_is_sane if the
 * current address contains zero.
 *
 * If addr is given, it is assumed to an address on the stack to be
 * searched. Otherwise, r31 of the current cpu is used.
 */
static void luna_db_frame_search(db_expr_t addr, boolean_t have_addr)
{
#if 1
    db_printf("sorry, frame search currently disabled.\n");
#else
    if (have_addr)
	addr &= ~3; /* round to word */
    else
	addr = (DDB_REGS -> r[31]);

    /* walk back up stack until 8k boundry, looking for 0 */
    while (addr & ((8*1024)-1))
    {
	int i;
	db_read_bytes(addr,4,&i,kernel_task);
	if (i == 0 && frame_is_sane(i))
	    db_printf("frame found at 0x%x\n", i);
	addr += 4;
    }

    db_printf("(Walked back until 0x%x)\n",addr);
#endif
}

/* flush icache of cpu number addr. If addr unspecified use, self */
static void luna_db_iflush(db_expr_t addr, boolean_t have_addr)
{
    if (have_addr)
    {
	if (!(addr >= 0 && addr < NCPUS && machine_slot[addr].is_cpu))
	{
	    db_printf("bad cpu number %d specified\n",addr);
	    return;
	}
    }
    else
	addr = cpu_number();

    if (cmmu_cpu_lock.lock_data)
    {
	db_printf("cmmu_cpu_lock held; clear to force this operation\n");
	return;
    }

    cmmu_remote_set(addr, CMMU_SCR, 0, CMMU_FLUSH_CACHE_CBI_ALL);
}

/* flush dcache of cpu number addr. If addr unspecified use, self */
static void luna_db_dflush(db_expr_t addr, boolean_t have_addr)
{
    if (have_addr)
    {
	if (!(addr >= 0 && addr < NCPUS && machine_slot[addr].is_cpu))
	{
	    db_printf("bad cpu number %d specified\n",addr);
	    return;
	}
    }
    else
	addr = cpu_number();

    if (cmmu_cpu_lock.lock_data)
    {
	db_printf("cmmu_cpu_lock held; clear to force this operation\n");
	return;
    }

    cmmu_remote_set(addr, CMMU_SCR, 1, CMMU_FLUSH_CACHE_CBI_ALL);
}

/* probe my cache */
static void luna_db_peek(
    db_expr_t addr,
    boolean_t have_addr,
    int count,
    char *modif)
{
    int pa12;
    int cpu = cpu_number();
    int valmask;

    if (!have_addr)
    {
	db_printf("peek requires requires address\n");
	return;
    }

    if (cmmu_cpu_lock.lock_data)
    {
	db_printf("cmmu_cpu_lock held; clear to force this operation\n");
	return;
    }

    pa12 = addr & ~((1<<12) -1);

    /* probe dcache */
    cmmu_remote_set(cpu, CMMU_SAR, 1, addr);

    valmask = cmmu_remote_get(cpu, CMMU_CSSP, 1);
    printf("dcache valmask 0x%x\n", (unsigned)valmask);
    printf("dcache tag ports 0x%x 0x%x 0x%x 0x%x\n",
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP0, 1),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP1, 1),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP2, 1),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP3, 1));

#if 0
    if ( (pa12 == cmmu_remote_get(cpu, CMMU_CTP0, 1) &&
	(valmask & 0x3000 != 0x3000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP1, 1) &&
	(valmask & 0xb000 != 0xb000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP2, 1) &&
	(valmask & 0x30000 != 0x30000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP3, 1) &&
	(valmask & 0xb0000 != 0xb0000)) )
    {
	db_printf("line held in dcache\n");
    }
#endif

    /* probe icache */
    cmmu_remote_set(cpu, CMMU_SAR, 0, addr);

    valmask = cmmu_remote_get(cpu, CMMU_CSSP, 0);
    printf("icache valmask 0x%x\n", (unsigned)valmask);
    printf("icache tag ports 0x%x 0x%x 0x%x 0x%x\n",
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP0, 0),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP1, 0),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP2, 0),
    (unsigned)cmmu_remote_get(cpu, CMMU_CTP3, 0));

#if 0
    if ( (pa12 == cmmu_remote_get(cpu, CMMU_CTP0, 0) &&
	(valmask & 0x3000 != 0x3000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP1, 0) &&
	(valmask & 0xb000 != 0xb000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP2, 0) &&
	(valmask & 0x30000 != 0x30000)) ||
	(pa12 == cmmu_remote_get(cpu, CMMU_CTP3, 0) &&
	(valmask & 0xb0000 != 0xb0000)) )
    {
	db_printf("line held in icache\n");
    }
#endif
}


/*
 * control how much info the debugger prints about itself
 */
static void luna_db_noise(db_expr_t addr, boolean_t have_addr)
{
    if (!have_addr)
    {
	/* if off make noisy; if noisy or very noisy turn off */
	if (db_noisy)
	{
	    db_printf("changing debugger status from %s to quiet\n",
		db_noisy == 1 ? "noisy" :
		db_noisy == 2 ? "very noisy" : "violent");
	    db_noisy = 0;
	}
	else
	{
	    db_printf("changing debugger status from quiet to noisy\n");
	    db_noisy = 1;
	}
    }
    else
	if (addr < 0 || addr > 3)
	    db_printf("invalid noise level to luna_db_noisy; should be 0, 1, 2, or 3\n");
	else
	{
	    db_noisy = addr;
	    db_printf("debugger noise level set to %s\n",
		db_noisy == 0 ? "quiet" :
		(db_noisy == 1 ? "noisy" :
		db_noisy==2 ? "very noisy" : "violent"));
	}
}

/*
 * Check access to that address in user space on the
 * current processor.
 */

unsigned int db_check_user_addr(vm_offset_t va)
{
    vm_offset_t pa;
    return db_task_va_to_phys(va, current_thread()->task, &pa);
}

/*
 * Check if the two (task,va) pairs map to the same
 * address
 */
unsigned int db_phy_eq(task_t t1, task_t t2, vm_offset_t va1, vm_offset_t va2)
{
    vm_offset_t pa1, pa2;
    if (db_task_va_to_phys(va1,t1,&pa1) &&
	db_task_va_to_phys(va2,t2,&pa2) &&
	(pa1 == pa2))
	    return 1;
    return 0;
}

unsigned int db_check_access(task_t task, vm_offset_t va, unsigned int size)
{
    vm_offset_t pa;

    while (size)
    {
	if (!db_task_va_to_phys(va,task,&pa))
	    return 0;
	size--;
	va++;
    }
    return 1;
}

/*
 * See how a virtual address translates.
 * Must have an address.
 * If count is given, taken as a CMMU number. Otherwise (count will be -1),
 * the data CMMU for the current cpu is used.
 */
static void luna_db_translate(
    db_expr_t addr,
    boolean_t have_addr,
    unsigned count,
    char *modif)
{
    char c;
    int verbose_flag = 0;
    int supervisor_flag = 1;
    int wanthelp = 0;
    thread_t thread = 0;

    if (!have_addr)
	wanthelp = 1;
    else while (c = *modif++, c != 0) switch (c) {
      default:
	db_printf("bad modifier [%c]\n", c); 
	wanthelp = 1;
	break;
      case 'h':
	wanthelp = 1;
	break;
      case 'v':
	verbose_flag++;
	break;
      case 's':
	supervisor_flag = 1;
	break;
      case 't':
       if (!db_get_next_thread(&thread, 0))
	    return;
	break;
      case 'u':
	supervisor_flag = 0;
	break;
    }

    if (wanthelp) {
	db_printf("usage: translate[/vvsu] address [,cmmu#] [thread]\n");
	db_printf("If no CMMU# given, current cpu's data cmmu used.\n");
	db_printf("flags: v - be verbose (vv - be very verbose)\n");
	db_printf("       s - use cmmu's supervisor area pointer (default)\n");
	db_printf("       u - use cmmu's user area pointer\n");
	db_printf("       t - use thread's user area pointer [implies /u]\n");
	return;
    }

    cmmu_show_translation(addr, supervisor_flag, verbose_flag, count, thread);
}

void kdb_kintr(void)
{}

void cpu_interrupt_to_db(int cpu_no)
{}


/************************/
/* COMMAND TABLE / INIT */
/************************/

static struct db_command luna_cpu_cmds[] =
{
    {"freeze",    luna_db_freeze_cpu,0, 0},
    {"thaw",      luna_db_thaw_cpu,  0, 0},
    {"status",    luna_db_status_cpu, 0, 0},
    {(char  *) 0,}
};

static struct db_command luna_cache_cmds[] =
{
    { "iflush",    luna_db_iflush, 0, 0},
    { "dflush",    luna_db_dflush, 0, 0},
    { "peek",      luna_db_peek, 0, 0},
    { (char *) 0,}
};

struct db_command db_machine_cmds[] =
{
    {"cpu",		0,			0, luna_cpu_cmds},
    {"cache",		0,			0, luna_cache_cmds},
    {"POWEROFF",	luna_db_poweroff,	0, 0},
    {"RESET",		luna_db_sysreset,  	0, 0},
    {"abort",		luna_db_rom_abort,	0, 0},
    {"cls",	        luna_db_clear_screen,	0, 0},
    {"frame",		luna_db_print_frame,	0, 0},
    {"noise",		luna_db_noise,		0, 0},
    {"regs",		luna_db_registers,	0, 0},
    {"rom",		luna_db_rom_abort,	0, 0},
    {"searchframe",	luna_db_frame_search,	0, 0},
    {"translate",	luna_db_translate,      0, 0},
    {"where",		luna_db_where,		0, 0},
    {(char  *) 0,}
};

/*
 * Called from "luna88k/luna_init.c"
 */
void kdb_init(void)
{
    simple_lock_init(&ddb_entry_lock);
    simple_lock_init(&ddb_softcall_lock);
    simple_lock_init(&ddb_print_lock);
#ifdef DB_MACHINE_COMMANDS
    db_machine_commands_install(db_machine_cmds);
#endif
    ddb_init();

    /*
     * if dipswitch 2 (#1) is set, we're going across the serial line and
     * so don't want any artificial line wrap... we're probably going
     * to an X window with a scrollbar....
     */
    if (diptest(1)) {
	extern int db_max_width;
	extern int db_max_line;
	db_max_width = 1024;
	db_max_line = 128;
    }

    printf("ddb enabled\n");
    /*
     * if dipswitch 4 (#3) is set, or if the "gimmeabreak" boot-time option has been given,
     * trap into the debugger via gimmeabreak
     */
    if (diptest(3) || bootarg("gimmeabreak"))
	gimmeabreak();
}

/*
 * Attempt to figure out the UX name of the task.
 * This is kludgy at best... we can't even be sure the task is a UX task...
 */
#define TOP_OF_USER_STACK 0xffbffb40
#define MAX_DISTANCE_TO_LOOK (1024 * 10)

#define DB_TASK_NAME_LEN 50

char *db_task_name(task_t task)
{
    static unsigned buffer[(DB_TASK_NAME_LEN + 5)/sizeof(unsigned)];
    unsigned ptr = (vm_offset_t)(TOP_OF_USER_STACK - 4);
    unsigned limit = ptr - MAX_DISTANCE_TO_LOOK;
    unsigned word;
    int i;

    /* skip zeros at the end */
    while (ptr > limit &&
	   (i = db_trace_get_val((vm_offset_t)ptr, &word, task))
	   && (word == 0))
    {
	ptr -= 4; /* continue looking for a non-null word */
    }

    if (ptr <= limit) {
	db_printf("bad task name at line %d\n", __LINE__);
	return "<couldn't find 1>";
    } else if (i != 1) {
 	return "<nostack>";
    }

    /* skip looking for null before all the text */
    while (ptr > limit
	&&(i = db_trace_get_val(ptr, &word, task))
    	&& (word != 0))
    {
	ptr -= 4; /* continue looking for a null word */
    }
    
    if (ptr <= limit) {
	db_printf("bad task name at line %d\n", __LINE__);
	return "<couldn't find 2>";
    } else if (i != 1) {
	db_printf("bad task name read of %x from task %x "
	          "at line %d\n", ptr, task, __LINE__);
 	return "<bad read 2>";
    }

    ptr += 4; /* go back to the non-null word after this one */

    for (i = 0; i < sizeof(buffer); i++, ptr+=4) {
	buffer[i] = 0; /* just in case it's not read */
	db_trace_get_val((vm_offset_t)ptr, &buffer[i], task);
    }
    return (char*)buffer;
}
