#include "stdafx.h"
#include "ErasureLibrary.h"
#include "ErasureImpl.h"


/*动态链接库导出函数，获取函数接口*/
IErasureLibrary LDLIB_API * API_Init(CLdApp* pThisApp)
{
	ThisApp = pThisApp;
	if (!ThisLibrary)
	{
		ThisLibrary = new CErasureImpl();
	}
	return ThisLibrary;
}

VOID LDLIB_API API_UnInit()
{
	if (ThisLibrary)
	{
		delete ThisLibrary;
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason) {
	case DLL_PROCESS_DETACH:
		API_UnInit();
		break;
	case DLL_PROCESS_ATTACH:
		ThisLibrary = new CErasureImpl();
		ThisLibrary->m_hModule = (HMODULE)hModule;
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
