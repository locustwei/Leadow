#include "StdAfx.h"
#include "LdFunction.h"
#include "../LdFileProtect/FPExprot.h"

BOOL ProtectFiles(LPTSTR lpPipeFileName, HWND hParentWnd)
{
	BOOL b = false;
	DWORD dwFlag = API_SelectProtectFlag(hParentWnd);

	if (dwFlag == 0)
		return b;

	CLdNamedPipe Pipe;
	if (!Pipe.Open(lpPipeFileName))
		return b;

	DWORD dwFileCount = 0;

	if(Pipe.ReadData(&dwFileCount, sizeof(dwFileCount)) != sizeof(dwFileCount))
		return false;

	LPTSTR* lpFileNames = new LPTSTR[dwFileCount];
	ZeroMemory(lpFileNames, dwFileCount * sizeof(LPTSTR));
	for (int i = 0; i < dwFileCount; i++)
	{
		lpFileNames[i] = new TCHAR[MAX_PATH];
		ZeroMemory(lpFileNames[i], MAX_PATH * sizeof(TCHAR));

		Pipe.ReadData(lpFileNames[i], MAX_PATH * sizeof(TCHAR));
	}

	b = API_ProtectFiles(dwFlag, dwFileCount, lpFileNames);

	for (int i = 0; i < dwFileCount; i++)
	{
		delete[] lpFileNames[i];
	}
	delete[] lpFileNames;

	return b;
}

BOOL InvokeLdFunc(LD_FUNCTION_ID id, LPTSTR lpPipeName, HWND hMainWnd)
{
	BOOL b = FALSE;
	
	switch (id)
	{
	case LFI_NONE:
		break;
	case LFI_HIDE_FILE:
		b = ProtectFiles(lpPipeName, hMainWnd);
		break;
	case LFI_DELETE_FILE:
		break;
	default:
		break;
	}

	return b;
}
