#pragma once

#include "LdStructs.h"

DWORD LDLIB_API API_SelectProtectFlag(HWND hParentWnd);

BOOL LDLIB_API API_ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR* lpFileNames);
