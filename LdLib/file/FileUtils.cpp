#include "stdafx.h"
#include "FileUtils.h"
#include "../classes/ldstring.h"
#include "../LdLib.h"

namespace LeadowLib {
	BOOL CFileUtils::ExtractFileDrive(TCHAR* lpFullName, TCHAR* lpDriveName)
	{
		size_t len;
		if (!lpFullName || !lpDriveName)
			return FALSE;
		len = _tcslen(lpFullName);
		if (len >= 2 && lpFullName[1] == ':')
		{
			_tcsncpy(lpDriveName, lpFullName, 2);
			return TRUE;
		}
		else
			return FALSE;
	}

	UINT CFileUtils::ExtractFilePath(TCHAR* lpFullName, TCHAR* lpFilePath)
	{
		if (!lpFullName)
			return 0;

		TCHAR* s = _tcsrchr(lpFullName, '\\');
		if (s == NULL)
			return 0;
		UINT_PTR len = s - lpFullName;
		if (lpFilePath)
			_tcsncpy(lpFilePath, lpFullName, len);
		return (UINT)len;
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
		return (UINT)_tcslen(s);
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
		return (UINT)_tcslen(s);;
	}

	UINT CFileUtils::Win32Path2DevicePath(TCHAR* lpFullName, TCHAR* lpDevicePath)
	{
		TCHAR Device[10] = { 0 };
		if (!ExtractFileDrive(lpFullName, Device))
			return 0;
		TCHAR DevicePath[MAX_PATH] = { 0 };
		UINT ret = QueryDosDevice(Device, DevicePath, MAX_PATH);
		if (ret && lpDevicePath)
		{
			wcscat(lpDevicePath, DevicePath);
			wcscat(lpDevicePath, lpFullName + _tcslen(Device));
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
				if (_wcsnicmp(szDevice, lpDevicePath, _tcslen(szDevice)) == 0)
				{
					if (lpDosPath)
					{
						wcscat(lpDosPath, szDrive);
						wcscat(lpDosPath, lpDevicePath + _tcslen(szDevice));
					}
					break;
				}
			}
			while (*lpDrives++);
		} while (*lpDrives);

		return (UINT)_tcslen(lpDosPath);
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

		if (path[path.GetLength() - 1] != '\\')
		{
			path += '\\';
		}

		WIN32_FIND_DATA Data = { 0 };
		HANDLE hFind = FindFirstFile(path + lpFilter, &Data);
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
		fo.fFlags = (FILEOP_FLAGS)dwFlag;
		fo.wFunc = FO_DELETE;
		int result = SHFileOperation(&fo);
		if (result != 0)
			DebugOutput(L"delete error %d", result);
		return result;
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
		double Size = (double)nSize;

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

	void CFileUtils::GenerateRandomFileName(int length, CLdString* Out)
	{
		TCHAR validFileNameChars[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ _+=-()[]{}',`~!");
		Out->SetLength(length + 1);
		//	TCHAR* result = new TCHAR[length + 1];
		//	ZeroMemory(result, (length + 1) * sizeof(TCHAR));
		for (int j = 0; j < length; j++)
		{
			Out->GetData()[j] = validFileNameChars[rand() % 78];
		}
	}

	DWORD CFileUtils::GetFileADSNames(TCHAR* lpFileName, CLdArray<TCHAR*>* result)
	{
		HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return GetLastError();
		
		PUCHAR p = new UCHAR[1024*10]; //GetFileInformationByHandleEx 支持Vista
		NTSTATUS status = NtQueryInformationFile(hFile, p, 10240, FileStreamInformation);
		if(NT_SUCCESS(status))
		{
			PFILE_STREAM_INFO information = (PFILE_STREAM_INFO)p;
			while (information)
			{
				TCHAR* name = (TCHAR*)new UCHAR[information->StreamNameLength + sizeof(TCHAR)];
				ZeroMemory(name, information->StreamNameLength + sizeof(TCHAR));
				CopyMemory(name, information->StreamName, information->StreamNameLength);
				
				TCHAR* pType = _tcsrchr(name, ':');
				if(!pType || _tcscmp(pType, _T(":$DATA")) != 0) //非交换数据流
				{
					delete name;
				}else
				{
					pType[0] = '\0';
					if (_tcslen(name) == 1) //::$DATA 这是默认的数据流，不要
						delete name;
					else
						result->Add(name);
				}

				if (information->NextEntryOffset == 0)
					information = nullptr;
				else
					information = (PFILE_STREAM_INFO)((PUCHAR)information + information->NextEntryOffset);
			}
		}

		CloseHandle(hFile);

		delete p;

		return (DWORD)status;
	}

	DWORD CFileUtils::RenameFile(TCHAR* lpFrom, TCHAR* lpTo)
	{
		CLdString s = lpTo;
		TCHAR* p = _tcsrchr(lpTo, '\\');
		if(p == nullptr)  //没有包含路径
		{
			p = _tcsrchr(lpFrom, '\\');
			if (!p)
				return (DWORD)-1;
			p[1] = '\0';
			s = lpFrom;
			s += lpTo;
		}

		if (::MoveFile(lpFrom, s))
			return 0;
		else
			return GetLastError();
	}
}
