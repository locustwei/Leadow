// test.cpp : 定义控制台应用程序的入口点。
//

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "NtfsMtfReader.h"
#include "RecordFile.h"
#include "ntfs.h"
#include "DiskVolume.h"
#include <Shlwapi.h>
#include <iostream>
#include "MD5.h"
#include <Wincrypt.h>
#include <SetupAPI.h>
#include <VersionHelpers.h>

using namespace  std;

TCHAR IndexPath[81] = {0};


BOOL WINAPI UsnEnumCallback(PUSN_RECORD record, PVOID Param)
{
	wprintf(L"%s \n", record->FileName);
	CRecordFile *f = (CRecordFile*)Param;
	f->WriteRecord(record->FileReferenceNumber & 0x0000FFFFFFFFFFFF, (PUCHAR)record->FileName, record->FileNameLength);
	return TRUE;
}

BOOL WINAPI MtfEnumCallback(UINT64 ReferenceNumber, ATTRIBUTE_TYPE attr, PVOID record, PVOID Param)
{
	CRecordFile *f = (CRecordFile*)Param;
	PFILE_INFO pFileName = (PFILE_INFO)record;
	switch(attr){
	case AttributeFileName:
		//wprintf(L"%4d   %s \n", ReferenceNumber, pFileName->Name);
		f->WriteRecord(ReferenceNumber, (PUCHAR)pFileName->Name, pFileName->NameLength * sizeof(TCHAR));
		break;
	}
	return TRUE;
}

bool is_vista_or_later()
{
	return IsWindowsVistaOrGreater();
	/*
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwVersion < 0x80000000)	// Windows NT/2000, Whistler
	{
		if(dwWindowsMajorVersion >= 6)	// Windows vista or later
		{
			return true;
		}
	}
	return false;
	*/
}

BOOL IsElevated( BOOL * pbElevated ) /*另一种判断是否高低权限的办法*/
{
	if( !is_vista_or_later() )
	{
		if ( pbElevated)
			*pbElevated = TRUE;
		return TRUE ;
	}
	HRESULT hResult = E_FAIL; // assume an error occured
	HANDLE hToken	= NULL;
	BOOL bRet = FALSE;
	if ( !::OpenProcessToken( 
		::GetCurrentProcess(), 
		TOKEN_QUERY, 
		&hToken ) )
	{
		return FALSE ;
	}

	TOKEN_ELEVATION te = { 0 };
	DWORD dwReturnLength = 0;

	if ( !::GetTokenInformation(
		hToken,
		TokenElevation,
		&te,
		sizeof( te ),
		&dwReturnLength ) )
	{
		DebugBreak();
	}
	else
	{
		if (dwReturnLength != sizeof( te ))
		{
			DebugBreak();
		}

		bRet = TRUE ; 

		if ( pbElevated)
			*pbElevated = (te.TokenIsElevated != 0);
	}

	::CloseHandle( hToken );

	return bRet;
}

#define SEE_MASK_FORCENOIDLIST 0x1000

void WipeShellExecuteEx(LPCTSTR lpFile, LPCTSTR pCommandLine, int nShow,  DWORD dwWaitInputIdle = 0, HANDLE* lphProcess = NULL, bool b_need_admin = false)
{
	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.lpFile = lpFile;
	si.lpParameters = pCommandLine;
	si.nShow = nShow;
	si.fMask = (SEE_MASK_FLAG_NO_UI | SEE_MASK_FORCENOIDLIST);
	si.lpVerb = L"open";
	if (is_vista_or_later() && b_need_admin)
	{
		si.lpVerb = L"runas";
	}

	if (dwWaitInputIdle || lphProcess)
		si.fMask |= SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&si);

	if (dwWaitInputIdle)
		WaitForInputIdle(si.hProcess, dwWaitInputIdle);

	if(lphProcess)
		*lphProcess = si.hProcess;
	else
		CloseHandle(si.hProcess);
}

bool get_app_path(std::wstring& wstr_app_path, HMODULE hm_handle = NULL)
{
	wchar_t wsz_temp[_MAX_PATH] = {0};
	if (0 == ::GetModuleFileNameW(hm_handle, wsz_temp, _MAX_PATH))
	{
		return false;
	}
	wstr_app_path = wsz_temp;
	std::wstring::size_type pos = 0;
	pos = wstr_app_path.find_last_of(L'\\');
	if (std::wstring::npos == pos)
	{
		return false;
	}
	wstr_app_path.erase(pos);
	wstr_app_path += L"\\";
	return true;
}

void DisplayVolumePaths(
	__in PWCHAR VolumeName
	)
{
	DWORD  CharCount = MAX_PATH + 1;
	PWCHAR Names     = NULL;
	PWCHAR NameIdx   = NULL;
	BOOL   Success   = FALSE;

	for (;;) 
	{
		//
		//  Allocate a buffer to hold the paths.
		Names = (PWCHAR) new BYTE [CharCount * sizeof(TCHAR)];

		if ( !Names ) 
		{
			//
			//  If memory can't be allocated, return.
			return;
		}

		//
		//  Obtain all of the paths
		//  for this volume.
		Success = GetVolumePathNamesForVolumeNameW(
			VolumeName, Names, CharCount, &CharCount
			);

		if ( Success ) 
		{
			break;
		}

		if ( GetLastError() != ERROR_MORE_DATA ) 
		{
			break;
		}

		//
		//  Try again with the
		//  new suggested size.
		delete [] Names;
		Names = NULL;
	}

	if ( Success )
	{
		//
		//  Display the various paths.
		for ( NameIdx = Names; 
			NameIdx[0] != L'\0'; 
			NameIdx += wcslen(NameIdx) + 1 ) 
		{
			wprintf(L"  %s", NameIdx);
		}
		wprintf(L"\n");
	}

	if ( Names != NULL ) 
	{
		delete [] Names;
		Names = NULL;
	}

	return;
}

typedef union DUMMYUNIONNAME {
	DRIVE_LAYOUT_INFORMATION_MBR Mbr;
	DRIVE_LAYOUT_INFORMATION_GPT Gpt;
}dumm ;

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	HDEVINFO devInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	SP_DEVICE_INTERFACE_DATA devInterface = { 0 };
	devInterface.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
	for (int i = 0; SetupDiEnumDeviceInterfaces(devInfoSet, NULL, &GUID_DEVINTERFACE_DISK, i, &devInterface); ++i) {
		SP_DEVINFO_DATA devInfoData = { 0 };
		devInfoData.cbSize = sizeof (SP_DEVINFO_DATA);
		DWORD len;
		SetupDiGetDeviceInterfaceDetail(devInfoSet, &devInterface, NULL, 0, &len, &devInfoData);
		std::CLdArray<char> buf(len);
		SP_DEVICE_INTERFACE_DETAIL_DATA *devInterfaceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA *) &buf[0];
		devInterfaceDetail->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (SetupDiGetDeviceInterfaceDetail(devInfoSet, &devInterface, devInterfaceDetail, len, NULL, &devInfoData)) {
			wprintf(devInterfaceDetail->DevicePath);
			//printf("\n");
			ZeroMemory((PBYTE)&buf[0], buf.size());
			DWORD PropertyRegDataType;
				if (SetupDiGetDeviceRegistryProperty(devInfoSet, &devInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &PropertyRegDataType, (PBYTE)&buf[0], buf.size(), &len)) {
					PWSTR p = (PWSTR)&buf[0];
					TCHAR dos[60] = {0};
					wcscat(dos, L"\\?\\");
					wcscat(dos, p);
					//p[wcslen(p)] = '\\';
					wprintf(L"%s\n", dos);
					QueryDosDevice(dos, dos, 60);
					wprintf(L"%d %S\n", GetLastError(), (PCWSTR)dos);
				}
			
		}
	}

	DWORD  CharCount            = 0;
	TCHAR  DeviceName[MAX_PATH] = L"";
	DWORD  Error                = ERROR_SUCCESS;
	HANDLE FindHandle           = INVALID_HANDLE_VALUE;
	BOOL   Found                = FALSE;
	size_t Index                = 0;
	BOOL   Success              = FALSE;
	TCHAR  VolumeName[MAX_PATH] = L"";

	//
	//  Enumerate all volumes in the system.
	FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

	if (FindHandle == INVALID_HANDLE_VALUE)
	{
		Error = GetLastError();
		wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
		return 0;
	}

	for (;;)
	{
		//
		//  Skip the \\?\ prefix and remove the trailing backslash.
		Index = wcslen(VolumeName) - 1;

		if (VolumeName[0]     != L'\\' ||
			VolumeName[1]     != L'\\' ||
			VolumeName[2]     != L'?'  ||
			VolumeName[3]     != L'\\' ||
			VolumeName[Index] != L'\\') 
		{
			Error = ERROR_BAD_PATHNAME;
			wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
			break;
		}

		//
		//  QueryDosDeviceW does not allow a trailing backslash,
		//  so temporarily remove it.
		VolumeName[Index] = L'\0';

		CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName)); 

		VolumeName[Index] = L'\\';

		if ( CharCount == 0 ) 
		{
			Error = GetLastError();
			wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
			break;
		}

		wprintf(L"\nFound a device:\n %s", DeviceName);
		wprintf(L"\nVolume name: %s", VolumeName);
		wprintf(L"\nPaths:");
		DisplayVolumePaths(VolumeName);

		CDiskVolume* volume = new CDiskVolume();

		volume->OpenVolume(VolumeName);
		//
		//  Move on to the next volume.
		Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

		if ( !Success ) 
		{
			Error = GetLastError();

			if (Error != ERROR_NO_MORE_FILES) 
			{
				wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
				break;
			}

			//
			//  Finished iterating
			//  through all the volumes.
			Error = ERROR_SUCCESS;
			break;
		}
	}

	FindVolumeClose(FindHandle);
	FindHandle = INVALID_HANDLE_VALUE;

	*/

	if (is_vista_or_later())
	{
		BOOL b_elevate = FALSE;
		IsElevated(&b_elevate);
		if (!b_elevate)
		{
			CLdString wstr_path;
			get_app_path(wstr_path);
			wstr_path += L"wjslib.exe";
			WipeShellExecuteEx(wstr_path, L"autorun", SW_SHOWNORMAL, 0, 0, true);

			return 0;
		}
	}

	setlocale(LC_ALL,"chs");


	//printf("输入索引文件存放位置 如：D:\\ \n");
	//_getws_s(IndexPath, 80);

	CLdArray<CDiskVolume*> volumes;
	DWORD tickt;
	//CWJSLib::EnumDiskVolumes();
	
	
	CDiskVolume* volume = new CDiskVolume();
	
	volume->OpenVolumePath(L"C:");
	if(!volume->UpdateMftDump(false)){
	}

	printf("create index.... \n");

	tickt = GetTickCount();
	volume->UpdateMftDump(FALSE);

	printf("used %d seconds  \n", (GetTickCount() - tickt) / 1000);
	//volume->StartThreadUpdateChange();
	
	tickt = GetTickCount();
	volumes.Add(volume);
	
	while(TRUE){
		printf("\n input option; press \"exit\" to quit \n");
		printf("1:search by name eg: abc, *abc, *abc* \n");
		printf("2:search by size \n");
		//printf("3、se \n");
		TCHAR buffer[81] = {0};
		_getws_s(buffer, 80);

		if(_wcsicmp(buffer, L"exit") == 0)
			break;
		UINT64 count = 0, found = 0, tmp;
		int m, n;

		switch(StrToInt(buffer)){
		case 1:
			printf("input file name \n");
			_getws_s(buffer, 80);
			tickt = GetTickCount();
			for(DWORD i=0; i<volumes.size(); i++){
				tmp = 0;
				FILE_FILTER filter = {0};
				filter.beginSize = -1;
				filter.endSize = -1;
				filter.pIncludeNames = new PCWSTR[2];
				filter.pIncludeNames[0] = buffer;
				filter.pIncludeNames[1] = NULL;

				volumes[i]->SearchFile(&filter, FALSE);
			}
			break;
		case 2:
			printf("more then  （-1）limit \n");
			_getws_s(buffer, 80);
			m = StrToInt(buffer);
			printf("less then （-1）limit \n");
			_getws_s(buffer, 80);
			n = StrToInt(buffer);

			tickt = GetTickCount();
			for(DWORD i=0; i<volumes.size(); i++){
				tmp = 0;
				FILE_FILTER filter = {0};
				filter.beginSize = m;
				filter.endSize = n;

				volumes[i]->SearchFile(&filter, FALSE);
			}
			break;
		case 3:
			
			
			
			break;
		default:
			continue;
		}
				
	}
	
	for(DWORD i=0; i<volumes.size(); i++){
		delete volumes[i];
	}

exit:
	printf("press any key exit.");
	getchar();
	return 0;
}
