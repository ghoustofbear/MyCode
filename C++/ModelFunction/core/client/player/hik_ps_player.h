/**
* @file      hik_ps_player.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    quxiaohui
* @date      2017-07-06 17:00:59
*
* @note
*  hik_ps_player.h defines
*/
#ifndef _HIK_PS_PLAYER_H_
#define _HIK_PS_PLAYER_H_


#ifndef __ANDROID__

#include "PsProcessApi.h"
#include "player.h"
#include "3rd/hik/sdk-plat.h"
#define CBK                  __stdcall
class PlayerListener;
class HikPsPlayer : public Player
{
public:
	HikPsPlayer(PlayerListener *listener);
	~HikPsPlayer();
	virtual int        open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);
	virtual int        open(HWND wnd, string filename, int poolSize);
	virtual void       close();
	virtual int        openReverse(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size);

	virtual int        inputData(void *hdr, uint32_t size);
	virtual int        control(int code, void *val, int valSize);
	virtual int        capture(string filepath, int type);
	virtual int        pos(datetime_s *start = NULL, datetime_s *end = NULL);
	virtual int        time(datetime_s &time);

	virtual int        startREC(string filepath);
	virtual void       stopREC();

	virtual int        changeWndSize(int width, int height);
	virtual int        setDisplayRegion(float left, float right, float top, float bottom);

	virtual int        opensound();
	virtual int        closesound();
	virtual int        setVolume(int vol);

	virtual void       enableDraw(bool enable = true);
	virtual void       enableDecode(bool enable = true);
	virtual void       REC(void *frame, size_t size);
	virtual void       timerdriver();
protected:
#if defined(__linux__)
	static void        decdatacb(int port, char *data, int size, FRAME_INFO *info, void *reserved1, int reserved2);
	static void        drawdatacb(int handle, PLAYM4_HDC hdc, void *userData);
#elif defined(_WIN64)
	static void CBK    decdatacb(long port, char *data, long size, FRAME_INFO *info, HIK_USERDATA reserved1, HIK_USERDATA reserved2);
	static void CBK    drawdatacb(long handle, HDC hdc, HIK_USERDATA userData);
#elif defined(_WIN32)
	static void CBK    decdatacb(long port, char *data, long size, FRAME_INFO *info, long reserved1, long reserved2);
	static void CBK    drawdatacb(long handle, HDC hdc, long userData);
#endif

public:
	long               m_playID;
    uint64_t           m_framePts;
private:
	bool               m_bReverse;
	FILE             * m_recfile;
	void             * pspacker;  //PS·â×°
};
#endif

#endif //__ANDROID__


