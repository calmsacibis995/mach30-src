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
 * $Log:	autoconf.c,v $
 * Revision 2.11  93/03/09  17:56:26  danner
 * 	strarg->bootarg; cleaned up.
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.10  93/02/01  09:49:48  danner
 * 	Made bitfields unsigned.
 * 	[93/01/26            jfriedl]
 * 
 * Revision 2.9  93/01/26  18:02:21  danner
 * 	ANSIfied. Inlined and removed slave_config().
 * 	Moved cpu_configuration_print, bootarg etc., from machdep.c
 * 	Cleaned up includes.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.8  93/01/14  17:37:40  danner
 * 	Removed unneeded frame buffer "controller"
 * 	[92/12/07            jfriedl]
 * 
 * 	support for frame buffer (fb) and ethernet address for mapped
 * 	ethernet [sugahara@omron].
 * 	[92/11/27            jfriedl]
 * 
 * Revision 2.7  92/08/03  17:40:51  jfriedl
 * 	Add call to inttodr(), lcd_slave().
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.5.2.1  92/05/27  14:34:29  danner
 * 	Conditionalized out old pmap module support code.
 * 	[92/05/27            danner]
 * 
 * Revision 2.6  92/05/21  17:17:36  jfriedl
 * 	Cleaned up and de-linted.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.5  92/05/04  11:26:17  danner
 * 	Lance support. 
 * 	[92/05/03  16:45:17  danner]
 * 
 * Revision 2.4  92/02/18  18:00:26  elf
 * 	Include updates.
 * 	[92/01/20            danner]
 * 
 * Revision 2.3  91/08/24  12:01:23  af
 * 	pcb_zone_init -> pcb_module_init and now is called by MI code.
 * 	[91/08/14            rvb]
 * 	Modifed slave_machine_configure to call cpu_configuration_print.
 * 	[91/07/18            danner]
 * 
 * Revision 2.2.3.1  91/08/19  13:45:50  danner
 * 	pcb_zone_init -> pcb_module_init and now is called by MI code.
 * 	[91/08/14            rvb]
 * 	Modifed slave_machine_configure to call cpu_configuration_print.
 * 	[91/07/18            danner]
 * 
 * Revision 2.2  91/07/09  23:16:34  danner
 * 	cputypes.h -> platforms.h
 * 	[91/06/24            danner]
 * 
 * 	Butchered for 3.0
 * 	[91/05/06            danner]
 * 
 * Revision 2.2  91/04/05  14:01:06  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/*
 * Copyright (c) 1982,1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)autoconf.c	7.1 (Berkeley) 6/6/86
 */

/*
 * Setup the system to run on the current machine.
 *
 * Configure() is called at boot time and initializes the uba and mba
 * device tables and the memory controller monitoring.  Available
 * devices are determined (from possibilities mentioned in ioconf.c),
 * and the drivers are initialized.
 */

#define caddr_t vm_offset_t /* until all caddr_t's are replaced.... */

#include <luna88k/machdep.h>	/* lots of stuff */
#include <mach/machine.h>       /* CPU_SUBTYPE_LUNA88K, machine_slot[]... */
#include <chips/busses.h>       /* struct bus_driver */
#include <chips/lance.h>	/* se_sw */
#include <m88k/cpu_number.h>	/* master_cpu */
#include <vm/vm_kern.h>		/* kernel_map */
#include <luna88k/softint.h>	/* softcall_init() */
#include <luna88k/nvram.h>	/* NVVALSZ, etc. */
#include <luna88k/rom1ep.h>	/* GETXARGS */
#include <motorola/m88k/m88100/m882xx.h> /* cmmu_get_idr(), cmmu_cpu_number() */


extern struct bus_driver spc_driver;
extern struct bus_device se_driver;
extern struct bus_driver fb_driver;

extern void se_intr();
extern void spc_intr();
extern void fbintr();

struct bus_ctlr bus_master_init[] =
{
 /* driver, name, unit, intr, addr+am, phys, adaptor, alive, flags */
 { &spc_driver, "spc", 0, (int(*)())spc_intr, 0x0, 0, 0, '?', 0, 0} ,
 { 0 }
};


/*   driver,    name, unit   intr  addr/am/phys    alive  slave         */
/*                                           adaptor | ctrl  | flags    */
/*                                               |   |   |   |   |      */
struct bus_device bus_device_init[] = {   /*     v   v   v   v   v      */
 { &spc_driver, "rz",  0,       0,  0x0, 0, 0,  '?', 0,  0,  0,  0 },
 { &spc_driver, "rz",  1,       0,  0x0, 0, 0,  '?', 0,  0,  1,  0 },
 { &spc_driver, "rz",  2,       0,  0x0, 0, 0,  '?', 0,  0,  2,  0 },
 { &spc_driver, "rz",  3,       0,  0x0, 0, 0,  '?', 0,  0,  3,  0 },
 { &spc_driver, "rz",  4,       0,  0x0, 0, 0,  '?', 0,  0,  4,  0 },
 { &spc_driver, "rz",  5,       0,  0x0, 0, 0,  '?', 0,  0,  5,  0 },
 { &spc_driver, "rz",  6,       0,  0x0, 0, 0,  '?', 0,  0,  6,  0 },
 { &spc_driver, "rz",  7,       0,  0x0, 0, 0,  '?', 0,  0,  7,  0 },
 { &spc_driver, "tz",  0,       0,  0x0, 0, 0,  '?', 0,  0,  0,  0 },
 { &spc_driver, "tz",  1,       0,  0x0, 0, 0,  '?', 0,  0,  1,  0 },
 { &spc_driver, "tz",  2,       0,  0x0, 0, 0,  '?', 0,  0,  2,  0 },
 { &spc_driver, "tz",  3,       0,  0x0, 0, 0,  '?', 0,  0,  3,  0 },
 { &spc_driver, "tz",  4,       0,  0x0, 0, 0,  '?', 0,  0,  4,  0 },
 { &spc_driver, "tz",  5,       0,  0x0, 0, 0,  '?', 0,  0,  5,  0 },
 { &spc_driver, "tz",  6,       0,  0x0, 0, 0,  '?', 0,  0,  6,  0 },
 { &spc_driver, "tz",  7,       0,  0x0, 0, 0,  '?', 0,  0,  7,  0 },
 { (struct bus_driver *)&se_driver,
                "am",  0, (int(*)())se_intr,
                                    0x0, 0, 0,  '?', 0, -1, -1,  0,},
 { &fb_driver,  "fb",  0,       0,  0x0, 0, 0,    0, 0,  0, -1,  0,},
 { 0 }
};

/*
 * Get non-volatile argument.
 * Like getenv(), but pulls from the non-volatile RAM area.
 */
static inline char *getnvarg(char *p)
{
    register struct nv_system *nvp;
    register i;
    static char sbuf[NVSYMSZ];
    static char abuf[NVVALSZ];

    nvp = &((struct nvram *)NVRAM_ADDR)->nv_system[0];
    for (i = 0; i < NVSYSSZ; i++) {
	nvram_read(sbuf, (vm_offset_t)nvp->nv_symbol, NVSYMSZ);
	if (strncmp(p,sbuf,NVSYMSZ) == 0) {
		nvram_read(abuf, (vm_offset_t)nvp->nv_value, NVVALSZ);
		return (abuf);
	}
	nvp++;
    }
    return 0;
}

/*
 * The "FUSE" ROM data area begins at FUSE_ROM_ADDR.
 * Only the first four bits of each 32 bit word has valid data.
 * Thus, one virtual data byte is found in each raw two words.
 * There are FUSE_ROM_SPACE raw words, so there are FUSE_ROM_SPACE/8
 * virtual data bytes.
 */
#define FUSE_ROM_DATA_BYTES	(FUSE_ROM_SPACE/8)

/*
 * Get FUSE-ROM data byte from raw device.
 */
static inline unsigned char get_FUSErom_byte(unsigned index)
{
    volatile struct {
	unsigned   h_data: 4,
	          h_dummy:28;
	unsigned   l_data: 4,
	          l_dummy:28;
    } *base = (void *)FUSE_ROM_ADDR, *entry = &base[index];

    return (entry->h_data << 4) | entry->l_data;
}

/*
 * get FUSE data from offet to NULL data
 *	return max data length is NVVALSZ+NVSYMSZ(not include NULL)
 */
static inline int getFUSEline(char *setp, int offset)
{
    int i;
    static char fuse_rom_data[FUSE_ROM_DATA_BYTES];
    static int fuse_rom_fg = 0;
    unsigned char sum;

    if (!fuse_rom_fg)
    {
	fuse_rom_fg = 1;
	sum = 0;
	for (i = 0; i < FUSE_ROM_DATA_BYTES; i++)
	    sum ^= fuse_rom_data[i] = get_FUSErom_byte(i);
	if (sum != 0) {
	    printf("FUSE-ROM CHECK SUM ERROR\n");
	    fuse_rom_fg = -1;
	}
    }
    if ( fuse_rom_fg < 0 )
	return 0;				/* check sum-error */
    for (i = 0; i < (NVSYMSZ+NVVALSZ+1); i++)
    {
	*setp = fuse_rom_data[offset++];
	if (*setp++ == '\0')
	    return offset;			/* find parameter */
    }
    return 0;				       	/* not found */
}

/*
 * getfusearg() : return symbol value
 */
static inline char *getfusearg(char *p)
{
    int i;
    int len;
    static char checkbuf[NVSYMSZ+NVVALSZ+1];  /* actualy no-limit in FUSEROM */

    len = strlen(p);
    if (len > NVSYMSZ) {
	/* cannot return value */
	return 0;
    }
    for (i = 0; i < FUSE_ROM_DATA_BYTES-1; )
    {
	/* get a FUSEROM data */
	i = getFUSEline(checkbuf, i);
	if ( !i )
	    return 0;
	/* check symbol name */
	if (strncmp(checkbuf,p,len) == 0 && checkbuf[len] == '=') {
	    return (&checkbuf[len+1]);
	}
    }
    return 0;
}

/*
 * Return the boot variable of the given name, if any.
 * The variable might come from
 *   + boot-time args, as in "b sd() vmunix  ENADDR=12345 NODEBUG".
 *   + non-volatile RAM (nvram), changed with ROM 'nvram' command.
 *   + fuse-rom data (??)
 *
 * Boot-time args override others.
 * Returns 0 if no arg found.
 */
char *bootarg(char *name)
{
    static char *boottime_arg_area;
    static unsigned boottime_arg_length;
    unsigned namelen = strlen(name);
    char *str;
    int i;

    if (boottime_arg_area == 0)
    {
	/* initialize... replace whitespace in boottime_arg_area with nulls */
	boottime_arg_area = GETXARGS; /* get boot args from rom monitor */
	boottime_arg_length = strlen(boottime_arg_area);
	for (i = 0; i < boottime_arg_length; i++)
	    if (boottime_arg_area[i] == ' ' ||
		boottime_arg_area[i] == '\t')
		    boottime_arg_area[i] = 0;
    }

    /*
     * See if there is a command line argument of the form "NAME" or "NAME=val".
     */
    for (i = 0; i < boottime_arg_length; i++)
    {
	if (i == 0 || boottime_arg_area[i-1] == 0)
	{
	    /* we're at the beginning of a word */
	    if (strncmp(name, &boottime_arg_area[i], namelen) == 0)
	    {
		/*
		 * We seem to have a match. The end of the match should
		 * Be either an '=' for the "NAME=val" case, or a null
		 * for the "NAME" case.  For the former, we'll return a pointer
		 * to the "val", while in the latter we'll return the same
		 * pointer we got in.
		 */
		if (boottime_arg_area[i+namelen] == '=')
		    return &boottime_arg_area[i+namelen+1];
		if (boottime_arg_area[i+namelen] == '\0')
		    return name;
	    }
	}
    }
	    
    /* See if it's in the non-volatile ram */
    if (str = getnvarg(name), str != 0)
	return(str);

    /* See if it's in the FUSE rom */
    if (str = getfusearg(name), str != 0)
	return(str);

    return 0;
}


/*
 * Fish out our ethernet address
 */
static void set_ether_addr(void)
{
    register int i;
    register unsigned char *src, c;
    unsigned int *ether_addr;

    if (src = bootarg("ENADDR"), src == 0)
	printf("set_etheraddr: Ethernet address not found\n");
    else if (strlen(src) != 12)
        printf("set_etheraddr: The length of address must be 12.\n");
    else
    {
	if (kmem_alloc_wired(kernel_map, (vm_offset_t *)&ether_addr, PAGE_SIZE)
		  != KERN_SUCCESS)
			panic("set_ether_addr");
	se_sw->romspace += (unsigned int)ether_addr;

	for(i = 0; i < 6; i++)
	{
	    c = *src++;
	    c = c <= '9' ? c - '0' : (c&0xF) + 9;
	    c*=16;
	    c += *src <= '9' ? *src - '0' : (*src&0xF) + 9;
	    src++;
	    ether_addr[i] = c;
	}
    }
}

static char *mmutypes[8] = {
    "Unknown (0)",
    "Unknown (1)",
    "Unknown (2)",
    "Unknown (3)",
    "Unknown (4)",
    "M88200 (16K)",
    "M88204 (64K)",
    "Unknown (7)"
};

/*
 * Should only be called after the calling cpus knows its cpu
 * number and master/slave status . Should be called first
 * by the master, before the slaves are started.
*/
static void cpu_configuration_print(int master)
{
    int pid = read_processor_identification_register();
    int proctype  = (pid & 0xff00) >> 8;
    int immu = cmmu_get_idr(0);
    int iid = (0xe00000 & immu)>>21;
    int dmmu = cmmu_get_idr(1);
    int did = (0xe00000 & dmmu)>>21;
    static simple_lock_data_t print_lock;

    if (master)
	simple_lock_init(&print_lock);

    simple_lock(&print_lock);

    printf("Processor %d: ", cpu_number());
    if (proctype)
	printf("Architectural Revision 0x%x UNKNOWN CPU TYPE Version 0x%x\n",
	    proctype, (pid & 0xe)>>1);
    else
	printf("M88100 Version 0x%x\n", (pid & 0xe)>>1);

    if (mmutypes[iid][0] == 'U')
	printf("    Instruction Cache: Type 0x%x UNKNOWN Version 0x%x\n",
	    iid,(immu & 0x1f0000)>>16);
    else
	printf("    Instruction Cache: %s Version 0x%x\n", mmutypes[iid],
		(immu & 0x1f0000)>>16);

    if (mmutypes[did][0] == 'U')
	printf("    Data        Cache: Type 0x%x UNKNOWN Version 0x%x\n",
	    did,(dmmu & 0x1f0000)>>16);
    else
	printf("    Data        Cache: %s Version 0x%x\n", mmutypes[did],
	    (dmmu & 0x1f0000) >> 16);

    printf  ("    Configured as %s and started\n", master ? "master" : "slave");

    simple_unlock(&print_lock);
}

/*
 * Determine mass storage and memory configuration for a machine.
 * Get cpu type, and then switch out to machine specific procedures
 * which will probe adaptors to see what is out there.
 *
 * Called from "kern/startup.c"
 */
void machine_init(void)
{
    int i;
    /*
     * Copy the cpu existing flag from cpu_sets.
     */
    for (i = 0; i < NCPUS; i++)
	machine_slot[i].is_cpu = cpu_sets[i] ? TRUE : FALSE;

    machine_slot[master_cpu].is_cpu = TRUE;
    machine_slot[master_cpu].running = TRUE;
    machine_slot[master_cpu].cpu_type = CPU_TYPE_MC88000;
    machine_slot[master_cpu].cpu_subtype = CPU_SUBTYPE_LUNA88K;

    softcall_init();		/* initialize softcall */
    utinit();

    scb_level3[0]  = level3_intr;

    spl2();
    enable_interrupt();
    /* Probe the spc based scsi busses */
    configure_bus_master("spc", (vm_offset_t)SCSI_ADDR, 0, 0, "On Board SCSI");
    configure_bus_device("fb", (vm_offset_t)BMAP_BMP, 0, 0, "frame buffer#");
    set_ether_addr();
    configure_bus_device("am", (vm_offset_t)TRI_PORT_RAM,
	(vm_offset_t)TRI_PORT_RAM, 0, "On board LANCE");

    lcd_slave();
    spl0();

    cpu_configuration_print(TRUE);
    inittodr();
}

/*
 * Slave CPU pre-main routine.
 * Determine CPU number and set it.
 *
 * Running on an interrupt stack here; do nothing fancy.
 *
 * Called from "luna88k/locore.s"
 */
void slave_pre_main(void)
{
    set_cpu_number(cmmu_cpu_number());	/* Determine cpu number by CMMU */
    splhigh();
    enable_interrupt();
}

/*
 * Called from "kern/startup.c"
 */
void slave_machine_init(void)
{
    int i, ncpus = 0;

    /*
     *	Slave MMU start
     */
    register int mycpu = cpu_number();
    machine_slot[mycpu].is_cpu = TRUE;
    machine_slot[mycpu].running = TRUE;
    machine_slot[mycpu].cpu_type = CPU_TYPE_MC88000;
    machine_slot[mycpu].cpu_subtype = CPU_SUBTYPE_LUNA88K;
    cpu_configuration_print(FALSE);

    scpus++;

    /* interlock until all slaves are up */
    for (i = 0; i < NCPUS; i++)
	if (machine_slot[i].is_cpu)
	    ncpus++;
    ncpus--; /* discount one for the master cpu not being a slave */

    /* see machdep.c (start_other_cpus) for why this wait is here */
    while(scpus != ncpus)
	/*just wait*/;
}
