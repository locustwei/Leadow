#pragma once

DWORD LDLIB_API API_Init(PAuthorization);
VOID LDLIB_API API_UnInit();

DWORD LDLIB_API API_SelectProtectFlag(HWND hParentWnd);
BOOL LDLIB_API API_ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR* lpFileNames);
BOOL LDLIB_API API_DeleteFiles(LPTSTR lpFileNames, DWORD dwFlags);