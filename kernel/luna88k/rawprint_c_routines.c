/*
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 *
 * HISTORY
 * $Log:	rawprint_c_routines.c,v $
 * Revision 2.4  93/03/09  17:58:34  danner
 * 	Added no_rawprintf option.
 * 	[93/02/22            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:05:00  danner
 * 	Tidy.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:44:43  jfriedl
 * 	Created.
 * 	[92/07/24            jfriedl]
 * 
 *
 */

/*
 *****************************************************************RCS***
 *
 *	VSNprintf()  ---
 *		v is for Vector (takes a vector of printf arguments)
 *		s is for String (prints to a given buffer)
 *		n is as in strNcpy ... gives maximum length of the buffer.
 *
 *	I got the idea for the N part of this (making this vsnprintf
 *	rather than vsprintf) from some code from CMU I saw once).
 *
 *	Written for the MACH kernel for use in debugging routines.
 *	Jeffrey Friedl (jfriedl@omron.co.jp)
 *	Spring 1990
 *
 * My [jfriedl@nff.ncl.omron.co.jp] vsnprintf() routine for kernel debugging.
 *
 * It use rather little stack (i.e. no internal buffer), and so is good for
 * debugging things like kernel stack overflows, etc (which is one reason I
 * wrote it).  It does all it's work in the given buffer itself.
 *
 *	recognizes:
 *		%%		%
 *		%d		decimal int
 *		%u		unsigned int
 *		%x		hex (unsigned) int
 *		%o		octal (unsigned) int
 *		%b		binary (unsigned) int
 *		%c		int as a character
 *		%C		"cat -v"-like character.
 *		%s		string (char *)
 *		%f		float (sort of).
 *
 *
 * Between '%' and '[duxobcsf]' you may have a value (or a '*' in which case
 * the next argument is used) to specify a LENGTH value.
 *
 * Examples using sprintf(), etc., assumes that they use this vsnprintf()
 * as their base routine (makes the examples easier).
 *
 * You may also have a period followed by a value (or '*') to specify the
 * maximum length for numbers and strings. For example,
 *		sprintf(buffer, "%*.*s", len, len, string);
 * prints exactly <len> characters each time, regardless of what's in <string>.
 *
 * Note:  If numbers get chopped off because of a too-short length argument,
 * 	  or because of running out of room in the buffer, they will NOT come
 *	  out as expected....
 *
 * For %[duxobs], the LENGTH is the minimum length used.  If the number has
 * fewer characters (for %d, '-' counts as a character if it is present) than
 * LENGTH, it is pre-padded with spaces (with '0' if the first digit of LENGTH
 * is '0'). [to get padding with '0' and use '*', use something like "%0*x"].
 *
 *
 *	Example: the following are the same
 *		sprintf(buffer, "%05b", value);
 *		sprintf(buffer, "%0*b", 5, value);
 *
 * For %f, LENGTH concerns only the integral part. The decimal part is always
 * '.' plus three digits.  It is very simple... the following are the same
 * because the first just does (internaly) the 2nd.
 *		printf("%*f", len, value)
 *
 *		printf("%*d.%03d", len, (int)value,
 *			(int)((value - (int)value) * 1000.0));
 *
 * LENGTH is ignored for %C and %c.
 *
 * If vsnprintf() is called with the buffer area as NULL, it just returns
 * the number of characters that would be output [WITHOUT the trailing null].
 * The maximum length given, however, DOES include the trailing null.
 * Thus,
 *		snprintf(buffer, 5, "%s", "abcdefghijk")
 * would fill buffer as 'abcd\0' and return 4.
 */

/* name of the routine */
#ifndef VSNPRINTF_FCN_NAME
#define VSNPRINTF_FCN_NAME raw_vsnprintf
#endif

/* to have it translate \n to \n\r on ouput */
#ifndef TRANSLATE_NL_TO_NLCR
# define TRANSLATE_NL_TO_NLCR	 1
#endif

/* to use a local (but less efficient) ctype rather than libc one */
#ifndef USE_LOCAL_CTYPE
#define USE_LOCAL_CTYPE 1
#endif


#ifdef __STDC__
#  include <stdarg.h>
#else
#  include <varargs.h>
#endif

#if USE_LOCAL_CTYPE
# define isprint(c)	(c >= ' ' && c <= '~')
# define isdigit(c)	(c >= '0' && c <= '9')
#else
# include </usr/include/ctype.h>
#endif


#define SIGNED 1
#define UNSIGNED 0

/*
 * Insert2buffer(character)
 *
 * Macro that works with the variables 'max_len', 'out' and 'buffer'
 * in whatever scope it's used.  It works like:
 *
 * 	If there is room to send a new character
 *		Decrement the "space left" value.
 *		If there is a buffer to whoich to put it.
 *			put the character in the buffer, increment the ptr
 *		else
 *			increment the pointer.
 */
#define insert2buffer(character)		\
    (						\
	max_len == 0 ?				\
	    (void)0				\
	:					\
	(					\
	    (max_len--, buffer) ?		\
		(void)(*out++ = (character))	\
	    :					\
		(void)out++			\
	)					\
    )						\


/*
 * show_num(buffer, value, min_len, max_len, base, zerofill, sign)
 *
 * Fill the textual representation of VALUE (as base BASE)
 * to the buffer starting at OUT.
 *
 * A minimum of MIN_LEN characters will be output.  The number is treated
 * as signed if SIGN is non-zero.  If extra characters need to be pre-printed
 * to bring the length up to the minimum, ' ' will be used unles ZEROFILL
 * is non-zero in which case '0' is used.
 *
 * A maximum of MAX_LEN characters will be output. MAX_LEN overrides MIN_LEN.
 *
 * The number of characters actually "printed" is returned.
 */
#ifdef __STDC__
static int show_num(
    unsigned char *buffer,
    unsigned value,
    int min_len,
    int max_len,
    int base,
    int zerofill,
    int sign)
#else
static int show_num(buffer, value, min_len, max_len, base, zerofill, sign)
    unsigned char *buffer;
    unsigned value;
    int min_len;
    int max_len;
    int base;
    int zerofill;
    int sign;
#endif
{
	unsigned char *out;
	char *strcpy();
	int count;

	out = buffer;

	/*
	 * If we're to look at the sign, take care of all that stuff
	 * now.  Leave SIGN set to mean we must output a leading '-' if
	 * it is true later.
 	 */
	if (sign)
	{
		int signed_value = value;
		if (signed_value >= 0)
			sign = 0;
		else
		{
			min_len--;
			value = -signed_value;
		}
	}

	/* main loop to output digits */
	do
	{
		unsigned char C;

		if (C = '0' + (value % base), C > '9')
			C += 'a' - '9' - 1;

		/*
		 * The following is to attempt to make chopped-off numbers
		 * come out correctly (chopped off at the end).
		 */
		if (max_len == 0 && out != buffer)
		{
		    #ifndef bcopy
	               extern void bcopy();
		    #endif
		    /* I hope bcopy gets this overlapping move correct! */
		    out--;
		    max_len++;
		    bcopy(buffer+1, buffer, out-buffer);
		}


		insert2buffer(C);

		value /= base;
		min_len--;
	} while (value);

	while (min_len-- > 0)
		insert2buffer(zerofill ? '0' : ' ');

	if (sign)
		insert2buffer('-');

	count = out - buffer;

	if (buffer)
	{
		*out = '\0';

		/*
		 * Characters are now there, but backwards. Put right.
		 */
		while (buffer < --out)
		{
			unsigned char C;
			C = *buffer;
			*buffer++ = *out;
			*out = C;
		}
	}
	return(count);
}

#ifdef __STDC__
int VSNPRINTF_FCN_NAME(
    unsigned char *buffer,
    unsigned int max_len,
    unsigned char const *format,
    va_list ap)
#else
int VSNPRINTF_FCN_NAME(buffer, max_len, format, ap)
    unsigned char *buffer;
    unsigned int max_len;
    unsigned char *format;
    va_list ap;
#endif
{
    unsigned char *out = buffer;/* current "output" pointer */
    unsigned char C;		/* current character from the format string */
    unsigned zerofill;		/* if true, pad with '0' (rather than ' ') */
    int len;			/* minimum length as given in format string */
    int max_arg_len;		/* if nonzero, the max output from a %s */

    #define NEXT_ARG(type)	(va_arg(ap, type))

    if (max_len-- == 0)	/* subtract for the '\0' we always append */
	return 0;

    while (max_len != 0  &&  (C = *format++))
    {
	if (C != '%')
	{
		/* easy case.. just "print" the character */
		insert2buffer(C);

		#if TRANSLATE_NL_TO_NLCR
		    if (C == '\n')
			    insert2buffer('\r');
		#endif

		continue;
	}

	zerofill = (*format=='0'); /* pad with zero if 1st length char is '0'*/
	max_arg_len = max_len;

    /* at this point, C is '%', *format is the next character after */
	/* grab the length, if it's there */
	for (len = 0; C = *format, isdigit(C); format++)
		len = len * 10 + (C - '0');

    /* at this point, C and *format are the first character after any length */
	/*
	 * Maybe we read a length, maybe not.
	 * See if there's a '*' there to override
	 */
	if (C == '*')
	{
		len = NEXT_ARG(int);
		/* we don't yet, support negative lengths */
		if (len < 0)
			len = -len;

		C = *++format;
	}

    /* at this point, C and *format are the first character after any '*' */
	/* try to read a max string length */
	if (C == '.')
	{
		++format;	/* not beyond the '.' */

		/* now either a value or '*' */
		if ('*' == *format)
		{
			max_arg_len = NEXT_ARG(unsigned int);
			++format;	/* now beyond the '*' */
		}
		else
		{
			for (max_arg_len = 0; C = *format, isdigit(C); format++)
				max_arg_len = max_arg_len * 10 + (C - '0');
		}

		/* even if they want a lot, they still need to have the room */
		if (max_arg_len > max_len)
			max_arg_len = max_len;
	}

	/* should be at the format character now */
	C = *format++;

	switch(C)
	{
	    case '%':
		insert2buffer('%');
		break;

	    default:	/* huh? */
		insert2buffer('%');
		insert2buffer('?');
		insert2buffer(C);
		break;

	    case 'c':	/* character */
	    {
		C =NEXT_ARG(unsigned int);
		insert2buffer(C);
		#if TRANSLATE_NL_TO_NLCR
		    if (C == '\n')
			    insert2buffer('\r');
		#endif
		break;
	    }

	    case 'C':	/* CAT-V like character */
	    {
		unsigned char value = NEXT_ARG(unsigned int);

		if (isprint(value))
			insert2buffer(value);
		else if (value >= ('A' - 'A' + 1) && value <= ('Z' - 'A' + 1))
		{
			insert2buffer('^');
			insert2buffer(value + 'A' - 1);
		}
		else
		{
			int chars;
			insert2buffer('\\');
			chars = show_num(buffer ? out : 0, value,
				3, max_arg_len, 8, 1, UNSIGNED);
			out += chars;
			max_len -= chars;
		}
		break;
	    }

	    case 's':	/* string */
	    {
		unsigned char *ptr = NEXT_ARG(unsigned char *);
		while ((C = *ptr) && max_arg_len-- > 0)
		{
			insert2buffer(C);
			#if TRANSLATE_NL_TO_NLCR
			    if (C == '\n')
				    insert2buffer('\r');
			#endif
			ptr++;
			len--;
		}
		while (len-- > 0)
			insert2buffer(' ');
		break;
	    }

	    case 'x':	/* hex */
	    {
		int chars = show_num(buffer ? out : 0, NEXT_ARG(unsigned int),
			len, max_arg_len, 16, zerofill, UNSIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	    case 'd':	/* decimal (signed) */
	    {
		int chars = show_num(buffer ? out : 0, NEXT_ARG(int),
			len, max_arg_len, 10, zerofill, SIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	    case 'u':	/* unsigned decimal */
	    {
		int chars = show_num(buffer ? out : 0, NEXT_ARG(unsigned int),
			len, max_arg_len, 10, zerofill, UNSIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	    case 'o':  /* octal */
	    {
		int chars = show_num(buffer ? out : 0, NEXT_ARG(unsigned int),
			len, max_arg_len, 8, zerofill, UNSIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	    case 'b':	/* binary */
	    {
		int chars = show_num(buffer ? out : 0, NEXT_ARG(unsigned int),
			len, max_arg_len, 2, zerofill, UNSIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	    case 'F':	/* "float" */
	    case 'E':
	    case 'G':
	    case 'f':
	    case 'e':
	    case 'g':
	    {
		int chars;
		double value = NEXT_ARG(double);
		chars = show_num(buffer ? out : 0,
			(int)value, len, max_arg_len, 10, 0, SIGNED);
		out += chars;
		max_len -= chars;
		max_arg_len -= chars;

		value -= (int)value;
		insert2buffer('.');
		chars = show_num(buffer ? out : 0, (int)(value * 1000.0),
			3, max_arg_len, 10, 1, UNSIGNED);
		out += chars;
		max_len -= chars;
		break;
	    }
	}
    }
    if (buffer)
	*out = '\0';
    return(out - buffer);
}

/* 
 * If NO_RAWPRINTF is true, raw_printf calls get pumped to db_printf.
 */
unsigned no_rawprintf = 0;
void raw_printf(const char *fmt,...) asm ("raw_printf");
asm("  		    text				\n"
    "  		    align 4				\n"
    "  _raw_printf: global _raw_printf			\n"
    "               or.u  r10,  r0, hi16(_no_rawprintf)	\n"
    "		    ld    r10, r10, lo16(_no_rawprintf)	\n"
    "		    bcnd  eq0, r10, raw_printf		\n"
    "		    br	  _db_printf			\n");


/*
 * Raw_printf() is a very low-level print routine.
 * It uses a small buffer on the stack to so a sprintf of the data,
 * and then calls raw_putstr.
 *
 * Raw_putstr is defined in locore.c and, if JEFF_UART_DEBUG is turned
 * on, it sends the string to the serial tty line.
 * Otherwise, nothing is done and the whole call to raw_printf is just
 * about a No-op.
 */
#ifdef __STDC__
void raw_printf(const char *fmt, ...)
{
	va_list ap;
	unsigned char buffer[100];

	va_start(ap, fmt);
	(void)raw_vsnprintf(buffer, sizeof(buffer), (unsigned char *)fmt, ap);
	va_end(ap);
	raw_putstr(buffer);
}

#else /* else not STDC */

void raw_printf(fmt, va_alist)
unsigned char *fmt;
va_dcl
{
	va_list ap;
	unsigned char buffer[100];

	va_start(ap);
	(void)raw_vsnprintf(buffer, sizeof(buffer), (unsigned char *)fmt, ap);
	va_end(ap);
	raw_putstr(buffer);
}
#endif	/* end elst not STDC */
