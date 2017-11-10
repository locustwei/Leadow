#include "stdafx.h"
#include "ErasureImpl.h"
#include "../LdApp/LdStructs.h"
#include "../LdApp/LdApp.h"


/*动态链接库导出函数，获取函数接口*/
IErasureLibrary LDLIB_API * API_Init(CLdApp* pThisApp)
{
	ThisApp = pThisApp;
	if (!ErasureImpl)
	{
		ErasureImpl = new CErasureImpl();
	}
	return ErasureImpl;
}

VOID LDLIB_API API_UnInit()
{
	if (ErasureImpl)
	{
		delete ErasureImpl;
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason) {
	case DLL_PROCESS_DETACH:
		API_UnInit();
		break;
	case DLL_PROCESS_ATTACH:
		ErasureImpl = new CErasureImpl();
		ErasureImpl->m_hModule = (HMODULE)hModule;
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
