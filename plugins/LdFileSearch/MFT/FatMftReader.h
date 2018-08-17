#pragma once

#include "MftReader.h"
#include "Fat.h"

class CFatMftReader: public CMftReader
{
public:
	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;

	UINT64 EnumDeleteFiles(IMftDeleteReaderHandler*, PVOID) override;

public:
	CFatMftReader(HANDLE hVolume);
	~CFatMftReader(void) override;

	BOOL ReadCluster(UINT64 Cluster, UINT count, PVOID buffer) override;
protected:
	bool Init() override;
	void ZeroMember() override;

	virtual BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) override;
private:

	typedef struct FAT_CACHE{
		UINT beginSector;
		UINT sectorCount;
		BYTE* buffer;
	};
	FAT_CACHE m_fatCache;
	FAT_CACHE m_ClusterBitmap;

	USHORT m_SectorsPerRootDirectory;
	UINT m_SectorsPerFAT;
	UINT m_FirstDataSector;
	UINT m_FirstFatSector;
	UINT m_TotalSectors;

	UCHAR m_EntrySize;
	FAT_FILE m_Root;

	INT64 EnumDirectoryFiles(PFAT_FILE pParentDir, int op = 0);
	UINT DataClusterStartSector(UINT ClusterNumber);
	UINT GetNextClusterNumber(UINT ClusterNumber, PUINT count = nullptr);
	PUCHAR CacheFat(UINT sector, UINT count, BOOL cache);
	BOOL DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, bool deleted = false);

	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = false) override;
	void ClearCatch();
	bool IsClusterUser(UINT ClusterNumber);
	void InitClusterBitmap();
};

