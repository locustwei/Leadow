#pragma once

#include <Windows.h>

/*��̬���ӿ⵼����������ȡ�����ӿ�*/
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
