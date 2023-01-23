#
# Mach Operating System
# Copyright (c) 1992, 1991 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
#
# HISTORY
# $Log:	Makefile,v $
# Revision 2.3  93/08/10  15:56:42  mrt
# 	DEBUG -> LABEL_DEBUG
# 	[93/08/02            rvb]
# 	Enable Debugging printouts.
# 	[93/07/09  15:22:09  rvb]
# 
# Revision 2.2  92/04/04  11:33:46  rpd
# 	Fabricated for 3.0
# 	[92/03/30            mg32]
# 
#

DEBUG=-DLABEL_DEBUG
CFLAGS	= -O
LIBS= -lc

# start.o should be first, table.o should be second
OBJS = start.o table.o boot2.o boot.o asm.o bios.o io.o disk.o sys.o

all: boot.sed $(OBJS)
	$(LD) -N -T 0 -o boot $(OBJS) $(LIBS)
	@strip boot
	@./rmaouthdr boot boot.tmp
	@mv -f boot.tmp boot
	@ls -l boot

clean:
	/bin/rm -f *.o *.d boot

.SUFFIXES: .s .c .o

.c.o:
	@echo $(CC) -c $(CFLAGS) $(DEBUG) $(INC) $*.c
	-@trap "/bin/rm -f $*.i $*.s; exit 0" 0 1 2 3 10 15; \
	$(CC) -MD $(CFLAGS) $(DEBUG) $(INC) -S $<;	\
	if [ $$? != 0 ]; then :; else \
		sed -f boot.sed $*.s > $*.i;	\
		$(AS) $*.i -o $@; \
	fi

.s.o:
	@echo $(AS) -o $*.o $<
	-@trap "/bin/rm -f $*.i X$*.c; exit 0" 0 1 2 3 10 15; \
	/bin/rm -f X$*.c;	\
	ln $*.s X$*.c;	\
	$(CC) -E $(CFLAGS) $(DEBUG) X$*.c > $*.i;	\
	if [ $$? != 0 ]; then :;	\
	else \
		$(AS) $*.i -o $@;	\
	fi

