#include "StdAfx.h"
#include "FileProtectDirver.h"
#include "FPDControlCode.h"
#include <Shlwapi.h>

CFileProtectDirver::CFileProtectDirver(void)
{
	
}


CFileProtectDirver::~CFileProtectDirver(void)
{
}

BOOL CFileProtectDirver::ProtectFile(LPCTSTR lpFileName, DWORD dwFlag)
{
	PFS_FILE_RECORD pFile = BuildDeviceData(lpFileName, dwFlag);
	BOOL Result = CallDriver(IOCTL_FS_ADD_FILE, );
	return TRUE;
}

void CFileProtectDirver::SetDriverName()
{
	m_DriverName = LD_FPDRV_NAME;
}

PFS_FILE_RECORD CFileProtectDirver::BuildDeviceData(LPCTSTR lpFileName, DWORD dwFlag)
{
	PFS_FILE_RECORD pFile = NULL;
	LPTSTR path, name;
	DWORD datalen;
	PVOID pData;
	TCHAR Driver[MAX_PATH] = { 0 };

	if (!GetVolumePathName(lpFileName, Driver, MAX_PATH))
		return NULL;

	path = PathFindOnPath(lpFileName, NULL);

	name = ExtractFileName(FileName);
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
