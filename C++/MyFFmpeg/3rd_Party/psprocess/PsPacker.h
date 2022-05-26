#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PsProcess.h"
#include "PsProcessApi.h"
#define PS_HDR_LEN  14
#define SYS_HDR_LEN 18
#define PSM_HDR_LEN 24
#define PES_HDR_LEN 19
#define PS_PES_PAYLOAD_SIZE 65000
#define AND8BIT(X) (0XFF >> (8 -X))//ANDBIT(5)获取NAL单元类型
/*	H264定义的类型 values for nal_unit_type	*/
typedef enum
{
	NALU_TYPE_SLICE = 1,	//	P帧				(非IDR图像的编码条带(Coded slice of a non-IDR picture))
	NALU_TYPE_DPA = 2,		//					(编码条带数据分割块A (Coded slice data partition A))
	NALU_TYPE_DPB = 3,		//					(编码条带数据分割块B (Coded slice data partition B))
	NALU_TYPE_DPC = 4,		//					(编码条带数据分割块C (Coded slice data partition C))
	NALU_TYPE_IDR = 5,		//	IDR帧			(IDR图像的编码条带(Coded slice of an IDR picture))
	NALU_TYPE_SEI = 6,		//  SEI				(辅助增强信息 Supplemental enhancement information(SEI))
	NALU_TYPE_SPS = 7,		//	SPS关键帧		(序列参数集 Sequence parameter set)
	NALU_TYPE_PPS = 8,		//	PPS关键帧		(图像参数集 Picture parameter set)
	NALU_TYPE_AUD = 9,		//	AUD				(访问单元分隔符 Access Unit Delimiter)
	NALU_TYPE_EOSEQ = 10,	//					(序列结尾 End of sequence)
	NALU_TYPE_EOSTREAM = 11,//					(流结尾 End of stream)
	NALU_TYPE_FILL = 12,	//					(填充数据 Filler data)

} NAL_UNIT_TYPE;

struct bits_buffer_s {
	uint8_t* p_data;
	uint8_t  i_mask;
	int i_size;
	int i_data;
};
/***
*@remark:  讲传入的数据按地位一个一个的压入数据
*@param :  buffer   [in]  压入数据的buffer
*          count    [in]  需要压入数据占的位数
*          bits     [in]  压入的数值
*/
#define bits_write(buffer, count, bits)\
{\
	bits_buffer_s *p_buffer = (buffer); \
	int i_count = (count); \
	uint64_t i_bits = (bits); \
while (i_count > 0)\
{\
	i_count--; \
if ((i_bits >> i_count) & 0x01)\
{\
	p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask; \
}\
	else\
{\
	p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask; \
}\
	p_buffer->i_mask >>= 1;         /*操作完一个字节第一位后，操作第二位*/\
if (p_buffer->i_mask == 0)     /*循环完一个字节的8位后，重新开始下一位*/\
{\
	p_buffer->i_data++; \
	p_buffer->i_mask = 0x80; \
}\
}\
}

class PsPacker : public PsProcess
{
public:
	PsPacker();
	~PsPacker();
	uint8_t* workspace = NULL;
	int index = 0;//workspace的下标值
	virtual void process(dataInfo *info);
	uint64_t m_pts;//视频时间戳
	uint64_t n_pts;//音频时间戳
	int flag = 0;//双场时使用，0表示左场，1表示右场
private:
	int pack(uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime);
	int gb28181_make_pes_header(uint8_t *pData, int stream_id, int payload_len, uint64_t pts, uint64_t dts);
	int gb28181_make_psm_header(uint8_t *pData);
	int gb28181_make_sys_header(uint8_t *pData);
	int gb28181_make_ps_header(uint8_t *pData, uint64_t s64Scr);
	int getNextNALU(uint8_t*pData, int size, int type);
};