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
 * $Log:	machdep.c,v $
 * Revision 2.10  93/03/09  17:57:50  danner
 * 	Added stop_on_unsup_inter; cleaned nvram stuff.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.9  93/01/26  18:03:15  danner
 * 	ANSIfied, reorginized. Cleaned out cruft.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.8  93/01/14  17:38:51  danner
 * 	Added some cache flushed when starting slaves.
 * 	Some gcc -Wall cleanup.
 * 	[92/12/07            jfriedl]
 * 
 * Revision 2.7  92/08/03  18:14:06  jfriedl
 * 	Brought my bitmap palette stuff from dd_interface.
 * 	Adjusted int_mask_reg to be "volatile int *" rather than the
 * 	incorrect "int * volatile". Updated includes to include m88k
 * 	rather than luna88k versions if available.
 * 	[92/07/24            jfriedl]
 * 
 *
 * Revision 2.5.3.2  92/05/27  14:41:42  danner
 *	Header file corrections, support for new pmap module. [danner]
 * 
 * Revision 2.6  92/05/21  17:05:07  jfriedl
 * 	Cleaned up and de-linted.
 * 	Removed subyte() and fubyte() to luna88kdev/kbmain.c
 * 	Appended 'U' to constants that would otherwise be signed.
 * 	Made non-returning fcns volatile under gcc.
 * 	Brought contents of scb.h here; removed scb.h.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.5.3.1  92/05/23  16:18:15  danner
 * 	Header update.
 * 	Changes for new pmap module. 
 * 	Add phystokv for norma.
 * 	Load_context now sets active_threads, active_stacks.
 * 
 * Revision 2.5  92/04/01  10:55:33  rpd
 * 	spl0 now enables interrupts. 
 * 	Removed splcounters
 * 	[92/03/16  17:30:15  danner]
 * 
 * Revision 2.4  92/02/19  13:54:31  elf
 * 	Longjmp_int_enable in load_context should have THREAD_NULL as its
 * 	 second argument in the stack handoff world.
 * 	[92/02/02            danner]
 * 	Changed include of trap.h
 * 	[92/01/30            danner]
 * 	luna88k/cpu.h -> luna88k/cpu_number.h
 * 	[92/01/21            danner]
 * 	Updated includes.
 * 	[92/01/20            danner]
 * 	Fix problem related to the wrong value in the softcall cause register.
 * 	[91/09/19  13:49:10  danner]
 * 
 * Revision 2.3  91/08/24  12:12:58  af
 * 	Change instack declaration type and add a cast for 
 * 	assignment into the interrupt_stack of instack.
 * 	[91/08/16  16:34:38  rvb]
 * 	Add get_slave_stack to do kmem_alloc for slave's stack.
 * 	[91/08/15            rvb]
 * 	Added cpu_configuration_print.
 * 	[91/07/18            danner]
 * 
 * Revision 2.2  91/07/09  23:18:38  danner
 * 	Support for remunged pcb. Untested.
 * 	[91/07/05            danner]
 * 	cputypes.h -> platforms.h
 * 	[91/06/24            danner]
 * 
 * 	Added halt_all_cpus.
 * 	[91/05/10            danner]
 * 
 * Revision 2.8  91/05/07  11:11:58  rvb
 * 	Omron changes April 1991 via J. Friedl - maybe -
 * 	[91/05/07            rvb]
 * 
 * 	Removed debugger initialization from startup; moved to luna_init()
 * 	 in luna_init.c
 * 	[91/05/03            danner]
 * 
 * 	Added missing ifdef MACH_KDB's for ddb support functions
 * 	[91/05/01            danner]
 * 
 * Revision 2.7  91/04/11  10:28:02  mbj
 * 	Conditionalized changes since non-VFS case doesn't have
 * 	dounmount().
 * 	[91/04/11            berman]
 * 
 * Revision 2.6  91/04/10  17:02:01  mbj
 * 	Added some code in boot() to take care of unmounting all the
 * 	filesystems when shutting down.
 * 	[91/02/15            berman]
 * 
 * Revision 2.5  91/04/10  16:05:59  mbj
 * 	Added spl_level_table to acclerate splx. 
 * 	 
 * 	[91/04/09            danner]
 * 
 * 	Added spl_remote to support the pmap module
 * 	[91/04/09            danner]
 * 
 * 	Added counters for spln, splx.
 * 	[91/03/20            danner]
 * 
 * Revision 2.4  91/04/05  13:56:00  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 * 	Flush jkdb
 * 	[91/04/01            rvb]
 * 	Add ovbcopy in C.
 * 	[91/04/01            rvb]
 * 	Completely reorganized ext_int, including support for both
 * 	ddb and jkdb. 
 * 	[91/03/09            danner]
 * 
 * 	Changed mach/luna to  mach/luna88k
 * 	[91/03/08            danner]
 * 
 *
 */

#define __IS_MACHDEP_C__
#include <luna88k/machdep.h>
#include <assym.s>			  /* EF_EPSR, etc. */

#if MACH_KDB
#  include <machine/db_machdep.h>
#endif
#include <motorola/m88k/m88100/m88100.h>  /* DMT_VALID        */
#include <motorola/m88k/m88100/m882xx.h>  /* CMMU stuff       */
#include <vm/vm_kern.h>			  /* kernel_map       */
#include <kern/task.h>			  /* full task defn   */
#include <kern/thread.h>		  /* full thread defn */
#include <luna88k/softint.h>		  /* softcall(), etc. */
#include <luna88k/locore.h>		  /* USERMODE         */
#include <luna88k/nvram.h>		  /* nvram stuff      */
#include <luna88k/rom1ep.h>		  /* RVPtr            */

volatile int scpus = 0; /* number of slave cpus */

/*
 * *int_mask_reg[CPU]
 *
 * Points to the hardware interrupt status register for each CPU.
 *
 * When written, bits 26..31 enable (if set) or disable (if clear) interrupt
 * level one..six respectivel (interrupt level seven... abort switch... is
 * always enabled).
 *
 * When read, bits 18..23 return the current mask (that was set via bits
 * 26..31 the last time this register was written) and bits 29..31 return
 * the highest level of current interrupt outstanding.
 */
volatile unsigned int * int_mask_reg[MAX_CPUS] = {
    (volatile unsigned int *)INT_ST_MASK0,
    (volatile unsigned int *)INT_ST_MASK1,
    (volatile unsigned int *)INT_ST_MASK2,
    (volatile unsigned int *)INT_ST_MASK3
};

static unsigned int int_mask_val[INT_LEVEL] = {
    INT_SET_LV0,
    INT_SET_LV1,
    INT_SET_LV2,
    INT_SET_LV3,
    INT_SET_LV4,
    INT_SET_LV5,
    INT_SET_LV6,
    INT_SET_LV7
};

/* static */ int clock_enabled[MAX_CPUS];

static volatile unsigned int *clock_reg[MAX_CPUS] = {
    (volatile unsigned int *)OBIO_CLOCK0,
    (volatile unsigned int *)OBIO_CLOCK1,
    (volatile unsigned int *)OBIO_CLOCK2,
    (volatile unsigned int *)OBIO_CLOCK3
};

#if !MACH_KDB
static
#endif
volatile unsigned int *soft_reg[MAX_CPUS] = {
    (volatile unsigned int *)SOFT_INT0,
    (volatile unsigned int *)SOFT_INT1,
    (volatile unsigned int *)SOFT_INT2,
    (volatile unsigned int *)SOFT_INT3
};

#if !MACH_KDB
static
#endif
volatile unsigned int *soft_flag_reg[MAX_CPUS] = {
    (volatile unsigned int *)SOFT_INT_FLAG0,
    (volatile unsigned int *)SOFT_INT_FLAG1,
    (volatile unsigned int *)SOFT_INT_FLAG2,
    (volatile unsigned int *)SOFT_INT_FLAG3
};

static simple_lock_data_t soft_lock;

extern void utintr();
extern void spc_intr();
extern void se_intr();

extern int xp_l1_intr();
extern int xp_l5_intr();

static void level4_intr(void)
{
  se_intr(0,/* dummy*/ 0); /* lance */
}

void level3_intr(void)
{
    spc_intr(0);
}

static void level5_inter(void)
{
    /* xp_l5_intr(); */
    /* should have both, but xp not supported yet */
    utintr();
}

#ifdef luna88k
unsigned stop_on_unsup_inter = 0;
#endif

static void level1_inter(void)
{
    #if 0
      xp_l1_intr();
    #else
	printf("Unsupported level 1 XP interrupt\n");
	#if MACH_KDB
#ifdef luna88k
	  if (stop_on_unsup_inter)
#endif
	    gimmeabreak();
	#endif
    #endif
}

/*
 * We allow same levels interrupts.
 * SAME_LEVEL_DEVICES means how many interrupts that are same level
 * are allowed.
 */
#define SAME_LEVEL_DEVICES  5

/*
 * We support same level interrupts
 */
static void (*scb_level0[SAME_LEVEL_DEVICES])();

static void (*scb_level1[SAME_LEVEL_DEVICES])();

static void (*scb_level2[SAME_LEVEL_DEVICES])() =
{
#ifdef MACH_KERNEL
    level1_inter,
#else
    xp_l1_intr,
#endif
    0,
    0,
    0,
    0
};


void (*scb_level3[SAME_LEVEL_DEVICES])();

static void (*scb_level4[SAME_LEVEL_DEVICES])() =
{
  level4_intr,
  0,
  0,
  0,
  0
  };

static void (*scb_level5[SAME_LEVEL_DEVICES])() =
{
    level5_inter,
    0,
    0,
    0,
    0
};

static void (*scb_level6[SAME_LEVEL_DEVICES])();

static void (**scb_device[INT_LEVEL+1])() =
{
    scb_level0,
    scb_level1,
    scb_level2,
    scb_level3,
    scb_level4,
    scb_level5,
    scb_level6,
    0
};

/*
 * Machine-dependent startup code
 */
void startup(vm_offset_t firstaddr)
{
    gimmeabreak();
}


#ifdef PGINPROF
/*
 * Return the difference (in microseconds)
 * between the  current time and a previous
 * time as represented  by the arguments.
 * If there is a pending clock interrupt
 * which has not been serviced due to high
 * ipl, return error code.
 */
unsigned vmtime(int otime, int olbolt, int oicr)
{
    return ((time.tv_sec-otime)*60 + lbolt-olbolt)*16667;
}
#endif /* PGINPROF */

__volatile__ void halt_cpu(void)
{
    register int cpunum = cpu_number();
    machine_slot[cpunum].running = FALSE;
    for (/*empty*/;  /*empty*/;  /*empty*/)
		/*empty*/  ;
    /*NOTREACHED*/
}

/*
 * version of spl for the kernel debugger;
 * never should have breakpoints set in it.
 */
#if MACH_KDB
    unsigned db_spl(void)
    {
	return *int_mask_reg[cpu_number()]<<8;
    }
#endif

unsigned spl_mask(int level)
{
    int cpu = cpu_number();
    int mask = int_mask_val[level];
    if (cpu != master_cpu)
	mask &= INT_SLAVE_MASK;
    return mask;
}

/*
 * version for the debugger entry path -
 * should be kept in sync with above.
 */
#if MACH_KDB
static inline unsigned db_spl_mask(int level)
{
    int cpu = cpu_number();
    int mask = int_mask_val[level];
    if (cpu != master_cpu)
	mask &= INT_SLAVE_MASK;
    return mask;
}
#endif

unsigned spln(int level)
{
    register int cpu = cpu_number();
    register int mask, new_mask;
    m88k_psr_type psr; /* processor status register */

    new_mask = int_mask_val[level];
    if (cpu != master_cpu)
	new_mask &= INT_SLAVE_MASK;
    psr = disable_interrupts_return_psr();
    mask = *int_mask_reg[cpu]<<8;
    *int_mask_reg[cpu] = new_mask;
    flush_pipeline();

    set_psr(psr);
    return mask;
}

/*
 *  Exactly the same as spln except that the debugger will use
 *  this version and no breakpoints should ever be set in it.
 */
#if MACH_KDB
static unsigned db_spln(int level)
{
    register int cpu = cpu_number();
    register int mask, new_mask;
    m88k_psr_type psr;	/* processor status register */

    new_mask = int_mask_val[level];
    if (cpu != master_cpu)
	new_mask &= INT_SLAVE_MASK;
    psr = disable_interrupts_return_psr();
    mask = *int_mask_reg[cpu]<<8;
    *int_mask_reg[cpu] = new_mask;
    db_flush_pipeline();

    set_psr(psr);
    return mask;
}
#endif

/*
 *  Let i be the first bit set (from the right) the index variable j, with
 * no bits set equalling six. The following table contains INT_SET_LVi.
 * Eq. spl_level_table[35] = INT_SET_LV0
 */
#define I6 INT_SET_LV6
#define I5 INT_SET_LV5
#define I4 INT_SET_LV4
#define I3 INT_SET_LV3
#define I2 INT_SET_LV2
#define I1 INT_SET_LV1
#define I0 INT_SET_LV0

static int spl_level_table[64] = {
  I6, I0, I1, I0, I2, I0, I1, I0,
  I3, I0, I1, I0, I2, I0, I1, I0,
  I4, I0, I1, I0, I2, I0, I1, I0,
  I3, I0, I1, I0, I2, I0, I1, I0,
  I5, I0, I1, I0, I2, I0, I1, I0,
  I3, I0, I1, I0, I2, I0, I1, I0,
  I4, I0, I1, I0, I2, I0, I1, I0,
  I3, I0, I1, I0, I2, I0, I1, I0
};

#undef I6
#undef I5
#undef I4
#undef I3
#undef I2
#undef I1
#undef I0

#define	MAX_INT_LEVEL	6
unsigned splx(unsigned mask)
{
    register int cpu = cpu_number();
    register int old_mask;
    m88k_psr_type psr; /* proccessor status register */

    psr = disable_interrupts_return_psr();
    old_mask = *int_mask_reg[cpu]<<8;
    mask = mask >> 26;/* "int(32bits)"-6(MAX_INT_LEVEL) */

    mask = spl_level_table[mask];
    if (cpu != master_cpu) {
	mask &= INT_SLAVE_MASK;
    }
    *int_mask_reg[cpu] = mask;
    flush_pipeline();

    /* The flush pipeline is required to make sure the above write gets
     * through the data pipe and to the hardware; otherwise, the next
     * bunch of instructions could execute at the wrong spl protection
     */
    set_psr(psr);
    return old_mask;
}

/*
 * exactly the same as splx except used by the debugger and therefore
 * should have no breakpoints ever placed in it
 */
#if MACH_KDB
unsigned db_splx(unsigned mask)
{
    register int cpu = cpu_number();
    register int old_mask;
    m88k_psr_type psr; /* proccessor status register */

    psr = disable_interrupts_return_psr();
    old_mask = *int_mask_reg[cpu]<<8;
    mask = mask >> 26;/* "int(32bits)"-6(MAX_INT_LEVEL) */

    mask = spl_level_table[mask];
    if (cpu != master_cpu) {
	mask &= INT_SLAVE_MASK;
    }
    *int_mask_reg[cpu] = mask;
    db_flush_pipeline();                 /* For Hardware spec. */
    set_psr(psr);
    return old_mask;
}
#endif

/* returns positive if memory is not there; */
unsigned check_memory(void *addr, unsigned flag)
{
  return badaddr((vm_offset_t)addr, 1);
}

void start_clock(void)
{
    clock_enabled[cpu_number()] = 1;
}

#if MACH_KDB
unsigned db_splhigh(void)   /* debugger version - no breakpoints */
{
  return db_spln(6);
}
#endif

static volatile int nmi_debounce_count[MAX_CPUS] = {0,0,0,0};
static volatile int interrupt_processing[MAX_CPUS];

/*
 *	Device interrupt handler
 *
 *      when we enter, interrupts are disabled;
 *      when we leave, they should be disabled,
 *      but they need not be enabled throughout
 *      the routine.
 *
 *      This routine is a preamble to the real device
 *      interrupt handler, and is in the entry path for
 *      the kernel debugger. It should never be traced or
 *      have breakpoint/watchpoint protection established upon
 *      it.
 */

/* continuation codes for ext_int */
#define EI_NONE 0
#define EI_CLOCK 1
#define EI_SOFTINT 2
#define EI_SKIP 3

/*
 * Part two of the ext_int code (part one is pre_ext_int below) -
 * this part can have
 * be examined with the debugger. This routine
 * disables interrupts. Will never get here with EI_SKIP
 * as the continuation code
 */
static void ext_int(int *eframe, int vec, int level, int basepri, int cont_code)
{
    register int cpu = cpu_number();

    switch (cont_code)
    {
      case EI_NONE:
	break;

      case EI_CLOCK:
	*clock_reg[cpu] = 0xFFFFFFFFU;	/* reset clock */
	if (clock_enabled[cpu])
	clock_interrupt(10*(1000), USERMODE(eframe[EF_EPSR]), basepri);
	break;

      case EI_SOFTINT:
	softint();
	break;

      default:
	panic("ext_int");
	break;
    }

    if (cpu == master_cpu && *scb_device[level])
	(*scb_device[level])(level,eframe);


    /*
     * process any remaining data access exceptions before
     * returning to assembler
     */
    disable_interrupt();
    if (eframe[EF_DMT0] && DMT_VALID)
    {
	trap(T_DATAFLT, eframe);
	data_access_emulation(eframe);
    }
    *int_mask_reg[cpu] = eframe[EF_MASK];
    interrupt_processing[cpu]--;
}

void pre_ext_int(unsigned vec, unsigned *eframe)
{
    register int cpu;
    register unsigned int mask, level;
    register int basepri;	/* was base priority */
    register int s;		/* XXX */
    register int part2,i;

    part2 = EI_NONE;
    cpu = cpu_number();
    mask = *int_mask_reg[cpu];
    eframe[EF_MASK] = mask<<8;

    level = mask >> 29;
    basepri = (mask<<8 & INT_SET_LV0) == db_spl_mask(0);
    interrupt_processing[cpu]++;

    /*
     * Someone from omron needs to explain this ... (danner@cs)
     *
     * 	if the below condition holds, the hardware lied;
     *  ignore the interupt; likewise if level 0 is
     *  specified
     */
    if (level == 0 || !((mask>>18) & (1<<(level-1))))
    {
	part2 = EI_SKIP;
   #if 0
	printf("(cpu %d, level 0x%x, mask 0x%x, s 0x%x) spurious interrupt\n",
		cpu,level,mask,s);
   #endif
	goto int_done;
    }

    db_spln(level);
    switch (level)
    {
      case CLOCK_INT_LEVEL:
	enable_interrupt();
	part2 = EI_CLOCK;
	break;

    /*
     * It appears that the number from request_from_rom is somewhat
     * unreliable. This results in the loss of softint calls after
     * the debugger is entered. As a partial fix for this problem,
     * we are going to fall through from the debuuger softcall
     * to the case of normal softcall.
    */

#define request_from_rom(cpu)	((RVPtr->EPsoft_intr_rom)[cpu])
      case SOFT_INT_LEVEL:
	s = db_splhigh();			/* XXX */
	enable_interrupt();
	db_simple_lock(&soft_lock);
	i = *soft_flag_reg[cpu];
	if (i & (1L<<SOFT_INT_BIT))
	{
	    /*just read*/ *soft_reg[cpu];	/* reset soft int */
	    db_simple_unlock(&soft_lock);
	    switch(request_from_rom(cpu))
	    {
	      case 1: /* rom */
		part2 = EI_SKIP;
		call_rom_abort(level, eframe);
		break;
#if MACH_KDB
	      case LUNA_DDB_SOFTCALL:
		ddb_softcall_trap(level,(db_regs_t*)eframe);
		/* fall through */
#endif

	      default:
		part2 = EI_SOFTINT;
		break;
	   } /* end switch request */
	}
	else
	{
	    db_simple_unlock(&soft_lock);
	}
	db_splx(s);			/* XXX */
	break;

      case NON_MASKABLE_LEVEL:
      {
	  int count;
	  /* the nmi switch tends to generate multiple nmis.
	     So we debounce the switch by trying to count up to 100.
	     If we hit here and the count is nonzero, bypass the debugger,
	     assuming we are in the debounce phase . */

	  count = nmi_debounce_count[cpu]++;
	  if (count != 0)
	    return;

	  while (++nmi_debounce_count[cpu] < 150);
	  s = db_splhigh();
	  enable_interrupt();
#if	MACH_KDB
	  while (++nmi_debounce_count[cpu] < 100);
	  /* reset it to zero to allow recursive entries, however */
	  nmi_debounce_count[cpu]=0;
	  ddb_nmi_trap(level,(db_regs_t*)eframe);
#endif	MACH_KDB

	  /* Check the status again 5/7/90 by Inui */
	  mask = *int_mask_reg[cpu];
	  cmmu_flush_cache(0, FLUSH_ALL);
	  if (level == (mask >> 29))
	    cmmu_flush_cache(0, FLUSH_ALL);
	  db_splx(s);
	  part2 = EI_SKIP;
	  break; /* NMI processing */
      }

      default:
	enable_interrupt();
	break;

    } /* end switch level */

int_done:
    if (part2 == EI_SKIP)
    {
	disable_interrupt();
	*int_mask_reg[cpu] = eframe[EF_MASK];
	interrupt_processing[cpu]--;
    }
    else
    {
        ext_int(eframe,vec,level,basepri,part2);
    }
}

/*
 *	software interrupt
 */
void softint_on(int cpu)
{
    simple_lock(&soft_lock);
    *soft_reg[cpu] = 0xFFFFFFFFU;
    simple_unlock(&soft_lock);
}

/*
 *
 * Schedule a soft interrupt for the softclock routine
 * 
 * EXPORTED (mach_clock.c)
 */
void setsoftclock(void)
{
  extern void softclock(void);
  softcall((func_t)softclock, 0, cpu_number());
}

/*
 *	init_ast_check - initialize for remote invocation of ast_check.
 *		Allocate the vector exactly once, and fill in cpu fields.
 */
void init_ast_check(processor_t processor)
{
}

/*
 *	cause_ast_check - cause remote invocation of ast_check.  Caller
 *		is at splsched().  Just send the vector to the processor.
 */
void cause_ast_check(processor_t processor)
{
}

void start_other_cpus(void)
{
    int ncpus = 0;
    int s;
    int i;

    /*
     * This routine launches the other cpus. It is called after the
     * first thread is started.
     *
     * From the point we let go of the inter_processor_lock until
     * we know the other slaves are up virtual, shared memory
     * may become incoherent. Thus dsable all interrupts to wait
     * for them to come up, go virtual, flush their caches and
     * report for work.
     *
     * In particular the cmmu_cpu_lock is touched by the slaves
     * when they are physical.
     */
    s = splhigh();
    simple_unlock(&inter_processor_lock);
    cmmu_flush_cache(FLUSH_ALL, FLUSH_ALL);

    for (i=0; i<NCPUS; i++)
	if (machine_slot[i].is_cpu)
	{
	    ncpus++;

	    /* just to make sure... */
	    cmmu_flush_remote_cache(i, FLUSH_ALL, FLUSH_ALL);
	    cmmu_flush_remote_tlb(i, /*kernel*/1,FLUSH_ALL, FLUSH_ALL);
	}

    ncpus--; /* discount one for the master cpu */
    while(scpus != ncpus)
	;
    splx(s);

    /* Overloading it with the determine_cpu_speed routine also */
    determine_cpu_speed();
}

vm_offset_t interrupt_stack[NCPUS] = {0};

vm_offset_t get_slave_stack(void)
{
    vm_offset_t addr = 0;
    kern_return_t ret;

    ret = kmem_alloc(kernel_map, &addr, INTSTACK_SIZE);

    if (ret != KERN_SUCCESS)
	    panic("Cannot allocate slave stack");

    if (interrupt_stack[0] == 0)
	    interrupt_stack[0] = (vm_offset_t) intstack;
    interrupt_stack[cpu_number()] = addr;
    return addr;
}

/*
 * load_context(thread)
 *
 * Start execution of the specified thread.
 * Used only by cpu_launch_first_thread in kern/startup.c
 */
void load_context(thread_t thread)
{
    if (thread->task->map != kernel_map) 
      PMAP_ACTIVATE_USER(vm_map_pmap(thread->task->map),thread,cpu_number());

    disable_interrupt();
    set_current_thread(thread);	
    active_threads[cpu_number()] = thread;
    active_stacks[cpu_number()] = thread->kernel_stack;
    splx(thread->pcb->kernel_state.pcb_mask);

    longjmp_int_enable((jmp_buf_t*)&(thread->pcb->kernel_state),
		(int)THREAD_NULL);
}


kern_return_t cpu_start(int slot_num)
{
    return KERN_FAILURE;
}

kern_return_t cpu_control(int slot_num, int *info, int count)
{
    return KERN_FAILURE;
}

void halt_all_cpus(boolean_t reboot)
{
    if (!reboot)
    {
	call_rom_abort(3, 0); /* reason, exception frame */
	/* if the user exits from the rom, we go boom */
    }
    *((volatile unsigned *) RESET_CPU_ALL) = 0; /* boom */
    for(/*empty*/; /*empty*/; /*empty*/)
	/*empty*/;
    /*NOTREACHED*/
}


/*
 * Given a physical address, return a kernel address maps to
 * that page. XXX cachability on machines with dma.
 */
vm_offset_t phystokv(vm_offset_t phys)
{
    return phys+VEQR_ADDR;
}

/*
 * check a word wide address.
 * write < 0 -> check for write access.
 * otherwise read.
 */
int wprobe(void *addr, unsigned int write)
{
    /* XXX only checking reads */
    return badaddr((vm_offset_t)addr, sizeof(int));
}

/*
 * BMSEL has low-order bits representing each bit plane (1, 4, or 8).
 * The value selects which planes will be affected by subsequent actions
 * to the common bitplane and the common functionset.
 *
 * BMAP_FN[5] is the magic common functionset location that means 'reset'
 * and thus no special transformation (AND, OR, NOR, XOR, etc...) will be
 * done on data written to the bitplanes...
 */
void bm_set_direct_framebuffer_access(void)
{
    *(volatile unsigned *)BMAP_BMSEL = 0xff;        /* seletct all planes */
    ((volatile unsigned *)BMAP_FN)[5]= 0xFFFFFFFFU; /* direct-data mode on */
}

/*
 * The following magic comes from the Luna/88k graphics board hardware info.
 *
 * The RFCNT looks like
 *	struct { unsigned char pad1, vert_loc, pad2, horiz_loc;}
 * and the magic is that the left edge of the screen will be placed
 * at 'vert_loc - 7' in the full hardware window (which is larger than the
 * display window) and that the top line will be at 'horiz_loc + 26'.
 * If you want to roll the screen like the V-Hold is off, cycle 'horiz_loc'...
 */
void bm_align_screen(void)
{
    *(volatile unsigned *)BMAP_RFCNT = 0x0007ffe5;
}

/*
 * set_bm_palette_colors(old, fg_R, fg_G, fg_B, bg_R, bg_G, bg_B)
 * unsigned char old[2][3];
 * unsigned fg_R, fg_G, fg_B, bg_R, bg_G, bg_B;
 *
 * Set the foreground and background colors to those colors specified by
 * the two sets of RGB values.  Each value is in the range 0..15.
 * Thus, [0,0,0] is black and [15, 15, 15] is white.
 *
 * If OLD is given, the previous entries for the two colors is saved
 * if it's an 8-plane system (left alone otherwise).
 */
void bm_set_palette_colors(
    unsigned char old[2/*fg and bg*/][3/*r g b*/],
    unsigned fg_R,
    unsigned fg_G,
    unsigned fg_B,
    unsigned bg_R,
    unsigned bg_G,
    unsigned bg_B)
{
    if (!badwordaddr((vm_offset_t)BMAP_FN4))
    {
	/* This is an 8-plane system */
	volatile struct {
		unsigned char index, pad[3], value;
	} *pal = (void*)BMAP_PALLET2;
	/*
	 * After writing 0x00 to INDEX, VALUE may be read/written in
 	 * groups of three (R G B) to read/write the color components
	 * of the palette.  The first group read/written is for color
	 * index 0, the next for 1, etc. up to a maximum of 255.
	 *
	 * Since we will modify only the first two colors, we'll save
	 * only the first two...
	 *
	 * Actually, I think only the upper four bits of 'value' are
	 * used, so I'll shift those up four bits....
	 */
	if (old)
	{
	    pal->index = 0;
	    old[0][0] = pal->value;
	    old[0][1] = pal->value;
	    old[0][2] = pal->value;
	    old[1][0] = pal->value;
	    old[1][1] = pal->value;
	    old[1][2] = pal->value;
	}

	/* now write... works just like reading */
	pal->index = 0;
	pal->value = (unsigned char)(bg_R)<<4;
	pal->value = (unsigned char)(bg_G)<<4;
	pal->value = (unsigned char)(bg_B)<<4;
	pal->value = (unsigned char)(fg_R)<<4;
	pal->value = (unsigned char)(fg_G)<<4;
	pal->value = (unsigned char)(fg_B)<<4;
    }
    else
    {
	/*
	 * A 1 or 4-plane system... the palette looks a bit different
	 * from the 8-plane system, and the palette can't be read.
	 */
	volatile struct {
		unsigned char index, value;
	} *pal = (void*)BMAP_PALLET2;
	pal->index = 0;
	pal->value = (unsigned char)(bg_R)<<4;
	pal->value = (unsigned char)(bg_G)<<4;
	pal->value = (unsigned char)(bg_B)<<4;
	pal->value = (unsigned char)(fg_R)<<4;
	pal->value = (unsigned char)(fg_G)<<4;
	pal->value = (unsigned char)(fg_B)<<4;
    }
}

/*
 * nvram_read(BUF, ADDRESS, SIZE)
 * nvram_write(BUF, ADDRESS, SIZE)
 *
 * Read and write non-volatile RAM.
 * Only one byte from each word in the NVRAM area is accessable.
 * ADDRESS points to the virtual starting address, which is some address
 * after the nvram start (NVRAM_ADDR). SIZE refers to virtual size.
 */
void nvram_read(char *buf, vm_offset_t address, unsigned size)
{
    unsigned index = (unsigned)address - NVRAM_ADDR;
    unsigned char *source = (char*)(NVRAM_ADDR + index * 4);

    while (size-- > 0)
    {
        *buf++ = *source;
        source += 4; /* bump up to point to next readable byte */
    }
}

void nvram_write(char *buf, vm_offset_t address, unsigned size)
{
    unsigned index = (unsigned)address - NVRAM_ADDR;
    unsigned char *source = (char*)(NVRAM_ADDR + index * 4);

    while (size-- > 0)
    {
        *source = *buf++;
        source += 4; /* bump up to point to next readable byte */
    }
}
