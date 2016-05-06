#include <unistd.h>
#include <pthread.h>
#include "rb.h"

//testing create and write to ringbuffer in shared memory

static pthread_t writer_thread={0};
static pthread_mutex_t writer_thread_lock=PTHREAD_MUTEX_INITIALIZER;

rb_t *rb_=NULL;

//=============================================================================
static void *writer_thread_func(void *arg)
{
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer was NULL.\n");
		exit(1);
	}
	rb_debug(rb_);

	//example data buffer
	const char *buf="ringbuffers are cool. ";
	const int content_length=22;

	//in one rb_write operation (reset each cylce)
	int write=0;
	//relative to full content length (reset after fully written)
	int write_total=0;

	//repeat to write contents of buf to ringbuffer
	//handle case when rb_write returns less bytes than requested
	//a reading process should get a stream of identical sentences
	while(1==1)
	{
		if(write_total>=content_length)
		{
			write_total=0;
			fprintf(stdout,"\n");
			fflush(stdout);
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

		fprintf(stderr,"write %d write_total %d '%s'\n"
			,write		//bytes written in last rb_write
			,write_total	//bytes written relative to source buffer pos 0
			,buf_written	//contents that were written
		);
		fprintf(stdout,"%s",buf_written);
		fflush(stdout);
		rb_debug(rb_);

		//int r=rand()/1000000;
		//fprintf(stderr,"rand %d\n",r);
		//usleep(100000);//r); //random sleep
	}
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
	// ./rb_shared_writer 33 2>/dev/null

	if(argc<2)
	{
		fprintf(stderr,"need number: ringbuffer size [bytes] OR shared memory handle\n");
		exit(1);
	}

	fprintf(stderr,"trying to attach ringbuffer in shared memory\n");
	rb_=rb_open_shared(argv[1]);

	if(rb_!=NULL)
	{
		goto _have_rb;
	}

	int arg=atoi(argv[1]);
	//fprintf(stderr,"%d\n",arg);
	if(arg>0)
	{
		int rb_size_request=arg;

		fprintf(stderr,"creating new ringbuffer of size %d in shared memory\n",rb_size_request);
	
		rb_=rb_new_shared(rb_size_request);

		if(rb_==NULL)
		{
			fprintf(stderr,"ringbuffer with size 0?\n");
			exit(1);
		}
		fprintf(stderr,"mlock: %d\n",rb_mlock(rb_));
		fprintf(stdout,"header size: %zu\n",sizeof(rb_t));

		fprintf(stdout,"call rb_shared_reader with this handle:\n%s\n",rb_->shm_handle);
		fflush(stdout);
	}
	else
	{
		fprintf(stderr,"error\n");
		exit(1);
	}

_have_rb:

	rb_debug(rb_);

	setup_writer_thread();
	while(1==1)
	{
		usleep(100000);
	}

	//catch ctrl+c

	//rb_free(rb);

}//end main

//EOF
