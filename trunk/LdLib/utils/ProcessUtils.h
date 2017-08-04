#pragma once

#include <TlHelp32.h>

namespace LeadowLib {
	typedef BOOL(*EnumProcessCallback)(PPROCESSENTRY32 pEntry32, PVOID pParam);
	typedef BOOL(*EnumProcessModalCallback)(PMODULEENTRY32 pEntry32, PVOID pParam);

	class CProcessUtils
	{
	public:
		static DWORD KillProcess(DWORD dwProcessId);
		static DWORD EnumProcess(EnumProcessCallback callback, PVOID pParam);
		static DWORD EnumProcessModule(DWORD dwProcessId, EnumProcessModalCallback callback, PVOID pParam);
		static DWORD GetProcessFileName(DWORD dwProcessId, LPTSTR lpFileName);
		static DWORD FindOpenProcess(LPTSTR lpFileName, PDWORD OutIds);
		static DWORD FindOpenModule(LPTSTR lpFileName, PDWORD OutIds);
	private:
		struct FPCallback_param
		{
			UINT nLength;
			UINT nCount;
			LPTSTR lpFileName;
			PDWORD pids;
		};
		static BOOL FindProcessCallback(PPROCESSENTRY32 pEntry32, PVOID pParam);
		static BOOL FindModuleCallback(PPROCESSENTRY32 pEntry32, PVOID pParam);

	};

};