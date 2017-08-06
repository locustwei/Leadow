#include "stdafx.h"
#include "FatUtils.h"
#include "fat.h"

namespace LeadowLib {
	DWORD CFatUtils::GetBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData)
	{
		DWORD result = 0;
		HANDLE hVolume = Volume->OpenVolumeHandle();

		if (hVolume == INVALID_HANDLE_VALUE)
			return GetLastError();

		DWORD CB;
		PFAT_BPB pBpb = (PFAT_BPB)new UCHAR[512];
		if (!::ReadFile(hVolume, pBpb, 512, &CB, nullptr))
		{
			result = GetLastError();
		}
		else
		{
			pBpbData->BytesPerSector = pBpb->BytesPerSector;
			pBpbData->SectorsPerCluster = pBpb->SectorsPerCluster;
			pBpbData->TotalSectors = pBpb->SectorsPerFAT16 ? pBpb->TotalSectors16 : pBpb->TotalSectors32;
			pBpbData->MediaType = pBpb->Media;
			pBpbData->BytesPerClusters = pBpb->BytesPerSector * pBpb->SectorsPerCluster;
			pBpbData->SectorsPerRootDirectory = (pBpb->RootEntriesCount * 0x20 + pBpb->BytesPerSector - 1) / pBpb->BytesPerSector;
			pBpbData->SectorsPerFAT = pBpb->SectorsPerFAT16 ? pBpb->SectorsPerFAT16 : pBpb->FAT32.SectorsPerFAT32;;
			pBpbData->FirstDataSector = pBpb->ReservedSectors + pBpb->NumFATs * pBpbData->SectorsPerFAT + pBpbData->SectorsPerRootDirectory;;
			pBpbData->FirstFatSector = pBpb->ReservedSectors;;
			pBpbData->RootClus = pBpb->FAT32.RootClus;
		}
		CloseHandle(hVolume);

		delete[](PUCHAR)pBpb;

		return result;
	}
};