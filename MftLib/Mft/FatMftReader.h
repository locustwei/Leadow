#pragma once

#include "MftReader.h"
#include "Fat.h"

class CFatMftReader: public CMftReader
{
public:
	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;

	UINT64 EnumDeleteFiles(IMftDeleteReaderHandler*, PVOID) override;
	BOOL GetFileStats(PUINT64 FileCount, PUINT64 FolderCount, PUINT64 DeletedFileTracks, PUINT64 DeleteFileCount) override;

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
	UINT64 m_FileReferenceNumber;        //自增长的文件号
	MFT_FILE_DATA m_FileInfo;            //临时使用的中间变量

	//GetFileStats 函数参数
	PUINT64 m_FileCount_Stats;
	PUINT64 m_FolderCount_Stats;
	PUINT64 m_DeleteFileTracks_Stats;
	PUINT64 m_DeleteFileCount_Stats;

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

