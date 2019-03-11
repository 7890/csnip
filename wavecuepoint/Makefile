INSTALLDIR=/usr/local/bin

default: wavecuepoint

wavecuepoint:	wavecuepoint.c
	$(CC) -o wavecuepoint wavecuepoint.c

clean:
	-rm wavecuepoint

install:
	install -m755 wavecuepoint $(INSTALLDIR)/

uninstall:
	-rm $(INSTALLDIR)/wavecuepoint
