#include "rb.h"

//a bit of non-systematic testing

//=============================================================================
int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr,"need number\n");
		exit(1);
	}

	int rb_size_request=atoi(argv[1]);

	fprintf(stderr,"\n==creating new ringbuffer of size %d\n",rb_size_request);
	
	rb_t *rb=rb_new(rb_size_request);
	if(rb==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	rb_debug(rb);

	fprintf(stderr,"\n==write full + 1\n");
	int i;
	for(i=0;i<rb_size_request+1;i++)
	{
		char put[1]={i};
		int wrote=rb_write(rb,put,1);
		rb_debug(rb);
	}

	fprintf(stderr,"\n==peek full + 1\n");
	char pull[rb_size_request];
	int peeked=rb_peek(rb,pull,rb_size_request+1);
	rb_debug(rb);

	fprintf(stderr,"\n==read full + 1\n");
	for(i=0;i<rb_size_request+1;i++)
	{
		char pull[1];
		int read=rb_read(rb,pull,1);
		rb_debug(rb);
	}

	fprintf(stderr,"\n==write full\n");
	for(i=0;i<rb_size_request;i++)
	{
		char put[1]={i};
		int wrote=rb_write(rb,put,1);
		rb_debug(rb);
	}

	rb_print_regions(rb);

	fprintf(stderr,"\n==advance read pointer 1\n");
	rb_advance_read_index(rb,1);
	rb_debug(rb);

	fprintf(stderr,"\n==drop\n");
	rb_drop(rb);
	rb_debug(rb);
	rb_print_regions(rb);

	fprintf(stderr,"\n==write 1\n");
	char put[1]={'a'};
	int wrote=rb_write(rb,put,1);
	rb_debug(rb);
	rb_print_regions(rb);

	fprintf(stderr,"\n==read 1\n");
	char put2[1];
	int read=rb_read(rb,put2,1);
	rb_debug(rb);
	rb_print_regions(rb);

	fprintf(stderr,"\n==write 4\n");
	char put3[4]={'a','b','c','d'};
	wrote=rb_write(rb,put3,4);
	rb_debug(rb);

	rb_print_regions(rb);

	int advanced;
	int k;
	for(k=0;k<20;k++)
	{
		fprintf(stderr,"\n==advance write 3\n");
		advanced=rb_advance_write_index(rb,3);
		rb_debug(rb);
		rb_print_regions(rb);

		fprintf(stderr,"\n==advance read 2\n");
		advanced=rb_advance_read_index(rb,2);
		rb_debug(rb);
		rb_print_regions(rb);
	}
	for(k=0;k<20;k++)
	{
		fprintf(stderr,"\n==advance read 3\n");
		advanced=rb_advance_read_index(rb,2);
		rb_debug(rb);
		rb_print_regions(rb);

		fprintf(stderr,"\n==advance write 2\n");
		advanced=rb_advance_write_index(rb,3);
		rb_debug(rb);
		rb_print_regions(rb);
	}

	rb_region_t d;
	rb_get_next_write_region(rb,&d);

	fprintf(stderr,"\n==got next write buffer, can write %" PRId64 "\n",d.size);

	fprintf(stderr,"\n==drop\n");
	rb_drop(rb);
	rb_debug(rb);
	rb_print_regions(rb);

	rb_get_next_write_region(rb,&d);

	fprintf(stderr,"\n==got next write buffer, can write %" PRId64 "\n",d.size);
	d.buffer[0]='x';

	fprintf(stderr,"\n==advance write 4\n");
	advanced=rb_advance_write_index(rb,4);
	rb_debug(rb);
	rb_print_regions(rb);

	rb_get_next_read_region(rb,&d);

	fprintf(stderr,"\n==got next read buffer, can read %" PRId64 "\n",d.size);

	fprintf(stderr,"\n==advance read 4\n");
	rb_advance_read_index(rb,4);
	rb_debug(rb);
	rb_print_regions(rb);

	rb_get_next_read_region(rb,&d);

	fprintf(stderr,"\n==got next read buffer, can read %" PRId64 "\n",d.size);
	d.buffer[0]='x';

	fprintf(stderr,"\n==advance read 1\n");
	advanced=rb_advance_read_index(rb,1);
	rb_debug(rb);
	rb_print_regions(rb);
	
	fprintf(stderr,"\n==reset\n");
	rb_reset(rb);
	rb_debug(rb);
	fprintf(stderr,"\n==free\n");
	rb_free(rb);
	rb_debug(rb);

	return 0;
}//end main
//EOF
