#ifndef FFMPEG_DEMUX_H
#define FFMPEG_DEMUX_H
#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
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
#ifdef __cplusplus
};
#endif
#endif
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
//该函数主要提供解封装功能
class FFmpeg_Demux
{
private:
    /* data */
public:
    FFmpeg_Demux(/* args */);
    ~FFmpeg_Demux();
         //打开每天文件，或者流媒体rtmp 、http 、rtsp
    virtual bool Open(const char *url);

    //空间需要调用者释放，释放AVPacket对象空间和，数据空间 av_packet_free
    virtual AVPacket *Read();

    //只读视频，音频丢弃空间释放
    virtual AVPacket *ReadVideo();

    //获取视频参数 返回空间需要清理 avcodec_parameters_free
    virtual AVCodecParameters *CopyVPara();

    //获取音频参数 返回空间需要清理 avcodec_parameters_free
    virtual AVCodecParameters *CopyAPara();

    //seek 位置pos 0.0~1.0
    virtual bool Seek(double pos);

    //清空读取缓存
    virtual void Clear();
    virtual void Close();
   
  
    virtual bool IsAudio(AVPacket *avpacket);

    int getWidth(){ return m_width; }
    int getHeight(){ return m_height; }
    int getTolalMs(){ return m_totalMs; }
    int getSampleRate(){ return m_sampleRate; }
    int getChannels(){ return m_channels; }
    double getVideoFps(){return m_videoFps;}
    AVFormatContext * getAVFormatContext(){return m_AVFormatContex;}
   protected:
    std::mutex  m_mutex;
     //解封装上下文
    AVFormatContext *m_AVFormatContex;
    //音视频索引，读取时区分音视频
    int m_VideoStream = 0;
    int m_AudioStream = 1;

    //总时长 毫秒
    int m_totalMs = 0;
    int m_width = 0;
    int m_height = 0;
    int m_sampleRate = 0;
    int m_channels = 0;
    double m_videoFps=0;

};


#endif
