/* 
 * Mach Operating System
 * Copyright (c) 1992,1991,1990 Carnegie Mellon University
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
 * $Log:	db_sym.c,v $
 * Revision 2.17  93/03/09  10:53:50  danner
 * 	String protos.
 * 	[93/03/07            af]
 * 
 * Revision 2.16  93/01/14  17:25:45  danner
 * 	Handle multiple, coexisting symbol table types.
 * 	64bit cleanup.
 * 	[92/11/30            af]
 * 
 * 	Check for null db_last_symtab in db_line_at_pc.
 * 	[92/10/06            jvh]
 * 
 * Revision 2.15  92/08/03  17:32:03  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.14  92/05/21  17:07:47  jfriedl
 * 	Cleanup to quiet gcc warnings.
 * 	Changed CHAR arg of db_eqname to UNSIGNED.
 * 	Made arg types proper for db_line_at_pc().
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.13  92/05/05  11:05:16  danner
 * 	typo correction
 * 
 * Revision 2.12  92/05/05  10:52:16  danner
 * 	Bulletproof db_search_in_task_symbol against symbol tables that return
 * 	success and huge offsets for address outside of their domain.
 * 
 * 
 * 
 * Revision 2.11  92/05/04  13:48:25  danner
 * 	Rewrote db_search_task_symbol to deal more reasonably 
 * 	 with overlapping user and kernel address ranges.
 * 	[92/03/23  15:03:21  danner]
 * 	Complete rewrite of db_search_task_symbol logic to deal
 * 	 gracefully with the case of overlapping user and kernel address
 * 	 space.
 * 	[92/03/21            danner]
 * 	Upped MAXNOSYMTABS from 3 to 5. Now there is space for kernel,
 * 	 bootstrap, server, and emulator symbols - plus one for future
 * 	 expansion.
 * 	[92/03/21            danner]
 * 
 * Revision 2.10  91/10/09  16:02:30  af
 * 	 Changed symbol table name qualification syntax from "xxx:yyy"
 * 	  to "xxx::yyy" to allow "file:func:line" in "yyy" part.
 * 	  "db_sym_parse_and_lookup" is also added for "yyy" part parsing.
 * 	  Replaced db_search_symbol with db_search_task_symbol, and moved
 * 	  it to "db_sym.h" as a macro.
 * 	  Added db_task_printsym, and changed db_printsym to call it.
 * 	  Added include "db_task_thread.h".
 *  	  Fixed infinite recursion of db_symbol_values.
 * 	 [91/08/29            tak]
 * 
 * 
 * Revision 2.9  91/07/31  17:31:14  dbg
 * 	Add task pointer and space for string storage to symbol table
 * 	descriptor.
 * 	[91/07/31            dbg]
 * 
 * Revision 2.8  91/07/09  23:16:08  danner
 * 	Changed a printf.
 * 	[91/07/08            danner]
 * 
 * Revision 2.7  91/05/14  15:35:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:42:40  rpd
 * 	Changed the default db_maxoff to 4K.
 * 	[91/03/10            rpd]
 * 
 * Revision 2.5  91/02/05  17:07:07  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:17  mrt]
 * 
 * Revision 2.4  90/10/25  14:44:05  rwd
 * 	Changed db_printsym to print unsigned.
 * 	[90/10/19            rpd]
 * 
 * Revision 2.3  90/09/09  23:19:56  rpd
 * 	Avoid totally incorrect guesses of symbol names for small values.
 * 	[90/08/30  17:39:48  af]
 * 
 * Revision 2.2  90/08/27  21:52:18  dbg
 * 	Removed nlist.h.  Fixed some type declarations.
 * 	Qualifier character is ':'.
 * 	[90/08/20            dbg]
 * 	Modularized symtab info into a new db_symtab_t type.
 * 	Modified db_add_symbol_table  and others accordingly.
 * 	Defined db_sym_t, a new (opaque) type used to represent
 * 	symbols.  This should support all sort of future symtable
 * 	formats. Functions like db_qualify take a db_sym_t now.
 * 	New db_symbol_values() function to explode the content
 * 	of a db_sym_t.
 * 	db_search_symbol() replaces db_find_sym_and_offset(), which is
 * 	now a macro defined in our (new) header file.  This new
 * 	function accepts more restrictive searches, which are
 * 	entirely delegated to the symtab-specific code.
 * 	Accordingly, db_printsym() accepts a strategy parameter.
 * 	New db_line_at_pc() function.
 * 	Renamed misleading db_eqsym into db_eqname.
 * 	[90/08/20  10:47:06  af]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.1  90/07/26  16:43:52  dbg
 * Created.
 * 
 */
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

#include <mach/std_types.h>
#include <kern/strings.h>
#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

#include <vm/vm_map.h>	/* vm_map_t */

/*
 * Multiple symbol tables
 */
#define	MAXNOSYMTABS	5	/* mach, bootstrap, ux, emulator, 1 spare */

db_symtab_t	db_symtabs[MAXNOSYMTABS] = {{0,},};
int db_nsymtab = 0;

db_symtab_t	*db_last_symtab;

db_sym_t	db_lookup();	/* forward */

/*
 * Add symbol table, with given name, to list of symbol tables.
 */
boolean_t
db_add_symbol_table(type, start, end, name, ref, map_pointer)
	int type;
	char *start;
	char *end;
	char *name;
	char *ref;
	char *map_pointer;
{
	register db_symtab_t *st;
	extern vm_map_t kernel_map;

	if (db_nsymtab >= MAXNOSYMTABS)
	    return (FALSE);

	st = &db_symtabs[db_nsymtab];
	st->type = type;
	st->start = start;
	st->end = end;
	st->private = ref;
	st->map_pointer = (map_pointer == (char *)kernel_map)? 0: map_pointer;
	strcpy(st->name, name);

	db_nsymtab++;

	return (TRUE);
}

/*
 *  db_qualify("vm_map", "ux") returns "ux::vm_map".
 *
 *  Note: return value points to static data whose content is
 *  overwritten by each call... but in practice this seems okay.
 */
static char *
db_qualify(symname, symtabname)
	char		*symname;
	register char	*symtabname;
{
	static char     tmp[256];
	register char	*s;

	s = tmp;
	while (*s++ = *symtabname++) {
	}
	s[-1] = ':';
	*s++ = ':';
	while (*s++ = *symname++) {
	}
	return tmp;
}


boolean_t
db_eqname( char* src, char* dst, char c )
{
	if (!strcmp(src, dst))
	    return (TRUE);
	if (src[0] == c)
	    return (!strcmp(src+1,dst));
	return (FALSE);
}

boolean_t
db_value_of_name(name, valuep)
	char		*name;
	db_expr_t	*valuep;
{
	db_sym_t	sym;

	sym = db_lookup(name);
	if (sym == DB_SYM_NULL)
	    return (FALSE);
	db_symbol_values(0, sym, &name, valuep);
	return (TRUE);
}

/*
 * Lookup a symbol.
 * If the symbol has a qualifier (e.g., ux::vm_map),
 * then only the specified symbol table will be searched;
 * otherwise, all symbol tables will be searched.
 */
db_sym_t
db_lookup(symstr)
	char *symstr;
{
	db_sym_t sp;
	register int i;
	int symtab_start = 0;
	int symtab_end = db_nsymtab;
	register char *cp;

	/*
	 * Look for, remove, and remember any symbol table specifier.
	 */
	for (cp = symstr; *cp; cp++) {
		if (*cp == ':' && cp[1] == ':') {
			*cp = '\0';
			for (i = 0; i < db_nsymtab; i++) {
				if (! strcmp(symstr, db_symtabs[i].name)) {
					symtab_start = i;
					symtab_end = i + 1;
					break;
				}
			}
			*cp = ':';
			if (i == db_nsymtab)
				db_error("Invalid symbol table name\n");
			symstr = cp+2;
		}
	}

	/*
	 * Look in the specified set of symbol tables.
	 * Return on first match.
	 */
	for (i = symtab_start; i < symtab_end; i++) {
		if (sp = X_db_lookup(&db_symtabs[i], symstr)) {
			db_last_symtab = &db_symtabs[i];
			return sp;
		}
	}
	return 0;
}

/*
 * Common utility routine to parse a symbol string into a file
 * name, a symbol name and line number.
 * This routine is called from X_db_lookup if the object dependent
 * handler supports qualified search with a file name or a line number.
 * It parses the symbol string, and call an object dependent routine
 * with parsed file name, symbol name and line number.
 */ 
db_sym_t
db_sym_parse_and_lookup(func, symtab, symstr)
	db_sym_t	(*func)();
	db_symtab_t	*symtab;
	char		*symstr;
{
	register 	char *p;
	register 	n;
	int	 	n_name;
	int	 	line_number;
	char	 	*file_name = 0;
	char	 	*sym_name = 0;
	char		*component[3];
	db_sym_t 	found = DB_SYM_NULL;

	/*
	 * disassemble the symbol into components:
	 *	[file_name:]symbol[:line_nubmer]
	 */
	component[0] = symstr;
	component[1] = component[2] = 0;
	for (p = symstr, n = 1; *p; p++) {
		if (*p == ':') {
			if (n >= 3)
				break;
			*p = 0;
			component[n++] = p+1;
		}
	}
	if (*p != 0)
		goto out;
	line_number = 0;
	n_name = n;
	p = component[n-1];
	if (*p >= '0' && *p <= '9') {
		if (n == 1)
			goto out;
		for (line_number = 0; *p; p++) {
			if (*p < '0' || *p > '9')
				goto out;
			line_number = line_number*10 + *p - '0';
		}
		n_name--;
	} else if (n >= 3)
		goto out;
	if (n_name == 1) {
		for (p = component[0]; *p && *p != '.'; p++);
		if (*p == '.') {
			file_name = component[0];
			sym_name = 0;
		} else {
			file_name = 0;
			sym_name = component[0];
		}
	} else {
		file_name = component[0];
		sym_name = component[1];
	}
	found = func(symtab, file_name, sym_name, line_number);
	
out:
	while (--n >= 1)
		component[n][-1] = ':';
	return(found);
}

/*
 * Does this symbol name appear in more than one symbol table?
 * Used by db_symbol_values to decide whether to qualify a symbol.
 */
boolean_t db_qualify_ambiguous_names = FALSE;

boolean_t
db_name_is_ambiguous(sym_name)
	char		*sym_name;
{
	register int	i;
	register
	boolean_t	found_once = FALSE;

	if (!db_qualify_ambiguous_names)
		return FALSE;

	for (i = 0; i < db_nsymtab; i++) {
		if (X_db_lookup(&db_symtabs[i], sym_name)) {
			if (found_once)
				return TRUE;
			found_once = TRUE;
		}
	}
	return FALSE;
}


db_sym_t db_search_in_task_symbol();

/*
 * Find the closest symbol to val, and return its name
 * and the difference between val and the symbol found.
 *
 * Logic change. If the task argument is non NULL and a
 * matching symbol is found in a symbol table which explictly
 * specifies its map to be task->map, that symbol will have 
 * precedence over any symbol from a symbol table will a null 
 * map. This allows overlapping kernel/user maps to work correctly.
 *
 */
db_sym_t
db_search_task_symbol(val, strategy, offp, task)
	register db_addr_t	val;
	db_strategy_t		strategy;
	db_addr_t		*offp; /* better be unsigned */
	task_t			task;
{
  db_sym_t ret;

  if (task != TASK_NULL)
    ret = db_search_in_task_symbol(val, strategy, offp, task);
  else
    {
      ret = db_search_in_task_symbol(val, strategy, offp, task);
      /* 
	db_search_in_task_symbol will return success with 
	a very large offset when it should have failed. 
	*/
      if (ret == DB_SYM_NULL || (*offp) > 0x1000000)
	{
	  task = db_current_task();
	  ret = db_search_in_task_symbol(val, strategy, offp, task);
	}
    }

  return ret;
}

db_sym_t
db_search_in_task_symbol(val, strategy, offp, task)
	register db_addr_t	val;
	db_strategy_t		strategy;
	db_addr_t		*offp;
	task_t			task;
{
  register vm_size_t diff;
  vm_size_t	newdiff;
  register int	i;
  db_symtab_t	*sp;
  db_sym_t	ret = DB_SYM_NULL, sym;
  vm_map_t	map_for_val;

  map_for_val = (task == TASK_NULL)? VM_MAP_NULL: task->map;
  newdiff = diff = ~0;
  db_last_symtab = (db_symtab_t *) 0;
  for (sp = &db_symtabs[0], i = 0; i < db_nsymtab;  sp++, i++) 
    {
      newdiff = ~0;
      if ((vm_map_t)sp->map_pointer == VM_MAP_NULL ||
	  (vm_map_t)sp->map_pointer == map_for_val) 
	{
	  sym = X_db_search_symbol(sp, val, strategy, (db_expr_t*)&newdiff);
	  if (sym == DB_SYM_NULL)
	    continue;
	  if (db_last_symtab == (db_symtab_t *) 0)
	    { /* first hit */
	      db_last_symtab = sp;
	      diff = newdiff;
	      ret = sym;
	      continue;
	    }
	  if ((vm_map_t) sp->map_pointer == VM_MAP_NULL && 
	      (vm_map_t) db_last_symtab->map_pointer == VM_MAP_NULL &&
	      newdiff < diff )
	    { /* closer null map match */
	      db_last_symtab = sp;
	      diff = newdiff;
	      ret = sym;
	      continue;
	    }		    
	  if ((vm_map_t) sp->map_pointer != VM_MAP_NULL && 
	      (newdiff < 0x100000) &&
	      ((vm_map_t) db_last_symtab->map_pointer == VM_MAP_NULL ||
	       newdiff < diff ))
	    { /* update if new is in matching map and symbol is "close", 
		 and 
		 old is VM_MAP_NULL or old in is matching map but is further away 
		 */
	      db_last_symtab = sp;
	      diff = newdiff;
	      ret = sym;
	      continue;
	    }		    
	}
    }
  
  *offp = diff;
  return ret;
}

/*
 * Return name and value of a symbol
 */
void
db_symbol_values(stab, sym, namep, valuep)
	db_symtab_t	*stab;
	db_sym_t	sym;
	char		**namep;
	db_expr_t	*valuep;
{
	db_expr_t	value;
	char		*name;

	if (sym == DB_SYM_NULL) {
		*namep = 0;
		return;
	}
	if (stab == 0)
		stab = db_last_symtab;

	X_db_symbol_values(stab, sym, &name, &value);

	if (db_name_is_ambiguous(name))
		*namep = db_qualify(name, db_last_symtab->name);
	else
		*namep = name;
	if (valuep)
		*valuep = value;
}


/*
 * Print the closest symbol to value
 *
 * After matching the symbol according to the given strategy
 * we print it in the name+offset format, provided the symbol's
 * value is close enough (eg smaller than db_maxoff).
 * We also attempt to print [filename:linenum] when applicable
 * (eg for procedure names).
 *
 * If we could not find a reasonable name+offset representation,
 * then we just print the value in hex.  Small values might get
 * bogus symbol associations, e.g. 3 might get some absolute
 * value like _INCLUDE_VERSION or something, therefore we do
 * not accept symbols whose value is zero (and use plain hex).
 */

unsigned int	db_maxoff = 0x4000;

void
db_task_printsym(off, strategy, task)
	db_expr_t	off;
	db_strategy_t	strategy;
	task_t		task;
{
	db_addr_t	d;
	char 		*filename;
	char		*name;
	db_expr_t	value;
	int 		linenum;
	db_sym_t	cursym;

	cursym = db_search_task_symbol(off, strategy, &d, task);
	db_symbol_values(0, cursym, &name, &value);
	if (name == 0 || d >= db_maxoff || value == 0) {
		db_printf("%#n", off);
		return;
	}
	db_printf("%s", name);
	if (d)
		db_printf("+0x%x", d);
	if (strategy == DB_STGY_PROC) {
		if (db_line_at_pc(cursym, &filename, &linenum, off)) {
			db_printf(" [%s", filename);
			if (linenum > 0)
				db_printf(":%d", linenum);
			db_printf("]");
		}
	}
}

void
db_printsym(off, strategy)
	db_expr_t	off;
	db_strategy_t	strategy;
{
	db_task_printsym(off, strategy, TASK_NULL);
}

boolean_t
db_line_at_pc( sym, filename, linenum, pc)
	db_sym_t	sym;
	char		**filename;
	int		*linenum;
	db_expr_t	pc;
{
	return (db_last_symtab) ?
		X_db_line_at_pc( db_last_symtab, sym, filename, linenum, pc) :
		FALSE;
}

/*
 * Switch into symbol-table specific routines
 */

extern boolean_t aout_db_sym_init(), aout_db_line_at_pc();
extern db_sym_t aout_db_lookup(), aout_db_search_symbol();
extern void aout_db_symbol_values();

extern boolean_t coff_db_sym_init(), coff_db_line_at_pc();
extern db_sym_t coff_db_lookup(), coff_db_search_symbol();
extern void coff_db_symbol_values();

struct db_sym_switch x_db[] = {

	/* BSD a.out format (really, sdb/dbx(1) symtabs) */
#ifdef	DB_NO_AOUT
	{ 0,},
#else	/* DB_NO_AOUT */
	{ aout_db_sym_init, aout_db_lookup, aout_db_search_symbol,
	  aout_db_line_at_pc, aout_db_symbol_values },
#endif	/* DB_NO_AOUT */

#ifdef	DB_NO_COFF
	{ 0,},
#else	/* DB_NO_COFF */
	{ coff_db_sym_init, coff_db_lookup, coff_db_search_symbol,
	  coff_db_line_at_pc, coff_db_symbol_values },
#endif	/* DB_NO_COFF */

	/* Machdep, not inited here */
	{ 0,}

};
