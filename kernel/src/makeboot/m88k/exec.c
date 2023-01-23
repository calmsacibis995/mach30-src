/* 
 * Mach Operating System
 * Copyright (c) 1992,1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */

/*
 * HISTORY
 * $Log:	exec.c,v $
 * Revision 2.4  93/03/09  18:02:05  danner
 * 	replaced
 * 	[93/03/09            jfriedl]
 * 
 * 	Added include of m88k/exec.h.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.3  93/02/01  09:54:46  danner
 * 	Fix includes.
 * 
 * Revision 2.2  92/08/03  17:59:39  jfriedl
 * 	moved from luna88k. [danner]
 * 
 * Revision 2.2  92/02/18  18:04:01  elf
 * 	Correctly handle symbol offset table size in 407 case.
 * 	413 support.
 * 	[92/01/21            danner]
 * 
 */

/*
 * Executable file format for m88k.
 */
#include <sys/types.h>
#include <sys/file.h>
#include <a.out.h>

#include <mach/machine/vm_types.h>
#include <mach/boot_info.h>

off_t
exec_header_size()
{
	return sizeof(struct exec);
}

/*
  The boot loader on the luna appears to load the symbol table 
  (anything that follows it) at the end of the bss.
  So just reguritating the kernel header should be good enough.
  (We may, in fact, have to modify the kernel symbol table size
  to get it work right.
*/

void
write_exec_header(out_file, kp, file_size)
	int		   out_file;	/* output file */
	struct loader_info *kp;		/* kernel load info */
	off_t		   file_size;	/* size of output file */
{
	struct exec	out_header;

	out_header.a_magic = OMAGIC; 
	out_header.a_text  = (int) file_size - sizeof(struct exec);
	out_header.a_data = 0;
	out_header.a_syms = 0;
	out_header.a_bss  = 0;
	out_header.a_trsize = 0;
	out_header.a_drsize = 0;
	out_header.a_entry  = kp->entry_1;

	lseek(out_file, (off_t) 0, L_SET);
	write(out_file, (char *)&out_header, sizeof(out_header));
}

#define SEGMENT_ALIGN   0x20000         /* align data seg to this boundry */
					/* for 0413 images */


/*
 *		ex_get_header			EXPORTED function
 *
 *		Reads the exec header for the loader's benefit
 *
 */

int ex_get_header(in_file, is_kernel, lp, 
		  sym_header, sym_header_size)
	int	in_file;
	int	is_kernel;
	struct loader_info *lp;
	char	*sym_header;		/* OUT */
	int	*sym_header_size;	/* OUT */
{
  struct exec		x;
  vm_offset_t		str_size;
  
  lseek(in_file, (off_t) 0, L_SET);
  read(in_file, (char *)&x, sizeof(x));

  if (x.a_machtype != M_88000)
    return 0;
  
  switch (x.a_magic)
    {
    case 0407:
      lp->text_start  = 0;
      lp->text_size   = x.a_text;
      lp->text_offset = sizeof(struct exec);

      lp->data_start  = x.a_text;
      lp->data_size   = x.a_data;
      lp->data_offset = lp->text_offset + x.a_text;

      lp->bss_size    = x.a_bss;

      lp->sym_size    = x.a_syms;
      lp->sym_offset  = lp->data_offset + lp->data_size;
      break;

    case 0413:
      if (x.a_text == 0) 
	return 0;

      lp->text_start  = 0;
      lp->text_size   = x.a_text;
      lp->text_offset = 0; /* the a.out header is included in text */

#ifdef OMRON
#define ROUND_SEG(x) (((x)+SEGMENT_ALIGN-1) & ~(SEGMENT_ALIGN-1))
#else
#define ROUND_SEG(x) ((x+SEGMENT_ALIGN) & ~(SEGMENT_ALIGN-1))
#endif

      lp->data_start  = ROUND_SEG(x.a_text);
      lp->data_size   = x.a_data;
      lp->data_offset = x.a_text;

      lp->bss_size    = x.a_bss;

      lp->sym_size    = x.a_syms;
      lp->sym_offset  = lp->data_offset + lp->data_size;
      break;

#if 0 /* dont trust this yet */
    case 0410:
#endif
      
    default:
      return 0;
    }

  lp->entry_1     = x.a_entry;
  lp->entry_2	  = 0;

  /*
   * Read string table size.
   */
  lseek(in_file, (off_t) (lp->sym_offset+x.a_syms), L_SET);
  read(in_file, (char *)&str_size, sizeof(str_size));

  *(int *)sym_header = x.a_syms;
  *sym_header_size = sizeof(int);

  lp->sym_size = x.a_syms + str_size;

  return 1;
}
