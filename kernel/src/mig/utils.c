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
 * $Log:	utils.c,v $
 * Revision 2.9  93/05/10  17:50:19  rvb
 * 	Fix include to use < vs " for new ode shadowing
 * 	[93/05/10  10:30:03  rvb]
 * 
 * Revision 2.8  93/01/14  17:59:30  danner
 * 	Converted file and generated code to ANSI C.
 * 	Changed code to always use static message types.
 * 	[92/12/08            pds]
 * 
 * 	Fixed bug checking for the Hi-C compiler: hc -> __HC__.
 * 	Made text on #endif/#else lines into comments.
 * 	[92/08/06            pds]
 * 
 * Revision 2.7  92/01/14  16:47:08  rpd
 * 	Modified WriteTypeDeclIn and WriteTypeDeclOut to disable
 * 	the deallocate flag on Indefinite arguments.
 * 	[92/01/09            rpd]
 * 
 * Revision 2.6  92/01/03  20:30:51  dbg
 * 	Change argByReferenceUser and argByReferenceServer to fields in
 * 	argument_t.
 * 	[91/08/29            dbg]
 * 
 * Revision 2.5  91/07/31  18:11:45  dbg
 * 	Accept new dealloc_t argument type in WriteStaticDecl,
 * 	WritePackMsgType.
 * 
 * 	Don't need to zero last character of C string.  Mig_strncpy does
 * 	the proper work.
 * 
 * 	Add SkipVFPrintf, so that WriteCopyType doesn't print fields in
 * 	comments.
 * 	[91/07/17            dbg]
 * 
 * Revision 2.4  91/06/25  10:32:36  rpd
 * 	Changed WriteVarDecl to WriteUserVarDecl.
 * 	Added WriteServerVarDecl.
 * 	[91/05/23            rpd]
 * 
 * Revision 2.3  91/02/05  17:56:28  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:56:39  mrt]
 * 
 * Revision 2.2  90/06/02  15:06:11  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:14:54  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 21-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Added deallocflag to the WritePackMsg routines.
 *
 * 29-Jul-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Changed WriteVarDecl to not automatically write
 *	semi-colons between items, so that it can be
 *	used to write C++ argument lists.
 *
 * 27-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#include <mach/message.h>
#include <stdarg.h>
#include <write.h>
#include <utils.h>

extern int vfprintf(FILE *file, const char *fmt, va_list);

void
WriteImport(FILE *file, const_string_t filename)
{
    fprintf(file, "#include %s\n", filename);
}

void
WriteRCSDecl(FILE *file, identifier_t name, const_string_t rcs)
{
    fprintf(file, "#ifndef\tlint\n");
    fprintf(file, "#if\tUseExternRCSId\n");
    fprintf(file, "char %s_rcsid[] = %s;\n", name, rcs);
    fprintf(file, "#else\t/* UseExternRCSId */\n");
    fprintf(file, "static char rcsid[] = %s;\n", rcs);
    fprintf(file, "#endif\t/* UseExternRCSId */\n");
    fprintf(file, "#endif\t/* lint */\n");
    fprintf(file, "\n");
}

void
WriteBogusDefines(FILE *file)
{
    fprintf(file, "#ifndef\tmig_internal\n");
    fprintf(file, "#define\tmig_internal\tstatic\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");

    fprintf(file, "#ifndef\tmig_external\n");
    fprintf(file, "#define mig_external\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");

    fprintf(file, "#ifndef\tTypeCheck\n");
    fprintf(file, "#define\tTypeCheck 1\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");

    fprintf(file, "#ifndef\tUseExternRCSId\n");
    fprintf(file, "#define\tUseExternRCSId\t\t1\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");
}

void
WriteList(FILE *file, const argument_t *args, write_list_fn_t *func, u_int mask,
	  const char *between, const char *after)
{
    register const argument_t *arg;
    register boolean_t sawone = FALSE;

    for (arg = args; arg != argNULL; arg = arg->argNext)
	if (akCheckAll(arg->argKind, mask))
	{
	    if (sawone)
		fprintf(file, "%s", between);
	    sawone = TRUE;

	    (*func)(file, arg);
	}

    if (sawone)
	fprintf(file, "%s", after);
}

static boolean_t
WriteReverseListPrim(FILE *file, register const argument_t *arg,
		     write_list_fn_t *func, u_int mask, const char *between)
{
    boolean_t sawone = FALSE;

    if (arg != argNULL)
    {
	sawone = WriteReverseListPrim(file, arg->argNext, func, mask, between);

	if (akCheckAll(arg->argKind, mask))
	{
	    if (sawone)
		fprintf(file, "%s", between);
	    sawone = TRUE;

	    (*func)(file, arg);
	}
    }

    return sawone;
}

void
WriteReverseList(FILE *file, const argument_t *args, write_list_fn_t *func,
		 u_int mask, const char *between, const char *after)
{
    boolean_t sawone;

    sawone = WriteReverseListPrim(file, args, func, mask, between);

    if (sawone)
	fprintf(file, "%s", after);
}

void
WriteNameDecl(FILE *file, const argument_t *arg)
{
    fprintf(file, "%s", arg->argVarName);
}

void
WriteUserVarDecl(FILE *file, const argument_t *arg)
{
    const char *ref = arg->argByReferenceUser ? "*" : "";

    fprintf(file, "\t%s %s%s", arg->argType->itUserType, ref, arg->argVarName);
}

void
WriteServerVarDecl(FILE *file, const argument_t *arg)
{
    const char *ref = arg->argByReferenceServer ? "*" : "";
  
    fprintf(file, "\t%s %s%s",
	    arg->argType->itTransType, ref, arg->argVarName);
}

void
WriteTypeDeclIn(FILE *file, register const argument_t *arg)
{
    WriteStaticDecl(file, arg->argType,
		    arg->argType->itIndefinite ? d_NO : arg->argDeallocate,
		    arg->argLongForm, TRUE, arg->argTTName);
}

void
WriteTypeDeclOut(FILE *file, register const argument_t *arg)
{
    WriteStaticDecl(file, arg->argType,
		    arg->argType->itIndefinite ? d_NO : arg->argDeallocate,
		    arg->argLongForm, FALSE, arg->argTTName);
}

void
WriteCheckDecl(FILE *file, register const argument_t *arg)
{
    register const ipc_type_t *it = arg->argType;

    /* We'll only be called for short-form types.
       Note we use itOutNameStr instead of itInNameStr, because
       this declaration will be used to check received types. */

    fprintf(file, "\tstatic mach_msg_type_t %sCheck = {\n", arg->argVarName);
    fprintf(file, "\t\t/* msgt_name = */\t\t%s,\n", it->itOutNameStr);
    fprintf(file, "\t\t/* msgt_size = */\t\t%d,\n", it->itSize);
    fprintf(file, "\t\t/* msgt_number = */\t\t%d,\n", it->itNumber);
    fprintf(file, "\t\t/* msgt_inline = */\t\t%s,\n",
	    strbool(it->itInLine));
    fprintf(file, "\t\t/* msgt_longform = */\t\tFALSE,\n");
    fprintf(file, "\t\t/* msgt_deallocate = */\t\t%s,\n",
	    strbool(!it->itInLine));
    fprintf(file, "\t\t/* msgt_unused = */\t\t0\n");
    fprintf(file, "\t};\n");
}

const char *
ReturnTypeStr(const routine_t *rt)
{
    if (rt->rtReturn == argNULL)
	return "void";
    else
	return rt->rtReturn->argType->itUserType;
}

const char *
FetchUserType(const ipc_type_t *it)
{
    return it->itUserType;
}

const char *
FetchServerType(const ipc_type_t *it)
{
    return it->itServerType;
}

void
WriteFieldDeclPrim(FILE *file, const argument_t *arg,
		   const char *(*tfunc)(const ipc_type_t *))
{
    register const ipc_type_t *it = arg->argType;

    fprintf(file, "\t\tmach_msg_type_%st %s;\n",
	    arg->argLongForm ? "long_" : "", arg->argTTName);

    if (it->itInLine && it->itVarArray)
    {
	register ipc_type_t *btype = it->itElement;

	/*
	 *	Build our own declaration for a varying array:
	 *	use the element type and maximum size specified.
	 *	Note arg->argCount->argMultiplier == btype->itNumber.
	 */
	fprintf(file, "\t\t%s %s[%d];",
			(*tfunc)(btype),
			arg->argMsgField,
			it->itNumber/btype->itNumber);
    }
    else
	fprintf(file, "\t\t%s %s;", (*tfunc)(it), arg->argMsgField);

    if (it->itPadSize != 0)
	fprintf(file, "\n\t\tchar %s[%d];", arg->argPadName, it->itPadSize);
}

void
WriteStructDecl(FILE *file, const argument_t *args, write_list_fn_t *func,
		u_int mask, const char *name)
{
    fprintf(file, "\ttypedef struct {\n");
    fprintf(file, "\t\tmach_msg_header_t Head;\n");
    WriteList(file, args, func, mask, "\n", "\n");
    fprintf(file, "\t} %s;\n", name);
    fprintf(file, "\n");
}

static void
WriteStaticLongDecl(FILE *file, register const ipc_type_t *it,
		    dealloc_t dealloc, boolean_t inname, identifier_t name)
{
    fprintf(file, "\tstatic mach_msg_type_long_t %s = {\n", name);
    fprintf(file, "\t{\n");
    fprintf(file, "\t\t/* msgt_name = */\t\t0,\n");
    fprintf(file, "\t\t/* msgt_size = */\t\t0,\n");
    fprintf(file, "\t\t/* msgt_number = */\t\t0,\n");
    fprintf(file, "\t\t/* msgt_inline = */\t\t%s,\n",
	    strbool(it->itInLine));
    fprintf(file, "\t\t/* msgt_longform = */\t\tTRUE,\n");
    fprintf(file, "\t\t/* msgt_deallocate = */\t\t%s,\n",
	    strdealloc(dealloc));
    fprintf(file, "\t\t/* msgt_unused = */\t\t0\n");
    fprintf(file, "\t},\n");
    fprintf(file, "\t\t/* msgtl_name = */\t%s,\n",
	    inname ? it->itInNameStr : it->itOutNameStr);
    fprintf(file, "\t\t/* msgtl_size = */\t%d,\n", it->itSize);
    fprintf(file, "\t\t/* msgtl_number = */\t%d,\n", it->itNumber);
    fprintf(file, "\t};\n");
}

static void
WriteStaticShortDecl(FILE *file, register const ipc_type_t *it,
		     dealloc_t dealloc, boolean_t inname, identifier_t name)
{
    fprintf(file, "\tstatic mach_msg_type_t %s = {\n", name);
    fprintf(file, "\t\t/* msgt_name = */\t\t%s,\n",
	    inname ? it->itInNameStr : it->itOutNameStr);
    fprintf(file, "\t\t/* msgt_size = */\t\t%d,\n", it->itSize);
    fprintf(file, "\t\t/* msgt_number = */\t\t%d,\n", it->itNumber);
    fprintf(file, "\t\t/* msgt_inline = */\t\t%s,\n",
	    strbool(it->itInLine));
    fprintf(file, "\t\t/* msgt_longform = */\t\tFALSE,\n");
    fprintf(file, "\t\t/* msgt_deallocate = */\t\t%s,\n",
	    strdealloc(dealloc));
    fprintf(file, "\t\t/* msgt_unused = */\t\t0\n");
    fprintf(file, "\t};\n");
}

void
WriteStaticDecl(FILE *file, const ipc_type_t *it, dealloc_t dealloc,
		boolean_t longform, boolean_t inname, identifier_t name)
{
    if (longform)
	WriteStaticLongDecl(file, it, dealloc, inname, name);
    else
	WriteStaticShortDecl(file, it, dealloc, inname, name);
}

/*
 * Like vfprintf, but omits a leading comment in the format string
 * and skips the items that would be printed by it.  Only %s, %d,
 * and %f are recognized.
 */
static void
SkipVFPrintf(FILE *file, register const char *fmt, va_list pvar)
{
    if (*fmt == 0)
	return;	/* degenerate case */

    if (fmt[0] == '/' && fmt[1] == '*') {
	/* Format string begins with C comment.  Scan format
	   string until end-comment delimiter, skipping the
	   items in pvar that the enclosed format items would
	   print. */

	register int c;

	fmt += 2;
	for (;;) {
	    c = *fmt++;
	    if (c == 0)
		return;	/* nothing to format */
	    if (c == '*') {
		if (*fmt == '/') {
		    break;
		}
	    }
	    else if (c == '%') {
		/* Field to skip */
		c = *fmt++;
		switch (c) {
		    case 's':
			(void) va_arg(pvar, char *);
			break;
		    case 'd':
			(void) va_arg(pvar, int);
			break;
		    case 'f':
			(void) va_arg(pvar, double);
			break;
		    case '\0':
			return; /* error - fmt ends with '%' */
		    default:
			break;
		}
	    }
	}
	/* End of comment.  To be pretty, skip
	   the space that follows. */
	fmt++;
	if (*fmt == ' ')
	    fmt++;
    }

    /* Now format the string. */
    (void) vfprintf(file, fmt, pvar);
}

void
WriteCopyType(FILE *file, const ipc_type_t *it, const char *left,
	      const char *right, ...)
{
    va_list pvar;
    va_start(pvar, right);

    if (it->itStruct)
    {
	fprintf(file, "\t");
	SkipVFPrintf(file, left, pvar);
	fprintf(file, " = ");
	SkipVFPrintf(file, right, pvar);
	fprintf(file, ";\n");
    }
    else if (it->itString)
    {
	fprintf(file, "\t(void) mig_strncpy(");
	SkipVFPrintf(file, left, pvar);
	fprintf(file, ", ");
	SkipVFPrintf(file, right, pvar);
	fprintf(file, ", %d);\n", it->itTypeSize);
    }
    else
    {
	fprintf(file, "\t{ typedef struct { char data[%d]; } *sp; * (sp) ",
		it->itTypeSize);
	SkipVFPrintf(file, left, pvar);
	fprintf(file, " = * (sp) ");
	SkipVFPrintf(file, right, pvar);
	fprintf(file, "; }\n");
    }
    va_end(pvar);
}

void
WritePackMsgType(FILE *file, const ipc_type_t *it, dealloc_t dealloc,
		 boolean_t longform, boolean_t inname, const char *left,
		 const char *right, ...)
{
    va_list pvar;
    va_start(pvar, right);

    fprintf(file, "\t");
    SkipVFPrintf(file, left, pvar);
    fprintf(file, " = ");
    SkipVFPrintf(file, right, pvar);
    fprintf(file, ";\n");

    va_end(pvar);
}
