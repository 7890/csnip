#include "rb.h"
//=============================================================================
int main(int argc, char* argv[])
{
	if(argc<3)
	{
		fprintf(stderr,"syntax: <size in bytes> <name>\n");		
		return 1;
	}
	int size=atoi(argv[1]);
	rb_t *rb=rb_new_shared_named(size,argv[2]);
	rb_debug(rb);
	return 0;
}
