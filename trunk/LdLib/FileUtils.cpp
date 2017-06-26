#include "stdafx.h"
#include "FileUtils.h"

#pragma region CFileUtils

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

UINT CFileUtils::ExtractFileExt(LPTSTR lpFullName, LPTSTR lpName)
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

UINT CFileUtils::Win32Path2DevicePath(LPTSTR lpFullName, LPTSTR lpDevicePath)
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

	return wcslen(lpDosPath);
}

DWORD CFileUtils::ForceDirectories(LPTSTR lpFullPath)
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

BOOL CFileUtils::IsDirectoryExists(LPTSTR lpFullPath)
{
	DWORD dwAttr = GetFileAttributes(lpFullPath);
	return (dwAttr != INVALID_FILE_ATTRIBUTES) && (FILE_ATTRIBUTE_DIRECTORY & dwAttr);
}

DWORD CFileUtils::GetCompressStatus(LPTSTR lpFullName, PWORD pStatus)
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

BOOL CFileUtils::SetCompression(LPTSTR lpFullName, BOOL bCompress)
{
	BOOL Result = FALSE;
	DWORD compressionStatus = bCompress ? COMPRESSION_FORMAT_DEFAULT : COMPRESSION_FORMAT_NONE;
	DWORD bytesReturned = 0;

	HANDLE handle = CreateFile(lpFullName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return FALSE;
	Result = DeviceIoControl(handle, FSCTL_SET_COMPRESSION, &compressionStatus, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);
	CloseHandle(handle);
	return Result;
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region CFileInfo


CFileInfo::CFileInfo():
	Name()
{
	ClearValue();
}

CFileInfo::CFileInfo(TCHAR * pFileName):
	Name()
{
	ClearValue();
	Name = pFileName;
}

CFileInfo::~CFileInfo()
{
}

LARGE_INTEGER CFileInfo::GetCreateTime()
{
	GetFileBasicInfo();
	return m_Baseinfo.CreationTime;
}

LARGE_INTEGER CFileInfo::GetChangeTime()
{
	GetFileBasicInfo();
	return m_Baseinfo.ChangeTime;
}

LARGE_INTEGER CFileInfo::GetLastWriteTime()
{
	GetFileBasicInfo();
	return m_Baseinfo.LastWriteTime;
}

LARGE_INTEGER CFileInfo::GetLastAccessTime()
{
	GetFileBasicInfo();
	return m_Baseinfo.LastAccessTime;
}

INT64 CFileInfo::GetAllocateSize()
{
	GetFileStandardInfo();
	return m_StandrardInfo.AllocationSize.QuadPart;
}

INT64 CFileInfo::GetDataSize()
{
	GetFileStandardInfo();
	return m_StandrardInfo.EndOfFile.QuadPart;
}

DWORD CFileInfo::GetAttributes()
{
	GetFileBasicInfo();
	return m_Baseinfo.FileAttributes;
}

CLdString & CFileInfo::GetFileName()
{
	return Name;
}

VOID CFileInfo::SetFileName(TCHAR * pFileName)
{
	ClearValue();
	Name = pFileName;
}

CLdString CFileInfo::FormatFileSize(INT64 nSize)
{
#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)
#define TB (1024 * 1024 * 1024 * 1024)

	double Size = nSize;
	CLdString s((UINT)100);
	if (Size < KB)
		s.Format(_T("%.2fBytes"), Size);
	else if (Size < MB)
		s.Format(_T("%.2fKB"), Size / KB);
	else if (Size < GB)
		s.Format(_T("%.2fMB"), Size / MB);
	else if (Size < TB)
		s.Format(_T("%.2fGB"), Size / GB);
	else
		s.Format(_T("%.2fTB"), Size / TB);
	return s;
}

void CFileInfo::ClearValue()
{
	ZeroMemory(&m_Baseinfo, sizeof(FILE_BASIC_INFO));
	ZeroMemory(&m_StandrardInfo, sizeof(FILE_STANDARD_INFO));
	m_StandrardInfo.EndOfFile.QuadPart = -1;
	Name.Empty();
}

void CFileInfo::GetFileBasicInfo()
{
	if (m_Baseinfo.CreationTime.QuadPart == 0)
	{
		if (!Name.IsEmpty())
		{
			HANDLE hFile = CreateFile(Name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				GetFileInformationByHandleEx(hFile, FileBasicInfo, &m_Baseinfo, sizeof(FILE_BASIC_INFO));
				CloseHandle(hFile);
			}
		}
	}
}

void CFileInfo::GetFileStandardInfo()
{
	if (m_StandrardInfo.EndOfFile.QuadPart == -1)
	{
		if (!Name.IsEmpty())
		{
			HANDLE hFile = CreateFile(Name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				GetFileInformationByHandleEx(hFile, FileStandardInfo, &m_StandrardInfo, sizeof(FILE_STANDARD_INFO));
				CloseHandle(hFile);
			}
		}
	}
}

#pragma endregion