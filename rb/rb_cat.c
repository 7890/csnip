#include <unistd.h>
#include <pthread.h>
#include "rb.h"

rb_t *rb_=NULL;
unsigned char *buf;
int bufsize=64000;

#define RB_READ 1
#define RB_WRITE 2
int mode=RB_READ;

int rb_arg_index=1;

//tb/1605

//dump ringbuffer contents to stdout:
//rb_cat <shm handle>

//dump from stdin to ringbuffer
//... | rb_cat - <shm handle>

//=============================================================================
int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr,"need ringbuffer handle\n");
		exit(1);
	}

	if( argc>2 && strcmp(argv[1], "-") == 0)
	{
		mode=RB_WRITE;
		rb_arg_index=2;
	}

	fprintf(stderr,"opening ringbuffer from shared memory:\n%s\n",argv[rb_arg_index]);
	rb_=rb_open_shared(argv[rb_arg_index]);
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	rb_debug(rb_);

	buf=calloc(bufsize,1);

	if(mode==RB_READ)
	{
		while(rb_->no_more_input_data<1)
		{
			size_t count=rb_read(rb_,buf,bufsize);
			int i=0;
			for(;i<count;i++)
			{
				fprintf(stdout,"%c",buf[i]);
			}
			fflush(stdout);
			usleep(500);
		}
	}
	else
	{
		while(1==1)
		{
			int count=read(STDIN_FILENO, buf, bufsize);
			if(count<1){break;}
			////write to rb here
			rb_write(rb_,buf,count);
			usleep(500);
		}
	}
	free(buf);
	//catch ctrl+c
}//end main
//EOF
