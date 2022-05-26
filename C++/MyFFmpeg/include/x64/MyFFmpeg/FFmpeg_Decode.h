#ifndef FFMPEG_DECODE_H
#define FFMPEG_DECODE_H
#include<list>
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
extern void FFFreePacket(AVPacket **pkt);
extern void FFFreeFrame(AVFrame **frame);

class FFmpeg_Decode
{
private:
    /* data */
public:
    FFmpeg_Decode(/* args */);
    ~FFmpeg_Decode();
    //打开解码器,不管成功与否并释放para空间
    virtual bool Open(AVCodecParameters *para, bool bClearPara = true);

    //发送到解码线程,不管成功与否并释放pkt空间（对象和媒体内容
    virtual bool Send(AVPacket *pkt);
    virtual bool FreePkt(AVPacket *pkt);

    //获取解码后数据，一次send可能需要Recv，最后获取缓冲中的数据Send NULL 再Recv多次
    //每次复制一份，有调用者释放 av_frame_free
    virtual AVFrame *Recv();

    long long getPts(){ return m_pts; }
    






    //void setPts(long long pts){ this->pts = pts; }
    AVCodecContext * getAVCodecContext(){return m_AVCodecContext;}
    virtual void Close();
    virtual void Clear();






     virtual void Push(AVPacket *pkt);
    //清理队列



    //	取一帧数据，并出栈，如果没有返回NULL
    virtual AVPacket* Pop();
protected:
    AVCodecContext *m_AVCodecContext;
    std::mutex m_mutex;
    std::list <AVPacket *> m_packs;
    //当前解码到的pts
    long long m_pts = 0;
    bool m_isAudio = false;
        //最大队列
    int m_maxList = 100;
    bool m_isExit = false;
};


#endif