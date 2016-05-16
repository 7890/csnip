CC ?= gcc
PDF_DOC_NAME ?= "lock-free_ringbuffer_rb.h.pdf"

ARCH := $(shell getconf LONG_BIT)

CFLAGS_32 := -D_32_BIT
CFLAGS_64 := -D_64_BIT

CFLAGS := $(CFLAGS_$(ARCH)) -O3 -Wall -Wno-unused-variable -Wno-unused-but-set-variable

#CFLAGS ?= -lpthread -lrt -luuid
#CFLAGS ?= -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM

PREFIX=/usr/local

default: rb

all: rb doc

rb:	rb.h rb_test0.c rb_test1.c rb_test2.c rb_test3.c rb_test4.c rb_shared_reader.c rb_shared_writer.c

	#test compile and link variations

	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DISABLE_MLOCK 	-DRB_DISABLE_RW_MUTEX 	-DRB_DISABLE_SHM 	$(CFLAGS)
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DISABLE_MLOCK 	-DRB_DISABLE_RW_MUTEX 	-lrt -luuid		$(CFLAGS)
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 				-DRB_DISABLE_RW_MUTEX 	-DRB_DISABLE_SHM	$(CFLAGS)
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DEFAULT_USE_SHM 	-lpthread 		-lrt -luuid		$(CFLAGS)
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 				-lpthread 		-lrt -luuid		$(CFLAGS)
	-./rb_test0

	$(CC) -o rb_test1 rb_test1.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM	$(CFLAGS)
	$(CC) -o rb_test2 rb_test2.c -DRB_DISABLE_MLOCK -lrt -luuid -lpthread
	$(CC) -o rb_test3 rb_test3.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM	$(CFLAGS)
	#-DRB_DEFAULT_USE_SHM -lrt -luuid
	$(CC) -o rb_test4 rb_test4.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM	$(CFLAGS)

	$(CC) -o rb_test_interleave rb_test_interleave.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM -lm $(CFLAGS)

	$(CC) -o rb_shared_reader rb_shared_reader.c -lpthread -lrt -luuid	$(CFLAGS)
	$(CC) -o rb_shared_writer rb_shared_writer.c -lpthread -lrt -luuid	$(CFLAGS)

	$(CC) -o rb_cat rb_cat.c -lpthread -lrt -luuid	$(CFLAGS)

	$(CC) -o rb_new rb_new.c -lpthread -lrt -luuid	$(CFLAGS)

	$(CC) -o img_cat img_cat.c -lpthread -lrt -luuid	$(CFLAGS)

	$(CC) -o rb_show_fill rb_show_fill.c -lpthread -lrt -luuid -lm $(CFLAGS)

doc:	rb.h doxyfile img/drawing_ringbuffer_1_path.svg img/drawing_ringbuffer_1_path.eps

	which doxygen || (echo "doxygen not found." && exit 1)
	doxygen ./doxyfile >/dev/null 2>&1 || (echo "html doc could not be created." && exit 1)
	cp img/drawing_ringbuffer_1_path.svg doc/html

	which pdflatex || (echo "pdflatex not found." && exit 1)
	which makeindex || (echo "makeindex not found." && exit 1)
	cp img/drawing_ringbuffer_1_path.eps doc/latex
	(cd doc/latex && make >/dev/null 2>&1 && mv refman.pdf $(PDF_DOC_NAME)) || (echo "pdf doc could not be created." && exit 1)

	@echo "==================================="
	@echo the documentation can be found here:
	@echo doc/html/index.html
	@echo doc/latex/$(PDF_DOC_NAME)

install:
	install -m755 rb_show_fill $(DESTDIR)$(PREFIX)/bin
	install -m755 rb_cat $(DESTDIR)$(PREFIX)/bin
	install -m755 rb_new $(DESTDIR)$(PREFIX)/bin
	install -m755 img_cat $(DESTDIR)$(PREFIX)/bin

uninstall:
	-rm -f $(DESTDIR)$(PREFIX)/bin/rb_show_fill
	-rm -f $(DESTDIR)$(PREFIX)/bin/rb_cat
	-rm -f $(DESTDIR)$(PREFIX)/bin/rb_new
	-rm -f $(DESTDIR)$(PREFIX)/bin/img_cat

clean:
	-rm -f rb_test0
	-rm -f rb_test1
	-rm -f rb_test2
	-rm -f rb_test3
	-rm -f rb_test4
	-rm -f rb_test_interleave
	-rm -f rb_shared_reader
	-rm -f rb_shared_writer
	-rm -f rb_show_fill
	-rm -f rb_cat
	-rm -f rb_new
	-rm -f img_cat
	-rm -rf doc
