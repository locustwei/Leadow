#include "stdafx.h"
#include "WJVolume.h"
#include "..\MFT\NtfsUSN.h"
#include "..\MFT\NtfsMtfReader.h"
#include "..\MFT\FatMftReader.h"

CWJVolume::CWJVolume(const TCHAR* volume)
	: m_hVolume(INVALID_HANDLE_VALUE)
	, m_VolumeGuid()
	, m_FileSystem(FILESYSTEM_TYPE_UNKNOWN)
	//, m_Reader(nullptr)
	//, m_Data(nullptr)
{
	if (volume && _tcslen(volume) > 0)
	{
		if (volume[0] == '\\')
		{
			m_VolumeGuid = volume;
			if (m_VolumeGuid[m_VolumeGuid.GetLength() - 1] != '\\')
				m_VolumeGuid += '\\';
		}
		else
		{
			m_VolumePath = volume;
			if (m_VolumePath[m_VolumePath.GetLength() - 1] != '\\')
				m_VolumePath += '\\';
		}
	}
}


CWJVolume::~CWJVolume()
{
	CloseHandle();
}

HANDLE  CWJVolume::OpenHandle()
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
	{
		CLdString path;
		if (!m_VolumeGuid.IsEmpty())
		{
			path = m_VolumeGuid;
			path.SetAt(path.GetLength() - 1, '\0');
		}
		else if (!m_VolumePath.IsEmpty())
		{
			path = L"\\\\.\\";
			path += m_VolumePath;
			path.SetAt(path.GetLength() - 1, '\0');
		}
		else
			return INVALID_HANDLE_VALUE;
		
		if (!path.IsEmpty())
			m_hVolume = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	}
	return m_hVolume;
}

const TCHAR*  CWJVolume::GetVolumeGuid()
{
	if(m_VolumeGuid.IsEmpty() && !m_VolumePath.IsEmpty())
	{
		CLdString path = m_VolumePath;
		if (path[path.GetLength() - 1] != '\\')
			path += '\\';
		WCHAR guid[MAX_PATH + 1] = { 0 };
		DWORD cb = MAX_PATH;
		if (GetVolumeNameForVolumeMountPoint(path, guid, cb))
			m_VolumeGuid = guid;
	}
	if (m_VolumeGuid.IsEmpty())
		return nullptr;
	else
	{
		/*TCHAR* p = _tcschr(m_VolumeGuid.GetData(), '\\');
		if (p)
			return p++;
		else
			return nullptr;*/
		return  _tcschr(m_VolumeGuid.GetData(), '{');
	}
	//return m_VolumeGuid;
}

const TCHAR*  CWJVolume::GetVolumePath()
{
	if (m_VolumePath.IsEmpty() && !m_VolumeGuid.IsEmpty())
	{
		WCHAR Names[MAX_PATH + 1] = { 0 };
		DWORD cb;
		if (GetVolumePathNamesForVolumeName(m_VolumeGuid, Names, MAX_PATH, &cb))
			m_VolumePath = Names;
	}
	return m_VolumePath;
}

BOOL IsWindowsVersionOrGreater_(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

BOOL GetVolumeInformationByHandle(
	HANDLE hFile,
	LPWSTR lpVolumeNameBuffer,
	DWORD nVolumeNameSize,
	LPDWORD lpVolumeSerialNumber,
	LPDWORD lpMaximumComponentLength,
	LPDWORD lpFileSystemFlags,
	LPWSTR lpFileSystemNameBuffer,
	DWORD nFileSystemNameSize
) 
{
	if (!IsWindowsVersionOrGreater_(6, 0, 0))
		return FALSE;
	typedef BOOL(WINAPI *GetByHandle)(__in      HANDLE hFile,
		__out_ecount_opt(nVolumeNameSize) LPWSTR lpVolumeNameBuffer,
		__in      DWORD nVolumeNameSize,
		__out_opt LPDWORD lpVolumeSerialNumber,
		__out_opt LPDWORD lpMaximumComponentLength,
		__out_opt LPDWORD lpFileSystemFlags,
		__out_ecount_opt(nFileSystemNameSize) LPWSTR lpFileSystemNameBuffer,
		__in      DWORD nFileSystemNameSize);

	static GetByHandle fn = (GetByHandle)GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetVolumeInformationByHandleW");
	if (fn)
		return fn(hFile, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
	else
		return FALSE;
}

VOID CWJVolume::GetVolumeInfo()
{
	GetVolumePath();

	DWORD MaxFilenameLength;
	TCHAR FileSystemNameBuffer[MAX_PATH] = { 0 };
	TCHAR VolumeName[MAX_PATH] = { 0 };

	if (!m_VolumePath.IsEmpty())
	{
		if (!GetVolumeInformation(m_VolumePath, VolumeName, MAX_PATH, &m_VolumeSerialNumber, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
			return;
	}
	else if (!GetVolumeInformationByHandle(OpenHandle(), VolumeName, MAX_PATH, &m_VolumeSerialNumber, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
		return;

	if (_tcscmp(FileSystemNameBuffer, _T("NTFS")) == 0)
		m_FileSystem = FILESYSTEM_TYPE_NTFS;
	else if (_tcscmp(FileSystemNameBuffer, _T("FAT32")) == 0)
		m_FileSystem = FILESYSTEM_TYPE_FAT;
	else if (_tcscmp(FileSystemNameBuffer, _T("FAT")) == 0)
		m_FileSystem = FILESYSTEM_TYPE_FAT;
	else if (_tcscmp(FileSystemNameBuffer, _T("exFAT")) == 0)
		m_FileSystem = FILESYSTEM_TYPE_EXFAT;
	else if (_tcscmp(FileSystemNameBuffer, _T("UDF")) == 0)
		m_FileSystem = FILESYSTEM_TYPE_UDF;
	m_VolumeName = VolumeName;
}

VOID  CWJVolume::CloseHandle()
{
	if(m_hVolume != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hVolume);
	m_hVolume = INVALID_HANDLE_VALUE;
}

WJFILESYSTEM_TYPE  CWJVolume::GetFileSystem()
{
	if (m_FileSystem == FILESYSTEM_TYPE_UNKNOWN)
	{
		GetVolumeInfo();
	}
	return m_FileSystem;
}

MEDIA_TYPE  CWJVolume::GetMediaType()
{
	return F3_120M_512;
}

UINT64  CWJVolume::GetTotalSize()
{
	return 0;
}

USN CWJVolume::GetLastUsn(HANDLE hVolume)
{
	if (hVolume == INVALID_HANDLE_VALUE/* || GetFileSystem() != FILESYSTEM_TYPE_NTFS*/)
		return 0;

	USN_JOURNAL_DATA ujd = { 0 };

	if (CNtfsUSN::QueryUsnStatus(hVolume, &ujd)) {
		return ujd.NextUsn;
	}
	else {
		CNtfsUSN::CreateUsnJournal(hVolume);
		if (CNtfsUSN::QueryUsnStatus(hVolume, &ujd))
			return ujd.NextUsn;
	}
	
	return 0;
}

const TCHAR*  CWJVolume::GetVolumeLabel()
{
	if (m_VolumeName.IsEmpty())
		GetVolumeInfo();
	return m_VolumeName;
}

const TCHAR* CWJVolume::GetShlDisplayName()
{
	if (m_VolumePath.IsEmpty())
		GetVolumePath();
	if (m_DisplayName.IsEmpty())
	{
		SHFILEINFOW info = { 0 };
		SHGetFileInfo(m_VolumePath, 0, &info, sizeof(info), SHGFI_DISPLAYNAME);
		m_DisplayName = info.szDisplayName;
	}
	return m_DisplayName;
}
