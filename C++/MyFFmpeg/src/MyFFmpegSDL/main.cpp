#include <stdio.h>
#define __STDC_CONSTANT_MACROS
#define SDL_INIT_TIMER          0x00000001u
#define SDL_INIT_AUDIO          0x00000010u
#define SDL_INIT_VIDEO          0x00000020u  

#define SDL_WINDOWPOS_UNDEFINED_MASK    0x1FFF0000u
#define SDL_WINDOWPOS_UNDEFINED_DISPLAY(X)  (SDL_WINDOWPOS_UNDEFINED_MASK|(X))
#define SDL_WINDOWPOS_UNDEFINED         SDL_WINDOWPOS_UNDEFINED_DISPLAY(0)
#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/error.h"
#include "libavutil/avutil.h"
#include"SDL2/SDL.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "libavutil/error.h"
#include <SDL2/SDL.h>
#ifdef __cplusplus
};
#endif
#endif
#include<iostream>

void showYUV(const char* url)
{
    avformat_network_init();
      int countFrame=0;
      int lost=0;
    AVFormatContext* m_AVFormatContext;
    AVCodecContext* m_AVCodecContext;
    int screen_w=0,screen_h=0;
    SDL_Window *m_sdlwindows;
    SDL_Renderer* m_sdlrender;
    SDL_Texture* m_sdltexture;
    SDL_Rect m_sdlrect;
    m_AVFormatContext=avformat_alloc_context();
    avformat_open_input(&m_AVFormatContext,url,NULL,NULL);
    avformat_find_stream_info(m_AVFormatContext,NULL);
    av_dump_format(m_AVFormatContext,0,url,0);
      
     AVCodec * m_AVCodec=avcodec_find_decoder(m_AVFormatContext->streams[0]->codecpar->codec_id);
    m_AVCodecContext=avcodec_alloc_context3(m_AVCodec);


    avcodec_parameters_to_context(m_AVCodecContext,m_AVFormatContext->streams[0]->codecpar);

//十六线程解码
    m_AVCodecContext->thread_count = 16;
    //初始化 一个视音频编解码器的AVCodecContext。
    int ret= avcodec_open2(m_AVCodecContext,m_AVCodec,NULL);
    if (ret != 0)
    {
        avcodec_free_context(&m_AVCodecContext);
        
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf));
        std::cout << "videoStream avcodec_open2 failed!" << buf <<std::endl;
        return ;

    }
    AVFrame* pFrame=av_frame_alloc();
    av_image_alloc(pFrame->data,
        pFrame->linesize,
        m_AVCodecContext->width,
        m_AVCodecContext->height,
        AV_PIX_FMT_YUV420P,
        1);
   
     AVFrame* pYUVFrame=av_frame_alloc();

    //  unsigned char* outbuf=(unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,m_AVCodecContext->width,m_AVCodecContext->height,1));

    //  av_image_fill_arrays(pYUVFrame->data,
    //                         pFrame->linesize,
    //                         outbuf,
    //                         m_AVCodecContext->pix_fmt,
    //                         m_AVCodecContext->width,
    //                         m_AVCodecContext->height,
    //                         1);
    
av_image_alloc(pYUVFrame->data, 
	        pFrame->linesize,
	        m_AVCodecContext->width,
            m_AVCodecContext->height,
	        AV_PIX_FMT_YUV420P, 
	        1);
     SwsContext *m_SwsContext=sws_alloc_context();
    m_SwsContext=sws_getContext(m_AVCodecContext->width,
                                m_AVCodecContext->height,
                                m_AVCodecContext->pix_fmt,
                               m_AVCodecContext->width,
                                m_AVCodecContext->height,
                                AV_PIX_FMT_YUV420P,
                                4,
                                NULL,
                                NULL,
                                NULL);
    AVPacket *m_AVPacket=av_packet_alloc();
    if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_TIMER|SDL_INIT_VIDEO))
    {

        	printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return ;
    }
    	screen_w = m_AVCodecContext->width;
	screen_h = m_AVCodecContext->height;
	//SDL 2.0 Support for multiple windows
	m_sdlwindows = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,
		SDL_WINDOW_OPENGL);

	if(!m_sdlwindows) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return ;
	}

	m_sdlrender = SDL_CreateRenderer(m_sdlwindows, -1, 0);  
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	m_sdltexture = SDL_CreateTexture(m_sdlrender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,m_AVCodecContext->width,m_AVCodecContext->height);  

	m_sdlrect.x=0;
	m_sdlrect.y=0;
	m_sdlrect.w=screen_w;
	m_sdlrect.h=screen_h;

	//SDL End----------------------
    while(av_read_frame(m_AVFormatContext,m_AVPacket)>=0)
    {
     
        int ret1=avcodec_send_packet(m_AVCodecContext,m_AVPacket);
        av_packet_unref(m_AVPacket);
          if (ret1 != 0)
        {
            continue;
        }

        if (m_AVPacket->stream_index == 0)
            {
                ret1 = avcodec_receive_frame(m_AVCodecContext, pFrame);
                while (ret1 >= 0)
                {
                    /* code */
                    countFrame++;
                    sws_scale(m_SwsContext,
                        pFrame->data,
                        pFrame->linesize,
                        0,
                        pFrame->height,
                        pYUVFrame->data,
                        pYUVFrame->linesize);

                    int	y_size = m_AVCodecContext->width * m_AVCodecContext->height;
           //SDL---------------------------
		
        SDL_UpdateYUVTexture(m_sdltexture, &m_sdlrect,
            pYUVFrame->data[0], pYUVFrame->linesize[0],
            pYUVFrame->data[1], pYUVFrame->linesize[1],
            pYUVFrame->data[2], pYUVFrame->linesize[2]);
		SDL_RenderClear( m_sdlrender );  
		SDL_RenderCopy( m_sdlrender, m_sdltexture,  NULL, &m_sdlrect);  
		SDL_RenderPresent( m_sdlrender );  
		//SDL End-----------------------
		//Delay 40ms
		SDL_Delay(40);

                }
                
                
                    lost++;
                    printf("failed to decode %d frame!\n", lost);
                

                

            }
    }
}




int main(int argc, char *argv[])
{
    /* code */
    char filepath[] = "H:\\Mycodes\\C++\\MyFFmpeg\\src\\MyFFmpeg\\bigbuckbunny_480x272.h265";
    showYUV(filepath);
    std::cout << "hello world" << std::endl;
    return 0;
}
