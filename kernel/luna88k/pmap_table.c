/* 
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
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
 *
 * HISTORY
 * $Log:	pmap_table.c,v $
 * Revision 2.4  93/03/09  17:58:31  danner
 * 	replaced BMAP_PALLET2
 * 	[93/03/09            jfriedl]
 * 
 * Revision 2.3  93/01/26  18:03:23  danner
 * 	Fixed up includes.
 * 	[93/01/25            jfriedl]
 * 
 * Revision 2.2  92/08/03  17:44:29  jfriedl
 * 	Created [danner].
 * 	[92/07/24            jfriedl]
 * 
 */

#include <luna88k/board.h>
#include <mach/m88k/vm_types.h>		/* for pmap_table.h */
#include <m88k/pmap_table.h>
#include <mach/vm_prot.h>
#include <m88k/mmu.h>

#define R VM_PROT_READ
#define RW VM_PROT_READ|VM_PROT_WRITE
#define C CACHE_DFL
#define CWT CACHE_WT
#define CN CACHE_INH
#define PAGE M88K_PGBYTES
#define SEG M88K_SGBYTES

static pmap_table_entry board_table[] = {
  { TRI_PORT_RAM  ,	TRI_PORT_RAM,   TRI_PORT_RAM_SPACE, 	RW, 	CN },
  { 0, 	 		VEQR_ADDR,	0/*filled in later*/,	RW, 	C|CACHE_GLOBAL},
  { 0,			0,		0/*filled in late*/,	RW,	C|CACHE_GLOBAL},
  { PROM_ADDR    ,	PROM_ADDR,	PROM_SPACE, 		R, 	C }, 
  { FUSE_ROM_ADDR,	FUSE_ROM_ADDR,	FUSE_ROM_SPACE, 	RW,     CN },
  { NVRAM_ADDR	 ,	NVRAM_ADDR,	NVRAM_SPACE,		RW,	CN },
  { OBIO_PIO0_BASE,	OBIO_PIO0_BASE, PAGE,			RW,     CN },
  { OBIO_PIO1_BASE,	OBIO_PIO1_BASE, PAGE,			RW,     CN },
  { OBIO_SIO	  ,	OBIO_SIO,	PAGE,			RW,     CN },
  { OBIO_TAS	  ,	OBIO_TAS,	PAGE,			RW,     CN },
  { OBIO_CLOCK0	  ,	OBIO_CLOCK0,	PAGE,			RW,     CN },
  { INT_ST_MASK0  ,	INT_ST_MASK0,	PAGE,			RW,     CN },
  { SOFT_INT0  	  ,	SOFT_INT0,	PAGE,			RW,     CN },
  { SOFT_INT_FLAG0,	SOFT_INT_FLAG0,	PAGE,			RW,     CN },
  { RESET_CPU0 	  ,	RESET_CPU0,	PAGE,			RW,     CN },
#if 0
  { EXT_A_ADDR 	  ,	EXT_A_ADDR,	EXT_A_SPACE,		RW,     CN },
  { EXT_B_ADDR 	  ,	EXT_B_ADDR,	EXT_B_SPACE,		RW,     CN },
  { PC_BASE 	  ,	PC_BASE,	PC_SPACE,		RW,     CN },
#endif
  { MROM_ADDR	  ,	MROM_ADDR,	MROM_SPACE,		R,	CN },
  { BMAP_START	  ,	BMAP_START,	BMAP_SPACE,		RW,	CN },
  { BMAP_PALLET0  ,	BMAP_PALLET0,	PAGE,			RW,	CN },
  { BMAP_PALLET1  ,	BMAP_PALLET1,	PAGE,			RW,	CN },
  { BMAP_PALLET2  ,	BMAP_PALLET2,	PAGE,			RW,	CN },
  { BOARD_CHECK_REG,	BOARD_CHECK_REG,PAGE,			RW,	CN },
  { BMAP_CRTC,		BMAP_CRTC,	PAGE,			RW,	CN },
  { SCSI_ADDR,		SCSI_ADDR,	PAGE,			RW,	CN },
  { LANCE_ADDR,		LANCE_ADDR,	PAGE,			RW,	CN },
  { 0,			0,		0xffffffff,		0,	0  },
};


pmap_table_t pmap_table_build(unsigned memory_size)
{
  extern int kernelstart;
  unsigned int i;
  /* fill in the veqr map entry */
  board_table[1].size = memory_size;
  board_table[2].size = (unsigned)&kernelstart;

  board_table[0].size = (board_table[0].size + M88K_SGBYTES-1) & ~(M88K_SGBYTES-1);

  /* round off to nearest segment */
  for (i = 3; board_table[i].size != 0xffffffff; i++)
    if (board_table[i].size>0)
      board_table[i].size = (board_table[i].size + M88K_SGBYTES-1) & ~(M88K_SGBYTES-1);

  return board_table;
}
