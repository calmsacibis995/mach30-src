/*
 * Mach Operating System
 * Copyright (c) 1993 Carnegie Mellon University
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
 * $Log:	machdep.h,v $
 * Revision 2.4  93/05/17  10:24:31  rvb
 * 	Type casts, etc to quiet gcc 2.3.3 warnings
 * 
 * Revision 2.3  93/03/09  17:58:00  danner
 * 	Added prototype for bootarg()
 * 	[93/02/25            jfriedl]
 * 
 * Revision 2.2  93/01/26  18:02:11  danner
 * 	Created.
 * 	[93/01/24            jfriedl]
 * 
 */

#ifndef __LUNA88K_MACHDEP_H__
#define __LUNA88K_MACHDEP_H__

/* 
 ** This file contains prototypes and other decs. for
 ** "luna88k/machdep.c", the assembley files in "luna88k/",
 ** as well as a few misc C files in "luna88k/".
  */

#include <mach_kdb.h>           	/* MACH_KDB */
#include <mach/m88k/vm_types.h>	  	/* vm_offset_t */
#include <kern/lock.h>			/* simple_lock_t */
#include <motorola/m88k/cpu_number.h>	/* cpu_number */
#include <m88k/asm_macro.h>        	/* disable_interrupts(), etc. */
#include <m88k/setjmp.h>		/* jump_buf_t */
#include <m88k/board.h>			/* MAX_CPUS */
#include <kern/kern_types.h>		/* thread_t, task_t, etc. */
#include <mach/m88k/kern_return.h>	/* kern_return_t */

/* from "luna88k/machdep.c" */
extern volatile unsigned int *int_mask_reg[MAX_CPUS];
extern volatile int scpus;

#ifndef __IS_MACHDEP_C__
  #define MACHDEP_STATIC static
#else
  #define MACHDEP_STATIC /* make a copy for everyone */
#endif

MACHDEP_STATIC inline unsigned spl(void)
{
    return *int_mask_reg[cpu_number()]<<8;
}

/* from "luna88k/machdep.c" */
unsigned spln(int level);

MACHDEP_STATIC inline unsigned spl0(void)
{
    int i = spln(0);
    enable_interrupt();
    return i;
}

#define spl1()		spln(1)
#define spl2()		spln(2)
#define spl3()		spln(3)
#define spl4()		spln(4)
#define spl5()		spln(5)
#define spl6()		spln(6)
#define spl7()		spln(7)
#define splsoftclock()	spln(1)
#define splbio()	spln(3)
#define splimp()	spln(4)
#define splvm()		spln(5)
#define splclock()	spln(6)
#define spltty()	spln(5)
#define splhigh()	spln(6)
#define splsched()	spln(6)

/*
 * Prototypes from "luna88k/machdep.h"
 */
void (*scb_level3[])();
void level3_intr(void);
unsigned spl_mask(int level);
unsigned splx(unsigned mask);
unsigned check_memory(void *addr, unsigned flag);
void start_clock(void);
void pre_ext_int(unsigned vec, unsigned *eframe);
void softint_on(int cpu);
void setsoftclock(void);
void init_ast_check(processor_t processor);
void cause_ast_check(processor_t processor);
void start_other_cpus(void);
vm_offset_t get_slave_stack(void);
void load_context(thread_t thread);
kern_return_t cpu_start(int slot_num);
kern_return_t cpu_control(int slot_num, int *info, int count);
void halt_all_cpus(boolean_t reboot);
vm_offset_t phystokv(vm_offset_t phys);
int wprobe(void *addr, unsigned int write);
void bm_set_direct_framebuffer_access(void);
void bm_align_screen(void);
void bm_set_palette_colors(
    unsigned char old[2/*fg and bg*/][3/*r g b*/],
    unsigned fg_R,
    unsigned fg_G,
    unsigned fg_B,
    unsigned bg_R,
    unsigned bg_G,
    unsigned bg_B);
void nvram_read(char *buf, vm_offset_t address, unsigned size);
void nvram_write(char *buf, vm_offset_t address, unsigned int size);

#if MACH_KDB
 unsigned db_spl(void);
 unsigned db_splx(unsigned mask);
 unsigned db_splhigh(void);
#endif

/*
 * Prototypes from "luna88k/locore.s"
 */
extern volatile unsigned initialized_cpus;
extern simple_lock_data_t inter_processor_lock;
extern int intstack[];
extern int boothowto;

void thread_bootstrap(void);
unsigned measure_pause(volatile int *flag);
void delay_in_microseconds(unsigned count);
void delay(unsigned count);
void switch_to_shutdown_context(
    thread_t thread,
    void (*routine)(processor_t),
    processor_t processor);


/*
 * Prototypes from "luna88k/locore_asm_routines.s"
 */
unsigned do_load_word(
	vm_offset_t address,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_load_half(
	vm_offset_t address,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_load_byte(
	vm_offset_t address,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_store_word(
	vm_offset_t address,
	unsigned data,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_store_half(
	vm_offset_t address,
	unsigned data,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_store_byte(
	vm_offset_t address,
	unsigned data,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_xmem_word(
	vm_offset_t address,
	unsigned data,
	boolean_t supervisor_mode,
	boolean_t little_endian);
unsigned do_xmem_byte(
	vm_offset_t address,
	unsigned data,
	boolean_t supervisor_mode,
	boolean_t little_endian);
void set_cpu_number(unsigned number);
void simple_lock_init(simple_lock_t lock);
void simple_unlock(simple_lock_t lock);
void simple_lock(simple_lock_t lock);
unsigned simple_lock_try(simple_lock_t lock);
void set_current_thread(thread_t thread);
/*	These cause too man conflicts
int copyin(vm_offset_t from, void *to, vm_size_t size);
int copyinmsg(vm_offset_t from, void *to, vm_size_t size);
int copyout(void *from, vm_offset_t to, vm_size_t size);
int copyoutmsg(void *from, vm_offset_t to, vm_size_t size);
void bzero(void *addr, unsigned size);

 * Prototypes from "luna88k/eh.s"

boolean_t badaddr(vm_offset_t addr, unsigned len);
boolean_t badwordaddr(vm_offset_t addr);
*/
void ovbcopy(void *source, void *dest, unsigned count);
void blkclr(void *addr, unsigned size);
int _setjmp(jmp_buf_t *buf);
volatile void _longjmp(jmp_buf_t *buf, int value);
volatile void longjmp_int_enable(jmp_buf_t *buf, int value);
unsigned read_processor_identification_register(void);
void call_rom_abort(unsigned level, unsigned *);


/*
 * Prototypes from "luna88k/rawprint_asm_routines.s"
 */
#ifdef RAW_PRINTF
void raw_putstr(char *str);
void raw_putchar(char c);
#endif

/*
 * Prototypes from "luna88k/rawprint_c_routines.c"
 */
int raw_vsnprintf();
void raw_printf(const char *fmt, ...)
    __attribute__ ((format (printf,1,2)));

#ifdef MACH_KDB
void db_simple_unlock(simple_lock_t lock);
void db_simple_lock(simple_lock_t lock);
unsigned db_simple_lock_try(simple_lock_t lock);
unsigned db_simple_lock_held(simple_lock_t lock);
unsigned db_are_interrupts_disabled(void);
#endif


/*
 * Prototypes from "luna88k/locore_c_routines.c"
 */
void data_access_emulation(unsigned *eframe);

/*
 * Prototypes from "luna88k/locore_c_routines.c"
 */
char *bootarg(char *name);

#endif /* __LUNA88K_MACHDEP_H__ */


