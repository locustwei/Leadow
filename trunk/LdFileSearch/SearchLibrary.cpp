// LdFileSearch.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SearchLibrary.h"

class CSearchLibrary;

CSearchLibrary* g_Library = nullptr;

class CSearchLibrary : public ISearchLibrary
{
public:
	UINT64 EnumVolumeFiles(CVolumeInfo* pVolume, IMftReadeHolder* callback, PVOID Context) override
	{
		return 0;
	};

	CSearchLibrary()
	{
	};

	~CSearchLibrary() override
	{
		g_Library = nullptr;
	};

private:
};


ISearchLibrary LDLIB_API * API_Init(PAuthorization)
{
	if (!g_Library)
	{
		CLdApp::Initialize(nullptr);
		g_Library = new CSearchLibrary();
	}
	return g_Library;
}

VOID LDLIB_API API_UnInit()
{
	if (g_Library)
	{
		delete g_Library;
		g_Library = nullptr;
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason) {
	case DLL_PROCESS_DETACH:
		API_UnInit();
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}
	return TRUE;
}
