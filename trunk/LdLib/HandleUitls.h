#pragma once

#include "PublicRoutimes.h"

class IEnumSystemHandleCallback
{
public:
	virtual BOOL SystemHandleCallback(PSYSTEM_HANDLE pHandle, PVOID pParam) = 0;
};

class CHandleUitls
{
public:
	static DWORD EnumSystemHandleInfo(IEnumSystemHandleCallback* callback, PVOID pParam);
	static DWORD FindOpenFileHandle(LPTSTR lpFullFileName, IEnumSystemHandleCallback* callback, PVOID pParam);
	static DWORD DuplicateSysHandle(PSYSTEM_HANDLE pHandle, HANDLE& hOutHandle);
	static DWORD GetSysHandleName(HANDLE hDupHandle, LPTSTR lpName);
	static DWORD GetSysHandleType(HANDLE hDupHandle, SYSTEM_HANDLE_TYPE& sysType);
private:
};

