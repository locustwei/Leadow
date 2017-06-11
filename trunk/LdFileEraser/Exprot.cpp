#include "stdafx.h"
#include "Exprot.h"
#include "FileEraser.h"
#include "ErasureMainWnd.h"


DWORD LDLIB_API API_Init(PAuthorization)
{
	return 0;
}

VOID LDLIB_API API_UnInit()
{
	
}

CFramWnd LDLIB_API *  API_LibraryUI()
{
	return new CErasureMainWnd();
}


BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		API_UnInit();
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}
	return TRUE;
}