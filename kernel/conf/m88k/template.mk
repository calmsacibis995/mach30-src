# 
# Mach Operating System
# Copyright (c) 1993,1992 Carnegie Mellon University
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
# HISTORY
# $Log:	template.mk,v $
# Revision 2.2  93/02/04  16:57:40  mrt
# 	Added "OBJSDEPS=assym.s" so assym.s is available to all.
# 	Removed CC_OPT_LEVEL assignment since it is the same as 
# 	in the MI template.mk -- mrt
# 	[93/01/25            jfriedl]
# 
# Revision 2.12  92/08/03  18:13:36  jfriedl
# 	Removed (now unneeded) special rules. Cleaned up.
# 	Condensed history.
# 	[92/07/24            jfriedl]
# 
# Revision 2.9.3.1  92/05/27  14:32:36  danner
# 	Changes to support new compilation options.
# 

######################################################################
#BEGIN	Machine dependent Makefile fragment for the Omron Luna 88k
######################################################################


MK_LDFLAGS = -N -e start -T 20000 
LDOBJS_PREFIX = ${ORDERED}
OBJSDEPS= assym.s

######################################################################
#END	Machine dependent Makefile fragment for the LUNA-88K
######################################################################
