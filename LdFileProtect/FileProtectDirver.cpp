#include "StdAfx.h"
#include "FileProtectDirver.h"


CFileProtectDirver::CFileProtectDirver(void)
{
	m_hDirver = CreateFile(L"\\\\.\\LdFileProtect", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
}


CFileProtectDirver::~CFileProtectDirver(void)
{
	if(m_hDirver != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDirver);
}

BOOL CFileProtectDirver::ProtectFile(LPCTSTR FileName, DWORD Flag)
{
	return TRUE;
}
