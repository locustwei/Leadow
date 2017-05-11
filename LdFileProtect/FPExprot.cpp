#include "stdafx.h"
#include "FPExprot.h"
#include "SelectProtectFlag.h"
#include "FileProtectDirver.h"

DWORD LDLIB_API API_SelectProtectFlag(HWND hParentWnd)
{
	return CSelectProtectFlag::SelectFlags(hParentWnd);
}

BOOL LDLIB_API API_ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR * lpFileNames)
{
	CFileProtectDirver Driver;
	for (int i = 0; i < nFileCount; i++)
	{
		if(!Driver.ProtectFile(lpFileNames[i], dwFlags))
			return FALSE;
	}
	return TRUE;
}

BOOL LDLIB_API API_DeleteFiles(LPTSTR lpFileNames, DWORD dwFlags)
{
	BOOL Result = DeleteFile(lpFileNames);
	if (Result)
		return TRUE;
	
	return 0;
}
