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
 * $Log:	fb.c,v $
 * Revision 2.4  93/03/09  17:59:33  danner
 * 	Put palette base back to BMAP_PALLET2.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:08:43  danner
 * 	caddr_t -> vm_offset_t.
 * 
 * Revision 2.2  93/01/14  17:39:25  danner
 * 	Brought from Omron as "fbmap.c", renamed to "fb.c".
 * 	Reworked, made (more) UX-independent.
 * 	[92/12/08            jfriedl]
 * 
 *
 * Omron Revision 1.2  1992/02/12  13:46:17  shin
 * 	24plane bitmap boad support.
 * 	Identify boad by read from ROM in fbprobe().
 *
 */

/*
 * Frame memory device driver.
 *
 * device minor 	what
 *  0 (FBMAP)           frame memory map (0xb1080-)
 *  1 (ACMAP)           crtc-2 map (0xb12c0-)
 *  2 (PCEX)            pc-9800 board map. (defunct?)
 *  3 (MASKROM)		mask-rom map.
 *  4 (PALLET)		pallet register map.
 *  5 (ALLMAP)		all map (0xb1000000-0xd1000002)
 *
 */

#include "fb.h"
#if	NFB > 0

#include <luna88k/machdep.h>
#include <luna88k/fb.h>
#include <sys/types.h>
#include <device/device_types.h>
#include <chips/busses.h>
#include <luna88k/rom1ep.h> /* for CONSON and CONSOFF */

/* physcal page number for mmap supported device */
#define	BM_BASE		m88k_btop(BMAP_BMP)			
#define BM_TOP		( 0x240 - 0x1 )
#define ACRTC_BASE	m88k_btop(BMAP_FN)
#define ACRTC_TOP	( 0x240 - 0x1 )
#define PLT_BASE	m88k_btop(BMAP_PALLET2)
#define PLT_SRT		0x240
#define PLT_TOP		( 0x280 - 0x1 )
#define CRTC_BASE	m88k_btop(BMAP_CRTC)
#define	CRTC_SRT	0x280
#define	CRTC_TOP	( 0x2c0 - 0x1 )
#define RFCNT_BASE	m88k_btop(BMAP_RFCNT)
#define RFCNT_SRT	0x2c0
#define BMSEL_TOP	( 0x340 - 0x1 )
#define MASKROM_BASE	m88k_btop(MROM_ADDR)
#define MASKROM_TOP 	( 0x100 - 0x1 )
#define PALLET_TOP 	( 0x2 - 0x1 )
#define	ALLMAP_BASE	m88k_btop(BMAP_START)
#define	ALLMAP_TOP	( 0x300000 -0x1 )

#define PLANECOM	0x80	/* common plane + function set */

static unsigned long fb_framemem = 0;
static unsigned fb_planes = 0; /* #planes (nonzero implies BM present) */


int fb_open(dev,flag)
int dev, flag;
{
    if (fb_framemem == 0)
	return D_NO_SUCH_DEVICE;
    return D_SUCCESS;
}

int fb_close(dev)
int dev;
{
#if 0 /* isn't there a better way to provide this support? */
    /* for GMW I/F */
    if (minor(dev) == PALLET) {
#ifdef LUNA_ICON
	display_luna_icon();
#endif LUNA_ICON
	CONSON();
    }
#endif

    return D_SUCCESS;
}

/*
 * Return physical page frame of frame memory # corresponding to byte offset.
 * Return -1 if illegal.
 */
int fb_mmap(dev,pos,prot)
dev_t dev;
register unsigned long pos;
int prot;
{
    unsigned pg_pfn = (int)-1; /* default is error */
    pos = m88k_btop(pos);

    switch (minor(dev))
    {
      default:
	break;

      case FBMAP:
	if (pos < fb_framemem + PLANECOM)
	    pg_pfn = pos + BM_BASE;
        break;

      case ACMAP:
	if (pos <= ACRTC_TOP)
	    pg_pfn = pos + ACRTC_BASE;
	else if (pos >= PLT_SRT && pos <= PLT_TOP)
	    pg_pfn = pos - PLT_SRT + PLT_BASE;
	else if (pos >= CRTC_SRT && pos <= CRTC_TOP)
	    pg_pfn = pos - CRTC_SRT + CRTC_BASE;
	else if (pos >= RFCNT_SRT && pos <= BMSEL_TOP)
	    pg_pfn = pos - RFCNT_SRT + RFCNT_BASE;
        break;
	
      case MASKROM:
	if (pos <= MASKROM_TOP)
	    pg_pfn = pos + MASKROM_BASE;
	break;

      case PALLET:
	if (pos <= PALLET_TOP)
	    pg_pfn = pos + PLT_BASE;
	break;

      case ALLMAP:
	if (pos <= ALLMAP_TOP)
	    pg_pfn = pos + ALLMAP_BASE;
	break;
    }
    return (int)pg_pfn;
}

int fb_set_status(dev, flavor, ptr, size)
int dev;
int flavor;
dev_status_t ptr;
unsigned int size;
{
    switch(flavor)
    {
      default:
	return D_NO_SUCH_DEVICE;

      case FB_SET_CONS_ON:
	CONSON();	/* turn on */
	break;

      case FB_WRITE_LOGO:
	#ifdef LUNA_ICON
	display_luna_icon();
	#endif LUNA_ICON
	break;

      case FB_SET_CONS_OFF:
#ifndef JEFF_DEBUG /* don't want this during raw_printf debugging */
	CONSOFF();	/* turn off */
#endif
	break;

      case FB_SET_RFCNT:
	*(volatile unsigned *)BMAP_RFCNT = *(unsigned *)ptr;
	break;

      case FB_SET_BMSEL:
	*(volatile unsigned *)BMAP_BMSEL = *(unsigned *)ptr;
	break;

      case FB_MAP_VIA_BATC:
      {
	unsigned addr = *(unsigned *)ptr;
        extern task_t port_name_to_task();
	task_t t = port_name_to_task((mach_port_t)ptr[1]);
	/* data-cmmu num    virtual       physical    s  wt g  ci wp v */
	use_batc(t, 1,  0, addr + 0x000000U, 0xb1080000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  1, addr + 0x080000U, 0xb1100000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  2, addr + 0x100000U, 0xb1180000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  3, addr + 0x180000U, 0xb1200000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  4, addr + 0x200000U, 0xb1280000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  5, addr + 0x280000U, 0xb1300000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  6, addr + 0x300000U, 0xb1380000U, 0, 0, 0, 1, 0, 1);
	use_batc(t, 1,  7, addr + 0x380000U, 0xb1400000U, 0, 0, 0, 1, 0, 1);
	break;
      }
    }
    return D_SUCCESS;
}

int fb_get_status(dev, flavor, ptr, size)
int dev;
int flavor;
dev_status_t ptr;
unsigned int *size;
{
    switch(flavor)
    {
      default:
	return D_NO_SUCH_DEVICE;

      case FB_GET_PLANES:
	*(unsigned int *)ptr = fb_planes;
        *(size) = sizeof(unsigned int);
        break;
    }
    return(D_SUCCESS);
}

/*
 * Return true if frame buffer exists, false otherwise.
 * Also set external variables fb_framemem and fb_planes if appropriate.
 */
static int fbprobe(addr, unit)
vm_offset_t addr;
int unit;
{
    static int probed = 0;

    /* avoid recursive probe */
    if (probed)
	return 0;
    probed = 1;

    /* Check 24plane boad */
    if (addr != BMAP_BMP)
    {
	#define BM24_NAME_LEN 7
	#define BM24_NAME_SHIFT 24
	char ident[BM24_NAME_LEN] = "PWB7145";
	unsigned int *caddr;
	unsigned int words;
	int i;
	int bad = 0;

	/* Read Identify ROM */
	for (i = 0; i < BM24_NAME_LEN; i++)
	{
	    /* long word access only */
	    caddr = (long *)BMAP_IDENTROM + i;
	    if (badwordaddr(caddr)) {
		bad = 1;
		break;
	    }
	    /* mask lower 24bit */
	    words = *caddr & (0xff << BM24_NAME_SHIFT);
	    /* ROM substance appers highest 8bit */
	    if ((unsigned int)(ident[i] << BM24_NAME_SHIFT) != words) {
		bad = 1;
		break;
	    }
	}
	if (!bad) {
	    fb_planes = 24;
	    fb_framemem = m88k_btop((unsigned)
			   ((unsigned)BMAP_END24P - (unsigned)BMAP_START));
	    return (int)BMAP_START;
	}
    }

    /* Count how many planes, for 1,4,8 plane boads. */
    if (addr != BMAP_START)
    {
	static unsigned fb_chkaddr[] = {
	    BMAP_BMAP0, BMAP_BMAP1, BMAP_BMAP2, BMAP_BMAP3,
	    BMAP_BMAP4, BMAP_BMAP5, BMAP_BMAP6, BMAP_BMAP7
	};
	#define chk_count (sizeof(fb_chkaddr)/sizeof(fb_chkaddr[0]))
	register int i;
    
	for (i = 0; i < chk_count; i++)
	{
	    volatile unsigned *ptr = (unsigned *)fb_chkaddr[i];
	    if (badwordaddr((unsigned)ptr))
		break;
	    else {
		unsigned saved_data = *ptr;
		if (*ptr = 0x12345678, *ptr != 0x12345678)
		    break;
		*ptr = saved_data; /* restore */
	    }
	}
	if (i != 0) {
	    fb_planes = i;
	    if (i >= chk_count)
	        fb_framemem = m88k_btop(BMAP_FN0 - BMAP_BMAP0);
	    else
	        fb_framemem = m88k_btop(fb_chkaddr[i] - BMAP_BMAP0);
	}
    }
    
    if (fb_framemem)
	return (int)m88k_ptob(fb_framemem + PLANECOM);

    return 0;
}
static void fbattach()
{
    printf(" (%d plane%s)", fb_planes, fb_planes != 1 ? "s" : "");
}

static vm_offset_t fbSTD[] = { BMAP_BMP };
struct bus_device *fbdinfo[NFB];

struct bus_driver fb_driver = {
    fbprobe,		/* see if the driver is there */
    0,			/* see if the slave is there */
    fbattach,		/* setup after probe */
    0,			/* start transfer */
    fbSTD,		/* "device csr addresses"(??) */
    "fb",		/* device name */
    fbdinfo,		/* backpointers */
    0,			/* controller name */
    0,			/* backpointers */
#if 1
	0
#else
    BUS_INTR_B4_PROBE	/* flags */
#endif
};
#endif /*NFB*/


#if 0
  case FBIOCBATC:
    a = (struct a *)arg;
    addr = a->arg1;
    my_task = port_name_to_task(a->arg2);
    /* data-cmmu num    virtual       physical    s  wt g  ci wp v */
    use_batc(my_task, 1,0, addr+0x000000, 0xb1080000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,1, addr+0x080000, 0xb1100000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,2, addr+0x100000, 0xb1180000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,3, addr+0x180000, 0xb1200000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,4, addr+0x200000, 0xb1280000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,5, addr+0x280000, 0xb1300000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,6, addr+0x300000, 0xb1380000U, 0,0, 0, 1, 0, 1);
    use_batc(my_task, 1,7, addr+0x380000, 0xb1400000U, 0,0, 0, 1, 0, 1);
#endif
