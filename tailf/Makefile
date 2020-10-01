
INSTALLDIR=/usr/local/bin
CC=cc

default: tailf

tailf:	tailf.c
	$(CC) -Wall -O2 -o tailf tailf.c

clean:
	-rm tailf

install:
	install -m755 tailf $(INSTALLDIR)/

uninstall:
	-rm $(INSTALLDIR)/tailf
