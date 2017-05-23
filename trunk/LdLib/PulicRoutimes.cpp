#include "stdafx.h"
#include <winternl.h>
#include "PublicRoutimes.h"
#include <string.h>

#pragma comment(lib, "Shell32.lib")

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

BOOL OpenURL(LPCTSTR lpCmd, LPCTSTR lpParam)
{
	ShellExecute(0, _T("open"), lpCmd, lpParam, NULL, SW_SHOWNORMAL);
	return 0;
}

CLdString SysErrorMsg(DWORD dwErrorCode)
{
	LPTSTR pMsg = NULL;

	UINT nLen = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_ARGUMENT_ARRAY |
		FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwErrorCode, 0, (LPTSTR)&pMsg, 0, NULL);
	CLdString s(pMsg);
	LocalFree(pMsg);
	return s;
}

#pragma region Î´¹«¿ªAPI

//SYSTEM_INFORMATION_CLASS SystemHandleInformation = (SYSTEM_INFORMATION_CLASS)16;

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

NTSTATUS WINAPI NtQueryObject_(
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_writes_bytes_opt_(ObjectInformationLength) PVOID ObjectInformation,
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
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass)
{
	static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
		(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");

	if (fpNtQueryInformationFile == NULL) {
		return (NTSTATUS)-1;
	}

	IO_STATUS_BLOCK IoStatusBlock;

	return fpNtQueryInformationFile(FileHandle, &IoStatusBlock, FileInformation, Length, FileInformationClass);

}

typedef NTSTATUS(WINAPI *NTSETINFORMATIONFILE)(
	HANDLE FileHandle,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass
);

NTSTATUS NtSetInformationFile(
	HANDLE FileHandle, 
	PVOID FileInformation, 
	ULONG Length, 
	FILE_INFORMATION_CLASS FileInformationClass)
{
	static NTSETINFORMATIONFILE fpNtSetInformationFile = 
		(NTSETINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtSetInformationFile");
	if (fpNtSetInformationFile == NULL)
		return (NTSTATUS)-1;

	IO_STATUS_BLOCK IoStatusBlock;

	return fpNtSetInformationFile(FileHandle, &IoStatusBlock, FileInformation, Length, FileInformationClass);
}

#pragma endregion
