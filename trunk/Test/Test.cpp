#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <Commdlg.h>
#include <shellapi.h>
#include <io.h>
#include <fcntl.h>
#include "../LdFileSearch/SearchLibrary.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

typedef PVOID(*Library_Init)(PAuthorization);

ISearchLibrary* InitLib(TCHAR * pLibFile, PAuthorization pAut)
{
	HMODULE hModule = LoadLibrary(pLibFile);
	if (hModule == NULL)
		return nullptr;
	Library_Init fnInit = (Library_Init)GetProcAddress(hModule, "API_Init");
	if (fnInit == NULL)
		return NULL;
	return (ISearchLibrary*)fnInit(pAut);
}

class CMftReadImpl: public IMftReadeHolder
{
public:
	CMftReadImpl()
	{
		nCount = 0;
	};

	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param) override
	{
		
		printf("%lld  %S\n", nCount++);
		return true;
	};
private:
	UINT64 nCount;
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	DWORD cb;
	UCHAR buffer[1024];

	CVolumeInfo volume;
	volume.SetFileName(L"i:\\");

/*
	HANDLE result = CreateFile(L"\\\\?\\Volume{6a205adf-0000-0000-0000-010000000000}", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if(result == INVALID_HANDLE_VALUE)
		return 0;
	DWORD CB;
	if (!DeviceIoControl(result, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &CB, nullptr))
		return 0;
	DeviceIoControl(result, FSCTL_UNLOCK_VOLUME, nullptr, 0, nullptr, 0, &CB, nullptr);
	return 0;

*/
	ISearchLibrary * library = InitLib(L"LdFileSearch_d64.dll", nullptr);
	CMftReadImpl impl;
	library->EnumVolumeFiles(&volume, &impl, (PVOID)0xFF1);
	printf("\npress any key exit");
	getchar();
	return 0;
}
