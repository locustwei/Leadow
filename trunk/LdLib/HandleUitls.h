#pragma once

#include "PublicRoutimes.h"

typedef BOOL (*EnumSystemHandleCallback)(PSYSTEM_HANDLE pHandle, PVOID pParam);

class CHandleUitls
{
public:
	static DWORD EnumSystemHandleInfo(EnumSystemHandleCallback callback, PVOID pParam);
	static DWORD FindOpenFileHandle(LPTSTR lpFullFileName, EnumSystemHandleCallback callback, PVOID pParam);
	static DWORD DuplicateSysHandle(PSYSTEM_HANDLE pHandle, HANDLE& hOutHandle);
	static DWORD GetSysHandleName(HANDLE hDupHandle, LPTSTR lpName);
	static DWORD GetSysHandleType(HANDLE hDupHandle, SYSTEM_HANDLE_TYPE& sysType);
private:
	typedef struct FIND_FILE_HANDLE_PARAM
	{
		//HANDLE hCurrentProcess;
		UINT nLength;
		LPTSTR lpFileName;
		EnumSystemHandleCallback callback;
		PVOID pParam;
	};
	static BOOL FindOpenFileCallback(PSYSTEM_HANDLE pHandle, PVOID pParam);
};

