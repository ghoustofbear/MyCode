/**
 * @file      play_stream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-13 16:58:03
 *
 * @note
 *  common stream play defines
 */
#ifndef __PLAY_STREAM_H__
#define __PLAY_STREAM_H__

#include "tvector.h"
#include "tqueue.h"
#include "tevqueue.h"
#include "vdeclistener.h"
#include "renderlistener.h"
#include "adeclistener.h"
#include "thread.h"
#include "c2play.h"

class VideoDecoder;
class AudioDecoder;
class Render;
class AudioTrack;

typedef tevqueue<string, struct snap_s> SnapQueue;

class StreamPlay : public VDecListener, public ADecListener, public RenderListener
{
protected:
    enum
    {
        MIN_THRESHOLD,
        MAX_THRESHOLD,
    };

public:
    enum
    {
        T_VDEC,
        T_ADEC,
    };

    StreamPlay(int type, int mode);
    virtual ~StreamPlay();

    int                type();

    int                addWnd(HWND wnd, int rtype);
    int                delWnd(HWND wnd);
    HWND               wnd(int idx);
    int                changeWnnSize(HWND wnd, int width, int height);
    int                changeWnnPos(HWND wnd, int left, int top);

    int                capture(const string path, int type);
    int                captureRGB(void *buf, int &size, int &w, int &h);
    int                captureYUV(void *buf, int &size, int &w, int &h);

    int                openSound();
    void               closeSound();
    int                setVolume(int vol);

    void               setMinThreshold(uint32_t min, c2_threshold_cb threshold_cb, void *userdata);
    void               setMaxThreshold(uint32_t max, c2_threshold_cb threshold_cb, void *userdata);

    int                remainFrame();
    int                resetFrame();

    int                currentTime(datetime_s &time);
    datetime_s         u64Time2DataTime(uint64_t t);

    void               setDrawCB(HWND wnd, c2_draw_cb draw_cb, void *userdata);
    void               setDecodeCB(c2_decode_cb decode_cb, void *userdata);

    virtual int        start(c2_status_cb statusCB, void *userData);
    virtual int        stop();
    virtual int        inputData(void *data, int size, bool block);

    virtual int        pause(bool pause);
    virtual int        fast(int &speed);
    virtual int        slow(int &speed);

protected:
    virtual void       onYUV(void *data, int size, int w, int h);
    virtual void       onRGB(void *data, int size, int w, int h);
    virtual void       onHDC(HDC hdc, HWND wnd, int w, int h);
    virtual void       onADPCM(void *data, int size);

protected:
    virtual void       vdecloop();
    virtual void       adecloop();

    int                setVCodec(uint8_t encode, uint32_t w, uint32_t h, uint32_t fps);
    int                setACodec(uint8_t encode, uint32_t sps, uint8_t channels, uint8_t bps);

    static  void     * threadProc(ThreadInfo *);

private:
    int                m_type;                             /**< reference STREAM_TYPE_E */

    c2_status_cb       m_statusCB;
    void             * m_statusData;

    volatile bool      m_snap;
    int                m_snapType;                         /**< 0:bmp 1:jpeg 2:YUV */
    SnapQueue          m_snapQu;

protected:
    int                m_mode;                             /**< 0-pts 1-fps */

    VideoDecoder     * m_vdec;
    AudioDecoder     * m_adec;

    Thread             m_vdectrd;
    Thread             m_adectrd;

    tvector<Render *>  m_renders;
    tqueue<uint8_t *>  m_frameQu;

    AudioTrack       * m_track;

    int                m_speed;                            /**< -4~4 */

    int                m_minThreshold;                     /**< 0~100 */
    int                m_maxThreshold;                     /**< 100~5000 */
    c2_threshold_cb    m_minThresholdCB;                   /**< minimun threshold callback */
    c2_threshold_cb    m_maxThresholdCB;                   /**< maximun threshold callback */
    void             * m_minThresholdData;
    void             * m_maxThresholdData;

    uint64_t           m_time;                             /**< current play time */
    bool               m_iflag;                            /**< need i frame */

    c2_draw_cb         m_drawCB;
    void             * m_drawData;
    c2_decode_cb       m_decodeCB;
    void             * m_decodeData;
};

struct snap_s
{
    int                width;
    int                height;
    uint8_t          * buf;
    uint32_t           size;
};

#endif
