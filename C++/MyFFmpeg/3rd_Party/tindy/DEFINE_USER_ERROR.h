#ifndef _DEFINE_USER_ERROR_H
#define _DEFINE_USER_ERROR_H

//�ṩ���û�����ϸ����˵��
#define USER_ERROR  0x10000000
#define ERROR_NO_DEV					(USER_ERROR+0x01)   //û���ҵ��豸
#define ERROR_UNLOGON					(USER_ERROR+0x02)   //δ��¼
#define ERROR_PARAM_OVER				(USER_ERROR+0x03)   //����Խ��
#define ERROR_REOPERATION				(USER_ERROR+0x04)   //�ظ�����
#define ERROR_WSASTARTUP				(USER_ERROR+0x05)   //WSAStartupִ��ʧ��
#define ERROR_CREATEMSG					(USER_ERROR+0x06)   //������Ϣѭ��ʧ��
#define ERROR_NOSUPPORTRECORD   		(USER_ERROR+0x07)   //��֧��ǰ��¼��
#define ERROR_INITCHANNELNET    		(USER_ERROR+0x08)   //ͨ�����紴��ʧ��
#define ERROR_CREATEDDRAW       		(USER_ERROR+0x09)   //�޷����������DirectDraw
#define ERROR_NO_CHANNEL				(USER_ERROR+0x0A)   //ͨ��û�д���
#define ERROR_NO_FUN            		(USER_ERROR+0x0B)   //�޴˹���
#define ERROR_PARAM_INVALID     		(USER_ERROR+0x0C)   //������Ч 
#define ERROR_DEV_FULL					(USER_ERROR+0x0D)   //�豸�б�����
#define ERROR_LOGON						(USER_ERROR+0x0E)	//�豸�Ѿ���¼�����ڵ�¼
#define ERROR_CREATECPUTHREAD   		(USER_ERROR+0x0F)	//����CPU����߳�ʧ��
#define ERROR_CREATEPLAYER      		(USER_ERROR+0x10)   //����Playerʧ��
#define ERROR_NOAUTHORITY				(USER_ERROR+0x11)   //Ȩ�޲���
#define ERROR_NOTALK            		(USER_ERROR+0x12)   //δ�Խ�
#define ERROR_NOCALLBACK				(USER_ERROR+0x13)   //û���豸�ص�����
#define ERROR_CREATEFILE        		(USER_ERROR+0x14)   //�����ļ�ʧ��
#define ERROR_NORECORD					(USER_ERROR+0x15)   //���Ǵӵ�ǰPlayer�����¼��
#define ERROR_NOPLAYER					(USER_ERROR+0x16)   //û�ж�ӦPlayer
#define ERROR_INITCHANNEL       		(USER_ERROR+0x17)   //ͨ��û�г�ʼ��
#define ERROR_NOPLAYING         		(USER_ERROR+0x18)   //Playerû�в���
#define ERROR_PARAM_LONG				(USER_ERROR+0x19)   //�ַ����������ȹ���
#define ERROR_INVALID_FILE      		(USER_ERROR+0x1A)   //�ļ�������Ҫ��
#define ERROR_USER_FULL					(USER_ERROR+0x1B)   //�û��б�����
#define ERROR_USER_DEL					(USER_ERROR+0x1C)  	//��ǰ�û��޷�ɾ��
#define ERROR_CARD_LOAD					(USER_ERROR+0x1D)	//���ؿ�dllʧ��
#define ERROR_CARD_CORE					(USER_ERROR+0x1E)	//���ؿ��ں�ʧ��
#define ERROR_CARD_COREFILE				(USER_ERROR+0x1F)	//���ؿ��ں��ļ�ʧ��
#define ERROR_CARD_INIT					(USER_ERROR+0x20)	//����ʼ��ʧ��
#define ERROR_CARD_COREREAD				(USER_ERROR+0x21)	//��ȡ���ں��ļ�ʧ��
#define ERROR_CHARACTER_LOAD			(USER_ERROR+0x22)	//�����ֿ�ʧ��
#define ERROR_NOCARD					(USER_ERROR+0x23)   //��δ��ʼ��
#define ERROR_SHOW_MODE					(USER_ERROR+0x24)   //��ʾģʽδ����
#define ERROR_FUN_LOAD					(USER_ERROR+0x25)	//����δ����
#define ERROR_CREATE_DOWNLOAD			(USER_ERROR+0x26)	//û�и��������ͨ������
#define	ERROR_PROXY_DELACT				(USER_ERROR+0x27)   //ɾ����������ģʽ�ķ�����
#define ERROR_PROXY_HASCONNECT			(USER_ERROR+0x28)	//��������
#define ERROR_PROXY_NOPROXY				(USER_ERROR+0x29)   //����û������
#define ERROR_PROXY_IDENTITY			(USER_ERROR+0x2A)	//���Ǵ�������
#define	ERROR_CONNECT_MAX				(USER_ERROR+0x2B)	//�����Ѿ��������
#define	ERROR_NO_DISK					(USER_ERROR+0x2C)	//û�йҽӴ洢�豸
#define	ERROR_NO_DRAW					(USER_ERROR+0x2D)	//û���㹻��Draw
#define	ERROR_INIT_DATA_CHAN			(USER_ERROR+0x2E)	//��ʼ������ͨ��ʧ��
#define	ERROR_SOCKET_CLOSED				(USER_ERROR+0x2F)	//socket�ر�
#define	ERROR_SOCKET_SEND_DATA			(USER_ERROR+0x30)	//socket��������ʧ��
#define	ERROR_DO_UPGRADING				(USER_ERROR+0x31)	//��������
#define ERROR_ALREADY_INTERTALK			(USER_ERROR+0x32)	//���ڽ���˫��Խ�
#define ERROR_FUNCTION_NOT_SUPPORTED	(USER_ERROR+0x33)	//���ܲ������豸��֧��
#define ERROR_DISPLAY_ON_OTHER_WND		(USER_ERROR+0x34)	//����������������ʾ
#define ERROR_BUFFER_TOO_SMALL			(USER_ERROR+0x35)	//������̫С
#define ERROR_NVR_NOT_SUPPORT			(USER_ERROR+0x36)	//���豸��֧��NVR��ع���
#define	ERROR_SOCKET_NULL				(USER_ERROR+0x37)	//socketδ���������ͷ�
#define	ERROR_EVENT_NOT_ENABLE			(USER_ERROR+0x38)	//�������д��¼�û��ʹ��
#define ERROR_RULE_NOT_ENALE			(USER_ERROR+0x39)	//�˹���û��ʹ��
#define	ERROR_INIT_CHAN					(USER_ERROR+0x3A)	//��ʼ��ͨ��ʧ��
#define	ERROR_PROLEN_INVALID 			(USER_ERROR+0x3B)	//Э�鳤�Ȳ��Ϸ�
#define	ERROR_UNSUPPORT_CHARSET 		(USER_ERROR+0x3C)	//��֧�ֵ��ַ���
#define ERROR_CHARTRANS_FAILED			(USER_ERROR+0x3D)	//�ַ�ת��ʧ��
#define ERROR_WAITREPLY_FAILED			(USER_ERROR+0x3E)	//�ȴ��豸��Ŀ¼����������ʧ��(δ�����ʱ)
#define ERROR_NOTFOUND_PLAYERID			(USER_ERROR+0x3F)	//SDK�ڲ�δ�ҵ��Ϸ���playerID
#define ERROR_UNSUPPORT_STREAM			(USER_ERROR+0x40)	//��֧����������
#define ERROR_INVALID_DECLIBIDX			(USER_ERROR+0x41)	//������������Ϸ�
#define ERROR_SYNCSTARTRECV_TIMEOUT		(USER_ERROR+0x42)	//ͬ������������Ƶ��ʱ
#define ERROR_USERINPUT_NULLPTR			(USER_ERROR+0x43)	//�û������ָ��
#define ERROR_IPANDID_BOTHINVALID		(USER_ERROR+0x44)	//�û������豸IP�ͳ���ID���Ƿ�
#define ERROR_INVALID_IPADDR			(USER_ERROR+0x45)	//�û�����Ƿ���ip��ַ
#define ERROR_INVALID_BUFLEN			(USER_ERROR+0x46)	//�û�����Ƿ��Ļ���������
#define ERROR_CUR_UPGRADING 			(USER_ERROR+0x47)	//����������
#define	ERROR_BUILD_PROTOCOL			(USER_ERROR+0x48)	//��Э��ʧ�� build protocol error
#define ERROR_INNERCALL_FAILED			(USER_ERROR+0x49)	//�����ڲ�����ʧ��
#define	ERROR_COMMANDCHAN_EMPTY			(USER_ERROR+0x50)	//CommanChan��
#define ERROR_GET_VIDEOSIZE_FAILED		(USER_ERROR+0x51)	//��ȡ�ֱ���ʧ��
#define ERROR_GET_DEVICETYPE_FAILED		(USER_ERROR+0x52)	//��ȡ�����豸����ʧ��
#define ERROR_GET_SENSORFLIP_FAILED		(USER_ERROR+0x53)	//��ȡCMOS��ͷ��תʧ��
#define ERROR_MALLOCMEM_FALIED			(USER_ERROR+0x54)	//�����ڴ�ʧ��
#define ERROR_CALLCURLAPI_FALIED		(USER_ERROR+0x55)	//����curl��ӿ�ʧ��

#endif
