#include "helmet_interface.h"
//
//pfnIC_Initialize      fnIC_Initialize = NULL;
//pfnIC_Terminate       fnIC_Terminate  = NULL;
//pfnIC_EnableConvertCharacter fnIC_EnableConvertCharacter = NULL;
//pfnIC_Open            fnIC_Open       = NULL;
//pfnIC_Close           fnIC_Close      = NULL;
//pfnIC_ForkPUList      fnIC_ForkPUList = NULL;
//pfnIC_GetResourceName fnIC_GetResourceName = NULL;
//pfnIC_GetResourcePUID fnIC_GetResourcePUID = NULL;
//pfnIC_GetResourceType fnIC_GetResourceType = NULL;
//pfnIC_GetPUOnlineFlag fnIC_GetPUOnlineFlag = NULL;
//pfnIC_ForkPUResource  fnIC_ForkPUResource  = NULL;
//pfnIC_StartStream     fnIC_StartStream     = NULL;
//pfnIC_StopStream      fnIC_StopStream      = NULL;
//pfnIC_ReceiveNotify   fnIC_ReceiveNotify   = NULL;
//pfnIC_ReceiveFrame    fnIC_ReceiveFrame    = NULL;
//pfnIC_SendFrame       fnIC_SendFrame       = NULL;
//pfnIC_SendEndPacket   fnIC_SendEndPacket   = NULL;
//pfnIC_StartTalk       fnIC_StartTalk       = NULL;
//pfnIC_GetResourceHandle fnIC_GetResourceHandle = NULL;
//pfnIC_CSSQueryFile    fnIC_CSSQueryFile    = NULL;
//pfnIC_CSSVODFile      fnIC_CSSVODFile      = NULL;
//pfnIC_CSSVODTime      fnIC_CSSVODTime      = NULL;
//pfnIC_CSSVODPause     fnIC_CSSVODPause     = NULL;
//pfnIC_CSSVODResume    fnIC_CSSVODResume    = NULL;
//pfnIC_CSSVODChangePosition fnIC_CSSVODChangePosition = NULL;
//pfnIC_CSSVODChangeRate fnIC_CSSVODChangeRate = NULL;
//pfnIC_CSSDownloadFile fnIC_CSSDownloadFile = NULL;
//pfnIC_RecvEventMessage fnIC_RecvEventMessage = NULL;
//pfnIC_ReleaseMessage   fnIC_ReleaseMessage   = NULL;
//pfnIC_QueryFile        fnIC_QueryFile        = NULL;
//pfnIC_GetPULongitude   fnIC_GetPULongitude   = NULL;
//pfnIC_GetPULatitude    fnIC_GetPULatitude    = NULL;
//
//void loadlibrary()
//{
//#if defined _WIN32
//    if (NULL == hDllModule)
//    {
//        hDllModule = LoadLibrary("iCVSC2.dll");
//    }
//    
//#define GET_FUN_ADDR    GetProcAddress
//
//#elif defined __linux__
//    if (NULL == hDllModule)
//    {
//        hDllModule = dlopen("libiCVSC2_x64.so", RTLD_NOW);
//    }
//
//#define GET_FUN_ADDR    dlsym
//
//#endif
//
//    if (NULL != hDllModule)
//    {
//        fnIC_Initialize = (pfnIC_Initialize)GET_FUN_ADDR(hDllModule, "IC_Initialize");
//        fnIC_Terminate  = (pfnIC_Terminate)GET_FUN_ADDR(hDllModule, "IC_Terminate");
//        fnIC_EnableConvertCharacter = (pfnIC_EnableConvertCharacter)GET_FUN_ADDR(hDllModule, "IC_EnableConvertCharacter");
//        fnIC_Open = (pfnIC_Open)GET_FUN_ADDR(hDllModule, "IC_Open");
//        fnIC_Close = (pfnIC_Close)GET_FUN_ADDR(hDllModule, "IC_Close");
//        fnIC_ForkPUList = (pfnIC_ForkPUList)GET_FUN_ADDR(hDllModule, "IC_ForkPUList");
//        fnIC_GetResourceName = (pfnIC_GetResourceName)GET_FUN_ADDR(hDllModule, "IC_GetResourceName");
//        fnIC_GetResourcePUID = (pfnIC_GetResourcePUID)GET_FUN_ADDR(hDllModule, "IC_GetResourcePUID");
//        fnIC_GetResourceType = (pfnIC_GetResourceType)GET_FUN_ADDR(hDllModule, "IC_GetResourceType");
//        fnIC_GetPUOnlineFlag = (pfnIC_GetPUOnlineFlag)GET_FUN_ADDR(hDllModule, "IC_GetPUOnlineFlag");
//        fnIC_ForkPUResource  = (pfnIC_ForkPUResource)GET_FUN_ADDR(hDllModule, "IC_ForkPUResource");
//
//        fnIC_StartStream = (pfnIC_StartStream)GET_FUN_ADDR(hDllModule, "IC_StartStream");
//        fnIC_StopStream = (pfnIC_StopStream)GET_FUN_ADDR(hDllModule, "IC_StopStream");
//        fnIC_ReceiveNotify = (pfnIC_ReceiveNotify)GET_FUN_ADDR(hDllModule, "IC_ReceiveNotify");
//        fnIC_ReceiveFrame = (pfnIC_ReceiveFrame)GET_FUN_ADDR(hDllModule, "IC_ReceiveFrame");
//        fnIC_SendFrame = (pfnIC_SendFrame)GET_FUN_ADDR(hDllModule, "IC_SendFrame");
//        fnIC_SendEndPacket = (pfnIC_SendEndPacket)GET_FUN_ADDR(hDllModule, "IC_SendEndPacket");
//        fnIC_StartTalk = (pfnIC_StartTalk)GET_FUN_ADDR(hDllModule, "IC_StartTalk");
//        fnIC_GetResourceHandle = (pfnIC_GetResourceHandle)GET_FUN_ADDR(hDllModule, "IC_GetResourceHandle");
//        fnIC_CSSQueryFile = (pfnIC_CSSQueryFile)GET_FUN_ADDR(hDllModule, "IC_CSSQueryFile");
//        fnIC_CSSVODFile   = (pfnIC_CSSVODFile)GET_FUN_ADDR(hDllModule, "IC_CSSVODFile");
//        fnIC_CSSVODTime   = (pfnIC_CSSVODTime)GET_FUN_ADDR(hDllModule, "IC_CSSVODTime");
//        fnIC_CSSVODPause  = (pfnIC_CSSVODPause)GET_FUN_ADDR(hDllModule, "IC_CSSVODPause");
//        fnIC_CSSVODResume = (pfnIC_CSSVODResume)GET_FUN_ADDR(hDllModule, "IC_CSSVODResume");
//        fnIC_CSSVODChangePosition = (pfnIC_CSSVODChangePosition)GET_FUN_ADDR(hDllModule, "IC_CSSVODChangePosition");
//        fnIC_CSSVODChangeRate = (pfnIC_CSSVODChangeRate)GET_FUN_ADDR(hDllModule, "IC_CSSVODChangeRate");
//        fnIC_CSSDownloadFile  = (pfnIC_CSSDownloadFile)GET_FUN_ADDR(hDllModule, "IC_CSSDownloadFile");
//        fnIC_RecvEventMessage = (pfnIC_RecvEventMessage)GET_FUN_ADDR(hDllModule, "IC_RecvEventMessage");
//        fnIC_ReleaseMessage   = (pfnIC_ReleaseMessage)GET_FUN_ADDR(hDllModule, "IC_ReleaseMessage");
//        fnIC_QueryFile        = (pfnIC_QueryFile)GET_FUN_ADDR(hDllModule, "IC_QueryFile");
//        fnIC_GetPULongitude   = (pfnIC_GetPULongitude)GET_FUN_ADDR(hDllModule, "IC_GetPULongitude");
//        fnIC_GetPULatitude    = (pfnIC_GetPULatitude)GET_FUN_ADDR(hDllModule, "IC_GetPULatitude");
//    }
//
//#undef GET_FUN_ADDR
//}
//
//void freelibrary()
//{
//#if defined _WIN32
//    if (NULL != hDllModule)
//    {
//        FreeLibrary(hDllModule);
//        hDllModule = NULL;
//    }
//#elif defined __linux__
//    if (NULL != hDllModule)
//    {
//        dlclose(hDllModule);
//        hDllModule = NULL;
//    }
//#endif
//}