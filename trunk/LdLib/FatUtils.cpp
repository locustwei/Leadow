#include "stdafx.h"
#include "FatUtils.h"
#include "fat.h"

DWORD CFatUtils::GetFatBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData)
{
	HANDLE hVolume = Volume->OpenVolumeHandle();

	if (hVolume == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD CB;
	PFAT_BPB pBpb = (PFAT_BPB)new UCHAR[Volume->GetSectorSize()];
	if (!::ReadFile(hVolume, pBpb, Volume->GetSectorSize(), &CB, nullptr))
	{
		delete[](PUCHAR)pBpb;
		return GetLastError();
	}

	pBpbData->SectorsPerRootDirectory = (pBpb->RootEntriesCount * 0x20 + pBpb->BytesPerSector - 1) / pBpb->BytesPerSector; //FAT16ÓÐÐ§
	pBpbData->SectorsPerFAT = pBpb->SectorsPerFAT16 ? pBpb->SectorsPerFAT16 : pBpb->FAT32.SectorsPerFAT32;
	pBpbData->FirstDataSector = pBpb->ReservedSectors + pBpb->NumFATs * pBpbData->SectorsPerFAT + pBpbData->SectorsPerRootDirectory;
	pBpbData->FirstFatSector = pBpb->ReservedSectors;
	pBpbData->TotalSectors = pBpb->TotalSectors16 ? pBpb->TotalSectors16 : pBpb->TotalSectors32;
	pBpbData->RootClus = pBpb->FAT32.RootClus;

	delete[] (PUCHAR)pBpb;

	return 0;
}
