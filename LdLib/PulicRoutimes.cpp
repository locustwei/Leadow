#include "stdafx.h"
#include "PublicRoutimes.h"
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

BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege, BOOL bEnable)
{
	HANDLE hToken = 0;
	TOKEN_PRIVILEGES tkp = { 0 };

	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	if (LookupPrivilegeValue(NULL, pszPrivilege, &tkp.Privileges[0].Luid))
	{
		tkp.PrivilegeCount = 1;  // one privilege to set    
		
		tkp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return FALSE;
	}

	return FALSE;
}

#pragma region Î´¹«¿ªAPI

typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

NTSTATUS WINAPI ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL)
{
	static NTQUERYSYSTEMINFORMATION fpZwQuerySystemInformation =
		(NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(_T("ntdll")), "ZwQuerySystemInformation");

	if (fpZwQuerySystemInformation == NULL) {
		return (NTSTATUS)-1;
	}

	return fpZwQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
}

typedef NTSTATUS(WINAPI *NTQUERYOBJECT)(
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_opt_ PVOID ObjectInformation,
	_In_ ULONG ObjectInformationLength,
	_Out_opt_ PULONG ReturnLength);

NTSTATUS WINAPI NtQueryObject(
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_opt_ PVOID ObjectInformation,
	_In_ ULONG ObjectInformationLength,
	_Out_opt_ PULONG ReturnLength)
{
	static NTQUERYOBJECT fpNtQueryObject =
		(NTQUERYOBJECT)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryObject");

	if (fpNtQueryObject == NULL) {
		return (NTSTATUS)-1;
	}

	return fpNtQueryObject(Handle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength);

}

typedef NTSTATUS(WINAPI *NTQUERYINFORMATIONFILE)(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass);

NTSTATUS WINAPI NtQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass)
{
	static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
		(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");

	if (fpNtQueryInformationFile == NULL) {
		return (NTSTATUS)-1;
	}
	return fpNtQueryInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);

}
#pragma endregion
