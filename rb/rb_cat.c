#include <unistd.h>
#include <pthread.h>
#include "rb.h"

#define RB_READ 1
#define RB_WRITE 2

rb_t *rb_=NULL;
unsigned char *buf;
int bufsize=64000;
int mode=RB_READ;
int rb_arg_index=1;

//tb/1605
/*
dump ringbuffer contents to stdout:
rb_cat <shm handle>

dump from stdin to ringbuffer:
... | rb_cat - <shm handle>
*/

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
			int count=rb_read(rb_,buf,bufsize);
//			fprintf(stderr,"read %d bytes from ringbuffer\n",count);
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
		int count=0;
		while(1==1)
		{
			count=read(STDIN_FILENO, buf, bufsize);
//			fprintf(stderr,"read %d bytes from stdin\n",count);
			if(count<1){break;}
			int didwrite=0;
			while(didwrite<count)
			{
				didwrite+=rb_write(rb_,buf+didwrite,(count-didwrite));
//				fprintf(stderr,"did write %d bytes\n",didwrite);
				usleep(500);
			}
		}
	}
	free(buf);
	//catch ctrl+c
}//end main
//EOF
