/*!
 * asa
 * 
 * NTFS MFT分析Class
 *
 * 1、直接读取NTFS分区格式的文件信息
 * 2、利用USNJournal更新文件变动
*/
#pragma once

#include "MftReader.h"
#include "ntfs.h"
#include "NtfsFile.h"


typedef struct RUN_LINKE{
	UINT64 lcn;
	UINT size;
	struct RUN_LINKE* next;
}*PRUN_LINKE;

class CNtfsMtfReader: public CMftReader
{
public:
	CNtfsMtfReader(HANDLE hVolume);
	~CNtfsMtfReader(void) override;

	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;
	UINT64 EnumDeleteFiles(IMftDeleteReaderHandler*, PVOID) override;
	BOOL GetFileStats(PUINT64 FileCount, PUINT64 FolderCount, PUINT64 DeletedFileTracks) override;

	BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) override;
	CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = true) override;
protected:
	void ZeroMember() override;
	bool Init() override;
private:

	typedef struct CACHE_INFO{
		UINT64 StartVcn;
		UINT Count;
		BYTE* Buffer;
	};
	CACHE_INFO m_FatCache;
	CACHE_INFO m_ClusterBitmapCache;

	UINT m_BytesPerCluster;
	UINT m_BytesPerFileRecord;
	UINT64 m_FileCount;
	//UINT m_BytesPerClusters;

	//PNTFS_FILE_RECORD_HEADER m_MftRecord;
	CNtfsFile m_MftFile;
	CNtfsFile m_ClusterBitmap;

	PUCHAR m_MftBitmap;

	PNTFS_FILE_RECORD_HEADER m_FileCanche;

	VOID FixupUpdateSequenceArray(PNTFS_FILE_RECORD_HEADER file);
	
	BOOL ReadFileData(CNtfsFile* file, UINT64 vcn, UINT count, PVOID buffer);
	BOOL CacheFileData(CNtfsFile*, UINT64, UINT);
	UINT CanReadFromCache(UINT64 lcn, UINT count);
	BOOL bitset(PUCHAR bitmap, UINT64 i);
	BOOL ReadFileRecord(UINT64 index, PNTFS_FILE_RECORD_HEADER file, bool cache = true);
	UINT64 ReadFileAttributeData(CNtfsFile* File, CNtfsFileAttribute* attr, PUCHAR buffer, UINT Size);
	bool GetBitmapFile();
	bool IsClusterUsed(UINT64);
	PUCHAR CactchClusterBitmap(UINT64 vcn);
private:
	void ClearCache();
	UINT64 GetFileLcn(UINT64 FileReferenceNumber);
};

