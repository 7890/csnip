#include "rb_float.h"

#define ITEM_COUNT 8

unsigned char bytes[ITEM_COUNT]={'A','B','C','D','E','F','G','H'};
float floats[ITEM_COUNT]={0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8};

void print_bytes()
{
	int i=0;
	for(i=0;i<ITEM_COUNT;i++)
	{
		fprintf(stderr,"%c",bytes[i]);
	}
	fprintf(stderr,"\n");
}

void print_deinterleaved_bytes(unsigned char b[], int count)
{
	print_bytes();
	int i;
	for(i=0;i<count;i++)
	{
		fprintf(stderr,"%c ",b[i]);
//		fprintf(stderr,"%c (%d) ",b[i],b[i]);
	}
	fprintf(stderr,"\n");
}

void print_floats()
{
	int i=0;
	for(i=0;i<ITEM_COUNT;i++)
	{
		fprintf(stderr,"%.3f ",floats[i]);
	}
	fprintf(stderr,"\n");
}

void print_deinterleaved_floats(float b[], int count)
{
	print_floats();
	int i;
	for(i=0;i<count;i++)
	{
		fprintf(stderr,"%.3f ",b[i]);
	}
	fprintf(stderr,"\n");
}

//=============================================================================
int main()
{
	rb_t *rb=rb_new(100);//ITEM_COUNT*sizeof(float));
	if(rb==NULL) {return 1;}

	size_t wrote=rb_write(rb,bytes,ITEM_COUNT);
	fprintf(stderr,"wrote %zu bytes\n",wrote);
	rb_debug(rb);

	unsigned char bytes_deinterleaved[4];

	size_t count=3; size_t size=1; size_t off=0; size_t block=2;
	size_t deinterleaved=rb_deinterleave_items(rb, bytes_deinterleaved, count,size,off,block);
	fprintf(stderr,"deinterleave %zu items, item size %zu bytes, item_offset %zu, block_size %zu return %zu bytes\n"
		,count,size,off,block,deinterleaved
	);
	print_deinterleaved_bytes(bytes_deinterleaved, count);

	memset(bytes_deinterleaved,0,count);

	count=4; size=1; off=1; block=2;
	deinterleaved=rb_deinterleave_items(rb, bytes_deinterleaved, count,size,off,block);
	fprintf(stderr,"deinterleave %zu items, item size %zu bytes, item_offset %zu, block_size %zu return %zu bytes\n"
		,count,size,off,block,deinterleaved
	);
	print_deinterleaved_bytes(bytes_deinterleaved, deinterleaved);

	memset(bytes_deinterleaved,0,count);


	rb_drop(rb);

	wrote=rb_write(rb,(unsigned char *)floats,ITEM_COUNT*sizeof(float));
	fprintf(stderr,"wrote %zu bytes (%zu floats)\n",wrote,wrote/sizeof(float));

	float floats_deinterleaved[3];//={9,9,9};

	count=3; size=sizeof(float); off=0; block=2;
	deinterleaved=rb_deinterleave_items(rb, (unsigned char *)floats_deinterleaved, count,size,off,block);
	fprintf(stderr,"deinterleave %zu items, item size %zu bytes, item_offset %zu, block_size %zu return %zu bytes\n"
		,count,size,off,block,deinterleaved
	);

	print_deinterleaved_floats(floats_deinterleaved, deinterleaved/sizeof(float));

	memset(floats_deinterleaved,0,count*sizeof(float));

	count=4; size=sizeof(float); off=1; block=2;
	deinterleaved=rb_deinterleave_items(rb, (unsigned char *)floats_deinterleaved, count,size,off,block);
	fprintf(stderr,"deinterleave %zu items, item size %zu bytes, item_offset %zu, block_size %zu return %zu bytes\n"
		,count,size,off,block,deinterleaved
	);

	print_deinterleaved_floats(floats_deinterleaved, deinterleaved/sizeof(float));

	memset(floats_deinterleaved,0,count*sizeof(float));

	rb_free(rb);

	//audio, 3 channels
	rb=rb_new_audio(ITEM_COUNT*sizeof(float),"audiobuf",1000,3,sizeof(float));

	if(rb==NULL) {return 1;}
	wrote=rb_write(rb,(unsigned char *)floats,ITEM_COUNT*sizeof(float));
	fprintf(stderr,"wrote %zu bytes (%zu floats)\n",wrote,wrote/sizeof(float));
	rb_debug_linearbar(rb);

	count=3; off=0;

	deinterleaved=rb_deinterleave_audio(rb, (unsigned char *)floats_deinterleaved, count,off);
	fprintf(stderr,"deinterleave %zu frames, item_offset %zu, return %zu bytes\n"
		,count,off,deinterleaved
	);

	print_deinterleaved_floats(floats_deinterleaved, deinterleaved/sizeof(float));

	memset(floats_deinterleaved,0,count*sizeof(float));

	count=3; off=1;

	deinterleaved=rb_deinterleave_audio(rb, (unsigned char *)floats_deinterleaved, count,off);
	fprintf(stderr,"deinterleave %zu frames, item_offset %zu, return %zu bytes\n"
		,count,off,deinterleaved
	);

	print_deinterleaved_floats(floats_deinterleaved, deinterleaved/sizeof(float));

	memset(floats_deinterleaved,0,count*sizeof(float));

	rb_free(rb);

	return 0;
}
