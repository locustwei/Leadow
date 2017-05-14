#include "stdafx.h"
#include <Psapi.h>
#include "ProcessUtils.h"
#include "PublicRoutimes.h"
#include "FileUtils.h"

DWORD CProcessUtils::KillProcess(DWORD dwProcessId)
{
	return 0;
}

DWORD CProcessUtils::EnumProcess(EnumProcessCallback callback, PVOID pParam)
{
	BOOL IsLoopContinue;
	HANDLE FSnapshotHandle;
	PROCESSENTRY32 FProcessEntry32 = { 0 };

	FSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (FSnapshotHandle == INVALID_HANDLE_VALUE)
		return GetLastError();
	FProcessEntry32.dwSize = sizeof(PROCESSENTRY32);
	IsLoopContinue = Process32First(FSnapshotHandle, &FProcessEntry32); 
	while (IsLoopContinue)
	{
		if (callback)
			IsLoopContinue = callback(&FProcessEntry32, pParam);
		if (IsLoopContinue)
			IsLoopContinue = Process32Next(FSnapshotHandle, &FProcessEntry32);
	}

	CloseHandle(FSnapshotHandle);
	return 0;
}

DWORD CProcessUtils::EnumProcessModule(DWORD dwProcessId, EnumProcessModalCallback callback, PVOID pParam)
{
	BOOL IsLoopContinue;
	HANDLE FSnapshotHandle;
	MODULEENTRY32 FProcessEntry32 = { 0 };

	FSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (FSnapshotHandle == INVALID_HANDLE_VALUE)
		return GetLastError();
	FProcessEntry32.dwSize = sizeof(MODULEENTRY32);
	IsLoopContinue = Module32First(FSnapshotHandle, &FProcessEntry32);
	while (IsLoopContinue)
	{
		if (callback)
			IsLoopContinue = callback(&FProcessEntry32, pParam);
		if (IsLoopContinue)
			IsLoopContinue = Module32Next(FSnapshotHandle, &FProcessEntry32);
	}

	CloseHandle(FSnapshotHandle);
	return 0;
}

DWORD CProcessUtils::GetProcessFileName(DWORD dwProcessId, LPTSTR lpFileName)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (hProcess == NULL)
		return 0;
	HMODULE hMod;
	DWORD dwCb, nLength;

	TCHAR FileName[MAX_PATH] = { 0 };
	if (GetProcessImageFileName(hProcess, FileName, MAX_PATH))
	{
		nLength = wcslen(FileName);

		if(lpFileName)
			nLength = CFileUtils::DevicePathToWin32Path(FileName, lpFileName);
	}
	else if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &dwCb))
	{
		if (lpFileName == NULL)
		{
			nLength = GetModuleFileNameEx(hProcess, hMod, lpFileName, 0);
		}
		else
		{
			nLength = GetModuleFileNameEx(hProcess, hMod, lpFileName, MAX_PATH);
		}
	}

	CloseHandle(hProcess);

	return nLength;
}

BOOL CProcessUtils::FindProcessCallback(PPROCESSENTRY32 pEntry32, PVOID pParam)
{
	FPCallback_param* Param = (FPCallback_param*)pParam;
	TCHAR name[MAX_PATH] = { 0 };
	DWORD ret = GetProcessFileName(pEntry32->th32ProcessID, name);
	if (ret > 0 && wcslen(name) == Param->nLength && wcsnicmp(Param->lpFileName, name, Param->nLength) == 0)
		Param->pids[Param->nCount++] = pEntry32->th32ProcessID;
	return TRUE;
}

DWORD CProcessUtils::FindOpenProcess(LPTSTR lpFileName, PDWORD OutIds)
{
	FPCallback_param Param;
	Param.lpFileName = lpFileName;
	Param.nLength = wcslen(lpFileName);
	Param.nCount = 0;
	Param.pids = OutIds;
	if(EnumProcess(FindProcessCallback, &Param) == 0)
		return Param.nCount;
	else
		return 0;
}

BOOL CProcessUtils::FindModuleCallback(PPROCESSENTRY32 pEntry32, PVOID pParam)
{
	FPCallback_param* Param = (FPCallback_param*)pParam;
	BOOL IsLoopContinue;
	HANDLE FSnapshotHandle;
	MODULEENTRY32 FModuleEntry32 = { 0 };

	FSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pEntry32->th32ProcessID);
	if (FSnapshotHandle == INVALID_HANDLE_VALUE)
		return TRUE;
	FModuleEntry32.dwSize = sizeof(MODULEENTRY32);
	IsLoopContinue = Module32First(FSnapshotHandle, &FModuleEntry32);
	while (IsLoopContinue)
	{
		if (wcslen(FModuleEntry32.szExePath) == Param->nLength && wcsnicmp(FModuleEntry32.szExePath, Param->lpFileName, Param->nLength) == 0)
		{
			Param->pids[Param->nCount++] = pEntry32->th32ProcessID;
			IsLoopContinue = FALSE;
		}
		if (IsLoopContinue)
			IsLoopContinue = Module32Next(FSnapshotHandle, &FModuleEntry32);
	}

	CloseHandle(FSnapshotHandle);
	return TRUE;
}

DWORD CProcessUtils::FindOpenModule(LPTSTR lpFileName, PDWORD OutIds)
{
	FPCallback_param Param;
	Param.lpFileName = lpFileName;
	Param.nLength = wcslen(lpFileName);
	Param.nCount = 0;
	Param.pids = OutIds;
	if (EnumProcess(FindModuleCallback, &Param) == 0)
		return Param.nCount;
	else
		return 0;

	return 0;
}
