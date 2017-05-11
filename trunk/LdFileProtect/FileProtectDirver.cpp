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
	dwSize = BuildDeviceData(lpFileName, dwFlag, &pFile);
	return CallDriver(IOCTL_FS_ADD_FILE, pFile, dwSize, &dwRetId, sizeof(dwRetId), &dwCb);
}

void CFileProtectDirver::SetDriverName()
{
	m_DriverName = LD_FPDRV_NAME;
}

DWORD CFileProtectDirver::BuildDeviceData(LPCTSTR lpFileName, DWORD dwFlag, PFS_FILE_RECORD* pOutFile)
{
	PFS_FILE_RECORD pFile = NULL;
	LPTSTR path, name;
	DWORD datalen;
	PVOID pData;
	TCHAR Driver[10] = { 0 }, path[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 };

	if (!CFileUtils::ExtractFileDrive(lpFileName, Driver))
		return 0;
	if (!CFileUtils::ExtractFilePath(lpFileName, path))
		return 0;
	if (!CFileUtils::ExtractFileName(lpFileName, name))
		return 0;

	Driver = Win32Path2DevicePath(Driver);

	cb = SizeOf(FS_FILE_RECORD) + (Length(Driver) + Length(path) + Length(name) + 3) * SizeOf(wchar) + datalen;
	pFile = GetMemory(cb);
	ZeroMemory(pFile, cb);
	pFile.id = FId;
	pFile.cb = cb;
	pFile.pt = FFsType;

	pFile.nVolume  = SizeOf(FS_FILE_RECORD) + datalen;
	CopyMemory((PByte(pFile) + pFile.nVolume), PChar(Driver), Length(Driver)* SizeOf(char));
	pFile.nPath = pFile.nVolume + (Length(Driver) + 1)* SizeOf(char);
	CopyMemory((PByte(pFile) + pFile.nPath), PChar(path), Length(path)* SizeOf(char));
	pFile.nName = pFile.nPath + (Length(path) + 1)* SizeOf(char);
	CopyMemory((PByte(pFile) + pFile.nName), PChar(name), Length(name)* SizeOf(char));

	if pData <> nil then
		FreeMemory(pData);

	return pFile;
}
