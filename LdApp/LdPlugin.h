#pragma once
#include "LdApp.h"

#define PLUGIN_PATH _T("modules")
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
