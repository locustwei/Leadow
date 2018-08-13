#pragma once

#include "MftReader.h"
#include "Fat.h"

class CExFatReader: public CMftReader
{
public:
	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;

	UINT64 EnumDeleteFiles(IMftDeleteReaderHandler*, PVOID) override;

public:
	CExFatReader(HANDLE hVolume);
	~CExFatReader(void) override;

	BOOL ReadCluster(UINT64 Cluster, UINT count, PVOID buffer) override;
protected:
	bool Init() override;
	void ZeroMember() override;

	virtual BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) override;
private:

	typedef struct FAT_CACHE{
		UINT64 beginSector;
		UINT sectorCount;
		BYTE* buffer;
	};
	FAT_CACHE m_fatCache;

	//USHORT m_SectorsPerRootDirectory;
	UINT m_SectorsPerFAT;
	UINT m_FirstDataSector;
	UINT m_FirstFatSector;
	UINT64 m_TotalSectors;

	UINT m_ClusterBitmapSectors;
	UINT64 m_ClusterBitmapStartSector;

	EXFAT_FILE m_Root;
	MFT_FILE_DATA m_FileInfo;
	FAT_CACHE m_BitmapCache;

	INT64 EnumDirectoryFiles(PEXFAT_FILE pParentDir, int op = 0);
	UINT64 DataClusterStartSector(UINT ClusterNumber);
	UINT GetNextClusterNumber(UINT ClusterNumber);
	PUCHAR CacheFat(UINT sector, UINT count);
	BOOL DoAFatFile(PEXFAT_FILE pParentDir, PEXFAT_FILE pFile = nullptr, bool deleted = false);

	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = false) override;
	void ClearCatch();
	PUCHAR CactchClusterBitmap(UINT StartCluster);
	bool IsClusterUsed(UINT ClusterNumber);
};

