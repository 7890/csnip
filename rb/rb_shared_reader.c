#include <unistd.h>
#include <pthread.h>
#include "rb.h"

//testing open and read ringbuffer in shared memory

static pthread_t reader_thread={0};
static pthread_mutex_t reader_thread_lock=PTHREAD_MUTEX_INITIALIZER;

rb_t *rb_=NULL;

//=============================================================================
static void *reader_thread_func(void *arg)
{
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer was NULL.\n");
		exit(1);
	}
	rb_debug(rb_);
	//srand(4321);

	int buf_size=32; //22 to match write thread content size
	char buf[buf_size];
	memset(buf,0,buf_size);
	uint64_t read=0;
	uint64_t read_total=0;

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

		fprintf(stderr,"read %" PRId64 " read_total %" PRId64 " '%s'\n"
			,read		//bytes read in last rb_read
			,read_total	//bytes read relative to main read buffer pos 0
			,buf_read	//contents that were read in last rb_read
		);
		fprintf(stdout,"%s",buf_read);
		fflush(stdout);
		rb_debug(rb_);

		//int r=rand()/50000;
		//fprintf(stderr,"rand %d\n",r);
		//usleep(r); //sleep random
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
int main(int argc, char *argv[])
{
	// ./rb_shared_reader 29d83c2e-9c83-11e5-a173-74d435e313ae 2>/dev/null

	if(argc<2)
	{
		fprintf(stderr,"need ringbuffer handle\n");
		exit(1);
	}

	fprintf(stderr,"opening ringbuffer from shared memory:\n%s\n",argv[1]);

	rb_=rb_open_shared(argv[1]);

	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	rb_debug(rb_);

	setup_reader_thread();
	while(1==1)
	{
		usleep(100000);
	}

	//catch ctrl+c

	//rb_free(rb);

}//end main

//EOF
