#pragma once
#include "..\LbLib\LdStructs.h"

class CFileProtectDirver
{
public:
	CFileProtectDirver(void);
	~CFileProtectDirver(void);
	BOOL ProtectFile(LPCTSTR FileName, DWORD Flag);
private:
	HANDLE m_hDirver;
};

