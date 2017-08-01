#include "stdafx.h"
#include "NtfsUtils.h"
#include "ntfs.h"

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

DWORD CNtfsUtils::GetNtfsBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData)
{
	HANDLE hVolume = Volume->OpenVolumeHandle();

	if (hVolume == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD CB;
	PNTFS_BPB pBpb = (PNTFS_BPB)new UCHAR[Volume->GetSectorSize()];
	if (!::ReadFile(hVolume, pBpb, Volume->GetSectorSize(), &CB, nullptr))
	{
		delete[](PUCHAR)pBpb;
		return GetLastError();
	}

	pBpbData->BytesPerSector = pBpb->BytesPerSector;
	pBpbData->MediaType = pBpb->MediaType;
	pBpbData->TotalSectors = pBpb->TotalSectors;
	pBpbData->BytesPerFileRecord = pBpb->ClustersPerFileRecord < 0x80
		? pBpb->ClustersPerFileRecord * pBpb->SectorsPerCluster
		* pBpb->BytesPerSector : 1 << (0x100 - pBpb->ClustersPerFileRecord);

	pBpbData->SectorsPerCluster = pBpb->SectorsPerCluster;
	pBpbData->ClustersPerFileRecord = pBpb->ClustersPerFileRecord;

	pBpbData->BytesPerClusters = pBpbData->BytesPerSector * pBpbData->SectorsPerCluster;

	delete[] (PUCHAR)pBpb;

	return 0;
}
