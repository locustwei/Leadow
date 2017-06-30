#include "stdafx.h"

#include "MftReader.h"
#include "NtfsMtfReader.h"
#include "FatMftReader.h"

void SaveDebugData(LPCTSTR lpName, PUCHAR pData, ULONG nSize)
{
	
}

CMftReader::CMftReader(void)
{
	ZeroMember();
}


CMftReader::~CMftReader(void)
{
	if(m_hVolumeBack != INVALID_HANDLE_VALUE)
		CloseHandle(m_hVolumeBack);
	m_hVolumeBack = NULL;
}

BOOL CMftReader::SetHandle(HANDLE hVolume)
{
	m_Handle = hVolume;
	return TRUE;
}

ULONGLONG CMftReader::EnumFiles(PVOID Param)
{
	return 0;
}

void CMftReader::ZeroMember()
{
	m_Handle = INVALID_HANDLE_VALUE;
	m_hVolumeBack = INVALID_HANDLE_VALUE;

	m_BytesPerSector = 0;
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
CMftReader* CMftReader::CreateReader(HANDLE hVolume)
{
	CMftReader* result = NULL;

	ULONG dw;
	FILESYSTEM_STATISTICS fss = {0};

	DeviceIoControl(hVolume, FSCTL_FILESYSTEM_GET_STATISTICS, NULL, 0, &fss, sizeof(FILESYSTEM_STATISTICS), &dw, NULL);
	if(GetLastError()!=0 && GetLastError()!=ERROR_MORE_DATA)
	{
#ifdef _DEBUG
		printf("Query Disk File System error code = %d \n", GetLastError());
#endif
		return NULL;
	}

#ifdef _DEBUG
	printf("File System type = %d \n", fss.FileSystemType);
#endif

	switch(fss.FileSystemType){
	case FILESYSTEM_STATISTICS_TYPE_NTFS:

		result = new CNtfsMtfReader();
		break;
	case FILESYSTEM_STATISTICS_TYPE_FAT:
		result = new CFatMftReader();
		break;
	default:
		return FALSE;
	}

	result->m_Handle = hVolume;
	if(!result->Init()){
#ifdef _DEBUG
	printf("mft reader init failed \n");
#endif // _DEBUG
		delete result;
		return NULL;
	}

	return result;
}

//************************************
// Method:    ReadSector
// FullName:  读取分区扇区数据
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: ULONGLONG sector 起始扇区
// Parameter: ULONG count  读取扇区数
// Parameter: PVOID buffer  
//************************************
BOOL CMftReader::ReadSector(ULONGLONG sector, ULONG count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = {0};
	ULONG n;
	offset.QuadPart = sector * m_BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	return ::ReadFile(m_Handle, buffer, count * m_BytesPerSector, &n, &overlap);
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

#ifdef _DEBUG
		printf("Query disk Bytes Per Sector error code = %d \n", GetLastError());
#endif

		return FALSE;
	}

	m_BytesPerSector = (USHORT)dg.BytesPerSector;
#ifdef _DEBUG
	printf("Bytes per sector = %d \n", m_BytesPerSector);
#endif
	return true;
}

//************************************
// Method:    GetFileInfo
// FullName:  获取指定文件信息
// Access:    virtual public 
// Returns:   const PFILE_INFO
// Qualifier:
// Parameter: ULONGLONG ReferenceNumber  文件编号
//************************************
const PFILE_INFO CMftReader::GetFileInfo(ULONGLONG ReferenceNumber)
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

BOOL CMftReader::Callback(ULONGLONG ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param)
{
	if(m_Holder)
		return m_Holder->EnumMftFileCallback(ReferenceNumber, pFileInfo, Param);
	else
		return FALSE;
}

BOOL CMftReader::OpenVolume(const wchar_t* wsz_volum)
{
	BOOL result = false;
	HANDLE hVolume;

	if (wsz_volum && m_wstr_volum.empty())
	{
		m_wstr_volum = wsz_volum;
		m_wstr_volum.to_lower();
		//m_wstr_volum.convert_code(m_str_volum_u8, CP_UTF8);
		stringxw wstr_volum = L"\\\\.\\";
		wstr_volum += m_wstr_volum;
		hVolume = CreateFile(wstr_volum.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		if (hVolume != INVALID_HANDLE_VALUE){
			result = SetHandle(hVolume);
			if(!result)
				CloseHandle(hVolume);
			else
				m_hVolumeBack = hVolume;
		}

	}

	return result;

}

USN CMftReader::GetLastUsn()
{
	return 0;
}

USN CMftReader::UpdateFiles(USN LastUsn, PVOID param)
{
	return 0;
}
