#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#define IVA_LOGIN            (100)  /**< 登录消息: SDK返回Json类型 */
#define IVA_LOGOUT           (101)  /**< 退出登录: HTTP请求类型 */
#define IVA_KEEPALIVE        (200)  /**< 心跳消息: SDK内部使用，客户端请勿使用该类型 */
#define IVA_TIMING           (201)  /**< 校时消息: SDK内部使用，客户端请勿使用该类型 */

#define IVA_ALARM_STATE      (300)  /**< 告警：所有分析点分析状态（登录、启停时发送） */
#define IVA_ALARM_RESULT     (301)  /**< 告警：区域分析结果消息 */
#define IVA_ALARM_TRACK      (302)  /**< 告警：通道目标跟踪分类消息 */
#define IVA_ALARM_LMTLIST    (303)  /**< 告警：当前最新流媒体列表（登录时、增删时发送） */
#define IVA_ALARM_DEVLIST    (304)  /**< 告警: 当前最新的设备列表及其登陆状态 */
#define IVA_ALAEM_VOICE      (305)  /**< 告警: 语音分析结果(声纹识别、语音识别)告警 */

#define IVA_SETPARAM_LMT     (400)  /**< 设置：流媒体登录参数 */
#define IVA_SETPARAM_PRESET  (401)  /**< 设置：单个文件/USB/预置位的单个/多个区域分析参数 */
#define IVA_SETPARAM_TURN    (402)  /**< 设置：轮巡任务参数 */
#define IVA_SETPARAM_USER    (403)  /**< 设置：增改用户登录参数 */
#define IVA_SETPARAM_GROUP   (404)  /**< 设置：增改用户组权限参数 */
#define IVA_SETPARAM_RULE    (405)  /**< 设置：分析策略参数 */
#define IVA_SETPARAM_SYSTEM  (406)  /**< 设置：服务端系统运行参数（看门狗将重启服务）*/
#define IVA_SETPARAM_IVAIMG  (407)  /**< 设置：用于功能分析的文件/USB视频流单帧图片 */
#define IVA_SETPARAM_FACEIMG (408)  /**< 设置：注册用户ID的单张人脸图片 */
#define IVA_SETLIST_DEV      (409)  /**< 设置：NVR登录信息列表 */
#define IVA_SETPARAM_CAPIM   (410)  /**< 设置：图片抓拍上传参数 */

#define IVA_GETPARAM_PRESET  (500)  /**< 获取：单个文件/USB/预置位的单个区域分析参数 */
#define IVA_GETPARAM_SYSTEM  (501)  /**< 获取：服务端系统运行参数 */
#define IVA_GETSTATE_PRESET  (502)  /**< 获取：预置位/区域当前分析状态 */
#define IVA_GETSTATE_TURN    (503)  /**< 获取：轮巡任务是否已启动 */
#define IVA_GETLIST_PRESET   (504)  /**< 获取：已配置的分析点列表 */
#define IVA_GETLIST_TURN     (505)  /**< 获取：轮巡任务配置参数列表 */
#define IVA_GETLIST_USER     (506)  /**< 获取：已注册用户列表 */
#define IVA_GETLIST_GROUP    (507)  /**< 获取：已注册用户组列表 */
#define IVA_GETLIST_RULE     (508)  /**< 获取：已配置的分析策略列表 */
#define IVA_GETLIST_LOG      (509)  /**< 获取：服务器日志信息记录 */
#define IVA_GETLIST_ALARMIMG (510)  /**< 获取：服务器告警图片文件名列表 */
#define IVA_GETDATA_ALARMIMG (511)  /**< 获取：服务器单张告警图片base64数据 */
#define IVA_GETLIST_FACEID   (512)  /**< 获取：已注册人脸的用户ID列表 */
#define IVA_GETLIST_FACEIMG  (513)  /**< 获取：某用户ID已注册的人脸图片文件名列表 */
#define IVA_GETDATA_FACEIMG  (514)  /**< 获取：单张人脸图片base64数据 */
#define IVA_GETLIST_DEV      (515)  /**< 获取：已设置NVR登录信息列表 */
#define IVA_GETPARAM_ABILITY (516)  /**< 获取：通道算法能力 */

#define IVA_DELPARAM_PRESET  (600)  /**< 删除：单个文件/USB/预置位的单个区域分析参数 */
#define IVA_DELPARAM_TURN    (601)  /**< 删除：单个轮巡任务参数 */
#define IVA_DELPARAM_USER    (602)  /**< 删除：单个用户登录参数 */
#define IVA_DELPARAM_GROUP   (603)  /**< 删除：单个用户组权限参数 */
#define IVA_DELPARAM_RULE    (604)  /**< 删除：单个分析策略参数 */
#define IVA_DELPARAM_LMT     (605)  /**< 删除：单个流媒体参数 */
#define IVA_DELLIST_ALARMIMG (606)  /**< 删除：服务器告警图片文件列表 */
#define IVA_DELLIST_FACEIMG  (607)  /**< 删除：已注册的人脸图片文件列表 */

#define IVA_START_IVA        (700)  /**< 控制：启动单个文件/USB/预置位的单个/多个（实时策略）区域分析 */
#define IVA_START_TURN       (701)  /**< 控制：启动轮巡分析 */
#define IVA_STOP_IVA         (702)  /**< 控制：停止单个文件/USB/预置位的单个/多个（实时策略）区域分析 */
#define IVA_STOP_TURN        (703)  /**< 控制：停止轮巡分析 */
#define IVA_STOP_ALARM       (704)  /**< 控制：设置单个分析点相同告警或复归消息抑制时间 */

enum IVA_SUPERTYPE                  /** < 分析功能大类   */      /** < 分析功能子类*/
{
    SUPER_UTIVA       = 0,          /** < 私有协议      */
                                                               /** < 0  人脸检测识别  */
                                                               /** < 1  压板状态      */
                                                               /** < 2  指示灯亮灭    */
                                                               /** < 3  二维码识别    */
                                                               /** < 4  区域人员计数   */
                                                               /** < 5  物体倾斜       */
                                                               /** < 6  地线挂拆       */
                                                               /** < 7  值班脱岗       */
                                                               /** < 8  绊线人数识别    */
															   /** < 9  工器具识别    */
															   /** < 10  工程车禁入      */
    SUPER_WORKING     = 1,          /** < 人员作业安全   */
                                                               /** < 0  安全帽         */
                                                               /** < 1  安全带         */
                                                               /** < 2  工作服         */
    SUPER_ACTIVITY    = 2,          /** < 人员行为安全   */
                                                               /** < 0  跌倒           */
                                                               /** < 1  吸烟           */
                                                               /** < 2  打架斗殴        */
                                                               /** < 3  超过限高        */
                                                               /** < 4  人员聚集        */
                                                               /** < 5  人员徘徊        */
    SUPER_CLASSIFY    = 3,          /** < 周界监控       */
                                                               /** < 0  人             */
                                                               /** < 1  车             */
                                                               /** < 2  非机动车        */
                                                               /** < 3  物             */
    SUPER_FIRE        = 4,          /** < 烟火监控       */
                                                               /** < 0  烟火           */
    SUPER_ITEMLEFT    = 5,          /** < 物品移除遗留   */
                                                               /** < 0  物品遗留        */
                                                               /** < 1  物品移除        */
    SUPER_INVADE      = 6,          /** < 入侵检测       */
                                                               /** < 0  绊线入侵        */
                                                               /** < 1  区域入侵        */
    SUPER_VECHILE     = 7,          /** < 车辆管控       */
                                                               /** < 0  禁入区域        */
                                                               /** < 1  禁停区域        */
    SUPER_TEMP_VAL    = 8,          /** < 区域温度阈值检测 */
                                                               /** < 0  区域测温        */
    SUPER_TEMP_DIFF   = 9,          /** < 区域温差检测    */
                                                               /** < 0  温差           */
    SUPER_TEMP_CHANGE = 10,         /** < 区域温度波动检测 */
                                                               /** < 0  温度波动        */
    SUPER_METER       = 11,         /** < 表计读取        */
                                                               /** < 0  单双指针式仪表   */
                                                               /** < 1  LED表计         */
    SUPER_CONNECTOR   = 12,         /** < 刀闸状态        */
                                                               /** < 0  隔离刀闸         */
    SUPER_SWITCH      = 13,         /** < 开关状态        */
                                                               /** < 0  分合翻牌         */
															   /** < 1  空气开关         */
    SUPER_APPEARANCE  = 14,         /** < 外观检测        */
                                                               /** < 0  铭牌            */
                                                               /** < 1  绝缘子           */
                                                               /** < 2  套管            */
                                                               /** < 3  变压器本体       */
                                                               /** < 4  避雷针           */
                                                               /** < 5  爬梯            */
                                                               /** < 6  墙体            */
                                                               /** < 7  杆塔            */
    SUPER_VQLTY       = 15,         /** < 视频质量诊断     */
                                                               /** < 0  图像偏色       */
                                                               /** < 1  条纹干扰       */
                                                               /** < 2  亮度异常       */
                                                               /** < 3  画面抖动       */
                                                               /** < 4  信号丢失       */
                                                               /** < 5  画面冻结       */
                                                               /** < 6  图像模糊       */
                                                               /** < 7  对比度异常      */
                                                               /** < 8  噪声干扰       */
                                                               /** < 9  黑白图像       */
                                                               /** < 10 视频遮挡       */
                                                               /** < 11 视频剧变       */
                                                               /** < 12 场景变更       */
};

#endif
