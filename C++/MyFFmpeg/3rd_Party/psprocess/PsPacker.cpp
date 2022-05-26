#include "PsPacker.h"
PsPacker::PsPacker()
{
    do
    {
        workspace = (uint8_t*)malloc(1024 * 1024);
    } while (NULL == workspace);
    memset(workspace, 0, 1024 * 1024);
    m_pts = sys->currentTimeMillis();
    n_pts = m_pts;
}

PsPacker::~PsPacker()
{
    if (NULL != workspace)
    {
        free(workspace);
        workspace = NULL;
    }
}

int
PsPacker::getNextNALU(uint8_t*pData, int size, int type)
{
    if (size < 5 || NULL == pData)
    {
        return -1;
    }
    for (int i = 0; i < size - 4; i++)
    {
        if (pData[i] == 0 && pData[i + 1] == 0 && pData[i + 2] == 0 && pData[i + 3] == 1 && (pData[i + 4] & 31) == type)
        {
            return i;
        }
    }
    return -1;
}
/***
*@remark:   ps头的封装,里面的具体数据的填写已经占位，可以参考标准
*@param :   pData  [in] 填充ps头数据的地址
*           s64Src [in] 时间戳
*@return:   0 success, others failed
*/
int
PsPacker::gb28181_make_ps_header(uint8_t *pData, uint64_t s64Scr)
{
    uint64_t lScrExt = (s64Scr) % 100;
    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PS_HDR_LEN);
    bits_write(&bitsBuffer, 32, 0x000001BA);      /*start codes*/
    bits_write(&bitsBuffer, 2, 1);           /*marker bits '01b'*/
    bits_write(&bitsBuffer, 3, (s64Scr >> 30) & 0x07);     /*System clock [32..30]*/
    bits_write(&bitsBuffer, 1, 1);           /*marker bit*/
    bits_write(&bitsBuffer, 15, (s64Scr >> 15) & 0x7FFF);   /*System clock [29..15]*/
    bits_write(&bitsBuffer, 1, 1);           /*marker bit*/
    bits_write(&bitsBuffer, 15, s64Scr & 0x7fff);         /*System clock [14..0]*/
    bits_write(&bitsBuffer, 1, 1);           /*marker bit*/
    bits_write(&bitsBuffer, 9, lScrExt & 0x01ff);    /*System clock ext*/
    bits_write(&bitsBuffer, 1, 1);           /*marker bit*/
    bits_write(&bitsBuffer, 22, 41994);    /*bit rate(n units of 50 bytes per second.)*/
    bits_write(&bitsBuffer, 2, 3);           /*marker bits '11'*/
    bits_write(&bitsBuffer, 5, 0x1f);          /*reserved(reserved for future use)*/
    bits_write(&bitsBuffer, 3, 0);           /*stuffing length*/
    return 0;
}
/***
*@remark:   sys头的封装,里面的具体数据的填写已经占位，可以参考标准
*@param :   pData  [in] 填充ps头数据的地址
*@return:   0 success, others failed
*/
int
PsPacker::gb28181_make_sys_header(uint8_t *pData)
{

    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = SYS_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, SYS_HDR_LEN);
    /*system header*/
    bits_write(&bitsBuffer, 32, 0x000001BB); /*start code*/
    bits_write(&bitsBuffer, 16, SYS_HDR_LEN - 6);/*header_length 表示次字节后面的长度，后面的相关头也是次意思*/
    bits_write(&bitsBuffer, 1, 1);            /*marker_bit*/
    bits_write(&bitsBuffer, 22, 50000);    /*rate_bound*/
    bits_write(&bitsBuffer, 1, 1);            /*marker_bit*/
    bits_write(&bitsBuffer, 6, 1);            /*audio_bound*/
    bits_write(&bitsBuffer, 1, 0);            /*fixed_flag */
    bits_write(&bitsBuffer, 1, 1);          /*CSPS_flag */
    bits_write(&bitsBuffer, 1, 1);          /*system_audio_lock_flag*/
    bits_write(&bitsBuffer, 1, 1);          /*system_video_lock_flag*/
    bits_write(&bitsBuffer, 1, 1);          /*marker_bit*/
    bits_write(&bitsBuffer, 5, 1);          /*video_bound*/
    bits_write(&bitsBuffer, 1, 0);          /*dif from mpeg1*/
    bits_write(&bitsBuffer, 7, 0x7F);       /*reserver*/
    /*audio stream bound*/
    bits_write(&bitsBuffer, 8, 0xC0);         /*stream_id*/
    bits_write(&bitsBuffer, 2, 3);          /*marker_bit */
    bits_write(&bitsBuffer, 1, 0);            /*PSTD_buffer_bound_scale*/
    bits_write(&bitsBuffer, 13, 512);          /*PSTD_buffer_size_bound*/
    /*video stream bound*/
    bits_write(&bitsBuffer, 8, 0xE0);         /*stream_id*/
    bits_write(&bitsBuffer, 2, 3);          /*marker_bit */
    bits_write(&bitsBuffer, 1, 1);          /*PSTD_buffer_bound_scale*/
    bits_write(&bitsBuffer, 13, 2048);       /*PSTD_buffer_size_bound*/
    return 0;
}
/***
*@remark:   psm头的封装,里面的具体数据的填写已经占位，可以参考标准
*@param :   pData  [in] 填充ps头数据的地址
*@return:   0 success, others failed
*/
int
PsPacker::gb28181_make_psm_header(uint8_t *pData)
{

    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PSM_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (unsigned char *)(pData);
    memset(bitsBuffer.p_data, 0, PSM_HDR_LEN);
    bits_write(&bitsBuffer, 24, 0x000001); /*start code*/
    bits_write(&bitsBuffer, 8, 0xBC);   /*map stream id*/
    bits_write(&bitsBuffer, 16, 18);     /*program stream map length 次字节长度之和*/

    bits_write(&bitsBuffer, 1, 1);      /*current next indicator 值为1时当前有效，值为0时下一个有效 */
    bits_write(&bitsBuffer, 2, 3);      /*reserved*/
    bits_write(&bitsBuffer, 5, 0);      /*program stream map version 每当节目流映射定义改变时，版本号增加*/
    bits_write(&bitsBuffer, 7, 0x7F);   /*reserved */
    bits_write(&bitsBuffer, 1, 1);      /*marker bit */

    bits_write(&bitsBuffer, 16, 0);      /*program stream info length 紧随此字段描述符的总长*/
    bits_write(&bitsBuffer, 16, 8);     /*elementary stream map length 基本流映射总长*/

    /*video 不可或缺*/
    bits_write(&bitsBuffer, 8, 0x1B);       /*stream_type h264 流编码类型*/
    bits_write(&bitsBuffer, 8, 0xE0);   /*elementary_stream_id指示存储此基本流的PES包的PES包头内stream_id字段的赋值*/
    bits_write(&bitsBuffer, 16, 0);     /*elementary_stream_info_length 紧随此字段描述符的总长*/

    /*audio 亦不可或缺*/
    bits_write(&bitsBuffer, 8, 0x90);       /*stream_type G711*/
    bits_write(&bitsBuffer, 8, 0xC0);   /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);     /*elementary_stream_info_length 紧随此字段描述符的总长*/


    /*crc (2e b9 0f 3d)*/
    bits_write(&bitsBuffer, 8, 0x45);   /*crc (24~31) bits*/
    bits_write(&bitsBuffer, 8, 0xBD);   /*crc (16~23) bits*/
    bits_write(&bitsBuffer, 8, 0xDC);   /*crc (8~15) bits*/
    bits_write(&bitsBuffer, 8, 0xF4);   /*crc (0~7) bits*/
    return 0;
}
/***
*@remark:   pes头的封装,里面的具体数据的填写已经占位，可以参考标准
*@param :   pData      [in] 填充ps头数据的地址
*           stream_id  [in] 码流类型
*           paylaod_len[in] 负载长度
*           pts        [in] 时间戳
*           dts        [in]
*@return:   0 success, others failed
*/

int
PsPacker::gb28181_make_pes_header(uint8_t *pData, int stream_id, int payload_len, uint64_t pts, uint64_t dts)
{

    bits_buffer_s   bitsBuffer;
    bitsBuffer.i_size = PES_HDR_LEN;
    bitsBuffer.i_data = 0;
    bitsBuffer.i_mask = 0x80;
    bitsBuffer.p_data = (uint8_t *)(pData);
    memset(bitsBuffer.p_data, 0, PES_HDR_LEN);
    /*system header*/
    bits_write(&bitsBuffer, 24, 0x000001);  /*start code*/
    bits_write(&bitsBuffer, 8, (stream_id)); /*streamID*/
    bits_write(&bitsBuffer, 16, (payload_len)+13);  /*packet_len*/ //负载和次字节长度之和

    bits_write(&bitsBuffer, 2, 2);    /*'10'*/
    bits_write(&bitsBuffer, 2, 0);    /*scrambling_control*/
    bits_write(&bitsBuffer, 1, 1);    /*priority*/
    bits_write(&bitsBuffer, 1, 0);    /*data_alignment_indicator*/
    bits_write(&bitsBuffer, 1, 0);    /*copyright*/
    bits_write(&bitsBuffer, 1, 0);    /*original_or_copy*/
    bits_write(&bitsBuffer, 1, 1);    /*PTS_flag*/
    bits_write(&bitsBuffer, 1, 1);    /*DTS_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*ESCR_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*ES_rate_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*DSM_trick_mode_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*additional_copy_info_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*PES_CRC_flag*/
    bits_write(&bitsBuffer, 1, 0);    /*PES_extension_flag*/

    bits_write(&bitsBuffer, 8, 10);    /*header_data_length*/
    /*PTS*/
    bits_write(&bitsBuffer, 4, 2);                    /*'0010'*/
    bits_write(&bitsBuffer, 3, ((pts) >> 30) & 0x07);     /*PTS[32..30]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, ((pts) >> 15) & 0x7FFF);    /*PTS[29..15]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, (pts)& 0x7FFF);          /*PTS[14..0]*/
    bits_write(&bitsBuffer, 1, 1);

    /*DTS*/
    bits_write(&bitsBuffer, 4, 2);                    /*'0010'*/
    bits_write(&bitsBuffer, 3, ((dts) >> 30) & 0x07);     /*PTS[32..30]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, ((dts) >> 15) & 0x7FFF);    /*PTS[29..15]*/
    bits_write(&bitsBuffer, 1, 1);
    bits_write(&bitsBuffer, 15, (dts)& 0x7FFF);          /*PTS[14..0]*/
    bits_write(&bitsBuffer, 1, 1);
    return 0;
}
/***
*@remark:   H264/G711A打包成PS流
*@param :   data       [in] H264视频流/G711A音频流
*           size       [in] 长度
*           type       [in] 类型
*           pts        [in] 时间戳
*           pdata      [out]封装的PS流
*           psize      [out]PS流长度
*@return:   0 success, others failed
*/
int
PsPacker::pack(uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime)
{
    if (NULL == workspace || NULL == data)
    {
        return -1;
    }
    if (size >= 1024 * 1024 || size <= 0)//判断数据是否大于工作空间的最大长度
    {
        return -1;
    }
    int FrameLen = 0;
    switch (type)
    {
    case NALU_TYPE_SLICE:   //P帧
    {
               if (0 == m_doubleField || 0 == flag)//单场或者双场左场
               {
                   index = 0;
                   gb28181_make_ps_header(workspace + index, pts);
                   index += PS_HDR_LEN;
               }
               while (size > 0)            //若需要拆包，则循环
               {
                   FrameLen = (size > PS_PES_PAYLOAD_SIZE) ? PS_PES_PAYLOAD_SIZE : size;
                   gb28181_make_pes_header(workspace + index, 0XE0, FrameLen, pts, pts);
                   index += PES_HDR_LEN;
                   memcpy(workspace + index, data, FrameLen);
                   index += FrameLen;
                   size -= FrameLen;
                   data += FrameLen;
               }
               m_pts += 3600;
               if (0 == m_doubleField || 1 == flag)//单场或者双场右场
               {
                   flag = 0;
                   break;
               }
               else//双场左场
               {
                   flag = 1;
                   return 0;
               }
    }
    case NALU_TYPE_IDR://I帧
    {
               while (size > 0)            //若需要拆包，则循环
               {
                   FrameLen = (size > PS_PES_PAYLOAD_SIZE) ? PS_PES_PAYLOAD_SIZE : size;
                   gb28181_make_pes_header(workspace + index, 0XE0, FrameLen, pts, pts);
                   index += PES_HDR_LEN;
                   memcpy(workspace + index, data, FrameLen);
                   index += FrameLen;
                   size -= FrameLen;
                   data += FrameLen;
               }
               m_pts += 3600;
               flag = 1;//右场
               if (1 == m_doubleField)//双场
               {
                   return 0;
               }
               else//单场
               {
                   break;
               }
    }
    case 7://SPS
    {
               index = 0;
               gb28181_make_ps_header(workspace + index, pts);
               index += PS_HDR_LEN;
               gb28181_make_psm_header(workspace + index);
               index += PSM_HDR_LEN;
               gb28181_make_pes_header(workspace + index, 0xE0, size, pts, pts);
               index += PES_HDR_LEN;
               memcpy(workspace + index, data, size);
               index += size;
               return 0;
    }
    case 8://PPS
    {
               gb28181_make_pes_header(workspace + index, 0xE0, size, pts, pts);
               index += PES_HDR_LEN;
               memcpy(workspace + index, data, size);
               index += size;
               return 0;
    }
    case -1://Audio Frame
    {
               //n_pts += 320;
               n_pts += size;
               index = 0;
               while (size > 0)
               {
                   FrameLen = (size > PS_PES_PAYLOAD_SIZE) ? PS_PES_PAYLOAD_SIZE : size;
                   gb28181_make_pes_header(workspace + index, 0XC0, FrameLen, pts, pts);
                   index += PES_HDR_LEN;
                   memcpy(workspace + index, data, FrameLen);
                   index += FrameLen;
                   size -= FrameLen;
                   data += FrameLen;
               }
               break;
    }
    default:
    {
               return 0;
    }
    }
    if (m_fnstreamcb)
    {
        m_fnstreamcb(workspace, index, m_puser, type, pts, frameTime);
    }
    return 0;
}
void PsPacker::process(dataInfo *info)
{
    if (m_ptsflag)//使用外部时间戳
    {
        pack(info->data, info->size, info->type,info->pts, info->frameTime);
    }
    else          //使用内部时间戳
    {
        if (-1 == info->type)//Audio Frame
        {
            pack(info->data, info->size, info->type, n_pts, info->frameTime);

        }
        else                 //Video Frame
        {
            pack(info->data, info->size, info->type, m_pts, info->frameTime);
        }
    }
}