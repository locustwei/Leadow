#include "stdafx.h"
#include "Library.h"

typedef PVOID(*Library_Init)(PAuthorization);

#pragma region CLdLibray

CLdLibray::CLdLibray()
{
	m_hModule = NULL;
}

CLdLibray::~CLdLibray()
{
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

#pragma endregion CLdLibray

CErasureLib::CErasureLib()
{
	m_Library = InitLib(_T("LdFileEraser_d64.dll"));
}
