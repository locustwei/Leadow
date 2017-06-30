/*!
 * asa
 * 
 * NTFS MFT分析Class
 *
 * 1、直接读取NTFS分区格式的文件信息
 * 2、利用USNJournal更新文件变动
*/
#pragma once

#include "stdafx.h"
#include "MftReader.h"
#include "ntfs.h"


typedef struct RUN_LINKE{
	ULONGLONG lcn;
	ULONG size;
	struct RUN_LINKE* next;
}*PRUN_LINKE;

/*
typedef struct FILE_HANDLE{
	ULONGLONG FileReferenceNumber;
	PFILE_RECORD_HEADER pFile;
	PATTRIBUTE DataAttr;
	PRUN_LINKE Run;
	ULONGLONG DataSize;
}*PFILE_HANDLE;
*/

class CNtfsMtfReader: public CMftReader
{
public:
	CNtfsMtfReader();
	~CNtfsMtfReader(void);
	
	virtual ULONGLONG EnumFiles(PVOID Param);
	virtual const PFILE_INFO GetFileInfo(ULONGLONG ReferenceNumber);
	virtual USN GetLastUsn();
	virtual USN UpdateFiles(USN LastUsn, PVOID param);
protected:
	virtual void ZeroMember();
	virtual bool Init();
private:

	typedef struct CACHE_INFO{
		ULONGLONG cache_lcn_begin;
		ULONGLONG cache_lcn_count;
		BYTE* g_pb;
		ULONGLONG cache_lcn_orl_begin;
		ULONGLONG cache_lcn_total;
	};

	ULONG m_BytesPerFileRecord;
	CACHE_INFO m_cache_info;
	PFILE_RECORD_HEADER m_Mft;
	PUCHAR m_MftBitmap;
	PUCHAR m_MftData;

	PFILE_RECORD_HEADER m_File;
	ULONGLONG m_FileCount;
	ULONG m_ClustersPerFileRecord;
	ULONG m_BytesPerClusters;

	VOID FixupUpdateSequenceArray(PFILE_RECORD_HEADER file);
	PATTRIBUTE FindAttribute(PFILE_RECORD_HEADER file,ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count = 0);
	ULONGLONG AttributeLengthAllocated(PATTRIBUTE attr);
	BOOL ReadAttribute(PATTRIBUTE attr, PVOID buffer, ULONG size = 0);
	BOOL  ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr,ULONGLONG vcn, ULONG count, PVOID buffer, bool b_allow_cache = false);
	BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count);
	BOOL NeedCache(ULONGLONG lcn);
	void NewCache(ULONGLONG lcn, ULONGLONG lcn_count);
	BOOL CanReadFromCache(ULONGLONG lcn);
	BOOL ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer);
	LONGLONG RunLCN(PUCHAR run);
	LONGLONG RunCount(PUCHAR run);
	ULONG RunLength(PUCHAR run);
	ULONGLONG AttributeLength(PATTRIBUTE attr);
	BOOL bitset(PUCHAR bitmap, ULONGLONG i);
	BOOL ReadFileRecord(PFILE_RECORD_HEADER Mft, ULONGLONG index, PFILE_RECORD_HEADER file);
	BOOL ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,ULONGLONG vcn, ULONG count, PVOID buffer);
	ULONGLONG GetFileCount();
	PFILENAME_ATTRIBUTE ReadFileNameInfoEx(ULONGLONG ReferenceNumber);
	PRUN_LINKE BuildRun(PNONRESIDENT_ATTRIBUTE attr);
	BOOL ReadAttributeData(PATTRIBUTE attr, PUCHAR Buffer, ULONG bufferSize);
	PFILE_INFO FileAttribute2Info(PFILENAME_ATTRIBUTE name);
	const PFILENAME_ATTRIBUTE ReadMtfFileNameAttribute(ULONGLONG ReferenceNumber);
	PATTRIBUTE EnumAttribute(PATTRIBUTE pAttrHeader, ULONG size, ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count /*= 0*/);
private:
	static BOOL QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus);
	static BOOL CreateUsnJournal(HANDLE hVolume);
	static BOOL DeleteUsnJournal(HANDLE hVolume);
};

