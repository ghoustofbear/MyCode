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
#define _IN_            // ��ʾ���
#endif

#ifndef _OUT_
#define _OUT_           // ��ʾ����
#endif

typedef _int64 VIDEO_INT64;  // ͳһ8�ֽ���������

                                ////////////////////////// �����붨�� ////////////////////////////////
#define VIDEO_ERR_FAIL            -1     // ʧ��
#define VIDEO_ERR_SUCCESS          0     // �ɹ�
#define VIDEO_ERR_PARAM            1     // ��������
#define VIDEO_ERR_INVALID_POINTER  2     // ��ָ��
#define VIDEO_ERR_NOT_INIT         3     // δ��ʼ��
#define VIDEO_ERR_INVALID_HANDLE   4     // �����Ч
#define VIDEO_ERR_CREATE_INSTANCE  5     // ����ʵ��ʧ��
#define VIDEO_ERR_APPLY_MEMORY     6     // �����ڴ�ʧ��
#define VIDEO_ERR_NET_EXCEPTION    7     // �����쳣
#define VIDEO_ERR_NOT_SUPPORT      8     // ��֧�֣���ֻȡ��ʱץͼ��rtp����ѯ��ǰ����ʱ���
#define VIDEO_ERR_SET_CALLBACK     9     // �ص�����ʧ��
#define VIDEO_ERR_START_STREAM     10    // ȡ��ʧ��
#define VIDEO_ERR_STOP_STREAM      11    // ֹͣȡ���쳣
#define VIDEO_ERR_START_PLAY       12    // ����ʧ��
#define VIDEO_ERR_STOP_PLAY        13    // ֹͣ�����쳣
#define VIDEO_ERR_TIMESTAMP        14    // ʱ�����Ч
#define VIDEO_ERR_SOUND            15    // ������ش���
#define VIDEO_ERR_SNAP             16    // ץ��ʧ��
#define VIDEO_ERR_GETPLAYTIME      17    // ��ѯ����ʱ���ʧ��
#define VIDEO_ERR_INNER            18    // �ڲ�����
#define VIDEO_ERR_PLAYCTRL         19    // ���š����ؿ���ʧ��
#define VIDEO_ERR_OPER_ORDER       20    // �������򲻶ԣ���δ�������ͻ�ȡ����������������
#define VIDEO_ERR_DISKSPACE        21    // ���̿ռ䲻��
#define VIDEO_ERR_LOADLIBARAY      22    // ģ�����ʧ��
#define VIDEO_ERR_VIDEOINFO        23    // ��ѯ��Ƶ����ʧ��
#define VIDEO_ERR_CREATEFILE       24    // ������Ŀ¼���ʧ�ܣ�һ����·�����ļ����ƺ��зǷ��ַ�����·����Ŀ¼����ȫ�ǿո��ַ�������Ӣ���µġ�*������|���Լ�Ӣ���µġ�?��
#define VIDEO_ERR_OSD              25    // �����ַ����Ӵ���
#define VIDEO_ERR_TALK             26    // �Խ���ش���
#define VIDEO_ERR_DEVICE           27    // �豸��ش�����Խ��޷���⵽�����豸�������ɼ��豸
#define VIDEO_ERR_ALLOC_RESOURCE   28    // ������Դ����
#define VIDEO_ERR_PRIVATEDATA_CTRL 29    // ˽�����ݿ���ʧ��
#define VIDEO_ERR_MAX_ABILITY      30    // �����������������¼�����ػ�¼����Ҫת��װʱ�ﵽת��װ·������
#define VIDEO_ERR_STREAM_TRANSFORM 31    // ������ת������
#define VIDEO_ERR_FILE_INIVALID    32    // ��Ч�ļ����ϵ�����ʱ�������ļ����ֲ����϶ϵ�����Ҫ��ʱ�ᱨ�˴����룬һ�㱨�˴������ʾ�ļ������ݲ��㣬�����Է����ϵ�������Ϣ��
#define VIDEO_ERR_TIMESTAMP_NOT_IN_RANGE 33  // ¼������ʱָ���ϵ����������������ϵ㴦ʱ�䲻��ָ���Ŀ�ʼ�����ʱ��֮��
#define VIDEO_ERR_FILE_NOT_EXIST   500   // �ļ������ڣ�����ϵ�����ʱ����Ҫ�����ļ�����·�����������ļ������ڣ��ᱨ�ô���
#define VIDEO_ERR_OTHER            10000 // ��������


////////////////////////// �ص����� ///////////////////////////////
// �����ص�
// i64PlayHandle��Ԥ����طŽӿڷ��صľ��
// iStreamDataType�������������ͣ�0-����ͷ 1-�������� 2-�������
// pDataArray�����������飨�赱�ɶ�����������������
// iDataLen�������ݳ���
// pUserData���û�����
typedef void(CALLBACK * pfnStreamCallback)(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData);

// ȡ����������Ϣ�ص�
// i64PlayHandle��Ԥ����طŽӿڷ��صľ�������ص����������Խ�ȡ����Ϣʱ����ֵ��Чֵ��-1��
// iMsg����Ϣ���ͣ�1-���ſ�ʼ 2-���Ž�������ƵԤ���޴���Ϣ�� 3-�����쳣 10-ȡ����ʼ 11-ȡ������ 12-ȡ���쳣 50-�첽ʱ����ȡ������ɹ� 51-�첽ʱ����ȡ������ʧ��
typedef void(CALLBACK * pfnMsgCallback)(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData);

// YUV���ݻص�
// i64PlayHandle��Ԥ����طŽӿڷ��صľ��
// pDataArray��YUV�������飨�赱�ɶ�����������������
// iDataLen�������ݳ���
// iWidth��ͼ����
// iHeight��ͼ��߶�
// iFrameType��ͼ��YUV���ͣ�ĿǰΪYV12��ֵΪ3��
// iTimeStamp��ʱ���
// pUserData���û�����
typedef void(CALLBACK* pfnDecodedDataCallback)(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, int iFrameType, int iTimeStamp, void* pUserData);

// ¼�����ػص����������Ϣ��
// i64DownloadHandle������¼�����ؾ��
// fPercent��������¼����ȣ���iMsgΪ0ʱ������Ч
// iMsg��¼��������Ϣ��0-��ʼ¼������ 1-¼�������� 2-¼��������� 3-¼�����ؼ����ְ� 4-¼�����طְ�ʧ�� 5-¼�����طְ���� 6-¼������ʱ���� 30-ת��װ��֧�� 50-�ϵ�������֧�� 100-����ʧ�ܣ��ڲ�����
// pUserData���û��Զ�������
typedef void (CALLBACK* pfnDownloadCallback)(VIDEO_INT64 i64DownloadHandle, float fPercent, int iMsg, void* pUserData);

////////////////////////// �ṹ�嶨�� //////////////////////////////////
typedef struct stPlayReq
{
    int iHardWareDecode;                // �Ƿ���GPUӲ�� 0-������ 1-�������������Ӳ�⣬�����Կ���֧�ֵȵ��´���SDK�ڲ��Զ��л�����⣻����Ӳ��ֻ��������ʾ���ܣ������ܼ����ڴ�ռ���ʣ���������ر����󣬽��鲻����Ӳ�⣩
    pfnStreamCallback fnStream;         // �������ݻص�
    pfnMsgCallback fnMsg;               // ���š�ȡ����Ϣ�ص�
    pfnDecodedDataCallback fnDecodedStream;     // ������YUV���ݻص���ע�⿪����GPUӲ�ⲻ֧�ֻص���������ݣ���ʱָ���˻ص�Ҳ����Ч
    void* pUserData;                    // �û�����
    char szReserve[64];                 
}VIDEO_PLAY_REQ, *PVIDEO_PLAY_REQ;

typedef struct stFilePlayReq
{
    pfnMsgCallback fnMsg;               // ���š�ȡ����Ϣ�ص�
    void* pUserData;                    // �û�����
    char szReserve[64];                 // ��������
}VIDEO_FILEPLAY_REQ, *PVIDEO_FILEPLAY_REQ;

typedef struct stDownloadReq
{
    pfnDownloadCallback fnDownload;     // ���ؽ��Ȼص�
    void* pUserData;                    // �û�����
    VIDEO_INT64 i64FileMaxSize;         // ¼��ְ���С
    VIDEO_INT64 i64RecordSize;          // ¼���ܴ�С��������ѯ�ط�URLʱ��ѯ���ĸ�¼��Ƭ�δ�С֮��
    VIDEO_INT64 i64StartTimeStamp;      // ¼�����ؿ�ʼʱ�䣬������ѯ�ط�URLʱ�Ĳ�ѯ��ʼʱ�������λ����
    VIDEO_INT64 i64EndTimeStamp;        // ¼�����ؽ���ʱ�䣬������ѯ�ط�URLʱ�Ĳ�ѯ����ʱ�������λ����
    char szReserve[64];             	// ¼�����ͣ���䡰0�������-��ͨ¼�����أ���1��-¼��ת��װ�������Ƶ����Ƶ��֧����ص���֧����Ϣ ��2�� - ¼��ת��װ������Ƶ����Ƶ��֧��ʱ����ת��װ���� ��3��-¼��ת��װ�������Ƶ��֧����ֻת��Ƶ�������ƵҲ��֧����ص���֧����Ϣ ��4��-���öϵ����� ����ֵ - �������󣨶���ָ��¼��ת��װ�����öϵ�������Ŀǰ��֧�ֺ���SDKЭ�顢EHOMEЭ������豸�����������Э�������豸����ص���֧�ֵ���Ϣ�����ڶϵ�������ֻ֧��δת��װ��¼�񣻵�¼������ʱ�����Ȳ�ѯ�豸����Э�飬�Ǻ���SDK��EHOMEЭ�鲻������䡰0�����ɣ�ת��װ�ǽ���Ƶת���ɱ�׼MP4�ļ���������ͨ�ò������ϲ��ţ��粻ת��װ����Ҫר�ò��������ţ����SDK�����豸���ص�¼����Ҫ�󻪲�����������SDKЭ��/EHOMEЭ������¼����Ҫ�������������ţ�
}VIDEO_DOWNLOAD_REQ, *PVIDEO_DOWNLOAD_REQ;

typedef struct stVideoDetailInfo
{
    int iWidth;                         // ��Ƶͼ����
    int iHeight;                        // ��Ƶͼ��߶�
    int iEncodeType;                    // ��������  1-H264 2-MPEG2_PS 3-MPEG4 4-H265 5-GB28181 6-raw 7-vag 8-dahua 9-smartH264 10-smartH265
    int iEncapsulationType;             // ��װ����  1-ps 2-ts 3-rtp 4-raw 5-rtp + ps
    VIDEO_INT64 i64FrameRate;           // ֡��    
    char szReserve[64];                 // ��������
	float fCodeRate;                    // ���ʣ�V1.2.0������
}VIDEO_DETAIL_INFO, *PVIDEO_DETAIL_INFO;

typedef struct stOSDInfo
{
    int iBold;                  // �����ַ����Ƿ���� 1-���� 0-�Ǵ���
    int ix;                     // �����ַ�����벥�Ŵ������Ͻ����ĺ�����
    int iy;                     // �����ַ�����벥�Ŵ������Ͻ�����������
    int iFontSize;              // �����С
    int iAlignType;             // ���뷽ʽ��0-����� 1-���ж��� 2-�Ҷ���
    VIDEO_INT64 i64Color;       // �ַ���ɫ��Windows��ʹ��RGB������ɫֵ
    char szReserve[64];         // ��������
}VIDEO_OSD_INFO, *PVIDEO_OSD_INFO;

////////////////////////// �ӿڶ��� /////////////////////////////
// SDK��ʼ��
// pszEvn��������������NULL����
// ��ע����һ�μ��ɣ���ʼ���ɹ�������ظ���ֱ�ӷ��سɹ�
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_Init(_IN_ const char* pszEvn);

// SDK����ʼ��
// ��ע����һ�μ��ɣ�����ʼ���ɹ�������ظ���ֱ�ӷ��سɹ�
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_Fini();

// ͬ����ʼ��ƵԤ����֧��ȡʵʱ�����ص�ȡ���벥�ŵ���Ϣ���Լ��ص�YUV���ݣ���Ҫ�������������ָ����
// pszUrl��Ԥ��URL��ÿ��Ԥ����Ҫ���²�ѯURL
// hWnd����ƵԤ����Windows���ھ��
// pstPlayReq��������������������������ṹ�嶨��
// ͬ�����ؽ�����ɹ����ش���0��Ԥ�������ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartPreview(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// �첽��ʼ��ƵԤ����֧��ȡʵʱ�����ص�ȡ���벥�ŵ���Ϣ���Լ��ص�YUV���ݣ���Ҫ�������������ָ����
// pszUrl��Ԥ��URL��ÿ��Ԥ����Ҫ���²�ѯURL
// hWnd����ƵԤ����Windows���ھ��
// pstPlayReq��������������������������ṹ�嶨��
// �첽���ؽ�����ɹ����ش���0��Ԥ�������ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ�����룻����ֵֻ��ʾ�����첽����Ľ������������ȡ������Ľ����ȡ���������ɹ����ο���Ϣ�ص��еġ��첽����ȡ������ɹ����롰�첽����ȡ������ʧ�ܡ���Ϣ
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartAsynPreview(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// ֹͣԤ����ֹͣȡ����ֹͣ�ص���Ϣ��ֹͣ�ص�YUV���ݣ�
// i64PlayHandle��Ԥ���������Դ��Video_StartPreview��Video_StartEzvizPreview����ֵ
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StopPreview(_IN_ VIDEO_INT64 i64PlayHandle);

// ͬ������¼��طš����ţ�֧��ȡ����¼�������ص�ȡ���벥�ŵ���Ϣ���Լ��ص�YUV���ݣ���Ҫ�������������ָ����
// pszUrl���ط�URL
// hWnd������¼�����Ż򵹷ŵ�Window���ھ��
// i64StartTimeStamp������ʱΪ��ѯ�ط�URLʱ�Ŀ�ʼʱ���������ʱΪ��ѯ�ط�URLʱ�Ľ���ʱ�������λ����
// i64EndTimeStamp������ʱΪ��ѯ�ط�URLʱ�Ľ���ʱ���������ʱΪ��ѯ�ط�URLʱ�Ŀ�ʼʱ�������λ����
// pstPlayReq�����������������ϸ��������ṹ�嶨��
// ͬ�����ؽ�����ɹ����ش���0�Ĳ��ž����ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartPlayback(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ VIDEO_INT64 i64StartTimeStamp, _IN_ VIDEO_INT64 i64EndTimeStamp, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// �첽����¼��طš����ţ�֧��ȡ����¼�������ص�ȡ���벥�ŵ���Ϣ���Լ��ص�YUV���ݣ���Ҫ�������������ָ����
// pszUrl���ط�URL
// hWnd������¼�����Ż򵹷ŵ�Window���ھ��
// i64StartTimeStamp������ʱΪ��ѯ�ط�URLʱ�Ŀ�ʼʱ���������ʱΪ��ѯ�ط�URLʱ�Ľ���ʱ�������λ����
// i64EndTimeStamp������ʱΪ��ѯ�ط�URLʱ�Ľ���ʱ���������ʱΪ��ѯ�ط�URLʱ�Ŀ�ʼʱ�������λ����
// pstPlayReq�����������������ϸ��������ṹ�嶨��
// �첽���ؽ�����ɹ����ش���0�Ĳ��ž����ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ�����룻����ֵֻ��ʾ�����첽����Ľ������������ȡ������Ľ����ȡ���������ɹ����ο���Ϣ�ص��еġ��첽����ȡ������ɹ����롰�첽����ȡ������ʧ�ܡ���Ϣ
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartAsynPlayback(_IN_ const char* pszUrl, _IN_ void* hWnd, _IN_ VIDEO_INT64 i64StartTimeStamp, _IN_ VIDEO_INT64 i64EndTimeStamp, _IN_ PVIDEO_PLAY_REQ pstPlayReq);

// ֹͣ����¼�����š����ţ�ֹͣȡ����ֹͣ�ص���Ϣ��ֹͣ�ص�YUV���ݣ�
// i64PlayHandle������¼�񲥷ž������Դ��Video_StartPlayback�ķ���ֵ
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StopPlayback(_IN_ VIDEO_INT64 i64PlayHandle);

// ��ѯ¼��طŵ�ǰ����ʱ���
// i64PlayHandle:����¼�񲥷ž������Դ��Video_StartPlayback��Video_StartEzvizPlayback�ķ���ֵ
// �ɹ����ش��ڵ���0�ĵ�ǰ�ط�ʱ�����ʧ�ܷ���VIDEO_ERR_FAIL����λ���롣�ӿڷ���ʧ��ͨ������Video_GetLastError��ȡ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetCurrentPlayTime(_IN_ VIDEO_INT64 i64PlayHandle);

// ��ʼ�ļ�����
// pszFileName������·��mp4�ļ�����
// hWnd���ļ����Ŵ��ھ��
// pstFilePlayReq���ļ�������������ṹ�壬����ص���Ϣ���ɴ�NULL
// ��ע����֧�ֲ���ͨ������SDKЭ�顢Ehome/ISUPЭ�顢ONVIFЭ�顢��SDKЭ�顢����Э�顢өʯ������豸����¼��¼�������¼�����ص��ļ�
// �ɹ����ش���0���ļ����ž����ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartFilePlay(_IN_ const char* pszFileName, _IN_ void* hWnd, _IN_ PVIDEO_FILEPLAY_REQ pstFilePlayReq);

// ֹͣ�ļ�����
// i64PlayHandle���ļ����ž������Դ��Video_StartFilePlay�ķ���ֵ
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StopFilePlay(_IN_ VIDEO_INT64 i64PlayHandle);

// ��ѯ�ļ�������ʱ��
// i64PlayHandle:�ļ����ž������Դ��Video_StartFilePlay�ķ���ֵ
// �ɹ����ش��ڵ���0����ʱ����ʧ�ܷ���VIDEO_ERR_FAIL����λ���롣�ӿڷ���ʧ��ͨ������Video_GetLastError��ȡ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetFilePlayDuration(_IN_ VIDEO_INT64 i64PlayHandle);

// ��ѯ�ļ��Ѳ���ʱ��
// i64PlayHandle���ļ����ž������Դ��Video_StartFilePlay�ķ���ֵ
// �ɹ����ش��ڵ���0���Ѳ���ʱ����ʧ�ܷ���VIDEO_ERR_FAIL����λ���롣�ӿڷ���ʧ��ͨ������Video_GetLastError��ȡ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_GetFilePlayedTime(_IN_ VIDEO_INT64 i64PlayHandle);

// ��ʼ¼������
// pszUrl:����ȡ��URL
// pszFileName:¼���ļ�����¼�����ƣ���ָ����׺mp4���硰E:\test.mp4����pszFileNameΪansi�����ַ�����·�����ļ����Ʋ��ܰ��������ַ�����Ӣ���µġ�*������|���Լ�Ӣ���µġ�?���������⣬·�������á�.���͡�..����¼�����ز�֧�����ص�����λ��,ֻ�����ص����ء�����pstDownloadReq��ָ���ϵ�����ʱ���봫���ϴ�¼������������ɵ�һ���ļ��ľ���·���ļ����ƣ������֧�ֻ�ͨ���ص�������Ϣ
// pstDownloadReq:¼�����������������ϸ��������ṹ�嶨��
// ��ע��֧��ͨ�������ļ�VideoSDK.int���ã��Ǻ�����ʱ��ȡ�������С��������ʱ�Ƿ�ת��װ��֧����ͨ����������
// �ɹ����ش���0�Ĳ��ž����ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE VIDEO_INT64 Video_StartDownload(_IN_ const char* pszUrl, _IN_ const char* pszFileName, _IN_ PVIDEO_DOWNLOAD_REQ pstDownloadReq);

// ֹͣ����
// i64DownloadHandle��¼�����ؾ������Դ��Video_StartDownload�ķ���ֵ
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StopDownload(_IN_ VIDEO_INT64 i64DownloadHandle);

// ��ʼ����¼�񣬶���ͬһ·��ͬһʱ��ֻ֧��һ·����¼�񣬷��򷵻ز�֧��
// i64PlayHandle�����ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback�ӿڷ���
// pszFileName:¼���ļ�����·�����ƣ���ָ����׺mp4���硰E:\test.mp4����pszFileNameΪansi�����ַ�����·�����ļ����Ʋ��ܰ��������ַ�����Ӣ���µġ�*������|���Լ�Ӣ���µġ�?���������⣬·�������á�.���͡�..����
// i64FileMaxSize���ְ���С��λ���ֽڣ�����С��5M������5M���㣻�ְ����������򣺡�E:\test-1.mp4����...��E:\test-n.mp4����n��ʾ����E:\test.mp4����ĵ�n���ְ�
// recordType��¼�����ͣ�0-��ת��װ��1-¼��ת��װ�����Ƶ����Ƶ��֧���򷵻ش��� 2-¼��ת��װ����Ƶ����Ƶ��֧��ʱ����ת��װ����ǰ���Ǵ�����Ƶ����Ƶ���ݣ� 3-¼��ת��װ�����Ƶ��֧����ֻת��Ƶ ����ֵ-��������1~3��Ժ������������ǰ��Ƶ�ǷǺ��������ӿڷ��ز�֧�֣�
// ��Щ�豸���Ļ��������Ƶ����֧��ת��װ����ʱֻ����Ƶ��ת��װ
// ��Щ�豸��������֧��ת��װ������豸������ʱ����ת��װ
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StartLocalRecord(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ const char* pszFileName, _IN_ VIDEO_INT64 i64FileMaxSize, _IN_ int recordType);

// ֹͣ����¼��
// i64PlayHandle�����ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback�ӿڷ���
// �ɹ������������ļ�������ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_StopLocalRecord(_IN_ VIDEO_INT64 i64PlayHandle);

//���š����ؿ���
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartAsynPlayback��Video_StartDownload�ӿڷ���
// iCtrlType:0-��ͣ 1-�ָ����� 2-��λ���� 3-���ٲ��� 4-��ͣ���� 5-�ָ�����
// i64Param:���Ʋ�������iCtrlType���ʹ��
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
// ��ע��1-�ָ������ǻָ���1���٣��������ϴεĲ����ٶȣ�2-ʱ��������ɵ��÷���֤���ڿ�ʼ�����ʱ���֮�䣬��λ���룻3-���Ʋ���Ϊ�ٶ�ȡֵ��16��8��4��2��1��-2��-4��-8��-16�������ɿ��������
VIDEOSDK_DECLARE int Video_PlayCtrl(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iCtrlType, _IN_ VIDEO_INT64 i64Param);

//���������л�
// iPlayHandle:���ž����Video_StartPreview��Video_StartAsynPreview����
// iAsyn���Ƿ��첽�л���1-�첽��0-ͬ����������첽�л����л��ɹ���Ϣ����Ϣ�ص�����������ӿڷ���ֵ�����Ƿ��л��ɹ���ȡ����ֵ�򷵻�ʧ��
// pszParam�����������л��������ò������ָ���������͵�Ԥ��ȡ��url
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_ChangeStreamType(VIDEO_INT64 i64PlayHandle, int iAsyn, const char* pszParam);

// ����ץͼ
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// pszFileName:Ҫ�����ͼƬ����·�������ļ������������ļ�����׺ȷ�������ͼƬ��ʽ��֧��bmp��jpg����������E:/SnapShot/���Լ�ص�.jpg����pszFileNameΪansi�����ַ�����·�����ļ����Ʋ��ܰ��������ַ�����Ӣ���µġ�*������|���Լ�Ӣ���µġ�?���������⣬·�������á�.���͡�..��
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_PlaySnap(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ const char* pszFileName);

// ��������
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// iCtrlType:�������ͣ�0-�� 1-�رգ�����ֵ-δ���塣
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
// ��ע������������Ҫ������������Ƶ���ݲ���Ч��
VIDEOSDK_DECLARE int Video_SoundCtrl(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iCtrlType);

// ��ȡ����
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// �ɹ���������ֵ��������Χ[0,100]��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
// ��ע����Ҫ�����������Ч��
VIDEOSDK_DECLARE int Video_GetVolume(_IN_ VIDEO_INT64 i64PlayHandle);

// ��������
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// iVolume:����ֵ����Χ[0,100]
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
// ��ע����Ҫ�����������Ч��
VIDEOSDK_DECLARE int Video_SetVolume(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iVolume);

// �����ַ�����
// i64PlayHandle:���ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// iOSDId:�ַ����ӱ�ʶ����һ�ε���ʱ�봫��0�����»����ϵ��ַ���ʱ�����һ�ε���ʱ���ص�Id
// pszText:�������ַ�����֧�֡�\n�����У��ַ���������512���ֽڣ��糬����ض���ʾ
// pstOSDInfo:�ַ�������Ϣ
// �ɹ����ر��ε��ӵ�id����Idȡֵ���ڵ���0��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_SetOSDText(_IN_ VIDEO_INT64 i64PlayHandle, _IN_ int iOSDId, _IN_ const char* pszText, _IN_ PVIDEO_OSD_INFO pstOSDInfo);

// ��ʼ�����Խ�
// pszUrl:�����Խ�URL���ɴ�OpenAPI��ѯ�Խ�URL��ÿ�ζԽ��������²�ѯ�Խ�URL
// pszFileName:�����Խ�������Ƶ�ľ���·���ļ����ƣ�ֻ֧��wav��׺���硰D:\test.wav�����粻��Ҫ¼�����ò����봫NULL��·�����ļ����Ʋ��ܰ��������ַ�����Ӣ���µġ�*������|���Լ�Ӣ���µġ�?���������⣬·�������á�.���͡�..������ʱ��֧�ֱ�����Ƶ�ļ����ܣ��ò��������NULL��
// fnMsg:�����Խ���Ϣ�����ڻص������Խ�ȡ����Ϣ���粻��Ҫ�ɴ�NULL��
// pUserData:�û����ݣ�����fnMsg�ص���͸����ȥ���粻��Ҫ�ɴ�NULL��
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
// ��ע��ͬһʱ��ֻ֧��һ·�Խ������޷����������Խ�ʧ�ܻ��������Խ�ͨ����ռ�á�ֻ֧����ǰ���豸�Խ���ǰ��IPC�豸����NVR��֧����ǰ��IPC�豸�Խ�����֧��ֱ����NVR�Խ���ÿ�ζԽ��������²�ѯ�Խ�URL���ݲ�֧�ֱ�����Ƶ�ļ����ܣ�pszFileName���������NULL�������Խ�ֻ֧�ֺ���SDK��EhomeЭ������豸��
VIDEOSDK_DECLARE int Video_StartTalk(_IN_ const char* pszUrl, _IN_ const char* pszFileName, _IN_ pfnMsgCallback fnMsg, _IN_ void* pUserData);

// ֹͣ�����Խ�
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL���ӿڷ���ʧ�������Video_GetLastError��ȡ�����룬ͨ���������жϳ���ԭ��
VIDEOSDK_DECLARE int Video_StopTalk();

// ��ѯ��Ƶ��ϸ��Ϣ
// i64PlayHandle�����ž������Դ��Video_StartPreview��Video_StartPlayback��Video_StartAsynPreview��Video_StartAsynPlayback����ֵ
// pstVideoDetailInfo����Ƶ��ϸ��Ϣ�ṹ��ָ��
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL���ӿڷ���ʧ�������Video_GetLastError��ȡ�����룬ͨ���������жϳ���ԭ��
VIDEOSDK_DECLARE int Video_GetVideoInfo(_IN_ VIDEO_INT64 i64PlayHandle, _OUT_ PVIDEO_DETAIL_INFO pstVideoDetailInfo);

// ˽�����ݿ��ƣ�ָ�����ϵ����ƶ������߿�����ݵĿ��ؿ��ƣ���Ժ������豸��֧�ֺ���SDK��EHOME/ISUP��ONVIF��GB28181Э����룩
// i64PlayHandle�����ž��������Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// iPrivateDataType��˽���������ͣ������ʹ�á�|����������������ƣ�0x0001-���ܷ��� 0x0002-�ƶ���� 0x0004-POS��Ϣ����� 0x0008-ͼƬ���� 0x0010-�ȳ�����Ϣ 0x0020-�¶���Ϣ
// iCtrlType����ʾ���ؿ��� 0-��ʾ 1-����
// iParam��һЩ˽���������ʹ��������ͣ���iDataTypeΪ0x0010ʱ����������0x0001-������ʾ 0x0002-����¶� 0x0004-����¶�λ�� 0x0008-����¶Ⱦ��룻��iDataTypeΪ0x0020ʱ����������0x0001-����� 0x0002-�߲��� 0x0004-�����
// ע�����ڲ��漰�����ͣ����������ͣ��ģ�֧�ָ���˽���������͵ġ�|�����㣬�����漰�����͵ģ���һ��һ���������ͽ��п��ƣ�ͬһ���͵�������֧�֡�|�����㣻iDataTypeΪ0x0010��0x0020ʱ��Ҫ�ȳ����豸֧�֣�����������ͨ�豸����֧�֣�����Ҫ�豸������
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ�����룬�Ǻ����豸��֧�ֻᣬĿǰ�޷������Ƿ�֧��
VIDEOSDK_DECLARE int Video_PrivateDataCtrl(_IN_ VIDEO_INT64 iPlayHandle, _IN_ int iPrivateDataType, _IN_ int iCtrlType, _IN_ int iParam);

// ˢ�²���
// i64PlayHandle�����ž����Video_StartPlayback��Video_StartPreview��Video_StartAsynPreview��Video_StartAsynPlayback��Video_StartFilePlay�ӿڷ���
// �ɹ�����VIDEO_ERR_SUCCESS��ʧ�ܷ���VIDEO_ERR_FAIL��ʧ����ͨ��Video_GetLastError��ѯ������
VIDEOSDK_DECLARE int Video_RefreshPlay(VIDEO_INT64 i64PlayHandle);

// ��ȡ������
VIDEOSDK_DECLARE int Video_GetLastError();

#endif  // HIK_VIDEOSDK_H