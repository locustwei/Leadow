#include "stdafx.h"
#include "FileUtils.h"

BOOL CFileUtils::ExtractFileDrive(LPTSTR lpFullName, LPTSTR lpDriveName)
{
	int len;
	if (!lpFullName || !lpDriveName)
		return FALSE;
	len = wcslen(lpFullName);
	if (len >= 2 && lpFullName[1] == ':')
	{
		wcsncpy(lpDriveName, lpFullName, 2);
		return TRUE;
	}
	else
		return FALSE;
}

UINT CFileUtils::ExtractFilePath(LPTSTR lpFullName, LPTSTR lpFilePath)
{
	if (!lpFullName)
		return FALSE;

	TCHAR* s = _tcsrchr(lpFullName, '\\');
	if (s == NULL)
		return 0;
	UINT len = s - lpFullName;
	if(lpFilePath)
		wcsncpy(lpFilePath, lpFullName, len);
	return len;
}

UINT CFileUtils::ExtractFileName(LPTSTR lpFullName, LPTSTR lpName)
{
	if (!lpFullName)
		return 0;

	TCHAR* s = _tcsrchr(lpFullName, '\\');
	if (s == NULL)
		return 0;
	s += 1;
	if (lpName)
		wcscat(lpName, s);
	return wcslen(s);
}

UINT CFileUtils::Win32Path2DevicePath(LPTSTR lpFullName, LPTSTR lpDevicePath)
{
	TCHAR Device[10] = {0};
	if (!ExtractFileDrive(lpFullName, Device))
		return 0;
	TCHAR DevicePath[MAX_PATH] = { 0 };
	UINT ret = QueryDosDevice(Device, DevicePath, MAX_PATH);
	if (ret && lpDevicePath)
		_tccpy(lpDevicePath, DevicePath);
	return ret;
}

UINT CFileUtils::DevicePathToWin32Path(LPTSTR lpDevicePath, LPTSTR lpDosPath)
{
	TCHAR szDrives[512];
	if (!GetLogicalDriveStrings(_countof(szDrives) - 1, szDrives)) {
		return 0;
	}

	TCHAR* lpDrives = szDrives;
	TCHAR szDevice[MAX_PATH];
	TCHAR szDrive[3] = _T(" :");
	do {
		*szDrive = *lpDrives;

		if (QueryDosDevice(szDrive, szDevice, MAX_PATH)) 
		{
			if (_wcsnicmp(szDevice, lpDevicePath, wcslen(szDevice)) == 0)
			{
				if (lpDosPath)
				{
					wcscat(lpDosPath, szDrive);
					wcscat(lpDosPath, lpDevicePath + wcslen(szDevice));
				}
				break;
			}
		}
		while (*lpDrives++);
	} while (*lpDrives);
	return 2;
}
