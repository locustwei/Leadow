#pragma once

#include "MftReader.h"
#include "Fat.h"

class CFatMftReader: public CMftReader
{
public:
	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;
public:
	CFatMftReader(HANDLE hVolume);
	~CFatMftReader(void) override;
protected:
	bool Init() override;
	void ZeroMember() override;

	virtual BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) override;
private:

	typedef struct FAT_CACHE{
		ULONG beginSector;
		ULONG sectorCount;
		BYTE* pFAT;
	};

	USHORT m_SectorsPerRootDirectory;
	ULONG m_SectorsPerFAT;
	ULONG m_FirstDataSector;
	ULONG m_FirstFatSector;
	ULONG m_TotalSectors;
	UCHAR m_EntrySize;
	FAT_CACHE m_fatCache;
	FAT_FILE m_Root;

	LONGLONG EnumDirectoryFiles(PFAT_FILE pParentDir);
	ULONG DataClusterStartSector(ULONG ClusterNumber);
	ULONG GetNextClusterNumber(ULONG ClusterNumber);
	PUCHAR ReadFat(ULONG sector, ULONG count, BOOL cache);
	BOOL DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir);

	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = false) override;

};

