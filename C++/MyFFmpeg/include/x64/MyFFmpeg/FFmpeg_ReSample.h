#ifndef FFMPEG_RESAMPLE_H
#define FFMPEG_RESAMPLE_H

#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;



class FFmpeg_ReSample
{
private:
    /* data */
public:
    FFmpeg_ReSample(/* args */);
    ~FFmpeg_ReSample();
    //输出参数和输入参数一致除了采样格式，输出为s16，会释放para
    virtual bool Open(AVCodecParameters *para,bool bClearPara = true);
    virtual void Close();

    //返回重采样后大小,不管成功与否都释放掉indata空间
    virtual int Resample(AVFrame *indata, unsigned char *data);
protected:
    std::mutex m_mutex;
    SwrContext *m_SwrContext;
};
#endif // FFRESAMPLE_H
