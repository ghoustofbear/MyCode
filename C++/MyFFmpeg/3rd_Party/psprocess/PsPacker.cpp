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
*@remark:   psͷ�ķ�װ,����ľ������ݵ���д�Ѿ�ռλ�����Բο���׼
*@param :   pData  [in] ���psͷ���ݵĵ�ַ
*           s64Src [in] ʱ���
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
*@remark:   sysͷ�ķ�װ,����ľ������ݵ���д�Ѿ�ռλ�����Բο���׼
*@param :   pData  [in] ���psͷ���ݵĵ�ַ
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
    bits_write(&bitsBuffer, 16, SYS_HDR_LEN - 6);/*header_length ��ʾ���ֽں���ĳ��ȣ���������ͷҲ�Ǵ���˼*/
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
*@remark:   psmͷ�ķ�װ,����ľ������ݵ���д�Ѿ�ռλ�����Բο���׼
*@param :   pData  [in] ���psͷ���ݵĵ�ַ
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
    bits_write(&bitsBuffer, 16, 18);     /*program stream map length ���ֽڳ���֮��*/

    bits_write(&bitsBuffer, 1, 1);      /*current next indicator ֵΪ1ʱ��ǰ��Ч��ֵΪ0ʱ��һ����Ч */
    bits_write(&bitsBuffer, 2, 3);      /*reserved*/
    bits_write(&bitsBuffer, 5, 0);      /*program stream map version ÿ����Ŀ��ӳ�䶨��ı�ʱ���汾������*/
    bits_write(&bitsBuffer, 7, 0x7F);   /*reserved */
    bits_write(&bitsBuffer, 1, 1);      /*marker bit */

    bits_write(&bitsBuffer, 16, 0);      /*program stream info length ������ֶ����������ܳ�*/
    bits_write(&bitsBuffer, 16, 8);     /*elementary stream map length ������ӳ���ܳ�*/

    /*video ���ɻ�ȱ*/
    bits_write(&bitsBuffer, 8, 0x1B);       /*stream_type h264 ����������*/
    bits_write(&bitsBuffer, 8, 0xE0);   /*elementary_stream_idָʾ�洢�˻�������PES����PES��ͷ��stream_id�ֶεĸ�ֵ*/
    bits_write(&bitsBuffer, 16, 0);     /*elementary_stream_info_length ������ֶ����������ܳ�*/

    /*audio �಻�ɻ�ȱ*/
    bits_write(&bitsBuffer, 8, 0x90);       /*stream_type G711*/
    bits_write(&bitsBuffer, 8, 0xC0);   /*elementary_stream_id*/
    bits_write(&bitsBuffer, 16, 0);     /*elementary_stream_info_length ������ֶ����������ܳ�*/


    /*crc (2e b9 0f 3d)*/
    bits_write(&bitsBuffer, 8, 0x45);   /*crc (24~31) bits*/
    bits_write(&bitsBuffer, 8, 0xBD);   /*crc (16~23) bits*/
    bits_write(&bitsBuffer, 8, 0xDC);   /*crc (8~15) bits*/
    bits_write(&bitsBuffer, 8, 0xF4);   /*crc (0~7) bits*/
    return 0;
}
/***
*@remark:   pesͷ�ķ�װ,����ľ������ݵ���д�Ѿ�ռλ�����Բο���׼
*@param :   pData      [in] ���psͷ���ݵĵ�ַ
*           stream_id  [in] ��������
*           paylaod_len[in] ���س���
*           pts        [in] ʱ���
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
    bits_write(&bitsBuffer, 16, (payload_len)+13);  /*packet_len*/ //���غʹ��ֽڳ���֮��

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
*@remark:   H264/G711A�����PS��
*@param :   data       [in] H264��Ƶ��/G711A��Ƶ��
*           size       [in] ����
*           type       [in] ����
*           pts        [in] ʱ���
*           pdata      [out]��װ��PS��
*           psize      [out]PS������
*@return:   0 success, others failed
*/
int
PsPacker::pack(uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime)
{
    if (NULL == workspace || NULL == data)
    {
        return -1;
    }
    if (size >= 1024 * 1024 || size <= 0)//�ж������Ƿ���ڹ����ռ����󳤶�
    {
        return -1;
    }
    int FrameLen = 0;
    switch (type)
    {
    case NALU_TYPE_SLICE:   //P֡
    {
               if (0 == m_doubleField || 0 == flag)//��������˫����
               {
                   index = 0;
                   gb28181_make_ps_header(workspace + index, pts);
                   index += PS_HDR_LEN;
               }
               while (size > 0)            //����Ҫ�������ѭ��
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
               if (0 == m_doubleField || 1 == flag)//��������˫���ҳ�
               {
                   flag = 0;
                   break;
               }
               else//˫����
               {
                   flag = 1;
                   return 0;
               }
    }
    case NALU_TYPE_IDR://I֡
    {
               while (size > 0)            //����Ҫ�������ѭ��
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
               flag = 1;//�ҳ�
               if (1 == m_doubleField)//˫��
               {
                   return 0;
               }
               else//����
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
    if (m_ptsflag)//ʹ���ⲿʱ���
    {
        pack(info->data, info->size, info->type,info->pts, info->frameTime);
    }
    else          //ʹ���ڲ�ʱ���
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