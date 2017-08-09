#include "stdafx.h"
#include <winternl.h>
#include "PublicRoutimes.h"
#include <string.h>
#include <Sddl.h>

#pragma comment(lib, "Shell32.lib")

namespace LeadowLib {

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
		OSVERSIONINFO osvi;
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
			OSVERSIONINFO osvi;
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
#pragma region 未公开API

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
}