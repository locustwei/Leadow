#pragma once
/*
模块统一导出函数模板。
使用时填入函数实体
*/

#include <Windows.h>

/*获取模块功能接口（IPluginInterface）*/
#define API_Init()                                      \
LDLIB_API IPluginInterface * API_Init(CLdApp* pThisApp) \
{                                                       \
	ThisApp = pThisApp;                                 \
	IPluginInterface* result = CreatePluginImpl();      \
	if(result)                                          \
		result->InitCommunicate();                      \
	return result;                                      \
}

/*资源释放*/
#define  API_UnInit()                                   \
VOID LDLIB_API API_UnInit()                             \
{                                                       \
	DeletePluginImpl();                                 \
}

/*注册，用于插件自身说明*/
#define  API_Register()                                 \
PLUGIN_PROPERTY LDLIB_API API_Register()                \
{                                                       \
	return GetSelfDesc();                               \
}

extern HANDLE ThisModule;
