/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
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
 * $Log:	clock.c,v $
 * Revision 2.7  93/01/26  18:02:31  danner
 * 	ANSIfied. Cleaned up includes.
 * 	Inlined and removed clock.h
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.6  92/08/03  17:40:57  jfriedl
 * 	Fixed the clock/calendar code [inui@omron.co.jp]
 * 	Fixed CPU speed code [jfriedl]
 * 
 * Revision 2.4.2.1  92/05/27  14:35:25  danner
 * 	Header file change support.
 * 	[92/05/27            danner]
 * 
 * Revision 2.5  92/05/21  17:17:48  jfriedl
 *      Cleaned up and de-linted.
 * 	Fixed call to gmtime() in resettodr() to pass the _address_.
 * 	[92/05/16            jfriedl]
 * 
 * Revision 2.4  92/02/18  18:00:31  elf
 * 	Since sprintf disappeared, just special case it.
 * 	[92/01/19            danner]
 * 
 * Revision 2.3  91/08/24  12:01:52  af
 * 	Changed snprintf to sprintf, upped buffer size.
 * 	[91/07/20  15:34:04  danner]
 * 
 * Revision 2.2.3.1  91/08/19  13:45:57  danner
 * 	Changed snprintf to sprintf, upped buffer size.
 * 	[91/07/20  15:34:04  danner]
 * 
 * Revision 2.2  91/07/09  23:16:42  danner
 * 	cputypes.h -> platforms.h
 * 	[91/06/24            danner]
 * 
 * 	Butchered for 3.0
 * 	[91/05/06            danner]
 * 
 * Revision 2.2  91/04/05  14:01:16  mbj
 * 	Initial code from the Omron 1.10 kernel release corresponding to X130.
 * 	The Copyright has been adjusted to correspond to the understanding
 * 	between CMU and the Omron Corporation.
 * 	[91/04/04            rvb]
 * 
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)clock.c	7.1 (Berkeley) 6/5/86
 */

#include <luna88k/machdep.h>	/* standard goodies */
#include <luna88k/nvram.h>	/* NVRAM_ADDR, etc. */
#include <m88k/cpu_number.h>    /* master_cpu       */
#include <m88k/pmap.h>		/* pmap_extract()   */
#include <kern/time_out.h>	/* hz               */
#include <sys/time.h>		/* time, etc.       */
#include <sys/types.h>		/* time_t           */

static int dmsize[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* lifted from BSD  sys/time.h */
struct tm {
    int	tm_sec;
    int	tm_min;
    int	tm_hour;
    int	tm_mday;
    int	tm_mon;
    int	tm_year;
    int	tm_wday;
    int	tm_yday;
    int	tm_isdst;
};

/*
 * Machine-dependent clock routines.
 *
 * Startrtclock restarts the real-time clock, which provides
 * hardclock interrupts to kern_clock.c.
 *
 * Inittodr initializes the time of day hardware which provides
 * date functions.  Its primary function is to use some file
 * system information in case the hardare clock lost state.
 *
 * Resettodr restores the time of day hardware after a time change.
 */
static int battery_clock;
static int battery_chkfg;	/* battery checked flag */

/*
 * The routine that sets cycles_per_microsecond is called pretty late in
 * the boot.  However, cycles_per_microsecond is used by
 * the delay_in_microseconds() routine for timing, and is used before
 * determine_cpu_speed() is called.
 *
 * Although it's not a great fix, it's easy.  We'll assume, for the limited
 * time that it's needed before really set, that we're on a really fast
 * processor (33Mhz).  That way, we'll wait too long on slower ones,
 * which is likely to be less painful than not waiting long enough
 * (waiting for whatever...)
 */
#define INITIAL_MHZ_GUESS 33.0
double cycles_per_microsecond = INITIAL_MHZ_GUESS;

static void timeout_routine(volatile int *timeoutflag)
{
    *timeoutflag = 1;
}

/*
 * Called from "luna88k/machdep.c"
 */
void determine_cpu_speed(void)
{
    static char *num[] = {"zero(?!)", "ONE", "TWO", "THREE", "FOUR"};
    static volatile int timeoutflag;
    unsigned cycles;
    double clock_mhz;
    char *speed, buffer[6];

    timeoutflag = 0;
    timeout((int(*)())timeout_routine, (char*)&timeoutflag, hz);/* wait 1 sec*/
    cycles = measure_pause(&timeoutflag);

    /*
     * Now have approximate number of cycles for the last second.
     * However, this doesn't count the cycles used servicing the 99 clock
     * interrupts, and the cycles spent between setting the timeout
     * and starting counting.
     *
     * By observation, I have noted that the time lost while starting
     * the timeout is insignificant when the length of the timeout
     * is over about 0.5 seconds.
     *
     * The number of cycles reported is about 99.17% of the true clock
     * speed.  Close'nuff. If we want to make up for this, we'll
     * multiply by 1.008377.
     */
    cycles = cycles * 1.008377;	/* make up difference */
    clock_mhz = cycles_per_microsecond = cycles / 1000000.0;
    if (clock_mhz >= 24.3 && clock_mhz <= 25.2)
	    speed = "25";
    else if (clock_mhz >= 19.7 && clock_mhz <= 20.1)
	    speed = "20";
    else if (clock_mhz >= 15.7 && clock_mhz <= 16.1)
	    speed = "16";
    else if (clock_mhz >= 32.7 && clock_mhz <= 33.1)
	    speed = "33";
    else if (clock_mhz >= 39.7 && clock_mhz <= 40.1)
	    speed = "40";
    else
    {
	unsigned a, b, c;
	static char digits[] = "0123456789";

	clock_mhz += 0.05; /* round to nearest single decimal place */
	a = ((unsigned)clock_mhz) / 10;
	b = ((unsigned)clock_mhz) % 10;
	c = (unsigned)((clock_mhz - (unsigned)clock_mhz) * 10.0);
	if (c > 9) /* over 99.9 MHZ! */
	    speed = ">99.9!";
	else
	{
	    speed = buffer;
	    buffer[0] = digits[a];
	    buffer[1] = digits[b];
	    buffer[2] = '.';
	    buffer[3] = digits[c];
	    buffer[4] = '\0';
	}
    }
    #define plural(num)     ((num)==1 ? "" : "s")
    printf("%s MC88100 CPU%s RUNNING AT %sMHz.\n",
	    num[initialized_cpus], plural(initialized_cpus), speed);
}

static void batterychk(void)
{
    static char btchkdata[] = "chk";
    struct nvram *nr = (struct nvram *)NVRAM_ADDR;
    char buf[sizeof(btchkdata)];

    /* if already checked, return */
    if (battery_chkfg)
	    return;

    battery_chkfg = 1;
    if (badaddr((vm_offset_t)nr, 2))
	    return;

    strcpy(buf, btchkdata);
    nvram_write(buf, (vm_offset_t)nr->nv_testwrite, sizeof(btchkdata));
    nvram_read(buf, (vm_offset_t)nr->nv_testwrite, sizeof(btchkdata));
    if (strncmp(buf, btchkdata, sizeof(btchkdata))) {
	    printf("WARNING: calendar clock battery down\n");
	    return;
    }
    /* shouldn't battery_clock be set based upon the above strncmp? */
    battery_clock = 1;
}

#define RTC_FRQ         0x40            /* Frequency test (in day) */
#define RTC_KICK        0x80            /* Kick start (in hour) */
#define RTC_STOP        0x80            /* Stop bit (in seconds) */
#define RTC_WRT         0x80            /* Write bit (in control) */
#define RTC_RD          0x40            /* Read bit (in control) */
#define RTC_SGN         0x20            /* Sign bit (in control) */
#define RTC_DELAY       180             /* delay time */

/*
 * Initialize the time keeper device.
 */
static void tmkeepinit(void)
{
    static char rtcstrings[] = "RTC";
    struct nvram *nr;
    char buf[sizeof(rtcstrings)];

    batterychk();
    if (!battery_clock)
	    return;

    nr = (struct nvram *)NVRAM_ADDR;
    nvram_read(buf, (vm_offset_t)nr->nv_calclock, sizeof(rtcstrings));
    if (!strncmp(buf, rtcstrings, sizeof(rtcstrings))) {
	    if (!((*(volatile unsigned char*)OBIO_CAL_SEC) & RTC_STOP))
		    return;
    }

    printf("Initialize Battery Backup Clock\n");
    *(volatile char *)OBIO_CAL_CTL |= RTC_WRT;
    *(volatile char *)OBIO_CAL_SEC &= ~RTC_STOP;
    *(volatile char *)OBIO_CAL_HOUR |= RTC_KICK;
    *(volatile char *)OBIO_CAL_DOW &= ~RTC_FRQ;
    *(volatile char *)OBIO_CAL_CTL &= ~RTC_WRT;
    delay_in_microseconds(RTC_DELAY);
    *(volatile char *)OBIO_CAL_CTL |= RTC_WRT;
    *(volatile char *)OBIO_CAL_HOUR &= ~RTC_KICK;
    *(volatile char *)OBIO_CAL_CTL &= ~RTC_WRT;
    strcpy(buf,rtcstrings);
    nvram_write(buf, (vm_offset_t)nr->nv_calclock, sizeof(rtcstrings));
}

/*
 * Start the real-time clock.
 */
void startrtclock(void)
{
    start_clock();
    if (master_cpu != cpu_number())
	return;
    tmkeepinit();

    /*
     * we should be able to run determine_cpu_speed here, but we can't
     * because we don't have a thread yet, so thread_quantum_update
     * (called by the clock interrupt routine) is unhappy.
     */
}

#define dysize(year) ((year)%4 ? 365 : 366)

__inline__ static unsigned binary_to_bcd(unsigned i)
{
    return (i / 10) << 4 | (i % 10);
}


__inline__ static unsigned bcd_to_binary(unsigned i)
{
    return (i >> 4) * 10 + (i & 0x0f);
}

/*
 * Initialize the time of day register, based on the time base which is, e.g.
 * from a filesystem.  Base provides the time to within six months,
 * and the time of year clock provides the rest.
 *
 * Called from "luna88k/autoconf.c"
 */
void inittodr(void)
{
    short year, month, day, hour, mins, secs;
    register int i;

    time_t base;

    /*
     * read the calendar clock
     */
    register int s = splimp();
    
    *(volatile char *)OBIO_CAL_CTL |= RTC_RD;
    year =  bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_YEAR);
    month = bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_MON);
    day =   bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_DAY);
    hour =  bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_HOUR);
    mins =  bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_MIN);
    secs =  bcd_to_binary(*(volatile unsigned char *)OBIO_CAL_SEC);
    *(volatile char *)OBIO_CAL_CTL &= ~RTC_RD;
    splx(s);
    year -= 70;
    base = 0;
    for (i = 0; i < year; i++)
      base += dysize(70 + i);
    /* Leap year */
    if (dysize(70 + year) == 366 && month >= 3)
      base++;
    while (--month)
      base += dmsize[month-1];
    base += day-1;
    base = 24*base + hour;
    base = 60*base + mins;
    base = 60*base + secs;
    time.seconds = base;
    return;
}

static struct tm *gmtime(unsigned long *tim)
{
    register int d0, d1;
    long hms, day;
    register int *tp;
    static struct tm xtime;

    /* break initial number into days */
    hms = *tim % 86400;
    day = *tim / 86400;
    if (hms < 0)
    {
	    hms += 86400;
	    day -= 1;
    }
    tp = (int *)&xtime;

    /* generate hours:minutes:seconds */
    *tp++ = hms % 60;
    d1 = hms / 60;
    *tp++ = d1 % 60;
    d1 /= 60;
    *tp++ = d1;

    /*
     * day is the day number. generate day of the week.
     * The addend is 4 mod 7 (1/1/1970 was Thursday)
     */
    xtime.tm_wday = (day+7340036)%7;

    /* year number */
    if (day>=0) for(d1=70; day >= dysize(d1); d1++)
	    day -= dysize(d1);
    else for (d1=70; day<0; d1--)
	    day += dysize(d1-1);
    xtime.tm_year = d1;
    xtime.tm_yday = d0 = day;

    /* generate month */
    if (dysize(d1) == 366)
	    dmsize[1] = 29;
    for(d1=0; d0 >= dmsize[d1]; d1++)
	    d0 -= dmsize[d1];
    dmsize[1] = 28;
    *tp++ = d0+1;
    *tp++ = d1;
    xtime.tm_isdst = 0;
    return(&xtime);
}
/*
 * Reset the TODR based on the time value; used when the TODR
 * has a preposterous value and also when the time is reset
 * by the stime system call.  Also called when the TODR goes past
 * TODRZERO + 100*(SECYEAR+2*SECDAY) (e.g. on Jan 2 just after midnight)
 * to wrap the TODR around.
 *
 * Called from "kern/mach_clock.c".
 */
void resettodr(void)
{
    register struct tm *tm;
    register int s;

    if (!battery_clock)
	    return;

    tm = gmtime(&time.seconds);
    tm->tm_mon += 1;

    /* I do not know whether I can change year, month,...
     * without setting somethine to the control register.
     */

    s = splimp();
    *(volatile unsigned char *)OBIO_CAL_CTL |= RTC_WRT;
    *(volatile unsigned char *)OBIO_CAL_YEAR = binary_to_bcd(tm->tm_year);
    *(volatile unsigned char *)OBIO_CAL_MON = binary_to_bcd(tm->tm_mon);
    *(volatile unsigned char *)OBIO_CAL_DAY = binary_to_bcd(tm->tm_mday);
    *(volatile unsigned char *)OBIO_CAL_DOW = binary_to_bcd(tm->tm_wday);
    *(volatile unsigned char *)OBIO_CAL_HOUR = binary_to_bcd(tm->tm_hour);
    *(volatile unsigned char *)OBIO_CAL_MIN = binary_to_bcd(tm->tm_min);
    *(volatile unsigned char *)OBIO_CAL_SEC = binary_to_bcd(tm->tm_sec);
    *(volatile unsigned char *)OBIO_CAL_CTL &= ~RTC_WRT;
    splx(s);
}

/*
 * Called from "luna88k/conf.c"
 */
int timemmap(int dev, int off, vm_prot_t prot)
{
    extern void *mtime;

    if (prot & VM_PROT_WRITE)
	    return -1;

    return (m88k_btop(pmap_extract(pmap_kernel(), (vm_offset_t)mtime)));
}
