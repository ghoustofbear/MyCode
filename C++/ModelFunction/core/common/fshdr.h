/**
 * @file      fs_head.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-23 18:03:58
 *   
 * @note
 *  fs_head.h defines 
 */
#ifndef __FS_HEAD_H__
#define __FS_HEAD_H__
#include "def.h"

#define FS_RECORD_FULL			*(uint32_t *)"Full"        /**< 已经建立了索引的录像文件 */
#define FS_RECORD_DATA	        *(uint32_t *)"Data"        /**< 只有帧头信息，没有建立索引的录像文件 */
#define FS_RECORD_AUDI	        *(uint32_t *)"Audi"        /**< 纯音频 */

#define FS_RECORD_TAG           *(uint64_t *)"ALRSFile"    /**< Identify ALRS media file */
#define FS_RECORD_DATAALIGN     (8)

#ifndef _WIN32
#include "uuid.h"
#endif

typedef struct
{
    uint64_t       tag;                                    /**< FS_RECORD_TAG */
    uint32_t       headInfoSize;                           /**< sizeof(fs_record_info_s) */
    uint32_t       status;                                 /**< FS_RECORD_FULL or FS_RECORD_DATA */
    uint64_t       tailOffset;                             /**< Tail info offset from the beginning of the file */
    uuid_t         uuid;                                   /**< Unique identifier for this file */
} fs_record_head_s;

typedef struct
{
    uint64_t       begin;                                  /**< Equal to ut_frame_s.time */
    uint64_t       end;                                    /**< Equal to ut_frame_s.time */
} fs_record_info_s;

#define FS_RECORD_INDEXLISTTAG  *(uint64_t *)"ListIdx"

typedef struct
{
    uint64_t       tag;                                    /**< RSFileIndexListTag ... */
    uint16_t       listHeadInfoSize;                       /**< Extra list head info size, 0 for RSFileIndexListTag */
    uint16_t       itemSize;                               /**< sizeof()fs_frame_idx_s */
    uint32_t       listSize;                               /**< all subsequent item size */
} fs_record_tail_s;

typedef struct
{
    uint64_t       time;                                   /**< I frame pts, equal to ut_frame_s.Time*/
    uint64_t       offset;                                 /**< Frame offset from the beginning of the file */
} fs_frame_idx_s;
#endif
