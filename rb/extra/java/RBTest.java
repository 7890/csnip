//tb/1604

//=============================================================================
//=============================================================================
public class RBTest
{
	int bufsize=1280000;
	byte[] tmpbuf=new byte[bufsize];


	/*
	//part of csnip/rb, produce data
	rb_shared_writer 200000 2>/dev/null 1>&2

	//in another terminal, get latest handle
	handle=/dev/shm/"`ls -1tr /dev/shm/|tail -1`"

	//access buffer from java
	java RBTest $handle
	*/

//=============================================================================
	public static void main(String[] args) throws Exception
	{
		if(args.length<1)
		{
			e("syntax: <ringbuffer shared memory file>");
			e("i.e. /dev/shm/e91880b6-0b01-11e6-a139-74d435e313ae");
			System.exit(1);
		}
		RBTest t=new RBTest(args);
	}

//=============================================================================
	public void print_rb(RB rb) throws Exception
	{
		//clear screen
		System.err.print("\033[2J\033[1;1H");
//		e("mapped byte buffer info: "+mbb.toString());
//		e("isDirect: "+mbb.isDirect());
//		p("filechannel size bytes: "+fc.size());
		e("is magic: "		+rb.magic());
		e("version: "		+rb.version());
		e("size: "		+rb.size());
		e("read index: "	+rb.read_index());
		e("write index: "	+rb.write_index());
		e("last_was_write: "	+rb.last_was_write());
		e("memory_locked: "	+rb.memory_locked());
		e("in_shared_memory: "	+rb.in_shared_memory());
		e("unlink_requested: "	+rb.unlink_requested());
		e("no_more_input_data: "+rb.no_more_input_data());
		e("sample_rate: "	+rb.sample_rate());
		e("channel_count: "	+rb.channel_count());
		e("bytes_per_sample: "	+rb.bytes_per_sample());
		e("total bytes read: "	+rb.total_bytes_read());
		e("total bytes write: "	+rb.total_bytes_write());
		e("total bytes peek: "	+rb.total_bytes_peek());
		e("total underflows: "	+rb.total_underflows());
		e("total overflows: "	+rb.total_overflows());
		e("shm_handle: '"	+rb.shm_handle()+"'");
		e("human_name: '"	+rb.human_name()+"'");
		e("can read: "		+rb.can_read());
		e("can read frames: "	+rb.can_read_frames());
		e("can write: "		+rb.can_write());
		e("can write frames: "	+rb.can_write_frames());
	}//end print_rb()

//=============================================================================
	public RBTest(String[] args) throws Exception
	{
		RB rb=new RB();
		rb.open_shared(args[0]);

		while(true)
		{
			print_rb(rb);
			if(rb.can_read()>0)//=bufsize)
			{
				//test read from buffer (which has size >=bufsize, available data >=bufsize)
				long count=rb.generic_read(tmpbuf,bufsize,0);
//				e("read count: "+count);
			}
//			e("can read after read: "+rb.can_read());
			try{Thread.sleep(4);}catch(Exception e){}
		}//end while true
//		rb.free();
	}//end RB constructor

//=============================================================================
	public static void p(String s)
	{
		System.out.println(s);
	}

//=============================================================================
	public static void e(String s)
	{
		System.err.println(s);
	}
}//end class RBTest
//EOF
