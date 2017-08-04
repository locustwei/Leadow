#pragma once

#include "MftReader.h"
#include <volume/fat.h>

class CFatMftReader: public CMftReader
{
public:
	CFatMftReader(void);
	~CFatMftReader(void);
	bool Init();
	UINT64 EnumFiles(PVOID Param) override;

protected:	
	void ZeroMember() override;

	//const PFILE_INFO GetFileInfo(UINT64 ReferenceNumber) override;
private:

	typedef struct FAT_CACHE{
		DWORD beginSector;
		DWORD sectorCount;
		BYTE* pFAT;
	};

	UCHAR m_EntrySize;
	FAT_CACHE m_fatCache;
	FAT_FILE m_Root;

	INT64 EnumDirectoryFiles(PFAT_FILE pParentDir, PVOID Param);
	DWORD DataClusterStartSector(DWORD ClusterNumber);
	DWORD GetNextClusterNumber(DWORD ClusterNumber);
	PUCHAR ReadFat(DWORD sector, DWORD count, BOOL cache);
	BOOL DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, PVOID Param);
};

