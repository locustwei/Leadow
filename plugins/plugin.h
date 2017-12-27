#pragma once

#include <Windows.h>

/*动态链接库导出函数，获取函数接口*/
#define API_Init()                                     \
IPluginInterface LDLIB_API * API_Init(CLdApp* pThisApp) \
{                                                      \
	ThisApp = pThisApp;                                \
	return CreatePluginImpl();                         \
}

#define  API_UnInit()                                  \
VOID LDLIB_API API_UnInit() \
{                                                      \
	DeletePluginImpl();                                \
}
