#include "rb.h"
#include <unistd.h>
//=============================================================================
int main()
{
	rb_t *rb=rb_new(11);
	rb_debug(rb);

	//fill ringbuffer once
	char *content="ringbuffer ";
	rb_write(rb,content,11);
	rb_debug(rb);

	//read back forever
	char readback[12];
	while(1==1)
	{
		size_t read=rb_overread(rb,readback,11);
		rb_debug(rb);
		readback[read]='\0';
		fprintf(stderr,"overread: %s\n",readback);
		//permute
		rb_overskip(rb,1);
		usleep(50000);
	}

	rb_free(rb);

	return 0;
}
