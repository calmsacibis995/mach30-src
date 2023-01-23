/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log:	global.h,v $
 * Revision 2.9  93/05/10  17:49:10  rvb
 * 	Fix include to use < vs " for new ode shadowing
 * 	[93/05/10  10:27:41  rvb]
 * 
 * Revision 2.8  93/01/14  17:58:00  danner
 * 	Made text on #endif lines into comments.
 * 	[92/12/14            pds]
 * 	Converted file to ANSI C.
 * 	[92/12/08            pds]
 * 
 * Revision 2.7  91/08/28  11:16:56  jsb
 * 	Replaced ServerProcName with ServerDemux.
 * 	[91/08/13            rpd]
 * 
 * 	Removed Camelot and TrapRoutine support.
 * 	Changed MsgKind to MsgSeqno.
 * 	[91/08/12            rpd]
 * 
 * Revision 2.6  91/06/26  14:39:32  rpd
 * 	Removed InitRoutineName.
 * 	[91/06/26            rpd]
 * 
 * Revision 2.5  91/06/25  10:31:12  rpd
 * 	Added ServerHeaderFileName.
 * 	[91/05/22            rpd]
 * 
 * Revision 2.4  91/02/05  17:54:29  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:54:12  mrt]
 * 
 * Revision 2.3  90/06/19  23:00:54  rpd
 * 	Added UserFilePrefix.
 * 	[90/06/03            rpd]
 * 
 * Revision 2.2  90/06/02  15:04:42  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:10:53  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 17-Sep-87  Bennet Yee (bsy) at Carnegie-Mellon University
 *	Added GenSymTab
 *
 * 16-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Added CamelotPrefix
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_GLOBAL_H
#define	_GLOBAL_H

#define	EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <sys/types.h>
#include <string.h>

extern boolean_t BeQuiet;	/* no warning messages */
extern boolean_t BeVerbose;	/* summarize types, routines */
extern boolean_t UseMsgRPC;
extern boolean_t GenSymTab;

extern boolean_t IsKernelUser;
extern boolean_t IsKernelServer;

extern const_string_t RCSId;

extern const_string_t SubsystemName;
extern u_int SubsystemBase;

extern const_string_t MsgOption;
extern const_string_t WaitTime;
extern const_string_t ErrorProc;
extern const_string_t ServerPrefix;
extern const_string_t UserPrefix;
extern const_string_t ServerDemux;

extern int yylineno;
extern string_t yyinname;

extern void init_global(void);

extern string_t UserFilePrefix;
extern string_t UserHeaderFileName;
extern string_t ServerHeaderFileName;
extern string_t InternalHeaderFileName;
extern string_t UserFileName;
extern string_t ServerFileName;

extern void more_global(void);

extern const char LintLib[];

#endif	/* _GLOBAL_H */
