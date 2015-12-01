default: rb

rb:	rb.h rb_test0.c rb_test1.c rb_test2.c

	gcc -o rb_test0 rb_test0.c
	gcc -o rb_test1 rb_test1.c
	gcc -o rb_test2 rb_test2.c -lpthread

doc:	rb.h

	which doxygen || (echo "doxygen not found." && exit 1)
	doxygen ./doxyfile

clean:
	-rm -f rb_test0
	-rm -f rb_test1
	-rm -f rb_test2
	-rm -rf doc
