#pragma once

#include <winternl.h>
#include "classes/LdString.h"

namespace LeadowLib {

#define MAKEINT64(low, hi)      ((UINT64)low & 0xffffffff) | (((UINT64)hi & 0xffffffff) << 32)

	enum WIN_OS_TYPE
	{
		Windows_2000,
		Windows_XP,
		Windows_XP_Professional_x64,
		Windows_Server_2003,
		Windows_Home_Server,
		Windows_Vista,
		Windows_Server_2008,
		Windows_Server_2008_R2,
		Windows_7,
		Windows_Server_2012,
		Windows_8,
		Windows_8_1,
		Windows_10
	};

	CLdString SysErrorMsg(DWORD dwErrorCode);         //系统错误消息
	//打开（关闭）程序令牌
	BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege, BOOL bEnable = TRUE);
	//ShllExecute 打开文件、网页等
	BOOL OpenURL(LPCTSTR lpCmd, LPCTSTR lpParam = NULL);
	DWORD GetCurrentUserSID(CLdString& sidStr);
	DWORD ClearFileSecurity(TCHAR* pFileName);
#pragma region Window 版本
	WIN_OS_TYPE GetOsType();
	BOOL RtlGetVersion(PRTL_OSVERSIONINFOW pOsvi);
	BOOL IsWindow64();
	BOOL IsVista64();
#pragma endregion

#pragma region 未公开API

#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW				((NTSTATUS)0x80000005L)

#define SystemHandleInformation (SYSTEM_INFORMATION_CLASS)16

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

#define ObjectNameInformation (OBJECT_INFORMATION_CLASS)1
#define ObjectAllInformation (OBJECT_INFORMATION_CLASS)3
#define ObjectDataInformation (OBJECT_INFORMATION_CLASS)4

	typedef struct _OBJECT_NAME_INFORMATION {
		UNICODE_STRING Name;
		WCHAR NameBuffer[1];
	} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

	//优先使用 NtQueryInformationFile
	//相似作用的函数GetFinalPathNameByHandle Vista以上支持
	NTSTATUS WINAPI NtQueryObject_(
		_In_opt_ HANDLE Handle,
		_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
		_Out_opt_ PVOID ObjectInformation,
		_In_ ULONG ObjectInformationLength,
		_Out_opt_ PULONG ReturnLength);

	typedef struct _FILE_NAME_INFORMATION {
		ULONG FileNameLength;
		WCHAR FileName[1];
	} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

	typedef struct _FILE_STREAM_INFORMATION {
		ULONG NextEntryOffset;
		ULONG StreamNameLength;
		LARGE_INTEGER StreamSize;
		LARGE_INTEGER StreamAllocationSize;
		WCHAR StreamName[1];
	} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

#define FileFullDirectoryInformation (FILE_INFORMATION_CLASS)2
#define FileBothDirectoryInformation (FILE_INFORMATION_CLASS)3
#define FileBasicInformation (FILE_INFORMATION_CLASS)4
#define	FileStandardInformation (FILE_INFORMATION_CLASS)5
#define	FileInternalInformation (FILE_INFORMATION_CLASS)6
#define	FileEaInformation (FILE_INFORMATION_CLASS)7
#define	FileAccessInformation (FILE_INFORMATION_CLASS)8
#define	FileNameInformation (FILE_INFORMATION_CLASS)9
#define	FileRenameInformation (FILE_INFORMATION_CLASS)10
#define	FileLinkInformation (FILE_INFORMATION_CLASS)11
#define FileStreamInformation (FILE_INFORMATION_CLASS)22

	/*  全部的 FILE_INFORMATION_CLASS 枚举值
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileCopyOnWriteInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileTrackingInformation,
	FileOleDirectoryInformation,
	FileContentIndexInformation,
	FileInheritContentIndexInformation,
	FileOleInformation,
	FileMaximumInformation
	*/
	NTSTATUS WINAPI NtQueryInformationFile(
		IN HANDLE FileHandle,
		OUT PVOID FileInformation,
		IN ULONG Length,
		IN FILE_INFORMATION_CLASS FileInformationClass);

	NTSTATUS NtSetInformationFile(
		HANDLE FileHandle,
		PVOID FileInformation,
		ULONG Length,
		FILE_INFORMATION_CLASS FileInformationClass);

	NTSTATUS NtQueryInformationProcess(
		_In_ HANDLE ProcessHandle,
		_Out_ PVOID ProcessInformation,
		_In_ ULONG ProcessInformationLength
	);

#pragma endregion
};