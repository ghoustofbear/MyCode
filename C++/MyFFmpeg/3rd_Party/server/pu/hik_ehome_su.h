/**
 * @file      hik_ehome_su.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    quxiaohui
 * @date      2017-06-20 15:14:00
 *
 * @note
 *  hik_ehome_su.h defines
 */
#ifndef __HIK_EHOME_SU_H__
#define __HIK_EHOME_SU_H__

#include "su.h"
#include "hik_ehome_pu.h"
#include "3rd/hik/sdk-plat.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "encoder.h"
class HikEhomeSu : public Su
{

public:
    HikEhomeSu(Pu *pu, int channelNo);
    HikEhomeSu(Pu *pu, string addrcode);
    virtual ~HikEhomeSu();

    virtual int        start();
    virtual void       stop();
    virtual int        reset();

    virtual void       doVideoStream(void *stream, uint32_t size);
	virtual void       doAudioStream(void *stream, uint32_t size, int flag);
	virtual void       doStatus(int code);

	virtual void       onTimer();

protected:
	void               encode_send(void* data, int size = 320);

private:
	const int SEND_SIZE = 160;
	const int BUFFER_SIZE = 2048;
	NET_EHOME_DEVICE_INFO m_deviceInfo;//设备信息结构体
	AudioEncoder*         m_audio_encode;
	string                m_deviceID;
	LONG                  m_lTalkHandle;
	unsigned char*        m_workspace;
	unsigned char*        m_buff;
	int                   m_size;//buffer remain data size
};
#endif
