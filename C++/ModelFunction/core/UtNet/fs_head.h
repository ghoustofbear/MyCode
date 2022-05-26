/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@qq.com>
 *
 * Tue Sep 22 14:57:27 CST 2015
 *
 * file system record header defines.
 */
#ifndef __FS_HEAD_H__
#define __FS_HEAD_H__
#include "defc1.h"

#define FS_RECORD_FULL			*(uint32_t *)"Full"      /**< 已经建立了索引的录像文件 */
#define FS_RECORD_DATA	        *(uint32_t *)"Data"      /**< 只有帧头信息，没有建立索引的录像文件 */

#define FS_RECORD_TAG           *(uint64_t *)"ALRSFile"  /**< Identify ALRS media file */ 
#define FS_RECORD_DATAALIGN     8

#ifndef _WIN32
#include "uuid.h"
#endif

typedef struct
{
	uint64_t       tag;
	uint32_t       headInfoSize;                         /**< sizeof(FS_RECORD_HEAD_INFO_S) */
	uint32_t       status;                               /**< FS_RECORD_FULL or FS_RECORD_DATA */
	uint64_t       tailOffset;                           /**< Tail info offset from the beginning of the file */
	uuid_t         uuid;                                 /**< Unique identifier for this file */
}FS_RECORD_HEAD_S;

typedef struct
{
	uint64_t begin;                                      /**< Equal to UT_FRAME_HEAD_S.Time */
	uint64_t end;                                        /**< Equal to UT_FRAME_HEAD_S.Time */
}FS_RECORD_HEAD_INFO_S;

#define FS_RECORD_INDEXLISTTAG  *(uint64_t *)"ListIdx"

typedef struct
{
	uint64_t  tag;                                       /**< RSFileIndexListTag ... */
	uint16_t  listHeadInfoSize;                          /**< Extra list head info size, 0 for RSFileIndexListTag */
	uint16_t  itemSize;                                  /**< sizeof()FS_RECORD_FRAME_INDEX_S */
	uint32_t  listSize;                                  /**< all subsequent item size */
}FS_RECORD_TAIL_S;

typedef struct
{
	uint64_t  time;                                      /**<  I frame pts, equal to UT_FRAME_HEAD_S.Time*/
	uint64_t  offset;                                    /**< Frame offset from the beginning of the file */
}FS_RECORD_FRAME_INDEX_S;
#endif
