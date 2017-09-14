#include "stdafx.h"
#include "Library.h"

#ifdef _DEBUG
#ifdef _X64
#define FILE_ERASURE_DLL _T("RdErasure_d64.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d64.dll")
#else
#define FILE_ERASURE_DLL _T("RdErasure_d32.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d32.dll")
#endif
#else
#ifdef _X64
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#endif
#endif

typedef PVOID(*Library_Init)(CLdApp*);
typedef VOID(*Library_UnInit)();

IErasureLibrary * CLdLibray::LoadErasureLibrary()
{
	IErasureLibrary * result = (IErasureLibrary*)InitLib(FILE_ERASURE_DLL, CLdApp::ThisApp);

	if(result)
	{
		CControlUI* lui = CLdLibray::BuildXml(result->UIResorce());
		if (lui)
		{
			result->SetUI(lui);
		}
	}

	return result;
}

void CLdLibray::FreeErasureLibrary()
{
	FreeLib(FILE_ERASURE_DLL);
}

CControlUI * CLdLibray::BuildXml(TCHAR * skinXml)
{
	if (skinXml == nullptr || _tcslen(skinXml) == 0)
		return nullptr;

	CDialogBuilder builder;
	CPaintManagerUI pm_ui;
	CControlUI * pControl = builder.Create(skinXml, nullptr, NULL, &pm_ui);
	_ASSERTE(pControl);

	return pControl;
}


PVOID CLdLibray::InitLib(TCHAR * pLibFile, CLdApp* ThisApp)
{
	HMODULE hModule = LoadLibrary(pLibFile);
	if (hModule == NULL)
		return nullptr;
	Library_Init fnInit = (Library_Init)GetProcAddress(hModule, "API_Init");
	if (fnInit == NULL)
		return NULL;
	return fnInit(ThisApp);
}

VOID CLdLibray::FreeLib(TCHAR * pLibFile)
{
	HMODULE hModule = GetModuleHandle(pLibFile);
	if (hModule == NULL)
		return;
	Library_UnInit fnUnInit = (Library_UnInit)GetProcAddress(hModule, "API_UnInit");
	if (fnUnInit == NULL)
		return ;
	fnUnInit();
}
