/*
 * HISTORY
 * $Log:	pmap.h,v $
 * Revision 2.6  93/05/15  19:04:42  mrt
 * 	machparam.h -> machspl.h
 * 
 * Revision 2.5  93/03/09  18:01:32  danner
 * 	Renamed NBATC -> BATC_MAX.
 * 	Correct arg order of pmap_set_batc's prototype.
 * 	[93/02/22            jfriedl]
 * 
 * 	Fixed prototype for pmap_set_batc.
 * 	[93/02/11            jfriedl]
 * 
 * Revision 2.4  93/01/26  18:05:44  danner
 * 	Added pmap.c's prototypes, fixed include files.
 * 	[93/01/23            jfriedl]
 * 
 * Revision 2.3  93/01/14  17:40:27  danner
 * 	Added OMRON_PMAP stuff.
 * 	[92/12/02            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:49:02  jfriedl
 * 	Created from Horizon Research version.
 * 	[92/07/24            danner]
 * 
 */
#ifndef _PMAP_MACHINE_
#define _PMAP_MACHINE_

/* use builtin memcpy in gcc 2.0 */
#if (__GNUC__ > 1)
#define bcopy(a,b,c) memcpy(b,a,c)
#endif

#include <m88k/machspl.h>		/* get standard goodies		*/
#include <kern/lock.h>			/* simple_lock_data_t 		*/
#include <mach/m88k/kern_return.h>	/* kern_return_t 		*/
#include <mach/vm_attributes.h> 	/* vm_machine_attribute_t, etc. */
#include <mach/vm_statistics.h>		/* pmap_statistics 		*/
#include <mach/vm_prot.h>		/* vm_prot_t 			*/
#include <m88k/mmu.h>			/* batc_template_t, BATC_MAX, etc.*/
#include <kern/kern_types.h>

typedef struct sdt_entry *sdt_ptr_t;

typedef struct pmap_range {
    struct pmap_range	*next;
    vm_offset_t		start;
    vm_offset_t		end;
} *pmap_range_t;

/*
 * PMAP structure
 */
typedef struct pmap *pmap_t;

/*
 *	10/3/90 Fuzzy modfied
 *	Miss:	typedef struct pmap -> struct pmap
 */
struct pmap {
    sdt_ptr_t		sdt_paddr;	/* physical pointer to sdt */
    sdt_ptr_t		sdt_vaddr;	/* virtual pointer to sdt */
    simple_lock_data_t	lock;		/* lock on map  */
    int			ref_count;	/* reference count */

    struct pmap_statistics stats;	/* pmap statistics */

    /* cpus using of this pmap; NCPU must be <= 32 */
    unsigned long      cpus_using;

#ifdef DBG
    pmap_t		next;
    pmap_t		prev;
#endif

   /* for OMRON_PMAP */
   batc_template_t i_batc[BATC_MAX];  /* instruction BATCs */
   batc_template_t d_batc[BATC_MAX];  /* data BATCs */
   /* end OMRON_PMAP */

   struct pmap_range *notcached; /* Uncached(CACHE_INH) addresses */
}; 

#define PMAP_NULL ((pmap_t) 0)

extern	pmap_t	kernel_pmap;


/*
 * Macros to operate cpus_using field
 */
#define SETBIT_CPUSET(cpu_number, cpuset) (*(cpuset)) |= (1 << (cpu_number));
#define CLRBIT_CPUSET(cpu_number, cpuset) (*(cpuset)) &= ~(1 << (cpu_number));

/*
 * Macro PMAP_ACTIVATE and PMAP_DEACTIVATE have changed to simply call
 * function _pmap_activate and _pmap_deactivate for easy debugging.
 *		N. Sugai	3/11/91
 */

#define PMAP_ACTIVATE(pmap, th, my_cpu)	_pmap_activate(pmap, th, my_cpu)
#define PMAP_DEACTIVATE(pmap, th, my_cpu) _pmap_deactivate(pmap, th, my_cpu)

#define PMAP_CONTEXT(pmap, thread)

#define pmap_resident_count(pmap) ((pmap)->stats.resident_count)

/* Used in builtin/device_pager.c */
#define pmap_phys_address(frame)        ((vm_offset_t) (M88K_PTOB(frame)))

/* Used in kern/mach_timedev.c */
#define pmap_phys_to_frame(phys)        ((int) (M88K_BTOP(phys)))

/*
 * Since Our PCB has no infomation about the mapping,
 * we have nothing to do in PMAP_PCB_INITIALIZE.
 */
/* Used in machine/pcb.c */
#define PMAP_PCB_INITIALIZE(x)

/*
 * Modes used when calling pmap_cache_fulsh().
 */
#define	FLUSH_CACHE		0
#define	FLUSH_CODE_CACHE	1
#define	FLUSH_DATA_CACHE	2
#define	FLUSH_LOCAL_CACHE	3
#define	FLUSH_LOCAL_CODE_CACHE	4
#define	FLUSH_LOCAL_DATA_CACHE	5

/**************************************************************************/
/*** Prototypes for public functions defined in pmap.c ********************/
/**************************************************************************/

void _pmap_activate(pmap_t pmap, thread_t th, int my_cpu);
void _pmap_deactivate(pmap_t pmap, thread_t th, int my_cpu);
void pmap_activate(pmap_t my_pmap, thread_t th, int my_cpu);
void pmap_deactivate(pmap_t pmap, thread_t th, int which_cpu);
void pmap_protect(pmap_t pmap, vm_offset_t s, vm_offset_t e, vm_prot_t prot);
boolean_t pmap_check_transaction(pmap_t pmap, vm_offset_t va, vm_prot_t type);
void pmap_page_protect(vm_offset_t phys, vm_prot_t prot);

vm_offset_t pmap_map(
      vm_offset_t virt,
      vm_offset_t start,
      vm_offset_t end,
      vm_prot_t prot
  #ifdef OMRON_PMAP
      , unsigned cmode
  #endif
      );

vm_offset_t pmap_map_batc(
      vm_offset_t virt,
      vm_offset_t start,
      vm_offset_t end,
      vm_prot_t prot,
      unsigned cmode);

void pmap_enter(
      pmap_t pmap,
      vm_offset_t va,
      vm_offset_t pa,
      vm_prot_t prot,
      boolean_t wired);

void pagemove(vm_offset_t from, vm_offset_t to, int size);

kern_return_t pmap_attribute(
    pmap_t pmap,
    vm_offset_t address,
    vm_size_t size,
    vm_machine_attribute_t attribute,
    vm_machine_attribute_val_t* value);  /* IN/OUT */

void pmap_bootstrap(
    vm_offset_t load_start, /* IN */
    vm_offset_t *phys_start, /* IN/OUT */
    vm_offset_t *phys_end, /* IN */
    vm_offset_t *virt_start, /* OUT */
    vm_offset_t *virt_end); /* OUT */

#ifdef MACH_KERNEL
 void pmap_init();
#else
 void pmap_init(vm_offset_t phys_start, vm_offset_t phys_end);
#endif

void pmap_copy(
    pmap_t dst_pmap,
    pmap_t src_pmap,
    vm_offset_t dst_addr,
    vm_size_t len,
    vm_offset_t src_addr);

void pmap_pageable(
    pmap_t pmap,
    vm_offset_t start,
    vm_offset_t end,
    boolean_t pageable);

pt_entry_t *pmap_pte(pmap_t map, vm_offset_t virt);
void pmap_cache_ctrl(pmap_t pmap, vm_offset_t s, vm_offset_t e, unsigned mode);
void pmap_zero_page(vm_offset_t phys);
pmap_t pmap_create(vm_size_t size);
void pmap_destroy(pmap_t p);
void pmap_reference(pmap_t p);
void pmap_remove(pmap_t map, vm_offset_t s, vm_offset_t e);
void pmap_remove_all(vm_offset_t phys);
void pmap_change_wiring(pmap_t map, vm_offset_t v, boolean_t wired);
vm_offset_t pmap_extract(pmap_t pmap, vm_offset_t va);
vm_offset_t pmap_extract_unlocked(pmap_t pmap, vm_offset_t va);
void pmap_update(void);
void pmap_collect(pmap_t pmap);
pmap_t pmap_kernel(void);
void pmap_copy_page(vm_offset_t src, vm_offset_t dst);
void copy_to_phys(vm_offset_t srcva, vm_offset_t dstpa, int bytecount);
void copy_from_phys(vm_offset_t srcpa, vm_offset_t dstva, int bytecount);
void pmap_redzone(pmap_t pmap, vm_offset_t va);
void pmap_clear_modify(vm_offset_t phys);
boolean_t pmap_is_modified(vm_offset_t phys);
void pmap_clear_reference(vm_offset_t phys);
boolean_t pmap_is_referenced(vm_offset_t phys);
boolean_t pmap_verify_free(vm_offset_t phys);
boolean_t pmap_valid_page(vm_offset_t p);
void icache_flush(vm_offset_t pa);
void pmap_dcache_flush(pmap_t pmap, vm_offset_t va);
void pmap_cache_flush(pmap_t pmap, vm_offset_t virt, int bytes, int mode);
void pmap_print (pmap_t pmap);
void pmap_print_trace (pmap_t pmap, vm_offset_t va, boolean_t long_format);
void pmap_virtual_space(vm_offset_t *startp, vm_offset_t *endp);
unsigned pmap_free_pages(void);
boolean_t pmap_next_page(vm_offset_t *addrp);

#ifdef OMRON_PMAP
 void pmap_set_batc(
    pmap_t pmap,
    boolean_t data,
    int i,
    vm_offset_t va,
    vm_offset_t pa,
    boolean_t super,
    boolean_t wt,
    boolean_t global,
    boolean_t ci,
    boolean_t wp,
    boolean_t valid);

 void use_batc(
    task_t task, 
    boolean_t data,         /* for data-cmmu ? */
    int i,                  /* batc number */
    vm_offset_t va,         /* virtual address */
    vm_offset_t pa,         /* physical address */
    boolean_t s,            /* for super-mode ? */
    boolean_t wt,           /* is writethrough */
    boolean_t g,            /* is global ? */
    boolean_t ci,           /* is cache inhibited ? */
    boolean_t wp,           /* is write-protected ? */
    boolean_t v);           /* is valid ? */
#endif

#ifdef OMRON_PMAP
 void pmap_destroy_ranges(pmap_range_t *ranges);
 boolean_t pmap_range_lookup(pmap_range_t *ranges, vm_offset_t address);
 void pmap_range_add(pmap_range_t *ranges, vm_offset_t start, vm_offset_t end);
 void pmap_range_remove(pmap_range_t *ranges,vm_offset_t start,vm_offset_t end);
#endif

#endif /* endif  _PMAP_MACHINE_ */
