
INSTALLDIR=/usr/local/bin

default: com

com:	com.c
	cc -o com com.c

clean:
	-rm com

install:
	install -m755 com $(INSTALLDIR)/

uninstall:
	-rm $(INSTALLDIR)/com
