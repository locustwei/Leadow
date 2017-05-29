#include "stdafx.h"
#include "VolumeUtils.h"
#include <Shlwapi.h>

CVolumeInfo::CVolumeInfo(void) :m_VolumeGuid(), m_VolumePath(), m_FileSystemName()
{
//	m_hVolume = INVALID_HANDLE_VALUE;
	m_FileSystemName = FS_UNKNOW;
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

HANDLE CVolumeInfo::OpenVolumeHandle()
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
	if (!GetVolumeInformation(path, VolumeNameBuffer, MAX_PATH, NULL, &MaxFilenameLength, &FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
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

void CVolumeInfo::MountedVolumes()
{
	//todo
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

