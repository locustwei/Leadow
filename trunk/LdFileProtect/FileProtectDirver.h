#pragma once
#include "LdStructs.h"
#include "LdDriver.h"

#define LD_FPDRV_NAME _T("\\\\.\\WiseFS")

class CFileProtectDirver : CLdDriver
{
public:
	CFileProtectDirver(void);
	~CFileProtectDirver(void);
	BOOL ProtectFile(LPCTSTR lpFileName, DWORD dwFlag);
protected:
	virtual void SetDriverName() override;
private:
	PFS_FILE_RECORD BuildDeviceData(LPCTSTR lpFileName, DWORD dwFlag);
};

