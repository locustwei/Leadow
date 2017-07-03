#include "stdafx.h"
#include "VolumeUtils.h"
#include <Shlwapi.h>

CVolumeInfo::CVolumeInfo(void) :
	m_VolumeGuid(), 
	m_VolumePath(), 
	m_FileSystemName(), 
	m_FileSystemFlags(FS_UNKNOW)
{
//	m_hVolume = INVALID_HANDLE_VALUE;
}

CVolumeInfo::~CVolumeInfo(void)
{
//	if(m_hVolume != INVALID_HANDLE_VALUE)
	//	CloseHandle(m_hVolume);
}

DWORD CVolumeInfo::OpenVolume(TCHAR* pGuid)
{
	WCHAR Names[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(Names); 
	if (GetVolumePathNamesForVolumeName(pGuid, Names, cb, &cb))
	{
		if (wcslen(Names)>0)
			Names[wcslen(Names) - 1] = '\0';
		m_VolumePath = Names;
	}

	pGuid[wcslen(pGuid) - 1] = '\0';
	
	m_VolumeGuid = pGuid;

	return 0;
}

DWORD CVolumeInfo::OpenVolumePath(const TCHAR* pPath)
{
	CLdString path = pPath;
	if(pPath[wcslen(pPath)-1] != '\\')
		path += '\\';
	WCHAR guid[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(guid); 
	if (GetVolumeNameForVolumeMountPoint(path, guid, cb))
	{
		if (wcslen(guid) > 0)
			guid[wcslen(guid) - 1] = '\0';
		m_VolumeGuid = guid;
	}

	path.Delete(path.GetLength() - 1, 1);
	
	m_VolumePath = path;

	return 0;
}

HANDLE CVolumeInfo::OpenVolumeHandle() const
{
	CLdString path;
	if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid;
	else if (!m_VolumePath.IsEmpty())
	{
		path = _T("\\\\.\\");
		path += m_VolumePath;
	}

	return CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
}

DWORD CVolumeInfo::GetVolumeInfo()
{
	CLdString path;
	if (!m_VolumePath.IsEmpty())
		path = m_VolumePath + _T("\\");
	else if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid + _T("\\");
	else
		return DWORD(-1);
	CLdString VolumeNameBuffer((UINT)MAX_PATH), FileSystemNameBuffer((UINT)MAX_PATH);

	DWORD MaxFilenameLength;
	if (!GetVolumeInformation(path, VolumeNameBuffer, MAX_PATH, NULL, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
		return GetLastError();
	else
	{
		if (FileSystemNameBuffer == _T("NTFS"))
			m_FileSystemName = FS_NTFS;
		else if (FileSystemNameBuffer == _T("FAT32"))
			m_FileSystemName = FS_FAT32;
		else if (FileSystemNameBuffer == _T("FAT"))
		{
			UINT totlCluters = 0;
			GetTotalClusters(&totlCluters);
			if (totlCluters <= 0x0FF)
				m_FileSystemName = FS_FAT12;
			else
				m_FileSystemName = FS_FAT16;
		}
	}
	return 0;
}

DWORD CVolumeUtils::MountedVolumes(IGernalCallback<TCHAR*>* callback, UINT_PTR Param)
{
	DWORD			dwSize = GetLogicalDriveStrings(0, NULL);
	dwSize += 2;
	TCHAR*			pszDrives = new TCHAR[dwSize];
	ZeroMemory(pszDrives, (dwSize) * sizeof(TCHAR));

	if(!GetLogicalDriveStrings((dwSize) * sizeof(TCHAR), (LPTSTR)pszDrives))
		return GetLastError();
	TCHAR* pstr = pszDrives;
	while (TCHAR('\0') != *pstr)
	{
		if (!callback->GernalCallback_Callback(pstr, Param))
			break;
		pstr += _tcslen(pstr) + 1;
	}
	return 0;
}

PCWSTR CVolumeInfo::GetVolumeGuid()
{
	return m_VolumeGuid;
}

PCWSTR CVolumeInfo::GetVolumePath()
{
	return m_VolumePath;
}

 DWORD CVolumeInfo::GetAvailableFreeSpace(PUINT64 pOut)
{
	CLdString path;
	if (!m_VolumePath.IsEmpty())
		path = m_VolumePath + _T("\\");
	else if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid + _T("\\");
	else
		return (DWORD)-1;

	ULARGE_INTEGER FreeBytesAvailable;
	if (GetDiskFreeSpaceEx(path, &FreeBytesAvailable, NULL, NULL))
		*pOut = FreeBytesAvailable.QuadPart;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetTotalFreeSpace(PUINT64 pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return (DWORD)-1;
	ULARGE_INTEGER TotalFreeSpace;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, NULL, &TotalFreeSpace))
		*pOut = TotalFreeSpace.QuadPart;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetTotalSize(PUINT64 pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return DWORD(-1);
	ULARGE_INTEGER TotalSize;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, &TotalSize, NULL))
		*pOut = TotalSize.QuadPart;
	else
	{
		return GetLastError();
		//IOCTL_DISK_GET_LENGTH_INFO
	}
	return 0;
}

DWORD CVolumeInfo::GetClusterSize(PUINT pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return DWORD(-1);
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		*pOut = SectorsPerCluster * BytesPerSector;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetSectorSize(PUINT pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return DWORD(-1);
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		*pOut = BytesPerSector;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetTotalClusters(PUINT pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return DWORD(-1);
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		*pOut = TotalNumberOfClusters;
	else
		return GetLastError();
	return 0;
}

BOOL CVolumeInfo::HasQuota()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER FreeBytesAvailable, lpTotalNumberOfFreeBytes;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, &FreeBytesAvailable, NULL, &lpTotalNumberOfFreeBytes))
		return lpTotalNumberOfFreeBytes.QuadPart > FreeBytesAvailable.QuadPart;
	else
		return 0;
}

BOOL CVolumeInfo::IsMounted()
{
	return !m_VolumePath.IsEmpty();
}


DWORD CVolumeInfo::GetFileSystem(VOLUME_FILE_SYSTEM* pOut)
{
	DWORD Result = 0;
	if (m_FileSystemName == FS_UNKNOW)
	{
		Result = GetVolumeInfo();
	}
	if (Result == 0)
		*pOut = m_FileSystemName;
	return Result;
}

DWORD CVolumeUtils::EnumVolumeNames(IGernalCallback<TCHAR*>* callback, UINT_PTR Param)
{
	WCHAR Volume[MAX_PATH] = { 0 };
	ULONG CharCount = MAX_PATH;

	HANDLE hFind = FindFirstVolume(Volume, ARRAYSIZE(Volume));
	if (hFind == INVALID_HANDLE_VALUE)
		return GetLastError();

	while (TRUE) {

		size_t Index = wcslen(Volume) - 1;
		if (Index < 0)
			Index = 0;

		if (Volume[0] != L'\\' ||
			Volume[1] != L'\\' ||
			Volume[2] != L'?' ||
			Volume[3] != L'\\' ||
			Volume[Index] != L'\\') {

			break;
		}

		if(!callback->GernalCallback_Callback(Volume, Param))
			break;

		ZeroMemory(Volume, ARRAYSIZE(Volume)*sizeof(TCHAR));

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
			return FS_NTFS;
		else if (FileSystemNameBuffer == _T("FAT32"))
			return FS_FAT32;
		else if (FileSystemNameBuffer == _T("FAT"))
		{
			return FS_FAT16;
		}
	}
	return FS_UNKNOW;
}
