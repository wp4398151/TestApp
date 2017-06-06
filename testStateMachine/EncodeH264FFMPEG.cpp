#include "stdafx.h"
#include "EncodeH264FFMPEG.h"

#ifdef USINGFFMPEG

EXTERN_C{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include <time.h>

CEncodeH264FFMPEG::CEncodeH264FFMPEG()
{

}

CEncodeH264FFMPEG::~CEncodeH264FFMPEG()
{

}

bool CEncodeH264FFMPEG::TestDecode()
{
	av_register_all();
	AVFormatContext *pFormatCtx = NULL;


	// url 为交错格式所以必须要全部读入流
	char url[] = "";
	if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0)
	{
		LOG(ERROR) << "avformat_open_input failed!";
		return false;
	}
	// Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		LOG(ERROR) << "avformat_open_input failed!";
		return false;
	}
	
	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, url, 0);

	int i;
	AVCodecContext *pCodecCtxOrig = NULL;
	AVCodecContext *pCodecCtx = NULL;

	// Find the first video stream
	//INT videoStream = -1;
	//for (INT i = 0; i<pFormatCtx->nb_streams; i++)
	//if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
	//	videoStream = i;
	//	break;
	//}
	//if (videoStream == -1)
	//	return -1; // Didn't find a video stream

}

#endif //USINGFFMPEG
