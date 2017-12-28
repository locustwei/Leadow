#pragma once

#include <Windows.h>

#ifndef UI_CLASS_NAME
#define UI_CLASS_NAME void
#endif

/*动态链接库导出函数，获取函数接口*/
#define API_Init()                                      \
IPluginInterface LDLIB_API * API_Init(CLdApp* pThisApp) \
{                                                       \
	ThisApp = pThisApp;                                 \
	return CreatePluginImpl();                          \
}

#define  API_UnInit()                                   \
VOID LDLIB_API API_UnInit()                             \
{                                                       \
	DeletePluginImpl();                                 \
}

enum PLUGIN_PROPERTYS
{
	PP_SHORT_NAME
};

interface IPluginInterface
{
	virtual UI_CLASS_NAME* CreateUI() = 0;
	virtual TCHAR* GetPropertys(PLUGIN_PROPERTYS nproperty) = 0;
};

extern HANDLE ThisModule;
