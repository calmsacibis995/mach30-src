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
 * $Log:	mouse.h,v $
 * Revision 2.2  93/02/05  07:50:59  danner
 * 	Created.
 * 	[93/02/04            jfriedl]
 * 
 */

#define MOUSE_BUFFER_SIZE 100
struct mouse_data
{
    unsigned short time;
    short x_delta;
    short y_delta;
    unsigned char button_info;
};

#define mouse_status_count_2_element_count(STATUS_COUNT) \
    (((STATUS_COUNT) * sizeof(unsigned)) / sizeof(struct mouse_data))

#define mouse_element_count_2_status_count(ELEMENT_COUNT)                  \
  (                                                                        \
    (                                                                      \
      (ELEMENT_COUNT) * sizeof(struct mouse_data) + (sizeof(unsigned) - 1) \
    ) / sizeof(unsigned)                                                   \
  )

/* for get status */
#define MOUSE_GET_NEXT_DATA_WITH_BLOCKING	0x1122000
#define MOUSE_GET_NEXT_DATA_WITHOUT_BLOCKING	0x1122001
#define MOUSE_GET_LAST_DATA_WITH_BLOCKING	0x1122002
#define MOUSE_GET_LAST_DATA_WITHOUT_BLOCKING	0x1122003

/* for set status */
#define MOUSE_SET_FLUSH_DATA                    0x1123000
