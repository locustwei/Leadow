#pragma once
/*
模块统一导出函数模板。
使用时填入函数实体
*/

#include <Windows.h>

enum PLUGIN_USAGE
{
	PLUGIN_USAGE_UI = 0b0001,
	PLUGIN_USAGE_BK = 0b0010,
};

typedef struct PLUGIN_PROPERTY
{
	DWORD func;
	TCHAR id[37];
}*PPLUGIN_PROPERTY;

#ifndef UI_CLASS_NAME
#define UI_CLASS_NAME void
#endif

//模块功能接口。
interface __declspec(dllexport) IPluginInterface
{
	virtual ~IPluginInterface()
	{
	}

	virtual UI_CLASS_NAME* CreateUI() = 0;
};

#ifdef _PLUGIN_

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

/*注册，用于插件自身说明*/
#define  API_Register()                                 \
PLUGIN_PRPPERTY LDLIB_API API_Register()                \
{                                                       \
	return GetSelfDesc();                               \
}

extern HANDLE ThisModule;
#else

typedef PLUGIN_PROPERTY(*_API_Register)();
typedef IPluginInterface(*API_Init)(PVOID);

class CPluginManager
{
public:
	static void FindPlugin(const TCHAR* path, PLUGIN_USAGE usage, CLdMap<CLdString, PLUGIN_PROPERTY>* out_result);
	static IPluginInterface 
};

#endif
