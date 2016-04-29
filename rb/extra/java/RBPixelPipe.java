import java.io.*;
import java.nio.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.color.*;
import javax.imageio.*;
import javax.swing.*;

//tb/1604

//test read image frames from ringbuffer
//each frame has a header followed by raw data (PGM or PPM)

//java RBPixelPipe /dev/shm/`ls -1tr /dev/shm/|tail -1`

//=============================================================================
//=============================================================================
public class RBPixelPipe
{
	final RB rb;
	ImgFrameHeader img_header;

	JFrame main_frame;
	ImagePanel ip;

//=============================================================================
	public static void main(String[] args) throws Exception
	{
		if(args.length<1)
		{
			e("syntax: <ringbuffer shared memory file>");
			e("i.e. /dev/shm/e91880b6-0b01-11e6-a139-74d435e313ae");
			System.exit(1);
		}
		RBPixelPipe t=new RBPixelPipe(args);
	}

//=============================================================================
	public RBPixelPipe(String[] args) throws Exception
	{
		setupGUI();
		rb=new RB();
		rb.open_shared(args[0]);

		img_header=new ImgFrameHeader();

		PixelPipeFrameReadThread pixelpipe=new PixelPipeFrameReadThread();
		pixelpipe.start();
//		rb.free();
	}

//=============================================================================
	private void setupGUI()
	{
		main_frame=new JFrame();
		ip=new ImagePanel();
		main_frame.add(ip);
		main_frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		main_frame.setSize(640,480);
		main_frame.show();		
	}

//http://stackoverflow.com/questions/12154090/creating-8-bit-image-from-byte-array
//=============================================================================
	public static BufferedImage _8itGrayToBuffered(byte[] buffer, int width, int height)
	{
		try
		{
			//https://docs.oracle.com/javase/7/docs/api/java/awt/color/ColorSpace.html
			ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_GRAY);
			int[] nBits = { 8 };
			//https://docs.oracle.com/javase/7/docs/api/java/awt/image/ComponentColorModel.html
			ColorModel cm = new ComponentColorModel(cs, null, false, true,
				Transparency.OPAQUE, DataBuffer.TYPE_BYTE);

			SampleModel sm = cm.createCompatibleSampleModel(width, height);
			DataBufferByte db = new DataBufferByte(buffer, width * height);
			WritableRaster raster = Raster.createWritableRaster(sm, db, null);
			BufferedImage result = new BufferedImage(cm, raster, false, null);
			return result;
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		return null;
	}

//=============================================================================
	public static BufferedImage _16bitGrayToBuffered(byte[] pixels, int width, int height)
	{
		try
		{
			short[] pixels_=new short[pixels.length/2];
			///http://stackoverflow.com/questions/736815/2-bytes-to-short-java
			for(int i=0;i<pixels.length;i+=2)
			{
				short val=(short)( ((pixels[i]&0xFF)<<8) | (pixels[i+1]&0xFF) );
				pixels_[i/2]=val;///hmm
			}

			BufferedImage image 
				= new BufferedImage(width, height, BufferedImage.TYPE_USHORT_GRAY);
			image.getRaster().setDataElements(0,0,width,height,pixels_);
			return image;
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		return null;
	}

//=============================================================================
	public static BufferedImage _8bitRGBToBuffered(byte[] pixels, int width, int height)
	{
		try
		{
			BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_3BYTE_BGR);
			image.getRaster().setDataElements(0,0,width,height,pixels);
			return image;
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		return null;
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

//http://stackoverflow.com/questions/299495/how-to-add-an-image-to-a-jpanel
//=============================================================================
//=============================================================================
public class ImagePanel extends JPanel
{
	private BufferedImage image;

//=============================================================================
	public ImagePanel(){}

//=============================================================================
	public ImagePanel(BufferedImage image) {
		this.image=image;
	}

//=============================================================================
	public void setImage(BufferedImage image)
	{
			this.image=image;
	}

//=============================================================================
	@Override
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		if(image!=null)
		{
			Graphics2D gd = (Graphics2D) g.create();
			gd.drawImage(image, 0, 0, getWidth(), getHeight(), this);
			gd.dispose();
			//g.drawImage(image, 0, 0, null); //see javadoc for more info on the parameters
		}
	}
}//end class ImagePanel

//=============================================================================
//=============================================================================
class ImgFrameHeader
{
/*
//-Wunknown-pragmas
#pragma pack(push)
#pragma pack(1)
typedef struct
{
        char magic[8];                  //8
        int pixel_data_size_bytes;      //+4 =12
        uint64_t frame_number;          //+8 =20
        int width;                      //+4 =24
        int height;                     //+4 =28
        int channel_count;              //+4 =32
        int bytes_per_channel;          //+4 =36
        int stream_number;              //+4 =40
        uint64_t millis_since_epoch;    //+8 =48
}
imgframe_t;
#pragma pack(pop)
*/

	static final int frameheader_size=48; //pragma packed
	byte[] frameheaderbuf=new byte[frameheader_size];

	byte magic[]=new byte[8];
	String magic_="";
	int frame_size=0;
	long frame_number=0;
	int w=0;
	int h=0;
	int channel_count=0;
	int bytes_per_channel=0;
	int stream_number=0;
	long millis_since_epoch=0;

	public boolean parse()
	{
		try
		{
			ByteBuffer bb=ByteBuffer.wrap(frameheaderbuf);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			bb.position(0);
			bb.get(magic);
			magic_=new String(magic,"UTF-8");
//			e("magic: "+magic_);
			if(!magic_.equals("imgf000\0"))
			{
				e("LOST SYNC");
				///find next frame with magic ...
				return false;
			}

			frame_size=bb.getInt();
//			e("pixel_data_size_bytes: "+frame_size);
			frame_number=bb.getLong();
//			e("frame_number: "+frame_number);
			w=bb.getInt();
//			e("width: "+w);
			h=bb.getInt();
//			e("height: "+h);
			channel_count=bb.getInt();
//			e("channel_count: "+channel_count);
			bytes_per_channel=bb.getInt();
//			e("bytes_per_channel: "+bytes_per_channel);
			return true;
		}
		catch(Exception e){e.printStackTrace();}
		return false;
	}
}//end class ImgFrameHeader

//=============================================================================
//=============================================================================
public class PixelPipeFrameReadThread extends Thread
{
	public void run()
	{
	while(true)
	{
		if(rb.can_read()>=img_header.frameheader_size)
		{
			long count=rb.peek(img_header.frameheaderbuf,img_header.frameheader_size);
			if(!img_header.parse())
			{
				e("could not parse image header");
				continue;
			}
		}//end rb.can_read()>=frameheader_size

		if(rb.can_read()>=img_header.frameheader_size+img_header.frame_size)
		{
			///need rb.skip method
			rb.generic_read(img_header.frameheaderbuf,img_header.frameheader_size,0);

			byte[] pixelbuf=new byte[img_header.frame_size];

			long count=rb.generic_read(pixelbuf,img_header.frame_size,0);
//			e("read count: "+count);
			//print first 64 pixel values
//			for(int k=0;k<64;k++){e((pixelbuf[k] & 0xFF)+" ");}e("");

			BufferedImage bi=null;
			if(img_header.channel_count==1)
			{
				if(img_header.bytes_per_channel==1)
				{
					bi=_8itGrayToBuffered(pixelbuf,img_header.w,img_header.h);
				}
				else if(img_header.bytes_per_channel==2)
				{
					bi=_16bitGrayToBuffered(pixelbuf,img_header.w,img_header.h);
				}
				else
				{}///
			}
			else if(img_header.channel_count==3)
			{
				if(img_header.bytes_per_channel==1)
				{
					bi=_8bitRGBToBuffered(pixelbuf,img_header.w,img_header.h);
				}
				else{}///

			}
			else{}///
/*
			FileOutputStream fos=new FileOutputStream(
			new File("/tmp/prefix"+System.currentTimeMillis()+".dump"));
			fos.write(pixelbuf,0,img_header.frame_size);
			System.exit(0);

			ImageIO.write(bi,"png"
				,new File("/tmp/prefix"+System.currentTimeMillis()+".png"));

			ImageIO.write(bi,"jpg"
				,new File("/tmp/prefix"+System.currentTimeMillis()+".jpg"));
*/
			if(bi!=null)
			{
				ip.setImage(bi);
				ip.repaint();
			}
		}//end rb.can_read()>=frameheader_size+img_header.frame_size 
//		e("can read after read: "+rb.can_read());
		try{Thread.sleep(1);}catch(Exception e){}
	}//end while true
	}//end run()
}//end class PixelPipeFrameReadThread
}//end class RBPixelPipe
//EOF
