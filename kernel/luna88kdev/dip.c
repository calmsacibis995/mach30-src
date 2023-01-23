/*
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
 * $Log:	dip.c,v $
 * Revision 2.2  93/01/14  17:39:20  danner
 * 	Added uncheked from Omron.
 * 	[92/12/08            jfriedl]
 * 
 */

#include <device/device_types.h>
#include <luna88k/pio.h>
#define DIPREAD 1
    
int dip_open(dev,flag)
int dev;
dev_mode_t flag;
{
    return D_SUCCESS;
}

int dip_close(dev)
int dev;
{
    return D_SUCCESS;
}

int dip_get_status(dev, flavor, status, status_count)
int dev;
int flavor;
dev_status_t status;
unsigned int *status_count;
{
    switch(flavor)
    {
	case DIPREAD:
	    *status = dipswitch.dipsws;
	    *status_count = sizeof(dipswitch.dipsws)/sizeof(int);
	    break;
    }
    return D_SUCCESS;
}
