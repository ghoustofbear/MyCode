#ifndef FFMPEG_SWSCALE_H
#define FFMPEG_SWSCALE_H

struct AVFrame;
struct SwsContext;
#include<mutex>

class FFmpeg_Swscale
{
private:
    /* data */
    SwsContext *m_SwsContext;
    std::mutex m_mutex;
public:
    FFmpeg_Swscale(/* args */);
    ~FFmpeg_Swscale();
    virtual bool ToRGB(const AVFrame *yuv, char *out, int outwidth, int outheight);
    virtual bool ToYUV(const AVFrame *frame,char *out, int outwidth, int outheight);
};
#endif

