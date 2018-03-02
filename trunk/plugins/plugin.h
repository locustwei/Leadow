#pragma once
/*
ģ��ͳһ��������ģ�塣
ʹ��ʱ���뺯��ʵ��
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

//ģ�鹦�ܽӿڡ�
interface __declspec(dllexport) IPluginInterface
{
	virtual ~IPluginInterface()
	{
	}

	virtual UI_CLASS_NAME* CreateUI() = 0;
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
