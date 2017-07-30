#include "stdafx.h"
#include "NtfsUtils.h"

DWORD CNtfsUtils::GetNtfsVolumeData(CVolumeInfo* Volume, PNTFS_VOLUME_DATA_BUFFER pVolumeData)
{
	HANDLE hVolume = Volume->OpenVolumeHandle();

	if (hVolume == INVALID_HANDLE_VALUE)
		return GetLastError();
	
	DWORD dwCb, Result = 0;

	if(!DeviceIoControl(hVolume, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, pVolumeData, sizeof(NTFS_VOLUME_DATA_BUFFER), &dwCb, NULL))
	{
		Result = GetLastError();
	}

	CloseHandle(hVolume);
	return Result;
}
