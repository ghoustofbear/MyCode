/**
 * @file      gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-19 16:28:25
 *
 * @note
 *  gu.h defines
 */

#ifndef __GU_H__
#define __GU_H__
#include "tlist.h"

#include "frame.h"

class StreamListener;
class Pu;
class OnUiUpdate;

/**
 * Gu
 */
class Gu : public tlist<StreamListener *>
{
public:

    friend class Pu;

    Gu(Pu *pu, int channelNo, string subHost, int subPort, int st, int ps);
    Gu(Pu *pu, string addrcode, int st, int ps);
    virtual ~Gu();

    string             addrcode();
    string             name();
    string             puname();
    string             puip();
    double             bps();
    int                status();
    int                streamType();
    int                streamProtoType();
    string             guid();

    void               addStreamListener(StreamListener *listener);
    void               delStreamListener(StreamListener *listener);

    void               setUi(OnUiUpdate *ui);
    void               updateUi();

    virtual int        start(int offset=0) = 0;
    virtual void       stop()  = 0;
    virtual int        reset(){ return 0; }

    virtual void       onTimer();

protected:
    void               notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag = TAG_STD);
    void               notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    void               notifyPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I');

    void               notifyStatus(int code);

    void               traffic(uint32_t size);

    int				   getH264Resolution(uint8_t *pu8Data, int s32Len, int &s32Height, int &s32Width);

    int                getH265Resolution(uint8_t* pu8Data, int s32Len, int &s32Height, int &s32Width);

private:
    uint32_t           Ue(unsigned char *pu8Data, uint32_t u32Len, uint32_t &u32StartBit);
    int				   Se(unsigned char *pu8Data, uint32_t u32Len, uint32_t &u32StartBit);
    uint32_t     	   U(uint32_t u32BitCount, uint8_t *pu8Data, uint32_t &u32StartBit);


protected:
    Pu               * m_pu;
    int                m_status;

    string             m_addrcode;                         /**< as id */
    string             m_name;

    int                m_channelNo;
    string             m_subHost;
    int                m_subPort;

    uint64_t           m_tmcnt;                            /**< timer driver count */
    uint64_t           m_lastpts;

    uint64_t           m_bpspts;
    uint64_t           m_sesspts;

    int                m_streamProtoType;                  /**< 0:private stream  1:stander stream */
    int                m_streamType;                       /**< 0:main stream     1:sub stream */
    int                m_s32Width;                  
    int                m_s32Height;                 

private:
    uint64_t           m_transSize;
    OnUiUpdate       * m_ui;
};

/**
 * RealGu
 */
class RealGu : public Gu
{
public:
    RealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    RealGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~RealGu();

    bool               isSame(int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    bool               isSame(string addrcode, int streamType, int streamPotoType);
	bool			 isSame(string addrcode, int streamType);

protected:

};

/**
 * RecordGu
 */
class RecordGu : public Gu
{
public:
    RecordGu(Pu *pu, string filename, string subHost, int subPort, int streamProtoType, int mode);
	RecordGu(Pu *pu, string filename, string addrcode, int streamProtoType, int mode);

	RecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamProtoType, int mode);
	RecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamProtoType, int mode);

	RecordGu(Pu *pu, string fileurl, int streamProtoType, int mode = 0);
    virtual ~RecordGu();

    bool               isSame(string filename, string subHost, int subPort);
    bool               isSame(string filename, string addrcode);
    bool               isSame(string fileurl);
    bool               isSame(StreamListener *listener);
    

public:
    virtual int        play(string scale, string range);

    virtual int        pause()     = 0;
    virtual int        continuex() = 0;

    virtual int        startRepos()           = 0;
    virtual int        setPos(int pos)        = 0;
    virtual int        setTime(datetime_s &t) = 0;
    virtual int        insertRecordLabel(string name){ return 0; }
    virtual int        playFrame(){ return 0; }
    virtual int        playNormal() { return 0; }
    virtual int        playReverse(datetime_s &t){ return 0; }
    virtual int        playFoward(datetime_s &t){ return 0; }

    virtual void       onTimer();

    bool               m_pause;
    
    virtual void       setReverse(){ m_bReverse = true; }
    virtual bool       getReverse(){ return m_bReverse; }

protected:
    bool               m_byname;
    string             m_filename;
    string             m_fileurl;
    int                m_type;
    datetime_s         m_start;
    datetime_s         m_end;    
    bool               m_repos;
    int                m_interval;
    uint64_t           m_prevTime;
    bool               m_bReverse;
	int                m_download; //0 - playback  1 - download
    int                m_speed; //-4:1/16  -3:1/8 -2:1/4 -1:1/2 0:1 1:2 2:4 3:8 4:16
};

class HEVCParser
{
public:
    HEVCParser() : m_data(NULL), m_len(0), m_idx(0), m_bits(0), m_byte(0), m_zeros(0) {}
    HEVCParser(void *data, int len) { init(data, len); }
    
    int  getH265Resolution(int &width, int &height);

private:
    unsigned char GetByte();
    uint32_t GetBit();
    uint32_t GetWord(int bits);
    uint32_t GetUE();
    int32_t  GetSE();
    void     init(void *data, int len);

private:
    char*			m_data;
    int				m_len;
    int				m_idx;
    int				m_bits;
    uint8_t         m_byte;
    int				m_zeros;
};

#endif
