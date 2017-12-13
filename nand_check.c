/*
 *  nand_check.c
 *
 *  Copyright (C) 2009, 2017 Chris Simmonds (chris@2net.co.uk)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Read a flash partition and report on block usage: for each eraseblock print
 *    B        Bad block
 *    .        Empty
 *    -        Partially filled
 *    =        Full, no summary node
 *    S        Full, summary node
 *
 * This program is based on jffs2dump by Thomas Gleixner
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <asm/types.h>
#include <mtd/mtd-user.h>

/* taken from linux/jffs2.h */
#define JFFS2_SUM_MAGIC	0x02851885

static unsigned long start_addr;	/* start address */

static void print_block(unsigned long block_num,
			int bad, int sum, int erase_block_size, int good_data)
{
	if (bad)
		printf("B");
	else {
		if (good_data == 0) {
			printf(".");
		} else if (good_data < (erase_block_size / 2)) {
			if (sum)
				printf("s");
			else
				printf("-");
		} else {
			if (sum)
				printf("S");
			else
				printf("=");
		}
	}
	if (block_num % 80 == 79)
		printf("\n");
}

int main(int argc, char **argv)
{
	unsigned long ofs, end_addr = 0;
	int ret, fd, bs;
	mtd_info_t meminfo;
	loff_t offset;
	int i;
	unsigned char *block_buf;
	int bad_block;
	int summary_info;

	/* Open MTD device */
	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror("open flash");
		exit(1);
	}

	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		perror("MEMGETINFO");
		close(fd);
		exit(1);
	}

	printf("Flash type is %d\n", meminfo.type);

	/* Make sure device page sizes are valid */
	if (!(meminfo.oobsize == 128) &&
	    !(meminfo.oobsize == 64) &&
	    !(meminfo.oobsize == 32) &&
	    !(meminfo.oobsize == 16) && !(meminfo.oobsize == 8)) {
		fprintf(stderr, "Unknown type of flash (not normal NAND)\n");
		close(fd);
		exit(1);
	}

	/* Read the real oob length */
	end_addr = meminfo.size;
	bs = meminfo.writesize;

	printf
	    ("B Bad block; . Empty; - Partially filled; = Full; S has a JFFS2 summary node\n");
	printf("Block size %u, page size %u, OOB size %u\n", meminfo.erasesize,
	       bs, meminfo.oobsize);
	printf("%lu bytes, %lu blocks\n", end_addr,
	       end_addr / meminfo.erasesize);

	block_buf = malloc(meminfo.erasesize);
	for (ofs = start_addr; ofs < end_addr; ofs += meminfo.erasesize) {
		/* Read the next erase block */
		if (read(fd, block_buf, meminfo.erasesize) != meminfo.erasesize) {
			perror("read block");
			printf(" ofs=%lu block %lu errno %d\n", ofs,
			       ofs / meminfo.erasesize, errno);
		}

		offset = ofs;
		ret = ioctl(fd, MEMGETBADBLOCK, &offset);
		if (ret > 0) {
			bad_block = 1;
		} else if (ret < 0) {
			perror("MEMGETBADBLOCK");
			printf("ofs=%lx\n", ofs);
			return 1;
		} else {
			bad_block = 0;
			/* See how much of the block contains "data", by
			   scanning backwards to find the first non 0xff byte */
			for (i = (meminfo.erasesize - 1); i >= 0; i--) {
				if (block_buf[i] != 0xff)
					break;
			}
			/* See if there is a summary node at the end of the
			   block. Here, we just check for the summary marker
			   in the last 4 bytes, we don't check that the block
			   it points to is  valid */
			if (*(unsigned long *)
			    (block_buf + meminfo.erasesize -
			     sizeof(unsigned long)) == JFFS2_SUM_MAGIC)
				summary_info = 1;
			else
				summary_info = 0;
		}
		print_block(ofs / meminfo.erasesize, bad_block, summary_info,
			    meminfo.erasesize, i + 1);
	}
	printf("\n");
	free(block_buf);
	return 0;
}
