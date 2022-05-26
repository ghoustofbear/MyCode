// c2platform_pcTalk.cpp: implementation of the c2platform_pcTalk class.
//
//////////////////////////////////////////////////////////////////////
 
#include "c2platform_pcTalk.h"
#include "c2platform_conn.h"

 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern HANDLE m_hLogin;

C2PlatformPcTalk::C2PlatformPcTalk()
{	
	m_hPCTalk = NULL;
}

C2PlatformPcTalk::~C2PlatformPcTalk()
{

}


//************************************************************************
//�������ƣ�C2PcTalkStatusCallback(int code, void* userdata)
//����˵��������״̬�ص�
//����������
//    (in) code ״̬��
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
void C2PcTalkStatusCallback(int code, void* userdata)
{	 
}

//************************************************************************
//�������ƣ�IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S *pParam)
//����˵������ʼ���Խ��豸
//����������
//    (in) u8Type ����
//    (in) pParam ������Ϣ
//����ֵ���ɹ�����TRUE/ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformPcTalk::IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S *pParam)
{	
	return FALSE;
}

//************************************************************************
//�������ƣ�UnIniPcTalk()
//����˵��������ʼ���Խ��豸
//���������� ��
//����ֵ���ɹ�����TRUE/ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformPcTalk::UnIniPcTalk()
{
	return FALSE;
}

//************************************************************************
//�������ƣ�SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size)
//����˵�������ͶԽ���Ϣ
//���������� 
//    (in) u8Type ���ͣ�1-����2-ACK,3-NAK,4-��Ƶ���ݣ�5-ֹͣ
//    (in) pData ����
//    (in) u32Size ���ݳ���
//����ֵ��void
//************************************************************************/
VOID C2PlatformPcTalk::SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size)
{ 
	if(NULL != m_hPCTalk)
	{
		HANDLE view = NULL; //aaaaaaaaaaaaaa;
		switch(u8Type)
		{
		case PCTALK_REQUEST://PC�Խ�����
			{
				c2_start_dev_dualtalk(m_hPCTalk, view, C2PcTalkStatusCallback, this);
			}			
			break;
		case PCTALK_ACK://���նԽ�����
			 
			break;
		case PCTALK_NAK:// �ܾ��Խ�����
			 
			break;
		case PCTALK_DATA://��Ƶ����
			{
				//c2_send_audio(m_hPCTalk, pData, u32Size, 4, 8000, 1, 100, 100);//aaaaaaaaaa
			}
			break;
		case PCTALK_STOP://�����Խ�
			{
				//c2_stop_dev_dualtalk(m_hPCTalk, pData); //aaaaaaaaaaaa
			}
			break;
		case PCTALK_OFFLINE:// �Խ�����
			
			break;
		}
	}
}
