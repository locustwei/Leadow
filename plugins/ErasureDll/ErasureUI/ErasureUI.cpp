#include "stdafx.h"
#include "ErasureUI.h"
#include "ErasureMainWnd.h"
#include "../../plugin.h"
#include "../ErasureConfig.h"

#define PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262F")

CErasureUI* Impl = nullptr;
CErasureMainWnd* Wnd = nullptr;

#pragma region 动态库导出函数

#define CreatePluginImpl() new CErasureUI();
#define DeletePluginImpl() \
if(Impl)            \
{                          \
	delete Impl;    \
	Impl = nullptr; \
}                          \

PLUGIN_PROPERTY GetSelfDesc()
{
	PLUGIN_PROPERTY ErasurePrpperty = { PLUGIN_USAGE_UI, PLUGIN_ID };
	return ErasurePrpperty;
}

API_Init();
API_UnInit();
API_Register();

#pragma endregion  动态库导出函数

CErasureUI::CErasureUI()
	: m_hModule(nullptr)
{
	m_hModule = (HMODULE)ThisModule;
}

CErasureUI::~CErasureUI()
{
	Impl = nullptr;
	if (Wnd)
		delete Wnd;
}


HMODULE CErasureUI::GetModuleHandle()
{
	return m_hModule;
}

CFramNotifyPump* CErasureUI::CreateUI()
{
	if(!Wnd)
		Wnd= new CErasureMainWnd();
	return Wnd;
}

DWORD CErasureUI::InitCommunicate()
{
	DebugOutput(L"InitCommunicate\n");
	CLdString configfile = ThisApp->GetAppDataPath();
	configfile += _T("ErasureConfig.cfg");
	ErasureConfig.LoadFromFile(configfile);
	return 0;
}
