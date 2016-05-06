#include "rb_float.h"

#define FLOAT_COUNT 8

//=============================================================================
int main()
{
	rb_t *rb=rb_new(FLOAT_COUNT*sizeof(float));
	if(rb==NULL) {return 1;}
	rb_debug(rb);

	float floats[FLOAT_COUNT]={0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8};
	int wrote=rb_write(rb,(char*)floats,FLOAT_COUNT*sizeof(float));
	fprintf(stderr,"wrote %d bytes (%zu floats)\n",wrote,wrote/sizeof(float));

	rb_debug(rb);
	rb_print_regions(rb);

	float f1;
	int read=rb_read_float(rb,&f1);
	fprintf(stderr,"read %d bytes (%zu float) %f\n",read,read/sizeof(float),f1);

	int peeked=rb_peek_float(rb,&f1);
	fprintf(stderr,"peeked %d bytes (%zu float) %f\n",peeked,peeked/sizeof(float),f1);

	float f2=0.99;
	wrote=rb_write_float(rb,&f2);
	fprintf(stderr,"wrote %d bytes (%zu float) %f\n",wrote,wrote/sizeof(float),f2);

	int skipped=rb_skip_float(rb);
	fprintf(stderr,"skipped %d bytes (%zu float)\n",skipped,skipped/sizeof(float));

	int peek_at=rb_can_read(rb)-sizeof(float);
	peeked=rb_peek_float_at(rb,&f1,peek_at);
	fprintf(stderr,"peeked %d bytes (%zu float) at %d %f\n",peeked,peeked/sizeof(float),peek_at,f1);

	rb_debug(rb);
	rb_print_regions(rb);

	rb_free(rb);
	return 0;
}
