#include "h264parse.h"
#include <math.h>

unsigned int Ue(unsigned char *pu8Data, unsigned int u32Len, unsigned int &u32StartBit)
{
	unsigned int u32ZeroNum = 0;
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

	long dwRet = 0;
	for (unsigned int u32I = 0; u32I < u32ZeroNum; ++u32I)
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

int Se(unsigned char *pu8Data, unsigned int u32Len, unsigned int &u32StartBit)
{
	int s32Val = Ue(pu8Data, u32Len, u32StartBit);
	int s32Ret = ceil(s32Val / 2.0);

	if (s32Val % 2 == 0)
	{
		s32Ret = -s32Ret;
	}

	return s32Ret;
}

long U(unsigned int u32BitCount, unsigned char *pu8Data, unsigned int &u32StartBit)
{
	long dwRet = 0;

	for (unsigned int s32I = 0; s32I < u32BitCount; ++s32I)
	{
		dwRet <<= 1;
		if (pu8Data[u32StartBit / 8] &
			(0x80 >> (u32StartBit % 8)))
		{
			dwRet += 1;
		}

		u32StartBit++;
	}

	return dwRet;
}

int parseh264resolution(unsigned char *pu8Data, int s32Len, int &s32Height, int &s32Width)
{
	unsigned int u32StartBit = 0;
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