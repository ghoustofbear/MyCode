/**
* (C) Copyright 2016, XXXX Co., Ltd.
* Zhong caiwang <caiwang213@163.com>
* Tue Nov  1 08:58:36 CST 2016
*
* ps stream parser function class.
*/

#ifndef __PS_PARSER__
#define __PS_PARSER__
#include <stdint.h>
#include "PsProcess.h"

class PsParser : public PsProcess
{
public:

#define SWAP2(v) ( ((v&0xff)<<8) | ((v&0xff00)>>8) )
#define SWAP4(v) (((v&0xff)<<24) | ((v&0xff00)<<8) | ((v&0xff0000)>>8) | ((v&0xff000000)>>24))

#pragma pack(1)
	/* PS Header */
	struct PACK_HEADER_S
	{
		uint8_t    pack_start_code[4];

		uint8_t    pad_fag01 : 2;
		uint8_t    system_clock_reference_base32_30 : 3;
		uint8_t    marker_bit1 : 1;	
		uint8_t    system_clock_reference_base29_28 : 2;

		uint8_t    system_clock_reference_base27_20;

		uint8_t    system_clock_reference_base19_15 : 5;
		uint8_t    marker_bit2 : 1;
		uint8_t    system_clock_reference_base14_13 : 2;

		uint8_t    system_clock_reference_base12_5;

		uint8_t    system_clock_reference_base4_0 : 5;
		uint8_t    marker_bit3 : 1;
		uint8_t    system_clock_reference_extension8_7 : 2;

		uint8_t    system_clock_reference_extension6_0 : 7;
		uint8_t    marker_bit4 : 1;

		uint8_t    program_mux_rate[3];
		uint8_t    pack_stuffing_length : 3;
		uint8_t    reserved : 5;

		/* pack_stuffing_length * sizeof(uint8_t) */
		//uint8_t    stuffing_byte; ;
	};

	/* 系统标题 */
	struct SYSTEM_HEADER_S
	{
		uint8_t    system_header_start_code[4];
		uint16_t   header_length;                      /* 注意标准是小端模式，大于两字节的数据需要转化 */
	};

	/* 节目流映射(PSM) */
	struct PROGRAM_STREAM_MAP_S
	{
		uint8_t    packet_start_code_prefix[3];
		uint8_t    map_stream_id;
		uint16_t   program_stream_map_length;

		/* program_stream_map_length * sizeof(uint8_t) */
		//uint8_t    program_stream_infos;
		//uint32_t   crc;
	};

	/* PES分组 */
	struct PES_PACKET_S
	{
		uint8_t    packet_start_code_prefix[3];
		uint8_t    stream_id;

		/* 包括PES_packet_length 至 PES_header_data_length之间的数据长度 */
		uint16_t   PES_packet_length;

		uint8_t    PES_pad10 : 2;
		uint8_t    PES_scrambling_control : 2;
		uint8_t    PES_priority : 1;
		uint8_t    data_alignment_indicator : 1;
		uint8_t    copyright : 1;
		uint8_t    original_or_copy : 1;

#if 0
		uint8_t    PTS_DTS_flags : 2;
		uint8_t    ESCR_flag : 1;
		uint8_t    ES_rate_flag : 1;
		uint8_t    DSM_trick_mode_flag : 1;
		uint8_t    additional_copy_info_flag : 1;
		uint8_t    PES_CRC_flag : 1;
		uint8_t    PES_extension_flag : 1;
#endif
		uint8_t    PES_extension_flag : 1;
		uint8_t    PES_CRC_flag : 1;
		uint8_t    additional_copy_info_flag : 1;
		uint8_t    DSM_trick_mode_flag : 1;
		uint8_t    ES_rate_flag : 1;
		uint8_t    ESCR_flag : 1;
		uint8_t    PTS_DTS_flags : 2;

		uint8_t    PES_header_data_length;
	};

	struct PES_PTS_S
	{
		uint8_t    marker_bit1 : 1;
		uint8_t    PTS32_30 : 3;
		uint8_t    bit0010 : 4;

		uint8_t    PTS29_22 : 8;

		uint8_t    marker_bit2 : 1;
		uint8_t    PTS21_15 : 7;

		uint8_t    PTS14_7 : 8;

		uint8_t    marker_bit3 : 1;
		uint8_t    PTS6_0 : 7;
	};

#pragma pack()

	PsParser();
	~PsParser();

public:

	virtual void process(dataInfo *info);

private:
    int  unpack(uint8_t* data, uint32_t size);

protected:
	inline int parseBA(uint8_t* p, uint32_t &pos);

	inline int parseBB(uint8_t* p, uint32_t &pos);

	inline int parseBC(uint8_t* p, uint32_t &pos);

	inline int parseBD(uint8_t* p, uint32_t &pos);

	inline int parseC0(uint8_t* p, uint32_t &pos);

	inline int parseE0(uint8_t* p, uint32_t &pos);

private:
	bool       m_findStart;
	bool       m_finished;

	uint8_t  * m_workspase;
	uint8_t  * m_frame;

	uint8_t  * m_sps;
	uint8_t  * m_pps;

	uint32_t   m_work_length;
	uint32_t   m_sps_length;
	uint32_t   m_pps_length;
	uint32_t   m_frame_length;

	uint8_t    m_frame_type;
	uint64_t   m_pts;

};
#endif