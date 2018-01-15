#pragma once
/*
模块统一导出函数模板。
使用时填入函数实体
*/

#include <Windows.h>

#ifndef UI_CLASS_NAME
#define UI_CLASS_NAME void
#endif

/*获取模块功能接口（IPluginInterface）*/
#define API_Init()                                      \
IPluginInterface LDLIB_API * API_Init(CLdApp* pThisApp) \
{                                                       \
	ThisApp = pThisApp;                                 \
	return CreatePluginImpl();                          \
}

/*资源释放*/
#define  API_UnInit()                                   \
VOID LDLIB_API API_UnInit()                             \
{                                                       \
	DeletePluginImpl();                                 \
}

typedef struct PLUGIN_PRPPERTY
{
	DWORD type;
	TCHAR* id;
}*PPLUGIN_PRPPERTY;

/*注册，用于插件自身说明*/
#define  API_Register()                                 \
PLUGIN_PRPPERTY LDLIB_API API_Register()               \
{                                                       \
	return GetSelfDesc();                               \
}

//模块功能接口。
interface IPluginInterface
{
	virtual UI_CLASS_NAME* CreateUI() = 0;
};

#ifdef _PLUGIN_
extern HANDLE ThisModule;
#endif
