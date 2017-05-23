#pragma once

#include <winternl.h>
#include <Shellapi.h>
#include "LdStructs.h"
#include "LdString.h"

BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName);
BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege, BOOL bEnable = TRUE);
BOOL OpenURL(LPCTSTR lpCmd, LPCTSTR lpParam = NULL);
CLdString SysErrorMsg(DWORD dwErrorCode);

#pragma region 未公开API

#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW				((NTSTATUS)0x80000005L)

typedef enum _SYSTEM_HANDLE_TYPE
{
	OB_TYPE_UNKNOWN = 0,    //0
	OB_TYPE_TYPE,           // 1,fixed
	OB_TYPE_DIRECTORY,      // 2,fixed
	OB_TYPE_SYMBOLIC_LINK,  // 3,fixed
	OB_TYPE_TOKEN,          // 4,fixed
	OB_TYPE_PROCESS,        // 5,fixed
	OB_TYPE_THREAD,         // 6,fixed
	OB_TYPE_JOB,            // 7,fixed
	OB_TYPE_DEBUG_OBJECT,   // 8,fixed
	OB_TYPE_EVENT,          // 9,fixed
	OB_TYPE_EVENT_PAIR,     //10,fixed
	OB_TYPE_MUTANT,         //11,fixed
	OB_TYPE_CALLBACK,       //12,fixed
	OB_TYPE_SEMAPHORE,      //13,fixed
	OB_TYPE_TIMER,          //14,fixed
	OB_TYPE_PROFILE,        //15,fixed
	OB_TYPE_KEYED_EVENT,    //16,fixed
	OB_TYPE_WINDOWS_STATION,//17,fixed
	OB_TYPE_DESKTOP,        //18,fixed
	OB_TYPE_SECTION,        //19,fixed
	OB_TYPE_KEY,            //20,fixed
	OB_TYPE_PORT,           //21,fixed 
	OB_TYPE_WAITABLE_PORT,  //22,fixed
	OB_TYPE_ADAPTER,        //23,fixed
	OB_TYPE_CONTROLLER,     //24,fixed
	OB_TYPE_DEVICE,         //25,fixed
	OB_TYPE_DRIVER,         //26,fixed
	OB_TYPE_IOCOMPLETION,   //27,fixed
	OB_TYPE_FILE,           //28,fixed
	OB_TYPE_WMIGUID,        //29,fixed
	OB_TYPE_SESTION,        //30
	OB_TYPE_WORKER_FACTORY, //31
	OB_TYPE_USER_APC_RESERVE//32
}SYSTEM_HANDLE_TYPE;

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
/*
#define	FileNamesInformation,
#define	FileDispositionInformation,
#define	FilePositionInformation,
#define	FileFullEaInformation,
#define	FileModeInformation,
#define	FileAlignmentInformation,
#define	FileAllInformation,
#define	FileAllocationInformation,
#define	FileEndOfFileInformation,
#define	FileAlternateNameInformation,
#define	FileStreamInformation,
#define	FilePipeInformation,
#define	FilePipeLocalInformation,
#define	FilePipeRemoteInformation,
#define	FileMailslotQueryInformation,
#define	FileMailslotSetInformation,
#define	FileCompressionInformation,
#define	FileObjectIdInformation,
#define	FileCompletionInformation,
#define	FileMoveClusterInformation,
#define	FileQuotaInformation,
#define	FileReparsePointInformation,
#define	FileNetworkOpenInformation,
#define	FileAttributeTagInformation,
#define	FileTrackingInformation,
#define	FileIdBothDirectoryInformation,
#define	FileIdFullDirectoryInformation,
#define	FileValidDataLengthInformation,
#define	FileShortNameInformation,
#define	FileIoCompletionNotificationInformation,
#define	FileIoStatusBlockRangeInformation,
#define	FileIoPriorityHintInformation,
#define	FileSfioReserveInformation,
#define	FileSfioVolumeInformation,
#define	FileHardLinkInformation,
#define	FileProcessIdsUsingFileInformation,
#define	FileNormalizedNameInformation,
#define	FileNetworkPhysicalNameInformation,
#define	FileIdGlobalTxDirectoryInformation,
#define	FileIsRemoteDeviceInformation,
#define	FileUnusedInformation,
#define	FileNumaNodeInformation,
#define	FileStandardLinkInformation,
#define	FileRemoteProtocolInformation,
#define	FileRenameInformationBypassAccessCheck,
#define	FileLinkInformationBypassAccessCheck,
#define	FileVolumeNameInformation,
#define	FileIdInformation,
#define	FileIdExtdDirectoryInformation,
#define	FileReplaceCompletionInformation,
#define	FileHardLinkFullIdInformation,
#define	FileIdExtdBothDirectoryInformation,
#define	FileMaximumInformation
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

#pragma endregion
