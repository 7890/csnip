CC ?= cc
INSTALLDIR ?= /usr/local/bin
FLAGS ?= -Wall -O2

default: tailf

tailf:	tailf.c
	$(CC) $(FLAGS) -o tailf tailf.c

clean:
	-rm tailf

install:
	install -m755 tailf $(INSTALLDIR)/

uninstall:
	-rm $(INSTALLDIR)/tailf
