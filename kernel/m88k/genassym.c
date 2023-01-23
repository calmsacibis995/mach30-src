/* 
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
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
 * $Log:	genassym.c,v $
 * Revision 2.3  93/01/26  18:05:10  danner
 * 	tidied up.
 * 	[93/01/24            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:47:48  jfriedl
 * 	Created m88k copy from luna [danner].
 * 	[92/07/24            jfriedl]
 * 
 * Revision 2.5  92/02/18  18:01:26  elf
 * 	Added DISP_MIN to correspond to disp_min field added in the
 * 	 emulation vector definition introducted in MK68.
 * 	[92/01/17            danner]
 * 
 * Revision 2.4  91/08/24  12:09:54  af
 * 	Updated to reflect new exception frame thread /
 * 	 luna88k_saved_state structure. Added sanity checking code to
 * 	 make sure the thread structure is an integral multiple of double
 * 	 words. 
 * 	[91/08/07            danner]
 * 
 * Revision 2.3.3.1  91/08/19  13:46:35  danner
 * 	Updated to reflect new exception frame thread /
 * 	 luna88k_saved_state structure. Added sanity checking code to
 * 	 make sure the thread structure is an integral multiple of double
 * 	 words. 
 * 	[91/08/07            danner]
 * 
 * Revision 2.3  91/07/11  11:00:46  danner
 * 	Copyright Fixes
 * 
 * Revision 2.2  91/07/09  23:17:22  danner
 * 	Added generation of ENTRY_ASM, the assembly instruction to
 * 	 enter the debugger.
 * 	[91/07/08            danner]
 * 
 * 	Added THREAD_SWAP_FUNC for switch_to_shutdown_context
 * 	[91/06/25            danner]
 * 
 */

#include <motorola/m88k/m88100/pcb.h>	/* m88100_saved_state, pcb_t */
#include <kern/thread.h>		/* thread_t */
#include <kern/task.h>			/* task_t */
#include <kern/syscall_emulation.h>	/* eml_dispatch_t */
#include <mach/message.h>		/* MACH_SEND_INTERRUPTED */
#include <luna88k/db_machdep.h>		/* ENTRY_ASM */

#define pair(TOKEN, ELEMENT) \
    printf("#define " TOKEN " %u\n", (unsigned)(ELEMENT))

#define int_offset_of_element(ELEMENT) (((unsigned)&(ELEMENT))/sizeof(int))

extern void printf();
extern int write();

int main()
{
    struct m88100_saved_state *ss = (struct m88100_saved_state *) 0;
    struct thread *	thread = (struct thread *) 0;
    struct task *	task = (struct task *) 0;
    pcb_t	        pcb = (pcb_t) 0;
    eml_dispatch_t	eml = (eml_dispatch_t) 0;

    printf("#ifndef __GENASSYM_INCLUDED\n");
    printf("#define __GENASSYM_INCLUDED 1\n\n");

    printf("#ifdef ASSEMBLER\n"
	   "#define NEWLINE \\\\ \n"
           "#endif\n");	

    pair("THREAD_KERNEL_STACK",	&thread->kernel_stack);
    pair("THREAD_SWAP_FUNC",	&thread->swap_func);
    pair("THREAD_PCB",		&thread->pcb);
    pair("THREAD_RECOVER",	&thread->recover);
    pair("THREAD_TASK",		&thread->task);
    pair("TASK_EMUL",		&task->eml_dispatch);
    pair("DISP_COUNT",		&eml->disp_count);
    pair("DISP_MIN",		&eml->disp_min);
    pair("DISP_VECTOR",		&eml->disp_vector[0]);
    pair("PCB_USER",		&pcb->user_state);
    pair("PCB_KERNEL",		&pcb->kernel_state);
    
    /* 
     * Needed in the exception handler. These are in ifdef assembler
     * Because they are duplications of constants found in other headers
     * that can't be included by assembly routines. 
     */
    printf("#ifdef ASSEMBLER\n");
     pair("MACH_SEND_INTERRUPTED", MACH_SEND_INTERRUPTED);
     printf("#define ENTRY_ASM	%s\n", ENTRY_ASM);
    printf("#endif /* ASSEMBLER */\n");

    /* these were all originally hard coded in m88k.h */
    pair("EF_R0",	int_offset_of_element(ss->r[0]));
    pair("EF_R31",	int_offset_of_element(ss->r[31]));
    pair("EF_FPSR",	int_offset_of_element(ss->fpsr));
    pair("EF_FPCR",	int_offset_of_element(ss->fpcr));
    pair("EF_EPSR",	int_offset_of_element(ss->epsr));
    pair("EF_SXIP",	int_offset_of_element(ss->sxip));
    pair("EF_SFIP",	int_offset_of_element(ss->sfip));
    pair("EF_SNIP",	int_offset_of_element(ss->snip));
    pair("EF_SSBR",	int_offset_of_element(ss->ssbr));
    pair("EF_DMT0",	int_offset_of_element(ss->dmt0));
    pair("EF_DMD0",	int_offset_of_element(ss->dmd0));
    pair("EF_DMA0",	int_offset_of_element(ss->dma0));
    pair("EF_DMT1",	int_offset_of_element(ss->dmt1));
    pair("EF_DMD1",	int_offset_of_element(ss->dmd1));
    pair("EF_DMA1",	int_offset_of_element(ss->dma1));
    pair("EF_DMT2",	int_offset_of_element(ss->dmt2));
    pair("EF_DMD2",	int_offset_of_element(ss->dmd2));
    pair("EF_DMA2",	int_offset_of_element(ss->dma2));
    pair("EF_FPECR",	int_offset_of_element(ss->fpecr));
    pair("EF_FPHS1",	int_offset_of_element(ss->fphs1));
    pair("EF_FPLS1",	int_offset_of_element(ss->fpls1));
    pair("EF_FPHS2",	int_offset_of_element(ss->fphs2));
    pair("EF_FPLS2",	int_offset_of_element(ss->fpls2));
    pair("EF_FPPT",	int_offset_of_element(ss->fppt));
    pair("EF_FPRH",	int_offset_of_element(ss->fprh));
    pair("EF_FPRL",	int_offset_of_element(ss->fprl));
    pair("EF_FPIT",	int_offset_of_element(ss->fpit));
    pair("EF_VECTOR",	int_offset_of_element(ss->vector));
    pair("EF_MASK",	int_offset_of_element(ss->mask));
    pair("EF_MODE",	int_offset_of_element(ss->mode));

    /* next: I know, I know */
    pair("EF_RET",	int_offset_of_element(ss->scratch1));
    pair("EF_NREGS",	sizeof(*ss)/sizeof(int));

    /* make a sanity check */
    if (sizeof(*ss) & 7)
    {
	/* 
	 * This contortion using write instead of fputs(stderr)
	 * is necessary because we can't include stdio.h in here.
	 */
	static char buf[] = 
	  "Exception frame not a multiple of double words\n";
	write(2 /* stderr */,buf,sizeof(buf));
	exit(1);
    }
    pair("SIZEOF_EF", sizeof(*ss));

    printf("\n#endif /* __GENASSYM_INCLUDED */\n");
    return 0;
}
