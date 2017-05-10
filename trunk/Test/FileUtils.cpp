#include "stdafx.h"
#include "FileUtils.h"

BOOL CFileUtils::ExtractFileDrive(LPTSTR lpFileName, LPTSTR lpDriveName)
{
	int len;
	if (!lpFileName || !lpDriveName)
		return FALSE;
	len = wcslen(lpFileName);
	if (len >= 2 && lpFileName[1] == ':')
	{
		wcsncpy(lpDriveName, lpFileName, 2);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileUtils::ExtractFilePath(LPTSTR lpFileName, LPTSTR lpFilePath)
{
	if (!lpFileName || !lpFilePath)
		return FALSE;

	TCHAR* s = _tcsrchr(lpFileName, '\\');
	if (s == NULL)
		return FALSE;

	wcsncpy(lpFilePath, lpFileName, s - lpFileName);
	return TRUE;
}
