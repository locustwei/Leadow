#include "stdafx.h"
#include "Library.h"

#ifdef _DEBUG
#ifdef WIN64
#define FILE_ERASURE_DLL _T("RdErasure_d64.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d64.dll")
#else
#define FILE_ERASURE_DLL _T("RdErasure_d32.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d32.dll")
#endif
#else
#ifdef _X64
#define FILE_ERASURE_DLL _T("RdErasure_d32.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d32.dll")
#else
#define FILE_ERASURE_DLL _T("RdErasure_d32.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d32.dll")
#endif
#endif

typedef PVOID(*Library_Init)(CLdApp*);
typedef VOID(*Library_UnInit)();

IErasureLibrary * CLdLibray::LoadErasureLibrary()
{
	IErasureLibrary * result = (IErasureLibrary*)InitLib(FILE_ERASURE_DLL, CLdApp::ThisApp);
	
	return result;
}

void CLdLibray::FreeErasureLibrary()
{
	FreeLib(FILE_ERASURE_DLL);
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
