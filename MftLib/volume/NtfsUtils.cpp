#include "stdafx.h"
#include "NtfsUtils.h"

namespace LeadowDisk {

	DWORD CNtfsUtils::GetBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData)
	{
		HANDLE hVolume = Volume->OpenVolumeHandle();

		if (hVolume == INVALID_HANDLE_VALUE)
			return GetLastError();

		DWORD dwCb, Result = 0;
		NTFS_VOLUME_DATA_BUFFER data = { 0 };
		if (!DeviceIoControl(hVolume, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &data, sizeof(NTFS_VOLUME_DATA_BUFFER), &dwCb, NULL))
		{
			Result = GetLastError();
		}
		else
		{
			pBpbData->BytesPerSector = data.BytesPerSector;
			pBpbData->SectorsPerCluster = data.BytesPerCluster / data.BytesPerSector;
			pBpbData->BytesPerClusters = data.BytesPerCluster;
			pBpbData->BytesPerFileRecordSegment = data.BytesPerFileRecordSegment;
			pBpbData->ClustersPerFileRecord = data.ClustersPerFileRecordSegment;
			pBpbData->MftValidDataLength = data.MftValidDataLength.QuadPart;
			pBpbData->MftStartLcn = data.MftStartLcn.QuadPart;
			pBpbData->TotalSectors = data.NumberSectors.QuadPart;
		}
		CloseHandle(hVolume);

		return Result;
	}
}