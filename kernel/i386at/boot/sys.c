/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 * $Log:	sys.c,v $
 * Revision 2.4  93/08/10  15:57:05  mrt
 * 	DEBUG -> LABEL_DEBUG
 * 	[93/08/02            rvb]
 * 	Under #ifdef DEBUG allow for FILE, LABEL and parition printout
 * 	[93/07/09  15:17:39  rvb]
 * 
 * Revision 2.3  93/05/10  17:47:12  rvb
 * 	bsdss has a different fs layout
 * 	[93/05/04  17:34:21  rvb]
 * 
 * Revision 2.2  92/04/04  11:36:34  rpd
 * 	Fabricated from 3.0 bootstrap and scratch.
 * 	[92/03/30            mg32]
 * 
 */

#include <boot.h>
#include <sys/reboot.h>

#ifdef	__386BSD__
#include <ufs/dir.h>
#else	/* __386BSD__ */
#include <sys/dir.h>
#endif	/* __386BSD__ */

#define BUFSIZE 8192

char buf[BUFSIZE], fsbuf[SBSIZE], iobuf[MAXBSIZE];

int xread(addr, size)
	char		* addr;
	int		size;
{
	int count = BUFSIZE;
	while (size > 0) {
		if (BUFSIZE > size)
			count = size;
		read(buf, count);
		pcpy(buf, addr, count);
		size -= count;
		addr += count;
	}
}

read(buffer, count)
	int count;
	char *buffer;
{
	int logno, off, size;

	while (count) {
		off = blkoff(fs, poff);
		logno = lblkno(fs, poff);
		cnt = size = blksize(fs, &inode, logno);
		bnum = fsbtodb(fs, block_map(logno)) + boff;
		if (!off && size <= count) {
			iodest = buffer;
			devread();
		} else {
			iodest = iobuf;
			size -= off;
			if (size > count)
				size = count;
			devread();
			bcopy(iodest+off,buffer,size);
		}
		buffer += size;
		count -= size;
		poff += size;
	}
}

find(path)
     char *path;
{
	char *rest, ch;
	int block, off, loc, ino = ROOTINO;
	struct direct *dp;
loop:	iodest = iobuf;
	cnt = fs->fs_bsize;
	bnum = fsbtodb(fs,itod(fs,ino)) + boff;
	devread();
#ifdef	__386BSD__
	bcopy(&((struct dinode *)iodest)[ino % fs->fs_inopb],
	      &inode.i_din,
	      sizeof (struct dinode));
#else	/* __386BSD__ */
	bcopy(&((struct dinode *)iodest)[ino % fs->fs_inopb].di_ic,
	      &inode.i_ic,
	      sizeof (struct dinode));
#endif	/* __386BSD__ */
	if (!*path)
		return 1;
	while (*path == '/')
		path++;
	if (!inode.i_size || ((inode.i_mode&IFMT) != IFDIR))
		return 0;
	for (rest = path; (ch = *rest) && ch != '/'; rest++) ;
	*rest = 0;
	loc = 0;
	do {
		if (loc >= inode.i_size)
			return 0;
		if (!(off = blkoff(fs, loc))) {
			block = lblkno(fs, loc);
			cnt = blksize(fs, &inode, block);
			bnum = fsbtodb(fs, block_map(block)) + boff;
			iodest = iobuf;
			devread();
		}
		dp = (struct direct *)(iodest + off);
		loc += dp->d_reclen;
#ifdef	LABEL_DEBUG
		if (dp->d_ino && iflag & I_FILE)
			printf("%s, ", dp->d_name);
#endif	/* LABEL_DEBUG */
	} while (!dp->d_ino || strcmp(path, dp->d_name));
	ino = dp->d_ino;
	*(path = rest) = ch;
	goto loop;
}

char mapbuf[BUFSIZE];
int mapblock = 0;

block_map(file_block)
     int file_block;
{
	if (file_block < NDADDR)
		return(inode.i_db[file_block]);
	if ((bnum=fsbtodb(fs, inode.i_ib[0])+boff) != mapblock) {
		iodest = mapbuf;
		cnt = fs->fs_bsize;
		devread();
		mapblock = bnum;
	}
	return (((int *)mapbuf)[(file_block - NDADDR) % NINDIR(fs)]);
}

openrd()
{
	char **devp, *cp = name;
	while (*cp && *cp!='(')
		cp++;
	if (!*cp)
		cp = name;
	else {
		if (cp++ != name) {
			for (devp = devs; *devp; devp++)
				if (name[0] == (*devp)[0] &&
				    name[1] == (*devp)[1])
					break;
			if (!*devp) {
				printf("Unknown device\n");
				return 1;
			}
			maj = devp-devs;
		}
		if (*cp >= '0' && *cp <= '9')
			if ((unit = *cp++ - '0') > 1) {
				printf("Bad unit\n");
				return 1;
			}
		if (!*cp || (*cp == ',' && !*++cp))
			return 1;
		if (*cp >= 'a' && *cp <= 'p')
			part = *cp++ - 'a';
		while (*cp && *cp++!=')') ;
		if (!*cp)
			return 1;
	}
	if (maj == 2) {
		printf("Wangtek unsupported\n");
		return 1;
	}
	inode.i_dev = (maj == 1);
	if (devopen())
		return 1;
	iodest = (char *)(fs = (struct fs *)fsbuf);
	cnt = SBSIZE;
	bnum = SBLOCK + boff;
	devread();
	if (!find(cp))
		return 1;
	poff = 0;
	name = cp;
	return 0;
}
