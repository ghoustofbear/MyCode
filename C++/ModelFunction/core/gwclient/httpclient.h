#ifndef  _HTTP_CLIENT_H_
#define  _HTTP_CLIENT_H_

#include "curl/curl.h"
#include "def.h"
#include "stk/stk_message_http.h"
#include "json/json.h"

typedef struct
{
	char  *ps8AddrCode;          /**< Ŀ���豸��ַ���� */
	char  *ps8HttpURL;
	char  *ps8SipURL;
	uint32_t u32Status;
}HTTP_UPDATE_REQ_S;

typedef struct
{
	char  * ps8AddrCode;          /**< Ŀ���豸��ַ���� */
}HTTP_HEARTBEAT_REQ_S;

typedef struct
{
	char  * ps8AddrCode;          /**< Ŀ���豸��ַ���� */
	char  * ps8UserAddrCode;      /**< �����û���ַ���� */
	char  * ps8HttpURL;
	char  * ps8SipURL;
	int   s32FromIndex;         /**< �������صĿ�ʼ��¼��Ŀ */
	int   s32ToIndex;           /**< �������صĽ�����¼��Ŀ */
}HTTP_RESOURCE_REQ_S;

typedef struct
{
	char  * ps8ChnAddrCode;       /**< Ŀ���豸ͨ����ַ���� */
	char  * ps8UserAddrCode;      /**< �����û���ַ���� */

	uint32_t   u32RecType;           /**< ¼������ bit0 - Normal, bit1 - Motion, bit2 - IO-Alarm, 0xffffffff - ALL */
	char  * ps8Begin;             /**< ��ʼʱ�� */
	char  * ps8End;	    	    /**< ����ʱ�� */

	int   s32FromIndex;         /**< �������صĿ�ʼ��¼��Ŀ */
	int   s32ToIndex;           /**< �������صĽ�����¼��Ŀ */
}HTTP_RECORD_REQ_S;

typedef struct
{
	char  * ps8ChnAddrCode;       /**< Ŀ���豸ͨ����ַ���� */
	char  * ps8UserAddrCode;      /**< �����û���ַ���� */

	uint32_t   u32AlarmType;         /**< ¼������ bit0 - Normal, bit1 - Motion, bit2 - IO-Alarm, 0xffffffff - ALL */
	uint32_t   u32AlarmLevel;        /**< �澯���� */
	char  * ps8Begin;             /**< ��ʼʱ�� */
	char  * ps8End;	    	    /**< ����ʱ�� */

	int   s32FromIndex;         /**< �������صĿ�ʼ��¼��Ŀ */
	int   s32ToIndex;           /**< �������صĽ�����¼��Ŀ */

}HTTP_ALARMLOG_REQ_S;

typedef struct
{
	char  * ps8AddrCode;          /**< Ŀ��ϵͳ��ַ���� */
	char  * ps8UserAddrCode;      /**< �����û���ַ���� */
}HTTP_FLOW_REQ_S;

typedef struct
{
	string   strType;
	union
	{
		HTTP_UPDATE_REQ_S    stUpdateReq;
		HTTP_HEARTBEAT_REQ_S stHeartBeatReq;
		HTTP_RESOURCE_REQ_S  stResouceReq;
		HTTP_RECORD_REQ_S    stRecordReq;
		HTTP_ALARMLOG_REQ_S  stAlarmLogReq;
	};
}HTTP_REQ_S;

class HttpClient
{
public:

	friend size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp);
	friend size_t post_resp_callback(const char *ptr, size_t size, size_t nmemb, void *user);

	HttpClient(const char *strUrl, void *pUser);

	~HttpClient();

	bool		  start();

	int			  query_resource(const char *ps8addrcode, int fromIndex, int toIndex, string &content, int jx = 0);

	int			  query_record(const char *ps8addrcode, const char *ps8Begin, const char *ps8End, uint32_t u32RecType, int fromIndex, int toIndex, string &content, int jx = 0);

	int			  query_alarm(const char *ps8addrcode, const char *ps8Begin, const char *ps8End, uint32_t u32AlarmType, uint32_t u32AlarmLevel, int fromIndex, int toIndex, string &content, int jx = 0);

	int			  update(const char *ps8AddrCode, int status);

	int			  heartbeat(const char *ps8AddrCode);

protected:

	int           doPost(const string eventType, const char *ps8Xml, int s32XmlLen);

	int           makeXmlReq(HTTP_REQ_S req, string& xml);

    void          onPostRsp(stk::HttpMsg &httpMsg, void *pUser);

private:

	void		  stop();

    int			  onUpdateRsp(stk::HttpMsg &httpMsg, void *pUser);

    int           onQueryResourceRsp(stk::HttpMsg &httpMsg, void *pUser);

    int           onQueryRecordRsp(stk::HttpMsg &httpMsg, void *pUser);

    int           onQueryAlarmLogRsp(stk::HttpMsg &httpMsg, void *pUser);

private:
	CURL        * m_pCurl;
	std::string   m_strUrl;
	void		* m_pUser;
	std::string   m_strResponse;
	int			  m_s32ContentLen;
	int           m_s32RecvSize;
	int           m_s32SendSize;
	char        * m_ps8ReadPtr;
	string        m_strXml;
};

extern HttpClient * httpClient;

#endif
