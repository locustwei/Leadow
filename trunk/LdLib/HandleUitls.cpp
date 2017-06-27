#include "stdafx.h"
#include <winternl.h>
#include <stdio.h>
#include "HandleUitls.h"
#include "PublicRoutimes.h"
#include "FileUtils.h"

const LPCTSTR HANDLE_TYPE_NAME[] = {
	_T("ALPC Port"),                                    
	_T("Desktop"),
	_T("Directory"),
	_T("Event"),
	_T("File"),
	_T("IoCompletion"),
	_T("IoCompletionReserve"),
	_T("Job"),
	_T("Key"),
	_T("KeyedEvent"),
	_T("Mutant"),
	_T("Process"),
	_T("Session"),
	_T("Section"),
	_T("Semaphore"),
	_T("SymbolicLink"),
	_T("Token"),
	_T("Timer"),
	_T("Thread"),
	_T("TpWorkerFactory"),
	_T("UserApcReserve")
	_T("WmiGuid"),
	_T("WindowStation"),
};

DWORD CHandleUitls::EnumSystemHandleInfo(IEnumSystemHandleCallback* callback, PVOID pParam)
{
	DWORD dwLength = 0;
	SYSTEM_HANDLE_INFORMATION shi;
	NTSTATUS status = ZwQuerySystemInformation(SystemHandleInformation, &shi, sizeof(shi), &dwLength);
	if (status != STATUS_INFO_LENGTH_MISMATCH) {
		return status;
	}

	PSYSTEM_HANDLE_INFORMATION pshi = (PSYSTEM_HANDLE_INFORMATION)malloc(dwLength);
	while (true) {
		status = ZwQuerySystemInformation(SystemHandleInformation, pshi, dwLength, &dwLength);
		if (status != STATUS_INFO_LENGTH_MISMATCH) {
			break;
		}
		pshi = (PSYSTEM_HANDLE_INFORMATION)realloc(pshi, dwLength);
	}

	if (!NT_SUCCESS(status)) {
		free(pshi);
		pshi = NULL;
	}
	else
	{
		for (DWORD i = 0; i < pshi->dwCount; i++)
		{
			if (callback)
			{
				if (!callback->SystemHandleCallback(&pshi->Handles[i], pParam))
					break;
			}
		}
	}
	free(pshi);
	return status;
}



DWORD CHandleUitls::FindOpenFileHandle(LPTSTR lpFullFileName, IEnumSystemHandleCallback* callback, PVOID pParam)
{
	class COpenHandleCallback : public IEnumSystemHandleCallback
	{
	public:
		COpenHandleCallback() :FileName((UINT)MAX_PATH) {};

		CLdString FileName;
		IEnumSystemHandleCallback* callback;

		virtual BOOL SystemHandleCallback(PSYSTEM_HANDLE pHandle, PVOID pParam) override
		{
			BOOL Result = TRUE;

			HANDLE tmphandle = NULL;
			if (DuplicateSysHandle(pHandle, tmphandle) != 0)
				return TRUE;

			SYSTEM_HANDLE_TYPE handletyep = OB_TYPE_UNKNOWN;
			GetSysHandleType(tmphandle, handletyep);
			if (handletyep == OB_TYPE_FILE)
			{
				TCHAR fileName[MAX_PATH] = { 0 };
				//if(GetFinalPathNameByHandle(tmphandle, fileName, MAX_PATH, 0))
				if (GetSysHandleName(tmphandle, fileName) == 0)
				{
					printf("%S\n", fileName);
					if (wcslen(fileName) == FileName.GetLength() && wcsnicmp(FileName, fileName, FileName.GetLength()) == 0)
					{
						Result = callback->SystemHandleCallback(pHandle, pParam);
					}
				}
			}
			CloseHandle(tmphandle);
			
			return Result;
		}
	};

	COpenHandleCallback tmp;

	tmp.callback = callback;
	//Param.hCurrentProcess = GetCurrentProcess();
	CFileUtils::Win32Path2DevicePath(lpFullFileName, tmp.FileName.GetData());
	DWORD Result = EnumSystemHandleInfo(&tmp, pParam);
	//CloseHandle(Param.hCurrentProcess);

	return Result;
}

DWORD CHandleUitls::GetSysHandleName(HANDLE hDupHandle, LPTSTR lpName)
{
	DWORD dwCb;
	NTSTATUS status = NtQueryObject_(hDupHandle, ObjectNameInformation, NULL, 0, &dwCb);
	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return status;
	POBJECT_NAME_INFORMATION pNameInfo = (POBJECT_NAME_INFORMATION)malloc(dwCb);
	status = NtQueryObject_(hDupHandle, ObjectNameInformation, pNameInfo, dwCb, &dwCb);
	if (!NT_SUCCESS(status))
		return status;
	if (lpName)
		CopyMemory(lpName, pNameInfo->Name.Buffer, pNameInfo->Name.Length);

	free(pNameInfo);
	return 0;
}

DWORD CHandleUitls::DuplicateSysHandle(PSYSTEM_HANDLE pHandle, HANDLE& hOutHandle)
{
	/* Query the object name (unless it has an access of
	0x0012019f, on which NtQueryObject could hang. */
	if (pHandle->GrantedAccess == 0x0012019f || pHandle->GrantedAccess == 0x00120189 || pHandle->GrantedAccess == 0x120089)
	{
		return ERROR_ACCESS_DENIED;
	}

	DWORD Result = 0;
	HANDLE tmphandle = NULL;
	HANDLE hProc = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pHandle->dwProcessId);
	if (hProc != NULL)
	{
		HANDLE hCurrentProcess = GetCurrentProcess();
		if (DuplicateHandle(hProc, (HANDLE)pHandle->wValue, hCurrentProcess, &tmphandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hOutHandle = tmphandle;
		}
		else
			Result = GetLastError();
		CloseHandle(hCurrentProcess);
		CloseHandle(hProc);
	}
	else
		Result = GetLastError();
	return Result;
}

DWORD CHandleUitls::GetSysHandleType(HANDLE hDupHandle, SYSTEM_HANDLE_TYPE& sysType)
{
	DWORD dwCb = 0;
	NTSTATUS status = NtQueryObject_(hDupHandle, ObjectTypeInformation, NULL, 0, &dwCb);
	if (status != STATUS_INFO_LENGTH_MISMATCH)
		return status;
	PPUBLIC_OBJECT_TYPE_INFORMATION pInfo = (PPUBLIC_OBJECT_TYPE_INFORMATION)malloc(dwCb);
	ZeroMemory(pInfo, dwCb);
	status = NtQueryObject_(hDupHandle, ObjectTypeInformation, pInfo, dwCb, &dwCb);
	//printf("%wZ \n", pInfo->TypeName);
	if (NT_SUCCESS(status))
	{
		if (pInfo->TypeName.Length == 0 || pInfo->TypeName.Buffer == NULL)
			return OB_TYPE_UNKNOWN;
		switch (pInfo->TypeName.Buffer[0])
		{
		case 'A': //("ALPC Port"),    
			if (pInfo->TypeName.Length == 9 * sizeof(TCHAR))
				sysType = OB_TYPE_PORT;
			else if (pInfo->TypeName.Length == 7 * sizeof(TCHAR))
				sysType = OB_TYPE_ADAPTER;
			break;	
		case 'C':
			if (pInfo->TypeName.Length == 7 * sizeof(TCHAR))
				sysType = OB_TYPE_CALLBACK;
			else if (pInfo->TypeName.Length == 8 * sizeof(TCHAR))
				sysType = OB_TYPE_CONTROLLER;
			break;
		case 'D':										
			switch (pInfo->TypeName.Length)
			{
			case 5 * sizeof(TCHAR) :
				sysType = OB_TYPE_DEBUG_OBJECT;
				break;
			case 6 * sizeof(TCHAR) :
				if (pInfo->TypeName.Buffer[1] == 'e')
					sysType = OB_TYPE_DEVICE;
				else if (pInfo->TypeName.Buffer[1] == 'r')
					sysType = OB_TYPE_DRIVER;
				break;
			case 7 * sizeof(TCHAR) :
				sysType = OB_TYPE_DESKTOP;
				break;
			case 9 * sizeof(TCHAR) :
				sysType = OB_TYPE_DIRECTORY;
				break;
			}
			break;										 
		case 'E':			
			if (pInfo->TypeName.Length == 5 * sizeof(TCHAR))
				sysType = OB_TYPE_EVENT;
			else
				sysType = OB_TYPE_EVENT_PAIR;
			break;										 
		case 'F':										
			sysType = OB_TYPE_FILE;
			break;										 
		case 'I':										
			switch (pInfo->TypeName.Length)
			{
			case 12 * sizeof(TCHAR) :
				sysType = OB_TYPE_IOCOMPLETION;
				break;
			case 19 * sizeof(TCHAR) :
				sysType = OB_TYPE_IOCOMPLETION;
				break;
			}
			break;										 
		case 'J':										 
			sysType = OB_TYPE_JOB;
			break;										 
		case 'K':										
			switch (pInfo->TypeName.Length)
			{
			case 3 * sizeof(TCHAR) :
				sysType = OB_TYPE_KEY;
				break;
			case 10 * sizeof(TCHAR) :
				sysType = OB_TYPE_KEYED_EVENT;
				break;
			}
			break;										
		case 'M':										
			sysType = OB_TYPE_MUTANT;
			break;										 
		case 'P':			
			if (pInfo->TypeName.Length == 7 * sizeof(TCHAR))
			{
				if(pInfo->TypeName.Buffer[4] == 'c')
					sysType = OB_TYPE_PROCESS;
				else if (pInfo->TypeName.Buffer[4] == 'f')
					sysType = OB_TYPE_PROFILE;
			}
			break;										 
		case 'S':
			switch (pInfo->TypeName.Length)
			{
			case 7 * sizeof(TCHAR) :
				if(pInfo->TypeName.Buffer[3] == 't')
					sysType = OB_TYPE_SECTION;
				else
					sysType = OB_TYPE_SESTION;
				break;
			case 9 * sizeof(TCHAR) :
				sysType = OB_TYPE_SEMAPHORE;
				break;
			case 12 * sizeof(TCHAR) :
				sysType = OB_TYPE_SYMBOLIC_LINK;
				break;
			}
			break;
		case 'T':
			switch (pInfo->TypeName.Length)
			{
			case 4 * sizeof(TCHAR) :
				sysType = OB_TYPE_TYPE;
				break;
			case 5 * sizeof(TCHAR) :
				if (pInfo->TypeName.Buffer[1] == 'o')
					sysType = OB_TYPE_TOKEN;
				else
					sysType = OB_TYPE_TIMER;
				break;
			case 6 * sizeof(TCHAR) :
				sysType = OB_TYPE_THREAD;
				break;
			case 15 * sizeof(TCHAR) :
				sysType = OB_TYPE_WORKER_FACTORY;
				break;
			}
			break;
		case 'U':
			sysType = OB_TYPE_USER_APC_RESERVE;
			break;
		case 'W':
			switch (pInfo->TypeName.Length)
			{
			case 7 * sizeof(TCHAR) :
				sysType = OB_TYPE_WMIGUID;
				break;
			case 9 * sizeof(TCHAR) :
				sysType = OB_TYPE_SEMAPHORE; //IoCompletionReserve
				break;
			case 12 * sizeof(TCHAR) :
				sysType = OB_TYPE_WAITABLE_PORT;
				break;
			case 13 * sizeof(TCHAR) :
				sysType = OB_TYPE_WINDOWS_STATION;
				break;
			}
			break;
		}

	}
	free(pInfo);
	return status;
}
