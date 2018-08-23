#pragma once
/*
ģ��ͳһ��������ģ�塣
ʹ��ʱ���뺯��ʵ��
*/

#include <Windows.h>

/*��ȡģ�鹦�ܽӿڣ�IPluginInterface��*/
#define API_Init()                                      \
LDLIB_API IPluginInterface * API_Init(CLdApp* pThisApp) \
{                                                       \
	ThisApp = pThisApp;                                 \
	IPluginInterface* result = CreatePluginImpl();      \
	if(result)                                          \
		result->InitCommunicate();                      \
	return result;                                      \
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
