#include "stdafx.h"
#include "NtfsUtils.h"
#include "ntfs.h"

namespace LeadowLib {

	DWORD CNtfsUtils::GetBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData)
	{
		HANDLE hVolume = Volume->OpenVolumeHandle();

		if (hVolume == INVALID_HANDLE_VALUE)
			return GetLastError();

		/*PNTFS_BPB pBpb = (PNTFS_BPB)new UCHAR[512];
		DWORD CB;
		if (!::ReadFile(hVolume, pBpb, 512, &CB, nullptr))
			return false;

		pBpbData->BytesPerSector = pBpb->BytesPerSector;
		pBpbData->BytesPerFileRecordSegment = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord < 0x80
			? ((PNTFS_BPB)pBpb)->ClustersPerFileRecord * ((PNTFS_BPB)pBpb)->SectorsPerCluster
			* ((PNTFS_BPB)pBpb)->BytesPerSector : 1 << (0x100 - ((PNTFS_BPB)pBpb)->ClustersPerFileRecord);

		pBpbData->SectorsPerCluster = ((PNTFS_BPB)pBpb)->SectorsPerCluster;
		pBpbData->ClustersPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord;
		pBpbData->BytesPerClusters = pBpbData->BytesPerSector * pBpbData->SectorsPerCluster;
		pBpbData->MftStartLcn = pBpb->MftStartLcn;
		pBpbData->TotalSectors = pBpb->TotalSectors;

		delete[] pBpb;*/

		DWORD dwCb, Result = 0;
		NTFS_VOLUME_DATA_BUFFER data = { 0 };
		if (!DeviceIoControl(hVolume, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &data, sizeof(NTFS_VOLUME_DATA_BUFFER), &dwCb, NULL))
		{
			Result = GetLastError();
		}

		pBpbData->BytesPerSector = data.BytesPerSector;
		pBpbData->SectorsPerCluster = data.BytesPerCluster / data.BytesPerSector;
		pBpbData->BytesPerClusters = data.BytesPerCluster;
		pBpbData->BytesPerFileRecordSegment = data.BytesPerFileRecordSegment;
		pBpbData->ClustersPerFileRecord = data.ClustersPerFileRecordSegment;
		pBpbData->MftValidDataLength = data.MftValidDataLength.QuadPart;
		pBpbData->MftStartLcn = data.MftStartLcn.QuadPart;
		pBpbData->TotalSectors = data.NumberSectors.QuadPart;

		CloseHandle(hVolume);

		return Result;
	}
}