#pragma once
/*
模块统一导出函数模板。
使用时填入函数实体
*/

#include <Windows.h>
#include <LdLib.h>
#include <LdApp.h>

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

//在没有引用界面库的时候，用VOID以便编译通过
#ifndef PLUGIN_UI_CLASS
#define PLUGIN_UI_CLASS VOID
#endif

//模块功能接口。
interface IPluginInterface
{
	virtual ~IPluginInterface()
	{
	}

	virtual PLUGIN_UI_CLASS* CreateUI() = 0;
	virtual DWORD InitCommunicate() = 0;
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
PLUGIN_PROPERTY LDLIB_API API_Register()                \
{                                                       \
	return GetSelfDesc();                               \
}

extern HANDLE ThisModule;
#else

typedef PLUGIN_PROPERTY(*_API_Register)();
typedef IPluginInterface*(*_API_Init)(CLdApp*);

class CPluginManager
{
public:
	CPluginManager(const TCHAR* path);
	~CPluginManager();
	void FindPlugin(PLUGIN_USAGE usage, CLdArray<PLUGIN_PROPERTY>* out_result);
	IPluginInterface* LoadPlugin(CLdApp* app, TCHAR* plugid);
private:
	CLdMap<CLdString, PLUGIN_PROPERTY> m_Plugins;
	CLdString m_Plug_path;
	INT_PTR EnumFile_Callback(PVOID data, UINT_PTR param);
	void LoadAllPlugins();
};

#endif
