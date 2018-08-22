#include "stdafx.h"
#include "ErasureUI.h"
#include "ErasureMainWnd.h"
#include "../../plugin.h"

#define PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262F")

CErasureUI* ErasureUI = nullptr;

#pragma region 动态库导出函数

#define CreatePluginImpl() new CErasureUI();
#define DeletePluginImpl() \
if(ErasureUI)            \
{                          \
	delete ErasureUI;    \
	ErasureUI = nullptr; \
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
	ErasureUI = nullptr;
}


HMODULE CErasureUI::GetModuleHandle()
{
	return m_hModule;
}

CFramNotifyPump* CErasureUI::CreateUI()
{
	return new CErasureMainWnd;
}

DWORD CErasureUI::InitCommunicate()
{
	DebugOutput(L"InitCommunicate");

	return 0;
}
