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
 * $Log:	locore_c_routines.c,v $
 * Revision 2.5  93/03/09  17:57:43  danner
 * 	Updated d.a.e. debug prints.
 * 	Created raw_xpr stuff.
 * 	[93/03/09            jfriedl]
 * 
 * 	Removed assembler xmem_dae: label.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.4  93/01/26  18:04:48  danner
 * 	Cleaned up includes. ANSIfied.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:38:46  danner
 * 	Removed LOOP_DEBUG. Added report_lock_info().
 * 	[92/12/07            jfriedl]
 * 
 * Revision 2.2  92/08/03  18:14:48  jfriedl
 * 	Moved from locore directory.
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.4.2.1  92/05/27  14:40:05  danner
 * 	Merged with revision 2.5, minor corrections.
 * 
 * Revision 2.5  92/05/21  17:19:42  jfriedl
 * 	Added a light-weight vsnprintf for debugging purposes.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.3  91/08/24  12:12:04  af
 * 	Removed everything except data access emulation and vector init.
 * 	 The rest of the functions are defunct in 3.0
 * 	[91/08/06            danner]
 * 
 * Revision 2.0  90/08/28  13:05:56  jfriedl
 * about to redo exception handler
 * 
 * Revision 1.8  90/08/21  11:54:32  jfriedl
 * ripped out the interrupt stack stuff.
 * 
 *    Revision 1.6  90/06/12  13:49:46  jfriedl
 *    Added raw_printf printf-type routine.  Debugged for data-access emulation
 *    loop.  Also allowed all exceptions to set the valid bit of DMT0 so
 *    that it's checked just before return.  This is so that a user program
 *    can set it when servicing a bus error and have it redone.
 *    
 *    Revision 1.5  90/06/05  10:49:31  jfriedl
 *    Brought RCS files from version 2.0 to here (version 2.5).
 *    Also added raw_printf() to print strings to the terminal
 *    without a lot of overhead.
 *    
 *    Revision 1.4  90/04/18  15:06:19  jfriedl
 *    after debugging new processor. Now has kluges for new processors.
 *    
 *    Revision 1.1.1.1  90/04/12  22:28:36  jfriedl
 *    Jeff's locore, initial check-in to RCS.
 *****************************************************************RCS**/
/* This file created by Omron Corporation, 1990. */

#include <luna88k/machdep.h>
#include <m88k/thread.h>		 /* current_thread()	*/
#include <kern/thread.h>		 /* current_task()	*/
#include <motorola/m88k/m88100/m88100.h> /* DMT_VALID		*/
#include <assym.s>			 /* EF_NREGS, etc.	*/
#include <luna88k/locore.h>		 /* END_OF_VECTOR_LIST, etc. */
#include <mach_kdb.h>			 /* MACH_KDB		*/
#ifdef MACH_KDB
  #include <ddb/db_output.h>		 /* db_printf() 	*/
#endif


#if defined(MACH_KDB) && defined(JEFF_DEBUG)
# define DATA_DEBUG  1
#endif


#if MACH_KDB
#  define DEBUG_MSG db_printf
#else
#  define DEBUG_MSG printf
#endif

/*
 *  data access emulation for M88100 exceptions
 */
#define DMT_BYTE	1
#define DMT_HALF	2
#define DMT_WORD	4

static struct
{
    unsigned char 	offset;
    unsigned char 	size;
} dmt_en_info[16] =
{
    {0, 0}, {3, DMT_BYTE}, {2, DMT_BYTE}, {2, DMT_HALF},
    {1, DMT_BYTE}, {0, 0}, {0, 0}, {0, 0},
    {0, DMT_BYTE}, {0, 0}, {0, 0}, {0, 0},
    {0, DMT_HALF}, {0, 0}, {0, 0}, {0, DMT_WORD}
};

#if DATA_DEBUG
   int data_access_emulation_debug = 0;
   static char *bytes[] =
   {
	    "____", "___x", "__x_", "__xx",
	    "_x__", "_x_x", "_xx_", "_xxx",
	    "x___", "x__x", "x_x_", "x_xx",
	    "xx__", "xx_x", "xxx_", "xxxx",
   };
  #define DAE_DEBUG(stuff) {						\
	if ((data_access_emulation_debug != 0) && (			\
	    data_access_emulation_debug == 0xffffffff ||		\
	    data_access_emulation_debug == (unsigned)current_thread() ||\
	    data_access_emulation_debug == (unsigned)current_task())) { stuff ;}   }
#else
  #define DAE_DEBUG(stuff)
#endif

void data_access_emulation(unsigned *eframe)
{
    register int x;
    register struct dmt_reg *dmtx;
    register unsigned  dmax, dmdx;
    register unsigned  v, reg;

    if (!(eframe[EF_DMT0] & DMT_VALID))
	return;

    for (x = 0; x < 3; x++)
    {
	dmtx = (struct dmt_reg *)&eframe[EF_DMT0+x*3];

	if (!dmtx->dmt_valid)
		continue;

	dmdx = eframe[EF_DMD0+x*3];
	dmax = eframe[EF_DMA0+x*3];

	DAE_DEBUG
	(
	  if (dmtx->dmt_write)
	    DEBUG_MSG("[cpu%d th=%x DMT%d=%x: st.%c %x to %x as [%s] %s %s]\n",
	      cpu_number(), current_thread(),
	      x, eframe[EF_DMT0+x*3], dmtx->dmt_das ? 's' : 'u',
	      dmdx, dmax, bytes[dmtx->dmt_en], 
	      dmtx->dmt_doub1 ? "double": "not double",
	      dmtx->dmt_lockbar ? "xmem": "not xmem");
	  else
	    DEBUG_MSG("[cpu%d th=%x DMT%d=%x: ld.%c r%d<-%x as [%s] %s %s]\n",
	      cpu_number(), current_thread(),
	      x, eframe[EF_DMT0+x*3], dmtx->dmt_das ? 's' : 'u',
	      dmtx->dmt_dreg, dmax, bytes[dmtx->dmt_en], 
	      dmtx->dmt_doub1 ? "double": "not double",
	      dmtx->dmt_lockbar ? "xmem": "not xmem");
	)

	dmax += dmt_en_info[dmtx->dmt_en].offset;
	reg = dmtx->dmt_dreg;

	if ( ! dmtx->dmt_lockbar)
	{
	    /* the fault is not during an XMEM */

	    if (x == 2 && dmtx->dmt_doub1)
	    {
		/* pipeline 2 (earliest stage) for a double */

		if (dmtx->dmt_write)
		{
		    /* STORE DOUBLE WILL BE RE-INITIATED BY rte */
		}
		else
		{
		    /* EMULATE ld.d INSTRUCTION */
		    if (dmtx->dmt_bo)
		    {
			/* LITTLE ENDIAN */
			v = do_load_word(dmax, dmtx->dmt_das, 1);
			if (reg != 31)
			    eframe[EF_R0 + reg + 1] = v;
			v = do_load_word(dmax ^ 4, dmtx->dmt_das, 1);
			if (reg != 0)
			    eframe[EF_R0 + reg] = v;
		    }
		    else
		    {
			/* BIG ENDIAN */
			v = do_load_word(dmax, dmtx->dmt_das, 0);
			if (reg != 0)
			    eframe[EF_R0 + reg] = v;
			v = do_load_word(dmax ^ 4, dmtx->dmt_das, 0);
			if (reg != 31)
			    eframe[EF_R0 + reg + 1] = v;
		    }
		}
	    }
	    else	 /* not pipeline #2 with a double */
	    {
		if (dmtx->dmt_write) switch (dmt_en_info[dmtx->dmt_en].size)
		{
		  case DMT_BYTE:
		    DAE_DEBUG(DEBUG_MSG("[cpu %d: byte %x -> [%x(%c)]\n",
			cpu_number(),
			dmdx & 0xff, dmax, dmtx->dmt_das ? 's' : 'u'))
		    do_store_byte(dmax, dmdx, dmtx->dmt_das, dmtx->dmt_bo);
		    break;
		  case DMT_HALF:
		    DAE_DEBUG(DEBUG_MSG("[cpu %d: half %x -> [%x(%c)]\n",
			cpu_number(),
			dmdx & 0xffff, dmax, dmtx->dmt_das ? 's' : 'u'))
		    do_store_half(dmax, dmdx, dmtx->dmt_das, dmtx->dmt_bo);
		    break;
		  case DMT_WORD:
		    DAE_DEBUG(DEBUG_MSG("[cpu %d: word %x -> [%x(%c)]\n",
			cpu_number(), dmdx, dmax, dmtx->dmt_das ? 's' : 'u'))
		    do_store_word(dmax, dmdx, dmtx->dmt_das, dmtx->dmt_bo);
		    break;
		}
		else	/* else it's a read */
		{
		    switch (dmt_en_info[dmtx->dmt_en].size)
		    {
		      case DMT_BYTE:
			v = do_load_byte(dmax, dmtx->dmt_das, dmtx->dmt_bo);
			if (!dmtx->dmt_signed)
			    v &= 0x000000ff;
			break;
		      case DMT_HALF:
			v = do_load_half(dmax, dmtx->dmt_das, dmtx->dmt_bo);
			if (!dmtx->dmt_signed)
			    v &= 0x0000ffff;
			break;
		      case DMT_WORD:
		      default: /* 'default' just to shut up lint */
			v = do_load_word(dmax, dmtx->dmt_das, dmtx->dmt_bo);
			break;
		    }
	   	    if (reg == 0) {
			DAE_DEBUG(DEBUG_MSG("[cpu %d: no write to r0 done]\n",
				cpu_number()));
		    }
		    else
		    {
			DAE_DEBUG(DEBUG_MSG("[cpu %d: r%d <- %x]\n",
				cpu_number(), reg, v));
			eframe[EF_R0 + reg] = v;
		    }
		}
	    }
	}
	else /* if lockbar is set... it's part of an XMEM */
	{
	    /*
	     * According to Motorola's "General Information",
	     * the dmt_doub1 bit is never set in this case, as it should be.
	     * They call this "general information" - I call it a f*cking bug!
	     *
	     * Anyway, if lockbar is set (as it is if we're here) and if
	     * the write is not set, then it's the same as if doub1
	     * was set...
	     */
	    if ( ! dmtx->dmt_write)
	    {
		if (x != 2)
		{
		    /* RERUN xmem WITH DMD(x+1) */
		    x++;
		    dmdx = eframe[EF_DMD0 + x*3];
		}
		else
		{
		    /* RERUN xmem WITH DMD2 */
		}

		if (dmt_en_info[dmtx->dmt_en].size == DMT_WORD)
		    v = do_xmem_word(dmax, dmdx, dmtx->dmt_das, dmtx->dmt_bo);
		else
		    v = do_xmem_byte(dmax, dmdx, dmtx->dmt_das, dmtx->dmt_bo);
		eframe[EF_R0 + reg] = v;
	    }
	    else
	    {
		if (x == 0)
		{
		    eframe[EF_R0 + reg] = dmdx;
		    eframe[EF_SFIP] = eframe[EF_SNIP];
		    eframe[EF_SNIP] = eframe[EF_SXIP];
		    eframe[EF_SXIP] = 0;
		    /* xmem RERUN ON rte */
		    eframe[EF_DMT0] = 0;
		    return;
		}
	    }
	}
    }
    eframe[EF_DMT0] = 0;
}

/*
 ***********************************************************************
 ***********************************************************************
 */
#define SIGSYS_MAX      501
#define SIGTRAP_MAX     511

#define EMPTY_BR	0xC0000000U      /* empty "br" instruction */
#define NO_OP 		0xf4005800U      /* "or r0, r0, r0" */

typedef struct
{
	unsigned word_one,
		 word_two;
} m88k_exception_vector_area;

#define BRANCH(FROM, TO) (EMPTY_BR | ((unsigned)(TO) - (unsigned)(FROM)) >> 2)

#define SET_VECTOR(NUM, to, VALUE) {                                       \
	unsigned _NUM = (unsigned)(NUM);                                   \
	unsigned _VALUE = (unsigned)(VALUE);                               \
	vector[_NUM].word_one = NO_OP; 	                                   \
	vector[_NUM].word_two = BRANCH(&vector[_NUM].word_two, _VALUE);    \
}


/*
 * vector_init(vector, vector_init_list)
 *
 * This routine sets up the m88k vector table for the running processor.
 * It is called with a very little stack, and interrupts disabled,
 * so don't call any other functions!
 */
void vector_init(
	m88k_exception_vector_area *vector,
	unsigned *vector_init_list)
{
    register unsigned num;
    register unsigned vec;
    extern void sigsys(), sigtrap(), stepbpt(), userbpt();

    for (num = 0; (vec = vector_init_list[num]) != END_OF_VECTOR_LIST; num++)
    {
	if (vec != PREDEFINED_BY_ROM)
	    SET_VECTOR(num, to, vec);
    }

    while (num <= SIGSYS_MAX)
	SET_VECTOR(num++, to, sigsys);

    while (num <= SIGTRAP_MAX)
	SET_VECTOR(num++, to, sigtrap);

    SET_VECTOR(504, to, stepbpt);
    SET_VECTOR(511, to, userbpt);
}

void report_lock_info(simple_lock_t lock, unsigned r1)
{
    DEBUG_MSG("[LOCK TIMEOUT cpu %d lock 0x%x [val %d] called from 0x%x\n",
	cpu_number(), lock, *lock, r1);
    #if 0
    {
        int i;
	for (i=0; i<4;i++) {
	    cmmu_flush_remote_cache(i, -1, -1);
	    cmmu_flush_remote_tlb(i, /*kernel*/1, -1, -1);
	}
    }
    #endif
}


/* JEFF_DEBUG stuff */
#include <m88k/asm_macro.h>
#include <m88k/thread.h>
#include <kern/thread.h>

#define MAX_XPR_COUNT	1000
struct {
    task_t task;
    char *fmt;
    unsigned arg1;
    unsigned arg2;
} raw_xpr_data[MAX_XPR_COUNT];
unsigned volatile raw_xpr_lock = 0;
unsigned raw_xpr_index = 0;

void _raw_xpr(char *fmt, unsigned b, unsigned c, task_t t)
{
    unsigned myindex;
    m88k_psr_type psr = disable_interrupts_return_psr();
    simple_lock(&raw_xpr_lock);
    if (raw_xpr_index < (MAX_XPR_COUNT - 1)) {
        myindex = raw_xpr_index++;
    } else {
	myindex = 0;
	raw_xpr_index = 1;
    }
    simple_unlock(&raw_xpr_lock);
    set_psr(psr);

    raw_xpr_data[myindex].task = t;
    raw_xpr_data[myindex].fmt  = fmt;
    raw_xpr_data[myindex].arg1 = b;
    raw_xpr_data[myindex].arg2 = c;
}

void raw_xpr_dump(int skipcount)
{
    int i, index = raw_xpr_index + 1;

    raw_xpr_lock = 1; /* forcefully grab the lock */

    if (index >= MAX_XPR_COUNT)
	index = 0;
    else if (raw_xpr_data[index].task == 0)
	index = 0; /* hasn't wrapped yet, so start at the beginning */

    for (i = 1; i < MAX_XPR_COUNT; i++) {
	if (raw_xpr_data[index].task == 0 || raw_xpr_data[index].fmt == 0)
	    break; /* all done */
	if (skipcount-- <= 0)
	{
	    db_printf("%04d: ", i);
	    if (db_lookup_task(raw_xpr_data[index].task) < 0)
	    {
		/* task no longer valid */
		db_printf("<task %x, fmt %x, arg %x, arg %x>\n",
			raw_xpr_data[index].task,
			raw_xpr_data[index].fmt,
			raw_xpr_data[index].arg1,
			raw_xpr_data[index].arg2);
	    } else {
		char buffer[120];
		buffer[0] = '\0';
		db_read_bytes(raw_xpr_data[index].fmt,
			      sizeof(buffer),
			      buffer,
			      raw_xpr_data[index].task);
		buffer[sizeof(buffer)-2] = '\n';
		buffer[sizeof(buffer)-1] = '\0';
		
		db_printf(buffer,
			raw_xpr_data[index].arg1,
			raw_xpr_data[index].arg2);
	   }
	}
	if (++index >= MAX_XPR_COUNT)
	    index = 0;
    }
    
    raw_xpr_lock = 0;
}
