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

	Pipe.ReadData()

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
