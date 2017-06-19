#include "stdafx.h"
#include "FileEraser.h"
#include "ErasureMainWnd.h"
#include "ErasureLibrary.h"

IErasureLibrary* g_Library = NULL;


IErasureLibrary LDLIB_API * API_Init(PAuthorization)
{
	if (!g_Library)
		g_Library = new IErasureLibrary();
	return g_Library;
}

VOID LDLIB_API API_UnInit()
{
	if (g_Library)
	{
		delete g_Library;
		g_Library = NULL;
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

CFramWnd LDLIB_API *  API_LibraryUI()
{
	return new CErasureMainWnd();
}

IErasureLibrary::IErasureLibrary()
{
	m_MainWnd = NULL;
}

IErasureLibrary::~IErasureLibrary()
{
	if(m_MainWnd)
		delete m_MainWnd;
}

CFramWnd * IErasureLibrary::LibraryUI()
{
	if (!m_MainWnd)
		m_MainWnd = new CErasureMainWnd();
	return nullptr;
}
