/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1992 OMRON Corporation
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
 * $Log:	msmain.c,v $
 * Revision 2.9  93/03/09  17:59:52  danner
 * 	ANSI and uart cleanup
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.8  93/02/05  08:07:43  danner
 * 	Completely rewrote for 3.0
 * 	[93/02/04            jfriedl]
 * 
 */

/*
 * Mouse driver
 */
#include <luna88k/machdep.h>
#include <sys/types.h>
#include <device/io_req.h>
#include <luna88kdev/uart.h>
#include <luna88kdev/kbms.h>
#include <luna88k/mouse.h>

/*
 * The following has a nice long descriptive name so that it'll be
 * easily understood in the DDB show-threads listing.
 */
static volatile int waiting_for_mouse_data = 0;

static simple_lock_data_t mouse_lock;
/*static*/ volatile int mouse_is_open = 0;
static volatile int async_flag = 0;
queue_head_t mouse_read_queue = {&mouse_read_queue, &mouse_read_queue};

#ifdef MOUSE_DEBUG
int mouse_debug = 0;
#endif

static int mouse_scan = 0;
static int mouse_scan_interval = 3;

#ifdef MOUSE_DEBUG
int mouse_spl_count = 0;
# define mouse_spltty()  (++mouse_spl_count, spltty())
# define mouse_splx(foo) (--mouse_spl_count, splx(foo))
#else
# define mouse_spltty()  spltty()
# define mouse_splx(foo) splx(foo)
#endif

/*****************************************************************************/
/*****************************************************************************/

/*
 * Standard circular buffer. MOUSE_BUFFER_SIZE elements of mouse_data.
 * Elements go in at 'next_free' and are taken out at 'last_read'.
 *
 * If (next_free == last_read), there are no elements.
 *
 * Access is controlled by 'lock'.
 *
 * This whole thing is in a structure simply to keep it logically together.
 *
 */
static volatile struct
{
    simple_lock_data_t lock;		/* Must have lock to access below. */
    #define mouse_buffer_lock()	        simple_lock(&mouse_data_buffer.lock)
    #define mouse_buffer_unlock()	simple_unlock(&mouse_data_buffer.lock)

    struct mouse_data
	 *next_free,			/* Elements inserted here.. */
	 *last_read,			/* ..and taken from here */
         data[MOUSE_BUFFER_SIZE];	/* The Elements */
} mouse_data_buffer;

/*
 * Returns the number of elements currently available in the mouse_data_buffer.
 * ASSUMES THAT THE LOCK IS HELD.
 */	
#define mouse_buffer_count()						\
    (mouse_data_buffer.next_free >= mouse_data_buffer.last_read ?	\
     mouse_data_buffer.next_free -  mouse_data_buffer.last_read :	\
     MOUSE_BUFFER_SIZE -						\
	(mouse_data_buffer.last_read - mouse_data_buffer.next_free))

/*
 * Returns true if the buffer is empty, false if there is data available.
 * ASSUMES THAT THE LOCK IS HELD.
 */
#define mouse_buffer_empty() \
    (mouse_data_buffer.next_free == mouse_data_buffer.last_read)
	
/*
 * Used to increment next_free and last_read (of mouse_data_buffer).
 * ASSUMES THAT THE LOCK IS HELD.
 */
#define bump_up(ptr)						\
{								\
    if (++(ptr) == &mouse_data_buffer.data[MOUSE_BUFFER_SIZE])	\
	    (ptr) = mouse_data_buffer.data;			\
}


static volatile int scan_delta_x = 0;
static volatile int scan_delta_y = 0;
static volatile unsigned current_button_state = 0;

/*****************************************************************************/
/*****************************************************************************/

/*
 * Reset the mouse data buffer to be cleared.
 */
static void flush_mouse_data(void)
{
    unsigned s = mouse_spltty();
    mouse_buffer_lock();

    mouse_data_buffer.next_free =
    mouse_data_buffer.last_read = mouse_data_buffer.data;

    mouse_buffer_unlock();
    mouse_splx(s);
}

static void insert_into_data_buffer(unsigned A, int B, int C, char *where)
{
    extern time_value_t *mtime;
    unsigned s;
    #define lbolt ((mtime) ? mtime->seconds : 0)


    current_button_state = A;

    s = mouse_spltty();
    mouse_buffer_lock();

    mouse_data_buffer.next_free->button_info = (unsigned char)A;
    mouse_data_buffer.next_free->x_delta     = (short)B;
    mouse_data_buffer.next_free->y_delta     = (short)C;
    mouse_data_buffer.next_free->time        = lbolt;

    #ifdef MOUSE_DEBUG
    if (mouse_debug)
	raw_printf("[K data_buffer %s @%d b:%x %d x %d]\n", where,
	    mouse_data_buffer.next_free->time,
	    mouse_data_buffer.next_free->button_info,
	    mouse_data_buffer.next_free->x_delta,
	    mouse_data_buffer.next_free->y_delta);
    #endif

    bump_up(mouse_data_buffer.next_free);

    if (mouse_buffer_empty())
    {
	/*
	 * If we've just added an element, but it looks like the buffer
	 * is empty, we must have just come full circle and filled up the
	 * buffer.
	 *
	 * We'll throw away the oldest element by pretending
	 * it was just read.
should not throw away button presses.
Maybe should merge x and y deltas....
	 */
	bump_up(mouse_data_buffer.last_read);
    }

    mouse_buffer_unlock();
    mouse_splx(s);

    if (waiting_for_mouse_data)
    {
	waiting_for_mouse_data = 0;
	wakeup((vm_offset_t)&waiting_for_mouse_data);
    }

    if (async_flag)
    {
	/* wake up those waiting for a read */
	io_req_t ior;
	while (ior = (io_req_t)dequeue_head(&mouse_read_queue), ior) {
	    #ifdef MOUSE_DEBUG
	    if (mouse_debug)
		raw_printf("[K mouse_read done(%x)]\n", ior);
	    #endif
	    iodone(ior);
	}
    }

}

simple_lock_data_t scan_delta_lock; /* controls access to scan_delta_{x,y} */
#define scan_delta_lock()     simple_lock(&scan_delta_lock)
#define scan_delta_unlock()   simple_unlock(&scan_delta_lock)

void mouse_scanner(void)
{
    if (mouse_scan == 0)
	return;

    if (scan_delta_x || scan_delta_y) {
 	int x, y;
        unsigned s = mouse_spltty();
	  scan_delta_lock();
	    x = scan_delta_x;
	    y = scan_delta_y;
	    scan_delta_x = 0;
	    scan_delta_y = 0;
          scan_delta_unlock();
	mouse_splx(s);

	if (x || y)
	    insert_into_data_buffer(current_button_state, x, y, "scanner");
    }

    timeout((void*)mouse_scanner, 0, mouse_scan_interval);
}

/*
 * Receive a byte of data from the mouse.
 * Data comes in groups of three bytes, the first of which is
 * in the range of [ 0x80 .. 0x87 ].
 *
 * Return true if we've eaten this character, false if it wasn't for us.
 */
int mouse_receive(unsigned data)
{
    static unsigned char button_data_byte;
    static signed char x_data_byte;
           signed char y_data_byte;
    static int data_byte_index_number = 0;

    #ifdef MOUSE_DEBUG
	static volatile int in = 0;
	if (in) raw_printf("[K IN IN IN IN IN IN IN\n");
	in = 1;
    #endif

    switch (data_byte_index_number)
    {
      case 0:
	/* if byte is 10000xxx, it's the first of three mouse bytes */
	if ((data & 0xF8) != 0x80) {
#ifdef MOUSE_DEBUG
	    in = 0;
#endif
	    return 0; /* not meant for us */
	}
	button_data_byte = (data & 0x07); /* isolate lower three bits*/
	data_byte_index_number = 1;
	break;

      case 1: /* have read one byte (button data), now get x value */
	x_data_byte = (signed char)data;
	data_byte_index_number = 2;
	break;

      case 2: /* this is the last of the three bytes */
	y_data_byte = (signed char)data;
	data_byte_index_number = 0;

	#ifdef MOUSE_DEBUG
	if (mouse_debug)
	    raw_printf("[K raw bytes %x %d %d]\n",
		button_data_byte,
		x_data_byte,
		y_data_byte);
	#endif

	if (!mouse_is_open)
	    break;

	/*
	 * Now have complete set of three bytes from mouse.... do something
	 * with it....
	 */

	if (!mouse_scan)
	    insert_into_data_buffer(button_data_byte, x_data_byte, y_data_byte, "regular");
	else {
	    unsigned s = mouse_spltty(); /* don't know if this is required or not */
	    scan_delta_lock();
	      scan_delta_x += x_data_byte;
	      scan_delta_y += y_data_byte;
	    scan_delta_unlock();
	    mouse_splx(s);

	    if (button_data_byte != current_button_state) {
		/* we have button action */
		insert_into_data_buffer(button_data_byte, 0, 0, "button");
	    }
	}
	break;
    }

#ifdef MOUSE_DEBUG
    in = 0;
#endif
    return 1;
}

/*
 * Returns the number of packets read.
 */
static int return_raw_data(unsigned nodelay,
			   unsigned latest,
			   unsigned count,
			   struct mouse_data *addr)
{
    int retval, element_count;
    unsigned s;

    #ifdef MOUSE_DEBUG
    if (mouse_debug) {
	raw_printf("[K read_raw_data(want %d, have %d)]\n",
		count, mouse_buffer_count());
    }
    #endif

    s = mouse_spltty();
    mouse_buffer_lock();

    while (mouse_buffer_empty())
    {
        if (nodelay) {
	    mouse_buffer_unlock();
            mouse_splx(s);
            return(0);
	}

	waiting_for_mouse_data = 1;

	/*
	 * XXX we have a race condition here...
	 * the wakeup could happen before the sleep.
	 */
	while (waiting_for_mouse_data)  {
	    #ifdef MOUSE_DEBUG
	    if (mouse_debug) raw_printf("[K sleeping waiting for data]\n");
	    #endif

	    /* Snooozzze */
	    assert_wait(&waiting_for_mouse_data, TRUE);
	    mouse_buffer_unlock();
            mouse_splx(s);
	    thread_block(0);

	    s = mouse_spltty();
            mouse_buffer_lock();
	}
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K awake from sleep]\n");
	#endif
	
    }

    /* find out what's there */
    element_count = mouse_buffer_count();

    /* and make sure we try to read no more than that */
    if (element_count < count)
	count = element_count;

    /*
     * Apparently, flag == 2 means to take the 'count' LATEST elements
     * and flush when we're done, rather than taking the next count
     * elements in chronolocial order.
     */
    if (latest)
    {
	int skip = element_count - count;

	while (skip--)
	    bump_up(mouse_data_buffer.last_read);
    }

    retval = count;	/* want to save 'count' as the return value */

    while (count--) {
	*addr++ = *mouse_data_buffer.last_read;
	bump_up(mouse_data_buffer.last_read);
    }

    mouse_buffer_unlock();
    mouse_splx(s);
    
    #ifdef MOUSE_DEBUG
    if (mouse_debug) raw_printf("[K return_raw_data returns %d]\n", retval);
    #endif
    return retval;
}

int mouse_set_status(
    int dev,
    int flavor,
    dev_status_t status,
    unsigned status_count)
{
    #ifdef MOUSE_DEBUG
    if (mouse_debug) raw_printf("[K mouse_set_status(flavor=%x)]\n", flavor);
    #endif

    switch(flavor)
    {
      case MOUSE_SET_FLUSH_DATA:
	flush_mouse_data();
        return D_SUCCESS;
    }
    return D_INVALID_OPERATION;
}

int mouse_get_status(
    int dev,
    int flavor,
    dev_status_t status,		/* OUT    */
    unsigned int *status_count)		/* IN/OUT */
{
    int elements;
    #ifdef MOUSE_DEBUG
    if (mouse_debug) raw_printf("[K mouse_get_status(flavor=%x)]", flavor);
    #endif

    switch(flavor)
    {
      default:
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K invalid flavor]\n");
	#endif
	return D_INVALID_OPERATION;

      case MOUSE_GET_NEXT_DATA_WITH_BLOCKING:
	elements = return_raw_data(
	    /* nodelay */0,
	    /* latest */ 0,
	    /* count */  mouse_status_count_2_element_count(*status_count),
	    /* addr */   (struct mouse_data *)status);
	(*status_count) = mouse_element_count_2_status_count(elements);
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K got w/b %d elements]\n", elements);
 	#endif
        return D_SUCCESS;

      case MOUSE_GET_NEXT_DATA_WITHOUT_BLOCKING:
	elements = return_raw_data(
	    /* nodelay */1,
	    /* latest */ 0,
	    /* count */  mouse_status_count_2_element_count(*status_count),
	    /* addr */   (struct mouse_data *)status);
	(*status_count) = mouse_element_count_2_status_count(elements);
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K got wo/b %d elements]\n", elements);
	#endif
        return D_SUCCESS;

      case MOUSE_GET_LAST_DATA_WITH_BLOCKING:
	elements = return_raw_data(
	    /* nodelay */0,
	    /* latest */ 1,
	    /* count */  mouse_status_count_2_element_count(*status_count),
	    /* addr */   (struct mouse_data *)status);
	(*status_count) = mouse_element_count_2_status_count(elements);
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K got last w/b %d elements]\n", elements);
	#endif
        return D_SUCCESS;

      case MOUSE_GET_LAST_DATA_WITHOUT_BLOCKING:
	elements = return_raw_data(
	    /* nodelay */1,
	    /* latest */ 1,
	    /* count */  mouse_status_count_2_element_count(*status_count),
	    /* addr */   (struct mouse_data *)status);
	(*status_count) = mouse_element_count_2_status_count(elements);
	#ifdef MOUSE_DEBUG
        if (mouse_debug) raw_printf("[K got last wo/b %d elements, status_count is %d]\n",elements, *status_count);
	#endif
        return D_SUCCESS;
    }
}

boolean_t mouse_read_done(io_req_t ior)
{
    ds_read_done(ior);
    return TRUE;
}

int mouse_read(dev_t dev, register io_req_t ior)
{
    int s = mouse_spltty();
    #ifdef MOUSE_DEBUG
    if (mouse_debug)
	raw_printf("[K mouse_read enqueuing(%x)]\n", ior);
    #endif

    async_flag = 1;
    ior->io_done = mouse_read_done;
    enqueue_tail(&mouse_read_queue, (queue_entry_t)ior);
    mouse_splx(s);
    return D_IO_QUEUED;
}

int mouse_open(unsigned dev, int flag)
{
    unsigned s;
    extern kbms_init;
    
    dev = minor(dev);
    simple_lock(&mouse_lock);
    if (dev != MOUSE || mouse_is_open) {
	simple_unlock(&mouse_lock);
	return D_NO_SUCH_DEVICE;
    }
    s = mouse_spltty();
    mouse_scan = 1;
    mouse_scanner();

    mouse_is_open++;
    flush_mouse_data();
    keyboard_control(MOUSE_ENABLE_CODE);

    if (kbms_init == 0) {
	extern void msparam();
	msparam(); /*???*/
	kbms_init = 1;
    }

    simple_unlock(&mouse_lock);
    mouse_splx(s);
    return 0;
}

int mouse_close(dev_t dev)
{
    unsigned s = mouse_spltty();
    simple_lock(&mouse_lock);

    if (mouse_is_open != 0 && --mouse_is_open == 0) {
        keyboard_control(MOUSE_DISABLE_CODE);  /* disable mouse */
        async_flag = 0;
        mouse_scan = 0;

	/* make sure that the waiting queue is empty */
	while (dequeue_head(&mouse_read_queue))
		;
    }

    mouse_splx(s);
    simple_unlock(&mouse_lock);
    return 0;
}
