CC ?= gcc
PDF_DOC_NAME ?= "lock-free_ringbuffer_rb.h.pdf"

default: rb

rb:	rb.h rb_test0.c rb_test1.c rb_test2.c

	$(CC) -o rb_test0 rb_test0.c
	$(CC) -o rb_test1 rb_test1.c
	$(CC) -o rb_test2 rb_test2.c -lpthread

doc:	rb.h doxyfile

	which doxygen || (echo "doxygen not found." && exit 1)
	doxygen ./doxyfile >/dev/null 2>&1 || (echo "html doc could not be created." && exit 1)

	which pdflatex || (echo "pdflatex not found." && exit 1)
	which makeindex || (echo "makeindex not found." && exit 1)

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
