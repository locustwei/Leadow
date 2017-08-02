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
#include <Ldlib.h>
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
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param) override
	{
		printf("%S\n", pFileInfo->Name);
		return true;
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	CVolumeInfo volume;
	volume.SetFileName(L"E:\\");
	ISearchLibrary * library = InitLib(L"LdFileSearch_d64.dll", nullptr);
	CMftReadImpl impl;
	library->EnumVolumeFiles(&volume, &impl, nullptr);
	printf("\npress any key exit");
	getchar();
	return 0;
}
