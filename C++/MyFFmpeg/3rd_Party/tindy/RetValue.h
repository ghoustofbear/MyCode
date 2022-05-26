/************************************************************************/
/*COMMON RETURN                                                                     
*/
/************************************************************************/
#define RET_NVD_ISVIEW						1       //网络视频解码器SDK接口返回值，存在正在连接的NVS
#define RET_NOTHING						((void)0)
#define RET_SUCCESS							0		//成功
#define RET_FAILED							-1		//失败
#define RET_NVD_INVALIDID					-1		//网络视频解码器SDK返回返回值，指定的解码器不存在
#define RET_MALLOC_FALIED					-2		//申请内存失败
#define RET_NVD_UNINIT						-2		//网络视频解码器SDK接口返回值，开发包尚未初始化/初始化失败
#define RET_INVALIDPARA						-3		//传入非法参数
#define RET_FAILED_UPGRADE					-4		//网络视频服务器SDK接口返回值，升级失败
#define RET_NVD_OUTMEMERY					-4		//网络视频解码器SDK接口返回值，内存分配错误
#define RET_APPLYMEMORY_FAILED				-5		//内存申请失败
#define RET_NVDNOACCOUNT					-5		//网络视频解码器SDK接口返回值，用户名错误
#define RET_NVDNOPASS						-6		//网络视频解码器SDK接口返回值，密码错误
#define RET_INVALIDFILE						-7		//文件格式非法
#define RET_NOTLOGON						-8		//没有登录，或登陆句柄已失效
#define RET_LOGING							-9		//已登录或正在登录
#define RET_NVD_MAXDEC						-10		//达到最大连接数
#define	RET_BUFFER_FULL						-11		//缓冲区满，数据没有放入缓冲区
#define RET_NVD_UPGRADING					-11		//网络视频解码器SDK升级接口返回值，正在升级
#define RET_FORBID_OPT						-12		//不允许操作
#define RET_NVD_RELOOPITEM					-13		//向循环列表中添加重复的连接
#define RET_NVD_RECONNECT					-14		//重复连接NVS
#define RET_INVALIDPOS						-15		//位置越界
#define RET_NOCONNECTED						-16		//还没有建立连接
#define RET_POS_X_Y							-17		//坐标非法
#define	RET_BUFFER_WILL_BE_FULL				-18		//即将满，降低送入数据的频率
#define RET_NVD_NULL_PTR					-18		//null pointer
#define	RET_BUFFER_WILL_BE_EMPTY			-19		//即将空，提高送入数据的频率
#define RET_NVD_UNDEFINED_BEHAVIOR			-19		//网络视频解码器SDK接口返回值， undefined behavior
#define RET_BUFFER_IS_OK					-20		//正常可以放数据
#define RET_NVD_INVALID_BUFF_SIZE			-20		//网络视频解码器SDK接口返回值，不合法的缓冲区大小
#define RET_INVALID_ARRAY_INDEX				-21		//不合法的数组下标
#define RET_NVD_INVALID_PARAM				-22		//参数不合法
#define RET_REPEAT_SET						-23		//重复设置
#define RET_BUILD_PROTOCOL					-24		//build protocol error
#define RET_SEND_PROTOCOL					-25		//send protocol error
#define RET_NVD_NO_IDLE_ITEM				-26		//no idle item
#define RET_INVALID_FILEHEADER				-27		//invalid file header
#define RET_LIGHTMODE_NOTSUPPORT			-28		//sdk light mode not support get para
#define RET_LOAD_OSCORE						-29		//sdk load OsCore.dll fail
#define RET_INVALID_OPT_TYPE				-30		//非法的操作类型
#define RET_NOT_FIND_VALIED_PLAYER_ID		-31		//not find valild playerid
#define RET_STARTPLAY_FAILED				-32		//start play failed

#define RET_GET_VIDEOSIZE_FAILED			-40		//获取分辨率失败
#define RET_GET_DEVICETYPE_FAILED			-41		//获取串口设备类型失败
#define RET_GET_SENSORFLIP_FAILED			-42		//获取CMOS镜头翻转失败

#define RET_GETDATA_END						-50		//获取数据正常结束，读到文件尾或者播放停止位置
#define RET_NVD_ERR_SYSTEM					-100	//系统错误				
#define RET_INVALID_PARA					-101	//非法参数
#define RET_DEVICE_NOT_LOGON				-102	//设备未登录
#define RET_MEMORY_OVER						-103	//内存不足
#define RET_ERR_AUTHORITY					-104	//权限不足
#define RET_SEND_LIST_FULL					-105	//发送列表满了
#define RET_DEVICE_CAPTURE_FAIL				-106	//设备远程抓拍失败，设备返回的数据长度为0
#define RET_DEVICE_CAPTURE_TIMEOUT			-107	//设备远程抓拍超时，设备未回复数据
#define RET_INVALID_LOGONID					-108	//传入的登陆句柄失效或非法
#define RET_CALLCURLAPI_FALIED				-109	//调用curl库接口失败
#define RET_INVALID_MEMORY					-110	//非法内存
#define RET_NEW_CLASS_FAILED				-111	//新建class失败

#define RET_SYSTEM_CALL_FAILED				-200	//调用系统函数失败
#define RET_SDK_CALL_FAILED     			-201	//调用SDK接口失败
#define RET_INNER_CALL_FAILED				-202	//调用内部函数失败
#define RET_DRAW_CREATE_FAILED				-203	//创建Draw失败
#define RET_NOSUPPORT_PARATYPE				-204	//轻量级获取参数不支持的参数类型
#define RET_LIGHTLOGON_NETERROR				-205	//轻量级发协议网络错误
#define RET_LIGHTLOGON_GET_TIME_OUT			-206	//轻量级发协议等待设备回复超时
#define RET_DEV_NOT_SUPPORT					-207	//设备不支持当前参数获取方式
#define RET_GETREGDEVCOUNT_TIMEOUT			-208	//带目录的主动模式获取注册设备总数超时
#define RET_ERR_RECREATE_DECODER			-209	//需要重新创建解码器
#define RET_FACE_CUT_QUERY_TIMEOUT			-210	//查询人脸抠图结果超时返回
#define RET_GETREGASSIGNPROXY_TIMEOUT		-211	//向目录服务器获取给设备分配的代理信息超时
#define RET_QUERYREGNVSBYID_TIMEOUT			-212	//根据出厂ID向目录服务器查询注册设备信息超时
#define RET_QUERYNVSBYDOMAINNAME_TIMEOUT	-213	//根据域名向目录服务器查询注册设备信息超时

#define RET_SYNCLOGON_TIMEOUT				-300	//同步登录超时，网络正常，但对端无任何响应
#define RET_SYNCLOGON_USENAME_ERROR			-301	//同步登录失败，用户名不存在，默认用户名：admin
#define RET_SYNCLOGON_USRPWD_ERROR			-302	//同步登录失败，密码错误，默认密码：1111，如果用户修改过密码请输入正确密码。
#define	RET_SYNCLOGON_PWDERRTIMES_OVERRUN	-303	//同步登录失败，密码错误次数超限，账号已锁定
#define RET_SYNCLOGON_NET_ERROR				-304	//同步登录失败，网络连接错误
#define RET_SYNCLOGON_PORT_ERROR			-305	//同步登录失败，通信端口输入有误，默认传入端口3000
#define RET_SYNCLOGON_UNKNOW_ERROR			-306	//同步登录失败，错误未知
#define RET_SYNCREALPLAY_TIMEOUT			-307	//同步连接视频超时，对端没有发送视频头协议
#define RET_SYNCREALPLAY_FAIL				-308	//同步连接视频时播放视频失败
#define RET_SYNCSUSPENDVCA_CONFIGING		-309	//同步暂停智能分析失败，智能分析参数正被其他客户端设置
#define RET_SYNCSUSPENDVCA_FAIL				-310	//同步暂停智能分析失败，设备未回码
#define RET_SYNCOPENVCA_CONFIGING			-311	//同步开启智能分析失败，智能分析参数正被其他客户端设置
#define RET_SYNCOPENVCA_FAIL				-312	//同步开启智能分析失败，设备未回码
#define RET_SYNCOPTVCA_TIMEOUE				-313	//同步暂停/开启 智能分析超时
#define RET_SYNCTALK_SOCKET_ERR				-320	//同步对讲发送协议失败，网络错误
#define RET_SYNCTALK_INNERCON_ERR			-321	//同步对讲开启双向对讲失败
#define RET_SYNCTALK_BEGIN_ERR				-322	//同步对讲开启回复失败
#define RET_SYNCTALK_END_ERR				-323	//同步对讲停止回复失败
#define RET_SYNCTALK_DEV_ERR				-324	//同步对讲设备回复失败
#define RET_SYNCTALK_RECSOUND_ERR			-325	//同步对讲采集音频数据失败
#define RET_SYNCCAPTURE_INVALID_MEM			-329	//同步抓拍接口传入非法内存地址，一般是结构体内部指针变量没有申请内存空间
#define RET_SYNCMSG_TIMEOUT					-330	//同步接口等待消息超时返回值


#define RET_DEC_NOT_INIT					-401	//解码器未初始化
#define RET_DEC_NO_FREE_HANDLE				-402	//解码器句柄没有空闲的，已使用完
#define RET_DEC_ILLEGAL_HANDLE				-403	//解码器句柄不合法
#define RET_DEC_ILLEGAL_PARAM				-404	//解码器传入参数不合法
#define RET_DEC_NOT_CREATE					-405	//解码器未创建
#define RET_DEC_CREATE_FAILED				-406	//解码器创建失败
#define RET_DEC_MEMORY_OVER					-407	//解码器内存不足
#define RET_DEC_NOT_SHOW_PRE_I				-408	//解码器不显示上一个I帧
#define RET_DEC_HS_LOAD_FAILED				-410	//解码器HS加载库失败
#define RET_DEC_HS_NEED_MORE				-411	//解码器HS流解码，数据不够
#define RET_DEC_HS_NO_PIC					-412	//解码器HS解码失败，没出图像输出
#define RET_DEC_HS_ERR_HANDLE				-413	//解码器HS句柄为空或输出参数为空
#define RET_DEC_HS_FAILED					-414	//解码器HS解码失败
#define RET_DEC_FF_LOAD_FAILED				-420	//解码器FF加载库失败
#define RET_DEC_FF_NOT_FIND_DECODER			-421	//解码器FF查找失败
#define RET_DEC_FF_ALLOC_FAILED				-422	//解码器FF申请内存失败
#define RET_DEC_FF_OPEN_FAILED				-423	//解码器FF打开上下文失败
#define RET_DEC_FF_VIDEO2_FAILED			-424	//解码器FF视频解码失败
#define RET_DEC_FF_AUDIO_ILLEGAL_HEAD		-425	//解码器FF音频头不对
#define RET_DEC_FF_AUDIO_LESS				-426	//解码器FF音频数据不够
#define RET_DEC_FF_AUDIO4_FAILED			-426	//解码器FF音频解码失败
#define RET_DEC_FF_FAILED					-427	//解码器FF解码失败
#define RET_DEC_FF_HW_UNSUPPORT				-428	//解码器FF硬解不支持的类型





