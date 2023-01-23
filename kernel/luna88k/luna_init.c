/*
 *
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
 * $Log:	luna_init.c,v $
 * Revision 2.10  93/03/09  17:57:46  danner
 * 	Fixed boothowto; cleanup.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.9  93/01/26  18:03:01  danner
 * 	ANSIfied. Cleaned up includes.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.8  92/08/03  18:13:55  jfriedl
 * 	Replaced luna_db_set_palette with bm_set_palette_colors.[jfriedl]
 * 	Merged with 2.7. Changes for new pmap module. [danner]
 * 
 * Revision 2.7  92/05/21  17:20:03  jfriedl
 *      Cleaned up and de-linted.
 * 	Removed include of <jkdb.h>.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.6.2.1  92/05/23  22:30:06  danner
 * 	Converted for use with new pmap module.
 * 	[92/05/23            danner]
 *
 * Revision 2.6  92/05/04  11:27:11  danner
 * 	Correct version declaration.
 * 	[92/04/13  10:04:08  danner]
 * 
 * 	Print version.
 * 	[92/04/10  20:15:12  danner]
 * 
 * Revision 2.5  92/03/03  15:38:36  rpd
 * 	Fixed irritating palette initialization code.
 * 	[92/03/02            danner]
 * 
 * Revision 2.4  92/02/18  18:02:14  elf
 * 	Fixed type of end, conditionalized db_pallete configuration on
 * 	 diptest.
 * 	[92/02/11            danner]
 * 	Changed types of edata and end to char. Without this, only 1/4
 * 	 of the bss gets zeroed.
 * 	[92/01/28            danner]
 * 
 * 	Made first_addr and last_addr initialized so they will not be in
 * 	 the BSS. They are refered to before move bootstrap
 * 	[92/01/28            danner]
 * 	luna88k/cpu.h -> luna88k/cpu_number.h
 * 	[92/01/21            danner]
 * 
 * 	Removed include of luna88k/m88k.h
 * 	[92/01/20            danner]
 * 
 * 	Adapted to use move_bootstrap.
 * 	[92/01/19            danner]
 * 
 * Revision 2.3  91/08/24  12:12:25  af
 * 	Changes from jfriedl, including 2.5 compatability
 * 	[91/08/06  11:49:31  danner]
 * 
 * Revision 2.2  91/07/09  23:18:25  danner
 * 	Added avail_next.
 * 	[91/06/25            danner]
 * 
 * 	Moved kdb_init here from machdep.
 * 	[91/05/13            danner]
 * 
 * Revision 2.2  91/04/05  14:03:33  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 * 
 */

/*
 *	File:	luna88k/luna_init.c
 *	Basic initialization for LUNA.
 */

#include <luna88k/machdep.h>
#include <motorola/m88k/m88100/m882xx.h> /* CMMU stuff */
#ifdef MACH_KDB
 #include <luna88k/db_machdep.h>
 #include <ddb/db_output.h>	        /* db_printf */
#endif
#include <sys/reboot.h>			/* RB_KDB */
#include <luna88k/pio.h> 		/* diptest */
#include <mach/boot_info.h>		/* boot_info */

int boothowto;	/* read in kern/bootstrap */

#define SYSBASE		SYSV_BASE
#define PHYS(addr)	((unsigned)(addr) - SYSV_BASE)
#define roundup(value, stride) (((unsigned)(value) + (stride) - 1) & ~((stride)-1))

vm_size_t	mem_size;
vm_size_t	rawmem_size;
vm_offset_t	first_addr = 0;
vm_offset_t	last_addr = 0;

vm_offset_t	avail_start, avail_next, avail_end;
vm_offset_t	virtual_avail, virtual_end;

/*
 * Figure out how much real memory is available.
 * Start looking from the megabyte after the end of the kernel data,
 * until we find non-memory.
 */
static void size_memory(void)
{
    volatile unsigned int *look;
    unsigned int *max;
    extern char end[];
    #define PATTERN   0x5a5a5a5a
    #define STRIDE    (1024*1024)

    /*
     * count it up.
     */
    max = (void*)MAXPHYSMEM;
    for (look = (void*)roundup(end, STRIDE); look < max; look += STRIDE/sizeof(*look))
    {
	unsigned save;

	/* if can't access, we've reached the end */
	if (badwordaddr((vm_offset_t)look))
		break;

	/*
	 * If we write a value, we expect to read the same value back.
	 * We'll do this twice, the 2nd time with the opposite bit
	 * pattern from the first, to make sure we check all bits.
	 */
	save = *look;
	if (*look = PATTERN, *look != PATTERN)
		break;
	if (*look = ~PATTERN, *look != ~PATTERN)
		break;
	*look = save;
    }
    rawmem_size = (vm_size_t)look;
    mem_size = (vm_size_t)look;

    /* first addr is now set by move_bootstrap, which is also
       smart enough to preserve the symbol table. */

    last_addr = trunc_page(look);
}

unsigned int luna_page_size = 2*4096;
boolean_t luna_software_dirty_bits = FALSE;

/*
 * Called from locore.s during boot,
 * this is the first C code that's run.
 */
void luna_init(void)
{
    extern char version[];
    extern char edata[], end[];
    extern int kernelstart;
    extern vm_fault_dirty_handling;
    extern boolean_t load_bootstrap_symbols; /* kern/bootstrap.c */
    int no_symbols = (bootarg("nosym") != 0);

    if (!no_symbols && bootarg("bootstrapsyms") != 0)
	load_bootstrap_symbols = TRUE;

    /*
     * These must be set before move_bootstrap is called,
     * else it might align things to the wrong page size
     */
    vm_fault_dirty_handling = luna_software_dirty_bits;
    page_size = luna_page_size;
    vm_set_page_size();

    if (first_addr = move_bootstrap(), first_addr == 0)
	panic("bad move_bootstrap");

    first_addr = round_page(first_addr);

    /* zero the kernel bss */

    blkclr((char*) PHYS(edata), (unsigned) ((end - edata) - sizeof(int)));

    printf("Page size: %d %s dirty bits\n", page_size,
	vm_fault_dirty_handling ? "software" : "hardware");
    /*
     *	We won't really know our cpu number till after
     *	we configure... but we still need to have a valid
     *	cpu number for routines that use it.  Fortunately,
     *	set_cpu_number will do the right thing for us here
     *	because cpu_number doesn't know we have multi-port
     *	memory yet.
     */
    cmmu_init();
    set_cpu_number(master_cpu = cmmu_cpu_number());

    /* get rid of that horrible black text on a white background */
    bm_set_palette_colors(/*save old*/0,  /*fg*/15,15,15,  /*bg*/0, 0, 0);

#if MACH_KDB
    /* so debugger won't wake slaves up until they are ready */
    {
	int i;
	for (i = 0; i < NCPUS; i++)
	    machine_slot[i].is_cpu = 0;
	machine_slot[master_cpu].is_cpu = 1;
    }
    /* initialize kernel debugger */
    kdb_init();
#endif MACH_KDB

    size_memory();
    cmmu_parity_enable();

    /*
     * Read in kern/bootstrap....
     * Always turn on KDB if compiled in.
     * Boot to singleuser only if dip#0 [leftmost] is on [down].
     * If there's a boot-line "multi" argument, we'll go multi as well.
     */
    boothowto = (diptest(0) && !bootarg("multi")) ? RB_SINGLE : 0;

    if (!no_symbols)
	boothowto |= RB_KDB;

    avail_start = first_addr;
    avail_end = last_addr;
    printf("%s",version);
    printf("LUNA boot: memory from 0x%x to 0x%x\n", avail_start, avail_end);
    pmap_bootstrap((vm_offset_t)&kernelstart /* loadpt */, 
		   &avail_start, &avail_end, &virtual_avail,
		   &virtual_end);
}
