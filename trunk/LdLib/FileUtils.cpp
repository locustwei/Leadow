#include "stdafx.h"
#include "FileUtils.h"
#include "ldstring.h"

BOOL CFileUtils::ExtractFileDrive(TCHAR* lpFullName, TCHAR* lpDriveName)
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

UINT CFileUtils::ExtractFilePath(TCHAR* lpFullName, TCHAR* lpFilePath)
{
	if (!lpFullName)
		return FALSE;

	TCHAR* s = _tcsrchr(lpFullName, '\\');
	if (s == NULL)
		return 0;
	UINT_PTR len = s - lpFullName;
	if(lpFilePath)
		wcsncpy(lpFilePath, lpFullName, len);
	return len;
}

UINT CFileUtils::ExtractFileName(TCHAR* lpFullName, TCHAR* lpName)
{
	//_tsplitpath(lpDir, lpDrive, NULL, lpFile, lpExt);
	if (!lpFullName)
		return 0;

	TCHAR* s = _tcsrchr(lpFullName, '\\');
	if (s == NULL)
		return 0;
	s += 1;
	if (lpName)
		_tcscpy(lpName, s);
	return wcslen(s);
}

UINT CFileUtils::ExtractFileExt(TCHAR* lpFullName, TCHAR* lpName)
{
	if (!lpFullName)
		return 0;

	TCHAR* s = _tcsrchr(lpFullName, '.');
	if (s == NULL)
		return 0;
	s += 1;
	if (lpName)
		wcscat(lpName, s);
	return wcslen(s);;
}

UINT CFileUtils::Win32Path2DevicePath(TCHAR* lpFullName, TCHAR* lpDevicePath)
{
	TCHAR Device[10] = {0};
	if (!ExtractFileDrive(lpFullName, Device))
		return 0;
	TCHAR DevicePath[MAX_PATH] = { 0 };
	UINT ret = QueryDosDevice(Device, DevicePath, MAX_PATH);
	if (ret && lpDevicePath)
	{
		wcscat(lpDevicePath, DevicePath);
		wcscat(lpDevicePath, lpFullName + wcslen(Device));
	}
	return ret;
}

UINT CFileUtils::DevicePathToWin32Path(TCHAR* lpDevicePath, TCHAR* lpDosPath)
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

	return wcslen(lpDosPath);
}

DWORD CFileUtils::ForceDirectories(TCHAR* lpFullPath)
{
	if (!lpFullPath || _tcslen(lpFullPath) < 2)
		return ERROR_NO_VOLUME_LABEL;
	if (IsDirectoryExists(lpFullPath))
		return 0;
	else
	{
		if (CreateDirectory(lpFullPath, NULL))
			return 0;
		else
			return GetLastError();
	}
}

BOOL CFileUtils::IsDirectoryExists(TCHAR* lpFullPath)
{
	DWORD dwAttr = GetFileAttributes(lpFullPath);
	return (dwAttr != INVALID_FILE_ATTRIBUTES) && (FILE_ATTRIBUTE_DIRECTORY & dwAttr);
}

DWORD CFileUtils::GetCompressStatus(TCHAR* lpFullName, PWORD pStatus)
{
	DWORD Result = 0;
	DWORD bytesReturned = 0;

	HANDLE handle = CreateFile(lpFullName, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return GetLastError();

	if (!DeviceIoControl(handle, FSCTL_GET_COMPRESSION, NULL, 0, pStatus, sizeof(WORD), &bytesReturned, NULL))
		Result = GetLastError();
	CloseHandle(handle);
	return Result;
}

BOOL CFileUtils::SetCompression(TCHAR* lpFullName, BOOL bCompress)
{
	BOOL Result = FALSE;
	DWORD compressionStatus = bCompress ? COMPRESSION_FORMAT_DEFAULT : COMPRESSION_FORMAT_NONE;
	DWORD bytesReturned = 0;

	HANDLE handle = CreateFile(lpFullName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return Result;
	Result = DeviceIoControl(handle, FSCTL_SET_COMPRESSION, &compressionStatus, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);
	CloseHandle(handle);
	return Result;
}

DWORD CFileUtils::FindFile(TCHAR* lpFullPath, TCHAR* lpFilter, IGernalCallback<LPWIN32_FIND_DATA>* callback, UINT_PTR Param)
{
	DWORD result = 0;
	CLdString path = lpFullPath;
	if (path.IsEmpty())
		return 0;

	if(path[path.GetLength()-1]!='\\')
	{
		path += '\\';
	}

	WIN32_FIND_DATA Data = { 0 };
	HANDLE hFind = FindFirstFile(path+lpFilter, &Data);
	if (hFind == INVALID_HANDLE_VALUE)
		return GetLastError();
	do
	{
		if ((Data.cFileName[0] == '.' && Data.cFileName[1] == '\0') ||
			(Data.cFileName[0] == '.' && Data.cFileName[1] == '.' && Data.cFileName[3] == '\0'))
			continue;

		if (!callback->GernalCallback_Callback(&Data, Param))
			break;

	} while (FindNextFile(hFind, &Data));

	FindClose(hFind);
	return result;
}

int CFileUtils::ShDeleteFile(TCHAR * lpFileName, DWORD dwFlag)
{
	SHFILEOPSTRUCT fo;
	ZeroMemory(&fo, sizeof(SHFILEOPSTRUCT));
	TCHAR name[MAX_PATH] = { 0 };  //double-null terminated
	_tcscpy(name, lpFileName);
	fo.pFrom = name;
	fo.fFlags = dwFlag;
	fo.wFunc = FO_DELETE;
	return SHFileOperation(&fo);
}

DWORD CFileUtils::DeleteFile(TCHAR * lpFileName)
{
	SetFileAttributes(lpFileName, FILE_ATTRIBUTE_NORMAL);
	if (!::DeleteFile(lpFileName))
		return GetLastError();

/*
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING UnicodeString;
	RtlInitUnicodeString(&UnicodeString, lpFileName);
	InitializeObjectAttributes(&ObjectAttributes, &UnicodeString, 0x40, 0, nullptr, nullptr);
*/

	return 0;
}

void CFileUtils::FormatFileSize(INT64 nSize, CLdString& result)
{
#define __KB 1024
#define __MB (1024 * __KB)
#define __GB (1024 * __MB)
#define __TB 0x10000000000 
	double Size = nSize;

	if (Size < __KB)
		result.Format(_T("%.2fBytes"), Size);
	else if (Size < __MB)
		result.Format(_T("%.2fKB"), Size / __KB);
	else if (Size < __GB)
		result.Format(_T("%.2fMB"), Size / __MB);
	else if (Size < __TB)
		result.Format(_T("%.2fGB"), Size / __GB);
	else
		result.Format(_T("%.2fTB"), Size / __TB);
}
