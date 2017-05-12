#pragma once

#include <winternl.h>
#include "LdStructs.h"

BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName);
BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege, BOOL bEnable = TRUE);
#pragma region 未公开API

SYSTEM_INFORMATION_CLASS SystemHandleInformation = (SYSTEM_INFORMATION_CLASS)16;

typedef struct _SYSTEM_HANDLE {
	DWORD dwProcessId;
	BYTE bObjectType;
	BYTE bFlags;
	WORD wValue;
	PVOID pAddress;
	DWORD GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	DWORD dwCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

NTSTATUS WINAPI ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

OBJECT_INFORMATION_CLASS ObjectNameInformation = (OBJECT_INFORMATION_CLASS)1;
OBJECT_INFORMATION_CLASS ObjectAllInformation = (OBJECT_INFORMATION_CLASS)3;
OBJECT_INFORMATION_CLASS ObjectDataInformation = (OBJECT_INFORMATION_CLASS)4;

typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
	WCHAR NameBuffer[1];
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

//优先使用 NtQueryInformationFile
//相似作用的函数GetFinalPathNameByHandle Vista以上支持
NTSTATUS WINAPI NtQueryObject(  
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_opt_ PVOID ObjectInformation,
	_In_ ULONG ObjectInformationLength,
	_Out_opt_ PULONG ReturnLength);

NTSTATUS WINAPI NtQueryInformationFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass);

#pragma endregion
