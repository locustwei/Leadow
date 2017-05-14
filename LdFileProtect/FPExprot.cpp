#include "stdafx.h"
#include "LdLib.h"
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


struct CALLBACK_PARAM
{
	UINT nCount;
	PDWORD pids;
};

BOOL DeleteApiFileCallback(PSYSTEM_HANDLE pHandle, PVOID pParam)
{
	struct CALLBACK_PARAM* Param = (struct CALLBACK_PARAM*) pParam;

	if (Param->nCount >= 100)
		return FALSE;
	Param->pids[Param->nCount++] = pHandle->dwProcessId;
	return TRUE;
}

BOOL LDLIB_API API_DeleteFiles(LPTSTR lpFileNames, DWORD dwFlags)
{
	BOOL Result = DeleteFile(lpFileNames);
	if (Result)
		return TRUE;
	struct CALLBACK_PARAM Param;
	Param.nCount = 0;
	Param.pids = new DWORD[100];
	DWORD ret = CHandleUitls::FindOpenFileHandle(lpFileNames, DeleteApiFileCallback, &Param);

	TCHAR* FileExt = _tcsstr(lpFileNames, _T(".exe"));
	if (FileExt && wcslen(FileExt) == 4)
	{
		Param.nCount += CProcessUtils::FindOpenProcess(lpFileNames, Param.pids + Param.nCount);
	}
	FileExt = _tcsstr(lpFileNames, _T(".dll"));
	if (FileExt && wcslen(FileExt) == 4)
	{
		Param.nCount += CProcessUtils::FindOpenModule(lpFileNames, Param.pids + Param.nCount);
	}

	if (Param.nCount)
	{

	}
	return 0;
}
