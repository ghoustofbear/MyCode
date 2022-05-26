#include "PLAYSDKM4_INTERFACE.h"
#include "AVPlaySdkTypes.h"

#ifdef __linux__
#include <dlfcn.h>
#endif


using namespace PLAYSDK_INTERFACE;
namespace PLAYSDK_INTERFACE
{
    #define IMPLEMENT_FUNCTION(funcName)  pf##funcName funcName = NULL

#if defined(WIN32)
    #define EXPORT_FUNCTION(hDll, funcName)	funcName = (pf##funcName)::GetProcAddress(hDll, #funcName);
#else
    #define EXPORT_FUNCTION(hDll, funcName)	funcName = (pf##funcName)dlsym(hDll, #funcName);
#endif

    IMPLEMENT_FUNCTION(TC_CreateSystem);
    IMPLEMENT_FUNCTION(TC_DeleteSystem);
    IMPLEMENT_FUNCTION(TC_RegisterNotifyPlayToEnd);
    IMPLEMENT_FUNCTION(TC_CreatePlayerFromStream);
    IMPLEMENT_FUNCTION(TC_CreatePlayerFromFile);
    IMPLEMENT_FUNCTION(TC_CreatePlayer);
    IMPLEMENT_FUNCTION(TC_DeletePlayer);
    IMPLEMENT_FUNCTION(TC_SetPlayRect);
    IMPLEMENT_FUNCTION(TC_SetPlayRectEx);
    IMPLEMENT_FUNCTION(TC_PlayAudio);
    IMPLEMENT_FUNCTION(TC_StopAudio);
    IMPLEMENT_FUNCTION(TC_Play);
    IMPLEMENT_FUNCTION(TC_Stop);
    IMPLEMENT_FUNCTION(TC_GetPlayingFrameNum);
    IMPLEMENT_FUNCTION(TC_FastForward);
    IMPLEMENT_FUNCTION(TC_GetFrameCount);
    IMPLEMENT_FUNCTION(TC_GetVersion);
    IMPLEMENT_FUNCTION(TC_SeekEx);
    IMPLEMENT_FUNCTION(TC_RegisterEventMsg);
    IMPLEMENT_FUNCTION(TC_RegisterCommonEventCallBackEx);
    IMPLEMENT_FUNCTION(TC_SetVideoDecryptKey);
    IMPLEMENT_FUNCTION(TC_PutStreamToPlayer);
    IMPLEMENT_FUNCTION(TC_GetStreamPlayBufferState);
    IMPLEMENT_FUNCTION(TC_CreatePlayerFromVoD);
    IMPLEMENT_FUNCTION(TC_CreatePlayerFromStreamEx);
    IMPLEMENT_FUNCTION(TC_CleanStreamBuffer);
    IMPLEMENT_FUNCTION(TC_GetUserDataInfo);
    IMPLEMENT_FUNCTION(TC_SetVoDPlayerOver);
    IMPLEMENT_FUNCTION(TC_SetModeRule);
    IMPLEMENT_FUNCTION(TC_CapturePicture);
    IMPLEMENT_FUNCTION(TC_RegisterDrawFun);
    IMPLEMENT_FUNCTION(TC_SetDecCallBack);
    IMPLEMENT_FUNCTION(TC_GetCurFrameNo);
    IMPLEMENT_FUNCTION(TC_GetPlayTime);
    IMPLEMENT_FUNCTION(TC_Pause);
    IMPLEMENT_FUNCTION(TC_SlowForward);
    IMPLEMENT_FUNCTION(TC_SetStreamBufferSize);
	IMPLEMENT_FUNCTION(TC_GetVideoParam);
	IMPLEMENT_FUNCTION(TC_StepForward);
	IMPLEMENT_FUNCTION(TC_DeletePlayerEx);
	IMPLEMENT_FUNCTION(TC_CaptureBmpPic);
	
	


#if defined(WIN32)
    static HINSTANCE hPLAYSDK = NULL;
#else
    static void* hPLAYSDK = NULL;
#endif
    int LoadPLAYSDK()
    {
        if (hPLAYSDK)
        {
            return	0;
        }      
        else
        {
#if defined(WIN32)
            hPLAYSDK = ::LoadLibrary("PlaySdkM4.dll");
#else
            hPLAYSDK = dlopen("libplaysdk.so", RTLD_LAZY);
#endif

            if (!hPLAYSDK)
            {
                return	-2;
            }
        }


        EXPORT_FUNCTION(hPLAYSDK, TC_CreateSystem);
        EXPORT_FUNCTION(hPLAYSDK, TC_DeleteSystem);
        EXPORT_FUNCTION(hPLAYSDK, TC_RegisterNotifyPlayToEnd);
        EXPORT_FUNCTION(hPLAYSDK, TC_CreatePlayerFromStream);
        EXPORT_FUNCTION(hPLAYSDK, TC_CreatePlayerFromFile);
        EXPORT_FUNCTION(hPLAYSDK, TC_CreatePlayer);
        EXPORT_FUNCTION(hPLAYSDK, TC_DeletePlayer);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetPlayRect);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetPlayRectEx);
        EXPORT_FUNCTION(hPLAYSDK, TC_PlayAudio);
        EXPORT_FUNCTION(hPLAYSDK, TC_StopAudio);
        EXPORT_FUNCTION(hPLAYSDK, TC_Play);
        EXPORT_FUNCTION(hPLAYSDK, TC_Stop);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetPlayingFrameNum);
        EXPORT_FUNCTION(hPLAYSDK, TC_FastForward);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetFrameCount);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetVersion);
        EXPORT_FUNCTION(hPLAYSDK, TC_SeekEx);
        EXPORT_FUNCTION(hPLAYSDK, TC_RegisterEventMsg);
        EXPORT_FUNCTION(hPLAYSDK, TC_RegisterCommonEventCallBackEx);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetVideoDecryptKey);
        EXPORT_FUNCTION(hPLAYSDK, TC_PutStreamToPlayer);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetStreamPlayBufferState);
        EXPORT_FUNCTION(hPLAYSDK, TC_CreatePlayerFromVoD);
        EXPORT_FUNCTION(hPLAYSDK, TC_CreatePlayerFromStreamEx);
        EXPORT_FUNCTION(hPLAYSDK, TC_CleanStreamBuffer);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetUserDataInfo);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetVoDPlayerOver);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetModeRule);
        EXPORT_FUNCTION(hPLAYSDK, TC_CapturePicture);
        EXPORT_FUNCTION(hPLAYSDK, TC_RegisterDrawFun);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetDecCallBack);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetCurFrameNo);
        EXPORT_FUNCTION(hPLAYSDK, TC_GetPlayTime);
        EXPORT_FUNCTION(hPLAYSDK, TC_Pause);
        EXPORT_FUNCTION(hPLAYSDK, TC_SlowForward);
        EXPORT_FUNCTION(hPLAYSDK, TC_SetStreamBufferSize);
		EXPORT_FUNCTION(hPLAYSDK,TC_GetVideoParam);
		EXPORT_FUNCTION(hPLAYSDK,TC_StepForward);
		EXPORT_FUNCTION(hPLAYSDK,TC_DeletePlayerEx);
		EXPORT_FUNCTION(hPLAYSDK,TC_CaptureBmpPic);
	
        
        int sss = TC_CreateSystem(NULL);
        return sss;
    }
    bool ReleasePLAYSDK()
    {
        if (hPLAYSDK)
        {
            TC_DeleteSystem();


#if defined(WIN32)
            ::FreeLibrary(hPLAYSDK);
#else
            dlclose(hPLAYSDK);
#endif
            hPLAYSDK = NULL;
        }

        return true;
    }
}
