# Copyright (C) 2009, 2017 Chris Simmonds (chris@2net.co.uk)
#
# If cross-compiling, CC must point to your cross compiler, for example:
# make CC=arm-linux-gnueabihf-gcc

DESTDIR ?= /usr
BINDIR ?= bin
LOCAL_CFLAGS ?= -Wall -g
PROGRAM = nand_check

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAM)

install:
	install -d $(DESTDIR)/$(BINDIR)
	install -m 0755 $(PROGRAM) $(DESTDIR)/$(BINDIR)
