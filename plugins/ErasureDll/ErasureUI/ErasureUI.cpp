#include "stdafx.h"
#include "ErasureUI.h"
#include "ErasureMainWnd.h"
#include "../../plugin.h"

#define PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262F")

CErasureUI* ErasureUI = nullptr;


//��̬�⵼������---------------------------------------
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
//--------------------------------------------------------

CErasureUI::CErasureUI()
	: m_hModule(nullptr)
	, m_Comm()
{
	m_hModule = (HMODULE)ThisModule;
}

CErasureUI::~CErasureUI()
{
	if (m_Comm)
		delete m_Comm;

	ErasureUI = nullptr;
}


HMODULE CErasureUI::GetModuleHandle()
{
	return m_hModule;
}

typedef struct ERASE_FILE_PARAM {
	CLdString progress;
	
	BOOL bRemoveFolder;
}*PERASE_FILE_PARAM;


CFramNotifyPump* CErasureUI::CreateUI()
{
	return new CErasureMainWnd;
}

DWORD CErasureUI::InitCommunicate()
{
	DebugOutput(L"InitCommunicate");

	return 0;
}

bool CErasureUI::OnCreate()
{
	return true;
}

void CErasureUI::OnTerminate(DWORD exitcode)
{
}

void CErasureUI::OnCommand(WORD id, TCHAR* ProcessName, PVOID data, WORD nSize)
{
	
	DebugOutput(L"unknown command id=%d", id);
}
