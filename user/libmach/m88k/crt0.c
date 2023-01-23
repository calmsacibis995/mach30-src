/*   
 *   Mach Operating System
 *   Copyright (c) 1991, 1992 Carnegie Mellon University
 *   Copyright (c) 1991, 1992 Omron Corporation
 *   All Rights Reserved.
 *   
 *   Permission to use, copy, modify and distribute this software and its
 *   documentation is hereby granted, provided that both the copyright
 *   notice and this permission notice appear in all copies of the
 *   software, derivative works or modified versions, and any portions
 *   thereof, and that both notices appear in supporting documentation.
 *   
 *   CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 *   CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 *   ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *   
 *   Carnegie Mellon requests users of this software to return to
 *   
 *    Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *    School of Computer Science
 *    Carnegie Mellon University
 *    Pittsburgh PA 15213-3890
 *   
 *   any improvements or extensions that they make and grant Carnegie Mellon 
 *   the rights to redistribute these changes.
 */

/* 
 *   Author :   Jeffrey Friedl
 *   Created:   July 1992
 *   Standalone crt0.
 */

/* 
 * HISTORY
 *  $Log:	crt0.c,v $
 * Revision 2.3  93/08/03  12:35:41  mrt
 * 	Added startup routine for gprof entry.
 * 	[93/08/02  16:57:32  bershad]
 * 
 * Revision 2.2  92/08/03  18:03:00  jfriedl
 * 	Flushed silly version that was here.
 * 	Brought my normal crt0.c and updated for 3.0
 * 	[92/08/02            jfriedl]
 * 
 */


/*
 * GCCisms used:
 * 	A "volatile void fcn()" is one that never returns.
 *	register var asm("r1"): variable VAR is raw access to named register.
 */

/* The following are for external consumption */
char    **environ = (char **)0;
int	errno = 0;

/*
 * Upon linking, the following either remain NULL or assume a value.
 * If they remain NULL, we'll not want to call them (obviously).
 * Otherwise, we will.
 */
void	 (*mach_init_routine)(void);
unsigned (*_cthread_init_routine)(void); /* returns a stack */
volatile void (*_cthread_exit_routine)(unsigned exit_val);
void	 (*_monstartup_routine)();

/*
 * When a program begins, r31 points to info passed from the kernel.
 *
 * The following shows the memory to which r31 points (think "int *r31;"),
 * and how we derive argc, argv, and envp from that:
 *
 *    +-------------------+ <-------------------------------------- r31
 *    | ARGC              | <- argc = r31[0];
 *    +-------------------+ <- argv = &r31[1];
 *    | &(argument #1)    |
 *    +-------------------+
 *    | &(argument #2)    |
 *     -  - - - - - - -  - 
 *    | &(argument #ARGC) |
 *    +-------------------+
 *    | 0x00000000        | <- end-of-ARGV-list marker (redundant information).
 *    +-------------------+ <- environ = envp =  &argv[argc+1];
 *    | &(env. var. #1)   |
 *    +-------------------+
 *    | &(env. var. #2)   |
 *     -  - - - - - - -  - 
 *    | &(env. var. #N)   |
 *    +-------------------+
 *    | 0x00000000        | <- end-of-ENVP-list marker (not redundant!).
 *    +-------------------+
 *
 * We use 'start:' to grab r31 and and call real_start(argc, argv, envp).
 * We must do this since the function prologue makes finding the initial
 * r31 difficult in C.
 */
asm("       text                                                             ");
asm("       align  4                                                         ");
asm("start: global start                                                     ");
asm("       ld     r2, r31,   0    ;; First arg to real_start: argc          ");
asm("       addu   r3, r31,   4    ;; Second arg to real_start: argv         ");
asm("       lda    r4,  r3  [r2]   ;; Third arg to real_start: envp, but.... ");
asm("       addu   r4,  r4,   4    ;;   ... don't forget to skip past marker ");
asm("       br.n   ___crt0_real_start                                        ");
asm("       subu   r31, r31, 32                                              ");

extern unsigned char _eprol;
extern unsigned char etext;


static void volatile __crt0_real_start(argc, argv, envp)
register int   argc;
register char *argv[];
register char *envp[];
{
    extern volatile void exit(unsigned exit_val);
    extern unsigned main(unsigned argc, char **argv, char **envp);


    environ = envp; /* environ is for the user that can't get at 'envp' */

    /* if mach_init_routine() got linked in, call it */
    if (mach_init_routine)
	(void) (*mach_init_routine)();

asm("__eprol: global __eprol");

    /* if _cthread_init_routine() got linked in, call it */
    if (_cthread_init_routine)
    {
	/* it returns a stack... use it (make sure to align). */
	register unsigned stack asm("r31"); /* GCCism: 'stack' is raw r31 */
	stack = (*_cthread_init_routine)() & ~7;
    }

    if (_monstartup_routine)  {
	(*_monstartup_routine)();
     }

    /*
     * If _cthread_exit_routine() got linked in, use it rather than exit()
     * 
     * 
     * I wonder if the following shouldn't really be
     *
     *	 ret = main(argc,argv,envp);
     *   (_cthread_exit_routine ? _cthread_exit_routine : exit)(ret);
     *
     * which would allow _cthread_exit_routine to be modified during
     * execution.....?
     *
     */
    (_cthread_exit_routine ? _cthread_exit_routine : exit)
						    (main(argc, argv, envp));

    /*NOTREACHED*/
}
