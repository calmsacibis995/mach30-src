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
 * $Log:	db_machdep.h,v $
 * Revision 2.10  93/03/09  17:56:40  danner
 * 	Added DB_TASK_NAME, etc.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.9  93/01/26  18:02:15  danner
 * 	added prototypes, fixed includes.
 * 	[93/01/22            jfriedl]
 * 
 * Revision 2.8  93/01/14  17:38:09  danner
 * 	We do not support coff.
 * 	[92/12/18            danner]
 *
 * 	fixed IS_WATCHPOINT_TRAP to not always be false.
 * 	[92/12/07            jfriedl]
 *
 * 	moved inst_unconditional_flow_transfer, inst_call,
 * 	and inst_return here from db_sstep.c.
 * 	Added db_branch_is_delayed.
 * 	[92/11/28            jfriedl]
 * 	ifdef lunatics do not belong in MI code.
 * 	[92/11/30            af]
 *
 * Revision 2.7  92/05/21  17:18:21  jfriedl
 * 	Appended 'U' to constants with high bit on.
 * 	[92/05/16            jfriedl]
 *
 * Revision 2.6  92/05/04  11:26:33  danner
 * 	Set DB_ACCESS_LEVEL to DB_ACCESS_ANY.
 * 	[92/04/18            danner]
 *
 * Revision 2.5  92/04/01  10:54:37  rpd
 * 	Made suitable for inclusion in assembly files.
 * 	[92/03/03            danner]
 *
 * Revision 2.4  92/02/18  18:00:52  elf
 * 	Changed BKPT_INST, ENTRY_ASM to be trap 144, which is excutable
 * 	 from user space.
 * 	[92/02/13            danner]
 *
 * 	Several new macros to support new ddb.
 * 	[92/02/12            danner]
 * 	Updated include of trap.h
 * 	[92/01/30            danner]
 * 	Moved redefinitions of _longjmp and _setjmp under 2.5
 * 	 conditionals.
 * 	[92/01/19            danner]
 *
 * Revision 2.3  91/08/24  12:07:20  af
 * 	Changes from jfriedl, including 2.5 compatability
 * 	[91/08/06  11:49:49  danner]
 *
 * Revision 2.2.3.1  91/08/19  13:46:16  danner
 * 	Changes from jfriedl, including 2.5 compatability
 * 	[91/08/06  11:49:49  danner]
 *
 * Revision 2.3  91/04/10  16:05:47  mbj
 * 	Added setjmp, longjmp defined to make ddb use the kernel
 * 	 versions, not the user level versions.
 * 	[91/03/31            danner]
 *
 * Revision 2.2  91/04/05  14:01:53  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 *
 * 	Created
 * 	[91/01/11            danner]
 *
 */

/*
 * Machine-dependent defined for the new kernel debugger
 */

#ifndef  LUNA_DB_MACHDEP_H_
#define  LUNA_DB_MACHDEP_H_ 1

#include <cpus.h>
#include <motorola/m88k/m88100/pcb.h>	/* m88100_saved_state */
#include <mach/m88k/vm_types.h>		/* natural_t */
#include <motorola/m88k/trap.h>		/* for T_KDB_BREAK */
#include <mach/m88k/boolean.h>		/* boolean_t */
#include <kern/task.h>
#include <kern/kern_types.h>

#define BKPT_SIZE        (4)                /* number of bytes in bkpt inst. */
#define BKPT_INST       (0xF000D082U)             /* tb0, 0,r0, vector 132 */
#define BKPT_SET(inst)  (BKPT_INST)

/* Entry trap for the debugger - used for inline assembly breaks*/
#define ENTRY_ASM       	"tb0 0, r0, 132"
#define DDB_ENTRY_TRAP_NO	132

typedef vm_offset_t   db_addr_t;
typedef int           db_expr_t;
typedef struct m88100_saved_state db_regs_t;

extern db_regs_t      db_push;
extern db_regs_t      *db_cur_exc_frame[NCPUS];     /* register state for debugger */
extern int             ddb_master;

extern int db_noisy;
#define NOISY(x) if (db_noisy) x
#define NOISY2(x) if (db_noisy >= 2) x
#define NOISY3(x) if (db_noisy >= 3) x

#define DB_TASK_NAME(task)	db_printf("\"%s\"", db_task_name(task))
#define DB_TASK_NAME_TITLE	"COMMAND"

extern int quiet_db_read_bytes;

/* get the registers from the master cpu */
#define  DDB_REGS   		(db_cur_exc_frame[ddb_master])
#define  SAVE_DDB_REGS 		db_push = *DDB_REGS
#define  RESTORE_DDB_REGS 	*DDB_REGS = db_push

/*
 * the low two bits of sxip, snip, sfip have valid bits
 * in them that need to masked to get the correct addresses
 */

/*
 * This is an actual function due to the fact that the sxip
 * or snip could be nooped out due to a jmp or rte
 */
#define PC_REGS(regs) (luna_pc(regs))

/* These versions are not constantly poing SPL */
#define  cnmaygetc()    db_getc()
#define  cngetc()       db_getc()
#define  cnputc(x)      db_putc(x)

/* breakpoint/watchpoint foo */
#define IS_BREAKPOINT_TRAP(type,code) ((type)==T_KDB_BREAK)
#define IS_WATCHPOINT_TRAP(type,code) ((type)==T_KDB_WATCH)

/* we don't want coff support */
#define DB_NO_COFF 1

/* need software single step */
#define SOFTWARE_SSTEP 1

/*
 * Debugger can get to any address space
 */

#define DB_ACCESS_LEVEL DB_ACCESS_ANY

#define DB_VALID_KERN_ADDR(addr) (!badaddr((void*)(addr), 1))
#define DB_VALID_ADDRESS(addr,user) \
  (user ? db_check_user_addr(addr) : DB_VALID_KERN_ADDR(addr))

#define DB_PHYS_EQ(task1,addr1,task2,addr2) \
  db_phy_eq(task1,addr1,task2,addr2)

#define DB_CHECK_ACCESS(addr,size,task) db_check_access(task,addr,size)

/* instruction type checking - others are implemented in db_sstep.c */

#define inst_trap_return(ins)  ((ins) == 0xf400fc00U)

/* don't need to load symbols */
#define DB_SYMBOLS_PRELOADED 1

/* softcall vector for softint */
#define LUNA_DDB_SOFTCALL 3

/* machine specific commands have been added to ddb */
#define DB_MACHINE_COMMANDS 1
/* inst_return(ins) - is the instruction a function call return.
 * Not mutually exclusive with inst_branch. Should be a jmp r1. */
#define inst_return(I) (((I)&0xfffffbffU) == 0xf400c001U ? TRUE : FALSE)

#ifdef __GNUC__
/*
 * inst_call - function call predicate: is the instruction a function call.
 * Could be either bsr or jsr
 */
#define inst_call(I) ({ unsigned i = (I); \
	((((i) & 0xf8000000U) == 0xc8000000U || /*bsr*/ \
          ((i) & 0xfffffbe0U) == 0xf400c800U)   /*jsr*/ \
	? TRUE : FALSE) \
;})

/*
 * This routine should return true for instructions that result in unconditonal
 * transfers of the flow of control. (Unconditional Jumps, subroutine calls,
 * subroutine returns, etc).
 *
 *  Trap and return from trap  should not  be listed here.
 */
#define inst_unconditional_flow_transfer(I) ({ unsigned i = (I); \
    ((((i) & 0xf0000000U) == 0xc0000000U || /* br, bsr */ \
      ((i) & 0xfffff3e0U) == 0xf400c000U)   /* jmp, jsr */ \
     ? TRUE: FALSE) \
;})

/* Return true if the instruction has a delay slot.  */
#define db_branch_is_delayed(I)	inst_delayed(I)

#endif __GNUC__

#define	db_printf_enter	db_printing



/*
 * Prototypes from "luna88k/db_disasm.c"
 */
int luna_print_instruction(unsigned iadr, long inst);
db_addr_t db_disasm(db_addr_t loc, boolean_t altfmt, task_t task);

/*
 * Prototypes, etc. from "luna88k/db_interface.c"
 */
int db_getc(void);
void db_putc(unsigned c);
void db_write_bytes(char *addr, unsigned size, char *data, task_t task);
void db_read_bytes(char *addr, unsigned size, char *data, task_t task);
int luna_pc(struct m88100_saved_state *regs);
void ddb_error_trap(char *error, db_regs_t *eframe);
int ddb_entry_trap(int level, db_regs_t *eframe);
int ddb_break_trap(int type, db_regs_t *eframe);
int ddb_softcall_trap(int level, db_regs_t *eframe);
int ddb_nmi_trap(int level, db_regs_t *eframe);
void gimmeabreak(void);
void db_printing(void);
unsigned int db_check_user_addr(vm_offset_t va);
unsigned int db_phy_eq(task_t t1, task_t t2, vm_offset_t va1, vm_offset_t va2);
unsigned int db_check_access(task_t task, vm_offset_t va, unsigned int size);
void cpu_interrupt_to_db(int cpu_no);
void kdb_kintr(void);
extern struct db_command db_machine_cmds[];
void kdb_init(void);
char *db_task_name(task_t task);

/*
 * Prototypes from "luna88k/db_sstep.c"
 */
boolean_t inst_branch(unsigned ins);
#ifndef inst_call
 boolean_t inst_call(unsigned ins);
#endif
#ifndef inst_return
 boolean_t inst_return(unsigned ins);
#endif
#ifndef inst_unconditional_flow_transfer
 boolean_t inst_unconditional_flow_transfer(unsigned ins);
#endif
unsigned inst_load(unsigned ins);
unsigned inst_store(unsigned ins);
boolean_t inst_delayed(unsigned ins);
db_addr_t next_instr_address(db_addr_t pc, unsigned delay_slot, task_t task);
unsigned branch_taken(
    unsigned inst,
    unsigned pc,
    unsigned (*func)(),
    unsigned char *func_data);
db_expr_t getreg_val(unsigned regno, db_regs_t *frame);

/*
 * Prototypes from "luna88k/db_trace.c"
 */
int frame_is_sane(db_regs_t *regs);
char *m88k_exception_name(unsigned int vector);
void db_stack_trace_cmd(
    db_regs_t *addr,
    int have_addr,
    db_expr_t count,
    char *modif);


#endif LUNA_DB_MACHDEP_H_
