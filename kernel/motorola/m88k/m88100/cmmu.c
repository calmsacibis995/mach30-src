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
 * $Log:	cmmu.c,v $
 * Revision 2.8  93/05/15  18:52:23  mrt
 * 	machparam.h -> machspl.h
 * 
 * Revision 2.7  93/03/09  18:01:46  danner
 * 	Turned off SHADOW_BATC debugging.
 * 	Got rid of a lot of debugging cache/tlb flushes.
 * 	Added cmmu_pmap_activate for pmap speed.
 * 	Corrected arguments to cmmu_show_translation; added thread option.
 * 	Corrected cmmu_show_translation to properly access physical address
 * 	through their associated virtual address.
 * 	[93/02/22            jfriedl]
 * 
 * 	Added cmmu_set_pair_batc_entry.
 * 	[93/02/11            jfriedl]
 * 
 * Revision 2.6  93/01/26  18:00:35  danner
 * 	ANSIfied. Cleaned up includes.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.5  93/01/14  17:53:01  danner
 * 	Substantial cleanup, enhancements.
 * 	  + ability to shadow BATC usage.
 * 	  + cleaned up code via CMMU and REGS defines.
 * 	  + cleaned up how CMMUs are initialized
 * 	  + added cmmu_shutdown_now (but may not be useful yet)
 * 	  + added debugging hooks / cmmu_show_translation()
 * 	[92/12/02            jfriedl]
 * 
 * Revision 2.4  92/08/03  17:51:47  jfriedl
 * 	Cmmu_cpu_num fixes for 3.0 [jfriedl]
 * 	Picked up new version from H R. Extensive changes.[danner]
 *
 * Revision 2.2  92/02/18  18:03:23  elf
 * 	Updated includes.
 * 	[92/02/18  16:37:40  danner]
 * 
 * 	Created for luna machine depedent source.
 * 	[92/02/06            danner]
 * 
 */

#include <machine/machspl.h>		/* for luna spl defines */
#include <mach_kdb.h>			/* MACH_KDB */
#include <motorola/m88k/m88100/m882xx.h>/* lots of stuff */
#include <motorola/m88k/cpus.h>		/* M88200, etc.. */
#include <kern/lock.h>			/* simple_lock_data_t */
#include <machine/cpu_number.h>		/* cpu_number */
#include <m88k/pmap.h>		  	/* map stuff */
#include <kern/thread.h>		/* full thread defn */
#include <kern/task.h>			/* full task defn */

#define MAX_CMMUS	(MAX_CPUS*2)

#define SNOOP_ENABLE
#define SHADOW_BATC 0

struct cmmu_regs
{
    /* base + $000 */ volatile unsigned idr;
    /* base + $004 */ volatile unsigned scr;
    /* base + $008 */ volatile unsigned ssr;
    /* base + $00C */ volatile unsigned sar;
    /*             */ unsigned padding1[0x3D];
    /* base + $104 */ volatile unsigned sctr;
    /* base + $108 */ volatile unsigned pfSTATUSr;
    /* base + $10C */ volatile unsigned pfADDRr;
    /*             */ unsigned padding2[0x3C];
    /* base + $200 */ volatile unsigned sapr;
    /* base + $204 */ volatile unsigned uapr;
    /*             */ unsigned padding3[0x7E];
    /* base + $400 */ volatile unsigned bwp[8];
    /*             */ unsigned padding4[0xF8];
    /* base + $800 */ volatile unsigned cdp[4];
    /*             */ unsigned padding5[0x0C];
    /* base + $840 */ volatile unsigned ctp[4];
    /*             */ unsigned padding6[0x0C];
    /* base + $880 */ volatile unsigned cssp;

    /* The rest for the 88204 */
    #define cssp0 cssp
    /*             */ unsigned padding7[0x03];
    /* base + $890 */ volatile unsigned cssp1;
    /*             */ unsigned padding8[0x03];
    /* base + $8A0 */ volatile unsigned cssp2;
    /*             */ unsigned padding9[0x03];
    /* base + $8B0 */ volatile unsigned cssp3;
};

static struct cmmu {
    struct cmmu_regs *cmmu_regs; /* CMMU "base" area */
    unsigned char  cmmu_cpu;	 /* cpu number it is attached to */
    unsigned char  which;	 /* either INST_CMMU || DATA_CMMU */
    unsigned char  cmmu_alive;
	#define CMMU_DEAD	0	/* This cmmu not there */
	#define CMMU_AVAILABLE	1	/* It's there, but which cpu's? */
	#define CMMU_MARRIED	2	/* Know which cpu it belongs to. */
    #if SHADOW_BATC
	unsigned batc[8];
    #endif
} cmmu[MAX_CMMUS] = {
    {(void *)CMMU_I0, 0, 0, 0},
    {(void *)CMMU_D0, 0, 1, 0},
    {(void *)CMMU_I1, 1, 0, 0},
    {(void *)CMMU_D1, 1, 1, 0},
    {(void *)CMMU_I2, 2, 0, 0},
    {(void *)CMMU_D2, 2, 1, 0},
    {(void *)CMMU_I3, 3, 0, 0},
    {(void *)CMMU_D3, 3, 1, 0}
};

/*
 * We rely upon and use INST_CMMU == 0 and DATA_CMMU == 1
 */
#if INST_CMMU != 0 || DATA_CMMU != 1
	ack gag barf!
#endif
struct cpu_cmmu {
    struct cmmu *pair[2];
} cpu_cmmu[MAX_CPUS];

/*
 * CMMU(cpu,data) Is the cmmu struct for the named cpu's indicated cmmu.
 * REGS(cpu,data) is the actual register structure.
 */
#define CMMU(cpu, data) cpu_cmmu[(cpu)].pair[(data)?DATA_CMMU:INST_CMMU]
#define REGS(cpu, data) (*CMMU(cpu,data)->cmmu_regs)

/* 
 * This lock protects the cmmu SAR and SCR's; other ports 
 * can be accessed without locking it 
 *
 * May be used from "luna88k/db_interface.c".
 */
simple_lock_data_t cmmu_cpu_lock;

#define CMMU_LOCK   simple_lock(&cmmu_cpu_lock)
#define CMMU_UNLOCK simple_unlock(&cmmu_cpu_lock)

unsigned cpu_sets[MAX_CPUS];
unsigned ncpus = 0;
unsigned cache_policy = 0;

#ifdef CMMU_DEBUG
void show_apr(unsigned value)
{
    union apr_template apr_template;
    apr_template.bits = value;
    raw_printf("table @ 0x%x000", apr_template.field.st_base);
    if (apr_template.field.wt) raw_printf(", writethrough");
    if (apr_template.field.g)  raw_printf(", global");
    if (apr_template.field.ci) raw_printf(", cache inhibit");
    if (apr_template.field.te) raw_printf(", valid");
    else                       raw_printf(", not valid");
    raw_printf("]\n");
}

void show_sctr(unsigned value)
{
    union {
	unsigned bits;
	struct {
	   unsigned :16,
		  pe: 1,
		  se: 1,
		  pr: 1,
		    :13;
	} fields;
    } sctr; 
    sctr.bits = value;
    raw_printf("%spe, %sse %spr]\n",
	sctr.fields.pe ? "" : "!",
	sctr.fields.se ? "" : "!",
	sctr.fields.pr ? "" : "!");
}
#endif

/*
 * CMMU initialization routine
 */
void cmmu_init(void)
{
    unsigned tmp, cpu, cmmu_num;

    union cpupid id;

    for (cpu = 0; cpu < MAX_CPUS; cpu++)
	cpu_cmmu[cpu].pair[INST_CMMU] = cpu_cmmu[cpu].pair[DATA_CMMU] = 0;

    simple_lock_init(&cmmu_cpu_lock);

    for (cmmu_num = 0; cmmu_num < MAX_CMMUS; cmmu_num++) {
	if (!wprobe((vm_offset_t)cmmu[cmmu_num].cmmu_regs, -1)) {
	    id.cpupid = cmmu[cmmu_num].cmmu_regs->idr;
	    if (id.m88200.type != M88200 && id.m88200.type != M88204)
		continue;
	    cmmu[cmmu_num].cmmu_alive = CMMU_AVAILABLE;

	    cpu_cmmu[cmmu[cmmu_num].cmmu_cpu].pair[cmmu[cmmu_num].which] =
		&cmmu[cmmu_num];

	    /*
	     * Reset cache data....
	     * as per M88200 Manual (2nd Ed.) section 3.11.
	     */
	    for (tmp = 0; tmp < 255; tmp++) {
		cmmu[cmmu_num].cmmu_regs->sar = tmp << 4;
		cmmu[cmmu_num].cmmu_regs->cssp = 0x3f0ff000;
	    }

	    /* 88204 has additional cache to clear */
	    if(id.m88200.type == M88204)
	    {
		for (tmp = 0; tmp < 255; tmp++) {
		    cmmu[cmmu_num].cmmu_regs->sar = tmp<<4;
		    cmmu[cmmu_num].cmmu_regs->cssp1 = 0x3f0ff000;
		}
		for (tmp = 0; tmp < 255; tmp++) {
		    cmmu[cmmu_num].cmmu_regs->sar = tmp<<4;
		    cmmu[cmmu_num].cmmu_regs->cssp2 = 0x3f0ff000;
		}
		for (tmp = 0; tmp < 255; tmp++) {
		    cmmu[cmmu_num].cmmu_regs->sar = tmp<<4;
		    cmmu[cmmu_num].cmmu_regs->cssp3 = 0x3f0ff000;
		}
	    }

	    /*
	     * Set the SCTR, SAPR, and UAPR to some known state
	     * (I don't trust the reset to do it).
	     */
	    tmp =
		! CMMU_SCTR_PE |   /* not parity enable */
		! CMMU_SCTR_SE |   /* not snoop enable */
		! CMMU_SCTR_PR ;   /* not priority arbitration */
	    cmmu[cmmu_num].cmmu_regs->sctr = tmp;

	    tmp =
		(0x00000 << 12) |	/* segment table base address */
		      AREA_D_WT |	/* write through */
	              AREA_D_G  |	/* global */
	              AREA_D_CI |	/* cache inhibit */
	            ! AREA_D_TE ;	/* not translation enable */
	    cmmu[cmmu_num].cmmu_regs->sapr =
	    cmmu[cmmu_num].cmmu_regs->uapr = tmp;

	
#if SHADOW_BATC
	    cmmu[cmmu_num].batc[0] =
	    cmmu[cmmu_num].batc[1] =
	    cmmu[cmmu_num].batc[2] =
	    cmmu[cmmu_num].batc[3] =
	    cmmu[cmmu_num].batc[4] =
	    cmmu[cmmu_num].batc[5] =
	    cmmu[cmmu_num].batc[6] =
	    cmmu[cmmu_num].batc[7] = 0;
#endif
	    cmmu[cmmu_num].cmmu_regs->bwp[0] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[1] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[2] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[3] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[4] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[5] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[6] = 
	    cmmu[cmmu_num].cmmu_regs->bwp[7] = 0;
	    cmmu[cmmu_num].cmmu_regs->scr = CMMU_FLUSH_CACHE_INV_ALL;
	    cmmu[cmmu_num].cmmu_regs->scr = CMMU_FLUSH_SUPER_ALL;
	    cmmu[cmmu_num].cmmu_regs->scr = CMMU_FLUSH_USER_ALL;
	}
    }

    /*
     * Now that we know which CMMUs are there, let's report on which
     * CPU/CMMU sets seem complete (hopefully all)
     */
    for (cpu = 0; cpu < MAX_CPUS; cpu++)
    {
	if (cpu_cmmu[cpu].pair[INST_CMMU] && cpu_cmmu[cpu].pair[DATA_CMMU])
	{
	    cpu_sets[cpu] = 1;	/* This cpu installed... */
	    ncpus++;		/* ....which makes one more installed */
	    id.cpupid = cmmu[cpu*2].cmmu_regs->idr;

	    if(id.m88200.type == M88204)
		printf("CPU%d is attached with MC88204 CMMU\n", cpu);
	    else
		printf("CPU%d is attached with MC88200 CMMU\n", cpu);

	}
	else if (cpu_cmmu[cpu].pair[INST_CMMU])
	{
		printf("CPU%d data CMMU is not working.\n", cpu);
		panic("cmmu-data");
	}
	else if (cpu_cmmu[cpu].pair[DATA_CMMU])
	{
	    printf("CPU%d instruction CMMU is not working.\n", cpu);
	    panic("cmmu");
	}
	else
	{
	    /* no CMMU there? CPU must not be there at all... */
	    cpu_sets[cpu] = 0;
	}
    }

    /*
     * Enable snooping...
     */
    for (cpu = 0; cpu < MAX_CPUS; cpu++)
    {
	if (!cpu_sets[cpu])
	    continue;

	/*
	 * Enable snooping.
	 * We enable it for instruction cmmus as well so that we can have
	 * breakpoints, etc, and modify code.
	 */
	tmp =
	    ! CMMU_SCTR_PE |   /* not parity enable */
	      CMMU_SCTR_SE |   /* snoop enable */
	    ! CMMU_SCTR_PR ;   /* not priority arbitration */
	REGS(cpu, DATA_CMMU).sctr = tmp;
	REGS(cpu, INST_CMMU).sctr = tmp;
	REGS(cpu, DATA_CMMU).scr  = CMMU_FLUSH_SUPER_ALL;
	REGS(cpu, INST_CMMU).scr  = CMMU_FLUSH_SUPER_ALL;
    }

    /*
     * Turn on some cache.
     */
    for (cpu = 0; cpu < MAX_CPUS; cpu++)
    {
	if (!cpu_sets[cpu])
	    continue;
	/*
	 * Enable some caching for the instruction stream.
	 * Can't cache data yet 'cause device addresses can never
	 * be cached, and we don't have those no-caching zones
	 * set up yet....
	 */
	tmp =
	    (0x00000 << 12) |	/* segment table base address */
		  AREA_D_WT |	/* write through */
		  AREA_D_G  |	/* global */
		  AREA_D_CI |	/* cache inhibit */
		! AREA_D_TE ;	/* not translation enable */
	REGS(cpu, INST_CMMU).sapr = tmp;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_SUPER_ALL;
    }
}

/*
 * Just before poweroff or reset....
 */
void cmmu_shutdown_now(void)
{
#if 0 /* was trying to fix a reboot problem... doesn't seem to help */
    unsigned tmp;
    unsigned cmmu_num;

    /*
     * Now set some state as we like...
     */
    for (cmmu_num = 0; cmmu_num < MAX_CMMUS; cmmu_num++)
    {
	tmp =
	    ! CMMU_SCTR_PE |   /* parity enable */
	    ! CMMU_SCTR_SE |   /* snoop enable */
	    ! CMMU_SCTR_PR ;   /* priority arbitration */
	cmmu[cmmu_num].cmmu_regs->sctr = tmp;


	tmp = 
	    (0x00000 << 12) |  /* segment table base address */
	    ! AREA_D_WT |      /* write through */
	    ! AREA_D_G  |      /* global */
	      AREA_D_CI |      /* cache inhibit */
	    ! AREA_D_TE ;      /* translation enable */
	cmmu[cmmu_num].cmmu_regs->sapr = tmp;
	cmmu[cmmu_num].cmmu_regs->uapr = tmp;
    }
#endif
}


/*
 * enable parity
 */
void cmmu_parity_enable(void)
{
#ifdef	PARITY_ENABLE
    register int cmmu_num;

    for (cmmu_num = 0; cmmu_num < MAX_CMMUS; cmmu_num++) {
	if (cmmu[cmmu_num].cmmu_alive != CMMU_DEAD) {
	    cmmu[cmmu_num].cmmu_regs->sctr |= CMMU_SCTR_PE;
	}
    }
#endif	PARITY_ENABLE
}

/*
 * Find out the CPU number from accessing CMMU
 * Better be at splhigh, or even better, with interrupts
 * disabled.
 */
unsigned cmmu_cpu_number(void)
{
    register unsigned cmmu_no;
    int i;

    CMMU_LOCK;
    for (i=0; i < 10; i++)
    {
	/* clear CMMU p-bus status registers */
	for (cmmu_no = 0; cmmu_no < MAX_CMMUS; cmmu_no++) 
	{
	    if (cmmu[cmmu_no].cmmu_alive == CMMU_AVAILABLE &&
	        cmmu[cmmu_no].which == DATA_CMMU) 
		    cmmu[cmmu_no].cmmu_regs->pfSTATUSr = 0;
	}

	/* access faulting address */
	badwordaddr((void *)ILLADDRESS);

	/* check which CMMU reporting the fault  */
	for (cmmu_no = 0; cmmu_no < MAX_CMMUS; cmmu_no++) 
	{
	    if (cmmu[cmmu_no].cmmu_alive == CMMU_AVAILABLE &&
	        cmmu[cmmu_no].which == DATA_CMMU &&
		cmmu[cmmu_no].cmmu_regs->pfSTATUSr & 0x70000)
	    {
		if (cmmu[cmmu_no].cmmu_regs->pfSTATUSr & 0x70000)
		{
		    cmmu[cmmu_no].cmmu_regs->pfSTATUSr = 0; /* to be clean */
		    cmmu[cmmu_no].cmmu_alive = CMMU_MARRIED;
		    CMMU_UNLOCK;
		    return cmmu[cmmu_no].cmmu_cpu;
		}
	    }
	}
    }
raw_printf("at cmmu.c line %d.\n", __LINE__);

    panic("could not determine my cpu number");
    return 0; /* to make compiler happy */
}

/**
 **	Funcitons that actually modify CMMU registers.
 **/

/* 
 *  The caller of this should hold the cmmu_cpu_lock
 *  if modify the CMMU_SCR or CMMU_SAR.
 */
#if !MACH_KDB
static
#endif
void cmmu_remote_set(unsigned cpu, unsigned r, unsigned data, unsigned x)
{
    *(volatile unsigned *)(r + (char*)&REGS(cpu,data)) = x;
}

/*
 * cmmu_cpu_lock should be held when called if read
 * the CMMU_SCR or CMMU_SAR.
**/
#if !MACH_KDB
static
#endif
unsigned cmmu_remote_get(unsigned cpu, unsigned r, unsigned data)
{
    return *(volatile unsigned *)(r + (char*)&REGS(cpu,data));
}

/* Needs no locking - read only registers */
unsigned cmmu_get_idr(unsigned data)
{
    return REGS(cpu_number(),data).idr;
}

void cmmu_set_sapr(unsigned ap)
{
    int cpu = cpu_number();
    if (cache_policy & CACHE_INH)
	ap |= AREA_D_CI;

    REGS(cpu, INST_CMMU).sapr = ap;
    REGS(cpu, DATA_CMMU).sapr = ap;
}

void cmmu_remote_set_sapr(unsigned cpu, unsigned ap)
{
    if (cache_policy & CACHE_INH)
	ap |= AREA_D_CI;
    REGS(cpu, INST_CMMU).sapr = ap;
    REGS(cpu, DATA_CMMU).sapr = ap;
}

void cmmu_set_uapr(unsigned ap)
{
    int cpu = cpu_number();
    /* this functionality also mimiced in cmmu_pmap_activate() */
    REGS(cpu, INST_CMMU).uapr = ap;
    REGS(cpu, DATA_CMMU).uapr = ap;
}

/*
 * Set batc entry number entry_no to value in 
 * the data or instruction cache depending on data.
 *
 * Except for the cmmu_init, this function, cmmu_set_pair_batc_entry,
 * and cmmu_pmap_activate are the only functions which may set the
 * batc values.
 */
void cmmu_set_batc_entry(
     unsigned cpu,
     unsigned entry_no,
     unsigned data,   /* 1 = data, 0 = instruction */
     unsigned value)  /* the value to stuff into the batc */
{
    CMMU_LOCK;

    REGS(cpu,data).bwp[entry_no] = value;
    #if SHADOW_BATC
	CMMU(cpu,data)->batc[entry_no] = value;
    #endif
#if 0 /* was for debugging piece (peace?) of mind */
    REGS(cpu,data).scr = CMMU_FLUSH_SUPER_ALL;
    REGS(cpu,data).scr = CMMU_FLUSH_USER_ALL;
#endif

    CMMU_UNLOCK;
}

/*
 * Set batc entry number entry_no to value in 
 * the data and instruction cache for the named CPU.
 */
void cmmu_set_pair_batc_entry(
     unsigned cpu,
     unsigned entry_no,
     unsigned value)  /* the value to stuff into the batc */
{
    CMMU_LOCK;

    REGS(cpu,DATA_CMMU).bwp[entry_no] = value;
    #if SHADOW_BATC
	CMMU(cpu,DATA_CMMU)->batc[entry_no] = value;
    #endif
    REGS(cpu,INST_CMMU).bwp[entry_no] = value;
    #if SHADOW_BATC
	CMMU(cpu,INST_CMMU)->batc[entry_no] = value;
    #endif

#if 0  /* was for debugging piece (peace?) of mind */
    REGS(cpu,INST_CMMU).scr = CMMU_FLUSH_SUPER_ALL;
    REGS(cpu,INST_CMMU).scr = CMMU_FLUSH_USER_ALL;
    REGS(cpu,DATA_CMMU).scr = CMMU_FLUSH_SUPER_ALL;
    REGS(cpu,DATA_CMMU).scr = CMMU_FLUSH_USER_ALL;
#endif

    CMMU_UNLOCK;
}

/**
 **	Functions that invalidate TLB entries.
 **/

/*
 *	flush any tlb
 *	Some functionality mimiced in cmmu_pmap_activate.
 */
void cmmu_flush_remote_tlb(
    unsigned cpu,
    unsigned kernel,
    vm_offset_t vaddr,
    int size)
{
    register s = splhigh();
    
    CMMU_LOCK;
    if ((unsigned)size > M88K_PGBYTES) 
      {
	REGS(cpu, INST_CMMU).scr =
	REGS(cpu, DATA_CMMU).scr =
	    kernel ? CMMU_FLUSH_SUPER_ALL : CMMU_FLUSH_USER_ALL;
      }
    else /* a page or smaller */
      {
	REGS(cpu, INST_CMMU).sar = (unsigned)vaddr;
	REGS(cpu, DATA_CMMU).sar = (unsigned)vaddr;

	REGS(cpu, INST_CMMU).scr =
	REGS(cpu, DATA_CMMU).scr =
	    kernel ? CMMU_FLUSH_SUPER_PAGE : CMMU_FLUSH_USER_PAGE;
    }
    CMMU_UNLOCK;
    splx(s);
}

/*
 *	flush my personal tlb
 */
void cmmu_flush_tlb(unsigned kernel, vm_offset_t vaddr, int size)
{
    cmmu_flush_remote_tlb(cpu_number(), kernel, vaddr, size);
}


/*
 * New fast stuff for pmap_activate.
 * Does what a few calls used to do.
 * Only called from pmap.c's _pmap_activate().
 */
void cmmu_pmap_activate(
    unsigned cpu,
    unsigned uapr,
    batc_template_t i_batc[BATC_MAX],
    batc_template_t d_batc[BATC_MAX])
{
    int entry_no;
    CMMU_LOCK;

    /* the following is from cmmu_set_uapr */
    REGS(cpu, INST_CMMU).uapr = uapr;
    REGS(cpu, DATA_CMMU).uapr = uapr;

    for (entry_no = 0; entry_no < BATC_MAX; entry_no++) {
	REGS(cpu,INST_CMMU).bwp[entry_no] = i_batc[entry_no].bits;
	REGS(cpu,DATA_CMMU).bwp[entry_no] = d_batc[entry_no].bits;
	#if SHADOW_BATC
	    CMMU(cpu,INST_CMMU)->batc[entry_no] = i_batc[entry_no].bits;
	    CMMU(cpu,DATA_CMMU)->batc[entry_no] = d_batc[entry_no].bits;
	#endif
    }
    CMMU_UNLOCK;

    /*
     * Flush the user TLB.
     * IF THE KERNEL WILL EVER CARE ABOUT THE BATC ENTRIES,
     * THE SUPERVISOR TLBs SHOULB EE FLUSHED AS WELL.
     */
    REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_USER_ALL;
    REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_USER_ALL;
}

/**
 **	Functions that invalidate caches.
 **
 ** Cache invalidates require physical addresses.  Care must be exercised when
 ** using segment invalidates.  This implies that the starting physical address
 ** plus the segment length should be invalidated.  A typical mistake is to
 ** extract the first physical page of a segment from a virtual address, and
 ** then expecting to invalidate when the pages are not physically contiguous.
 **
 ** We don't push Instruction Caches prior to invalidate because they are not
 ** snooped and never modified (I guess it doesn't matter then which form
 ** of the command we use then).
 **/
/*
 *	flush both Instruction and Data caches
 */
void cmmu_flush_remote_cache(int cpu, vm_offset_t physaddr, int size)
{
    register s = splhigh();

    CMMU_LOCK;

    if (size < 0 || size > NBSG ) {
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_ALL;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_ALL;
    }
    else if (size <= 16) {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_LINE;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_LINE;
    }
    else if (size <= NBPG) {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_PAGE;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_PAGE;
    }
    else {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_SEGMENT;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_SEGMENT;
    }

    CMMU_UNLOCK;

    splx(s);
}

/*
 *	flush both Instruction and Data caches
 */
void cmmu_flush_cache(vm_offset_t physaddr, int size)
{
    cmmu_flush_remote_cache(cpu_number(), physaddr, size);
}

/*
 *	flush Instruction caches
 */
void cmmu_flush_remote_inst_cache(int cpu, vm_offset_t physaddr, int size)
{
    register s = splhigh();

    CMMU_LOCK;

    if (size < 0 || size > NBSG ) {
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_ALL;
    }
    else if (size <= 16) {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_LINE;
    }
    else if (size <= NBPG) {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_PAGE;
    }
    else {
	REGS(cpu, INST_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, INST_CMMU).scr = CMMU_FLUSH_CACHE_CBI_SEGMENT;
    }

    CMMU_UNLOCK;

    splx(s);
}

/*
 *	flush Instruction caches
 */
void cmmu_flush_inst_cache(vm_offset_t physaddr, int size)
{
    cmmu_flush_remote_inst_cache(cpu_number(), physaddr, size);
}

void cmmu_flush_remote_data_cache(int cpu, vm_offset_t physaddr, int size)
{ 
    register s = splhigh();

    CMMU_LOCK;

    if (size < 0 || size > NBSG ) {
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_ALL;
    }
    else if (size <= 16) {
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_LINE;
    }
    else if (size <= NBPG) {
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_PAGE;
    }
    else {
	REGS(cpu, DATA_CMMU).sar = (unsigned)physaddr;
	REGS(cpu, DATA_CMMU).scr = CMMU_FLUSH_CACHE_CBI_SEGMENT;
    }

    
    CMMU_UNLOCK;
    splx(s);
}

/*
 * flush data cache
 */ 
void cmmu_flush_data_cache(vm_offset_t physaddr, int size)
{ 
    cmmu_flush_remote_data_cache(cpu_number(), physaddr, size);
}


#if MACH_KDB
union ssr {
    unsigned bits;
    struct {
	unsigned  :16,
		ce:1,
		be:1,
		  :4,
		wt:1,
		sp:1,
		 g:1,
		ci:1,
		  :1,
		 m:1,
		 u:1,
		wp:1,
		bh:1,
		 v:1;
    } field;
};

union cssp {
    unsigned bits;
    struct {
	unsigned   : 2,
		  l: 6,
		 d3: 1,
		 d2: 1,
		 d1: 1,
		 d0: 1,
		vv3: 2,
		vv2: 2,
		vv1: 2,
		vv0: 2,
		   :12;
    } field;
};

union batcu {
    unsigned bits;
    struct {              /* block address translation register */
	unsigned int
	    lba:13,            /* logical block address */
	    pba:13,            /* physical block address */
	    s:1,               /* supervisor */
	    wt:4,              /* write through */
	    g:1,               /* global */
	    ci:1,              /* cache inhibit */
	    wp:1,              /* write protect */
	    v:1;               /* valid */
    } field;
};

#define VV_EX_UNMOD		0
#define VV_EX_MOD		1
#define VV_SHARED_UNMOD		2
#define VV_INVALID		3

#define D(UNION, LINE) \
	((LINE) == 3 ? (UNION).field.d3 : \
	 ((LINE) == 2 ? (UNION).field.d2 : \
	  ((LINE) == 1 ? (UNION).field.d1 : \
	   ((LINE) == 0 ? (UNION).field.d0 : ~0))))
#define VV(UNION, LINE) \
	((LINE) == 3 ? (UNION).field.vv3 : \
	 ((LINE) == 2 ? (UNION).field.vv2 : \
	  ((LINE) == 1 ? (UNION).field.vv1 : \
	   ((LINE) == 0 ? (UNION).field.vv0 : ~0))))
	

/*
 * Show (for debugging) how the given CMMU translates the given ADDRESS.
 * If cmmu == -1, the data cmmu for the current cpu is used.
 * But if thread is not null, we'll use the uapr for the thread.
 */
void cmmu_show_translation(
    unsigned address,
    unsigned supervisor_flag,
    unsigned verbose_flag,
    int cmmu_num,
    thread_t thread)
{
    /*
     * A virtual address is split into three fields. Two are used as
     * indicies into tables (segment and page), and one is an offset into
     * a page of memory.
     */
    union {
	unsigned bits;
	struct {
	    unsigned segment_table_index:10,
		     page_table_index:10,
		     page_offset:12;
	} field;
    } virtual_address;
    unsigned value;

    if (verbose_flag)
        db_printf("-------------------------------------------\n");

    CMMU_LOCK;

    /****** ACCESS PROPER CMMU or THREAD ***********/
    if (thread != 0)
    {
	/* the following tidbit from _pmap_activate in m88k/pmap.c */
	register apr_template_t apr_data;
	supervisor_flag = 0;	/* thread implies user */

	if (thread->task == 0) {
	    db_printf("[thread %x has empty task pointer]\n", thread);
	    return;
	} else if (thread->task->map == 0) {
	    db_printf("[thread/task %x/%x has empty map pointer]\n",
		thread, thread->task);
	    return;
	} else if (thread->task->map->pmap == 0) {
	    db_printf("[thread/task/map %x/%x/%x has empty pmap pointer]\n",
		thread, thread->task, thread->task->map);
	    return;
	}
	if (thread->task->map->pmap->lock.lock_data) {
	    db_printf("[Warning: thread %x's task %x's map %x's "
	      "pmap %x is locked]\n", thread, thread->task,
		thread->task->map, thread->task->map->pmap);
	}
        apr_data.bits = 0;
        apr_data.field.st_base = M88K_BTOP(thread->task->map->pmap->sdt_paddr);
        apr_data.field.wt = 0;
        apr_data.field.g  = 1;
        apr_data.field.ci = 0;
        apr_data.field.te = 1;
	value = apr_data.bits;
        if (verbose_flag) {
	    db_printf("[thread %x task %x map %x pmap %x UAPR is %x]\n",
		thread, thread->task, thread->task->map,
		thread->task->map->pmap, value);
	}
    } else {
	if (cmmu_num == -1)
	{
	    if (cpu_cmmu[cpu_number()].pair[DATA_CMMU] == 0)
	    {
		db_printf("ack! can't figure my own data cmmu number.\n");
		CMMU_UNLOCK;
		return;
	    }
	    cmmu_num = cpu_cmmu[cpu_number()].pair[DATA_CMMU] - cmmu;
	    if (verbose_flag)
		db_printf("The data cmmu for cpu#%d is cmmu#%d.\n",
		   cpu_number(), cmmu_num);
	}
	else if (cmmu_num < 0 || cmmu_num >= MAX_CMMUS)
	{
	    db_printf("invalid cpu number [%d]... must be in range [0..%d]\n",
		    cmmu_num, MAX_CMMUS - 1);
	    CMMU_UNLOCK;
	    return;
	}

	if (cmmu[cmmu_num].cmmu_alive == 0)
	{
	    db_printf("warning: cmmu %d is not alive.\n", cmmu_num);
	    #if 0
	    CMMU_UNLOCK;
	    return;
	    #endif
	}

	if (!verbose_flag)
	{
	    if (!(cmmu[cmmu_num].cmmu_regs->sctr & CMMU_SCTR_SE))
		db_printf("WARNING: snooping not enabled for CMMU#%d.\n",
		    cmmu_num);
	}
	else
	{
	    int i;
	    for (i=0; i<MAX_CMMUS; i++)
		if ((i == cmmu_num || cmmu[i].cmmu_alive) &&
		    (verbose_flag>1 || !(cmmu[i].cmmu_regs->sctr&CMMU_SCTR_SE)))
		{
		    db_printf("CMMU#%d (cpu %d %s) snooping %s\n", i,
			cmmu[i].cmmu_cpu, cmmu[i].which ? "data" : "inst",
			(cmmu[i].cmmu_regs->sctr & CMMU_SCTR_SE) ? "on":"OFF");
		}
	}

	if (supervisor_flag)
	    value = cmmu[cmmu_num].cmmu_regs->sapr;
	else
	    value = cmmu[cmmu_num].cmmu_regs->uapr;

    }

    /******* LOOK AT THE BATC ** (if not a thread) **************/
    #if SHADOW_BATC
    if (thread == 0)
    {
	int i;
	union batcu batc;
	for (i = 0; i < 8; i++) {
	    batc.bits = cmmu[cmmu_num].batc[i];
	    if (batc.field.v == 0) {
		if (verbose_flag>1)
		    db_printf("cmmu #%d batc[%d] invalid.\n", cmmu_num, i);
	    } else {
		db_printf("cmmu#%d batc[%d] v%08x p%08x", cmmu_num, i,
			batc.field.lba << 18, batc.field.pba);
		if (batc.field.s)  db_printf(", supervisor");
		if (batc.field.wt) db_printf(", wt.th");
		if (batc.field.g)  db_printf(", global");
		if (batc.field.ci) db_printf(", cache inhibit");
		if (batc.field.wp) db_printf(", write protect");
	    }
	}
    }
    #endif

    /******* SEE WHAT A PROBE SAYS (if not a thread) ***********/ 
    if (thread == 0)
    {
 	union ssr ssr;
	struct cmmu_regs *cmmu_regs = cmmu[cmmu_num].cmmu_regs;
	cmmu_regs->sar = address;
	cmmu_regs->scr = supervisor_flag ? CMMU_PROBE_SUPER : CMMU_PROBE_USER;
	ssr.bits = cmmu_regs->ssr;
        if (verbose_flag > 1)
	    db_printf("probe of 0x%08x returns ssr=0x%08x\n",
		address, ssr.bits);
	if (ssr.field.v)
	    db_printf("PROBE of 0x%08x returns phys=0x%x",
		address, cmmu_regs->sar);
	else
	    db_printf("PROBE fault at 0x%x", cmmu_regs->pfADDRr);
	if (ssr.field.ce) db_printf(", copyback err");
	if (ssr.field.be) db_printf(", bus err");
	if (ssr.field.wt) db_printf(", writethrough");
	if (ssr.field.sp) db_printf(", sup prot");
	if (ssr.field.g)  db_printf(", global");
	if (ssr.field.ci) db_printf(", cache inhibit");
	if (ssr.field.m)  db_printf(", modified");
	if (ssr.field.u)  db_printf(", used");
	if (ssr.field.wp) db_printf(", write prot");
	if (ssr.field.bh) db_printf(", BATC");
	db_printf(".\n");
    }

    /******* INTERPRET AREA DESCRIPTOR *********/
    {
	union apr_template apr_template;
	apr_template.bits = value;
        if (verbose_flag > 1) {
	    if (thread == 0)
		db_printf("CMMU#%d", cmmu_num);
	    else
		db_printf("THREAD %x", thread);
	    db_printf(" %cAPR is 0x%08x\n",
		supervisor_flag ? 'S' : 'U', apr_template.bits);
	}
	if (thread == 0)
	    db_printf("CMMU#%d", cmmu_num);
	else
	    db_printf("THREAD %x", thread);
	db_printf(" %cAPR: SegTbl: 0x%x000p",
	    supervisor_flag ? 'S' : 'U', apr_template.field.st_base);
	if (apr_template.field.wt) db_printf(", WTHRU");
	else                       db_printf(", !wthru");
	if (apr_template.field.g)  db_printf(", GLOBAL");
	else                       db_printf(", !global");
	if (apr_template.field.ci) db_printf(", $INHIBIT");
	else                       db_printf(", $ok");
	if (apr_template.field.te) db_printf(", VALID");
	else                       db_printf(", !valid");
	db_printf(".\n");

	/* if not valid, done now */
	if (apr_template.field.te == 0) {
	    db_printf("<would report an error, valid bit not set>\n");
	    CMMU_UNLOCK;
	    return;
	}

	value = apr_template.field.st_base << 12; /* now point to seg page */
    }

    /* translate value from physical to virtual */
    if (verbose_flag)
	db_printf("[%x physical is %x virtual]\n", value, value + VEQR_ADDR);
    value += VEQR_ADDR;

    virtual_address.bits = address;
 
    /****** ACCESS SEGMENT TABLE AND INTERPRET SEGMENT DESCRIPTOR  *******/
    {
	union sdt_entry_template std_template;
	if (verbose_flag)
	    db_printf("will follow to entry %d of page at 0x%x...\n",
		virtual_address.field.segment_table_index, value);
	value |= virtual_address.field.segment_table_index *
		 sizeof(struct sdt_entry);

	if (badwordaddr(value)) {
	    db_printf("ERROR: unable to access page at 0x%08x.\n", value);
	    CMMU_UNLOCK;
	    return;
	}

	std_template.bits = *(unsigned *)value;
        if (verbose_flag > 1)
	    db_printf("SEG DESC @0x%x is 0x%08x\n", value, std_template.bits);
	db_printf("SEG DESC @0x%x: PgTbl: 0x%x000",
	    value, std_template.sdt_desc.table_addr);
	if (std_template.sdt_desc.wt)       db_printf(", WTHRU");
	else                                db_printf(", !wthru");
	if (std_template.sdt_desc.sup)      db_printf(", S-PROT");
	else                                db_printf(", UserOk");
	if (std_template.sdt_desc.g)        db_printf(", GLOBAL");
	else                                db_printf(", !global");
	if (std_template.sdt_desc.no_cache) db_printf(", $INHIBIT");
	else                                db_printf(", $ok");
	if (std_template.sdt_desc.prot)     db_printf(", W-PROT");
	else                                db_printf(", WriteOk");
	if (std_template.sdt_desc.dtype)    db_printf(", VALID");
	else                                db_printf(", !valid");
	db_printf(".\n");

	/* if not valid, done now */
	if (std_template.sdt_desc.dtype == 0) {
	    db_printf("<would report an error, STD entry not valid>\n");
	    CMMU_UNLOCK;
	    return;
	}

	value = std_template.sdt_desc.table_addr << 12;
    }

    /* translate value from physical to virtual */
    if (verbose_flag)
	db_printf("[%x physical is %x virtual]\n", value, value + VEQR_ADDR);
    value += VEQR_ADDR;

    /******* PAGE TABLE *********/
    {
	union pte_template pte_template;
	if (verbose_flag)
	    db_printf("will follow to entry %d of page at 0x%x...\n",
		virtual_address.field.page_table_index, value);
	value |= virtual_address.field.page_table_index *
		sizeof(struct pt_entry);

	if (badwordaddr(value)) {
	    db_printf("error: unable to access page at 0x%08x.\n", value);
	    CMMU_UNLOCK;
	    return;
	}

	pte_template.bits = *(unsigned *)value;
	if (verbose_flag > 1)
	    db_printf("PAGE DESC @0x%x is 0x%08x.\n", value, pte_template.bits);
	db_printf("PAGE DESC @0x%x: page @%x000",
		value, pte_template.pte.pfn);
	if (pte_template.pte.wired)    db_printf(", WIRE");
	else                           db_printf(", !wire");
	if (pte_template.pte.wt)       db_printf(", WTHRU");
	else                           db_printf(", !wthru");
	if (pte_template.pte.sup)      db_printf(", S-PROT");
	else                           db_printf(", UserOk");
	if (pte_template.pte.g)        db_printf(", GLOBAL");
	else                           db_printf(", !global");
	if (pte_template.pte.ci)       db_printf(", $INHIBIT");
	else                           db_printf(", $ok");
	if (pte_template.pte.modified) db_printf(", MOD");
	else                           db_printf(", !mod");
	if (pte_template.pte.pg_used)  db_printf(", USED");
	else                           db_printf(", !used");
	if (pte_template.pte.prot)     db_printf(", W-PROT");
	else                           db_printf(", WriteOk");
	if (pte_template.pte.dtype)    db_printf(", VALID");
	else                           db_printf(", !valid");
	db_printf(".\n");

	/* if not valid, done now */
	if (pte_template.pte.dtype == 0) {
	    db_printf("<would report an error, PTE entry not valid>\n");
	    CMMU_UNLOCK;
	    return;
	}
	
	value = pte_template.pte.pfn << 12;
	if (verbose_flag)
	    db_printf("will follow to byte %d of page at 0x%x...\n",
		virtual_address.field.page_offset, value);
	value |= virtual_address.field.page_offset;

	if (badwordaddr(value)) {
	    db_printf("error: unable to access page at 0x%08x.\n", value);
	    CMMU_UNLOCK;
	    return;
	}
    }

    /* translate value from physical to virtual */
    if (verbose_flag)
	db_printf("[%x physical is %x virtual]\n", value, value + VEQR_ADDR);
    value += VEQR_ADDR;

    db_printf("WORD at 0x%x is 0x%08x.\n", value, *(unsigned *)value);
    CMMU_UNLOCK;
}


void cmmu_cache_state(unsigned addr, unsigned supervisor_flag)
{
    static char *vv_name[4] =
	{"exclu-unmod", "exclu-mod", "shared-unmod", "invalid"};
    int cmmu_num;
    CMMU_LOCK;
    for (cmmu_num = 0; cmmu_num < MAX_CMMUS; cmmu_num++)
    {
	union ssr ssr;
	union cssp cssp;
	struct cmmu_regs *R;
	unsigned tag, line;
	if (!cmmu[cmmu_num].cmmu_alive)
	    continue;
	R = cmmu[cmmu_num].cmmu_regs;
	db_printf("cmmu #%d %s cmmu for cpu %d.\n", cmmu_num,
	    cmmu[cmmu_num].which ? "data" : "inst", 
	    cmmu[cmmu_num].cmmu_cpu);
	R->sar = addr;
	R->scr = supervisor_flag ? CMMU_PROBE_SUPER : CMMU_PROBE_USER;

	ssr.bits = R->ssr;
	if (!ssr.field.v) {
	    db_printf("PROBE of 0x%08x faults.\n",addr);
	    continue;
	}
	db_printf("PROBE of 0x%08x returns phys=0x%x", addr, R->sar);

	tag = R->sar & ~0xfff;
	cssp.bits = R->cssp;

	/* check to see if any of the tags for the set match the address */
	for (line = 0; line < 4; line++)
	{
	    if (VV(cssp, line) == VV_INVALID)
	    {
		db_printf("line %d invalid.\n", line);
		continue; /* line is invalid */
	    }
	    if (D(cssp, line))
	    {
		db_printf("line %d disabled.\n", line);
		continue; /* line is disabled */
	    }

	    if ((R->ctp[line] & ~0xfff) != tag)
	    {
		db_printf("line %d address tag is %x.\n", line,
			(R->ctp[line] & ~0xfff));
		continue;
	    }
	    db_printf("found in line %d as %08x (%s).\n",
		line, R->cdp[line], vv_name[VV(cssp, line)]);
        }
    }
    CMMU_UNLOCK;
}

void show_cmmu_info(unsigned addr)
{
    int cmmu_num;
    cmmu_cache_state(addr, 1);

    for (cmmu_num = 0; cmmu_num < MAX_CMMUS; cmmu_num++)
	if (cmmu[cmmu_num].cmmu_alive) {
	    db_printf("cmmu #%d %s cmmu for cpu %d: ", cmmu_num,
		cmmu[cmmu_num].which ? "data" : "inst", 
		cmmu[cmmu_num].cmmu_cpu);
	    cmmu_show_translation(addr, 1, 0, cmmu_num, (thread_t)0);
	}
}
#endif /* end if MACH_KDB */
