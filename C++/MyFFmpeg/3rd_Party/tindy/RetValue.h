/************************************************************************/
/*COMMON RETURN                                                                     
*/
/************************************************************************/
#define RET_NVD_ISVIEW						1       //������Ƶ������SDK�ӿڷ���ֵ�������������ӵ�NVS
#define RET_NOTHING						((void)0)
#define RET_SUCCESS							0		//�ɹ�
#define RET_FAILED							-1		//ʧ��
#define RET_NVD_INVALIDID					-1		//������Ƶ������SDK���ط���ֵ��ָ���Ľ�����������
#define RET_MALLOC_FALIED					-2		//�����ڴ�ʧ��
#define RET_NVD_UNINIT						-2		//������Ƶ������SDK�ӿڷ���ֵ����������δ��ʼ��/��ʼ��ʧ��
#define RET_INVALIDPARA						-3		//����Ƿ�����
#define RET_FAILED_UPGRADE					-4		//������Ƶ������SDK�ӿڷ���ֵ������ʧ��
#define RET_NVD_OUTMEMERY					-4		//������Ƶ������SDK�ӿڷ���ֵ���ڴ�������
#define RET_APPLYMEMORY_FAILED				-5		//�ڴ�����ʧ��
#define RET_NVDNOACCOUNT					-5		//������Ƶ������SDK�ӿڷ���ֵ���û�������
#define RET_NVDNOPASS						-6		//������Ƶ������SDK�ӿڷ���ֵ���������
#define RET_INVALIDFILE						-7		//�ļ���ʽ�Ƿ�
#define RET_NOTLOGON						-8		//û�е�¼�����½�����ʧЧ
#define RET_LOGING							-9		//�ѵ�¼�����ڵ�¼
#define RET_NVD_MAXDEC						-10		//�ﵽ���������
#define	RET_BUFFER_FULL						-11		//��������������û�з��뻺����
#define RET_NVD_UPGRADING					-11		//������Ƶ������SDK�����ӿڷ���ֵ����������
#define RET_FORBID_OPT						-12		//���������
#define RET_NVD_RELOOPITEM					-13		//��ѭ���б�������ظ�������
#define RET_NVD_RECONNECT					-14		//�ظ�����NVS
#define RET_INVALIDPOS						-15		//λ��Խ��
#define RET_NOCONNECTED						-16		//��û�н�������
#define RET_POS_X_Y							-17		//����Ƿ�
#define	RET_BUFFER_WILL_BE_FULL				-18		//�������������������ݵ�Ƶ��
#define RET_NVD_NULL_PTR					-18		//null pointer
#define	RET_BUFFER_WILL_BE_EMPTY			-19		//�����գ�����������ݵ�Ƶ��
#define RET_NVD_UNDEFINED_BEHAVIOR			-19		//������Ƶ������SDK�ӿڷ���ֵ�� undefined behavior
#define RET_BUFFER_IS_OK					-20		//�������Է�����
#define RET_NVD_INVALID_BUFF_SIZE			-20		//������Ƶ������SDK�ӿڷ���ֵ�����Ϸ��Ļ�������С
#define RET_INVALID_ARRAY_INDEX				-21		//���Ϸ��������±�
#define RET_NVD_INVALID_PARAM				-22		//�������Ϸ�
#define RET_REPEAT_SET						-23		//�ظ�����
#define RET_BUILD_PROTOCOL					-24		//build protocol error
#define RET_SEND_PROTOCOL					-25		//send protocol error
#define RET_NVD_NO_IDLE_ITEM				-26		//no idle item
#define RET_INVALID_FILEHEADER				-27		//invalid file header
#define RET_LIGHTMODE_NOTSUPPORT			-28		//sdk light mode not support get para
#define RET_LOAD_OSCORE						-29		//sdk load OsCore.dll fail
#define RET_INVALID_OPT_TYPE				-30		//�Ƿ��Ĳ�������
#define RET_NOT_FIND_VALIED_PLAYER_ID		-31		//not find valild playerid
#define RET_STARTPLAY_FAILED				-32		//start play failed

#define RET_GET_VIDEOSIZE_FAILED			-40		//��ȡ�ֱ���ʧ��
#define RET_GET_DEVICETYPE_FAILED			-41		//��ȡ�����豸����ʧ��
#define RET_GET_SENSORFLIP_FAILED			-42		//��ȡCMOS��ͷ��תʧ��

#define RET_GETDATA_END						-50		//��ȡ�������������������ļ�β���߲���ֹͣλ��
#define RET_NVD_ERR_SYSTEM					-100	//ϵͳ����				
#define RET_INVALID_PARA					-101	//�Ƿ�����
#define RET_DEVICE_NOT_LOGON				-102	//�豸δ��¼
#define RET_MEMORY_OVER						-103	//�ڴ治��
#define RET_ERR_AUTHORITY					-104	//Ȩ�޲���
#define RET_SEND_LIST_FULL					-105	//�����б�����
#define RET_DEVICE_CAPTURE_FAIL				-106	//�豸Զ��ץ��ʧ�ܣ��豸���ص����ݳ���Ϊ0
#define RET_DEVICE_CAPTURE_TIMEOUT			-107	//�豸Զ��ץ�ĳ�ʱ���豸δ�ظ�����
#define RET_INVALID_LOGONID					-108	//����ĵ�½���ʧЧ��Ƿ�
#define RET_CALLCURLAPI_FALIED				-109	//����curl��ӿ�ʧ��
#define RET_INVALID_MEMORY					-110	//�Ƿ��ڴ�
#define RET_NEW_CLASS_FAILED				-111	//�½�classʧ��

#define RET_SYSTEM_CALL_FAILED				-200	//����ϵͳ����ʧ��
#define RET_SDK_CALL_FAILED     			-201	//����SDK�ӿ�ʧ��
#define RET_INNER_CALL_FAILED				-202	//�����ڲ�����ʧ��
#define RET_DRAW_CREATE_FAILED				-203	//����Drawʧ��
#define RET_NOSUPPORT_PARATYPE				-204	//��������ȡ������֧�ֵĲ�������
#define RET_LIGHTLOGON_NETERROR				-205	//��������Э���������
#define RET_LIGHTLOGON_GET_TIME_OUT			-206	//��������Э��ȴ��豸�ظ���ʱ
#define RET_DEV_NOT_SUPPORT					-207	//�豸��֧�ֵ�ǰ������ȡ��ʽ
#define RET_GETREGDEVCOUNT_TIMEOUT			-208	//��Ŀ¼������ģʽ��ȡע���豸������ʱ
#define RET_ERR_RECREATE_DECODER			-209	//��Ҫ���´���������
#define RET_FACE_CUT_QUERY_TIMEOUT			-210	//��ѯ������ͼ�����ʱ����
#define RET_GETREGASSIGNPROXY_TIMEOUT		-211	//��Ŀ¼��������ȡ���豸����Ĵ�����Ϣ��ʱ
#define RET_QUERYREGNVSBYID_TIMEOUT			-212	//���ݳ���ID��Ŀ¼��������ѯע���豸��Ϣ��ʱ
#define RET_QUERYNVSBYDOMAINNAME_TIMEOUT	-213	//����������Ŀ¼��������ѯע���豸��Ϣ��ʱ

#define RET_SYNCLOGON_TIMEOUT				-300	//ͬ����¼��ʱ���������������Զ����κ���Ӧ
#define RET_SYNCLOGON_USENAME_ERROR			-301	//ͬ����¼ʧ�ܣ��û��������ڣ�Ĭ���û�����admin
#define RET_SYNCLOGON_USRPWD_ERROR			-302	//ͬ����¼ʧ�ܣ��������Ĭ�����룺1111������û��޸Ĺ�������������ȷ���롣
#define	RET_SYNCLOGON_PWDERRTIMES_OVERRUN	-303	//ͬ����¼ʧ�ܣ��������������ޣ��˺�������
#define RET_SYNCLOGON_NET_ERROR				-304	//ͬ����¼ʧ�ܣ��������Ӵ���
#define RET_SYNCLOGON_PORT_ERROR			-305	//ͬ����¼ʧ�ܣ�ͨ�Ŷ˿���������Ĭ�ϴ���˿�3000
#define RET_SYNCLOGON_UNKNOW_ERROR			-306	//ͬ����¼ʧ�ܣ�����δ֪
#define RET_SYNCREALPLAY_TIMEOUT			-307	//ͬ��������Ƶ��ʱ���Զ�û�з�����ƵͷЭ��
#define RET_SYNCREALPLAY_FAIL				-308	//ͬ��������Ƶʱ������Ƶʧ��
#define RET_SYNCSUSPENDVCA_CONFIGING		-309	//ͬ����ͣ���ܷ���ʧ�ܣ����ܷ����������������ͻ�������
#define RET_SYNCSUSPENDVCA_FAIL				-310	//ͬ����ͣ���ܷ���ʧ�ܣ��豸δ����
#define RET_SYNCOPENVCA_CONFIGING			-311	//ͬ���������ܷ���ʧ�ܣ����ܷ����������������ͻ�������
#define RET_SYNCOPENVCA_FAIL				-312	//ͬ���������ܷ���ʧ�ܣ��豸δ����
#define RET_SYNCOPTVCA_TIMEOUE				-313	//ͬ����ͣ/���� ���ܷ�����ʱ
#define RET_SYNCTALK_SOCKET_ERR				-320	//ͬ���Խ�����Э��ʧ�ܣ��������
#define RET_SYNCTALK_INNERCON_ERR			-321	//ͬ���Խ�����˫��Խ�ʧ��
#define RET_SYNCTALK_BEGIN_ERR				-322	//ͬ���Խ������ظ�ʧ��
#define RET_SYNCTALK_END_ERR				-323	//ͬ���Խ�ֹͣ�ظ�ʧ��
#define RET_SYNCTALK_DEV_ERR				-324	//ͬ���Խ��豸�ظ�ʧ��
#define RET_SYNCTALK_RECSOUND_ERR			-325	//ͬ���Խ��ɼ���Ƶ����ʧ��
#define RET_SYNCCAPTURE_INVALID_MEM			-329	//ͬ��ץ�Ľӿڴ���Ƿ��ڴ��ַ��һ���ǽṹ���ڲ�ָ�����û�������ڴ�ռ�
#define RET_SYNCMSG_TIMEOUT					-330	//ͬ���ӿڵȴ���Ϣ��ʱ����ֵ


#define RET_DEC_NOT_INIT					-401	//������δ��ʼ��
#define RET_DEC_NO_FREE_HANDLE				-402	//���������û�п��еģ���ʹ����
#define RET_DEC_ILLEGAL_HANDLE				-403	//������������Ϸ�
#define RET_DEC_ILLEGAL_PARAM				-404	//����������������Ϸ�
#define RET_DEC_NOT_CREATE					-405	//������δ����
#define RET_DEC_CREATE_FAILED				-406	//����������ʧ��
#define RET_DEC_MEMORY_OVER					-407	//�������ڴ治��
#define RET_DEC_NOT_SHOW_PRE_I				-408	//����������ʾ��һ��I֡
#define RET_DEC_HS_LOAD_FAILED				-410	//������HS���ؿ�ʧ��
#define RET_DEC_HS_NEED_MORE				-411	//������HS�����룬���ݲ���
#define RET_DEC_HS_NO_PIC					-412	//������HS����ʧ�ܣ�û��ͼ�����
#define RET_DEC_HS_ERR_HANDLE				-413	//������HS���Ϊ�ջ��������Ϊ��
#define RET_DEC_HS_FAILED					-414	//������HS����ʧ��
#define RET_DEC_FF_LOAD_FAILED				-420	//������FF���ؿ�ʧ��
#define RET_DEC_FF_NOT_FIND_DECODER			-421	//������FF����ʧ��
#define RET_DEC_FF_ALLOC_FAILED				-422	//������FF�����ڴ�ʧ��
#define RET_DEC_FF_OPEN_FAILED				-423	//������FF��������ʧ��
#define RET_DEC_FF_VIDEO2_FAILED			-424	//������FF��Ƶ����ʧ��
#define RET_DEC_FF_AUDIO_ILLEGAL_HEAD		-425	//������FF��Ƶͷ����
#define RET_DEC_FF_AUDIO_LESS				-426	//������FF��Ƶ���ݲ���
#define RET_DEC_FF_AUDIO4_FAILED			-426	//������FF��Ƶ����ʧ��
#define RET_DEC_FF_FAILED					-427	//������FF����ʧ��
#define RET_DEC_FF_HW_UNSUPPORT				-428	//������FFӲ�ⲻ֧�ֵ�����





