#include "stdafx.h"
#include "Library.h"

typedef PVOID(*Library_Init)(PAuthorization);
typedef VOID(*Library_UnInit)();

#pragma region CLdLibray

CLdLibray::CLdLibray()
{
	m_hModule = NULL;
}

CLdLibray::~CLdLibray()
{
	UnInitLib();
	if (m_hModule)
		FreeLibrary(m_hModule);
}

PVOID CLdLibray::InitLib(TCHAR * pLibFile)
{
	m_hModule = LoadLibrary(pLibFile);
	if (m_hModule == NULL)
		return FALSE;
	Library_Init fnInit = (Library_Init)GetProcAddress(m_hModule, "API_Init");
	if (fnInit == NULL)
		return NULL;
	return fnInit(NULL);

}

VOID CLdLibray::UnInitLib()
{
	if (m_hModule)
	{
		Library_UnInit fnUnInit = (Library_UnInit)GetProcAddress(m_hModule, "API_UnInit");
		if (fnUnInit)
			fnUnInit();
	}
}

#pragma endregion CLdLibray

CErasureLib::CErasureLib()
{
	m_Library = (IErasureLibrary*)InitLib(_T("LdFileEraser_d64.dll"));
}

CErasureLib::~CErasureLib()
{
	
}

CFramWnd * CErasureLib::LibraryUI()
{
	if (!m_Library)
		return NULL;
	else
		return m_Library->LibraryUI();
}

CProtectLib::CProtectLib()
{
	m_Library = (IProtectLibrary*)InitLib(_T("LdFileProtect_d64.dll"));
}

CFramWnd * CProtectLib::LibraryUI()
{
	if (!m_Library)
		return NULL;
	else
		return m_Library->LibraryUI();
}
