/**
 * @file      bitoperation.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:25:42
 *   
 * @note
 *  bitoperation.h defines 
 */
#ifndef __BITOPERATION_H__
#define __BITOPERATION_H__

/* 计算数组下标 */
#define ARRAY_INDEX(index, bitNum) ((index) / (bitNum))

/* 计算BIT位 */
#define BIT_INDEX(index, bitNum) ((index) % (bitNum))


#define CLR_BIT_NUM(addr, index, bitNum, type) \
		*(addr + ARRAY_INDEX(index, bitNum)) &= \
			(~(((type)0x01) << BIT_INDEX(index, bitNum)))

#define SET_BIT_NUM(addr, index, bitNum, type) \
		*(addr + ARRAY_INDEX(index, bitNum)) |= \
			(((type)0x01) << BIT_INDEX(index, bitNum))

#define CHK_BIT_NUM(addr, index, bitNum, type) \
		(*(addr + ARRAY_INDEX(index, bitNum)) >> BIT_INDEX(index, bitNum)) & ((type)0x01)


#define CLR_BIT_BYTE(addr, index)  (CLR_BIT_NUM(addr, index, 8, unsigned char))
#define SET_BIT_BYTE(addr, index)  (SET_BIT_NUM(addr, index, 8, unsigned char))
#define CHK_BIT_BYTE(addr, index)  (CHK_BIT_NUM(addr, index, 8, unsigned char))

#define CLR_BIT_SHORT(addr, index) (CLR_BIT_NUM(addr, index, 16, unsigned short))
#define SET_BIT_SHORT(addr, index) (SET_BIT_NUM(addr, index, 16, unsigned short))
#define CHK_BIT_SHORT(addr, index) (CHK_BIT_NUM(addr, index, 16, unsigned short))

#define CLR_BIT_LL(addr, index)    (CLR_BIT_NUM(addr, index, 64, unsigned long long))
#define SET_BIT_LL(addr, index)    (SET_BIT_NUM(addr, index, 64, unsigned long long))

#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define CHK_BIT_LL(addr, index)    (CHK_BIT_NUM(addr, index, 64, unsigned __int64))
#else
#define CHK_BIT_LL(addr, index)    (CHK_BIT_NUM(addr, index, 64, unsigned long long))
#endif


#define CLR_BIT_LONG(addr, index)  (CLR_BIT_NUM(addr, index, 32, unsigned long))
#define SET_BIT_LONG(addr, index)  (SET_BIT_NUM(addr, index, 32, unsigned long))
#define CHK_BIT_LONG(addr, index)  (CHK_BIT_NUM(addr, index, 32, unsigned long))

#define CLR_BIT_UNINT(addr, index) (CLR_BIT_NUM(addr, index, 32, unsigned int))
#define SET_BIT_UNINT(addr, index) (SET_BIT_NUM(addr, index, 32, unsigned int))
#define CHK_BIT_UNINT(addr, index) (CHK_BIT_NUM(addr, index, 32, unsigned int))


#define CLR_BIT(addr, index)       (CLR_BIT_BYTE(addr, index))
#define SET_BIT(addr, index)       (SET_BIT_BYTE(addr, index))
#define CHK_BIT(addr, index)       (CHK_BIT_BYTE(addr, index))

/* BIT位转换成字节数 */
#define BITS_TO_BYTES(bits) ((bits) + 7) / 8

/* 按2对齐 */
#define ALIGN_TWO(num) ((num)&(~0x1))

#endif



