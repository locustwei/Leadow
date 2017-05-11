/*!
 * \file LdInvoker.cpp
 * \date 2017/04/26 14:22
 *
 * asa
 * 
 * Invoker 用于加载并调用动态库函数。
 * Explorer 插件不能直接执行动态库函数（如需管理员权限）
 * 或为了避免插件占用太多资源，启动进程执行功能。
 * 
 *
 * 
*/

#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include "LdStructs.h"
#include <string.h>
#include "FileUtils.h"


#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#include <shlwapi.h>

#include <vector>
#include <map>
using namespace std;

#include <string>
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> tstring;

#include <winternl.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#define STATUS_INFO_LENGTH_MISMATCH			((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW				((NTSTATUS)0x80000005L)
#define SystemHandleInformation				((SYSTEM_INFORMATION_CLASS)16)

// NTQUERYOBJECT
typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
	WCHAR NameBuffer[1];
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;


typedef NTSTATUS(WINAPI *NTQUERYOBJECT)(
	_In_opt_ HANDLE Handle,
	_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_opt_ PVOID ObjectInformation,
	_In_ ULONG ObjectInformationLength,
	_Out_opt_ PULONG ReturnLength);

// NTQUERYSYSTEMINFORMATION
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

typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

//
// NtQueryInformationFile
//
#define FileNameInformation					((FILE_INFORMATION_CLASS)9)

// typedef struct _FILE_NAME_INFORMATION {
// 	ULONG FileNameLength;
// 	WCHAR FileName[1];
// } FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef NTSTATUS(WINAPI *NTQUERYINFORMATIONFILE)(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass);

// typedef struct _CLIENT_ID {
// 	HANDLE UniqueProcess;
// 	HANDLE UniqueThread;
// } CLIENT_ID, *PCLIENT_ID;

// ncScopedHandle
class ncScopedHandle
{
	ncScopedHandle(const ncScopedHandle&);
	ncScopedHandle& operator=(const ncScopedHandle&);
public:
	ncScopedHandle(HANDLE handle)
		: _handle(handle)
	{
	}

	~ncScopedHandle()
	{
		if (_handle != NULL) {
			CloseHandle(_handle);
		}
	}

	operator HANDLE() const
	{
		return _handle;
	}

	PHANDLE  operator& ()
	{
		return &_handle;
	}

	void operator=(HANDLE handle)
	{
		if (_handle != NULL) {
			CloseHandle(_handle);
		}
		_handle = handle;
	}

private:
	HANDLE _handle;
};

// ncFileHandle
struct ncFileHandle
{
	SYSTEM_HANDLE	_handle;
	tstring			_filePath;
	tstring			_path;

	ncFileHandle(SYSTEM_HANDLE handle, const tstring& filePath, const tstring& path)
		: _handle(handle)
		, _filePath(filePath)
		, _path(path)
	{
	}
};

// GetDeviceDriveMap
void GetDeviceDriveMap(std::map<tstring, tstring>& mapDeviceDrive)
{
	TCHAR szDrives[512];
	if (!GetLogicalDriveStrings(_countof(szDrives) - 1, szDrives)) {
		return;
	}

	TCHAR* lpDrives = szDrives;
	TCHAR szDevice[MAX_PATH];
	TCHAR szDrive[3] = _T(" :");
	do {
		*szDrive = *lpDrives;

		if (QueryDosDevice(szDrive, szDevice, MAX_PATH)) {
			mapDeviceDrive[szDevice] = szDrive;
		}
		while (*lpDrives++);
	} while (*lpDrives);
}

// DevicePathToDrivePath
BOOL DevicePathToDrivePath(tstring& path)
{
	static std::map<tstring, tstring> mapDeviceDrive;

	if (mapDeviceDrive.empty()) {
		GetDeviceDriveMap(mapDeviceDrive);
	}

	for (std::map<tstring, tstring>::const_iterator it = mapDeviceDrive.begin(); it != mapDeviceDrive.end(); ++it) {
		size_t nLength = it->first.length();
		if (_tcsnicmp(it->first.c_str(), path.c_str(), nLength) == 0) {
			path.replace(0, nLength, it->second);
			return TRUE;
		}
	}

	return FALSE;
}

// GetHandlePath
BOOL GetHandlePath(HANDLE handle, tstring& path)
{
	static NTQUERYOBJECT fpNtQueryObject =
		(NTQUERYOBJECT)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryObject");

	if (fpNtQueryObject == NULL) {
		return FALSE;
	}

	DWORD dwLength = 0;
	OBJECT_NAME_INFORMATION info;
	NTSTATUS status = fpNtQueryObject(handle, (OBJECT_INFORMATION_CLASS)1, &info, sizeof(info), &dwLength);
	if (status != STATUS_BUFFER_OVERFLOW) {
		return FALSE;
	}

	POBJECT_NAME_INFORMATION pInfo = (POBJECT_NAME_INFORMATION)malloc(dwLength);
	while (true) {
		status = fpNtQueryObject(handle, (OBJECT_INFORMATION_CLASS)1, pInfo, dwLength, &dwLength);
		if (status != STATUS_BUFFER_OVERFLOW) {
			break;
		}
		pInfo = (POBJECT_NAME_INFORMATION)realloc(pInfo, dwLength);
	}

	BOOL bRes = FALSE;
	if (NT_SUCCESS(status)) {
		path = pInfo->Name.Buffer;
		bRes = DevicePathToDrivePath(path);
		wprintf((wchar_t*)path.c_str());
		printf("\n");
	}
	free(pInfo);
	return bRes;
}

// GetSystemHandleInfo
PSYSTEM_HANDLE_INFORMATION GetSystemHandleInfo()
{
	static NTQUERYSYSTEMINFORMATION fpNtQuerySystemInformation =
		(NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQuerySystemInformation");

	if (fpNtQuerySystemInformation == NULL) {
		return NULL;
	}

	DWORD dwLength = 0;
	SYSTEM_HANDLE_INFORMATION shi;
	NTSTATUS status = fpNtQuerySystemInformation(SystemHandleInformation, &shi, sizeof(shi), &dwLength);
	if (status != STATUS_INFO_LENGTH_MISMATCH) {
		return NULL;
	}

	PSYSTEM_HANDLE_INFORMATION pshi = (PSYSTEM_HANDLE_INFORMATION)malloc(dwLength);
	while (true) {
		status = fpNtQuerySystemInformation(SystemHandleInformation, pshi, dwLength, &dwLength);
		if (status != STATUS_INFO_LENGTH_MISMATCH) {
			break;
		}
		pshi = (PSYSTEM_HANDLE_INFORMATION)realloc(pshi, dwLength);
	}

	if (!NT_SUCCESS(status)) {
		free(pshi);
		pshi = NULL;
	}

	return pshi;
}

//
// 检测指定句柄是否可能导致NtQueryObject卡死：
//     1.注意必须使用NtQueryInformationFile而不是NtQueryObject进行检测，否则可能导致WinXP系统
//       下进程死锁而无法结束。
//
void CheckBlockThreadFunc(void* param)
{
	static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
		(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");

	if (fpNtQueryInformationFile != NULL) {
		BYTE buf[1024];
		IO_STATUS_BLOCK ioStatus;
		fpNtQueryInformationFile((HANDLE)param, &ioStatus, buf, 1024, FileNameInformation);
	}
}

// IsBlockingHandle
BOOL IsBlockingHandle(HANDLE handle)
{
	HANDLE hThread = (HANDLE)_beginthread(CheckBlockThreadFunc, 0, (void*)handle);

	if (WaitForSingleObject(hThread, 100) != WAIT_TIMEOUT) {
		return FALSE;
	}

	TerminateThread(hThread, 0);
	return TRUE;
}

// FindFileHandle
BOOL FindFileHandle(LPCTSTR lpName, vector<ncFileHandle>& handles)
{
	handles.clear();

	if (lpName == NULL) {
		return FALSE;
	}

	// 打开“NUL”文件以便后续获取文件句柄类型值。
	ncScopedHandle hTempFile = CreateFile(_T("NUL"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	if (hTempFile == NULL) {
		return FALSE;
	}

	// 获取当前系统中所有的句柄信息。
	PSYSTEM_HANDLE_INFORMATION pshi = GetSystemHandleInfo();
	if (pshi == NULL) {
		return FALSE;
	}

	// 查询当前系统的文件句柄类型值。
	BYTE nFileType = 0;
	DWORD dwCrtPid = GetCurrentProcessId();
	for (DWORD i = 0; i < pshi->dwCount; ++i) {
		if (pshi->Handles[i].dwProcessId == dwCrtPid && hTempFile == (HANDLE)pshi->Handles[i].wValue) {
			nFileType = pshi->Handles[i].bObjectType;
			break;
		}
	}

	HANDLE hCrtProc = GetCurrentProcess();
	for (DWORD i = 0; i < pshi->dwCount; ++i) {
		// 过滤掉非文件类型的句柄。
		if (pshi->Handles[i].bObjectType != nFileType) {
			continue;
		}

		// 将上述句柄复制到当前进程中。
		ncScopedHandle handle = NULL;
		ncScopedHandle hProc = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pshi->Handles[i].dwProcessId);
		if (hProc == NULL || !DuplicateHandle(hProc, (HANDLE)pshi->Handles[i].wValue, hCrtProc, &handle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
			continue;
		}

		// 过滤掉会导致NtQueryObject卡死的句柄（如管道等）。
		if (IsBlockingHandle(handle)) {
			continue;
		}

		// 获取句柄对应的文件路径并进行匹配检查。
		tstring filePath;
		if (GetHandlePath(handle, filePath) && filePath.find(lpName) != tstring::npos) {
			ncScopedHandle hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, pshi->Handles[i].dwProcessId);

			TCHAR szProcName[MAX_PATH];
			GetProcessImageFileName(hProcess, szProcName, MAX_PATH);
			tstring path(szProcName);
			DevicePathToDrivePath(path);
			ncFileHandle fh(pshi->Handles[i], filePath, path);
			handles.push_back(fh);
		}
	}

	free(pshi);
	return TRUE;
}

// BOOL CloseHandleEx (HANDLE handle, DWORD dwPid)
// {
// 	if (GetCurrentProcessId () == dwPid)
// 		return CloseHandle (handle);
// 
// 	ncScopedHandle hProcess = OpenProcess (PROCESS_DUP_HANDLE, FALSE, dwPid);
// 	if (hProcess == NULL)
// 		return FALSE;
// 
// 	return DuplicateHandle (hProcess, handle, GetCurrentProcess (), NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
// }

/*
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	tstring path(_T("E:\\LdExplorerExtends\\trunk\\bin\\test.exe"));
	vector<ncFileHandle> vecHandles;
	if (!FindFileHandle(path.c_str(), vecHandles)) {
		return -1;
	}

	return (int) 0;
}
*/
int _tmain(int argc, _TCHAR* argv[])
{
	tstring path(_T("E:\\TDDOWNLOAD\\"));
	vector<ncFileHandle> vecHandles;
	if (!FindFileHandle(path.c_str(), vecHandles)) {
		return -1;
	}

	return 0;
}
