import java.io.*;
import java.nio.*;
import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.color.*;
import java.awt.event.*;
import javax.imageio.*;
import javax.swing.*;

//tb/1604

//test read image frames from ringbuffer
//each frame has a header followed by raw data (PGM or PPM)
//cat help.txt | shout --img --termwidth 180 --resize 640x --negate --trans black --eval - | convert - -define png:color-type=4 - > help.png

//=============================================================================
//=============================================================================
public class RBPixelPipe extends Thread
{
	private RB rb=new RB();
	private ImgFrameHeader img_header;
	private BufferedImage bi;

	private boolean is_paused=false;
	private boolean frame_available=false;

//=============================================================================
	public RBPixelPipe(String shm_uuid) throws Exception
	{
		rb.open_shared(shm_uuid);
		img_header=new ImgFrameHeader();
	}

//=============================================================================
	public void setPaused(boolean paused)
	{
		is_paused=paused;
	}

//=============================================================================
	public boolean isPaused()
	{
		return is_paused;
	}

//=============================================================================
	public float getBufferFillLevel()
	{
		return (float)rb.can_read()/rb.size();
	}

//=============================================================================
	public boolean available()
	{
		return frame_available;
	}

//=============================================================================
	public void next()
	{
		frame_available=false;
	}

//=============================================================================
	public ImgFrameHeader getFrameHeader()
	{
		return img_header;
	}

//=============================================================================
	public BufferedImage getFrame()
	{
		return bi;
	}

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

//=============================================================================
	public void run()
	{
	while(true)
	{
		if(is_paused)
		{
			try{Thread.sleep(10);}catch(Exception e){}
			continue;
		}

		if(rb.can_read()>=img_header.frameheader_size)
		{
			long count=rb.peek(img_header.frameheaderbuf,img_header.frameheader_size);
			if(!img_header.parse())
			{
				e("could not parse image header");
				try{Thread.sleep(1);}catch(Exception e){}
				continue;
			}
			if(rb.can_read()>=img_header.frameheader_size+img_header.frame_size)
			{
				///need rb.skip method
				rb.generic_read(img_header.frameheaderbuf,img_header.frameheader_size,0);

				byte[] pixelbuf=new byte[img_header.frame_size];

				count=rb.generic_read(pixelbuf,img_header.frame_size,0);
//				e("read count: "+count);
				//print first 64 pixel values
//				for(int k=0;k<64;k++){e((pixelbuf[k] & 0xFF)+" ");}e("");

				if(img_header.channel_count==1)
				{
					if(img_header.bytes_per_channel==1)
					{
						bi=ImgConv._8itGrayToBuffered(pixelbuf,img_header.w,img_header.h);
					}
					else if(img_header.bytes_per_channel==2)
					{
						bi=ImgConv._16bitGrayToBuffered(pixelbuf,img_header.w,img_header.h);
					}
					else
					{}///
				}
				else if(img_header.channel_count==3)
				{
					if(img_header.bytes_per_channel==1)
					{
						bi=ImgConv._8bitRGBToBuffered(pixelbuf,img_header.w,img_header.h);
					}
					else{}///
				}
				else{}///

				if(bi!=null)
				{
					frame_available=true;
					//wait here until frame fetched by calling process
					while(frame_available)
					{
						try{Thread.sleep(1);}catch(Exception e){}
					}
				}
			}//end rb.can_read()>=frameheader_size+img_header.frame_size 
		}//end rb.can_read()>=img_header.frameheader_size
//		e("can read after read: "+rb.can_read());
		try{Thread.sleep(1);}catch(Exception e){}
	}//end while true
	}//end run()
}//end class RBPixelPipe
//EOF
