/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * any improvements or extensions that they make and grant Carnegie Mellon rights
 * to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	queue.h,v $
 * Revision 2.3  93/02/01  09:57:03  danner
 * 	Fixed comment-with-comments.
 * 	[93/01/27            jfriedl]
 * 
 * Revision 2.2  92/01/03  19:58:09  dbg
 * 	Strip down for default pager use.
 * 	[91/10/02            dbg]
 * 
 * Revision 2.4  91/05/14  16:45:55  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:28:43  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:16:28  mrt]
 * 
 * Revision 2.2  89/09/08  11:26:11  dbg
 * 	Streamlined generic queue macros.
 * 	[89/08/17            dbg]
 * 
 * 19-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Added queue_remove_last; removed round_queue.  [Changes from
 *	mwyoung: 19 Dec 88.]
 *
 * 29-Nov-88  David Golub (dbg) at Carnegie-Mellon University
 *	Removed include of cputypes.h.  Added queue_iterate.
 *	Split into two groups the macros that operate directly on
 *	queues (or expect the queue chain to be first in a structure)
 *	and those that operate on generic structures (where the chain
 *	may be anywhere).
 *
 * 17-Jan-88  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Added queue_enter_first, queue_last and queue_prev for use by
 *	the TCP netport code.
 *
 * 12-Jun-85  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	File:	queue.h
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1985
 *
 *	Type definitions for generic queues.
 *
 */

#ifndef	_QUEUE_H_
#define	_QUEUE_H_

/*
 *	Queue of abstract objects.  Queue is maintained
 *	within that object.
 *
 *	Supports fast removal from within the queue.
 *
 *	How to declare a queue of elements of type "foo_t":
 *		In the "*foo_t" type, you must have a field of
 *		type "queue_chain_t" to hold together this queue.
 *		There may be more than one chain through a
 *		"foo_t", for use by different queues.
 *
 *		Declare the queue as a "queue_t" type.
 *
 *		Elements of the queue (of type "foo_t", that is)
 *		are referred to by reference, and cast to type
 *		"queue_entry_t" within this module.
 */

/*
 *	A generic doubly-linked list (queue).
 */

struct queue_entry {
	struct queue_entry	*next;		/* next element */
	struct queue_entry	*prev;		/* previous element */
};

typedef struct queue_entry	*queue_t;
typedef	struct queue_entry	queue_head_t;
typedef	struct queue_entry	queue_chain_t;
typedef	struct queue_entry	*queue_entry_t;

/*
 *	Macro:		queue_init
 *	Function:
 *		Initialize the given queue.
 *	Header:
 *		void queue_init(q)
 *			queue_t		q;	/ * MODIFIED * /
 */
#define	queue_init(q)	((q)->next = (q)->prev = q)

/*
 *	Macro:		queue_first
 *	Function:
 *		Returns the first entry in the queue,
 *	Header:
 *		queue_entry_t queue_first(q)
 *			queue_t	q;		/ * IN * /
 */
#define	queue_first(q)	((q)->next)

/*
 *	Macro:		queue_next
 *	Function:
 *		Returns the entry after an item in the queue.
 *	Header:
 *		queue_entry_t queue_next(qc)
 *			queue_t qc;
 */
#define	queue_next(qc)	((qc)->next)

/*
 *	Macro:		queue_last
 *	Function:
 *		Returns the last entry in the queue.
 *	Header:
 *		queue_entry_t queue_last(q)
 *			queue_t	q;		/ * IN * /
 */
#define	queue_last(q)	((q)->prev)

/*
 *	Macro:		queue_prev
 *	Function:
 *		Returns the entry before an item in the queue.
 *	Header:
 *		queue_entry_t queue_prev(qc)
 *			queue_t qc;
 */
#define	queue_prev(qc)	((qc)->prev)

/*
 *	Macro:		queue_end
 *	Function:
 *		Tests whether a new entry is really the end of
 *		the queue.
 *	Header:
 *		boolean_t queue_end(q, qe)
 *			queue_t q;
 *			queue_entry_t qe;
 */
#define	queue_end(q, qe)	((q) == (qe))

/*
 *	Macro:		queue_empty
 *	Function:
 *		Tests whether a queue is empty.
 *	Header:
 *		boolean_t queue_empty(q)
 *			queue_t q;
 */
#define	queue_empty(q)		queue_end((q), queue_first(q))


/*----------------------------------------------------------------*/
/*
 * Macros that operate on generic structures.  The queue
 * chain may be at any location within the structure, and there
 * may be more than one chain.
 */

/*
 *	Macro:		queue_enter
 *	Function:
 *		Insert a new element at the tail of the queue.
 *	Header:
 *		void queue_enter(q, elt, type, field)
 *			queue_t q;
 *			<type> elt;
 *			<type> is what's in our queue
 *			<field> is the chain field in (*<type>)
 */
#define queue_enter(head, elt, type, field)			\
{ 								\
	register queue_entry_t prev;				\
								\
	prev = (head)->prev;					\
	if ((head) == prev) {					\
		(head)->next = (queue_entry_t) (elt);		\
	}							\
	else {							\
		((type)prev)->field.next = (queue_entry_t)(elt);\
	}							\
	(elt)->field.prev = prev;				\
	(elt)->field.next = head;				\
	(head)->prev = (queue_entry_t) elt;			\
}

/*
 *	Macro:		queue_enter_first
 *	Function:
 *		Insert a new element at the head of the queue.
 *	Header:
 *		void queue_enter_first(q, elt, type, field)
 *			queue_t q;
 *			<type> elt;
 *			<type> is what's in our queue
 *			<field> is the chain field in (*<type>)
 */
#define queue_enter_first(head, elt, type, field)		\
{ 								\
	register queue_entry_t next;				\
								\
	next = (head)->next;					\
	if ((head) == next) {					\
		(head)->prev = (queue_entry_t) (elt);		\
	}							\
	else {							\
		((type)next)->field.prev = (queue_entry_t)(elt);\
	}							\
	(elt)->field.next = next;				\
	(elt)->field.prev = head;				\
	(head)->next = (queue_entry_t) elt;			\
}

/*
 *	Macro:		queue_field [internal use only]
 *	Function:
 *		Find the queue_chain_t (or queue_t) for the
 *		given element (thing) in the given queue (head)
 */
#define	queue_field(head, thing, type, field)			\
		(((head) == (thing)) ? (head) : &((type)(thing))->field)

/*
 *	Macro:		queue_remove
 *	Function:
 *		Remove an arbitrary item from the queue.
 *	Header:
 *		void queue_remove(q, qe, type, field)
 *			arguments as in queue_enter
 */
#define	queue_remove(head, elt, type, field)			\
{								\
	register queue_entry_t	next, prev;			\
								\
	next = (elt)->field.next;				\
	prev = (elt)->field.prev;				\
								\
	if ((head) == next)					\
		(head)->prev = prev;				\
	else							\
		((type)next)->field.prev = prev;		\
								\
	if ((head) == prev)					\
		(head)->next = next;				\
	else							\
		((type)prev)->field.next = next;		\
}

/*
 *	Macro:		queue_remove_first
 *	Function:
 *		Remove and return the entry at the head of
 *		the queue.
 *	Header:
 *		queue_remove_first(head, entry, type, field)
 *		entry is returned by reference
 */
#define	queue_remove_first(head, entry, type, field)		\
{								\
	register queue_entry_t	next;				\
								\
	(entry) = (type) ((head)->next);			\
	next = (entry)->field.next;				\
								\
	if ((head) == next)					\
		(head)->prev = (head);				\
	else							\
		((type)(next))->field.prev = (head);		\
	(head)->next = next;					\
}

/*
 *	Macro:		queue_remove_last
 *	Function:
 *		Remove and return the entry at the tail of
 *		the queue.
 *	Header:
 *		queue_remove_last(head, entry, type, field)
 *		entry is returned by reference
 */
#define	queue_remove_last(head, entry, type, field)		\
{								\
	register queue_entry_t	prev;				\
								\
	(entry) = (type) ((head)->prev);			\
	prev = (entry)->field.prev;				\
								\
	if ((head) == prev)					\
		(head)->next = (head);				\
	else							\
		((type)(prev))->field.next = (head);		\
	(head)->prev = prev;					\
}

/*
 *	Macro:		queue_assign
 */
#define	queue_assign(to, from, type, field)			\
{								\
	((type)((from)->prev))->field.next = (to);		\
	((type)((from)->next))->field.prev = (to);		\
	*to = *from;						\
}

/*
 *	Macro:		queue_iterate
 *	Function:
 *		iterate over each item in the queue.
 *		Generates a 'for' loop, setting elt to
 *		each item in turn (by reference).
 *	Header:
 *		queue_iterate(q, elt, type, field)
 *			queue_t q;
 *			<type> elt;
 *			<type> is what's in our queue
 *			<field> is the chain field in (*<type>)
 */
#define queue_iterate(head, elt, type, field)			\
	for ((elt) = (type) queue_first(head);			\
	     !queue_end((head), (queue_entry_t)(elt));		\
	     (elt) = (type) queue_next(&(elt)->field))



#endif	_QUEUE_H_
