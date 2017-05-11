#pragma once

#include <winioctl.h>
#include "LdStructs.h"
#include "LdDriver.h"

#define LD_FPDRV_NAME _T("\\\\.\\WiseFS")

#define IOCTL_SET_EXCLUDE_HANDLE      CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 1), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FS_ADD_FILE             CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 2), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FS_UPDATE_FILE          CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 3), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FS_REMOVE_FILE          CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 4), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FS_ENUM_FILES           CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 5), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_FS_ENABLE_FILE          CTL_CODE (FILE_DEVICE_UNKNOWN, (0x900 + 6), METHOD_BUFFERED, FILE_ANY_ACCESS)
/*
typedef enum _FS_FILE_TYPE {
	FS_HIDE = 0x1,
	FS_WRITE = 0x2,
	FS_DELETE = 0x4,
	FS_RENAME = 0x8,
	FS_CREATE = 0x10,
	FS_ALL = FS_HIDE | FS_WRITE | FS_DELETE | FS_RENAME | FS_CREATE
}FS_FILE_TYPE;
*/

typedef struct _FS_FILE_RECORD {  //隐藏文件记录。
	ULONG cb;                    //记录字节数
	ULONGLONG id;                //
	BOOLEAN bDisable;
	DWORD pt;
	USHORT nVolume;
	USHORT nPath;
	USHORT nName;
	WCHAR Data[1];
}FS_FILE_RECORD, *PFS_FILE_RECORD;

class CFileProtectDirver : public CLdDriver
{
public:
	CFileProtectDirver(void);
	~CFileProtectDirver(void);
	BOOL ProtectFile(LPCTSTR lpFileName, DWORD dwFlag);
protected:
	virtual void SetDriverName() override;
private:
	DWORD BuildDeviceData(LPTSTR lpFileName, DWORD dwFlag, PFS_FILE_RECORD* pOutFile);
};

