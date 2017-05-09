#include "stdafx.h"
#include "LdStructs.h"
#include <string.h>

BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName)
{
	TCHAR FileName[MAX_PATH] = { 0 };
	if (!GetModuleFileName(0, FileName, MAX_PATH))
		return false;

	TCHAR* s = _tcsrchr(FileName, '\\');
	if (s == NULL)
		return false;
	s += 1;
	_tcscpy(s, _T("LdInvoker_d64.exe"));

	TCHAR Params[100] = { 0 };
	wsprintf(Params, _T("%d %d %s"), id, Flag, lpPipeName);
	return ShellExecute(NULL, NULL, FileName, Params, NULL, SW_SHOWNORMAL) != NULL;

}