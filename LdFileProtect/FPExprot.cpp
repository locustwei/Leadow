#include "stdafx.h"
#include "FPExprot.h"
#include "SelectProtectFlag.h"

DWORD LDLIB_API API_SelectProtectFlag(HWND hParentWnd)
{
	return CSelectProtectFlag::SelectFlags(hParentWnd);
}

BOOL LDLIB_API API_ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR * lpFileNames)
{
	return 0;
}
