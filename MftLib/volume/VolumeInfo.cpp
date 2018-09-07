#include "stdafx.h"
#include "VolumeInfo.h"
#include "NtfsUtils.h"
#include "FatUtils.h"
#include "..\..\LdLib\PublicRoutimes.h"

namespace LeadowDisk {
	CVolumeInfo::CVolumeInfo(TCHAR* volume) :
		m_MftData(nullptr),
		m_FileSystemFlags(0),
		m_hVolume(INVALID_HANDLE_VALUE),
		m_FileSystem(FILESYSTEM_TYPE_UNKNOWN),
		m_VolumeSerialNumber(0)
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

	CVolumeInfo::~CVolumeInfo(void)
	{
		if (m_MftData)
			delete m_MftData;
		if(m_hVolume!=INVALID_HANDLE_VALUE)
			this->CloseHandle();

	}
	
	HANDLE CVolumeInfo::OpenVolumeHandle(PDWORD pErrorCode)
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
			{
				m_hVolume = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, 0);
				if (m_hVolume == INVALID_HANDLE_VALUE && pErrorCode)
					*pErrorCode = GetLastError();
			}
		}
		return m_hVolume;
	}

	VOID LeadowDisk::CVolumeInfo::CloseHandle()
	{
		if (m_hVolume != INVALID_HANDLE_VALUE)
			::CloseHandle(m_hVolume);
		m_hVolume = INVALID_HANDLE_VALUE;
	}

	DWORD CVolumeInfo::GetVolumeInfo()
	{
		GetVolumePath();

		DWORD MaxFilenameLength;
		TCHAR FileSystemNameBuffer[MAX_PATH] = { 0 };
		TCHAR VolumeName[MAX_PATH] = { 0 };

		if (!m_VolumePath.IsEmpty())
		{
			if (!GetVolumeInformation(m_VolumePath, VolumeName, MAX_PATH, &m_VolumeSerialNumber, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
				return GetLastError();
		}
		else if (!GetVolumeInformationByHandle_(OpenVolumeHandle(), VolumeName, MAX_PATH, &m_VolumeSerialNumber, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
			return GetLastError();

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
		m_VolumeLabel = VolumeName;

		return 0;
	}

	PVOLUME_BPB_DATA CVolumeInfo::RefreshBpbData()
	{
		if (m_MftData)
		{
			delete m_MftData;
			m_MftData = nullptr;
		}
		GetBpbData(nullptr);
		return m_MftData;
	}

	PVOLUME_BPB_DATA CVolumeInfo::GetBpbData(PDWORD pErrorCode)
	{
		DWORD result = 0;
		if (!m_MftData)
		{
			if (m_FileSystem == FILESYSTEM_TYPE_UNKNOWN)
			{
				result = GetVolumeInfo();
			}

			VOLUME_BPB_DATA tmp = { 0 };
			switch (m_FileSystem)
			{
			case FILESYSTEM_TYPE_UNKNOWN:
				result = -1;
				break;
			case FILESYSTEM_TYPE_NTFS:
				result = CNtfsUtils::GetBpbData(this, &tmp);
				if (result == 0)
				{
					m_MftData = new VOLUME_BPB_DATA;
					*m_MftData = tmp;
				}
				break;
			case FILESYSTEM_TYPE_FAT:
				result = CFatUtils::GetBpbData(this, &tmp);
				if (result == 0)
				{
					m_MftData = new VOLUME_BPB_DATA;
					*m_MftData = tmp;
				}
				break;
			case FILESYSTEM_TYPE_EXFAT: 
				break;
			default:
				result = LD_ERROR_DISK_UNKONW_FS;
				break;
			}
		}
		if (pErrorCode)
			*pErrorCode = result;
		return m_MftData;
	}

	const TCHAR* CVolumeInfo::GetVolumeGuid()
	{
		return m_VolumeGuid;
	}

	UINT64 CVolumeInfo::GetAvailableFreeSpace(PDWORD pErrorCode)
	{
		GetVolumePath();

		ULARGE_INTEGER FreeBytesAvailable;
		if (::GetDiskFreeSpaceEx(m_VolumePath, &FreeBytesAvailable, NULL, NULL))
		{
			if (pErrorCode)
				*pErrorCode = 0;
			return FreeBytesAvailable.QuadPart;
		}
		else if (pErrorCode)
		{
			*pErrorCode = GetLastError();
			return 0;
		}
	}

	UINT64 CVolumeInfo::GetTotalFreeSpace(PDWORD pErrorCode)
	{
		if (m_VolumeGuid.IsEmpty())
		{
			if (pErrorCode)
				*pErrorCode = -1;
			return 0;
		}
		ULARGE_INTEGER TotalFreeSpace;

		if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, NULL, &TotalFreeSpace))
		{
			if (pErrorCode)
				*pErrorCode = 0;
			return TotalFreeSpace.QuadPart;
		}
		else if(pErrorCode)
			*pErrorCode = GetLastError();
		return 0;
	}

	UINT64 CVolumeInfo::GetTotalSize(PDWORD pErrorCode)
	{
		if (m_VolumeGuid.IsEmpty())
		{
			if (pErrorCode)
				*pErrorCode = -1;
			return 0;
		}
		ULARGE_INTEGER TotalSize;
		if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, &TotalSize, NULL))
		{
			if (pErrorCode)
				*pErrorCode = 0;
			return TotalSize.QuadPart;
		}
		else if (pErrorCode)
			*pErrorCode = GetLastError();
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


	VOLUME_FILE_SYSTEM CVolumeInfo::GetFileSystem(PDWORD pErrorCode)
	{
		DWORD result = 0;
		if (m_FileSystem == FILESYSTEM_TYPE_UNKNOWN)
		{
			result = GetVolumeInfo();
		}

		if (pErrorCode)
			*pErrorCode = result;

		return m_FileSystem;
	}
}

const TCHAR* LeadowDisk::CVolumeInfo::GetVolumePath()
{
	if (m_VolumePath.IsEmpty() && !m_VolumeGuid.IsEmpty())
	{
		WCHAR Names[MAX_PATH + 1] = { 0 };
		DWORD cb;
		if (GetVolumePathNamesForVolumeName(m_VolumeGuid, Names, MAX_PATH, &cb))
			m_VolumePath = Names;
	}
	if (m_VolumePath.IsEmpty())
		return nullptr;
	else
		return m_VolumePath;
}
