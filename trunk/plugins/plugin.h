#pragma once
/*
ģ��ͳһ��������ģ�塣
ʹ��ʱ���뺯��ʵ��
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

//��û�����ý�����ʱ����VOID�Ա����ͨ��
#ifndef PLUGIN_UI_CLASS
#define PLUGIN_UI_CLASS VOID
#endif

//ģ�鹦�ܽӿڡ�
interface IPluginInterface
{
	virtual ~IPluginInterface()
	{
	}

	virtual PLUGIN_UI_CLASS* CreateUI() = 0;
	virtual DWORD InitCommunicate() = 0;
};

#ifdef _PLUGIN_

/*��ȡģ�鹦�ܽӿڣ�IPluginInterface��*/
#define API_Init()                                      \
IPluginInterface LDLIB_API * API_Init(CLdApp* pThisApp) \
{                                                       \
	ThisApp = pThisApp;                                 \
	return CreatePluginImpl();                          \
}

/*��Դ�ͷ�*/
#define  API_UnInit()                                   \
VOID LDLIB_API API_UnInit()                             \
{                                                       \
	DeletePluginImpl();                                 \
}

/*ע�ᣬ���ڲ������˵��*/
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
