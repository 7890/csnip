#include <unistd.h>
#include <sys/stat.h>
#include "rb.h"
#include "imgframe_t.h"


rb_t *rb_=NULL;
char *buf;
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
	int didwrite=0;
	while(didwrite<headersize)
	{
		didwrite+=rb_write(rb_,(void*)img_null_header+didwrite,(headersize-didwrite));
//		fprintf(stderr,"did write %d bytes\n",didwrite);
		usleep(500);
	}

	buf=calloc(bufsize,1);

	int count=0;
	while((count = read(file_fd, buf, bufsize)) > 0)
	{
//		fprintf(stderr,"count %d\n",count);
		int didwrite=0;
		while(didwrite<count)
		{
			didwrite+=rb_write(rb_,buf+didwrite,(count-didwrite));
//			fprintf(stderr,"did write %d bytes\n",didwrite);
			usleep(500);
		}
	}
	close (file_fd);
	free(buf);
	//catch ctrl+c

	return 0;
}//end main
//EOF
