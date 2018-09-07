#include "stdafx.h"
#include "VolumeUtils.h"

namespace LeadowDisk {
	DWORD CVolumeUtils::MountedVolumes(CLdMethodDelegate callback, UINT_PTR Param)
	{
		DWORD			dwSize = GetLogicalDriveStrings(0, NULL);
		dwSize += 2;
		TCHAR*			pszDrives = new TCHAR[dwSize];
		ZeroMemory(pszDrives, (dwSize) * sizeof(TCHAR));

		if (!GetLogicalDriveStrings((dwSize) * sizeof(TCHAR), (LPTSTR)pszDrives))
			return GetLastError();
		TCHAR* pstr = pszDrives;
		while (TCHAR('\0') != *pstr)
		{
			if (!callback(pstr, Param))
				break;
			pstr += _tcslen(pstr) + 1;
		}

		delete pszDrives;

		return 0;
	}


	DWORD CVolumeUtils::EnumVolumeNames(CLdMethodDelegate callback, UINT_PTR Param)
	{
		TCHAR Volume[MAX_PATH] = { 0 };
		ULONG CharCount = MAX_PATH;

		HANDLE hFind = FindFirstVolume(Volume, ARRAYSIZE(Volume));
		if (hFind == INVALID_HANDLE_VALUE)
			return GetLastError();

		while (TRUE) {

			size_t Index = _tcslen(Volume) - 1;
			if (Index < 0)
				Index = 0;

			if (Volume[0] != L'\\' ||
				Volume[1] != L'\\' ||
				Volume[2] != L'?' ||
				Volume[3] != L'\\' ||
				Volume[Index] != L'\\') {

				break;
			}

			if (!callback(Volume, Param))
				break;

			ZeroMemory(Volume, ARRAYSIZE(Volume) * sizeof(TCHAR));

			if (!FindNextVolume(hFind, Volume, ARRAYSIZE(Volume) * sizeof(TCHAR))) {
				break;
			}
		}

		FindVolumeClose(hFind);
		return 0;
	}

	VOLUME_FILE_SYSTEM CVolumeUtils::GetVolumeFileSystem(TCHAR * szPath)
	{
		DWORD MaxFilenameLength;
		CLdString FileSystemNameBuffer((UINT)MAX_PATH);
		if (GetVolumeInformation(szPath, nullptr, 0, nullptr, &MaxFilenameLength, nullptr, FileSystemNameBuffer, MAX_PATH))
		{
			if (FileSystemNameBuffer == _T("NTFS"))
				return FILESYSTEM_TYPE_NTFS;
			else if (FileSystemNameBuffer == _T("FAT32"))
				return FILESYSTEM_TYPE_FAT;
			else if (FileSystemNameBuffer == _T("exFAT"))
				return FILESYSTEM_TYPE_EXFAT;
			else if (FileSystemNameBuffer == _T("UDF"))
				return FILESYSTEM_TYPE_UDF;
			else if (FileSystemNameBuffer == _T("FAT"))
			{
				return FILESYSTEM_TYPE_FAT;
			}
		}
		return FILESYSTEM_TYPE_UNKNOWN;
	}
}