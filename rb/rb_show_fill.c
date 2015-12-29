#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "rb.h"

static char *bar_string="############################################################";

//=============================================================================
void debug(rb_t *rb)
{
	if(rb==NULL)
	{
		fprintf(stderr,"rb is NULL\n");
		return;
	}
	int max_width=10;

	fprintf(stderr,"\rr: %*zu @ %*zu  w: %*zu @ %*zu last: %s mlock: %s"
		,max_width
		,rb_can_read(rb)
		,max_width
		,rb->read_index
		,max_width
		,rb_can_write(rb)
		,max_width
		,rb->write_index
		,rb->last_was_write ? "w" : "r"
		,rb->memory_locked ? "y." : "n"
	);
}

//=============================================================================
void debug_linearbar(rb_t *rb, int sample_rate, int channel_count, int bytes_per_sample)
{
	if(rb==NULL)
	{
		fprintf(stderr,"rb is NULL\n");
		return;
	}

	int bar_ticks_count=45;

	size_t can_w=rb_can_write(rb);

	float fill_level;
	if(can_w==0)
	{
		fill_level=1;
	}
	else if(can_w==rb_size(rb))
	{
		fill_level=0;
	}
	else
	{
		fill_level=1-(float)can_w/rb_size(rb);
	}

	int bar_ticks_show=fill_level*bar_ticks_count;

	if(sample_rate>0)
	{
		fprintf(stderr,"\r%*s\rfill %.6f |%*.*s%s%*s| %9.3f s"
			,bar_ticks_count+15
			,""
			,fill_level
			,bar_ticks_show
			,bar_ticks_show
			,bar_string
			,fill_level==0 ? "_" : (fill_level==1 ? "^" : ">")
			,(bar_ticks_count-bar_ticks_show)
			,""
			,(float)(rb_size(rb)-can_w)/bytes_per_sample/sample_rate/channel_count
		);
	}
	else
	{
		fprintf(stderr,"\r%*s\rfill %.6f |%*.*s%s%*s| %10zu"
			,bar_ticks_count+15
			,""
			,fill_level
			,bar_ticks_show
			,bar_ticks_show
			,bar_string
			,fill_level==0 ? "_" : (fill_level==1 ? "^" : ">")
			,(bar_ticks_count-bar_ticks_show)
			,""
			,rb_size(rb)-can_w
		);
	}
}

//=============================================================================
int main(int argc, char *argv[])
{
	// ./rb_shared_debug 29d83c2e-9c83-11e5-a173-74d435e313ae 2>/dev/null

	if(argc<2)
	{
		fprintf(stderr,"need at least ringbuffer handle.\n");
		fprintf(stderr,"syntax: <ringbuffer handle>\n");
		exit(1);
	}

	rb_t *rb_=rb_open_shared(argv[1]);

	if(rb_==NULL)
	{
		fprintf(stderr,"ringbuffer with size 0?\n");
		exit(1);
	}

	int sample_rate=rb_sample_rate(rb_);
	int channel_count=rb_channel_count(rb_);
	int bytes_per_sample=rb_bytes_per_sample(rb_);

	fprintf(stderr,"%s\n",rb_shared_memory_handle(rb_));
	if(sample_rate>0)
	{
		fprintf(stderr,"audio: %3d channels @ %6d Hz, %2d bytes per sample, capacity %9.3f s\n"
			,channel_count
			,sample_rate
			,bytes_per_sample
			,(float)rb_size(rb_)/bytes_per_sample/sample_rate/channel_count
		);
	}

	while(1==1)
	{
//		debug(rb_);
		debug_linearbar(rb_,sample_rate,channel_count,bytes_per_sample);
		usleep(1000);
	}

}//end main

//EOF
