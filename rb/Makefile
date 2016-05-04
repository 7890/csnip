CC ?= gcc
PDF_DOC_NAME ?= "lock-free_ringbuffer_rb.h.pdf"

#CFLAGS ?= -lpthread -lrt -luuid
#CFLAGS ?= -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM

default: rb

all: rb doc

rb:	rb.h rb_test0.c rb_test1.c rb_test2.c rb_test3.c rb_test4.c rb_shared_reader.c rb_shared_writer.c

	#test compile and link variations

	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DISABLE_MLOCK 	-DRB_DISABLE_RW_MUTEX 	-DRB_DISABLE_SHM
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DISABLE_MLOCK 	-DRB_DISABLE_RW_MUTEX 	-lrt -luuid
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 				-DRB_DISABLE_RW_MUTEX 	-DRB_DISABLE_SHM
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 	-DRB_DEFAULT_USE_SHM 	-lpthread 		-lrt -luuid
	-./rb_test0
	$(CC) -o rb_test0 		rb_test0.c 				-lpthread 		-lrt -luuid
	-./rb_test0

	$(CC) -o rb_test1 rb_test1.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM
	$(CC) -o rb_test2 rb_test2.c -DRB_DISABLE_MLOCK -lrt -luuid -lpthread
	$(CC) -o rb_test3 rb_test3.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM
	#-DRB_DEFAULT_USE_SHM -lrt -luuid
	$(CC) -o rb_test4 rb_test4.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM

	$(CC) -o rb_test_interleave rb_test_interleave.c -DRB_DISABLE_MLOCK -DRB_DISABLE_RW_MUTEX -DRB_DISABLE_SHM -lm

	$(CC) -o rb_shared_reader rb_shared_reader.c -lpthread -lrt -luuid
	$(CC) -o rb_shared_writer rb_shared_writer.c -lpthread -lrt -luuid

	$(CC) -o rb_cat rb_cat.c -lpthread -lrt -luuid

	$(CC) -o rb_new rb_new.c -lpthread -lrt -luuid

	$(CC) -o rb_show_fill rb_show_fill.c -lpthread -lrt -luuid -lm

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
	-rm -rf doc
