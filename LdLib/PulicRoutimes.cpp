#include "stdafx.h"
#include <winternl.h>
#include "PublicRoutimes.h"
#include <string.h>
#include <Sddl.h>

#pragma comment(lib, "Shell32.lib")

namespace LeadowLib {
	
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

	typedef NTSTATUS(WINAPI * _RtlGetVersion) (PRTL_OSVERSIONINFOW);
	BOOL RtlGetVersion(PRTL_OSVERSIONINFOW pOsvi)
	{
		static _RtlGetVersion RtlGetVersion = nullptr;
		if (RtlGetVersion == nullptr)
		{
			HMODULE hMod = LoadLibrary(_T("ntdll.dll"));
			if (hMod)
			{
				RtlGetVersion = (_RtlGetVersion)GetProcAddress(hMod, "RtlGetVersion");
				if (RtlGetVersion == 0)
				{
					FreeLibrary(hMod);
					return FALSE;
				}
			}
		}
		pOsvi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
		return RtlGetVersion(pOsvi) == 0;
	}

	BOOL IsWindow64() //判断是否是64位机
	{
		BOOL isWoW64 = false;
		IsWow64Process(GetCurrentProcess(), &isWoW64);
		if (isWoW64) {
			SYSTEM_INFO SystemInfo;
			GetNativeSystemInfo(&SystemInfo);
			return (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) | (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64);
		}
		return false;
	}

	WIN_OS_TYPE GetOsType()
	{
		OSVERSIONINFOW osvi;
		RtlGetVersion(&osvi);
		switch (osvi.dwMajorVersion)
		{
		case 5:
			switch (osvi.dwMinorVersion)
			{
			case 0:
				return Windows_2000;
			case 1:
				return Windows_XP;
			case 2:
				return Windows_XP_Professional_x64; //Windows_Server_2003 or Windows_Home_Server
			}
			break;
		case 6:
			switch (osvi.dwMinorVersion)
			{
			case 0:
				return Windows_Vista; //or Windows_Server_2008
			case 1:
				return Windows_7; //or Windows_Server_2008_R2
			case 2:
				return Windows_8; //or Windows_Server_2012
			case 3:
				return Windows_8_1;
			}
			break;
		case 10:
			return Windows_10;
		}
		return Windows_2000;
	}

	BOOL IsVista64()
	{
		if (IsWindow64()) {
			OSVERSIONINFOW osvi;
			RtlGetVersion(&osvi);
			return ((osvi.dwMajorVersion = 6) && (osvi.dwMinorVersion = 0)) ||
				((osvi.dwMajorVersion = 5) && (osvi.dwMinorVersion = 02)); //XP 64
		}
		else
			return false;
	}

	DWORD GetCurrentUserSID(CLdString& sidStr)
	{
		DWORD dwUserBuf = 256;
		TCHAR chCurrentUser[256] = { 0 };
		PSID userSID = NULL;
		DWORD dwSID, dwDomainNameSize = 0;
		BYTE bySidBuffer[1024] = { 0 };
		TCHAR byDomainBuffer[MAX_PATH] = { 0 };
		SID_NAME_USE snu;
		userSID = (PSID)bySidBuffer;
		dwSID = sizeof(bySidBuffer);
		dwDomainNameSize = sizeof(byDomainBuffer);

		if (!GetUserName(chCurrentUser, &dwUserBuf))
			return GetLastError();
		if (!LookupAccountName(NULL, chCurrentUser, userSID, &dwSID, byDomainBuffer, &dwDomainNameSize, &snu))
		{
			return GetLastError();
		}

		TCHAR* chSID = NULL;
		if (ConvertSidToStringSid(userSID, &chSID))
		{
			sidStr = chSID;
			LocalFree((HLOCAL)chSID);
		}
		else
			return GetLastError();
		return 0;
	}

	DWORD ClearFileSecurity(TCHAR* pFileName)
	{
		SECURITY_DESCRIPTOR sd;
		EnableTokenPrivilege(SE_BACKUP_NAME);
		EnableTokenPrivilege(SE_RESTORE_NAME);
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, true, nullptr, false);
		SetFileSecurity(pFileName, DACL_SECURITY_INFORMATION, &sd);
		//SetEveryoneRWEDAccessToFileOrFolder(FileName);
		SetFileAttributes(pFileName, FILE_ATTRIBUTE_NORMAL);
		//return DeleteFile(pFileName);
		return 0;
	}

	DWORD RunProcess(TCHAR* cmd, TCHAR* param, bool admin, PPROCESS_INFORMATION out)
	{

		if (admin)
		{
			SHELLEXECUTEINFO info = { 0 };
			info.cbSize = sizeof(SHELLEXECUTEINFO);
			info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_DEFAULT;
			info.lpVerb = TEXT("runas");
			info.lpFile = cmd;
			info.lpParameters = param;

			if (!ShellExecuteEx(&info))
				return GetLastError();
			if(out)
			{
				out->hProcess = info.hProcess;
				out->dwProcessId = 0;
				out->dwThreadId = 0;
				out->hThread = nullptr;
			}
		}
		else
		{
			PROCESS_INFORMATION info = { 0 };
			STARTUPINFO si = { 0 };
			
			CLdString invoker;
			invoker.Format(_T("\"%s\" %s"), cmd, param);
			si.cb = sizeof(STARTUPINFO);
			if (!CreateProcess(nullptr, invoker, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &info))
				return GetLastError();
			if (out)
			{
				*out = info;
			}
		}
		return 0;
	}

	CLdString NewGuidString()
	{
		GUID guid;
		if(!SUCCEEDED(CoCreateGuid(&guid)))
			return _T("");
		LPOLESTR p;
		if(!SUCCEEDED(StringFromCLSID(guid, &p)))
			return _T("");
		CLdString result = (TCHAR*)p;
		CoTaskMemFree(p);
		return result;
	}

	BOOL GetVolumeInformationByHandle_(
		HANDLE hFile, 
		LPWSTR lpVolumeNameBuffer, 
		DWORD nVolumeNameSize, 
		LPDWORD lpVolumeSerialNumber, 
		LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags, 
		LPWSTR lpFileSystemNameBuffer, 
		DWORD nFileSystemNameSize)
	{
		typedef BOOL(WINAPI *GETVOLUMEINFORMATIONBYHANDLE)(__in      HANDLE hFile,
			__out_ecount_opt(nVolumeNameSize) LPWSTR lpVolumeNameBuffer,
			__in      DWORD nVolumeNameSize,
			__out_opt LPDWORD lpVolumeSerialNumber,
			__out_opt LPDWORD lpMaximumComponentLength,
			__out_opt LPDWORD lpFileSystemFlags,
			__out_ecount_opt(nFileSystemNameSize) LPWSTR lpFileSystemNameBuffer,
			__in      DWORD nFileSystemNameSize);

		static GETVOLUMEINFORMATIONBYHANDLE fn = (GETVOLUMEINFORMATIONBYHANDLE)GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetVolumeInformationByHandleW");
		if (fn)
			return fn(hFile, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
		else
			return FALSE;
	}
#pragma region 未公开API

	//SYSTEM_INFORMATION_CLASS SystemHandleInformation = (SYSTEM_INFORMATION_CLASS)16;

	NTSTATUS WINAPI ZwQuerySystemInformation(
		IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
		OUT PVOID SystemInformation,
		IN ULONG SystemInformationLength,
		OUT PULONG ReturnLength OPTIONAL)
	{
		typedef NTSTATUS(WINAPI *NTQUERYSYSTEMINFORMATION)(
			IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
			OUT PVOID SystemInformation,
			IN ULONG SystemInformationLength,
			OUT PULONG ReturnLength OPTIONAL);

		static NTQUERYSYSTEMINFORMATION fpZwQuerySystemInformation =
			(NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(_T("ntdll")), "ZwQuerySystemInformation");

		if (fpZwQuerySystemInformation == NULL) {
			return (NTSTATUS)-1;
		}

		return fpZwQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	}

	NTSTATUS WINAPI NtQueryObject_(
		_In_opt_ HANDLE Handle,
		_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
		_Out_writes_bytes_opt_(ObjectInformationLength) PVOID ObjectInformation,
		_In_ ULONG ObjectInformationLength,
		_Out_opt_ PULONG ReturnLength)
	{
		typedef NTSTATUS(WINAPI *NTQUERYOBJECT)(
			_In_opt_ HANDLE Handle,
			_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
			_Out_opt_ PVOID ObjectInformation,
			_In_ ULONG ObjectInformationLength,
			_Out_opt_ PULONG ReturnLength);

		static NTQUERYOBJECT fpNtQueryObject =
			(NTQUERYOBJECT)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryObject");

		if (fpNtQueryObject == NULL) {
			return (NTSTATUS)-1;
		}

		return fpNtQueryObject(Handle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength);

	}

	NTSTATUS WINAPI NtQueryInformationFile(
		IN HANDLE FileHandle,
		OUT PVOID FileInformation,
		IN ULONG Length,
		IN FILE_INFORMATION_CLASS FileInformationClass)
	{
		typedef NTSTATUS(WINAPI *NTQUERYINFORMATIONFILE)(
			IN HANDLE FileHandle,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			OUT PVOID FileInformation,
			IN ULONG Length,
			IN FILE_INFORMATION_CLASS FileInformationClass);

		static NTQUERYINFORMATIONFILE fpNtQueryInformationFile =
			(NTQUERYINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationFile");

		if (fpNtQueryInformationFile == NULL) {
			return (NTSTATUS)-1;
		}

		IO_STATUS_BLOCK IoStatusBlock;

		return fpNtQueryInformationFile(FileHandle, &IoStatusBlock, FileInformation, Length, FileInformationClass);

	}

	NTSTATUS NtSetInformationFile(
		HANDLE FileHandle,
		PVOID FileInformation,
		ULONG Length,
		FILE_INFORMATION_CLASS FileInformationClass)
	{
		typedef NTSTATUS(WINAPI *NTSETINFORMATIONFILE)(
			HANDLE FileHandle,
			PIO_STATUS_BLOCK IoStatusBlock,
			PVOID FileInformation,
			ULONG Length,
			FILE_INFORMATION_CLASS FileInformationClass
			);

		static NTSETINFORMATIONFILE fpNtSetInformationFile =
			(NTSETINFORMATIONFILE)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtSetInformationFile");
		if (fpNtSetInformationFile == NULL)
			return (NTSTATUS)-1;

		IO_STATUS_BLOCK IoStatusBlock;

		return fpNtSetInformationFile(FileHandle, &IoStatusBlock, FileInformation, Length, FileInformationClass);
	}

	NTSTATUS NtQueryInformationProcess(
		_In_ HANDLE ProcessHandle,
		_Out_ PVOID ProcessInformation,
		_In_ ULONG ProcessInformationLength)
	{
		typedef NTSTATUS(WINAPI *NtQueryInformationProcess_)(
			_In_ HANDLE ProcessHandle,
			_In_ PROCESSINFOCLASS ProcessInformationClass,
			_Out_ PVOID ProcessInformation,
			_In_ ULONG ProcessInformationLength,
			_Out_opt_ PULONG ReturnLength
			);

		static NtQueryInformationProcess_ fpNtQueryInformationProcess =
			(NtQueryInformationProcess_)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationProcess");
		if (fpNtQueryInformationProcess == NULL)
			return (NTSTATUS)-1;

		DWORD out;

		return fpNtQueryInformationProcess(ProcessHandle, ProcessBasicInformation, ProcessInformation, ProcessInformationLength, &out);
	}
#pragma endregion
}