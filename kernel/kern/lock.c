/* 
 * Mach Operating System
 * Copyright (c) 1993-1987 Carnegie Mellon University
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
 * $Log:	lock.c,v $
 * Revision 2.11  93/01/14  17:34:55  danner
 * 	Added ANSI function prototypes.
 * 	[92/12/30            dbg]
 * 	64bit cleanup.
 * 	[92/12/01            af]
 * 
 * Revision 2.10  92/08/03  17:38:01  jfriedl
 * 	removed silly prototypes
 * 	[92/08/02            jfriedl]
 * 
 * Revision 2.9  92/05/21  17:14:27  jfriedl
 * 	Added void to functions that yet needed it.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.8  92/01/23  15:20:56  rpd
 * 	Fixed lock_done and lock_read_to_write to only wakeup
 * 	if the number of readers is zero (from Sanjay Nadkarni).
 * 	Fixed db_show_all_slocks.
 * 	[92/01/20            rpd]
 * 
 * Revision 2.7  91/07/01  08:25:03  jsb
 * 	Implemented db_show_all_slocks.
 * 	[91/06/29  16:52:53  jsb]
 * 
 * Revision 2.6  91/05/18  14:32:07  rpd
 * 	Added simple_locks_info, to keep track of which locks are held.
 * 	[91/05/02            rpd]
 * 	Added check_simple_locks.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.5  91/05/14  16:43:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:27:31  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:31  mrt]
 * 
 * Revision 2.3  90/06/02  14:54:56  rpd
 * 	Allow the lock to be taken in simple_lock_try.
 * 	[90/03/26  22:09:13  rpd]
 * 
 * Revision 2.2  90/01/11  11:43:18  dbg
 * 	Upgrade to mainline: use simple_lock_addr when calling
 * 	thread_sleep.
 * 	[89/12/11            dbg]
 * 
 * Revision 2.1  89/08/03  15:45:34  rwd
 * Created.
 * 
 * Revision 2.3  88/09/25  22:14:45  rpd
 * 	Changed explicit panics in sanity-checking simple locking calls
 * 	to assertions.
 * 	[88/09/12  23:03:22  rpd]
 * 
 * Revision 2.2  88/07/20  16:35:47  rpd
 * Add simple-locking sanity-checking code.
 * 
 * 23-Jan-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	On a UNIPROCESSOR, set lock_wait_time = 0.  There is no reason
 *	for a uni to spin on a complex lock.
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Eliminated previous history.
 */
/*
 *	File:	kern/lock.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Locking primitives implementation
 */

#include <cpus.h>
#include <mach_kdb.h>

#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#if	MACH_KDB
#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#endif


#if	NCPUS > 1

/*
 *	Module:		lock
 *	Function:
 *		Provide reader/writer sychronization.
 *	Implementation:
 *		Simple interlock on a bit.  Readers first interlock,
 *		increment the reader count, then let go.  Writers hold
 *		the interlock (thus preventing further readers), and
 *		wait for already-accepted readers to go away.
 */

/*
 *	The simple-lock routines are the primitives out of which
 *	the lock package is built.  The implementation is left
 *	to the machine-dependent code.
 */

#ifdef	notdef
/*
 *	A sample implementation of simple locks.
 *	assumes:
 *		boolean_t test_and_set(boolean_t *)
 *			indivisibly sets the boolean to TRUE
 *			and returns its old value
 *		and that setting a boolean to FALSE is indivisible.
 */
/*
 *	simple_lock_init initializes a simple lock.  A simple lock
 *	may only be used for exclusive locks.
 */

void simple_lock_init(simple_lock_t l)
{
	*(boolean_t *)l = FALSE;
}

void simple_lock(simple_lock_t l)
{
	while (test_and_set((boolean_t *)l))
		continue;
}

void simple_unlock(simple_lock_t l)
{
	*(boolean_t *)l = FALSE;
}

boolean_t simple_lock_try(simple_lock_t l)
{
    	return (!test_and_set((boolean_t *)l));
}
#endif	/* notdef */
#endif	/* NCPUS > 1 */

#if	NCPUS > 1
int lock_wait_time = 100;
#else	/* NCPUS > 1 */

	/*
	 * 	It is silly to spin on a uni-processor as if we
	 *	thought something magical would happen to the
	 *	want_write bit while we are executing.
	 */
int lock_wait_time = 0;
#endif	/* NCPUS > 1 */

#if	MACH_SLOCKS && NCPUS == 1
/*
 *	This code does not protect simple_locks_taken and simple_locks_info.
 *	It works despite the fact that interrupt code does use simple locks.
 *	This is because interrupts use locks in a stack-like manner.
 *	Each interrupt releases all the locks it acquires, so the data
 *	structures end up in the same state after the interrupt as before.
 *	The only precaution necessary is that simple_locks_taken be
 *	incremented first and decremented last, so that interrupt handlers
 *	don't over-write active slots in simple_locks_info.
 */

unsigned int simple_locks_taken = 0;

#define	NSLINFO	1000		/* maximum number of locks held */

struct simple_locks_info {
	simple_lock_t l;
	unsigned int ra;
} simple_locks_info[NSLINFO];

void check_simple_locks(void)
{
	assert(simple_locks_taken == 0);
}

/* Need simple lock sanity checking code if simple locks are being
   compiled in, and we are compiling for a uniprocessor. */

void simple_lock_init(
	simple_lock_t l)
{
	l->lock_data = 0;
}

void simple_lock(
	simple_lock_t l)
{
	struct simple_locks_info *info;

	assert(l->lock_data == 0);

	l->lock_data = 1;

	info = &simple_locks_info[simple_locks_taken++];
	info->l = l;
	/* XXX we want our return address, if possible */
#ifdef	i386
	info->ra = *((unsigned int *)&l - 1);
#endif	/* i386 */
}

boolean_t simple_lock_try(
	simple_lock_t l)
{
	struct simple_locks_info *info;

	if (l->lock_data != 0)
		return FALSE;

	l->lock_data = 1;

	info = &simple_locks_info[simple_locks_taken++];
	info->l = l;
	/* XXX we want our return address, if possible */
#ifdef	i386
	info->ra = *((unsigned int *)&l - 1);
#endif	/* i386 */

	return TRUE;
}

void simple_unlock(
	simple_lock_t l)
{
	assert(l->lock_data != 0);

	l->lock_data = 0;

	if (simple_locks_info[simple_locks_taken-1].l != l) {
		unsigned int i = simple_locks_taken;

		/* out-of-order unlocking */

		do
			if (i == 0)
				panic("simple_unlock");
		while (simple_locks_info[--i].l != l);

		simple_locks_info[i] = simple_locks_info[simple_locks_taken-1];
	}
	simple_locks_taken--;
}

#endif	/* MACH_SLOCKS && NCPUS == 1 */

/*
 *	Routine:	lock_init
 *	Function:
 *		Initialize a lock; required before use.
 *		Note that clients declare the "struct lock"
 *		variables and then initialize them, rather
 *		than getting a new one from this module.
 */
void lock_init(
	lock_t		l,
	boolean_t	can_sleep)
{
	bzero((char *)l, sizeof(lock_data_t));
	simple_lock_init(&l->interlock);
	l->want_write = FALSE;
	l->want_upgrade = FALSE;
	l->read_count = 0;
	l->can_sleep = can_sleep;
	l->thread = (struct thread *)-1;	/* XXX */
	l->recursion_depth = 0;
}

void lock_sleepable(
	lock_t		l,
	boolean_t	can_sleep)
{
	simple_lock(&l->interlock);
	l->can_sleep = can_sleep;
	simple_unlock(&l->interlock);
}


/*
 *	Sleep locks.  These use the same data structure and algorithm
 *	as the spin locks, but the process sleeps while it is waiting
 *	for the lock.  These work on uniprocessor systems.
 */

void lock_write(
	register lock_t	l)
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return;
	}

	/*
	 *	Try to acquire the want_write bit.
	 */
	while (l->want_write) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->want_write)
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && l->want_write) {
			l->waiting = TRUE;
			thread_sleep(l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}
	l->want_write = TRUE;

	/* Wait for readers (and upgrades) to finish */

	while ((l->read_count != 0) || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && (l->read_count != 0 ||
					l->want_upgrade))
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && (l->read_count != 0 || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep(l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}
	simple_unlock(&l->interlock);
}

void lock_done(
	register lock_t	l)
{
	simple_lock(&l->interlock);

	if (l->read_count != 0)
		l->read_count--;
	else
	if (l->recursion_depth != 0)
		l->recursion_depth--;
	else
	if (l->want_upgrade)
	 	l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	/*
	 *	There is no reason to wakeup a waiting thread
	 *	if the read-count is non-zero.  Consider:
	 *		we must be dropping a read lock
	 *		threads are waiting only if one wants a write lock
	 *		if there are still readers, they can't proceed
	 */

	if (l->waiting && (l->read_count == 0)) {
		l->waiting = FALSE;
		thread_wakeup(l);
	}

	simple_unlock(&l->interlock);
}

void lock_read(
	register lock_t	l)
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->read_count++;
		simple_unlock(&l->interlock);
		return;
	}

	while (l->want_write || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && (l->want_write || l->want_upgrade))
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && (l->want_write || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep(l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}

	l->read_count++;
	simple_unlock(&l->interlock);
}

/*
 *	Routine:	lock_read_to_write
 *	Function:
 *		Improves a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		no lock is held upon return.
 *
 *		Returns TRUE if the upgrade *failed*.
 */
boolean_t lock_read_to_write(
	register lock_t	l)
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	l->read_count--;

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return(FALSE);
	}

	if (l->want_upgrade) {
		/*
		 *	Someone else has requested upgrade.
		 *	Since we've released a read lock, wake
		 *	him up.
		 */
		if (l->waiting && (l->read_count == 0)) {
			l->waiting = FALSE;
			thread_wakeup(l);
		}

		simple_unlock(&l->interlock);
		return TRUE;
	}

	l->want_upgrade = TRUE;

	while (l->read_count != 0) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->read_count != 0)
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && l->read_count != 0) {
			l->waiting = TRUE;
			thread_sleep(l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}

	simple_unlock(&l->interlock);
	return FALSE;
}

void lock_write_to_read(
	register lock_t	l)
{
	simple_lock(&l->interlock);

	l->read_count++;
	if (l->recursion_depth != 0)
		l->recursion_depth--;
	else
	if (l->want_upgrade)
		l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	if (l->waiting) {
		l->waiting = FALSE;
		thread_wakeup(l);
	}

	simple_unlock(&l->interlock);
}


/*
 *	Routine:	lock_try_write
 *	Function:
 *		Tries to get a write lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t lock_try_write(
	register lock_t	l)
{
	simple_lock(&l->interlock);

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return TRUE;
	}

	if (l->want_write || l->want_upgrade || l->read_count) {
		/*
		 *	Can't get lock.
		 */
		simple_unlock(&l->interlock);
		return FALSE;
	}

	/*
	 *	Have lock.
	 */

	l->want_write = TRUE;
	simple_unlock(&l->interlock);
	return TRUE;
}

/*
 *	Routine:	lock_try_read
 *	Function:
 *		Tries to get a read lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t lock_try_read(
	register lock_t	l)
{
	simple_lock(&l->interlock);

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->read_count++;
		simple_unlock(&l->interlock);
		return TRUE;
	}

	if (l->want_write || l->want_upgrade) {
		simple_unlock(&l->interlock);
		return FALSE;
	}

	l->read_count++;
	simple_unlock(&l->interlock);
	return TRUE;
}

/*
 *	Routine:	lock_try_read_to_write
 *	Function:
 *		Improves a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		the read lock is still held upon return.
 *
 *		Returns FALSE if the upgrade *failed*.
 */
boolean_t lock_try_read_to_write(
	register lock_t	l)
{
	check_simple_locks();
	simple_lock(&l->interlock);

	if (l->thread == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->read_count--;
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return TRUE;
	}

	if (l->want_upgrade) {
		simple_unlock(&l->interlock);
		return FALSE;
	}
	l->want_upgrade = TRUE;
	l->read_count--;

	while (l->read_count != 0) {
		l->waiting = TRUE;
		thread_sleep(l,
			simple_lock_addr(l->interlock), FALSE);
		simple_lock(&l->interlock);
	}

	simple_unlock(&l->interlock);
	return TRUE;
}

/*
 *	Allow a process that has a lock for write to acquire it
 *	recursively (for read, write, or update).
 */
void lock_set_recursive(
	lock_t		l)
{
	simple_lock(&l->interlock);
	if (!l->want_write) {
		panic("lock_set_recursive: don't have write lock");
	}
	l->thread = current_thread();
	simple_unlock(&l->interlock);
}

/*
 *	Prevent a lock from being re-acquired.
 */
void lock_clear_recursive(
	lock_t		l)
{
	simple_lock(&l->interlock);
	if (l->thread != current_thread()) {
		panic("lock_clear_recursive: wrong thread");
	}
	if (l->recursion_depth == 0)
		l->thread = (struct thread *)-1;	/* XXX */
	simple_unlock(&l->interlock);
}

#if	MACH_KDB
#if	MACH_SLOCKS && NCPUS == 1
void db_show_all_slocks(void)
{
	int i;
	struct simple_locks_info *info;
	simple_lock_t l;

	for (i = 0; i < simple_locks_taken; i++) {
		info = &simple_locks_info[i];
		db_printf("%d: ", i);
		db_printsym(info->l, DB_STGY_ANY);
#if i386
		db_printf(" locked by ");
		db_printsym(info->ra, DB_STGY_PROC);
#endif
		db_printf("\n");
	}
}
#else	/* MACH_SLOCKS && NCPUS == 1 */
void db_show_all_slocks(void)
{
	db_printf("simple lock info not available\n");
}
#endif	/* MACH_SLOCKS && NCPUS == 1 */
#endif	/* MACH_KDB */
