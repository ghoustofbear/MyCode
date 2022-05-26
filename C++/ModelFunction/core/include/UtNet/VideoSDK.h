#ifndef HIK_VIDEOSDK_H
#define HIK_VIDEOSDK_H

#if (defined(WIN32) || defined(WIN64))
#if defined(VIDEOSDK_EXPORTS)
#define VIDEOSDK_DECLARE extern "C" __declspec(dllexport)
#else
#define VIDEOSDK_DECLARE extern "C" __declspec(dllimport)
#endif // defined(VIDEOSDK_EXPORTS)

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#else
#define VIDEOSDK_DECLARE extern "C"
#ifndef CALLBACK
#define CALLBACK
#endif
#endif  // (defined(WIN32) || defined(WIN64))

#ifndef _IN_
#define _IN_            // 表示入参
#endif

#ifndef _OUT_
#define _OUT_           // 表示出参
#endif

typedef _int64 VIDEO_INT64;  // 统一8字节数据类型

                                ////////////////////////// 错误码定义 ////////////////////////////////
#define VIDEO_ERR_FAIL            -1     // 失败
#define VIDEO_ERR_SUCCESS          0     // 成功
#define VIDEO_ERR_PARAM            1     // 参数错误
#define VIDEO_ERR_INVALID_POINTER  2     // 空指针
#define VIDEO_ERR_NOT_INIT         3     // 未初始化
#define VIDEO_ERR_INVALID_HANDLE   4     // 句柄无效
#define VIDEO_ERR_CREATE_INSTANCE  5     // 创建实例失败
#define VIDEO_ERR_APPLY_MEMORY     6     // 申请内存失败
#define VIDEO_ERR_NET_EXCEPTION    7     // 网络异常
#define VIDEO_ERR_NOT_SUPPORT      8     // 不支持，如只取流时抓图，rtp流查询当前播放时间戳
#define VIDEO_ERR_SET_CALLBACK     9     // 回调设置失败
#define VIDEO_ERR_START_STREAM     10    // 取流失败
#define VIDEO_ERR_STOP_STREAM      11    // 停止取流异常
#define VIDEO_ERR_START_PLAY       12    // 解码失败
#define VIDEO_ERR_STOP_PLAY        13    // 停止解码异常
#define VIDEO_ERR_TIMESTAMP        14    // 时间戳无效
#define VIDEO_ERR_SOUND            15    // 声音相关错误
#define VIDEO_ERR_SNAP             16    // 抓拍失败
#define VIDEO_ERR_GETPLAYTIME      17    // 查询播放时间戳失败
#define VIDEO_ERR_INNER            18    // 内部错误
#define VIDEO_ERR_PLAYCTRL         19    // 播放、下载控制失败
#define VIDEO_ERR_OPER_ORDER       20    // 操作次序不对（如未打开声音就获取音量或设置音量）
#define VIDEO_ERR_DISKSPACE        21    // 磁盘空间不足
#define VIDEO_ERR_LOADLIBARAY      22    // 模块加载失败
#define VIDEO_ERR_VIDEOINFO        23    // 查询视频详情失败
#define VIDEO_ERR_CREATEFILE       24    // 创建文目录或件失败，一般是路径、文件名称含有非法字符，如路径中目录名称全是空格字符串，中英文下的“*”、“|”以及英文下的“?”
#define VIDEO_ERR_OSD              25    // 画面字符叠加错误
#define VIDEO_ERR_TALK             26    // 对讲相关错误
#define VIDEO_ERR_DEVICE           27    // 设备相关错误，如对讲无法检测到声卡设备或声音采集设备
#define VIDEO_ERR_ALLOC_RESOURCE   28    // 分配资源错误
#define VIDEO_ERR_PRIVATEDATA_CTRL 29    // 私有数据控制失败
#define VIDEO_ERR_MAX_ABILITY      30    // 超过最大处理能力，如录像下载或录像需要转封装时达到转封装路数上限
#define VIDEO_ERR_STREAM_TRANSFORM 31    // 流数据转换错误
#define VIDEO_ERR_FILE_INIVALID    32    // 无效文件（断点续传时，分析文件发现不符合断点续传要求时会报此错误码，一般报此错误码表示文件中数据不足，不足以分析断点续传信息）
#define VIDEO_ERR_TIMESTAMP_NOT_IN_RANGE 33  // 录像下载时指定断点续传，但分析出断点处时间不在指定的开始与结束时间之间
#define VIDEO_ERR_FILE_NOT_EXIST   500   // 文件不存在，例如断点续传时，需要传入文件绝对路径名，假如文件不存在，会报该错误
#define VIDEO_ERR_OTHER            10000 // 其它错误


////////////////////////// 回调定义 ///////////////////////////////
// 码流回调
// i64PlayHandle：预览或回放接口返回的句柄
// iStreamDataType：码流数据类型，0-码流头 1-码流数据 2-结束标记
// pDataArray：流数据数组（需当成二进制数据来解析）
// iDataLen：流数据长度
// pUserData：用户数据
typedef void(CALLBACK * pfnStreamCallback)(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData);

// 取流、播放消息回调
// i64PlayHandle：预览或回放接口返回的句柄，当回调的是语音对讲取流消息时，该值无效值（-1）
// iMsg：消息类型，1-播放开始 2-播放结束（视频预览无此消息） 3-播放异常 10-取流开始 11-取流结束 12-取流异常 50-异步时发起取流请求成功 51-异步时发起取流请求失败
typedef void(CALLBACK * pfnMsgCallback)(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData);

// YUV数据回调
// i64PlayHandle：预览或回放接口返回的句柄
// pDataArray：YUV数据数组（需当成二进制数据来解析）
// iDataLen：流数据长度
// iWidth：图像宽度
// iHeight：图像高度
// iFrameType：图像YUV类型（目前为YV12，值为3）
// iTimeStamp：时间戳
// pUserData：用户数据
typedef void(CALLBACK* pfnDecodedDataCallback)(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, int iFrameType, int iTimeStamp, void* pUserData);

// 录像下载回调（进入和消息）
// i64DownloadHandle：网络录像下载句柄
// fPercent：已下载录像进度，当iMsg为0时进度有效
// iMsg：录像下载消息，0-开始录像下载 1-录像下载中 2-录像下载完成 3-录像下载即将分包 4-录像下载分包失败 5-录像下载分包完成 6-录像下载时断流 30-转封装不支持 50-断点续传不支持 100-下载失败（内部错误）
// pUserData：用户自定义数据
typedef void (CALLBACK* pfnDownloadCallback)(VIDEO_INT64 i64DownloadHandle, float fPercent, int iMsg, void* pUserData);

////////////////////////// 结构体定义 //////////////////////////////////
typedef struct stPlayReq
{
    int iHardWareDecode;                // 是否开启GPU硬解 0-不开启 1-开启（如果开启硬解，但如显卡不支持等导致错误，SDK内部自动切换成软解；开启硬解只是增加显示性能，并不能减少内存占用率，如果不是特别需求，建议不开启硬解）
    pfnStreamCallback fnStream;         // 码流数据回调
    pfnMsgCallback fnMsg;               // 播放、取流消息回调
    pfnDecodedDataCallback fnDecodedStream;     // 解码后的YUV数据回调，注意开启了GPU硬解不支持回调解码后数据，即时指定了回调也不起效
    void* pUserData;                    // 用户数据
    char szReserve[64];                 
}VIDEO_PLAY_REQ, *PVIDEO_PLAY_REQ;

typedef struct stFilePlayReq
{
    pfnMsgCallback fnMsg;               // 播放、取流消息回调
    void* pUserData;                    // 用户数据
    char szReserve[64];                 // 保留参数
}VIDEO_FILEPLAY_REQ, *PVIDEO_FILEPLAY_REQ;

typedef struct stDownloadReq
{
    pfnDownloadCallback fnDownload;     // 下载进度回调
    void* pUserData;                    // 用户数据
    VIDEO_INT64 i64FileMaxSize;         // 录像分包大小
    VIDEO_INT64 i64RecordSize;          // 录像总大小，请填充查询回放URL时查询到的各录像片段大小之和
    VIDEO_INT64 i64StartTimeStamp;      // 录像下载开始时间，请填充查询回放URL时的查询开始时间戳，单位：秒
    VIDEO_INT64 i64EndTimeStamp;        // 录像下载结束时间，请填充查询回放URL时的查询结束时间戳，单位：秒
    char szReserve[64];             	// 录像类型，填充“0”或不填充-普通录像下载，“1”-录像转封装，如果音频或视频不支持则回调不支持消息 “2” - 录像转封装，但音频或视频不支持时按不转封装处理 “3”-录像转封装，如果音频不支持则只转视频，如果视频也不支持则回调不支持消息 “4”-启用断点续传 其它值 - 参数错误（对于指定录像转封装和启用断点续传，目前仅支持海康SDK协议、EHOME协议接入设备，如果是其它协议接入的设备，会回调不支持的消息；对于断点续传，只支持未转封装的录像；调录像下载时可以先查询设备接入协议，非海康SDK、EHOME协议不填充或填充“0”即可；转封装是将视频转换成标准MP4文件，可以在通用播放器上播放，如不转封装，需要专用播放器播放，如大华SDK接入设备下载的录像需要大华播放器，海康SDK协议/EHOME协议下载录像需要海康播放器播放）
}VIDEO_DOWNLOAD_REQ, *PVIDEO_DOWNLOAD_REQ;

typedef struct stVideoDetailInfo
{
    int iWidth;                         // 视频图像宽度
    int iHeight;                        // 视频图像高度
    int iEncodeType;                    // 编码类型  1-H264 2-MPEG2_PS 3-MPEG4 4-H265 5-GB28181 6-raw 7-vag 8-dahua 9-smartH264 10-smartH265
    int iEncapsulationType;             // 封装类型  1-ps 2-ts 3-rtp 4-raw 5-rtp + ps
    VIDEO_INT64 i64FrameRate;           // 帧率    
    char szReserve[64];                 // 保留参数
	float fCodeRate;                    // 码率（V1.2.0新增）
}VIDEO_DETAIL_INFO, *PVIDEO_DETAIL_INFO;

typedef struct stOSDInfo
{
    int iBold;                  // 叠加字符串是否粗体 1-粗体 0-非粗体
    int ix;                     // 叠加字符相对与播放窗口左上角起点的横坐标
    int iy;                     // 叠加字符相对与播放窗口左上角起点的纵坐标
    int iFontSize;              // 字体大小
    int iAlignType;             // 对齐方式，0-左对齐 1-居中对齐 2-右对齐
    VIDEO_INT64 i64Color;       // 字符颜色，Windows中使用RGB宏获得颜色值
    char szReserve[64];         // 保留参数
}VIDEO_OSD_INFO, *PVIDEO_OSD_INFO;

////////////////////////// 接口定义 /////////////////////////////
// SDK初始化
// pszEvn：保留参数，传NULL即可
// 备注：调一次即可，初始化成功情况下重复调直接返回成功
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_Init(_IN_ const char* pszEvn);

// SDK反初始化
// 备注：调一次即可，反初始化成功情况下重复调直接返回成功
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_Fini();

// 同步开始视频预览（支持取实时流、回调取流与播放的消息，以及回调YUV数据，需要播放请求参数中指定）
// pszUrl：预览URL，每次预览需要重新查询URL
// hWnd：视频预览的Windows窗口句柄
// pstPlayReq：播放请求参数，具体参数详见结构体定义
// 同步返回结果，成功返回大于0的预览句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartPreview(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// 异步开始视频预览（支持取实时流、回调取流与播放的消息，以及回调YUV数据，需要播放请求参数中指定）
// pszUrl：预览URL，每次预览需要重新查询URL
// hWnd：视频预览的Windows窗口句柄
// pstPlayReq：播放请求参数，具体参数详见结构体定义
// 异步返回结果，成功返回大于0的预览句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码；返回值只表示发起异步请求的结果，不代表发起取流请求的结果，取流请求结果成功与否参考消息回调中的“异步发起取流请求成功”与“异步发起取流请求失败”消息
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartAsynPreview(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// 停止预览（停止取流，停止回调消息，停止回调YUV数据）
// i64PlayHandle：预览句柄，来源于Video_StartPreview、Video_StartEzvizPreview返回值
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StopPreview(_IN_ VIDEO_INT64 i64PlayHandle);

// 同步网络录像回放、倒放（支持取网络录像流、回调取流与播放的消息，以及回调YUV数据，需要播放请求参数中指定）
// pszUrl：回放URL
// hWnd：网络录像正放或倒放的Window窗口句柄
// i64StartTimeStamp：正放时为查询回放URL时的开始时间戳，倒放时为查询回放URL时的结束时间戳，单位：秒
// i64EndTimeStamp：正放时为查询回放URL时的结束时间戳，倒放时为查询回放URL时的开始时间戳，单位：秒
// pstPlayReq：播放请求参数，详细参数详见结构体定义
// 同步返回结果，成功返回大于0的播放句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartPlayback(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ VIDEO_INT64 i64StartTimeStamp, _IN_ VIDEO_INT64 i64EndTimeStamp, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// 异步网络录像回放、倒放（支持取网络录像流、回调取流与播放的消息，以及回调YUV数据，需要播放请求参数中指定）
// pszUrl：回放URL
// hWnd：网络录像正放或倒放的Window窗口句柄
// i64StartTimeStamp：正放时为查询回放URL时的开始时间戳，倒放时为查询回放URL时的结束时间戳，单位：秒
// i64EndTimeStamp：正放时为查询回放URL时的结束时间戳，倒放时为查询回放URL时的开始时间戳，单位：秒
// pstPlayReq：播放请求参数，详细参数详见结构体定义
// 异步返回结果，成功返回大于0的播放句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码；返回值只表示发起异步请求的结果，不代表发起取流请求的结果，取流请求结果成功与否参考消息回调中的“异步发起取流请求成功”与“异步发起取流请求失败”消息
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartAsynPlayback(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ VIDEO_INT64 i64StartTimeStamp, _IN_ VIDEO_INT64 i64EndTimeStamp, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// 停止网络录像正放、倒放（停止取流，停止回调消息，停止回调YUV数据）
// i64PlayHandle：网络录像播放句柄，来源于Video_StartPlayback的返回值
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StopPlayback(_IN_ VIDEO_INT64 i64PlayHandle);

// 查询录像回放当前播放时间戳
// i64PlayHandle:网络录像播放句柄，来源于Video_StartPlayback、Video_StartEzvizPlayback的返回值
// 成功返回大于等于0的当前回放时间戳，失败返回VIDEO_ERR_FAIL，单位：秒。接口返回失败通过调用Video_GetLastError获取错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetCurrentPlayTime(_IN_ VIDEO_INT64 i64PlayHandle);

// 开始文件播放
// pszFileName：绝对路径mp4文件名称
// hWnd：文件播放窗口句柄
// pstFilePlayReq：文件播放请求参数结构体，不需回调消息，可传NULL
// 备注：仅支持播放通过海康SDK协议、Ehome/ISUP协议、ONVIF协议、大华SDK协议、国标协议、萤石接入的设备紧急录像、录像剪辑和录像下载的文件
// 成功返回大于0的文件播放句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartFilePlay(_IN_ const char* pszFileName, _IN_ void* hWnd, _IN_ PVIDEO_FILEPLAY_REQ pstFilePlayReq);

// 停止文件播放
// i64PlayHandle：文件播放句柄，来源于Video_StartFilePlay的返回值
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StopFilePlay(_IN_ VIDEO_INT64 i64PlayHandle);

// 查询文件播放总时长
// i64PlayHandle:文件播放句柄，来源于Video_StartFilePlay的返回值
// 成功返回大于等于0的总时长，失败返回VIDEO_ERR_FAIL，单位：秒。接口返回失败通过调用Video_GetLastError获取错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetFilePlayDuration(_IN_ VIDEO_INT64 i64PlayHandle);

// 查询文件已播放时长
// i64PlayHandle：文件播放句柄，来源于Video_StartFilePlay的返回值
// 成功返回大于等于0的已播放时长，失败返回VIDEO_ERR_FAIL，单位：秒。接口返回失败通过调用Video_GetLastError获取错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetFilePlayedTime(_IN_ VIDEO_INT64 i64PlayHandle);

// 开始录像下载
// pszUrl:正放取流URL
// pszFileName:录像文件绝对录像名称，请指定后缀mp4，如“E:\test.mp4”。pszFileName为ansi编码字符串。路径和文件名称不能包含特殊字符（中英文下的“*”、“|”以及英文下的“?”），此外，路径中慎用“.”和“..”。录像下载不支持下载到网络位置,只能下载到本地。对于pstDownloadReq中指定断点续传时，请传入上次录像下载最后生成的一个文件的绝对路径文件名称，如果不支持会通过回调给出消息
// pstDownloadReq:录像下载请求参数，详细参数详见结构体定义
// 备注：支持通过配置文件VideoSDK.int配置：非海康流时，取流缓冲大小；海康流时是否转封装已支持普通播放器播放
// 成功返回大于0的播放句柄，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartDownload(_IN_ const char* pszUrl, _IN_ const char* pszFileName, _IN_ PVIDEO_DOWNLOAD_REQ pstDownloadReq);

// 停止下载
// i64DownloadHandle：录像下载句柄，来源于Video_StartDownload的返回值
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StopDownload(_IN_ VIDEO_INT64 i64DownloadHandle);

// 开始本地录像，对于同一路，同一时刻只支持一路本地录像，否则返回不支持
// i64PlayHandle：播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview或Video_StartAsynPlayback接口返回
// pszFileName:录像文件绝对路径名称，请指定后缀mp4，如“E:\test.mp4”。pszFileName为ansi编码字符串。路径和文件名称不能包含特殊字符（中英文下的“*”、“|”以及英文下的“?”），此外，路径中慎用“.”和“..”。
// i64FileMaxSize：分包大小单位：字节，不得小于5M，否则按5M计算；分包后命名规则：“E:\test-1.mp4”、...“E:\test-n.mp4”，n表示除“E:\test.mp4”外的第n个分包
// recordType：录像类型，0-不转封装，1-录像转封装如果音频或视频不支持则返回错误 2-录像转封装但音频或视频不支持时按不转封装处理（前提是存在音频、视频数据） 3-录像转封装如果音频不支持则只转视频 其它值-参数错误（1~3针对海康流，如果当前视频是非海康流，接口返回不支持）
// 有些设备出的混合流中音频流不支持转封装，此时只将视频流转封装
// 有些设备出的流不支持转封装（如大华设备），此时将不转封装
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StartLocalRecord(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ const char* pszFileName, _IN_ VIDEO_INT64 i64FileMaxSize, _IN_ int recordType);

// 停止本地录像
// i64PlayHandle：播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview或Video_StartAsynPlayback接口返回
// 成功返回码流存文件个数，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_StopLocalRecord(_IN_ VIDEO_INT64 i64PlayHandle);

//播放、下载控制
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartAsynPlayback或Video_StartDownload接口返回
// iCtrlType:0-暂停 1-恢复播放 2-定位播放 3-倍速播放 4-暂停下载 5-恢复下载
// i64Param:控制参数，与iCtrlType配合使用
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
// 备注：1-恢复播放是恢复到1倍速，而不是上次的播放速度；2-时间戳参数由调用方保证介于开始与结束时间戳之间，单位：秒；3-控制参数为速度取值：16、8、4、2、1、-2、-4、-8、-16，依次由快放至慢放
VIDEOSDK_DECLARE int Video_PlayCtrl(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iCtrlType, _IN_ VIDEO_INT64 i64Param);

//主子码流切换
// iPlayHandle:播放句柄，Video_StartPreview或Video_StartAsynPreview返回
// iAsyn：是否异步切换，1-异步，0-同步；如果是异步切换，切换成功消息由消息回调给出，否则接口返回值表明是否切换成功，取其它值则返回失败
// pszParam：主子码流切换参数，该参数填充指定码流类型的预览取流url
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_ChangeStreamType(VIDEO_INT64 i64PlayHandle, int iAsyn, const char* pszParam);

// 播放抓图
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// pszFileName:要保存的图片绝对路径（含文件名），根据文件名后缀确定保存的图片格式（支持bmp、jpg），例：“E:/SnapShot/测试监控点.jpg”。pszFileName为ansi编码字符串。路径和文件名称不能包含特殊字符（中英文下的“*”、“|”以及英文下的“?”），此外，路径中慎用“.”和“..”
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_PlaySnap(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ const char* pszFileName);

// 声音控制
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// iCtrlType:控制类型，0-打开 1-关闭，其它值-未定义。
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
// 备注：声音功能需要流数据中有音频数据才有效。
VIDEOSDK_DECLARE int Video_SoundCtrl(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iCtrlType);

// 获取音量
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// 成功返回音量值，音量范围[0,100]，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
// 备注：需要打开声音后才有效。
VIDEOSDK_DECLARE int Video_GetVolume(_IN_ VIDEO_INT64 i64PlayHandle);

// 设置音量
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// iVolume:音量值，范围[0,100]
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
// 备注：需要打开声音后才有效。
VIDEOSDK_DECLARE int Video_SetVolume(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iVolume);

// 画面字符叠加
// i64PlayHandle:播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// iOSDId:字符叠加标识，第一次叠加时请传入0，更新画面上的字符串时填入第一次叠加时返回的Id
// pszText:待叠加字符串，支持“\n”换行，字符串不超过512个字节，如超过则截断显示
// pstOSDInfo:字符叠加信息
// 成功返回本次叠加的id，该Id取值大于等于0，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_SetOSDText(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iOSDId, _IN_ const char* pszText, _IN_ PVIDEO_OSD_INFO pstOSDInfo);

// 开始语音对讲
// pszUrl:语音对讲URL，可从OpenAPI查询对讲URL。每次对讲都需重新查询对讲URL
// pszFileName:语音对讲保存音频的绝对路径文件名称，只支持wav后缀，如“D:\test.wav”。如不需要录音，该参数请传NULL。路径和文件名称不能包含特殊字符（中英文下的“*”、“|”以及英文下的“?”），此外，路径中慎用“.”和“..”。暂时不支持保存音频文件功能，该参数请填充NULL。
// fnMsg:语音对讲消息，用于回调语音对讲取流消息，如不需要可传NULL。
// pUserData:用户数据，用于fnMsg回调中透传出去，如不需要可传NULL。
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
// 备注：同一时刻只支持一路对讲，暂无法区分语音对讲失败还是语音对讲通道被占用。只支持与前端设备对讲，前端IPC设备接入NVR后，支持与前端IPC设备对讲，不支持直接与NVR对讲。每次对讲都需重新查询对讲URL。暂不支持保存音频文件功能，pszFileName参数请填充NULL。语音对讲只支持海康SDK和Ehome协议接入设备。
VIDEOSDK_DECLARE int Video_StartTalk(_IN_ const char* pszUrl, _IN_ const char* pszFileName, _IN_ pfnMsgCallback fnMsg, _IN_ void* pUserData);

// 停止语音对讲
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL。接口返回失败请调用Video_GetLastError获取错误码，通过错误码判断出错原因
VIDEOSDK_DECLARE int Video_StopTalk();

// 查询视频详细信息
// i64PlayHandle：播放句柄，来源于Video_StartPreview、Video_StartPlayback、Video_StartAsynPreview或Video_StartAsynPlayback返回值
// pstVideoDetailInfo：视频详细信息结构体指针
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL。接口返回失败请调用Video_GetLastError获取错误码，通过错误码判断出错原因
VIDEOSDK_DECLARE int Video_GetVideoInfo(_IN_ VIDEO_INT64 i64PlayHandle, _OUT_ PVIDEO_DETAIL_INFO pstVideoDetailInfo);

// 私有数据控制（指画面上的如移动侦测的线框等数据的开关控制，针对海康的设备，支持海康SDK、EHOME/ISUP、ONVIF、GB28181协议接入）
// i64PlayHandle：播放句柄参数，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// iPrivateDataType：私有数据类型，多个可使用“|”运算进行批量控制，0x0001-智能分析 0x0002-移动侦测 0x0004-POS信息后叠加 0x0008-图片叠加 0x0010-热成像信息 0x0020-温度信息
// iCtrlType：显示隐藏开关 0-显示 1-隐藏
// iParam：一些私有数据类型存在子类型，当iDataType为0x0010时，子类型有0x0001-点火框显示 0x0002-最高温度 0x0004-最高温度位置 0x0008-最高温度距离；当iDataType为0x0020时，子类型有0x0001-框测温 0x0002-线测温 0x0004-点测温
// 注：对于不涉及子类型（或不需子类型）的，支持各个私有数据类型的“|”运算，对于涉及子类型的，需一个一个对主类型进行控制，同一类型的子类型支持“|”运算；iDataType为0x0010和0x0020时需要热成像设备支持，其它类型普通设备即可支持，但需要设备上配置
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码，非海康设备不支持会，目前无法区分是否支持
VIDEOSDK_DECLARE int Video_PrivateDataCtrl(_IN_ VIDEO_INT64 iPlayHandle, _IN_ int iPrivateDataType, _IN_ int iCtrlType, _IN_ int iParam);

// 刷新播放
// i64PlayHandle：播放句柄，Video_StartPlayback、Video_StartPreview、Video_StartAsynPreview、Video_StartAsynPlayback或Video_StartFilePlay接口返回
// 成功返回VIDEO_ERR_SUCCESS，失败返回VIDEO_ERR_FAIL，失败了通过Video_GetLastError查询错误码
VIDEOSDK_DECLARE int Video_RefreshPlay(VIDEO_INT64 i64PlayHandle);

// 获取错误码
VIDEOSDK_DECLARE int Video_GetLastError();

#endif  // HIK_VIDEOSDK_H