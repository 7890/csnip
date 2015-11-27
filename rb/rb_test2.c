#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "rb.h"

//testing ringbuffer with two threads

static pthread_t reader_thread={0};
static pthread_mutex_t reader_thread_lock=PTHREAD_MUTEX_INITIALIZER;

static pthread_t writer_thread={0};
static pthread_mutex_t writer_thread_lock=PTHREAD_MUTEX_INITIALIZER;

rb_t *rb_=NULL;

#define READER_THREAD 33
#define WRITER_THREAD 77

//=============================================================================
void debug(rb_t *rb, int from_thread)
{
	if(rb==NULL)
	{
		fprintf(stderr,"\nrb is NULL\n");
		return;
	}
	fprintf(stderr,"can read  %zu @ %zu  can write %zu @ %zu   %s\n"
		,rb_can_read(rb)
		,rb->read_pointer
		,rb_can_write(rb)
		,rb->write_pointer
		,(from_thread==READER_THREAD ? "reader" : "writer")
	);
}

//=============================================================================
void print_vectors(rb_t *rb)
{
	rb_data_t data[2];
	rb_get_read_vector(rb,data);
	fprintf(stderr,"read vec size  %zu %zu =%zu  "
		,data[0].size
		,data[1].size
		,data[0].size+data[1].size
	);

	rb_get_write_vector(rb,data);
	fprintf(stderr,"write vec size %zu %zu =%zu\n"
		,data[0].size
		,data[1].size
		,data[0].size+data[1].size
	);
}

//=============================================================================
static void *reader_thread_func(void *arg)
{
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	debug(rb_,READER_THREAD);

	srand(4321);
	while(1==1)//run thread forever
	{
		char buf[3];
		rb_read(rb_,buf,3);//read 3
		debug(rb_,READER_THREAD);

		int r=rand()/5000;
		if(r%9==0)//insert drops
		{
			fprintf(stderr,"drop\n");
			rb_drop(rb_);
			debug(rb_,READER_THREAD);
		}

		fprintf(stderr,"rand %d\n",r);
		usleep(r); //sleep random
	}
}

//=============================================================================
static void *writer_thread_func(void *arg)
{
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	debug(rb_,WRITER_THREAD);

	srand(1234);
	while(1==1)
	{
		char buf[1]={'x'};
		rb_write(rb_,buf,1);
		debug(rb_,WRITER_THREAD);

		int r=rand()/10000;

		if(r%7==0) //insert write pointer advances
		{
			fprintf(stderr,"advance write pointer\n");
			rb_advance_write_pointer(rb_,7);
		}
		fprintf(stderr,"rand %d\n",r);
		usleep(r); //random sleep
	}
}

//=============================================================================
static void setup_reader_thread()
{
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_mutex_lock(&reader_thread_lock);
	pthread_create(&reader_thread, NULL, reader_thread_func, NULL);
}

//=============================================================================
static void setup_writer_thread()
{
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_mutex_lock(&writer_thread_lock);
	pthread_create(&writer_thread, NULL, writer_thread_func, NULL);
}

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
	
	rb_=rb_new(rb_size_request);
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	debug(rb_,0);

	setup_reader_thread();
	setup_writer_thread();
	while(1==1)
	{
		usleep(100000);
	}

//	rb_free(rb);

}//end main

//EOF
