/**
 * @file      watchdog.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2019-05-31 11:32:59
 *
 * @note
 *  watch dog, use local socket
 */
#ifndef __UT_PROCESS_H__
#define __UT_PROCESS_H__
#include <string>

int isRuning(std::string ID);

int initDemo();

int killProcess(const char* appName);

int createProcess(const char* exeName, const char* appName, const char* workPath);


#endif
