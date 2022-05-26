/**
 * @file      triggerlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-28 11:43:46
 *   
 * @note
 *  triggerlistener.h defines 
 */
#ifndef __TRIGGERLISTENER_H__
#define __TRIGGERLISTENER_H__
#include "def.h"

class TriggerListener
{
public:
    /* push type bit map define */
    enum
    {
        TR_UNKNOW,
        TR_DEV_STATUS      = 1,                            /**< 设备状态报警, 1-上线 0-下线 */
        TR_DEV_IO,                                         /**< 设备I/O探头报警 */
        TR_DEV_HDD,                                        /**< 设备硬盘报警 */
        TR_DEV_VLOSS,                                      /**< 设备视频丢失报警 */
        TR_DEV_MD          = 5,                            /**< 设备移动侦测报警 */
        TR_DEV_VCOVER,                                     /**< 设备视频遮挡报警 */
        TR_DEV_INVADE,                                     /**< 设备非法入侵报警 */
        TR_DEV_MANUAL,                                     /**< 手持设备手动报警 */
        TR_DEV_GPS,                                        /**< 手持设备GPS报警 */
        TR_DEV_CALL        = 10,                           /**< 门口机呼叫报警用 */
        TR_DEV_END,                                        /**< 分隔标识, 新定义设备报警类型在这之前添加 */

        TR_AI_TRIPWIRE     = 20,                           /**< 拌线 */
        TR_AI_PERIMETER,                                   /**< 周界 */
        TR_AI_ITEMLEFT,                                    /**< 物品遗留丢失 */
        TR_AI_PLATE,                                       /**< 车牌检测 */
        TR_AI_HOVER,                                       /**< 徘徊检测 */
        TR_AI_ANTITHEFT    = 25,                           /**< 防盗检测 */
        TR_AI_OFFJOB,                                      /**< 脱岗检测 */
        TR_AI_MAINTENAREA,                                 /**< 检修区域检测 */
        TR_AI_PRESSPLATE,                                  /**< 压板投退状态检测 */
        TR_AI_INDICATOR,                                   /**< 分合指示牌检测 */
        TR_AI_DISCONNECTOR = 30,                           /**< 隔离开关(触头)状态检测 */
        TR_AI_LIGHT,                                       /**< 指示灯状态检测  */
        TR_AI_HELMET,                                      /**< 安全帽检测 */
        TR_AI_JUMPER,                                      /**< 工作服检测 */

        TR_AI_YX_END,                                      /**< 分隔标识, 新遥信类型在这之前添加 */

        TR_AI_LEDDIGIT     = 40,                           /**< 表计读数检测(七段数码管) */
        TR_AI_METER,                                       /**< 表计读数检测(刻度盘) */
        TR_AI_NUMPEOPLE,                                   /**< 人数统计 */
        TR_AI_THERMOMETRY,                                 /**< 测温 */

        TR_AI_YC_END,                                      /**< 分隔标识, 新遥测类型在这之前添加 */

        TR_PUSH_SYSTEMINFO = 50,                           /**< 系统运行参数 */
        TR_PUSH_LINKAGE,                                   /**< 五防联动上传 */
        TR_PUSH_LINKAGE_TRANSMIT,                          /**< 客户端联动转发 */
        TR_PUSH_VI,                                        /**< 智能分析巡警结果报警开始 */

        TR_PUSH_END,                                       /**< 分隔标识, 新定义推送类型在这之前添加 */

        TR_AI_SIGNAL_LOST = 60,                             /**< 视频信号丢失 */
        TR_AI_SIGNAL_CONTRASTLOW,                           /**< 对比度低 */
        TR_AI_SIGNAL_FUZZY,                                 /**< 图像模糊 */
        TR_AI_SIGNAL_OVERORUNDERBRIGHT,                     /**< 图像过亮\过暗 */
        TR_AI_SIGNAL_COLORCAST,                             /**< 图像偏色 */
        TR_AI_SIGNAL_NOISEJAMMING,                          /**< 噪声干扰 */
        TR_AI_SIGNAL_STRIPENOISE,                           /**< 条纹干扰 */
        TR_AI_SIGNAL_BLACKWHITEIMAGE,                       /**< 黑白图像 */
        TR_AI_SIGNAL_VIDEOOCCLUSION,                        /**< 视频遮挡 */
        TR_AI_SIGNAL_IMAGEFREEZE,                           /**< 画面冻结 */
        TR_AI_SIGNAL_VIDEOUPHEAVAL,                         /**< 视频剧变 */
        TR_AI_SIGNAL_VIDEOSHAKE,                            /**< 视频抖动 */
        TR_AI_SIGNAL_SCENECHANGE,                           /**< 场景变更 */
        TR_AI_SIGNAL_PTZ_OUTOFCONTROL,                      /**< 云台失控 */
        TR_AI_VIDEO_QUALITY_END,                            /**< 分隔标识 */

        TR_PB_EXCEEDHEIGHT,                                 /**< 超过限高 */
        TR_PERSION_TOGETHER,                                /**< 人员聚集 */
        TR_PERMETER_PERSON,                                 /**< 周界监控-人 */
        TR_PERMETER_CAR,                                    /**< 周界监控-车 */
        TR_PERMETER_NOCAR,                                  /**< 周界监控-非机动车 */
        TR_PERMETER_OBJECT,                                 /**< 周界监控-物 */
        TR_CAR_NOIN,                                        /**< 车辆管控-禁入区域 */
        TR_CAR_NOSTOP,                                      /**< 车辆管控-禁停区域 */
        TR_PB_FALLDOWN,                                     /**< 跌倒 */
        TR_PB_DAJIA,                                        /**< 打架 */
        TR_PB_SMOKE,                                        /**< 吸烟 */
        TR_FIRE_CONTROL,                                    /**< 烟火 */
        TR_TEMP_AREA,                                       /**< 区域测温 */
        TR_TEMP_DIFF,                                       /**< 温差检测 */

        TR_TEMP_BODONG,                                     /**< 温差波动 */
        TR_WAIGUAN_MINPAI,                                  /**< 外观检测-铭牌 */
        TR_WAIGUAN_JUEYUAN,                                 /**< 外观检测-绝缘子 */
        TR_WAIGUAN_TAOGUAN,                                 /**< 外观检测-套管 */
        TR_WAIGUAN_BIANYABT,                                /**< 外观检测-变压器本体 */
        TR_WAIGUAN_BILEIZHENG,                              /**< 外观检测-避雷针 */
        TR_WAIGUAN_PATI,                                    /**< 外观检测-爬梯 */
        TR_WAIGUAN_QIANGTI,                                 /**< 外观检测-墙体 */
        TR_WAIGUAN_GANTANG,                                 /**< 外观检测-杆塔 */

        TR_FACE_DETECTTION,                                 /**< 人脸检测 */
        TR_FACE_IDCARD_DETECTTION,                          /**< 人证合一检测 */
        TR_GATE_STATUS,                                     /**< 道闸状态 *//** 20200603 zhangshenyu add**/
        TR_CAPTURE_PIC,                                     /**< 图像抓拍 */


        TR_SYSTEM_STATUS,                                   /**< 系统运行状态, add by shl */
        TR_PTZ_CONTROL,                                     /**< 云台控制状态, add by shl */

        TR_GATEWAY_STATUS,                                  /**< 人员通道摆闸状态 */

        TR_THERMOMETRY_ALARM,                               /**< 红外摄像机的温度告警 */
        TR_STARTED_IVA,                                     /**< 智能分析已启动 */
        TR_PTZ_CONTROL_ALARM,                               /**< 云台控制告警 */
        TR_LAST,		
		TR_CHANNEL_STATUS,									  /**<通道状态告警 */
    };


    enum
    {
        PLATE_TIANDY_STANDARD = 1,                               /**< 天地车牌识别机(标准) */
        PLATE_DAHUA_STARNDARD,                                   /**< 大华车牌识别机(标准) */
        PLATE_HIK_STARNDARD,                                     /**< 海康车牌识别机(标准) */
        PLATE_HIK_NORMAL,                                        /**< 海康车牌识别机(普通) */
    };


    enum AI_COLOR_S
    {
        AI_COLOR_BLACK = 0,                                      /**< 黑色 */
        AI_COLOR_GRAY,                                           /**< 灰色 */
        AI_COLOR_WHITE,                                          /**< 白色 */
        AI_COLOR_RED,                                            /**< 红色 */
        AI_COLOR_YELLOW,                                         /**< 黄色 */
        AI_COLOR_GREEN,                                          /**< 绿色 */
        AI_COLOR_BLUE,                                           /**< 蓝色 */
        AI_COLOR_PURPLE,                                         /**< 紫色 */
        AI_COLOR_DARKGRAY,                                       /**< 深灰*/
        AI_COLOR_BKAIR,                                          /**< 民航黑色(海康) */
        AI_COLOR_SILVER,                                         /**< 银色 */
        AI_COLOR_DARKBLUE,                                       /**< 深蓝 */
        AI_COLOR_BROWN,                                          /**< 棕色 */
        AI_COLOR_PINK,                                           /**< 粉色 */
        AI_COLOR_CYAN,                                           /**< 青色 */
        AI_COLOR_ORANGE_RED,                                     /**< 橙红 */
        AI_COLOR_ORGANE,                                         /**< 橙色 */
        AI_COLOR_LIGHT_BLUE,                                     /**< 淡蓝 */
        AI_COLOR_LIGHT_YELLOW,                                   /**< 淡黄 */
        AI_COLOR_LIGHT_CYAN,                                     /**< 淡青 */
        AI_COLOR_LIGHT_GREEN,                                    /**< 淡绿 */
        AI_COLOR_LIGHT_PURPLE,                                   /**< 淡紫 */
        AI_COLOR_YELLOW_GREEN,                                   /**< 黄绿 */
        AI_COLOR_GOLDEN,                                         /**< 金色 */
        AI_COLOR_BLUE_GRAY,                                      /**< 蓝灰 */
        AI_COLOR_GREEN_GRAY,                                     /**< 绿灰 */
        AI_COLOR_CREAM,                                          /**< 米色 */
        AI_COLOR_CYAN_GREEN,                                     /**< 青绿 */
        AI_COLOR_DARK_RED,                                       /**< 深红 */
        AI_COLOR_SKY_BLUE,                                       /**< 天蓝 */
        AI_COLOR_SILVER_GRAY,                                    /**< 银灰 */
        AI_COLOR_PURPLE_RED,                                     /**< 紫红 */
        AI_COLOR_BROWN_RED,                                      /**< 棕红 */
        AI_COLOR_BROWN_YELLOW,                                   /**< 棕黄 */
        AI_COLOR_FLESH,                                          /**< 肉色 */
        AI_COLOR_UNKNOWN = 100,
    };

    virtual void       onAlarm(string &json, void *data, uint32_t size) = 0;
    virtual void       onAIYX(string &json, void *data, uint32_t size)  = 0;
    virtual void       onAIYC(string &json, void *data, uint32_t size)  = 0;
};

#endif

