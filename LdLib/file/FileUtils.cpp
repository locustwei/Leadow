#include "stdafx.h"
#include "FileUtils.h"
#include "../classes/ldstring.h"
#include "../PublicRoutimes.h"

namespace LeadowLib {

#define DOS_L_NAME TEXT("\\\\?\\")

	BOOL CFileUtils::ExtractFileDrive(TCHAR* lpFullName, CLdString& lpDriveName)
	{
		size_t len;
		if (!lpFullName || !lpDriveName)
			return FALSE;
		len = _tcslen(lpFullName);
		if (len >= 2 && lpFullName[1] == ':')
		{
			lpDriveName.Assign(lpFullName, 2);
			//_tcsncpy(lpDriveName, lpFullName, 2);
			return TRUE;
		}
		else
			return FALSE;
	}

	UINT CFileUtils::ExtractFilePath(TCHAR* lpFullName, CLdString& lpFilePath)
	{
		if (!lpFullName)
			return 0;

		TCHAR* s = _tcsrchr(lpFullName, '\\');
		if (s == NULL)
			return 0;
		UINT_PTR len = s - lpFullName;
		lpFilePath.Assign(lpFullName, len);

		return (UINT)len;
	}

	UINT CFileUtils::ExtractFileName(TCHAR* lpFullName, CLdString& lpName)
	{
		//_tsplitpath(lpDir, lpDrive, NULL, lpFile, lpExt);
		if (!lpFullName)
			return 0;

		TCHAR* s = _tcsrchr(lpFullName, '\\');
		if (s == NULL)
			return 0;
		s += 1;
		lpName = s;
		return (UINT)_tcslen(s);
	}

	UINT CFileUtils::ExtractFileExt(TCHAR* lpFullName, CLdString& lpName)
	{
		if (!lpFullName)
			return 0;

		TCHAR* s = _tcsrchr(lpFullName, '.');
		if (s == NULL)
			return 0;
		s += 1;
		lpName = s;
		return (UINT)_tcslen(s);;
	}

	UINT CFileUtils::Win32Path2DevicePath(TCHAR* lpFullName, CLdString& lpDevicePath)
	{
		CLdString Device;
		if (!ExtractFileDrive(lpFullName, Device))
			return 0;
		TCHAR DevicePath[MAX_PATH] = { 0 };
		UINT ret = QueryDosDevice(Device, DevicePath, MAX_PATH);
		if (ret)
		{
			lpDevicePath = DevicePath;
			lpDevicePath += (lpFullName + Device.GetLength());
		}
		return ret;
	}

	UINT CFileUtils::DevicePathToWin32Path(TCHAR* lpDevicePath, CLdString& lpDosPath)
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
					lpDosPath = szDrive;
					lpDosPath += (lpDevicePath + _tcslen(szDevice));
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
			FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS);
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

		HANDLE handle = CreateFile(lpFullName, GENERIC_READ | GENERIC_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS);
		if (handle == INVALID_HANDLE_VALUE)
			return Result;
		Result = DeviceIoControl(handle, FSCTL_SET_COMPRESSION, &compressionStatus, sizeof(DWORD), NULL, 0, &bytesReturned, NULL);
		CloseHandle(handle);
		return Result;
	}

	DWORD CFileUtils::EnumFiles(TCHAR* lpFullPath, TCHAR* lpFilter, CLdMethodDelegate callback, UINT_PTR Param)
	{
		DWORD result = 0;
		CLdString path = lpFullPath;
		if (path.IsEmpty())
			return 0;

		if (path[path.GetLength() - 1] != '\\')
		{
			path += '\\';
		}
		path += lpFilter;

		WIN32_FIND_DATA Data = { 0 };
		HANDLE hFind = FindFirstFile(path, &Data);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			if (path.Find(DOS_L_NAME) != 0)
				path.Insert(0, DOS_L_NAME);
			hFind = FindFirstFile(path, &Data);
			if (hFind == INVALID_HANDLE_VALUE)
				return GetLastError();
		}
		do
		{
			if ((Data.cFileName[0] == '.' && Data.cFileName[1] == '\0') ||
				(Data.cFileName[0] == '.' && Data.cFileName[1] == '.' && Data.cFileName[3] == '\0'))
				continue;

			if (!callback(&Data, Param))
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
		//if (result != 0)
			//DebugOutput(L"delete error %d", result);
		return result;
	}

	void CFileUtils::FormatFileSize(INT64 nSize, CLdString& result)
	{
#define __KB 1024
#define __MB (1024 * __KB)
#define __GB (1024 * __MB)
#define __TB 0x10000000000 

		if (nSize < __KB)
			result.Format(_T("%dBytes"), nSize);
		else if (nSize < __MB)
			result.Format(_T("%.2fKB"), double(nSize) / double(__KB));
		else if (nSize < __GB)
			result.Format(_T("%.2fMB"), double(nSize) / double(__MB));
		else if (nSize < __TB)
			result.Format(_T("%.2fGB"), double(nSize) / double(__GB));
		else
			result.Format(_T("%.2fTB"), double(nSize) / double(__TB));
	}

	void CFileUtils::GenerateRandomFileName(int length, CLdString* Out)
	{
		TCHAR validFileNameChars[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ _+=-()[]{}',`~!");
		Out->SetLength(length + 1);
		//	TCHAR* result = new TCHAR[length + 1];
		//	ZeroMemory(result, (length + 1) * sizeof(TCHAR));
		srand((UINT)(UINT_PTR)Out);
		for (int j = 0; j < length; j++)
		{
			Out->GetData()[j] = validFileNameChars[rand() % 78];
		}
	}

	DWORD CFileUtils::GetFileADSNames(TCHAR* lpFileName, CLdArray<PFILE_ADS_INFO>* result)
	{
		HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS);
		if (hFile == INVALID_HANDLE_VALUE)
			return GetLastError();
		
		PUCHAR p = new UCHAR[1024*10]; //GetFileInformationByHandleEx 支持Vista
		ZeroMemory(p, 1024 * 10);
		NTSTATUS status = NtQueryInformationFile(hFile, p, 1024 * 10, FileStreamInformation);
		if(NT_SUCCESS(status))
		{
			PFILE_STREAM_INFO information = (PFILE_STREAM_INFO)p;
			do
			{
				PFILE_ADS_INFO adsInfo = (PFILE_ADS_INFO)new UCHAR[sizeof(FILE_ADS_INFO) + information->StreamNameLength + sizeof(TCHAR)];
				ZeroMemory(adsInfo, sizeof(FILE_ADS_INFO) + information->StreamNameLength + sizeof(TCHAR));
				adsInfo->StreamSize = information->StreamSize;
				adsInfo->StreamAllocationSize = information->StreamAllocationSize;

				CopyMemory(adsInfo->StreamName, information->StreamName, information->StreamNameLength);
				
				TCHAR* pType = _tcsrchr(adsInfo->StreamName, ':');
				if(!pType || _tcscmp(pType, _T(":$DATA")) != 0) //非交换数据流
				{
					delete adsInfo;
				}else
				{
					pType[0] = '\0'; //把:$DATA去掉
					if (_tcslen(adsInfo->StreamName) == 1) //::$DATA 这是默认的数据流，不要
						delete adsInfo;
					else
						result->Add(adsInfo);
				}

				information = (PFILE_STREAM_INFO)((PUCHAR)information + information->NextEntryOffset);
			} while (information->NextEntryOffset);
		}

		CloseHandle(hFile);

		delete p;

		return (DWORD)status;
	}

	DWORD CFileUtils::RenameFile(TCHAR* lpFrom, TCHAR* lpTo)
	{
		CLdString from, to;
		TCHAR* p = _tcsrchr(lpTo, '\\');
		if(p == nullptr)  //没有包含路径
		{
			p = _tcsrchr(lpFrom, '\\');
			if (!p)
				return (DWORD)-1;

			CFileUtils::ExtractFilePath(lpFrom, to);
			to += '\\';
			to += lpTo;
		}
		from.Assign(lpFrom);
		from.Insert(0, DOS_L_NAME);
		to.Insert(0, DOS_L_NAME);
		if(!::MoveFile(from, to))
			return GetLastError();

		return 0;
	}

	HANDLE CFileUtils::CreateFile(TCHAR* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
	{
		if (lpFileName == nullptr)
			return INVALID_HANDLE_VALUE;

		HANDLE hFile = ::CreateFile(lpFileName, dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, dwFlagsAndAttributes, nullptr);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			if(_tcslen(lpFileName)>=MAX_PATH)
			{
				CLdString s = DOS_L_NAME;
				s += lpFileName;
				hFile = ::CreateFile(s.GetData(), dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, dwFlagsAndAttributes, nullptr);
			}
		}
		return hFile;
	}

	BOOL CFileUtils::DeleteFile(TCHAR* lpFileName)
	{
		BOOL result = ::DeleteFile(lpFileName);
		if(!result && (GetLastError() == ERROR_PATH_NOT_FOUND || GetLastError()== ERROR_FILE_NOT_FOUND))
		{
			CLdString file = DOS_L_NAME;
			file += lpFileName;
			result = ::DeleteFile(file);
		}
		return result;
	}

	BOOL CFileUtils::RemoveDirectory(TCHAR* lpPathName)
	{
		BOOL result = ::RemoveDirectory(lpPathName);
		if (!result && (GetLastError() == ERROR_PATH_NOT_FOUND))
		{
			CLdString file = DOS_L_NAME;
			file += lpPathName;
			result = ::RemoveDirectory(file);
		}
		return result;
	}
}
