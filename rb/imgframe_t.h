//tb/1604

///gcc -o imgframe_t imgframe_t.c -Wunknown-pragmas

#ifndef _IMGFRAME_T_H
#define _IMGFRAME_T_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static const char IMG_MAGIC[8]={'i','m','g','f','0','0','0','\0'};

//=============================================================================
//-Wunknown-pragmas
#pragma pack(push)
#pragma pack(1)
typedef struct
{
	char magic[8];			//8
	int pixel_data_size_bytes;	//+4 =12
	uint64_t frame_number;		//+8 =20
	int width;			//+4 =24
	int height;			//+4 =28
	int channel_count;		//+4 =32
	int bytes_per_channel;		//+4 =36
	int stream_number;		//+4 =40
	float fps;			//+4 =44
	uint64_t millis_since_epoch;	//+8 =52
}
imgframe_t;
#pragma pack(pop)

//=============================================================================
static inline void imgframe_set_dim(imgframe_t *img, int w, int h)
{
	img->width=w;
	img->height=h;
}

//=============================================================================
static inline imgframe_t *imgframe_new(int size, int w, int h, int channel_count, int bytes_per_channel, int stream_number, float fps)
{
	imgframe_t *img;
	img=(imgframe_t*)malloc(sizeof(imgframe_t));
	strncpy(img->magic, IMG_MAGIC, 8);
	img->pixel_data_size_bytes=size;
	img->frame_number=0;
	imgframe_set_dim(img,w,h);
	img->channel_count=channel_count;
	img->bytes_per_channel=bytes_per_channel;
	img->stream_number=stream_number;
	img->fps=fps;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	img->millis_since_epoch=
		(unsigned long long)(tv.tv_sec) * 1000 +
		(unsigned long long)(tv.tv_usec) / 1000;

//	fprintf(stderr,"%" PRId64 "\n", img->millis_since_epoch);
	return img;
}

#ifdef __cplusplus
}
#endif

#endif //header guard

//eof

