#include "stdafx.h"
#include "Library.h"

typedef DWORD(*Library_Init)(PAuthorization);
typedef CFramWnd *  (*LibraryUI)(VOID);

CLdLibray* CLdLibray::m_Erasure = NULL;
CLdLibray* CLdLibray::m_Protect = NULL;

CLdLibray::CLdLibray()
{
	m_hModule = NULL;
	m_MainUI = NULL;
}

CLdLibray::~CLdLibray()
{
}

CLdLibray * CLdLibray::ErasureLibrary()
{
	if (m_Erasure == NULL)
	{
		m_Erasure = new CErasureLib();
		if (!m_Erasure->InitLib(_T("LdFileEraser_d64.dll")))
		{
			delete m_Erasure;
			m_Erasure = NULL;
		}
	}
	_ASSERTE(m_Erasure);
	return m_Erasure;
}

CLdLibray * CLdLibray::ProtectLibrary()
{
	if (m_Protect == NULL)
	{
		m_Protect = new CProtectLib();
		if (!m_Protect->InitLib(_T("LdFileProtect_d64.dll")))
		{
			delete m_Protect;
			m_Protect = NULL;
		}
	}
	_ASSERTE(m_Protect);
	return m_Protect;
}

CFramWnd * CLdLibray::GetUI()
{
	if (m_MainUI == NULL)
	{
		LibraryUI fnLibrayUI = (LibraryUI)GetProcAddress(m_hModule, "API_LibraryUI");
		if (fnLibrayUI)
			m_MainUI = fnLibrayUI();
	}
	_ASSERTE(m_MainUI);
	return m_MainUI;
}

BOOL CLdLibray::InitLib(TCHAR * pLibFile)
{
	m_hModule = LoadLibrary(pLibFile);
	if (m_hModule == NULL)
		return FALSE;
	Library_Init fnInit = (Library_Init)GetProcAddress(m_hModule, "API_Init");
	if (fnInit == NULL)
		return FALSE;
	return fnInit(NULL) == 0;

}
