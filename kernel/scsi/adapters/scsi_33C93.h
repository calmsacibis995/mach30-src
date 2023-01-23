/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log:	scsi_33C93.h,v $
 * Revision 2.3  91/08/28  11:16:35  jsb
 * 	Name prefix change, revised list of spec documents.
 * 	[91/08/27  17:11:47  af]
 * 
 * Revision 2.2  91/08/24  12:24:51  af
 * 	Created, from the Western Digital specs:
 *	"WD33C93A SCSI Bus Interface Controller"
 *	Publication S0228 12/90
 *	Western Digital Corporation, Irvine, CA.
 *
 *	and from the Amd specs:
 * 	"Am33C93A Enhanced SCSI-Bus Interface Controller"
 * 	Publication #11853 Rev.A Amendment/0 Issue Date: June 1989
 * 	5/16/1989, Advanced Micro Devices Inc.,  Sunnyvale, CA.
 * 	[91/08/01            af]
 * 
 */
/*
 *	File: scsi_33C93.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/91
 *
 *	Defines for the WD/AMD 33C93 SBIC (SCSI Bus Interface Controller)
 */

/*
 * Register map, w mux addressing
 */

typedef struct {

	volatile unsigned char	sbic_myid;	/* rw: My SCSI id */
#define			sbic_cdbsize sbic_myid	/* w : size of CDB */

	volatile unsigned char	sbic_control;	/* rw: Control register */

	volatile unsigned char	sbic_timeo;	/* rw: Timeout period */

	volatile unsigned char	sbic_cdb1;	/* rw: CDB, 1st byte */
#define			sbic_tsecs sbic_cdb1	/* rw: Xlate: nsectors */

	volatile unsigned char	sbic_cdb2;	/* rw: CDB, 2nd byte */
#define			sbic_theads sbic_cdb2	/* rw: Xlate: nheads */

	volatile unsigned char	sbic_cdb3;	/* rw: CDB, 3rd byte */
#define			sbic_tcyl_hi sbic_cdb3	/* rw: Xlate: ncyls, hi */

	volatile unsigned char	sbic_cdb4;	/* rw: CDB, 4th byte */
#define			sbic_tcyl_lo sbic_cdb4	/* rw: Xlate: ncyls, lo */

	volatile unsigned char	sbic_cdb5;	/* rw: CDB, 5th byte */
#define			sbic_addr_hi sbic_cdb5	/* rw: Xlate: address, hi */

	volatile unsigned char	sbic_cdb6;	/* rw: CDB, 6th byte */
#define			sbic_addr_2 sbic_cdb6	/* rw: Xlate: address */

	volatile unsigned char	sbic_cdb7;	/* rw: CDB, 7th byte */
#define			sbic_addr_3 sbic_cdb7	/* rw: Xlate: address */

	volatile unsigned char	sbic_cdb8;	/* rw: CDB, 8th byte */
#define			sbic_addr_lo sbic_cdb8	/* rw: Xlate: address, lo */

	volatile unsigned char	sbic_cdb9;	/* rw: CDB, 9th byte */
#define			sbic_secno sbic_cdb9	/* rw: Xlate: sector no */

	volatile unsigned char	sbic_cdb10;	/* rw: CDB, 10th byte */
#define			sbic_headno sbic_cdb10	/* rw: Xlate: head no */

	volatile unsigned char	sbic_cdb11;	/* rw: CDB, 11th byte */
#define			sbic_cylno_hi sbic_cdb11/* rw: Xlate: cyl no, hi */

	volatile unsigned char	sbic_cdb12;	/* rw: CDB, 12th byte */
#define			sbic_cylno_lo sbic_cdb12/* rw: Xlate: cyl no, lo */

	volatile unsigned char	sbic_tlun;	/* rw: Target LUN */

	volatile unsigned char	sbic_cmd_phase;	/* rw: Command phase */

	volatile unsigned char	sbic_syn;	/* rw: Synch xfer params */

	volatile unsigned char	sbic_count_hi;	/* rw: Xfer count, hi */

	volatile unsigned char	sbic_count_med;	/* rw: Xfer count, med */

	volatile unsigned char	sbic_count_lo;	/* rw: Xfer count, lo */

	volatile unsigned char	sbic_selid;	/* rw: Target ID (select) */

	volatile unsigned char	sbic_rselid;	/* rw: Target ID (reselect) */

	volatile unsigned char	sbic_csr;	/* r : Status register */

	volatile unsigned char	sbic_cmd;	/* rw: Command register */

	volatile unsigned char	sbic_data;	/* rw: FIFO top */

	char u0;				/* unused, padding */
	char u1;				/* unused, padding */
	char u2;				/* unused, padding */
	char u3;				/* unused, padding */
	char u4;				/* unused, padding */

	volatile unsigned char	sbic_asr;	/* r : Aux Status Register */

} sbic_mux_regmap_t;

/*
 * Register map, non mux (indirect) addressing
 */
typedef struct {
	volatile unsigned char	sbic_asr;	/* r : Aux Status Register */
#define			sbic_address sbic_asr	/* w : desired register no */

	volatile unsigned char	sbic_value;	/* rw: register value */
} sbic_ind_regmap_t;

#define	sbic_read_reg(regs,regno,val)	{ \
		(regs)->sbic_address = (regno);	\
		wbflush();			\
		(val) = (regs)->sbic_value;	\
	}

#define	sbic_write_reg(regs,regno,val)	{ \
		(regs)->sbic_address = (regno);	\
		wbflush();			\
		(regs)->sbic_value = (val);	\
	}

#define	SBIC_myid	0
#define	SBIC_cdbsize	0
#define	SBIC_control	1
#define	SBIC_timeo	2
#define	SBIC_cdb1	3
#define	SBIC_tsecs	3
#define	SBIC_cdb2	4
#define	SBIC_theads	4
#define	SBIC_cdb3	5
#define	SBIC_tcyl_hi	5
#define	SBIC_cdb4	6
#define	SBIC_tcyl_lo	6
#define	SBIC_cdb5	7
#define	SBIC_addr_hi	7
#define	SBIC_cdb6	8
#define	SBIC_addr_2	8
#define	SBIC_cdb7	9
#define	SBIC_addr_3	9
#define	SBIC_cdb8	10
#define	SBIC_addr_lo	10
#define	SBIC_cdb9	11
#define	SBIC_secno	11
#define	SBIC_cdb10	12
#define	SBIC_headno	12
#define	SBIC_cdb11	13
#define	SBIC_cylno_hi	13
#define	SBIC_cdb12	14
#define	SBIC_cylno_lo	14
#define	SBIC_tlun	15
#define	SBIC_cmd_phase	16
#define	SBIC_syn	17
#define	SBIC_count_hi	18
#define	SBIC_count_med	19
#define	SBIC_count_lo	20
#define	SBIC_selid	21
#define	SBIC_rselid	22
#define	SBIC_csr	23
#define	SBIC_cmd	24
#define	SBIC_data	25
/* sbic_asr is addressed directly */

/*
 *	Register defines
 */

/*
 * Auxiliary Status Register
 */

#define SBIC_ASR_INT		0x80	/* Interrupt pending */
#define SBIC_ASR_LCI		0x40	/* Last command ignored */
#define SBIC_ASR_BSY		0x20	/* Busy, only cmd/data/asr readable */
#define SBIC_ASR_CIP		0x10	/* Busy, cmd unavail also */
#define SBIC_ASR_xxx		0x0c
#define SBIC_ASR_PE		0x02	/* Parity error (even) */
#define SBIC_ASR_DBR		0x01	/* Data Buffer Ready */

/*
 * My ID register, and/or CDB Size
 */

#define SBIC_ID_FS_8_10		0x00	/* Input clock is  8-10 Mhz */
					/* 11 Mhz is invalid */
#define SBIC_ID_FS_12_15	0x40	/* Input clock is 12-15 Mhz */
#define SBIC_ID_FS_16_20	0x80	/* Input clock is 16-20 Mhz */
#define SBIC_ID_EHP		0x10	/* Enable host parity */
#define SBIC_ID_EAF		0x08	/* Enable Advanced Features */
#define SBIC_ID_MASK		0x07
#define SBIC_ID_CBDSIZE_MASK	0x0f	/* if unk SCSI cmd group */

/*
 * Control register
 */

#define SBIC_CTL_DMA		0x80	/* Single byte dma */
#define SBIC_CTL_DBA_DMA	0x40	/* direct buffer acces (bus master)*/
#define SBIC_CTL_BURST_DMA	0x20	/* continuous mode (8237) */
#define SBIC_CTL_NO_DMA		0x00	/* Programmed I/O */
#define SBIC_CTL_HHP		0x10	/* Halt on host parity error */
#define SBIC_CTL_EDI		0x08	/* Ending disconnect interrupt */
#define SBIC_CTL_IDI		0x04	/* Intermediate disconnect interrupt*/
#define SBIC_CTL_HA		0x02	/* Halt on ATN */
#define SBIC_CTL_HSP		0x01	/* Halt on SCSI parity error */

/*
 * Timeout period register
 * [val in msecs, input clk in Mhz]
 */

#define	SBIC_TIMEOUT(val,clk)	((((val)*(clk))/80)+1)

/*
 * CDBn registers, note that
 *	cdb11 is used for status byte in target mode (send-status-and-cc)
 *	cdb12 sez if linked command complete, and w/flag if so
 */

/*
 * Target LUN register
 * [holds target status when select-and-xfer]
 */

#define	SBIC_TLUN_VALID		0x80	/* did we receive an Identify msg */
#define	SBIC_TLUN_DOK		0x40	/* Disconnect OK */
#define	SBIC_TLUN_xxx		0x38
#define	SBIC_TLUN_MASK		0x07

/*
 * Command Phase register
 */

#define	SBIC_CPH_MASK		0x7f	/* values/restarts are cmd specific */
#define	SBIC_CPH(p)		((p)&SBIC_CPH_MASK)

/*
 * FIFO register
 */

#define SBIC_FIFO_DEEP		12

/*
 * Synchronous xfer register
 */

#define	SBIC_SYN_OFF_MASK	0x0f
#define	SBIC_SYN_MAX_OFFSET	(SBIC_FIFO_DEEP-1)
#define	SBIC_SYN_PER_MASK	0x70
#define	SBIC_SYN_MIN_PERIOD	2		/* upto 8, encoded as 0 */

#define	SBIC_SYN(o,p)		(((o)&SBIC_SYN_OFF_MASK)|(((p)<<4)&SBIC_SYN_PER_MASK))

/*
 * Transfer count register
 * optimal access macros depend on addressing
 */

/*
 * Destination ID (selid) register
 */

#define	SBIC_SID_SCC		0x80	/* Select command chaining (tgt) */
#define	SBIC_SID_DPD		0x40	/* Data phase direction (inittor) */
#	define	SBIC_SID_FROM_SCSI		0x40
#	define	SBIC_SID_TO_SCSI		0x00
#define	SBIC_SID_xxx		0x38
#define	SBIC_SID_IDMASK		0x07

/*
 * Source ID (rselid) register
 */

#define	SBIC_RID_ER		0x80	/* Enable reselection */
#define	SBIC_RID_ES		0x40	/* Enable selection */
#define	SBIC_RID_DSP		0x20	/* Disable select parity */
#define	SBIC_RID_SIV		0x08	/* Source ID valid */
#define	SBIC_RID_MASK		0x07

/*
 * Status register
 */

#define	SBIC_CSR_CAUSE		0xf0
#	define	SBIC_CSR_RESET		0x00	/* chip was reset */
#	define	SBIC_CSR_CMD_DONE	0x10	/* cmd completed */
#	define	SBIC_CSR_CMD_STOPPED	0x20	/* interrupted or abrted*/
#	define	SBIC_CSR_CMD_ERR	0x40	/* end with error */
#	define	SBIC_CSR_BUS_SERVICE	0x80	/* REQ pending on the bus */

#define	SBIC_CSR_QUALIFIER	0x0f

	/* Reset State Interrupts */
#	define	SBIC_CSR_RESET		0x00	/* reset w/advanced features*/
#	define	SBIC_CSR_RESET_AM	0x01	/* reset w/advanced features*/

	/* Successful Completion Interrupts */
#	define	SBIC_CSR_TARGET		0x10	/* reselect complete */
#	define	SBIC_CSR_INITIATOR	0x11	/* select complete */
#	define	SBIC_CSR_WO_ATN		0x13	/* tgt mode completion */
#	define	SBIC_CSR_W_ATN		0x14	/* ditto */
#	define	SBIC_CSR_XLATED		0x15	/* translate address cmd */
#	define	SBIC_CSR_S_XFERRED	0x16	/* initiator mode completion*/
#	define	SBIC_CSR_XFERRED		0x18	/* phase in low bits */

	/* Paused or Aborted Interrupts */
#	define	SBIC_CSR_MSGIN_W_ACK	0x20	/* (I) msgin, ACK asserted*/
#	define	SBIC_CSR_SDP		0x21	/* (I) SDP msg received */
#	define	SBIC_CSR_SEL_ABRT	0x22	/* sel/resel aborted */
#	define	SBIC_CSR_XFR_PAUSED	0x23	/* (T) no ATN */
#	define	SBIC_CSR_XFR_PAUSED_ATN	0x24	/* (T) ATN is asserted */
#	define	SBIC_CSR_RSLT_AM	0x27	/* (I) lost selection (AM) */
#	define	SBIC_CSR_MIS		0x28	/* (I) xfer aborted, ph mis */

	/* Terminated Interrupts */
#	define	SBIC_CSR_CMD_INVALID	0x40
#	define	SBIC_CSR_DISC		0x41	/* (I) tgt disconnected */
#	define	SBIC_CSR_SEL_TIMEO	0x42
#	define	SBIC_CSR_PE		0x43	/* parity error */
#	define	SBIC_CSR_PE_ATN		0x44	/* ditto, ATN is asserted */
#	define	SBIC_CSR_XLATE_TOOBIG	0x45
#	define	SBIC_CSR_RSLT_NOAM	0x46	/* (I) lost sel, no AM mode */
#	define	SBIC_CSR_BAD_STATUS	0x47	/* status byte was nok */
#	define	SBIC_CSR_MIS_1		0x48	/* ph mis, see low bits */

	/* Service Required Interrupts */
#	define	SBIC_CSR_RSLT_NI	0x80	/* reselected, no ify msg */
#	define	SBIC_CSR_RSLT_IFY	0x81	/* ditto, AM mode, got ify */
#	define	SBIC_CSR_SLT		0x82	/* selected, no ATN */
#	define	SBIC_CSR_SLT_ATN	0x83	/* selected with ATN */
#	define	SBIC_CSR_ATN		0x84	/* (T) ATN asserted */
#	define	SBIC_CSR_DISC_1		0x85	/* (I) bus is free */
#	define	SBIC_CSR_UNK_GROUP	0x87	/* strange CDB1 */
#	define	SBIC_CSR_MIS_2		0x88	/* (I) ph mis, see low bits */

#define	SBIC_PHASE(csr)		SCSI_PHASE(csr)

/*
 * Command register (command codes)
 */

#define SBIC_CMD_SBT		0x80	/* Single byte xfer qualifier */
#define	SBIC_CMD_MASK		0x7f

					/* Miscellaneous */
#define SBIC_CMD_RESET		0x00		/* (DTI) lev I */
#define SBIC_CMD_ABORT		0x01		/* (DTI) lev I */
#define SBIC_CMD_DISC		0x04		/* ( TI) lev I */
#define SBIC_CMD_SSCC		0x0d		/* ( TI) lev I */
#define SBIC_CMD_SET_IDI	0x0f		/* (DTI) lev I */
#define SBIC_CMD_XLATE		0x18		/* (DT ) lev II */

					/* Initiator state */
#define SBIC_CMD_SET_ATN	0x02		/* (  I) lev I */
#define SBIC_CMD_CLR_ACK	0x03		/* (  I) lev I */
#define SBIC_CMD_XFER_INFO	0x20		/* (  I) lev II */

					/* Target state */
#define SBIC_CMD_SND_DISC	0x0e		/* ( T ) lev II */
#define SBIC_CMD_RCV_CMD	0x10		/* ( T ) lev II */
#define SBIC_CMD_RCV_DATA	0x11		/* ( T ) lev II */
#define SBIC_CMD_RCV_MSG_OUT	0x12		/* ( T ) lev II */
#define SBIC_CMD_RCV		0x13		/* ( T ) lev II */
#define SBIC_CMD_SND_STATUS	0x14		/* ( T ) lev II */
#define SBIC_CMD_SND_DATA	0x15		/* ( T ) lev II */
#define SBIC_CMD_SND_MSG_IN	0x16		/* ( T ) lev II */
#define SBIC_CMD_SND		0x17		/* ( T ) lev II */

					/* Disconnected state */
#define SBIC_CMD_RESELECT	0x05		/* (D  ) lev II */
#define SBIC_CMD_SEL_ATN	0x06		/* (D  ) lev II */
#define SBIC_CMD_SEL		0x07		/* (D  ) lev II */
#define SBIC_CMD_SEL_ATN_XFER	0x08		/* (D I) lev II */
#define SBIC_CMD_SEL_XFER	0x09		/* (D I) lev II */
#define SBIC_CMD_RESELECT_RECV	0x0a		/* (DT ) lev II */
#define SBIC_CMD_RESELECT_SEND	0x0b		/* (DT ) lev II */
#define SBIC_CMD_WAIT_SEL_RECV	0x0c		/* (DT ) lev II */


/* approximate, but we won't do SBT on selects */
#define	sbic_isa_select(cmd)	(((cmd)>0x5)&&((cmd)<0xa))

