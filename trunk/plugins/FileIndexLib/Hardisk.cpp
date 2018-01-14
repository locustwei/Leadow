#include "Hardisk.h"


CHardisk::CHardisk(void)
{
	m_hDisk = INVALID_HANDLE_VALUE;
}


CHardisk::~CHardisk(void)
{
	if(m_hDisk != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDisk);
}

CHardisk * CHardisk::CreateHardisk(DWORD nIndex)
{
	CHardisk * result = NULL;

	TCHAR sdisk[200] = {0};

	wsprintf(sdisk, L"\\.\\PhysicalDrive%d", nIndex);

	HANDLE h = CreateFile(sdisk, 0, FILE_SHARE_READ || FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_DEVICE, 0);
	if(h != INVALID_HANDLE_VALUE){
		result = new CHardisk();
		result->m_hDisk = h;
	}

	return result;
}

VOID CHardisk::GetDiskPartitions()
{
	if(m_hDisk == INVALID_HANDLE_VALUE)
		return;

	DWORD cb = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 1024;
	PDRIVE_LAYOUT_INFORMATION_EX buffer = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(cb);
	ZeroMemory(buffer, cb);

	if(!(DeviceIoControl(m_hDisk,IOCTL_DISK_GET_DRIVE_LAYOUT_EX,NULL, 0, &buffer, cb, &cb, NULL)))
		return;
	for(DWORD i=0; i<buffer->PartitionCount; i++){
	}

}
