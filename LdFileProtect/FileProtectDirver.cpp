#include "StdAfx.h"
#include "FileProtectDirver.h"
#include "FPDControlCode.h"
#include <Shlwapi.h>
#include "FileUtils.h"

CFileProtectDirver::CFileProtectDirver(void)
{
	
}


CFileProtectDirver::~CFileProtectDirver(void)
{
}

BOOL CFileProtectDirver::ProtectFile(LPCTSTR lpFileName, DWORD dwFlag)
{
	if (lpFileName == NULL || (dwFlag | FS_ALL) == 0)
		return FALSE;

	DWORD dwSize = 0, dwRetId = 0, dwCb;
	PFS_FILE_RECORD pFile;
	dwSize = BuildDeviceData((LPTSTR)lpFileName, dwFlag, &pFile);
	return CallDriver(IOCTL_FS_ADD_FILE, pFile, dwSize, &dwRetId, sizeof(dwRetId), &dwCb);
}

void CFileProtectDirver::SetDriverName()
{
	m_DriverName = LD_FPDRV_NAME;
}

DWORD CFileProtectDirver::BuildDeviceData(LPTSTR lpFileName, DWORD dwFlag, PFS_FILE_RECORD* pOutFile)
{
	PFS_FILE_RECORD pFile = NULL;
	DWORD datalen;
	PVOID pData;
	TCHAR Driver[100] = { 0 }, path[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 };

	if (!CFileUtils::ExtractFilePath(lpFileName, path))
		return 0;
	if (!CFileUtils::ExtractFileName(lpFileName, name))
		return 0;

	if (CFileUtils::Win32Path2DevicePath(lpFileName, Driver))
		return 0;

	DWORD cb = sizeof(FS_FILE_RECORD) + (_tcslen(Driver) + _tcslen(path) + _tcslen(name) + 3) * sizeof(TCHAR) + datalen;
	pFile = (PFS_FILE_RECORD)malloc(cb);
	ZeroMemory(pFile, cb);
	pFile->cb = cb;
	pFile->pt = dwFlag;

	pFile->nVolume  = sizeof(FS_FILE_RECORD) + datalen;
	memcpy((PUCHAR(pFile) + pFile->nVolume), Driver, _tcslen(Driver)* sizeof(TCHAR));
	pFile->nPath = pFile->nVolume + (_tcslen(Driver) + 1) * sizeof(TCHAR);
	memcpy((PUCHAR(pFile) + pFile->nPath), path, _tcslen(path) * sizeof(TCHAR));
	pFile->nName = pFile->nPath + (_tcslen(path) + 1) * sizeof(TCHAR);
	memcpy((PUCHAR(pFile) + pFile->nName), name, _tcslen(name) * sizeof(TCHAR));

	*pOutFile = pFile;
	return cb;
}
