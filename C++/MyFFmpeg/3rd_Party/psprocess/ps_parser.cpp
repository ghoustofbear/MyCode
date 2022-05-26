#include "ps_parser.h"
#include <string.h>
#include <stdio.h>
#define  MAX_FRAME_SIZE               (512*1024)

PsParser::PsParser()
{
	m_findStart = false;
	m_finished  = false;

	m_work_length = 0;
	m_sps_length  = 0;
	m_pps_length  = 0;
	m_frame_length = 0;

	m_frame_type   = 0;

//	m_callback  = NULL;
//	m_user_data = NULL;

	m_pts       = 0;

	m_workspase = new uint8_t[MAX_FRAME_SIZE + 1024];
	m_frame	    = new uint8_t[MAX_FRAME_SIZE];
	m_sps       = new uint8_t[1600];
	m_pps       = new uint8_t[1600];
}

PsParser::~PsParser()
{
	if (m_workspase)
	{
		delete[]m_workspase;
		m_workspase = NULL;
	}

	if (m_frame)
	{
		delete[]m_frame;
		m_frame = NULL;
	}

	if (m_sps)
	{
		delete[]m_sps;
		m_sps = NULL;
	}

	if (m_pps)
	{
		delete[]m_pps;
		m_pps = NULL;
	}
}



void 
PsParser::process(dataInfo *info)
{
	unpack(info->data, info->size);
}

int
PsParser::unpack(uint8_t* data, uint32_t size)
{
	uint8_t* input = data;
	uint32_t input_length = size;
	memcpy(m_workspase + m_work_length, input, input_length);
	m_work_length += input_length;

	ASSERT(m_work_length < MAX_FRAME_SIZE);

	uint8_t* p = m_workspase;
	uint32_t pos = 0;

	if (!m_findStart)
	{
		while (pos < m_work_length)
		{
			if (p[pos] == 0x00 &&
				p[pos + 1] == 0x00 &&
				p[pos + 2] == 0x01 &&
				p[pos + 3] == 0xBA)
			{
				m_findStart = true;
				break;
			}
			pos++;
		}

		if (pos >= m_work_length)
		{
			return -1;
		}
	}

	int res = 0;
	while (pos < m_work_length)
	{
		if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xBA)
		{
			res = parseBA(p, pos);
			if (res < 0)
			{
				return res;
			}
		}

		else if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xBB)
		{
			res = parseBB(p, pos);
			if (res < 0)
			{
				return res;
			}
		}

		else if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xBC)
		{
			res = parseBC(p, pos);
			if (res < 0)
			{
				return res;
			}
		}

		else if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xBD)
		{
			res = parseBD(p, pos);
			if (res < 0)
			{
				return res;
			}
		}

		else if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xC0)
		{
			res = parseC0(p, pos);
			if (res < 0)
			{
				return res;
			}
		}

		else if (p[pos] == 0x00 &&
			p[pos + 1] == 0x00 &&
			p[pos + 2] == 0x01 &&
			p[pos + 3] == 0xE0)
		{
			res = parseE0(p, pos);
			if (res < 0)
			{
				return res;
			}
		}
		else
		{
			printf("unkown type \n");
			m_work_length = 0;
			m_findStart = false;
			return -2;
		}
	}

	return 0;
}


int 
PsParser::parseBA(uint8_t* p, uint32_t &pos)
{
	PACK_HEADER_S* pack_header = (PACK_HEADER_S *)&p[pos];
	if (pos + sizeof(PACK_HEADER_S) + pack_header->pack_stuffing_length < m_work_length)
	{
		pos = pos + sizeof(PACK_HEADER_S) + pack_header->pack_stuffing_length;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}

		/* 数据不够 */
		return -1;
	}

	return 0;
}

int
PsParser::parseBB(uint8_t* p, uint32_t &pos)
{
	SYSTEM_HEADER_S* system_header = (SYSTEM_HEADER_S*)&p[pos];
	if (pos + sizeof(SYSTEM_HEADER_S) + SWAP2(system_header->header_length) < m_work_length)
	{
		pos = pos + sizeof(SYSTEM_HEADER_S) + SWAP2(system_header->header_length);

		return 0;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}

		return -1;
	}
}

int
PsParser::parseBC(uint8_t* p, uint32_t &pos)
{
	PROGRAM_STREAM_MAP_S* programe_system_map = (PROGRAM_STREAM_MAP_S*)&p[pos];
	if (pos + sizeof(PROGRAM_STREAM_MAP_S) + SWAP2(programe_system_map->program_stream_map_length) < m_work_length)
	{
		pos = pos + sizeof(PROGRAM_STREAM_MAP_S) + SWAP2(programe_system_map->program_stream_map_length);
		return 0;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}

		return -1;
	}
}

int
PsParser::parseBD(uint8_t* p, uint32_t &pos)
{
	/* private_stream_1 */
	PES_PACKET_S* pes_packet = (PES_PACKET_S*)&p[pos];

	uint16_t payload_length = SWAP2(pes_packet->PES_packet_length) - 2 - 1 - pes_packet->PES_header_data_length;

	if (pos + sizeof(PES_PACKET_S) + SWAP2(pes_packet->PES_packet_length) < m_work_length)
	{
		pos = pos + sizeof(PES_PACKET_S) + SWAP2(pes_packet->PES_packet_length) - 2 - 1;

		return 0;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}

		return -1;
	}
}

int
PsParser::parseC0(uint8_t* p, uint32_t &pos)
{
	/* GB/T XXXX.3或GB/T AAAA.3音频流编号 */
	PES_PACKET_S* pes_packet = (PES_PACKET_S*)&p[pos];
	PES_PTS_S*    pes_pts = (PES_PTS_S *)&p[pos + sizeof(PES_PACKET_S)];

	uint16_t payload_length = SWAP2(pes_packet->PES_packet_length) - 2 - 1 - pes_packet->PES_header_data_length;
	if (payload_length >= MAX_FRAME_SIZE)
	{
		return -2;
	}

	if (pos + sizeof(PES_PACKET_S) + SWAP2(pes_packet->PES_packet_length) < m_work_length)
	{
		pos = pos + sizeof(PES_PACKET_S) + pes_packet->PES_header_data_length;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}
		return -1;
	}

	uint8_t* payload = &p[pos];
	memcpy(m_frame, &p[pos], payload_length);
	m_frame_length = payload_length;
	m_frame_type = 'A';

	m_pts =
		(pes_pts->PTS32_30 << 30) |
		(pes_pts->PTS29_22 << 22) |
		(pes_pts->PTS21_15 << 15) |
		(pes_pts->PTS14_7 << 7) |
		(pes_pts->PTS6_0);
	//m_pts /= 1000;

	if (m_fnstreamcb)
	{
		m_fnstreamcb(m_frame, m_frame_length, m_puser,m_frame_type,m_pts, 0);
	}

	pos = pos + payload_length;
	m_frame_length = 0;

	return 0;
}

int
PsParser::parseE0(uint8_t* p, uint32_t &pos)
{
	/* GB/T XXXX.2或GB/T AAAA.2视频流编号 */
	PES_PACKET_S* pes_packet = (PES_PACKET_S*)&p[pos];
	PES_PTS_S*    pes_pts    = (PES_PTS_S *)&p[pos + sizeof(PES_PACKET_S)];
	uint16_t payload_length = SWAP2(pes_packet->PES_packet_length) - 2 - 1 - pes_packet->PES_header_data_length;
	if (payload_length >= MAX_FRAME_SIZE)
	{
		return -2;
	}

	if (pos + sizeof(PES_PACKET_S) + SWAP2(pes_packet->PES_packet_length) < m_work_length)
	{
		pos = pos + sizeof(PES_PACKET_S) + pes_packet->PES_header_data_length;
	}
	else
	{
		m_work_length = m_work_length - pos;
		if (0 != pos)
		{
			memcpy(m_workspase, &p[pos], m_work_length);
		}

		return -1;
	}

	uint8_t* payload = &p[pos];

	if (payload[0] == 0x00 &&
		payload[1] == 0x00 &&
		payload[2] == 0x00 &&
		payload[3] == 0x01 &&
		payload[4] == 0x67)
	{
		memcpy(m_sps, &p[pos], payload_length);
		m_sps_length = payload_length;

		m_pts =
			(pes_pts->PTS32_30 << 30) |
			(pes_pts->PTS29_22 << 22) |
			(pes_pts->PTS21_15 << 15) |
			(pes_pts->PTS14_7 << 7) |
			(pes_pts->PTS6_0);
		//m_pts /= 1000;
	}
	else if (
		payload[0] == 0x00 &&
		payload[1] == 0x00 &&
		payload[2] == 0x00 &&
		payload[3] == 0x01 &&
		payload[4] == 0x68)
	{
		memcpy(m_pps, &p[pos], payload_length);
		m_pps_length = payload_length;
	}
	else if (
		payload[0] == 0x00 &&
		payload[1] == 0x00 &&
		payload[2] == 0x00 &&
		payload[3] == 0x01 &&
		payload[4] == 0x65)
	{
		memcpy(m_frame, m_sps, m_sps_length);
		memcpy(m_frame + m_sps_length, m_pps, m_pps_length);
		memcpy(m_frame + m_sps_length + m_pps_length, &p[pos], payload_length);
		m_frame_length = m_sps_length + m_pps_length + payload_length;
		m_frame_type = 'I';
	}
	else if (
		payload[0] == 0x00 &&
		payload[1] == 0x00 &&
		payload[2] == 0x00 &&
		payload[3] == 0x01 &&
		payload[4] == 0x41)
	{
		memcpy(m_frame + m_frame_length, &p[pos], payload_length);
		m_frame_length += payload_length;
		m_frame_type = 'P';

		m_pts =
			(pes_pts->PTS32_30 << 30) |
			(pes_pts->PTS29_22 << 22) |
			(pes_pts->PTS21_15 << 15) |
			(pes_pts->PTS14_7 << 7) |
			(pes_pts->PTS6_0);
		//m_pts /= 1000;
	}
	else
	{
		memcpy(m_frame + m_frame_length, &p[pos], payload_length);
		m_frame_length += payload_length;
	}

	pos = pos + payload_length;

	if (!(p[pos] == 0x00 &&
		p[pos + 1] == 0x00 &&
		p[pos + 2] == 0x01 &&
		p[pos + 3] == 0xE0))
	{
		if (m_fnstreamcb)
		{
			m_fnstreamcb(m_frame, m_frame_length, m_puser, m_frame_type, m_pts, 0);
		}
		m_finished = true;
		m_frame_length = 0;
	}

	return 0;
}

