/*
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
 * $Log:	exec.c,v $
 * Revision 2.2  92/08/03  17:20:16  jfriedl
 * 	Created from luna version. [danner]
 * 
 * Revision 2.4  92/04/01  10:54:01  rpd
 * 	Initial stack alignment should be doubleword (8 byte).
 * 	[92/03/20            danner]
 * 
 * Revision 2.3  92/03/01  15:14:46  rpd
 * 	Removed struct exechdr, moved related definitions to exec.h.
 * 	[92/03/01            rpd]
 * 
 * Revision 2.2  92/02/19  13:32:56  elf
 * 	Moved out of kernel.
 * 	[92/01/17            danner]
 * 
 */

#include <mach/machine/thread_status.h>

#include <mach.h>
#include <mach/machine/vm_param.h>

#include "file_io.h"
#include "loader_info.h"

#include "m88k/exec.h"

/*
 *		ex_get_header			EXPORTED function
 *
 *		Reads the exec header for the loader's benefit
 *
 */
int ex_get_header(ip, lp)
	struct file		*ip;
	struct loader_info	*lp;
{
  struct exec	x;
  int	result;
  vm_size_t	resid;
  
  
  result = read_file(ip, 0, (vm_offset_t)&x, sizeof(x), &resid);
  if (result)
    return (result);
  
  if (resid || x.a_machtype != M_88000)
    return (EX_NOT_EXECUTABLE);
  
  switch (x.a_magic)
    {
    case 0407:
      lp->text_start  = 0;
      lp->text_size   = 0;
      lp->text_offset = 0;
      lp->data_start  = 0;
      lp->data_size   = x.a_text + x.a_data;
      lp->data_offset = 0;
      lp->bss_size    = x.a_bss;
    break;

    case 0410:
    case 0413:
      if (x.a_text == 0) {
	return(EX_NOT_EXECUTABLE);
      }
      lp->text_start  = 0;
      lp->text_size   = x.a_text;
      lp->text_offset = 0;
      lp->data_start  = ROUND_SEG(x.a_text);
      lp->data_size   = x.a_data;
      lp->data_offset = x.a_text;
      lp->bss_size    = x.a_bss;
      break;
      
    default:
      return (EX_NOT_EXECUTABLE);
    }

  lp->entry_1     = x.a_entry;
  lp->entry_2	  = 0; /* used for _gp on mips */
  
  return 0;
}


/*
 *	Object:
 *		set_regs			EXPORTED function
 *
 *		Initialize enough state for a thread to run
 *
 */
#define STACK_SIZE (vm_size_t)(128*1024)

char *set_regs(user_task, user_thread, lp, arg_size)
	mach_port_t	user_task;
	mach_port_t	user_thread;
	struct loader_info *lp;
	int		arg_size;
{
	vm_offset_t	stack_start;
	vm_offset_t	stack_end;
	struct luna88k_thread_state	regs;

	unsigned int		reg_size;

	/*
	 * Allocate stack.
	 */
	stack_end = VM_MAX_ADDRESS;
	stack_start = stack_end - STACK_SIZE;
	(void)vm_allocate(user_task,
			  &stack_start,
			  (vm_size_t)(STACK_SIZE),
			  FALSE);

	reg_size = LUNA88K_THREAD_STATE_COUNT;
	(void)thread_get_state(user_thread,
				LUNA88K_THREAD_STATE,
				(thread_state_t)&regs,
				&reg_size);

	regs.sfip = 2 | (lp->entry_1 & ~3) ;
	regs.snip = 0;    /* NOP */
	regs.psr = 0x3f0; /* user, big endian */
	/* setup stack - align to double boundry */
	regs.r[31] = (int)((stack_end - arg_size) & ~(sizeof(double)-1));

	(void)thread_set_state(user_thread,
				LUNA88K_THREAD_STATE,
				(thread_state_t)&regs,
				reg_size);

	return ((char *)regs.r[31]);
}

boolean_t
get_symtab(fp, symoff_p, symsize_p, header, header_size)
	struct file	*fp;
	vm_offset_t	*symoff_p;	/* out */
	vm_size_t	*symsize_p;	/* out */
	char		header[];	/* out array */
	vm_size_t	*header_size;	/* out */
{
	register int		result;
	vm_offset_t		resid;
	vm_offset_t		sym_off;
	vm_offset_t		str_off;
	vm_size_t		sym_size;
	vm_size_t		str_size;
	struct exec		x;

	result = read_file(fp, 0, (vm_offset_t)&x, sizeof(x), &resid);
	if (result || resid)
	    return (FALSE);

	sym_off = x.a_text + x.a_data + x.a_trsize + x.a_drsize;
	sym_size = x.a_syms;
	str_off  = sym_off + sym_size;
	result = read_file(fp, str_off,
			&str_size, sizeof(int), &resid);
	if (result || resid)
	    return (FALSE);

	/*
	 * Return the entire symbol table + string table.
	 * Add a header:
	 *   size of symbol table
	 */

	*symoff_p = sym_off;
	*symsize_p = sym_size + str_size;
	*(int *)header = sym_size;
	*header_size = sizeof(int);

	return (TRUE);
}
