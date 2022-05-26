#ifndef __MUX_AVFILE_H__
#define __MUX_AVFILE_H__

#define MUXAVF_EXPORTS

#ifdef _WIN32
#ifdef   MUXAVF_EXPORTS
#define  MUXAVF_API __declspec(dllexport)
#else
#define  MUXAVF_API __declspec(dllimport)
#endif
#else
#define  MUXAVF_API
#endif
#include <math.h>
#include <stdlib.h>
//#include "def.h"

#ifndef _MUX_DTAT_TYPE_
#define _MUX_DTAT_TYPE_
enum MUX_DTAT_TYPE{
    MUX_DATA_REMOTE,    //��������Ƶps��
    MUX_DATA_LOCAL,     //������Ƶ
};
#endif


#ifndef _MUX_CALL_TYPE_
#define _MUX_CALL_TYPE_
enum MUX_CALL_TYPE{
    MUX_CALL_START,     //��ʼ¼��
    MUX_CALL_END,       //����¼��
};
#endif //__MUX_AVFILE_H__

/**
* @brief ��װ�ļ��ص�
* @param strFilename���ļ�������·����
* @param type���ص�����
* @param pUser���û�����
*
* @author zhangshenyu
* @date 2020415
*/
typedef void(*muxavf_callback)(char *strFilename, MUX_CALL_TYPE type, void* pUser);

/**
* @brief ������װ�ļ�
* @param [in] strFilePath  �ļ�·�� ����E:/��trFilepath����ΪNULL�򳤶ȳ���1024������Ĭ��Ϊ��./��
* @param [in] nSubpackageTime �ְ�ʱ����min������С��5min������5min���㣻�ְ����������򣺡�test-1.avi����...��test-n.avi����n��ʾ����test.avi����ĵ�n���ְ�
* @param [in] callback  �ļ��������رջص� 
* @param [in] pUser  �û����� 
* @see    �ɹ����ط�װ�ļ����,ʧ�ܷ���NULL
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void *MuxAVF_creat(char *strFilePath="./", int nSubpackageTime=60, muxavf_callback callback=NULL, void *pUser=NULL);

/**
* @brief ��������
* @param [in] hMux  �Ѿ�������װ�ļ��ľ��,MuxAVF_creat����ֵ
* @param [in] data	����Ƶ֡����,������PS����������Ƶ��ʽ��G711u
* @param [in] nLen  ���ݳ���
* @param [in] type  ��������
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void MuxAVF_inputData(void *hMux, void *data, int nLen, MUX_DTAT_TYPE type);

/**
* @brief  �رշ�װ�ļ�
* @param [in] hMux  �Ѿ�������װ�ļ��ľ��,MuxAVF_creat����ֵ
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API void MuxAVF_close(void *hMux);

/**
* @brief  ��ȡ���Ĵ�����Ϣ
* @param [in] hMux  �Ѿ�������װ�ļ��ľ��,MuxAVF_creat����ֵ
*
* @author zhangshenyu
* @date 2020415
*/
MUXAVF_API const char *MuxAVF_getLastErr(void *hMux);


#endif