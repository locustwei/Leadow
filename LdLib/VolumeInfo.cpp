#include "stdafx.h"
#include "VolumeInfo.h"
#include "NtfsUtils.h"
#include "FatUtils.h"

CVolumeInfo::CVolumeInfo(void) :
	m_VolumeGuid(),
	m_FileSystemName(),
	m_MftData(nullptr),
	m_FileSystemFlags(FS_UNKNOW)
{
	//	m_hVolume = INVALID_HANDLE_VALUE;
}

CVolumeInfo::~CVolumeInfo(void)
{
	if (m_MftData)
		delete m_MftData;
}

DWORD CVolumeInfo::OpenVolume(TCHAR* pGuid)
{
	WCHAR Names[MAX_PATH + 1] = { 0 };
	ULONG cb = sizeof(Names);
	if (GetVolumePathNamesForVolumeName(pGuid, Names, cb, &cb))
	{
		if (wcslen(Names)>0)
			Names[wcslen(Names) - 1] = '\0';
		m_FileName = Names;
	}

	pGuid[wcslen(pGuid) - 1] = '\0';

	m_VolumeGuid = pGuid;

	return 0;
}

bool CVolumeInfo::SetFileName(TCHAR* pPath)
{
	CLdString path = (TCHAR*)pPath;
	if (pPath[wcslen(pPath) - 1] != '\\')
		path += '\\';

	CFileInfo::SetFileName(path);

	WCHAR guid[MAX_PATH + 1] = { 0 };
	ULONG cb = sizeof(guid);
	if (GetVolumeNameForVolumeMountPoint(path, guid, cb))
	{
		if (wcslen(guid) > 0)
			guid[wcslen(guid) - 1] = '\0';
		m_VolumeGuid = guid;
	}

	path.Delete(path.GetLength() - 1, 1);

	return 0;
}

HANDLE CVolumeInfo::OpenVolumeHandle(PDWORD pErrorCode) const
{
	CLdString path;
	if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid;
	else if (!m_FileName.IsEmpty())
	{
		path = _T("\\\\.\\");
		path += m_FileName;
		if (path[path.GetLength() - 1] == '\\')
			path.SetAt(path.GetLength() - 1, '\0');
	}
	HANDLE result = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if (pErrorCode)
	{
		if (result == INVALID_HANDLE_VALUE)
			*pErrorCode = GetLastError();
		else
			*pErrorCode = 0;
	}
	return result;
}

DWORD CVolumeInfo::GetVolumeInfo()
{
	CLdString path;
	if (!m_FileName.IsEmpty())
		path = m_FileName;
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
//			UINT64 totlCluters = GetTotalClusters();
//			if (totlCluters <= 0x0FF)
//				m_FileSystemName = FS_FAT12;
//			else
				m_FileSystemName = FS_FAT16;
		}
	}
	return 0;
}

void CVolumeInfo::RefreshBpbData()
{
	if (m_MftData)
		delete m_MftData;
	GetVolumeMftData(nullptr);
}

PVOLUME_BPB_DATA CVolumeInfo::GetVolumeMftData(PDWORD pErrorCode)
{
	DWORD result = 0;
	if (!m_MftData)
	{
		if (m_FileSystemName == FS_UNKNOW)
		{
			result = GetVolumeInfo();
		}
		VOLUME_BPB_DATA tmp = { 0 };
		switch (m_FileSystemName)
		{
		case FS_UNKNOW:
			result = -1;
			break;
		case FS_NTFS:
			result = CNtfsUtils::GetBpbData(this, &tmp);
			if(result == 0)
			{
				m_MftData = new VOLUME_BPB_DATA;
				*m_MftData = tmp;
			}
			break;
		case FS_FAT32:
		case FS_FAT16:
		case FS_FAT12:
			result = CFatUtils::GetBpbData(this, &tmp);
			if (result == 0)
			{
				m_MftData = new VOLUME_BPB_DATA;
				*m_MftData = tmp;
			}
			break;
		case FS_EXFAT: break;
		default:
			result = -1;
			break;
		}
	}
	if (pErrorCode)
		*pErrorCode = result;
	return m_MftData;
}

TCHAR* CVolumeInfo::GetVolumeGuid()
{
	return m_VolumeGuid;
}

UINT64 CVolumeInfo::GetAvailableFreeSpace(PDWORD pErrorCode)
{
	CLdString path;
	if (!m_FileName.IsEmpty())
		path = m_FileName;
	else if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid + _T("\\");
	else
		return 0;

	ULARGE_INTEGER FreeBytesAvailable;
	if (GetDiskFreeSpaceEx(path, &FreeBytesAvailable, NULL, NULL))
		return FreeBytesAvailable.QuadPart;
	else
		return 0;
}

UINT64 CVolumeInfo::GetTotalFreeSpace(PDWORD pErrorCode)
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER TotalFreeSpace;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, NULL, &TotalFreeSpace))
		return TotalFreeSpace.QuadPart;
	else
		return 0;
}

UINT64 CVolumeInfo::GetTotalSize(PDWORD pErrorCode)
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER TotalSize;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, &TotalSize, NULL))
		return TotalSize.QuadPart;
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
	return !m_FileName.IsEmpty();
}


VOLUME_FILE_SYSTEM CVolumeInfo::GetFileSystem(PDWORD pErrorCode)
{
	DWORD result = 0;
	if (m_FileSystemName == FS_UNKNOW)
	{
		result = GetVolumeInfo();
	}

	if (pErrorCode)
		*pErrorCode = result;

	return m_FileSystemName;
}
