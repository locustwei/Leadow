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
	ULONG size;
	struct RUN_LINKE* next;
}*PRUN_LINKE;

class CNtfsMtfReader: public CMftReader
{
public:
	CNtfsMtfReader(HANDLE hVolume);
	~CNtfsMtfReader(void) override;

	UINT64 EnumFiles(IMftReaderHandler*, PVOID Param) override;
	BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) override;
	//virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = true) override;
	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = true);
protected:
	virtual void ZeroMember() override;
	virtual bool Init() override;
private:

	typedef struct CACHE_INFO{
		/*CLdArray<UINT64> StartLcns;
		CLdArray<UINT> Counts;
		UINT GetCount()
		{
			UINT ret = 0;
			for (int i = 0; i < Counts.GetCount(); i++)
			{
				ret += Counts[i];
			}
			return ret;
		*/
		UINT64 StartLcn;
		UINT Count;
		BYTE* Buffer;
	};

	ULONG m_BytesPerFileRecord;
	CACHE_INFO m_Cache;
	PNTFS_FILE_RECORD_HEADER m_MftRecord;
	CNtfsFile m_MftFile;
	PUCHAR m_MftBitmap;
	//PUCHAR m_MftData;

	PNTFS_FILE_RECORD_HEADER m_FileCanche;
	UINT64 m_FileCount;
	ULONG m_ClustersPerFileRecord;
	ULONG m_BytesPerClusters;
	//PATTRIBUTE m_MftData;

	VOID FixupUpdateSequenceArray(PNTFS_FILE_RECORD_HEADER file);
	
	//PATTRIBUTE EnumAttributeList(PATTRIBUTE_LIST pAttrHeader, ULONG size, ATTRIBUTE_TYPE type, TCHAR* name, int n_attr_count = 0);

	//PATTRIBUTE FindAttribute(PNTFS_FILE_RECORD_HEADER file, ATTRIBUTE_TYPE type, TCHAR* name, int n_attr_count = 0);
	//UINT64 AttributeLengthAllocated(PATTRIBUTE attr);
	//BOOL ReadAttribute(PATTRIBUTE attr, PVOID buffer, ULONG size = 0);
	//BOOL ReadFileData(CNtfsFile * file, UINT64 vcn, ULONG count, PVOID buffer, bool b_allow_cache);
	BOOL ReadFileData(CNtfsFile* file, UINT64 vcn, ULONG count, PVOID buffer, bool cache = true);
	//BOOL  ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr, UINT64 vcn, ULONG count, PVOID buffer, bool b_allow_cache = false);
	//BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, UINT64 vcn, PULONGLONG lcn, PULONGLONG count);
	BOOL CacheFileData(CNtfsFile*, UINT64, UINT);
	UINT CanReadFromCache(UINT64 lcn, UINT count);
	BOOL ReadLCN(UINT64 lcn, ULONG count, PVOID buffer);
	//LONGLONG RunLCN(PUCHAR run);
	//LONGLONG RunCount(PUCHAR run);
	//ULONG RunLength(PUCHAR run);
	//UINT64 AttributeLength(PATTRIBUTE attr);
	BOOL bitset(PUCHAR bitmap, UINT64 i);
	BOOL ReadFileRecord(UINT64 index, PNTFS_FILE_RECORD_HEADER file, bool cache = true);
	//BOOL ReadVCN(PNTFS_FILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,UINT64 vcn, ULONG count, PVOID buffer);
	//UINT64 GetFileCount();
	//PFILENAME_ATTRIBUTE ReadFileNameInfoEx(UINT64 ReferenceNumber);
	//PRUN_LINKE BuildRun(PNONRESIDENT_ATTRIBUTE attr);
	//BOOL ReadAttributeData(PATTRIBUTE attr, PUCHAR Buffer, ULONG bufferSize);
	UINT64 ReadFileAttributeData(CNtfsFile* File, CNtfsFileAttribute* attr, PUCHAR buffer, UINT Size);
	//VOID FileAttribute2Info(PMFT_FILE_INFO aFileInfo, PFILENAME_ATTRIBUTE name);
	//const PFILENAME_ATTRIBUTE ReadMtfFileNameAttribute(UINT64 ReferenceNumber);
	//PATTRIBUTE EnumAttribute(PATTRIBUTE pAttrHeader, ULONG size, ATTRIBUTE_TYPE type, TCHAR* name, int n_attr_count /*= 0*/);
private:
	void ClearCache();
};

