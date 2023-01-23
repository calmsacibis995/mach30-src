#define KEYBOARD_DEBUG 		1

/*
 * Mach Operating System
 * Copyright (c) 1993,1991 Carnegie Mellon University
 * Copyright (c) 1993,1991 OMRON Corporation
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
 * $Log:	kbmain.c,v $
 * Revision 2.8  93/03/09  17:59:36  danner
 * 	Rewrote and simplified (in progress)
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.7  93/01/26  18:07:36  danner
 * 	ANSIfied.
 * 	[93/01/25            jfriedl]
 *
 * Revision 2.6  93/01/14  17:39:29  danner
 */

/*
 * Keyboard driver
 */

#include <luna88k/machdep.h>
#include <device/tty.h>
#include <luna88kdev/uart.h>
#include <luna88kdev/kbms.h>
#include <luna88kdev/codetbl.h>


static unsigned short *current_encoding_table;
static unsigned short tblspcl[CODTBSZ];
static unsigned short tblctrl[CODTBSZ];
static unsigned short tblcaps[CODTBSZ];
static unsigned short tblsift[CODTBSZ];
static unsigned short tblknsf[CODTBSZ];
static unsigned short tblcpsf[CODTBSZ];
static unsigned short tblnoml[CODTBSZ];
static int do_keyclick = 0;
static int keyclick_buzzer_code = /* set some reasonable default */
	BUZZER_CODE(BUZZER_LENGTH__40msec, BUZZER_TONE_3000Hz);

static inline void click(void) {
    if (do_keyclick)
	keyboard_control(keyclick_buzzer_code);
}

int kbreset; 	/* accessed from uart.c */
int bztime = 0; /* accessed from uart.c */


/*
 * Keyboard state.
 */
struct {
    unsigned alt:1;		/* true if ALT key is down */
    unsigned ctrl:1;		/* true if CTRL key is down */
    unsigned kana:1;		/* true if KANA key is locked (down) */
    unsigned caps:1;		/* true if CAPS key is locked (down) */
    unsigned leftshift:1;	/* true if left SHIFT key is down */
    unsigned rightshift:1;	/* true if right SHIFT key is down */
    unsigned encoding_mode:3; 	/* SEMIENCODED, ENCODED, KB_RAW */
    unsigned allow_repeat:1;    /* if false, all repeating is turned off */
    unsigned dorepeat:1;	/* if true, some characters are repeating now */
} kb_state;

static short repeat_start_delay;
static short intra_character_repeat_timing;

#define keyonly(code)	(code & 0x7f)
#define release(code)	((code & 0x80) == 0x80)

#define MAX_KEYS_DOWN	10
static unsigned char keydown_list[MAX_KEYS_DOWN];
static unsigned int chars_in_keydown_list = 0;

#define MODTBSZ 16             /* mode table size        */

#define REPCNT  30             /* repeat delay 16.6*30 ms */
#define RPINTVL 3              /* repeat interval timer 16.6*3 ms */


/*  MODE TABLE  */
static short t_mode[MODTBSZ] =
{
    ARST,      /* cursor key   */
    ANSIKNM,   /* keypad key   */
};

/*
 * Escape expansion table for full encoding.
 */
#define MAX_EXPANSION_LEN  	4
#define ESC 0x1b
static struct {
    unsigned char keycode;	/* raw keyboard code */
    unsigned char mode;
    unsigned char expansion[MAX_EXPANSION_LEN];
} encoded_expansions[] = {
    { KEYCODE_F1,	ANSIKNM, { ESC, 'O', 'P' }},  /* ESCOP */
    { KEYCODE_F2,	ANSIKNM, { ESC, 'O', 'Q' }},  /* ESCOQ */
    { KEYCODE_F3,	ANSIKNM, { ESC, 'O', 'R' }},  /* ESCOR */
    { KEYCODE_F4,	ANSIKNM, { ESC, 'O', 'S' }},  /* ESCOS */
    { KEYCODE_F5,	ANSIKNM, { ESC, 'O', 'T' }},  /* ESCOT */
    { KEYCODE_F6,	ANSIKNM, { ESC, 'O', 'U' }},  /* ESCOU */
    { KEYCODE_F7,	ANSIKNM, { ESC, 'O', 'V' }},  /* ESCOV */
    { KEYCODE_F8,	ANSIKNM, { ESC, 'O', 'W' }},  /* ESCOW */
    { KEYCODE_F9,	ANSIKNM, { ESC, 'O', 'X' }},  /* ESCOX */
    { KEYCODE_F10,	ANSIKNM, { ESC, 'O', 'Y' }},  /* ESCOY */
    { KEYCODE_F11,	ANSIKNM, { ESC, 'O', 'a' }},  /* ESCOa */
    { KEYCODE_F12,	ANSIKNM, { ESC, 'O', 'b' }},  /* ESCOb */
    { KEYCODE_F13,	ANSIKNM, { ESC, 'O', 'c' }},  /* ESCOc */
    { KEYCODE_F14,	ANSIKNM, { ESC, 'O', 'd' }},  /* ESCOd */
    { KEYCODE_UP,	ARST,	 { ESC, '[', 'A' }},  /* ESC[A */
    { KEYCODE_LEFT,	ARST,	 { ESC, '[', 'D' }},  /* ESC[D */
    { KEYCODE_RIGHT,	ARST,	 { ESC, '[', 'C' }},  /* ESC[C */
    { KEYCODE_DOWN,	ARST,	 { ESC, '[', 'B' }},  /* ESC[B */

    { KECODE_KP_RETURN,	ANSIKNM, 0x0d },
    { KECODE_KP_PLUS,	ANSIKNM, '+' },
    { KECODE_KP_MINUS,	ANSIKNM, '-' },
    { KECODE_KP_0,	ANSIKNM, '0' },
    { KECODE_KP_1,	ANSIKNM, '1' },
    { KECODE_KP_2,	ANSIKNM, '2' },
    { KECODE_KP_3,	ANSIKNM, '3' },
    { KECODE_KP_4,	ANSIKNM, '4' },
    { KECODE_KP_5,	ANSIKNM, '5' },
    { KECODE_KP_6,	ANSIKNM, '6' },
    { KECODE_KP_7,	ANSIKNM, '7' },
    { KECODE_KP_8,	ANSIKNM, '8' },
    { KECODE_KP_9,	ANSIKNM, '9' },
    { KECODE_KP_PERIOD, ANSIKNM, '.' },
    { KECODE_KP_MUL,	ANSIKNM, '*' },
    { KECODE_KP_DIV,	ANSIKNM, '/' },
    { KECODE_KP_EQUAL,	ANSIKNM, '=' },
    { KECODE_KP_COMMA,	ANSIKNM, ',' },

};
#define EXPANSIONS (sizeof(encoded_expansions)/sizeof(encoded_expansions[0]))


static int repeat_demon_start_countdown;
static int repeat_demon_intra_countdown;
static unsigned char repeat_buffer[MAX_EXPANSION_LEN];
static unsigned repeat_buffer_len;
#define MAX_UNINTERRUPTED_REPEAT_COUNT 256
unsigned uninterrupted_repeat_count;


/*
 * Resets the state of all keys that shift (ALT, CTRL, SHIFTs, etc.)
 * and resets no characters to be repeating at the moment.
 */
void luna_reset_simple_keyboard_state(void)
{
    kb_state.dorepeat = 0;
    chars_in_keydown_list = 0;
    kb_state.alt = 0;
    kb_state.ctrl = 0;
    kb_state.kana = 0;
    kb_state.caps = 0;
    kb_state.leftshift = 0;
    kb_state.rightshift = 0;
}

void luna_show_keyboard_state(void)
{
    db_printf("[keyboard state:");
    if (kb_state.alt) db_printf(" ALT");
    if (kb_state.ctrl) db_printf(" CTRL");
    if (kb_state.kana) db_printf(" KANA");
    if (kb_state.caps) db_printf(" CAPS");
    if (kb_state.leftshift) db_printf(" LEFT");
    if (kb_state.rightshift) db_printf(" RIGHT");
    db_printf(" encoding=%d", kb_state.encoding_mode);
    db_printf(" allow repeat=%d", kb_state.allow_repeat);
    db_printf(" dorepeat=%d]\n", kb_state.dorepeat);
}

/*
 * Returns true if it was added. We generally return the keycode, as
 * it's a non-zero value we have handy...
 */
static inline unsigned add_to_keydown_list(unsigned char keycode)
{
    int i;
    /*
     * If the character's already there, it's likely due to some hassles
     * with the user having entered the debugger or something, knocking
     * us out of sync.  We'll just pretend we've inserted it if it's
     * already there.
     */
    for (i = 0; i < chars_in_keydown_list; i++)
	if (keydown_list[i] == keycode)
	    return keycode;

    if (chars_in_keydown_list < MAX_KEYS_DOWN)
        return keydown_list[chars_in_keydown_list++] = keycode;

    return 0;/* too full to insert */
}

static void remove_from_keydown_list(unsigned char keycode)
{
    int i, j;
    for (i = 0; i < chars_in_keydown_list; i++)
	if (keydown_list[i] == keycode) {
	    /* found it, now remove; shuffle all others down one */
	    for (j = i+1; j < chars_in_keydown_list; j++)
		keydown_list[i] = keydown_list[j];
	    chars_in_keydown_list--;
	}
}


void kbsetinit(unsigned flag)  /*  flag=0:initial  ;  flag=1:reset  */
{
    int s;
    /* encoded table initial */
    /* special encoded table */
    memcpy((char*)tblspcl, (char*)t_specl, CODTBSZ*2);

    /* control encoded table */
    memcpy((char*)tblctrl, (char*)t_cntrl, CODTBSZ*2);

    /* capsloc encoded table */
    memcpy((char*)tblcaps, (char*)t_cploc, CODTBSZ*2);

    /* shift   encoded table */
    memcpy((char*)tblsift, (char*)t_shift, CODTBSZ*2);

    /* capslock + shift encoded table */
    memcpy((char*)tblcpsf, (char*)t_capsf, CODTBSZ*2);

    /* normal  encoded table */
    memcpy((char*)tblnoml, (char*)t_nomal, CODTBSZ*2);

    /* semi-encoded table */
#if 0
    memcpy((char*)keymap,  (char*)t_keymap ,CODTBSZ);
#endif

    /* mode table */
#if 0
    memcpy((char*)tblmode, (char*)t_mode , MODTBSZ*2);
#endif

    /* escape table */
#if 0
    memcpy((char*)tblescp, (char*)t_escap, ESCBYTE);
#endif

    /* flag initial */

    kb_state.ctrl = 0;
    kb_state.alt = 0;
    kb_state.leftshift = 0;
    kb_state.rightshift = 0;
    if (!flag) {
	kb_state.kana = 0;
	kb_state.caps = 0;
    }

    kb_state.encoding_mode = ENCODED;
    kb_state.dorepeat = 0;
    kb_state.allow_repeat = 1;

    /* counter initial */

    chars_in_keydown_list = 0;
    repeat_start_delay = REPCNT;
    intra_character_repeat_timing = RPINTVL;

    /* pointer initial */

    current_encoding_table = tblnoml;

    do_keyclick = 0;
    kbreset = ENABLE;
}

void set_dorepeat(void)
{
    repeat_demon_start_countdown = repeat_start_delay;
    repeat_demon_intra_countdown = 0;
    uninterrupted_repeat_count = MAX_UNINTERRUPTED_REPEAT_COUNT;
    kb_state.dorepeat = 1;
}

int kbrepeat(unsigned char *buf)
{
    int i;

    if(!kb_state.allow_repeat || !kb_state.dorepeat)
	return 0;

    if (repeat_demon_start_countdown != 0) {
	repeat_demon_start_countdown -= 1;
	return 0;
    }

    if (repeat_demon_intra_countdown != 0) {
	repeat_demon_intra_countdown -= 1;
	return 0;
    }

    /* reset for next time */
    repeat_demon_intra_countdown = intra_character_repeat_timing;

    if (--uninterrupted_repeat_count <= 0)
    {
	/* We've been repeating too long.... turn off these repeating keys */
	luna_reset_simple_keyboard_state();
	/* give a funky beep to indicate what we've done */

#ifdef JEFF_DEBUG
	raw_printf("<flushing repeat...count was %d, code is %x>\n",
		chars_in_keydown_list, keydown_list[chars_in_keydown_list-1]);
#endif

	BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE_1500Hz);
	delay_in_microseconds(150*1000);
	BUZZ(BUZZER_LENGTH__40msec,  BUZZER_TONE_3000Hz);
	delay_in_microseconds(40*1000);
	BUZZ(BUZZER_LENGTH_150msec,  BUZZER_TONE__600Hz);
	return 0;
    }

    for (i = 0; i < repeat_buffer_len; i++)
	buf[i] = repeat_buffer[i];

    click();

    return repeat_buffer_len;
}

int kb_get_status(struct tty *tp, int flavor, int *data, unsigned *count)
{
    switch(flavor) {
      case FLGMODE:
	    *data = kb_state.encoding_mode;
	    *count = 1;
	    break;
      case KBGETMAP:
      case KEYMAP:
#if 0
	    bcopy(keymap, data, CODTBSZ);
	    *count = CODTBSZ/sizeof(int);
#else
	*count = 0;
#endif
	    break;
      case TBLSPCL:
	    bcopy(tblspcl, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLKNSF:
	    bcopy(tblknsf, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLKANA:
	    *count = 0;
	    break;
      case TBLCPSF:
	    bcopy(tblcpsf, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLCAPS:
	    bcopy(tblcaps, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLSIFT:
	    bcopy(tblsift, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLNOML:
	    bcopy(tblnoml, data, CODTBSZ*2);
	    *count = CODTBSZ*2/sizeof(int);
	    break;
      case TBLESCP:
#if 0
	    bcopy(tblescp, data, ESCTSIZ*ESCSIZE);
	    *count = ESCTSIZ*ESCSIZE/sizeof(int);
#else
	*count = 0;
#endif
	    break;
      case TBLMODE:
#if 0
	    bcopy(tblmode, data, CODTBSZ);
	    *count = CODTBSZ/sizeof(int);
#else
	*count = 0;
#endif
	    break;
      case T_KEYMAP:
#if 0
	    bcopy(t_keymap, data, CODTBSZ);
	    *count = CODTBSZ/sizeof(int);
#else
	*count = 0;
#endif
	    break;
	}
    return(0);
}

int kb_set_status(struct tty *tp, int flavor, int *data, unsigned count)
{
    switch (flavor)
    {
      case KBSETM:	/* set keyboard mode */
	switch(*data)
	{
	  default:
	    return D_INVALID_OPERATION;

	  case KB_RAW:
	    kb_state.encoding_mode = KB_RAW;
	    kb_state.dorepeat = 0;
	    chars_in_keydown_list = 0;
	    break;

	  case ENCODED:
	    kb_state.dorepeat = 0;
	    chars_in_keydown_list = 0;
	    kbsetinit(1);
	    kb_state.encoding_mode = ENCODED; /* just to be sure */
	    break;

	  case SEMIENCODED:
	    chars_in_keydown_list = 0;
	    kb_state.dorepeat = 0;
	    kb_state.encoding_mode = SEMIENCODED;
	    break;
	}
	return D_SUCCESS;

      case KBRSTM:
	kbsetinit(1);
	return D_SUCCESS ;

      case KBREPTIM:
      {
	struct kbrepeat *r = (struct kbrepeat *)data;
	if (repeat_start_delay = r->repstart, repeat_start_delay == 0)
	    kb_state.allow_repeat = 0;
	else
	    kb_state.allow_repeat = 1;
	intra_character_repeat_timing = r->repinterval;
	return D_SUCCESS;
      }

      case KBRSTENABLE:
	switch (*data) {
	  case RSTENABLE  : kbreset = ENABLE;  return D_SUCCESS;
	  case RSTDISABLE : kbreset = DISABLE; return D_SUCCESS;
	  default:                             return D_INVALID_OPERATION;
	}

      case KBFLSH:
	tty_flush(tp,D_READ| D_WRITE);
	return D_SUCCESS ;

      case KBCLICK:
      {
	/*
	 * argument *data is an index number indicating keyclick
	 * buzzer frequency. If it is zero (or invliad), keyclick
	 * is turned off.
	 * The index number is offset by one from the required codes
	 * to be sent to the keyboard, so we can conveninetly subtract
	 * one from the argument and use that as our frequency index
	 * to the BUZZ macro.
	 */
	int i = (*data) - 1;
	if (i < BUZZER_TONE_6000Hz || i > BUZZER_TONE__100Hz)
	    do_keyclick = 0;
	else {
	    do_keyclick = 1;
	    keyclick_buzzer_code = BUZZ(BUZZER_LENGTH__40msec, i);
	}
	return D_SUCCESS;
      }

      case KBBELL:
	/* send the raw code to the keyboard */
	keyboard_control(0x40|(*data & 0x1f));
	return D_SUCCESS;
    }
    printf("<keyboard driver: unsupported setstatus flavor %x>\n", flavor);
    return D_INVALID_OPERATION;
}


static void set_encoding_table(void)
{
    if (kb_state.alt)
	current_encoding_table = tblspcl;     /* special encoded table */
    else if (kb_state.ctrl)
	current_encoding_table = tblctrl;     /* control encoded table */
     else if (kb_state.caps) {
	if(kb_state.leftshift || kb_state.rightshift)
	    current_encoding_table = tblcpsf; /* caps+shift */
	else
	    current_encoding_table = tblcaps; /* caps lock  */
    } else if (kb_state.leftshift || kb_state.rightshift)
	current_encoding_table = tblsift;     /* shift encoded table */
    else
	current_encoding_table = tblnoml;     /* normal encoded table */
}

static inline void do_semiencoded(unsigned keycode, int shift)
{
    if (!kb_state.allow_repeat)
	return;

    if (!shift)
    {
        if (release(keycode))
            remove_from_keydown_list(keyonly(keycode));
        else if (!add_to_keydown_list(keyonly(keycode)))
            return; /* too many keys down simultaneously. */
    }

    if (chars_in_keydown_list == 0)
        return;
    keycode = keydown_list[chars_in_keydown_list - 1];

    /*
     * If there's only one character in the keydown list,
     * we want to cause it to repeat
     */
    if (chars_in_keydown_list == 1)
    {
	repeat_buffer[0] = keycode;
        repeat_buffer_len = 1;
        set_dorepeat();
    }
}

static unsigned do_fullencoded(unsigned keycode, unsigned char *buf, int shift)
{
    unsigned repkeycode;
    inline unsigned get_translation(unsigned keycode, unsigned char *buf)
    {
	unsigned flags = current_encoding_table[keycode];
	unsigned mode;
	unsigned i;

	if ((flags & OK) == 0)
	    return 0; /* not a valid character */

	if ((flags & ESCLINK) == 0) {
	    buf[0] = flags & 0xff; /* lower byte of flags is ASCII to return */
	    return 1;
	}

	/* must get the expansion */
	mode = t_mode[flags & 0x0f];
	for (i = 0; i < EXPANSIONS; i++) {
	    if (encoded_expansions[i].keycode == keycode &&
		encoded_expansions[i].mode == mode)
	    {
		unsigned count;
		unsigned char *ptr = encoded_expansions[i].expansion;
		/* found the proper expansion */
		for (count = 0;
		     count < MAX_EXPANSION_LEN && *ptr;
		     count++, buf++, ptr++)
			*buf = *ptr;
		return count;
	    }
	}
	return 0;	/* couldn't find any translation */
    }

    /* If this is a shift key, we must set the encoding table to use.  */
    if (shift)
	set_encoding_table();

    if (!kb_state.allow_repeat)
    {
	if (release(keycode))
	    return 0;
	else
	    return get_translation(keyonly(keycode), buf);
    }

    if (!shift)
    {
	if (release(keycode))
	    remove_from_keydown_list(keyonly(keycode));
	else if (!add_to_keydown_list(keyonly(keycode)))
	    return 0; /* too many keys down simultaneously. */
    }

    /*
     * If there are no more characters in the keydown list, we've just
     * removed one (via a key-up).  Otherwise, we've changed shift-state
     * or added a character. If we've added a character we'll return info
     * is the last one in the list.
     *
     * Furthermore, if there is only one in the list, we'll auto-repeat it.
     */
    if (chars_in_keydown_list == 0)
	return 0;
    repkeycode = keydown_list[chars_in_keydown_list - 1];
    /*
     * If there's only one character in the keydown list, we want to cause
     * it to repeat (if it's a reasonable character and repeats)
     */
    if (chars_in_keydown_list == 1 &&
	(current_encoding_table[repkeycode] & (REPEATB|OK)) == (REPEATB|OK))
    {
	repeat_buffer_len = get_translation(repkeycode, repeat_buffer);
	set_dorepeat();
    }

    if (!release(keycode))
	return get_translation(keycode, buf);
    else
	return 0;
}


/*
 * The 8-bit code KEYCODE has been received from the I/O device.
 * The low seven bits indicate the code for the key (see uart.h),
 * while the high bit (keycode & 0x80) indicates the key is hit or released
 * (high bit on means released).
 *
 * The code (or when ENCODED is on, the ASCII) for the key is stuffed
 * in BUF, with the number of bytes actually stuffed returned.
 */
unsigned kbencode(unsigned keycode, unsigned char *buf)
{
    unsigned shift_key; /* is a key that works like a shift */
    /*
     * Although in the case when the encoding state is RAW we don't really
     * need to do all of what we do in the following switch, it's not so
     * much overhead, and allows us to expand what special combinations
     * we can recognize.
     *
     * Here we'll keep track of the state of keys that act like shift keys.
     * Also, we'll look for some special combinations...
     *		ALT+F1 means to return a BREAK code, while
     *		ALT+F2 means to perform a limited keyboard reset.
     */
    shift_key = 1;
    switch (keyonly(keycode))
    {
      case KEYCODE_CTRL:	kb_state.ctrl       = !release(keycode); break;
      case KEYCODE_KANA:	kb_state.kana       = !release(keycode); break;
      case KEYCODE_RIGHT_SHIFT:	kb_state.rightshift = !release(keycode); break;
      case KEYCODE_LEFT_SHIFT:	kb_state.leftshift  = !release(keycode); break;
      case KEYCODE_CAPS:	kb_state.caps       = !release(keycode); break;
      case KEYCODE_ALT: 	kb_state.alt        = !release(keycode); break;
      default:
        if (kb_state.alt && keycode == KEYCODE_F1)
	    return -1; /* break char */
	shift_key = 0;
	break;
    }

    /*
     * Look for some special keyboard states.
     */
    if (kb_state.leftshift && kb_state.rightshift && keycode == KEYCODE_SPACE) {
	  luna_reset_simple_keyboard_state();
          BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE__300Hz);
	  return 0;
    }
	
    if (kb_state.alt && kb_state.leftshift && kb_state.ctrl) switch (keycode) 
    {
	case KEYCODE_F2: /* toggle keyclick */
	  if (do_keyclick = !do_keyclick, do_keyclick)
	      click();
	  return 0;

	case KEYCODE_F3: /* cycle keyclick frequency */
	  keyclick_buzzer_code = (keyclick_buzzer_code & ~0x7) |
		 (((keyclick_buzzer_code & 0x7)+1) & 0x7);
	  click();
	  return 0;
	  
	case KEYCODE_F4: /* simple reset */
 	  luna_reset_simple_keyboard_state();
	  BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE__300Hz);
	  break;

	case KEYCODE_F6: /* full reset back to encoded mode */
 	  luna_reset_simple_keyboard_state();
	  kbsetinit(1);
	  BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE__300Hz);
	  return 0;

	case KEYCODE_F7: /* set to semi-encoded */
 	  luna_reset_simple_keyboard_state();
	  kb_state.encoding_mode = SEMIENCODED;
	  BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE__300Hz);
	  return 0;

	case KEYCODE_F8: /* set to raw */
 	  luna_reset_simple_keyboard_state();
	  kb_state.encoding_mode = KB_RAW;
	  BUZZ(BUZZER_LENGTH_150msec, BUZZER_TONE__300Hz);
	  return 0;

	case KEYCODE_F9: /* turn off both lamps */
	  keyboard_control(KANA_LIGHT_OFF_CODE);
	  BUZZ(BUZZER_LENGTH__40msec, BUZZER_TONE_6000Hz);
	  keyboard_control(CAPS_LIGHT_OFF_CODE);
	  return 0;

#ifdef MACH_KDB
	case KEYCODE_F10: /* gimmeabreak */
	  db_printf("\n[Dropping into kernel debugger "
		    "- type \"cont\" to resume.]\n");
	  luna_reset_simple_keyboard_state();
	  gimmeabreak();
	  break;
#endif
    }

    /*
     * If the encoding state is RAW, just return the keycode as is.
     */
    if (kb_state.encoding_mode == KB_RAW)
    {
	buf[0] = keycode;
	return 1; /* count of codes stuffed in buf[] */
    }

    kb_state.dorepeat = 0; /* we'll later set this on if need be */
    if (kb_state.encoding_mode == ENCODED) {
	unsigned count = do_fullencoded(keycode, buf, shift_key);
	if (count > 0)
	    click();
	return count;
    } else {
        buf[0] = keycode & 0xff;
	do_semiencoded(keycode, shift_key);
	if (!shift_key && !release(keycode))
		click();
	return 1;
    }
}

void keyboard_setencoded(void)
{
    kb_state.encoding_mode = ENCODED;
}
