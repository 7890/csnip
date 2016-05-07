#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include "rb.h"
#include "imgframe_t.h"


rb_t *rb_=NULL;
unsigned char *buf;
int bufsize=64000;
int rb_arg_index=1;
imgframe_t *img_null_header;
//tb/1605
/*
dump image file to ringbuffer:
img_cat <shm handle> <img1>
*/

//=============================================================================
int main(int argc, char *argv[])
{
	if(argc<3)
	{
		fprintf(stderr,"need ringbuffer handle and image file\n");
		exit(1);
	}

	rb_arg_index=1;
	fprintf(stderr,"opening ringbuffer from shared memory:\n%s\n",argv[rb_arg_index]);
	rb_=rb_open_shared(argv[rb_arg_index]);
	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}
	rb_debug(rb_);

	///should do for all following args

	int file_fd=open(argv[2], O_RDONLY);

	if(file_fd==-1)
	{
		fprintf(stderr,"could not open image file.\n");
		return 1;
	}

	struct stat st;
	fstat(file_fd,&st);
	int filesize = st.st_size;
/*
int size
, int w
, int h
, int channel_count
, int bytes_per_channel
, int stream_number
, float fps
*/
	img_null_header=imgframe_new(filesize,0,0,0,0,0,0);
	int headersize=sizeof(imgframe_t);

	//write header
	int canwrite=0;
	int dowrite=0;
	int didwrite=0;
	while(didwrite<headersize)
	{
		canwrite=rb_can_write(rb_);
//		fprintf(stderr,"cat write %d bytes\n",canwrite);
		dowrite=MIN((headersize-didwrite),canwrite);
//		fprintf(stderr,"do write %d bytes\n",dowrite);
		didwrite+=rb_write(rb_,(void*)img_null_header+didwrite,dowrite);
//		fprintf(stderr,"did write %d bytes\n",didwrite);
		usleep(500);
	}

	buf=calloc(bufsize,1);

	int ret_read=0;
	while((ret_read = read(file_fd, buf, bufsize)) > 0)
	{
//		fprintf(stderr,"ret_read %d\n",ret_read);
		int canwrite=0;
		int dowrite=0;
		int didwrite=0;
		while(didwrite<ret_read)
		{

			canwrite=rb_can_write(rb_);
//			fprintf(stderr,"cat write %d bytes\n",canwrite);
			dowrite=MIN((ret_read-didwrite),canwrite);
//			fprintf(stderr,"do write %d bytes\n",dowrite);
			didwrite+=rb_write(rb_,buf+didwrite,dowrite);
//			fprintf(stderr,"did write %d bytes\n",didwrite);
			usleep(500);
		}
	}
	close (file_fd);
	free(buf);
	//catch ctrl+c
}//end main
//EOF
