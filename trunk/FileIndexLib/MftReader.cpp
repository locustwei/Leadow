#include "stdafx.h"

#include "MftReader.h"
#include "NtfsMtfReader.h"
#include "FatMftReader.h"


CMftReader::CMftReader(void)
{
	ZeroMember();
}


CMftReader::~CMftReader(void)
{
	if(m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
	m_Handle = INVALID_HANDLE_VALUE;
}

void CMftReader::ZeroMember()
{
	m_Handle = INVALID_HANDLE_VALUE;
	m_Holder = NULL;
}

//************************************
// Method:    CreateReader
// FullName:  为分区格式创建文件分配表读取对象
// Access:    public static 
// Returns:   CMftReader*
// Qualifier:
// Parameter: HANDLE hVolume
//************************************
CMftReader* CMftReader::CreateReader(CVolumeInfo * pVolume)
{
	CMftReader* result = nullptr;
	DWORD r;
	VOLUME_FILE_SYSTEM fs = pVolume->GetFileSystem(&r);
	if (r != 0)
		return nullptr;

	switch(fs){
	case FS_NTFS:
		result = new CNtfsMtfReader();
		break;
	case FS_FAT32:
	case FS_FAT16:
	case FS_FAT12:
		result = new CFatMftReader();
		break;
	default:
		return nullptr;
	}
	result->m_Volume = pVolume;
	return result;
}

//************************************
// Method:    ReadSector
// FullName:  读取分区扇区数据
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: UINT64 sector 起始扇区
// Parameter: DWORD count  读取扇区数
// Parameter: PVOID buffer  
//************************************
BOOL CMftReader::ReadSector(UINT64 sector, DWORD count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = {0};
	DWORD n;
	offset.QuadPart = sector * m_Volume->GetSectorSize();
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	return ::ReadFile(m_Handle, buffer, count * m_Volume->GetSectorSize(), &n, &overlap);
}

//************************************
// Method:    GetFileInfo
// FullName:  获取指定文件信息
// Access:    virtual public 
// Returns:   const PFILE_INFO
// Qualifier:
// Parameter: UINT64 ReferenceNumber  文件编号
//************************************
const PFILE_INFO CMftReader::GetFileInfo(UINT64 ReferenceNumber)
{
	return NULL;
}

BOOL CMftReader::IsValid()
{
	return m_Handle != INVALID_HANDLE_VALUE;
}

PMftReadeHolder CMftReader::SetHolder(PMftReadeHolder pHolder)
{
	PMftReadeHolder result = m_Holder;
	m_Holder = pHolder;
	return result;
}

BOOL CMftReader::Callback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param)
{
	if(m_Holder)
		return m_Holder->EnumMftFileCallback(ReferenceNumber, pFileInfo, Param);
	else
		return FALSE;
}

USN CMftReader::GetLastUsn()
{
	return 0;
}

USN CMftReader::UpdateFiles(USN LastUsn, PVOID param)
{
	return 0;
}
