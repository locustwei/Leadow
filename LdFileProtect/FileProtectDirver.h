#pragma once
#include "LdStructs.h"
#include "LdDriver.h"
#include "FPDControlCode.h"

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
	DWORD BuildDeviceData(LPTSTR lpFileName, DWORD dwFlag, PFS_FILE_RECORD* pOutFile);
};

