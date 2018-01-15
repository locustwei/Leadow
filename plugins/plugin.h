#pragma once
/*
ģ��ͳһ��������ģ�塣
ʹ��ʱ���뺯��ʵ��
*/

#include <Windows.h>

#ifndef UI_CLASS_NAME
#define UI_CLASS_NAME void
#endif

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

typedef struct PLUGIN_PRPPERTY
{
	DWORD type;
	TCHAR* id;
}*PPLUGIN_PRPPERTY;

/*ע�ᣬ���ڲ������˵��*/
#define  API_Register()                                 \
PLUGIN_PRPPERTY LDLIB_API API_Register()               \
{                                                       \
	return GetSelfDesc();                               \
}

//ģ�鹦�ܽӿڡ�
interface IPluginInterface
{
	virtual UI_CLASS_NAME* CreateUI() = 0;
};

#ifdef _PLUGIN_
extern HANDLE ThisModule;
#endif
