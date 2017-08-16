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
#include "../LdLib/volume/ntfs.h"

typedef struct RUN_LINKE{
	UINT64 lcn;
	DWORD size;
	struct RUN_LINKE* next;
}*PRUN_LINKE;

/*
typedef struct FILE_HANDLE{
	UINT64 FileReferenceNumber;
	PFILE_RECORD_HEADER pFile;
	PATTRIBUTE DataAttr;
	PRUN_LINKE Run;
	UINT64 DataSize;
}*PFILE_HANDLE;
*/

class CNtfsMtfReader: public CMftReader
{
public:
	CNtfsMtfReader();
	~CNtfsMtfReader(void);

	UINT64 EnumFiles(UINT_PTR Param) override;
	//const PFILE_INFO GetFileInfo(UINT64 ReferenceNumber) override;
	//USN GetLastUsn() override;
	//USN UpdateFiles(USN LastUsn, PVOID param) override;
protected:
	void ZeroMember() override;
	virtual bool Init();
private:

	typedef struct CACHE_INFO{
		UINT64 cache_lcn_begin;
		UINT64 cache_lcn_count;
		BYTE* g_pb;
		UINT64 cache_lcn_orl_begin;
		UINT64 cache_lcn_total;
	};

	CACHE_INFO m_cache_info;
	PFILE_RECORD_HEADER m_Mft;
	PUCHAR m_MftBitmap;
	PUCHAR m_MftData;

	PFILE_RECORD_HEADER m_File;
	UINT64 m_FileCount;
	PATTRIBUTE m_MftDataAttribute;
	VOID FixupUpdateSequenceArray(PFILE_RECORD_HEADER file);
	PATTRIBUTE FindAttribute(PFILE_RECORD_HEADER file,ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count = 0);
	UINT64 AttributeLengthAllocated(PATTRIBUTE attr);
	BOOL ReadAttribute(PATTRIBUTE attr, PVOID buffer, DWORD size = 0);
	BOOL  ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr,UINT64 vcn, DWORD count, PVOID buffer, bool b_allow_cache = false);
	BOOL FindRun(PNONRESIDENT_ATTRIBUTE attr, UINT64 vcn, PULONGLONG lcn, PULONGLONG count);
	BOOL NeedCache(UINT64 lcn);
	void NewCache(UINT64 lcn, UINT64 lcn_count);
	BOOL CanReadFromCache(UINT64 lcn);
	BOOL ReadLCN(UINT64 lcn, DWORD count, PVOID buffer);
	INT64 RunLCN(PUCHAR run);
	INT64 RunCount(PUCHAR run);
	DWORD RunLength(PUCHAR run);
	UINT64 AttributeLength(PATTRIBUTE attr);
	BOOL bitset(PUCHAR bitmap, UINT64 i);
	BOOL ReadFileRecord(PFILE_RECORD_HEADER Mft, UINT64 index, PFILE_RECORD_HEADER& file);
	BOOL ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,UINT64 vcn, DWORD count, PVOID buffer);
	//UINT64 GetFileCount();
	PFILENAME_ATTRIBUTE ReadFileNameInfoEx(UINT64 ReferenceNumber);
	//PRUN_LINKE BuildRun(PNONRESIDENT_ATTRIBUTE attr);
	//BOOL ReadAttributeData(PATTRIBUTE attr, PUCHAR Buffer, DWORD bufferSize);
	PFILE_INFO FileAttribute2Info(PFILENAME_ATTRIBUTE name);
	const PFILENAME_ATTRIBUTE ReadMtfFileNameAttribute(UINT64 ReferenceNumber);
	PATTRIBUTE EnumAttribute(PATTRIBUTE pAttrHeader, DWORD size, ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count /*= 0*/);
private:
//	static BOOL QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus);
//	static BOOL CreateUsnJournal(HANDLE hVolume);
//	static BOOL DeleteUsnJournal(HANDLE hVolume);
};

