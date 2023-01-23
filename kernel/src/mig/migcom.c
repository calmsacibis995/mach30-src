/* 
 * Mach Operating System
 * Copyright (c) 1993,1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	migcom.c,v $
 * Revision 2.3  93/05/10  17:49:23  rvb
 * 	Fix include to use < vs " for new ode shadowing
 * 	[93/05/10  10:28:09  rvb]
 * 
 * Revision 2.2  93/02/04  13:26:42  mrt
 * 	renamed from mig.c
 * 
 * 
 * Revision 2.7  93/01/14  17:58:20  danner
 * 	Converted file to ANSI C.
 * 	[92/12/08            pds]
 * 
 * Revision 2.6  91/08/28  11:17:04  jsb
 * 	Removed TrapRoutine support.
 * 	[91/08/12            rpd]
 * 
 * Revision 2.5  91/06/25  10:31:32  rpd
 * 	Added ServerHeaderFileName and -sheader.
 * 	[91/05/22            rpd]
 * 
 * Revision 2.4  91/02/05  17:55:02  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:54:42  mrt]
 * 
 * Revision 2.3  90/06/19  23:01:01  rpd
 * 	Added prefix argument to -i option.
 * 	Replaced GenIndividualUser with UserFilePrefix.
 * 	[90/06/03            rpd]
 * 
 * Revision 2.2  90/06/02  15:04:59  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:11:47  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 *  8-Feb-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added -user, -server, and -header switches to name output files.
 *	Added -i switch to write individual files for user routines.
 *
 * 17-Aug-87  Bennet Yee (bsy) at Carnegie-Mellon University
 *	Added -s,-S switches for generating a SymTab
 *
 *  3-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Removed -t,-T switch as code is now the same for
 *	multi and single threaded use.
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

/*
 *	Switches are;
 *		-[v,Q]  verbose or not quiet:  prints out type
 *			and routine information as mig runs.
 *		-[V,q]  not verbose or quiet : don't print 
 *			information during compilation
 *			(this is the default)
 *		-[r,R]  do or don't use rpc calls instead of 
 *			send/receive pairs. Default is -r.
 *		-[s,S]	generate symbol table or not:  generate a
 *			table of rpc-name, number, routine triplets
 *			as an external data structure -- main use is
 *			for protection system's specification of rights
 *			and for protection dispatch code.  Default is -s.
 *		-i <prefix>
 *			Put each user routine in its own file.  The
 *			file is named <prefix><routine-name>.c.
 *		-user <name>
 *			Name the user-side file <name>
 *		-server <name>
 *			Name the server-side file <name>
 *		-header <name>
 *			Name the user-side header file <name>
 *		-iheader <name>
 *			Name the user-side internal header file <name>
 *		-sheader <name>
 *			NAme the server-side header file <name>
 *
 *  DESIGN:
 *	Mig uses a lexxer module created by lex from lexxer.l and
 *	a parser module created by yacc from parser.y to parse an
 *	interface definitions module for a mach server.
 *	The parser module calls routines in statement.c
 *	and routines.c to build a list of statement structures.
 *	The most interesting statements are the routine definitions
 *	which contain information about the name, type, characteristics
 *	of the routine, an argument list containing information for
 *	each argument type, and a list of special arguments. The
 *	argument type structures are build by routines in type.c
 *	Once parsing is completed, the three code generation modules:
 *	header.c user.c and server.c are called sequentially. These
 *	do some code generation directly and also call the routines
 *	in utils.c for common (parameterized) code generation.
 *	
 */

#include <stdio.h>
#include <error.h>
#include <lexxer.h>
#include <global.h>
#include <write.h>

extern int yyparse();
static FILE *myfopen(const char *name, const char *mode);

static void
parseArgs(int argc, char **argv)
{
    while (--argc > 0)
	if ((++argv)[0][0] == '-') 
	{
	    switch (argv[0][1]) 
	    {
	      case 'q':
		BeQuiet = TRUE;
		break;
	      case 'Q':
		BeQuiet = FALSE;
		break;
	      case 'v':
		BeVerbose = TRUE;
		break;
	      case 'V':
		BeVerbose = FALSE;
		break;
	      case 'r':
		UseMsgRPC = TRUE;
		break;
	      case 'R':
		UseMsgRPC = FALSE;
		break;
	      case 's':
		if (streql(argv[0], "-server"))
		{
		    --argc; ++argv;
		    if (argc == 0)
			fatal("missing name for -server option");
		    ServerFileName = strmake(argv[0]);
		}
		else if (streql(argv[0], "-sheader"))
		{
		    --argc; ++argv;
		    if (argc == 0)
		      fatal ("missing name for -sheader option");
		    ServerHeaderFileName = strmake(argv[0]);
		}
		else
		    GenSymTab = TRUE;
		break;
	      case 'S':
	        GenSymTab = FALSE;
		break;
	      case 'i':
		if (streql(argv[0], "-iheader"))
		{
		    --argc; ++argv;
		    if (argc == 0)
			fatal("missing name for -iheader option");
		    InternalHeaderFileName = strmake(argv[0]);
		}
		else
		{
		    --argc; ++argv;
		    if (argc == 0)
			fatal("missing prefix for -i option");
		    UserFilePrefix = strmake(argv[0]);
		}
		break;
	      case 'u':
		if (streql(argv[0], "-user"))
		{
		    --argc; ++argv;
		    if (argc == 0)
			fatal("missing name for -user option");
		    UserFileName = strmake(argv[0]);
		}
		else
		    fatal("unknown flag: '%s'", argv[0]);
		break;
	      case 'h':
		if (streql(argv[0], "-header"))
		{
		    --argc; ++argv;
		    if (argc == 0)
			fatal("missing name for -header option");
		    UserHeaderFileName = strmake(argv[0]);
		}
		else
		    fatal("unknown flag: '%s'", argv[0]);
		break;
	      default:
		fatal("unknown flag: '%s'", argv[0]);
		/*NOTREACHED*/
	    }
	}
	else
	    fatal("bad argument: '%s'", *argv);
}

void
main(int argc, char **argv)
{
    FILE *uheader, *server, *user;
    FILE *iheader, *sheader;

    set_program_name("mig");
    parseArgs(argc, argv);
    init_global();
    init_type();

    LookNormal();
    (void) yyparse();

    if (errors > 0)
	exit(1);

    more_global();

    uheader = myfopen(UserHeaderFileName, "w");
    if (!UserFilePrefix)
	user = myfopen(UserFileName, "w");
    server = myfopen(ServerFileName, "w");
    if (ServerHeaderFileName)
	sheader = myfopen(ServerHeaderFileName, "w");
    if (IsKernelServer)
    {
	iheader = myfopen(InternalHeaderFileName, "w");
    }

    if (BeVerbose)
    {
	printf("Writing %s ... ", UserHeaderFileName);
	fflush(stdout);
    }
    WriteUserHeader(uheader, StatementList);
    fclose(uheader);
    if (ServerHeaderFileName)
    {
	if (BeVerbose)
	{
	    printf ("done.\nWriting %s ...", ServerHeaderFileName);
	    fflush (stdout);
	}
	WriteServerHeader(sheader, StatementList);
	fclose(sheader);
    }
    if (IsKernelServer)
    {
	if (BeVerbose)
	{
	    printf("done.\nWriting %s ... ", InternalHeaderFileName);
	    fflush(stdout);
	}
	WriteInternalHeader(iheader, StatementList);
	fclose(iheader);
    }
    if (UserFilePrefix)
    {
	if (BeVerbose)
	{
	    printf("done.\nWriting individual user files ... ");
	    fflush(stdout);
	}
	WriteUserIndividual(StatementList);
    }
    else
    {
	if (BeVerbose)
	{
	    printf("done.\nWriting %s ... ", UserFileName);
	    fflush(stdout);
	}
	WriteUser(user, StatementList);
	fclose(user);
    }
    if (BeVerbose)
    {
	printf("done.\nWriting %s ... ", ServerFileName);
	fflush(stdout);
    }
    WriteServer(server, StatementList);
    fclose(server);
    if (BeVerbose)
	printf("done.\n");

    exit(0);
}

static FILE *
myfopen(const char *name, const char *mode)
{
    const char *realname;
    FILE *file;

    if (name == strNULL)
	realname = "/dev/null";
    else
	realname = name;

    file = fopen(realname, mode);
    if (file == NULL)
	fatal("fopen(%s): %s", realname, unix_error_string(errno));

    return file;
}
