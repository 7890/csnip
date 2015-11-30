#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "rb.h"

//testing ringbuffer with two uncoordinated, non-waiting threads:
//a writer thread and a reader thread
//data pushed through the ringbuffer should keep the same byte values, sequence of bytes

static pthread_t reader_thread={0};
static pthread_mutex_t reader_thread_lock=PTHREAD_MUTEX_INITIALIZER;

static pthread_t writer_thread={0};
static pthread_mutex_t writer_thread_lock=PTHREAD_MUTEX_INITIALIZER;

rb_t *rb_=NULL;

#define READER_THREAD 0
#define WRITER_THREAD 1

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
		fprintf(stderr,"ringbuffer was NULL.\n");
		exit(1);
	}
	debug(rb_,READER_THREAD);
	//srand(4321);

	size_t buf_size=32; //22 to match write thread content size
	char buf[buf_size];
	memset(buf,0,buf_size);
	size_t read=0;
	size_t read_total=0;

	while(1==1)
	{
		if(read_total>=buf_size)
		{
			//memset(buf,0,buf_size);
			read_total=0;
			fprintf(stdout,"\n");
			fflush(stdout);
		}

		read=rb_read(
			rb_			//read from this ringbuffer
			,buf+read_total		//read into buf at offset read_total
			,buf_size-read_total	//request to read remaining bytes to fill buf
		);

		//buffer to indicate which part was read into buffer
		char buf_read[read+1]; //+1 for null termination for printing
		buf_read[read]='\0';

		//copy from main read buffer
		memcpy(buf_read,buf+read_total,read);

		read_total+=read;

		fprintf(stderr,"read %zu read_total %zu '%s'\n"
			,read		//bytes read in last rb_read
			,read_total	//bytes read relative to main read buffer pos 0
			,buf_read	//contents that were read in last rb_read
		);
		fprintf(stdout,"%s",buf_read);
		fflush(stdout);
		debug(rb_,READER_THREAD);

		//int r=rand()/50000;
		//fprintf(stderr,"rand %d\n",r);
		//usleep(r); //sleep random
	}
}

//=============================================================================
static void *writer_thread_func(void *arg)
{
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer was NULL.\n");
		exit(1);
	}
	debug(rb_,WRITER_THREAD);
	//srand(1234);

	//example data buffer
	char *buf="ringbuffers are cool. ";//\0";
	size_t content_length=22;

	//in one rb_write operation (reset each cylce)
	size_t write=0;
	//relative to full content length (reset after fully written)
	size_t write_total=0;

	//repeat to write contents of buf to ringbuffer
	//handle case when rb_write returns less bytes than requested
	//a reading process should get a stream of identical sentences
	while(1==1)
	{
		if(write_total>=content_length)
		{
			write_total=0;
//			fprintf(stdout,"\n");
//			fflush(stdout);
		}

		write=rb_write(
			rb_				//write to this ringbuffer
			,buf+write_total		//from source buffer, starting at offset 'write_total'
			,content_length-write_total	//request to write all remaining bytes
		);

		//buffer to indicate which part of source buffer could be written
		char buf_written[write+1];
		buf_written[write]='\0';

		//copy from source buffer
		memcpy(buf_written,buf+write_total,write);

		write_total+=write;

		fprintf(stderr,"write %zu write_total %zu '%s'\n"
			,write		//bytes written in last rb_write
			,write_total	//bytes written relative to source buffer pos 0
			,buf_written	//contents that were written
		);
//		fprintf(stdout,"%s",buf_written);
//		fflush(stdout);
		debug(rb_,WRITER_THREAD);

		//int r=rand()/100000;
		//fprintf(stderr,"rand %d\n",r);
		//usleep(r); //random sleep
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

	// ./rb_test2 1 >/tmp/out.txt
	// ./rb_test2 32 2>/dev/null

	if(argc<2)
	{
		fprintf(stderr,"need number: ringbuffer size [bytes]\n");
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

	//rb_free(rb);

}//end main

//EOF
