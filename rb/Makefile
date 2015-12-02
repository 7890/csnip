CC ?= gcc
PDF_DOC_NAME ?= "lock-free_ringbuffer_rb.h.pdf"

CFLAGS ?= -lpthread -lrt -luuid

default: rb

rb:	rb.h rb_test0.c rb_test1.c rb_test2.c

	$(CC) -o rb_test0 rb_test0.c $(CFLAGS)
	$(CC) -o rb_test1 rb_test1.c $(CFLAGS)
	$(CC) -o rb_test2 rb_test2.c $(CFLAGS)

doc:	rb.h doxyfile img/drawing_ringbuffer_1_path.svg img/drawing_ringbuffer_1_path.eps

	which doxygen || (echo "doxygen not found." && exit 1)
	doxygen ./doxyfile >/dev/null 2>&1 || (echo "html doc could not be created." && exit 1)
	cp img/drawing_ringbuffer_1_path.svg doc/html

	which pdflatex || (echo "pdflatex not found." && exit 1)
	which makeindex || (echo "makeindex not found." && exit 1)
	cp img/drawing_ringbuffer_1_path.eps doc/latex
	cd doc/latex && make >/dev/null 2>&1 && mv refman.pdf $(PDF_DOC_NAME) || (echo "pdf doc could not be created." && exit 1)

	@echo "==================================="
	@echo the documentation can be found here:
	@echo doc/html/index.html
	@echo doc/latex/$(PDF_DOC_NAME)

clean:
	-rm -f rb_test0
	-rm -f rb_test1
	-rm -f rb_test2
	-rm -rf doc
