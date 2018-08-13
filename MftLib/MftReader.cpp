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
	m_Inited = Init();
	return 0;
}

UINT64 CMftReader::EnumDeleteFiles(IMftDeleteReaderHandler* hander, PVOID Param)
{
	m_DelHolder = hander;
	m_Param = Param;
	m_Inited = Init();
	return 0;
}

void CMftReader::ZeroMember()
{
	m_BytesPerSector = 0;
	m_Holder = nullptr;
	m_DelHolder = nullptr;
	m_Inited = false;
}

//************************************
// Method:    ReadSector
// FullName:  读取分区扇区数据
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: UINT64 sector 起始扇区
// Parameter: UINT count  读取扇区数
// Parameter: PVOID buffer  
//************************************
BOOL CMftReader::ReadSector(UINT64 sector, UINT count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = {0};
	DWORD n;
	offset.QuadPart = sector * m_BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	BOOL result = ::ReadFile(m_Handle, buffer, count * m_BytesPerSector, &n, &overlap);
	
	return result;
}

BOOL CMftReader::ReadCluster(UINT64 Cluster, UINT count, PVOID buffer)
{
	return ReadSector(Cluster * m_SectorsPerCluster, count * m_SectorsPerCluster, buffer);
}

USHORT CMftReader::GetBytesPerSector()
{
	if (!m_Inited)
		m_Inited = Init();
	return m_BytesPerSector;
}

USHORT CMftReader::GetSectorsPerCluster()
{
	if (!m_Inited)
		m_Inited = Init();
	return m_SectorsPerCluster;
}

UINT64 CMftReader::GetTotalCluster()
{
	if (!m_Inited)
		m_Inited = Init();
	return m_TotalCluster;
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
	DWORD dw;
	DISK_GEOMETRY dg = {0};

	if (!m_Inited)
	{
		if (!DeviceIoControl(m_Handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &dg, sizeof(DISK_GEOMETRY), &dw, NULL))
		{
			return false;
		}

		m_BytesPerSector = (USHORT)dg.BytesPerSector;
	}
	//m_Inited = true;

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

BOOL CMftReader::DelCallback(UINT64 ReferenceNumber, PMFT_FILE_DATA pFileInfo, PFILE_DATA_STREAM datastream)
{
	if (m_DelHolder)
		return m_DelHolder->EnumMftDeleteFileCallback(pFileInfo, datastream, m_Param);
	else
		return FALSE;
}
