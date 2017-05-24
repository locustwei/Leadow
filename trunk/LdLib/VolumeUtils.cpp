#include "stdafx.h"
#include "VolumeUtils.h"
#include <Shlwapi.h>

CVolumeInfo::CVolumeInfo(void) :m_VolumeGuid(), m_VolumePath(), m_FileSystemName()
{
	m_hVolume = INVALID_HANDLE_VALUE;
}

CVolumeInfo::~CVolumeInfo(void)
{
	if(m_hVolume != INVALID_HANDLE_VALUE)
		CloseHandle(m_hVolume);
}

DWORD CVolumeInfo::OpenVolume(TCHAR* pGuid)
{
	if (m_hVolume != INVALID_HANDLE_VALUE || pGuid == NULL)
		return ERROR_ALIAS_EXISTS;
	
	WCHAR Names[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(Names); 
	if (GetVolumePathNamesForVolumeName(pGuid, Names, cb, &cb))
	{
		if (wcslen(Names)>0)
			Names[wcslen(Names) - 1] = '\0';
		m_VolumePath = Names;
	}
	else
		return GetLastError();

	pGuid[wcslen(pGuid) - 1] = '\0';

	DWORD Result = OpenVolumeHandle(pGuid);
	if(Result == 0)
		m_VolumeGuid = pGuid;

	return Result;
}

DWORD CVolumeInfo::OpenVolumePath(const TCHAR* pPath)
{
	if(m_hVolume != INVALID_HANDLE_VALUE || pPath == NULL)
		return ERROR_ALIAS_EXISTS;

	CLdString path = pPath;
	if(pPath[wcslen(pPath)-1] != '\\')
		path += '\\';
	WCHAR guid[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(guid); 
	if (!GetVolumeNameForVolumeMountPoint(path, guid, cb))
		return GetLastError();
	if(wcslen(guid)>0)
		guid[wcslen(guid) - 1] = '\0';

	path.Delete(path.GetLength() - 1, 1);
	
	m_VolumePath = path;

	path.Insert(0, L"\\\\.\\");
	DWORD Result = OpenVolumeHandle(path);
	if(Result == 0)
		m_VolumeGuid = guid;

	return Result;
}

DWORD CVolumeInfo::OpenVolumeHandle(PCWSTR wsz_path)
{
	m_hVolume = CreateFile(wsz_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return GetLastError();
	else
		return 0;
}

BOOL CVolumeInfo::GetVolumeInfo()
{
	if (m_VolumeGuid.IsEmpty())
		return FALSE;
	CLdString VolumeNameBuffer((UINT)MAX_PATH), FileSystemNameBuffer((UINT)MAX_PATH);
	if (!GetVolumeInformation(m_VolumeGuid, VolumeNameBuffer, MAX_PATH, NULL, &MaxFilenameLength, &FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
		return FALSE;
	else
	{
		if (FileSystemNameBuffer == _T("FAT"))
		{
			if (GetTotalClusters() <= 0x0FF)
				FileSystemNameBuffer += _T("12");
			else
				FileSystemNameBuffer += _T("16");
		}
		m_FileSystemName = FileSystemNameBuffer;
	}
	return TRUE;
}

PCWSTR CVolumeInfo::GetVolumeGuid()
{
	return m_VolumeGuid;
}

PCWSTR CVolumeInfo::GetVolumePath()
{
	return m_VolumePath;
}

UINT64 CVolumeInfo::GetAvailableFreeSpace()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER FreeBytesAvailable;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, &FreeBytesAvailable, NULL, NULL))
		return FreeBytesAvailable.QuadPart;
	else
		return 0;
}

UINT64 CVolumeInfo::GetTotalFreeSpace()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER TotalFreeSpace;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, NULL, &TotalFreeSpace))
		return TotalFreeSpace.QuadPart;
	else
		return 0;
}

UINT64 CVolumeInfo::GetTotalSize()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER TotalSize;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, &TotalSize, NULL))
		return TotalSize.QuadPart;
	else
	{
		//IOCTL_DISK_GET_LENGTH_INFO
	}
	return 0;
}

UINT CVolumeInfo::GetClusterSize()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		return SectorsPerCluster * BytesPerSector;
	else
		return 0;
}

UINT CVolumeInfo::GetSectorSize()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		return BytesPerSector;
	else
		return 0;
}

UINT CVolumeInfo::GetTotalClusters()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
	if (GetDiskFreeSpace(m_VolumeGuid, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
		return TotalNumberOfClusters;
	else
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

