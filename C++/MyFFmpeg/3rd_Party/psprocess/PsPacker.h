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
#define AND8BIT(X) (0XFF >> (8 -X))//ANDBIT(5)��ȡNAL��Ԫ����
/*	H264��������� values for nal_unit_type	*/
typedef enum
{
	NALU_TYPE_SLICE = 1,	//	P֡				(��IDRͼ��ı�������(Coded slice of a non-IDR picture))
	NALU_TYPE_DPA = 2,		//					(�����������ݷָ��A (Coded slice data partition A))
	NALU_TYPE_DPB = 3,		//					(�����������ݷָ��B (Coded slice data partition B))
	NALU_TYPE_DPC = 4,		//					(�����������ݷָ��C (Coded slice data partition C))
	NALU_TYPE_IDR = 5,		//	IDR֡			(IDRͼ��ı�������(Coded slice of an IDR picture))
	NALU_TYPE_SEI = 6,		//  SEI				(������ǿ��Ϣ Supplemental enhancement information(SEI))
	NALU_TYPE_SPS = 7,		//	SPS�ؼ�֡		(���в����� Sequence parameter set)
	NALU_TYPE_PPS = 8,		//	PPS�ؼ�֡		(ͼ������� Picture parameter set)
	NALU_TYPE_AUD = 9,		//	AUD				(���ʵ�Ԫ�ָ��� Access Unit Delimiter)
	NALU_TYPE_EOSEQ = 10,	//					(���н�β End of sequence)
	NALU_TYPE_EOSTREAM = 11,//					(����β End of stream)
	NALU_TYPE_FILL = 12,	//					(������� Filler data)

} NAL_UNIT_TYPE;

struct bits_buffer_s {
	uint8_t* p_data;
	uint8_t  i_mask;
	int i_size;
	int i_data;
};
/***
*@remark:  ����������ݰ���λһ��һ����ѹ������
*@param :  buffer   [in]  ѹ�����ݵ�buffer
*          count    [in]  ��Ҫѹ������ռ��λ��
*          bits     [in]  ѹ�����ֵ
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
	p_buffer->i_mask >>= 1;         /*������һ���ֽڵ�һλ�󣬲����ڶ�λ*/\
if (p_buffer->i_mask == 0)     /*ѭ����һ���ֽڵ�8λ�����¿�ʼ��һλ*/\
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
	int index = 0;//workspace���±�ֵ
	virtual void process(dataInfo *info);
	uint64_t m_pts;//��Ƶʱ���
	uint64_t n_pts;//��Ƶʱ���
	int flag = 0;//˫��ʱʹ�ã�0��ʾ�󳡣�1��ʾ�ҳ�
private:
	int pack(uint8_t *data, int size, int type, uint64_t pts, uint64_t frameTime);
	int gb28181_make_pes_header(uint8_t *pData, int stream_id, int payload_len, uint64_t pts, uint64_t dts);
	int gb28181_make_psm_header(uint8_t *pData);
	int gb28181_make_sys_header(uint8_t *pData);
	int gb28181_make_ps_header(uint8_t *pData, uint64_t s64Scr);
	int getNextNALU(uint8_t*pData, int size, int type);
};