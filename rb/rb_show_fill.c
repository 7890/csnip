#include <unistd.h>
#include <pthread.h>
#include "rb.h"

//=============================================================================
void debug(rb_t *rb)
{
	if(rb==NULL)
	{
		fprintf(stderr,"rb is NULL\n");
		return;
	}
	int max_width=10;

	fprintf(stderr,"\rr: %*" PRId64 " @ %*" PRId64 " w: %*" PRId64 " @ %*" PRId64 " last: %s mlock: %s"
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
int main(int argc, char *argv[])
{
	// ./rb_show_fill 29d83c2e-9c83-11e5-a173-74d435e313ae

	if(argc<2)
	{
		fprintf(stderr,"need at least one ringbuffer handle.\n");
		fprintf(stderr,"syntax: <ringbuffer handle> (<ringbuffer handle> ...)\n");
		exit(1);
	}
	//rb_t *rb_=rb_open_shared(argv[1]);

	//array of pointers to ringbuffers
	rb_t *rbs[argc-1];
	int i;
	for(i=0;i<argc-1;i++)
	{
		rbs[i]=rb_open_shared(argv[i+1]);
		if(rbs[i]==NULL)
		{
			fprintf(stderr,"ringbuffer with size 0?\n");
			exit(1);
		}
	}

	int number_of_buffers=argc-1;

	while(1==1)
	{
		//clear screen
		fprintf(stderr,"\033[2J\033[1;1H");

		for(i=0;i<argc-1;i++)
		{
			//ignore unlinked
			if(rbs[i]==NULL)
			{
				//ev. try "reconnect"? -> if same name used again
				///rbs[i]=rb_open_shared(argv[i+1]);
				continue;
			}
			//remove buffers requested to unlink
			if(rb_is_unlink_requested(rbs[i]))
			{
				fprintf(stderr,"\nbuffer disappeared gracefully.\n");
				rb_free(rbs[i]);
				rbs[i]=NULL;
				number_of_buffers--;
				if(number_of_buffers<=0)
				{
					return 0;
				}
			}
			//display buffer
			else
			{
				rb_debug_linearbar(rbs[i]);
				fprintf(stderr,"\n");
			}
		}
		usleep(50000);
	}
}//end main
//EOF
