/**
 * @file      c2types.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-01 17:28:57
 *
 * @note
 *  c2types.h defines
 */
#ifndef __C2TYPES_H__
#define __C2TYPES_H__
#include "def.h"

#define JSON_DATA_VERSION    "1.0"
#define MAX_CHN_NUM          (128)

typedef struct
{
    char         * key;
    char         * value;
} c2_ex_param_s;

typedef struct
{
    char         * servIp;                                 /**< 连接服务器IP */
    int            servPort;                               /**< 连接服务器端口 */
    char         * username;                               /**< 登录用户名 */
    char         * password;                               /**< 登录密码 */
    bool           subscribe;                              /**< 默认是否订阅报警 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_login_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    int            streamType;                             /**< 0-大码流, 1-从码流 */

    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            transport;                              /**< IPPROTO_TCP/IPPROTO_UDP, transport */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_preview_s;


typedef struct
{
    int            type;                                   /**< 设备类型, 参考国网B协议设备类型定义值 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_devinfo_s;

typedef struct
{
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_channel_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            speed;                                  /**< 速度 0~10 */
    int            preset;                                 /**< 预制位 0~128 */
    bool           stop;                                   /**< 是否停止 false-start, true-stop */

    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_ptz_s;


typedef struct
{
    char         * addrcode        ;                       /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    int            channelNo;                              /**< 通道号 */
      
    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_preset_s;

typedef struct
{
    char         * name;                                   /**< 预置位名称 */
    int            idx;                                    /**< 预置位序列号 */
    int            type;                                   /**< 预置位类型 */
    int            isIVS;                                  /**< 是否开启智能分析，0-否 */
} c2_preset_s;

typedef struct
{
    char         * addrcode;                               /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    int            channelNo;                              /**< 通道号 */

    int            presetNum;                              /**< 预置位个数 */
    c2_preset_s  * presetList;                             /**< 预置列表 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_set_preset_s;

typedef struct
{
    char         * addrcode;                               /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    int            channelNo;                              /**< 通道号 */

    char         * name;                                   /**< 预置位名称 */
    int            idx;                                    /**< 预置位序列号 */
    int            type;                                   /**< 预置位类型 */
    int            isIVS;                                  /**< 是否开启智能分析，0-否 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_set_preset_v1_s;


typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            type;                                   /**< 0-所有录像 1-计划录像 2 手动录像 3-报警录像 */

    datetime_s     start;                                  /**< 查找开始时间 */
    datetime_s     end;                                    /**< 查找结束时间 */

    int            fromIndex;                              /**< 期望返回的开始记录序号, 从1开始 */
    int            toIndex;                                /**< 期望返回的结束记录序号(数) */

    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_query_record_s;

typedef struct
{
    char         * filename;                               /**< 录像文件名 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_pb_name_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */

    int            type;                                   /**< 0-所有录像 1-计划录像 2 手动录像 3-报警录像 */

    datetime_s     start;                                  /**< 回放开始时间 */
    datetime_s     end;                                    /**< 回放结束时间 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_pb_time_s;

typedef struct
{
    char         * savepath;                               /**< 录像保存路径 */
    char         * filename;                               /**< 录像文件名 */

    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_download_s;

typedef struct
{
    char         * savepath;                               /**< 录像保存路径 */

    datetime_s     start;                                  /**< 下载开始时间 */
    datetime_s     end;                                    /**< 下载结束时间 */

    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            obtainType;                             /**< 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流 */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_download_time_s;

typedef struct
{
    datetime_s     time;                                   /**< 同步时间值 */
    char         * zone;                                   /**< 时区, Asia/Shanghai */
    bool           setLocal;                               /**< 是否设置本地时间 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_synct_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    char         * deviceHost;                             /**< 设备IP */
    int            devicePort;                             /**< 设备端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */
    
}c2_dutk_chn;

typedef struct
{
    int            obtainType;                             /**< 0-IP/PORT, 1-IP/PORT 2-地址码, 3-地址码 */
    int            chnNum;                                 /**< 通道个数 */
    c2_dutk_chn  * chnList;                                /**< 通道列表 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_dualtalk_dev_s;


typedef struct
{
    int            inPort;                                 /**< 矩阵输入端口号 */
    int            outPort;                                /**< 矩阵输出端口号 */
    char         * deviceHost;                             /**< 设备IP地址 */
    int            devicePort;                             /**< 设备端口号 */
    char         * subServHost;                            /**< 二级流媒体服务器地址 */
    int            subServPort;                            /**< 二级流媒体端口号 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
}c2_matrix_s;

typedef struct
{
    int            mode;                                   /**显示模式，NET_SHOW_MODE_E, 1, 4, 9 ,16*/
    int            channelNo;                              /**需要解码显示的通道，一位对应一个通道*/
    int            outPort;                                /**显示输出端口，从1开始 */
    char         * deviceHost;                             /**< 设备ip地址 */
    int            devicePort;                             /**< 设备端口 */

    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */

}c2_decoder_param_s;

typedef struct
{
    int            opera;                                  /**< 操作类型，0：添加人脸信息 1：删除人脸信息 */

    int            picSize;                                /**< 图片大小（未使用，图片大小不得超过1.5M） */
    char         * picPath;                                /**< 图片路径 */
    char         * name;                                   /**< 人脸名称 */
    char         * Id;                                     /**< 人脸ID，不能重复 */

    int            channelNo;                              /**< 通道号，一般为 0 */
    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_set_face_s;

typedef struct
{
    int            channelNo;                              /**< 通道号 0,1,2,3... */
    int            patternCmd;                             /**< 云台轨迹操作命令码,详见下面定义*/
    int            patternID;                              /**< 云台轨迹ID(删除所有轨迹时无效)*/

    char         * deviceHost;                             /**< 设备IP, 为NULL时addrcode有效 */
    int            devicePort;                             /**< 设备端口 */
    char         * subServHost;                            /**< 二级流媒体服务器IP */
    int            subServPort;                            /**< 二级流媒体服务器端口 */
    char         * addrcode;                               /**< 前端设备地址编码 */

    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_ptz_pattern_s;

typedef struct
{
    float          left;
    float          right;
    float          top;
    float          bottom;
    int            exLen;                                  /**< 扩展参数长度 */
    void         * exParam;                                /**< 扩展参数 */
} c2_display_region_s;

#endif
