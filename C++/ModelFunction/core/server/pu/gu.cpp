#include "gu.h"
#include "pu.h"
#include "session.h"
#include "onuiupdate.h"
#include "json/json.h"

#include <sstream>
#include <string>

#define DELAY_CLOSE_TIME      (30)
/**
 * Gu
 */
Gu::Gu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType)
{
    m_status    = StreamListener::S_SOK;
    m_transSize = 0;
    m_lastpts   = sys->currentTimeMillis();
    m_bpspts    = m_lastpts;
    m_sesspts   = m_lastpts;
    m_tmcnt     = 0;
    m_ui        = NULL;

    m_pu        = pu;
    m_channelNo = channelNo;
    m_subHost   = subHost;
    m_subPort   = subPort;
    m_streamProtoType = streamPotoType;
    m_streamType = streamType;

    //if (channelNo < m_pu->channels())
    //{
        m_addrcode = m_pu->gucode(channelNo);
        m_name     = m_pu->guname(channelNo);
    //}

    if (m_pu->protocol() == Pu::PROTO_TYPE_UT)
    {
        m_name = "utLmtStream";
    }

    m_s32Width = 0;
    m_s32Height = 0;
}

Gu::Gu(Pu *pu, string addrcode, int streamType, int streamPotoType)
{
    m_status    = StreamListener::S_SOK;
    m_transSize = 0;
    m_lastpts   = sys->currentTimeMillis();
    m_bpspts    = m_lastpts;
    m_sesspts   = m_lastpts;
    m_tmcnt     = 0;
    m_ui        = NULL;

    m_pu        = pu;
    m_addrcode  = addrcode;
    m_streamProtoType = streamPotoType;
    m_streamType = streamType;
    m_channelNo = pu->guchannel(addrcode);
    m_name      = pu->guname(addrcode);
    if ("" == m_name)
    {
        m_name = "gwbstream";
    }
    m_s32Width = 0;
    m_s32Height = 0;
}

Gu::~Gu()
{
    notifyStatus(StreamListener::S_GU_CLOSED);

    /* TODO: waiting all listener exit */

    clear();
}

string
Gu::addrcode()
{
    return m_addrcode;
}

string
Gu::name()
{
    return m_name;
}

string
Gu::puname()
{
    return m_pu->name();
}

string
Gu::puip()
{
    return m_pu->ip();
}

double
Gu::bps()
{
    uint64_t now    = sys->currentTimeMillis();
    uint64_t elapse = now - m_bpspts;
    if (elapse <= 0)
    {
        elapse = 1000;
    }

    double bitrate = (m_transSize >> 7) / (double)(elapse/1000) ;
    //double bitrate = (m_transSize >> 10) / (double)(elapse / 1000);

    m_transSize = 0;
    m_bpspts    = sys->currentTimeMillis();

    return bitrate;
}

int
Gu::status()
{
    return m_status;
}

int
Gu::streamType()
{
    return m_streamType;
}

int
Gu::streamProtoType()
{
    return m_streamProtoType;
}

string
Gu::guid()
{
    const void * address = static_cast<const void*>(this);
    std::stringstream ss;
    ss << address;
    return ss.str();

}

void
Gu::addStreamListener(StreamListener *listener)
{
    AutoLock locker(_mtx);
    list<StreamListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
Gu::delStreamListener(StreamListener *listener)
{
    AutoLock locker(_mtx);
    list<StreamListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }

    m_sesspts = sys->currentTimeMillis();
}

void
Gu::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
Gu::updateUi()
{
    if (m_ui && size() > 0)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        AutoLock locker(_mtx);

        for (list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
        {
            SHL_Json::Value item;
            item["peerip"]   = (*i)->peerip();
            item["peerport"] = (*i)->peerport();
            item["peertime"] = (*i)->peertime();
            item["stream"]   = (*i)->stream();

            value["peers"].append(item);
        }

        value["puip"]   = m_pu->ip();
        value["puname"] = m_pu->name();

        string json = SHL_Json::writeString(writer, value);
        m_ui->onUpdate(json, NULL, 0);
    }
}

void
Gu::notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onVideoStream(stream, size, encode, type, w, h, fps, pts, tag);
    }
}

void
Gu::notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onAudioStream(stream, size, encode, sps, channels, bps, pts);
    }
}

void
Gu::notifyPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onPrivStream(stream, size, encode, tag, type);
    }
    
}

void
Gu::notifyStatus(int code)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onStatus(code);
    }
}

void
Gu::traffic(uint32_t size)
{
    m_transSize += size;
}

void
Gu::onTimer()
{
    if (!m_pu->online())
    {
        m_status = StreamListener::S_PU_OFFLINE;
        notifyStatus(StreamListener::S_PU_OFFLINE);
    }

    uint64_t pts = sys->currentTimeMillis();
    uint64_t dlta = pts - m_lastpts;

    if (dlta > 5 * 1000) //songhongliang 15->5
    {
        m_status = StreamListener::S_GU_OFFLINE;
        notifyStatus(StreamListener::S_GU_OFFLINE);

        /* log every 5 seconds */
        if (0 == (m_tmcnt * 5) % 1000)
        {
            LOGW_MSV("gu receice data timeout. pu: %s-%s-%s, gu: %s-%s",
                     m_pu->name().c_str(),
                     m_pu->addrcode().c_str(),
                     m_pu->host().c_str(),
                     m_name.c_str(),
                     m_addrcode.c_str());
        }
    }
    else
    {
        m_status = StreamListener::S_SOK;
    }

    {
        AutoLock locker(_mtx);
        /* delay close real stream */
        if (0 == size())
        {
            if ((pts - m_sesspts) > DELAY_CLOSE_TIME * 1000)
            {
                m_status = StreamListener::S_GU_CLOSED;
            }
        }
    }

    m_tmcnt++;
}

uint32_t Gu::Ue(unsigned char *pu8Data, uint32_t u32Len, uint32_t &u32StartBit)
{
    uint32_t u32ZeroNum = 0;
    while (u32StartBit < u32Len * 8)
    {
        if (pu8Data[u32StartBit / 8] &
            (0x80 >> (u32StartBit % 8)))
        {
            break;
        }

        u32ZeroNum++;
        u32StartBit++;
    }

    u32StartBit++;

    unsigned long dwRet = 0;
    for (uint32_t u32I = 0; u32I < u32ZeroNum; ++u32I)
    {
        dwRet <<= 1;
        if (pu8Data[u32StartBit / 8] &
            (0x80 >> (u32StartBit % 8)))
        {
            dwRet += 1;
        }
        u32StartBit++;
    }

    return (1 << u32ZeroNum) - 1 + dwRet;
}

#include <math.h>
int Gu::Se(unsigned char *pu8Data, uint32_t u32Len, uint32_t &u32StartBit)
{
    int s32Val = Ue(pu8Data, u32Len, u32StartBit);
    int s32Ret = ceil(s32Val / 2.0);

    if (s32Val % 2 == 0)
    {
        s32Ret = -s32Ret;
    }

    return s32Ret;
}

uint32_t Gu::U(uint32_t u32BitCount, unsigned char *pu8Data, uint32_t &u32StartBit)
{
    unsigned long dwRet = 0;

	for (uint32_t s32I = 0; s32I < u32BitCount; ++s32I)
    {
        dwRet <<= 1;
        if (pu8Data[u32StartBit / 8] & (0x80 >> (u32StartBit % 8)))
        {
            dwRet += 1;
        }

        u32StartBit++;
    }

    return dwRet;
}

int Gu::getH264Resolution(unsigned char *pu8Data, int s32Len, int &s32Height, int &s32Width)
{
    uint32_t u32StartBit = 0;
    int forb_zero_b = U(1, pu8Data, u32StartBit);
    int nal_ref_idc = U(2, pu8Data, u32StartBit);
    int nal_unit_type = U(5, pu8Data, u32StartBit);

    if (9 == nal_unit_type)
    {
        unsigned char  *p_buffer = pu8Data;

        int i_offset;
        int i_size = 0;

        for (i_offset = 0; i_offset + 2 < s32Len; i_offset++)
        {
            if (p_buffer[i_offset] == 0 && p_buffer[i_offset + 1] == 0 && p_buffer[i_offset + 2] == 1)
            {
                i_size = i_offset - (p_buffer[i_offset - 1] == 0 ? 1 : 0);
                break;
            }
        }

        pu8Data += (i_size + 4);
        u32StartBit = 0;
        forb_zero_b = U(1, pu8Data, u32StartBit);
        nal_ref_idc = U(2, pu8Data, u32StartBit);
        nal_unit_type = U(5, pu8Data, u32StartBit);
    }

    if (7 == nal_unit_type)
    {
        int profile_idc = U(8, pu8Data, u32StartBit);
        int constraint_set0_flag = U(1, pu8Data, u32StartBit);
        int constraint_set1_flag = U(1, pu8Data, u32StartBit);
        int constraint_set2_flag = U(1, pu8Data, u32StartBit);
        int constraint_set3_flag = U(1, pu8Data, u32StartBit);
        int reserved_zero_4bits = U(4, pu8Data, u32StartBit);
        int level_idc = U(8, pu8Data, u32StartBit);

        int seq_param_set_id = Ue(pu8Data, s32Len, u32StartBit);

        if (profile_idc == 100 || profile_idc == 110 ||
            profile_idc == 122 || profile_idc == 144)
        {
            int chroma_format_idc = Ue(pu8Data, s32Len, u32StartBit);
            if (chroma_format_idc == 3)
                int residual_colour_transform_flag = U(1, pu8Data, u32StartBit);
            int bit_depth_luma_minus8 = Ue(pu8Data, s32Len, u32StartBit);
            int bit_depth_chroma_minus8 = Ue(pu8Data, s32Len, u32StartBit);
            int qpprime_y_zero_flag = U(1, pu8Data, u32StartBit);
            int seq_scaling_matrix_flag = U(1, pu8Data, u32StartBit);

            int seq_scaling_list_preset_flag[8];
            if (seq_scaling_matrix_flag)
            {
                for (int s32I = 0; s32I < 8; ++s32I)
                {
                    seq_scaling_list_preset_flag[s32I] = U(1, pu8Data, u32StartBit);
                    if (seq_scaling_list_preset_flag[s32I])
                    {
                        int sizeOfScale = 16;
                        if (s32I > 6)
                        {
                            sizeOfScale = 64;
                        }

                        int lastScale = 8, nextScale = 8;
                        int deltaScale;

                        for (int jx = 0; jx < sizeOfScale; jx++)
                        {
                            if (nextScale != 0)
                            {
                                deltaScale = Se(pu8Data, s32Len, u32StartBit);
                                nextScale = (lastScale + deltaScale + 256) % 256;
                            }
                            if (nextScale == 0)
                            {
                                lastScale = lastScale;
                            }
                            else
                            {
                                lastScale = nextScale;
                            }
                        }
                    }
                }
            }
        }

        int log2_max_frame_num_minus4 = Ue(pu8Data, s32Len, u32StartBit);
        int pic_order_cnt_type = Ue(pu8Data, s32Len, u32StartBit);
        if (pic_order_cnt_type == 0)
        {
            int log2_max_pic_order_cnt_minus4 = Ue(pu8Data, s32Len, u32StartBit);
        }
        else if (pic_order_cnt_type == 1)
        {
            int delta_pic_order_flag = U(1, pu8Data, u32StartBit);
            int offset_for_non_ref_pic = Se(pu8Data, s32Len, u32StartBit);
            int offset_for_top_to_bottom = Se(pu8Data, s32Len, u32StartBit);
            int num_ref_frames_in_pic_cnt = Ue(pu8Data, s32Len, u32StartBit);

            int *offset_for_ref_frame = new int[num_ref_frames_in_pic_cnt];
            for (int s32I = 0; s32I < num_ref_frames_in_pic_cnt; s32I++)
            {
                offset_for_ref_frame[s32I] = Se(pu8Data, s32Len, u32StartBit);
            }

            delete[] offset_for_ref_frame;
        }

        int num_ref_frames = Ue(pu8Data, s32Len, u32StartBit);
        int gaps_in_frame_flag = U(1, pu8Data, u32StartBit);
        int pic_width_in_mbs_minus1 = Ue(pu8Data, s32Len, u32StartBit);
        int pic_height_in_map_units_minus1 = Ue(pu8Data, s32Len, u32StartBit);

        int frame_mbs_only_flag = U(1, pu8Data, u32StartBit);

        s32Width = (pic_width_in_mbs_minus1 + 1) * 16;
        s32Height = (pic_height_in_map_units_minus1 + 1) * 16;

        if (0 == frame_mbs_only_flag)
        {
            s32Height *= 2;
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

int Gu::getH265Resolution(uint8_t* pu8Data, int s32Len, int &s32Height, int &s32Width)
{
    char buf[800 * 1024] = { 0 };
    memcpy(buf, pu8Data, s32Len);
    char sps_buf[1024] = { 0 };
    int sps_len = 0;

    bool bFound = false;
    while (!bFound)
    {
        bool first = false;
        bool second = false;
        int  firstindex = 0;
        int  secondindex = 0;

        for (int index = 0; index < s32Len; ++index)
        {
            if (0x00 == buf[index] &&
                0x00 == buf[index + 1] &&
                0x00 == buf[index + 2] &&
                0x01 == buf[index + 3])
            {
                if (!first)
                {
                    first = true;
                    firstindex = index;
                }
                else
                {
                    second = true;
                    secondindex = index;

                    int dwBufSize = (secondindex - firstindex);
                    memcpy(sps_buf, buf + firstindex, dwBufSize);

                    int nalu_type = (sps_buf[4] & 0x7E) >> 1;
                    if (33 == nalu_type)
                    {
                        sps_len = dwBufSize;
                        bFound = true;
                        break;
                    }
                    else
                    {
                        index = index - 1;
                        first = false;
                        second = false;
                    }
                }
            }
        }
    }

    if (!bFound)
    {
        return -1;
    }

    HEVCParser stream(sps_buf+4, sps_len-4);
    return stream.getH265Resolution(s32Width, s32Height);
}

/**
 * RealGu
 */
RealGu::RealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType)
    : Gu(pu, channelNo, subHost, subPort, streamType, streamPotoType)
{

}

RealGu::RealGu(Pu *pu, string addrcode, int streamType, int streamPotoType)
    : Gu(pu, addrcode, streamType, streamPotoType)
{

}

RealGu::~RealGu()
{

}

bool
RealGu::isSame(int channelNo, string subHost, int subPort, int streamType, int streamPotoType)
{
    return m_channelNo  == channelNo &&
           m_subPort    == subPort &&
           m_streamType == streamType &&
           m_streamProtoType == streamPotoType &&
           (0 == m_subHost.compare(subHost));
}

bool
RealGu::isSame(string addrcode, int streamType, int streamPotoType)
{
    return (0 == m_addrcode.compare(addrcode)) &&
            m_streamProtoType == streamPotoType&&
            m_streamType == streamType;
}

bool
RealGu::isSame(string addrcode, int streamType)
{
	return (0 == m_addrcode.compare(addrcode)) &&
		m_streamType == streamType;
}

/**
 * RecordGu
 */
RecordGu::RecordGu(Pu *pu, string filename, string subHost, int subPort, int streamProtoType, int mode)
    : Gu(pu, 0, subHost, subPort, 0, streamProtoType)
{
    m_byname   = true;
    m_filename = filename;
    m_pause    = false;
    m_repos    = false;
    m_interval = 5;
    m_prevTime = time(NULL);
    m_bReverse = false;
	m_download = mode;
    
    m_prevTime = 0;
    m_speed = 0;
}

RecordGu::RecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamProtoType, int mode)
    : Gu(pu, channelNo, subHost, subPort, 0, streamProtoType)
{
    m_byname = false;
    m_type   = type;
    m_start  = start;
    m_end    = end;
    m_pause  = false;
    m_repos  = false;
    m_interval = 5;
    m_prevTime = time(NULL);
    m_bReverse = false;
	m_download = mode;

    m_prevTime = 0;
    m_speed = 0;
}

RecordGu::RecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamProtoType, int mode)
    : Gu(pu, addrcode, 0, streamProtoType)
{
    m_byname = false;
    m_type   = type;
    m_start  = start;
    m_end    = end;
    m_pause  = false;
    m_repos  = false;
    m_interval = 5;
    m_prevTime = time(NULL);
    m_bReverse = false;
	m_download = mode;

    m_prevTime = 0;
    m_speed = 0;
}

RecordGu::RecordGu(Pu *pu, string filename, string addrcode, int streamProtoType, int mode)
    : Gu(pu, addrcode, 0, streamProtoType)
{
    m_byname   = true;
    m_addrcode = addrcode;
    m_filename = filename;
    m_pause    = false;
    m_repos    = false;
    m_interval = 5;
    m_prevTime = time(NULL);
    m_bReverse = false;
	m_download = mode;

    m_prevTime = 0;
    m_speed = 0;
}

RecordGu::RecordGu(Pu *pu, string fileurl, int streamProtoType, int mode)
    : Gu(pu, 0, "", 0, 0, streamProtoType)
{
    m_byname   = true;
    m_fileurl  = fileurl;
    m_pause    = false;
    m_repos    = false;
    m_interval = 5;
    m_prevTime = time(NULL);
    m_bReverse = false;
	m_download = mode;

    m_prevTime = 0;
    m_speed = 0;
}

RecordGu::~RecordGu()
{

}

bool
RecordGu::isSame(string filename, string subHost, int subPort)
{
    return m_subPort == subPort &&
           (0 == m_filename.compare(filename)) &&
           (0 == m_subHost.compare(subHost));
}

bool
RecordGu::isSame(string filename, string addrcode)
{
    return (0 == m_filename.compare(filename)) &&
           (0 == m_addrcode.compare(addrcode));
}

bool
RecordGu::isSame(string fileurl)
{
    return (0 == m_fileurl.compare(fileurl));
}

bool
RecordGu::isSame(StreamListener *listener)
{
    AutoLock locker(_mtx);
    list<StreamListener *>::const_iterator i = find(begin(), end(), listener);

    return  i != end() ? true : false;
}

int
RecordGu::play(string scale, string range)
{
    return 0;
}

void
RecordGu::onTimer()
{
    Gu::onTimer();
    //{
    //    AutoLock locker(_mtx);
    //    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    //    {
    //        if (StreamListener::S_BUFF_FULL == (*i)->onBuffStatus())
    //        {
    //            //if (!m_pause)
    //            //{
    //            //    pause();
    //            //}

    //        }
    //        else if (StreamListener::S_BUFF_EMPTY == (*i)->onBuffStatus())
    //        {
    //            //if (m_pause)
    //            //{
    //            //    continuex();
    //            //}

    //        }
    //    }
    //}
}

void HEVCParser::init(void *data, int len)
{
    m_data = (char*)data; 
    m_len = len; 
    m_idx = 0; 
    m_bits = 0; 
    m_byte = 0; 
    m_zeros = 0;
}

unsigned char HEVCParser::GetByte()
{
    if (m_idx >= m_len)
    {
        return 0;
    }
    unsigned char b = m_data[m_idx++];
    if (0 == b)
    {
        m_zeros++;
        if ((m_idx < m_len) && (m_zeros == 2) && (m_data[m_idx] == 0x03))
        {
            m_idx++;
            m_zeros = 0;
        }
    }
    else
    {
        m_zeros = 0;
    }
    return b;
}

uint32_t HEVCParser::GetBit()
{
    if (m_bits == 0)
    {
        m_byte = GetByte();
        m_bits = 8;
    }
    m_bits--;
    return (m_byte >> m_bits) & 0x1;
}

uint32_t HEVCParser::GetWord(int bits)
{
    uint32_t u = 0;
    while (bits > 0)
    {
        u <<= 1;
        u |= GetBit();
        bits--;
    }
    return u;
}

uint32_t HEVCParser::GetUE()
{  
    int zeros = 0;
    while (m_idx < m_len && GetBit() == 0) zeros++;
    return GetWord(zeros) + ((1 << zeros) - 1);
}

int32_t HEVCParser::GetSE()
{  
    uint32_t UE = GetUE();
    bool positive = UE & 1;
    int32_t SE = (UE + 1) >> 1;
    if (!positive)
    {
        SE = -SE;
    }
    return SE;
}

int HEVCParser::getH265Resolution(int &width, int &height)
{
    // seq_parameter_set_rbsp()   
    GetWord(4);// sps_video_parameter_set_id   
    int sps_max_sub_layers_minus1 = GetWord(3); // "The value of sps_max_sub_layers_minus1 shall be in the range of 0 to 6, inclusive."   
    if (sps_max_sub_layers_minus1 > 6)
    {
        return false;
    }
    GetWord(1);// sps_temporal_id_nesting_flag   
    // profile_tier_level( sps_max_sub_layers_minus1 )   
    {
        GetWord(2);// general_profile_space   
        GetWord(1);// general_tier_flag   
        int profile = GetWord(5);// general_profile_idc   
        GetWord(32);// general_profile_compatibility_flag[32]   
        GetWord(1);// general_progressive_source_flag   
        GetWord(1);// general_interlaced_source_flag   
        GetWord(1);// general_non_packed_constraint_flag   
        GetWord(1);// general_frame_only_constraint_flag   
        GetWord(44);// general_reserved_zero_44bits   
        int level = GetWord(8);// general_level_idc   
        uint8_t sub_layer_profile_present_flag[6] = { 0 };
        uint8_t sub_layer_level_present_flag[6] = { 0 };
        for (int i = 0; i < sps_max_sub_layers_minus1; i++)
        {
            sub_layer_profile_present_flag[i] = GetWord(1);
            sub_layer_level_present_flag[i] = GetWord(1);
        }
        if (sps_max_sub_layers_minus1 > 0)
        {
            for (int i = sps_max_sub_layers_minus1; i < 8; i++)
            {
                uint8_t reserved_zero_2bits = GetWord(2);
            }
        }
        for (int i = 0; i < sps_max_sub_layers_minus1; i++)
        {
            if (sub_layer_profile_present_flag[i])
            {
                GetWord(2);// sub_layer_profile_space[i]   
                GetWord(1);// sub_layer_tier_flag[i]   
                GetWord(5);// sub_layer_profile_idc[i]   
                GetWord(32);// sub_layer_profile_compatibility_flag[i][32]   
                GetWord(1);// sub_layer_progressive_source_flag[i]   
                GetWord(1);// sub_layer_interlaced_source_flag[i]   
                GetWord(1);// sub_layer_non_packed_constraint_flag[i]   
                GetWord(1);// sub_layer_frame_only_constraint_flag[i]   
                GetWord(44);// sub_layer_reserved_zero_44bits[i]   
            }
            if (sub_layer_level_present_flag[i])
            {
                GetWord(8);// sub_layer_level_idc[i]   
            }
        }
    }
    uint32_t sps_seq_parameter_set_id = GetUE(); // "The  value  of sps_seq_parameter_set_id shall be in the range of 0 to 15, inclusive."   
    if (sps_seq_parameter_set_id > 15)
    {
        return false;
    }
    uint32_t chroma_format_idc = GetUE(); // "The value of chroma_format_idc shall be in the range of 0 to 3, inclusive."   
    if (sps_seq_parameter_set_id > 3)
    {
        return false;
    }
    if (chroma_format_idc == 3)
    {
        GetWord(1);// separate_colour_plane_flag   
    }
    width = GetUE(); // pic_width_in_luma_samples   
    height = GetUE(); // pic_height_in_luma_samples   
    if (GetWord(1))
    {// conformance_window_flag   
        GetUE();  // conf_win_left_offset   
        GetUE();  // conf_win_right_offset   
        GetUE();  // conf_win_top_offset   
        GetUE();  // conf_win_bottom_offset   
    }
    uint32_t bit_depth_luma_minus8 = GetUE();
    uint32_t bit_depth_chroma_minus8 = GetUE();
    if (bit_depth_luma_minus8 != bit_depth_chroma_minus8)
    {
        return false;
    }

    return true;
}
