#include "stdafx.h"
#include "MftReader.h"
#include "NtfsMtfReader.h"
#include "FatMftReader.h"


CMftReader::CMftReader(HANDLE hVolume)
{
	m_Handle = hVolume;
}


CMftReader::~CMftReader(void)
{
}

UINT64 CMftReader::EnumFiles(IMftReaderHandler* hander, PVOID Param)
{
	m_Holder = hander;
	m_Param = Param;
	return 0;
}

UINT64 CMftReader::EnumDeleteFiles(IMftReaderHandler*, PVOID)
{
	return 0;
}

void CMftReader::ZeroMember()
{
	m_BytesPerSector = 0;
	m_Holder = NULL;
}

//************************************
// Method:    ReadSector
// FullName:  读取分区扇区数据
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: UINT64 sector 起始扇区
// Parameter: ULONG count  读取扇区数
// Parameter: PVOID buffer  
//************************************
BOOL CMftReader::ReadSector(UINT64 sector, ULONG count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = {0};
	ULONG n;
	offset.QuadPart = sector * m_BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	BOOL result = ::ReadFile(m_Handle, buffer, count * m_BytesPerSector, &n, &overlap);
	
	return result;
}

//************************************
// Method:    Init
// FullName:  初始化分区数据
// Access:    virtual protected 
// Returns:   bool
// Qualifier:
//************************************

bool CMftReader::Init()
{
	ULONG dw;
	DISK_GEOMETRY dg = {0};

	if(!DeviceIoControl(m_Handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &dg, sizeof(DISK_GEOMETRY), &dw, NULL))
	{
		return false;
	}

	m_BytesPerSector = (USHORT)dg.BytesPerSector;

	return true;
}

BOOL CMftReader::IsValid()
{
	return m_Inited;
}

BOOL CMftReader::Callback(UINT64 ReferenceNumber, PMFT_FILE_DATA pFileInfo)
{
	if(m_Holder)
		return m_Holder->EnumMftFileCallback(pFileInfo, m_Param);
	else
		return FALSE;
}
