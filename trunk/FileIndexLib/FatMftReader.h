#pragma once

#include "MftReader.h"
#include "Fat.h"

class CFatMftReader: public CMftReader
{
public:
	virtual ULONGLONG EnumFiles(PVOID Param);
public:
	CFatMftReader(void);
	~CFatMftReader(void);
protected:
	virtual bool Init();
	void ZeroMember() override;

	virtual const PFILE_INFO GetFileInfo(ULONGLONG ReferenceNumber);
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

	LONGLONG EnumDirectoryFiles(PFAT_FILE pParentDir, PVOID Param);
	ULONG DataClusterStartSector(ULONG ClusterNumber);
	ULONG GetNextClusterNumber(ULONG ClusterNumber);
	PUCHAR ReadFat(ULONG sector, ULONG count, BOOL cache);
	BOOL DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, PVOID Param);
};

