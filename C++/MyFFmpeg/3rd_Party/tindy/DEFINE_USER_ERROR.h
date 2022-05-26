#ifndef _DEFINE_USER_ERROR_H
#define _DEFINE_USER_ERROR_H

//提供给用户的详细错误说明
#define USER_ERROR  0x10000000
#define ERROR_NO_DEV					(USER_ERROR+0x01)   //没有找到设备
#define ERROR_UNLOGON					(USER_ERROR+0x02)   //未登录
#define ERROR_PARAM_OVER				(USER_ERROR+0x03)   //参数越界
#define ERROR_REOPERATION				(USER_ERROR+0x04)   //重复操作
#define ERROR_WSASTARTUP				(USER_ERROR+0x05)   //WSAStartup执行失败
#define ERROR_CREATEMSG					(USER_ERROR+0x06)   //创建消息循环失败
#define ERROR_NOSUPPORTRECORD   		(USER_ERROR+0x07)   //不支持前端录像　
#define ERROR_INITCHANNELNET    		(USER_ERROR+0x08)   //通道网络创建失败
#define ERROR_CREATEDDRAW       		(USER_ERROR+0x09)   //无法创建更多的DirectDraw
#define ERROR_NO_CHANNEL				(USER_ERROR+0x0A)   //通道没有创建
#define ERROR_NO_FUN            		(USER_ERROR+0x0B)   //无此功能
#define ERROR_PARAM_INVALID     		(USER_ERROR+0x0C)   //参数无效 
#define ERROR_DEV_FULL					(USER_ERROR+0x0D)   //设备列表已满
#define ERROR_LOGON						(USER_ERROR+0x0E)	//设备已经登录，正在登录
#define ERROR_CREATECPUTHREAD   		(USER_ERROR+0x0F)	//创建CPU检测线程失败
#define ERROR_CREATEPLAYER      		(USER_ERROR+0x10)   //创建Player失败
#define ERROR_NOAUTHORITY				(USER_ERROR+0x11)   //权限不足
#define ERROR_NOTALK            		(USER_ERROR+0x12)   //未对讲
#define ERROR_NOCALLBACK				(USER_ERROR+0x13)   //没有设备回调函数
#define ERROR_CREATEFILE        		(USER_ERROR+0x14)   //创建文件失败
#define ERROR_NORECORD					(USER_ERROR+0x15)   //不是从当前Player发起的录像
#define ERROR_NOPLAYER					(USER_ERROR+0x16)   //没有对应Player
#define ERROR_INITCHANNEL       		(USER_ERROR+0x17)   //通道没有初始化
#define ERROR_NOPLAYING         		(USER_ERROR+0x18)   //Player没有播放
#define ERROR_PARAM_LONG				(USER_ERROR+0x19)   //字符串参数长度过长
#define ERROR_INVALID_FILE      		(USER_ERROR+0x1A)   //文件不符合要求
#define ERROR_USER_FULL					(USER_ERROR+0x1B)   //用户列表已满
#define ERROR_USER_DEL					(USER_ERROR+0x1C)  	//当前用户无法删除
#define ERROR_CARD_LOAD					(USER_ERROR+0x1D)	//加载卡dll失败
#define ERROR_CARD_CORE					(USER_ERROR+0x1E)	//加载卡内核失败
#define ERROR_CARD_COREFILE				(USER_ERROR+0x1F)	//加载卡内核文件失败
#define ERROR_CARD_INIT					(USER_ERROR+0x20)	//卡初始化失败
#define ERROR_CARD_COREREAD				(USER_ERROR+0x21)	//读取卡内核文件失败
#define ERROR_CHARACTER_LOAD			(USER_ERROR+0x22)	//加载字库失败
#define ERROR_NOCARD					(USER_ERROR+0x23)   //卡未初始化
#define ERROR_SHOW_MODE					(USER_ERROR+0x24)   //显示模式未设置
#define ERROR_FUN_LOAD					(USER_ERROR+0x25)	//函数未加载
#define ERROR_CREATE_DOWNLOAD			(USER_ERROR+0x26)	//没有更多的下载通道可用
#define	ERROR_PROXY_DELACT				(USER_ERROR+0x27)   //删除的是主动模式的服务器
#define ERROR_PROXY_HASCONNECT			(USER_ERROR+0x28)	//还有连接
#define ERROR_PROXY_NOPROXY				(USER_ERROR+0x29)   //代理没有启动
#define ERROR_PROXY_IDENTITY			(USER_ERROR+0x2A)	//不是代理的身份
#define	ERROR_CONNECT_MAX				(USER_ERROR+0x2B)	//连接已经到达最大
#define	ERROR_NO_DISK					(USER_ERROR+0x2C)	//没有挂接存储设备
#define	ERROR_NO_DRAW					(USER_ERROR+0x2D)	//没有足够的Draw
#define	ERROR_INIT_DATA_CHAN			(USER_ERROR+0x2E)	//初始化数据通道失败
#define	ERROR_SOCKET_CLOSED				(USER_ERROR+0x2F)	//socket关闭
#define	ERROR_SOCKET_SEND_DATA			(USER_ERROR+0x30)	//socket发送数据失败
#define	ERROR_DO_UPGRADING				(USER_ERROR+0x31)	//正在升级
#define ERROR_ALREADY_INTERTALK			(USER_ERROR+0x32)	//正在进行双向对讲
#define ERROR_FUNCTION_NOT_SUPPORTED	(USER_ERROR+0x33)	//功能不被此设备所支持
#define ERROR_DISPLAY_ON_OTHER_WND		(USER_ERROR+0x34)	//正在其它窗口中显示
#define ERROR_BUFFER_TOO_SMALL			(USER_ERROR+0x35)	//缓冲区太小
#define ERROR_NVR_NOT_SUPPORT			(USER_ERROR+0x36)	//此设备不支持NVR相关功能
#define	ERROR_SOCKET_NULL				(USER_ERROR+0x37)	//socket未创建或已释放
#define	ERROR_EVENT_NOT_ENABLE			(USER_ERROR+0x38)	//本规则中此事件没有使能
#define ERROR_RULE_NOT_ENALE			(USER_ERROR+0x39)	//此规则没有使能
#define	ERROR_INIT_CHAN					(USER_ERROR+0x3A)	//初始化通道失败
#define	ERROR_PROLEN_INVALID 			(USER_ERROR+0x3B)	//协议长度不合法
#define	ERROR_UNSUPPORT_CHARSET 		(USER_ERROR+0x3C)	//不支持的字符集
#define ERROR_CHARTRANS_FAILED			(USER_ERROR+0x3D)	//字符转码失败
#define ERROR_WAITREPLY_FAILED			(USER_ERROR+0x3E)	//等待设备或目录服务器回码失败(未回码或超时)
#define ERROR_NOTFOUND_PLAYERID			(USER_ERROR+0x3F)	//SDK内部未找到合法的playerID
#define ERROR_UNSUPPORT_STREAM			(USER_ERROR+0x40)	//不支持码流类型
#define ERROR_INVALID_DECLIBIDX			(USER_ERROR+0x41)	//解码库索引不合法
#define ERROR_SYNCSTARTRECV_TIMEOUT		(USER_ERROR+0x42)	//同步阻塞连接视频超时
#define ERROR_USERINPUT_NULLPTR			(USER_ERROR+0x43)	//用户传入空指针
#define ERROR_IPANDID_BOTHINVALID		(USER_ERROR+0x44)	//用户传入设备IP和出厂ID都非法
#define ERROR_INVALID_IPADDR			(USER_ERROR+0x45)	//用户传入非法的ip地址
#define ERROR_INVALID_BUFLEN			(USER_ERROR+0x46)	//用户传入非法的缓冲区长度
#define ERROR_CUR_UPGRADING 			(USER_ERROR+0x47)	//正在升级中
#define	ERROR_BUILD_PROTOCOL			(USER_ERROR+0x48)	//组协议失败 build protocol error
#define ERROR_INNERCALL_FAILED			(USER_ERROR+0x49)	//调用内部函数失败
#define	ERROR_COMMANDCHAN_EMPTY			(USER_ERROR+0x50)	//CommanChan空
#define ERROR_GET_VIDEOSIZE_FAILED		(USER_ERROR+0x51)	//获取分辨率失败
#define ERROR_GET_DEVICETYPE_FAILED		(USER_ERROR+0x52)	//获取串口设备类型失败
#define ERROR_GET_SENSORFLIP_FAILED		(USER_ERROR+0x53)	//获取CMOS镜头翻转失败
#define ERROR_MALLOCMEM_FALIED			(USER_ERROR+0x54)	//申请内存失败
#define ERROR_CALLCURLAPI_FALIED		(USER_ERROR+0x55)	//调用curl库接口失败

#endif
