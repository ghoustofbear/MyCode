/**
* @file      sdk-plat.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     different platform header files defines
* @author    chenjiaqi
* @date      2018-2-6
*
* @note
*  sdk-plat.h defines
*/
#ifndef __TINDY_SDK_PLATFORM_H__
#define __TINDY_SDK_PLATFORM_H__

#if defined(_WIN32)

//#include "3rd\tindy\NetClientTypes.h"
#include "3rd\tindy\NetSdkClient.h"
#include "3rd\tindy\ActionControl.h"
#include "3rd\tindy\PLAYSDKM4_INTERFACE.h"




#elif defined(__linux__)

#include "3rd/tindy/NetSdkClient.h"
#include "3rd/tindy/ActionControl.h"
#include "3rd/tindy/PLAYSDKM4_INTERFACE.h"

#elif defined(__APPLE__)

#elif defined(ANDROID)

#endif
#endif
